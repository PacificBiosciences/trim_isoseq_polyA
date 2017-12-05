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
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations" // ignore deprecated declarations

#ifndef format_h
#define format_h

#include <unistd.h>
#include <memory>
#include <iostream>
#include <fstream>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include "type_policy.h"

#ifdef TO_SUPPORT_COMPRESSED_INPUT
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#endif

template<class T>
class FormatReader;

/* define iterator */
template<class T>
class FormatReaderIter: public boost::iterator_facade<FormatReaderIter<T>, T, std::input_iterator_tag>
{
public:
    // default constructr, meant to be called by FormatReader.end();
    FormatReaderIter()
        : fp_(nullptr), data_{new T{}}
    {
    }

    explicit FormatReaderIter(boost::iostreams::filtering_istream *fp)
        : fp_(fp)
    {
        this->increment();
    }

//    std::shared_ptr<T> data() { return data_; } /* not a good design */

private:
    friend class boost::iterator_core_access;

    void increment()
    {
        data_.reset(new T{read_policy<T>::read(fp_)});
        // the policy should return default T{} to indicate EOF or ill-formated file
    }

    bool equal(const FormatReaderIter &other) const
    {
        return *data_ == *(other.data_); // only called by != ...end()
    }

    T &dereference() const
    {
        return *data_;
    }

    boost::iostreams::filtering_istream *fp_ = nullptr;
    std::shared_ptr<T> data_ = nullptr;
};
/* end of iterator definition */

template<class T>
class FormatReader
{
public:
    using iterator = FormatReaderIter<T>;
    using const_iterator = const iterator;

    explicit FormatReader(const std::string &file_name)
    {
        std::istream *p_ist_in{&std::cin};
        if (file_name != "stdin" && file_name != "-") {
            if (access(file_name.c_str(), R_OK) != 0) {
                fprintf(stderr, "error, cannot read file %s. Please double check.\n", file_name.c_str());
                exit(EXIT_FAILURE);
            }
            p_ist_in = new std::ifstream{file_name};
#ifdef TO_SUPPORT_COMPRESSED_INPUT
#define GZIP_MAGIC "\037\213"
#define BZIP2_MAGIC "BZ"
            char magic_number[3];
            p_ist_in->get(magic_number, 3);
            if (memcmp(magic_number, GZIP_MAGIC, 2) == 0) {
                ins_.push(boost::iostreams::gzip_decompressor());
            }
            else if (memcmp(magic_number, BZIP2_MAGIC, 2) == 0) {
                ins_.push(boost::iostreams::bzip2_decompressor());
            }
            p_ist_in->seekg(0, p_ist_in->beg);
#endif
        }
        else {
            std::ios::sync_with_stdio(false);
            std::cin.tie(nullptr);
            std::cerr.tie(nullptr);
        }
        ins_.push(*p_ist_in);
    }

    ~FormatReader()
    { }

    iterator begin()
    {
        return iterator{&ins_};
    }

    iterator end()
    {
        return iterator{};
    }

protected:
    boost::iostreams::filtering_istream ins_;
};

#endif /* format_h */

#pragma GCC diagnostic pop
