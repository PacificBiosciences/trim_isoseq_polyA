// Copyright (c) 2015, Pacific Biosciences of California, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//  * Neither the name of Pacific Biosciences nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY PACIFIC
// BIOSCIENCES AND ITS CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL PACIFIC BIOSCIENCES OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
// USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

// Author: Bo Han

#include <stdio.h>
#include <assert.h>
#include <thread>
#include <mutex>
#include <boost/program_options.hpp>
#include <pacbio/trima/fasta.hpp>
#include <pacbio/trima/thread.hpp>
#include <pacbio/trima/polyA_hmm_model.hpp>
#include <pacbio/trima/kernel_color.h>

/* send every 100 fasta entries to a thread each time */
const int default_bulk_size = 100;

/* buffer to hold std output in each thread, assuming each fasta (name + seq) is shorter than 50000 */
constexpr int stdout_buffer_size = default_bulk_size * 50000;

/* buffer to hold log in each thread, assuming each fasta (name + polyA length) is shorter than 1000 */
constexpr int stderr_buffer_size = default_bulk_size * 1000;

/* default # of threads */
const int default_num_threads = 8;

/* distance in the header section of flnc file from "C" in "_CCS" to the first digit after "fiveend=" */
const size_t k_header_distance1 = 57;

/* mutex for io */
std::mutex k_io_mx;


using fasta_t = Fasta<caseInsensitiveString>;

void setDefaultHMM(PolyAHmmMode &);

/* Iso-Seq specific stuff */
void adjustHeader(std::string &, size_t);

/* thread worker*/
template<class MTQ, bool showColor, bool isoSeqFormat>
class Worker
{
    using multi_thread_safe_queue_type = MTQ;
    using container_type = typename multi_thread_safe_queue_type::container_type;
public:
    Worker(const PolyAHmmMode &hmm, multi_thread_safe_queue_type &producer)
        : hmm_(hmm), producer_(producer)
    { }
    Worker(const Worker &other)
        : hmm_(other.hmm_), producer_(other.producer_)
    { }
    Worker &operator=(const Worker &) = delete;

    void operator()()
    {
        auto data = producer_.get();
        char *stdout_buf = (char*)malloc(stdout_buffer_size);
        char *stderr_buf = (char*)malloc(stderr_buffer_size);
        size_t stdout_buff_off{0}, stderr_buff_off{0};
        while (!data.empty()) {
            size_t polyalen;
            for (auto &fa : data) {
                const Matrix<int> &path = hmm_.calculateVirtabi(fa.seq_.rbegin(), fa.seq_.size());
                for (polyalen = 0; polyalen < path.size();
                     ++polyalen) { /* cannot use binary search because polyA might appear in the middle */
                    if (path[polyalen] == PolyAHmmMode::States::NONPOLYA) {
                        break;
                    }
                }
                if (isoSeqFormat) { // static decision
                    if (polyalen)
                        adjustHeader(fa.name_, polyalen);
                }
                stderr_buff_off += sprintf(stderr_buf + stderr_buff_off, "%s\t%zu\n", fa.name_.c_str(), polyalen);
                if (stderr_buff_off * 5 > stderr_buffer_size * 4) {
                    /* manually flush stderr */
                    std::lock_guard<std::mutex> lock(k_io_mx);
                    fwrite(stderr_buf, 1, stderr_buff_off, stderr);
                    stderr_buff_off = 0;
                }

                if (showColor) { // static decision; always print
                    stdout_buff_off += sprintf(stdout_buf + stdout_buff_off, ">%s\n%s" KERNAL_RED "%s \n" KERNAL_RESET,
                                               fa.name_.c_str(),
                                               fa.seq_.substr(0, fa.seq_.size() - polyalen).c_str(),
                                               fa.seq_.substr(fa.seq_.size() - polyalen).c_str()
                    );
                }
                if (!showColor) { // static decision
                    if (polyalen < fa.size()) { // print only when there are at least some non-polyA region
                        stdout_buff_off += sprintf(stdout_buf + stdout_buff_off, ">%s\n%s\n",
                                                   fa.name_.c_str(),
                                                   fa.seq_.substr(0, fa.seq_.size() - polyalen).c_str()
                        );
                    }
                }
                if (stdout_buff_off * 5 > stdout_buffer_size * 4) {
                    /* manually flush stdout */
                    std::lock_guard<std::mutex> lock(k_io_mx);
                    fwrite(stdout_buf, 1, stdout_buff_off, stdout);
                    stdout_buff_off = 0;
                }
            } /* end of for loop to process each fasta in data */
            {
                /* flush buffer */
                std::lock_guard<std::mutex> lock(k_io_mx);
                fwrite(stdout_buf, 1, stdout_buff_off, stdout);
                fwrite(stderr_buf, 1, stderr_buff_off, stderr);
            }
            stdout_buff_off = stderr_buff_off = 0;
            data = producer_.get(); /* get new chulk of data */
        }
        free(stdout_buf);
        free(stderr_buf);
    }
private:
    PolyAHmmMode hmm_;
    /* keep a COPY of the HMM model since it does mutable calculation inside the class */
    multi_thread_safe_queue_type &producer_;
};

int main(int argc, const char *argv[])
{
    boost::program_options::options_description
        opts(R"(this program trims the polyA tail specifically from the 3' ends of fasta files)");
    /** options **/
    std::string input_fa_file;
    std::string model_file;
    std::string train_polya_file;
    std::string train_nonpolya_file;
    std::string train_model_file;
    int num_thread;
    bool show_color;
    bool generic_format;
    try {
        opts.add_options()
            ("help,h", "display this help message and exit")
            ("input,i",
             boost::program_options::value<std::string>(&input_fa_file)->required(),
             "The input fasta file with polyA, can be compressed by gzip or bzip2")
            ("model,m",
             boost::program_options::value<std::string>(&model_file)->default_value(""),
             "HMM model file to use; if not specified, will use default values")
            ("polyA_training,a",
             boost::program_options::value<std::string>(&train_polya_file)->default_value(""),
             "Fasta file with polyA sequences for training with maximum-likelihood estimation")
            ("non_polyA_training,b",
             boost::program_options::value<std::string>(&train_nonpolya_file)->default_value(""),
             "Fasta file with non-polyA sequences for training with maximum-likelihood estimation")
            ("new_model,n",
             boost::program_options::value<std::string>(&train_model_file)->default_value(""),
             "New trained model file to output")
            ("color,c",
             boost::program_options::bool_switch(&show_color),
             "To color polyA sequences in the output instead of trimming away them")
            ("thread,t",
             boost::program_options::value<int>(&num_thread)->default_value(default_num_threads),
             "Number of threads to use")
            ("generic,G",
             boost::program_options::bool_switch(&generic_format),
             "Input is generic fasta format; By default, this script adjusts the coordinate in the header section of output fasta format for Iso-seq input. This option switches off this behavior.");
        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, opts), vm);
        boost::program_options::notify(vm);
        if (vm.count("help")) {
            std::cerr << opts << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << opts << std::endl;
        exit(EXIT_FAILURE);
    }
    PolyAHmmMode hmm;
    // initializing HMM model
    if (!train_polya_file.empty() && !train_nonpolya_file.empty()) {
        // if both training data are set, use that to train the data
        if (!model_file.empty()) {
            fprintf(stderr, "Error: cannot specify -m with -a/-b\n");
            exit(EXIT_FAILURE);
        }
        FastaReader<> plA_file{train_polya_file};
        FastaReader<> nplA_file{train_nonpolya_file};
        hmm.maximumLikelihoodEstimation(plA_file.begin(), plA_file.end(), nplA_file.begin(), nplA_file.end());
        if (!train_model_file.empty())
            hmm.write(train_model_file);
    }
    else if (!model_file.empty()) {
        // read saved HMM model from file
        if (!hmm.read(model_file))
            return EXIT_FAILURE;
    }
    else {
        if (!train_polya_file.empty() || !train_nonpolya_file.empty()) {
            fprintf(stderr, "Error: need to specify both -a and -b\n");
            exit(EXIT_FAILURE);
        }
        // using default HMM model
        setDefaultHMM(hmm);
    }

    // trim
    FastaReader<> reader(input_fa_file);
    auto iter = reader.begin();
    auto end = reader.end();
    MultiThreadSafeQueue<fasta_t, std::vector> producer(iter, end, default_bulk_size);
    std::vector<std::thread> threads;
    if (show_color) {
        if (generic_format) /* generic fasta */
            for (int i = 0; i < num_thread; ++i)
                threads.emplace_back(Worker<decltype(producer), true, false>(hmm, producer));
        else /* Iso-Seq FLNC specific fasta, need to adjust some coordinates in the header */
            for (int i = 0; i < num_thread; ++i)
                threads.emplace_back(Worker<decltype(producer), true, true>(hmm, producer));
    }
    else { // don't show color
        if (generic_format) /* generic fasta */
            for (int i = 0; i < num_thread; ++i)
                threads.emplace_back(Worker<decltype(producer), false, false>(hmm, producer));
        else /* Iso-Seq FLNC specific fasta, need to adjust some coordinates in the header */
            for (int i = 0; i < num_thread; ++i)
                threads.emplace_back(Worker<decltype(producer), false, true>(hmm, producer));
    }
    for (auto &t : threads)
        if (t.joinable())
            t.join();
    return EXIT_SUCCESS;
}

void setDefaultHMM(PolyAHmmMode &hmm)
{
    hmm.initialProb(PolyAHmmMode::States::POLYA) = 0.99283668;
    hmm.initialProb(PolyAHmmMode::States::NONPOLYA) = 0.00716332;

    hmm.transProb(PolyAHmmMode::States::POLYA, PolyAHmmMode::States::NONPOLYA) = 3.16493e-07;
    hmm.transProb(PolyAHmmMode::States::POLYA, PolyAHmmMode::States::POLYA) =
        1.0 - hmm.transProb(PolyAHmmMode::States::POLYA, PolyAHmmMode::States::NONPOLYA);
    hmm.transProb(PolyAHmmMode::States::NONPOLYA, PolyAHmmMode::States::POLYA) = 2.74842e-09;
    hmm.transProb(PolyAHmmMode::States::NONPOLYA, PolyAHmmMode::States::NONPOLYA) =
        1.0 - hmm.transProb(PolyAHmmMode::States::NONPOLYA, PolyAHmmMode::States::POLYA);

    hmm.emitProb(PolyAHmmMode::States::POLYA, to_idx['A']) = 0.928165;
    hmm.emitProb(PolyAHmmMode::States::POLYA, to_idx['C']) = 0.025917;
    hmm.emitProb(PolyAHmmMode::States::POLYA, to_idx['G']) = 0.024170;
    hmm.emitProb(PolyAHmmMode::States::POLYA, to_idx['T']) = 0.021748;
    hmm.emitProb(PolyAHmmMode::States::NONPOLYA, to_idx['A']) = 0.271806;
    hmm.emitProb(PolyAHmmMode::States::NONPOLYA, to_idx['C']) = 0.249539;
    hmm.emitProb(PolyAHmmMode::States::NONPOLYA, to_idx['G']) = 0.281787;
    hmm.emitProb(PolyAHmmMode::States::NONPOLYA, to_idx['T']) = 0.196867;
}

void adjustHeader(std::string &s, size_t polyalen)
{
    // fl.trimmed.fasta file only:
    // format: <movie_name>/<ZMW name>/<start>_<end>_<CCS> strand=[+|-];fiveseen=1;polyAseen=1;threeseen=1;fiveend=[\d+];polyAend=[\d+];threeend=[\d+];primer=1;chimra=NA

    // example: (- strand), length of CCS: 1564
    //          >m160403_065056_42175_c100993391270000001823222007191686_s1_p0/13/1533_53_CCS strand=-;fiveseen=1;polyAseen=1;threeseen=1;fiveend=31;polyAend=1511;threeend=1535;primer=1;chimera=NA
    //                                                                            | start; 1533 == 1564 - fiveseen(31)
    //                                                                                | end <- update this one; 53 = 1564 - polyAend (1511)

    // example: (+ strand), length of CCS: 1534 (not used)
    //          >m160403_065056_42175_c100993391270000001823222007191686_s1_p0/9/30_1487_CCS strand=+;fiveseen=1;polyAseen=1;threeseen=1;fiveend=30;polyAend=1487;threeend=1514;primer=1;chimera=NA
    //                                                                           | start; fiveend (30)
    //                                                                              | end <- update this one; 1487 = polyAend
    auto l = s.cbegin();
    while (*l++ != '/'); // l is now point to the char pass the 1st '\'       l
    while (*l++ != '/'); // l is now point to the char pass the 2nd '\'         l
    auto r = l; //                                                              r
    std::string newstr{s.cbegin(), l}; // >m16................................../
    newstr.reserve(s.size() + 3);
    while (*r++ != '_'); //                                                         r
    int start = std::stoi(std::string{l,
                                      r}); // this will likely throw exception if the format is not Iso-Seq specific, trailing _ is find for stoi
    l = r; //                                                                       l
    while (*r++ != '_'); //                                                              r
    assert(*r == 'C' && "invalid flnc file!");
    int end = std::stoi(std::string{l, r}); // this will likely throw exception if the format is not Iso-Seq specific
    if (start < end) { // + strand, update end
        end -= polyalen;
    }
    else { // - strand, update start
        end += polyalen;
    }
    newstr += std::to_string(start) + '_' + std::to_string(end) + '_';
    newstr.append(s.c_str() + (r - s.cbegin()),
                  k_header_distance1
                      - 1); // append "CCS strand=+;fiveseen=1;polyAseen=1;threeseen=1;fiveend=30;polyAend="
    r += k_header_distance1; // jump from "C" in "_CCS" to the first digit after in "fiveend="
    l = r - 1; // l at "=" of "fiveend=""
    while (*r++ != '='); // r is the first digit of polyAseen=
    newstr.append(l, r); // append the value of fiveend
    l = r; // l is not at the first digit of the original polyAseen
    while (*++r != ';'); // r is at ';'
    int polyAend = std::stoi(std::string{l, r});
    polyAend -= polyalen; // polyAend further away from threeend
    newstr.append(std::to_string(polyAend));
    newstr.append(r, s.cend());
    s.swap(newstr);
}

