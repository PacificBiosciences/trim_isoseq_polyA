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
#include <string>
#include <iostream>
#include <pacbio/trima/fastq.hpp>
#include <gmock/gmock.h>
#include "TestData.h"

using namespace std;
namespace {
    class FastqTest : public ::testing::Test {
    protected:
        FastqTest()
        : reader(tests::polyA_Fastq)//"/Users/hanb/Dropbox/git/pacBio/HMM/HMM/tests/polyA.fq")
        {
        }

    public:
        FastqReader<> reader;
    };

    TEST_F(FastqTest, FastaReaderIterator)
    {
        auto fq = reader.begin();
        {
            EXPECT_EQ(fq->size(), 469);
            Sequence<> s{ "TGGCCGATCGTTCTTGAAGCTATATCTGGTAGATCCTCTGCCAGTCATAGTCGGTAGTCGTAGGTACTGGAACTTGCGTGCGTCTAGGACGCGTTGAAGCTATCTTATTGCCCGAAGGAGCCGAAAGTCTGGGACACGTCGTGGGACGTATCGTTAGGACTATGGGACTAGTCGTCTTCGGTCTTACAAATGTTGCAATGCCCTGTGAGCTACTTATGAAACATGAATGGTCGGTCTTGTGGTCGCTTTTGTCGAAATCACCGGACAAAAAGAAGAAATATCAATTGCAAAAATAACTATAGGTACTTCTTTGAAGAAACCAAACGTGAGTTTCTAACACGACTAAATCCATTTGATCTTTGAGAAAGTCGAAGACGAATGAAGTGCGTATAATTGATGAACGAGTCGAAGTCGAAGTTGTTCAACAAGAGAGTTAGGTAGTGCCGCTTGAAAAGTAATTAGTGCAAAT" };
            EXPECT_TRUE(static_cast<Sequence<> >(fq->seq_) == s);
        }
        ++fq;
        {
            EXPECT_EQ(fq->size(), 601);
            Sequence<> s{ "aatattgtttcatggtcgtggcgctggtcagactgctcgctaagtacgtccgaagaaacagctagaccatctattctctgaacttcctaagtggtcgggttggtcgtggcgctcatagttagcccagaacgtccagtttatgtggtggtgatcttggttgccagactcagtatggtcaaaatagttgtgctcgccatttcaactagtgcagttgctaaagtgccaactaaggttcgtcttgaagcaatcggtagtatctgcagtcaagtcgtagtcgtaggtatggcacttgtcgtgcgtctaggacgcgttgaagctatcttattgcccgaaggagccgaaagtctgggacacgtcgtgggacgtatcgttaggactatgggactagtcgtcttcggtcttacaaatgttgcaatgccctgtgagctacttatgaaacatgaatggtcggtcttgtggtcgcttttgtcgaaatcaccgaagtgcgtataattgatgaacgagtcgaagtcgaagttgttcaacaagagagttaggtagtgccaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaag" };
            EXPECT_TRUE(static_cast<Sequence<> >(fq->seq_) == s);
        }
        ++fq;
        EXPECT_TRUE(*fq == Fastq<>{});
    }

    TEST_F(FastqTest, FastqReaderForLoop)
    {
        size_t sizes[] = { 469, 601, 0 };
        int i = 0;
        for (auto fq : reader) {
            EXPECT_EQ(fq.size(), sizes[i++]);
        }
    }

    TEST_F(FastqTest, FastqSequence)
    {
        auto fqiter = reader.begin();
        {
            auto striter = fqiter->seq_.begin();
            EXPECT_EQ(*striter, 't');
            ++striter;
            EXPECT_EQ(*striter, 'g');
            ++striter;
            EXPECT_EQ(*striter, 'g');
        }
    }
}

/*
int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
*/
