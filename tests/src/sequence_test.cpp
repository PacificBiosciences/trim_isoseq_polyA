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
#include <gmock/gmock.h>
#include <pacbio/trima/sequence.hpp>

using namespace std;
namespace {
class SequenceTest : public ::testing::Test {
protected:
    SequenceTest()
        : seq("GGATCGATCcatcga")
    {
    }

public:
    Sequence<> seq;
};

TEST_F(SequenceTest, Constructor1)
{
    Sequence<> seq2{ "GGATCGATCCATCGA" };
    fprintf(stderr, "%s\n", seq.seq_.c_str());
    fprintf(stderr, "%s\n", seq2.seq_.c_str());
    EXPECT_TRUE(seq == seq2);
}

TEST_F(SequenceTest, Reverse)
{
    Sequence<> seq2{ "agctacCTAGCTAGG" };
    seq.reverse();
    EXPECT_TRUE(seq == seq2);
}

TEST_F(SequenceTest, ReverseCopy)
{
    Sequence<> seq2{ "agctacCTAGCTAGG" };
    auto seq3 = seq.reverse_copy();
    EXPECT_TRUE(seq3 == seq2);
}

TEST_F(SequenceTest, Complement1)
{
    Sequence<> seq2{ "CCTAGCTAGGTAGCT" };
    seq.complement();
    EXPECT_TRUE(seq == seq2);
}

TEST_F(SequenceTest, Complement2)
{
    Sequence<> seq2{ "CCTAGCTAGGTAGC" };
    Sequence<> seq3{ "GGATCGATCcatcg" };
    seq2.complement();
    EXPECT_TRUE(seq3 == seq2);
}

TEST_F(SequenceTest, ComplementCopy)
{
    auto seq2 = seq.complement_copy();
    Sequence<> seq3{ "CCTAGCTAGGTAGCT" };
    EXPECT_TRUE(seq3 == seq2);
}

TEST_F(SequenceTest, ReverseComplement)
{
    Sequence<> seq2{ "TCGATGGATCGATCC" };
    seq.reverse_complement();
    EXPECT_TRUE(seq == seq2);
}

TEST_F(SequenceTest, ReverseComplementCopy)
{
    Sequence<> seq2{ "TCGATGGATCGATCC" };
    auto seq3 = seq.reverse_complement_copy();
    EXPECT_TRUE(seq3 == seq2);
}

TEST_F(SequenceTest, Size)
{
    EXPECT_TRUE(seq.size() == seq.seq_.size());
}
}

/*
int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
*/
