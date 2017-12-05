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
#include <pacbio/trima/hmm_model.hpp>
#include <gmock/gmock.h>
#include "TestData.h"

using namespace std;
namespace {
class HmmModeBaseTest : public ::testing::Test {
protected:
    HmmModeBaseTest()
        : hmm(2, 4)
    {
        hmm.initialProb(0) = .5;
        hmm.initialProb(1) = .5;

        hmm.transProb(0, 0) = .7;
        hmm.transProb(0, 1, .3);
        hmm.transProb(1, 0, .3);
        hmm.transProb(1, 1, .7);

        hmm.emitProb(0, 0, .96);
        hmm.emitProb(0, 1, .01);
        hmm.emitProb(0, 2, .01);
        hmm.emitProb(0, 3) = .01;

        hmm.emitProb(1, 0, .3);
        hmm.emitProb(1, 1, .2);
        hmm.emitProb(1, 2, .3);
        hmm.emitProb(1, 3) = .2;
    }

public:
    HmmModeBase hmm;
};

TEST_F(HmmModeBaseTest, Constructor)
{
    EXPECT_EQ(hmm.states(), 2);
    EXPECT_EQ(hmm.symbols(), 4);
    EXPECT_DOUBLE_EQ(hmm.initialProb(0), 0.5);
    EXPECT_DOUBLE_EQ(hmm.initialProb(1), 0.5);

    EXPECT_DOUBLE_EQ(hmm.transProb(0, 0), 0.7);
    EXPECT_DOUBLE_EQ(hmm.transProb(0, 1), 0.3);
    EXPECT_DOUBLE_EQ(hmm.transProb(1, 0), .3);
    EXPECT_DOUBLE_EQ(hmm.transProb(1, 1), .7);

    EXPECT_DOUBLE_EQ(hmm.emitProb(0, 0), .96);
    EXPECT_DOUBLE_EQ(hmm.emitProb(0, 1), .01);
    EXPECT_DOUBLE_EQ(hmm.emitProb(0, 2), .01);
    EXPECT_DOUBLE_EQ(hmm.emitProb(0, 3), .01);
    EXPECT_DOUBLE_EQ(hmm.emitProb(1, 0), .3);
    EXPECT_DOUBLE_EQ(hmm.emitProb(1, 1), .2);
    EXPECT_DOUBLE_EQ(hmm.emitProb(1, 2), .3);
    EXPECT_DOUBLE_EQ(hmm.emitProb(1, 3), .2);
}

TEST_F(HmmModeBaseTest, MoveConstructor)
{
    HmmModeBase hmm2 = std::move(hmm);
    EXPECT_EQ(hmm2.states(), 2);
    EXPECT_EQ(hmm2.symbols(), 4);
    EXPECT_DOUBLE_EQ(hmm2.initialProb(0), 0.5);
    EXPECT_DOUBLE_EQ(hmm2.initialProb(1), 0.5);

    EXPECT_DOUBLE_EQ(hmm2.transProb(0, 0), 0.7);
    EXPECT_DOUBLE_EQ(hmm2.transProb(0, 1), 0.3);
    EXPECT_DOUBLE_EQ(hmm2.transProb(1, 0), .3);
    EXPECT_DOUBLE_EQ(hmm2.transProb(1, 1), .7);

    EXPECT_DOUBLE_EQ(hmm2.emitProb(0, 0), .96);
    EXPECT_DOUBLE_EQ(hmm2.emitProb(0, 1), .01);
    EXPECT_DOUBLE_EQ(hmm2.emitProb(0, 2), .01);
    EXPECT_DOUBLE_EQ(hmm2.emitProb(0, 3), .01);
    EXPECT_DOUBLE_EQ(hmm2.emitProb(1, 0), .3);
    EXPECT_DOUBLE_EQ(hmm2.emitProb(1, 1), .2);
    EXPECT_DOUBLE_EQ(hmm2.emitProb(1, 2), .3);
    EXPECT_DOUBLE_EQ(hmm2.emitProb(1, 3), .2);
}

TEST_F(HmmModeBaseTest, MoveAssignmentOperator)
{
    HmmModeBase hmm2;
    hmm2 = std::move(hmm);
    EXPECT_EQ(hmm2.states(), 2);
    EXPECT_EQ(hmm2.symbols(), 4);
    EXPECT_DOUBLE_EQ(hmm2.initialProb(0), 0.5);
    EXPECT_DOUBLE_EQ(hmm2.initialProb(1), 0.5);

    EXPECT_DOUBLE_EQ(hmm2.transProb(0, 0), 0.7);
    EXPECT_DOUBLE_EQ(hmm2.transProb(0, 1), 0.3);
    EXPECT_DOUBLE_EQ(hmm2.transProb(1, 0), .3);
    EXPECT_DOUBLE_EQ(hmm2.transProb(1, 1), .7);

    EXPECT_DOUBLE_EQ(hmm2.emitProb(0, 0), .96);
    EXPECT_DOUBLE_EQ(hmm2.emitProb(0, 1), .01);
    EXPECT_DOUBLE_EQ(hmm2.emitProb(0, 2), .01);
    EXPECT_DOUBLE_EQ(hmm2.emitProb(0, 3), .01);
    EXPECT_DOUBLE_EQ(hmm2.emitProb(1, 0), .3);
    EXPECT_DOUBLE_EQ(hmm2.emitProb(1, 1), .2);
    EXPECT_DOUBLE_EQ(hmm2.emitProb(1, 2), .3);
    EXPECT_DOUBLE_EQ(hmm2.emitProb(1, 3), .2);
}
TEST_F(HmmModeBaseTest, ReadFromFile)
{
    bool status = hmm.write(tests::hmm_Default_Out);//"/Users/hanb/Dropbox/git/pacBio/HMM/HMM/tests/HMM_default.txt");
    decltype(hmm) hmm2;
    status = hmm2.read(tests::hmm_Default_Out);//"/Users/hanb/Dropbox/git/pacBio/HMM/HMM/tests/HMM_default.txt");

    EXPECT_EQ(hmm2.states(), 2);
    EXPECT_EQ(hmm2.symbols(), 4);
    EXPECT_DOUBLE_EQ(hmm2.initialProb(0), 0.5);
    EXPECT_DOUBLE_EQ(hmm2.initialProb(1), 0.5);

    EXPECT_DOUBLE_EQ(hmm2.transProb(0, 0), 0.7);
    EXPECT_DOUBLE_EQ(hmm2.transProb(0, 1), 0.3);
    EXPECT_DOUBLE_EQ(hmm2.transProb(1, 0), .3);
    EXPECT_DOUBLE_EQ(hmm2.transProb(1, 1), .7);

    EXPECT_DOUBLE_EQ(hmm2.emitProb(0, 0), .96);
    EXPECT_DOUBLE_EQ(hmm2.emitProb(0, 1), .01);
    EXPECT_DOUBLE_EQ(hmm2.emitProb(0, 2), .01);
    EXPECT_DOUBLE_EQ(hmm2.emitProb(0, 3), .01);
    EXPECT_DOUBLE_EQ(hmm2.emitProb(1, 0), .3);
    EXPECT_DOUBLE_EQ(hmm2.emitProb(1, 1), .2);
    EXPECT_DOUBLE_EQ(hmm2.emitProb(1, 2), .3);
    EXPECT_DOUBLE_EQ(hmm2.emitProb(1, 3), .2);

    status = hmm2.read("/a");
    EXPECT_TRUE(!status);
    status = hmm2.write("/a");
    EXPECT_TRUE(!status);
}
}

/*
int main(int argc, char** argv)
{
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
*/
