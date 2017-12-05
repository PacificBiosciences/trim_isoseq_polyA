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

#include <pacbio/trima/polyA_hmm_model.hpp>
//constexpr size_t PolyAHmmMode::nStates = 2;
//constexpr size_t PolyAHmmMode::nSymbol = 4;

PolyAHmmMode::PolyAHmmMode()
    : _base(nStates, nSymbol)
{ }

PolyAHmmMode::PolyAHmmMode(const PolyAHmmMode &other)
    : _base(other), forw_(other.forw_), back_(other.back_), post_(other.post_), path_(other.path_)
{ }

PolyAHmmMode::PolyAHmmMode(PolyAHmmMode &&other)
    : _base(std::move(other)), forw_(std::move(other.forw_)), back_(std::move(other.back_)), post_(std::move(other
                                                                                                                 .post_)), path_(
    std::move(other.path_))
{ }

PolyAHmmMode &PolyAHmmMode::operator=(PolyAHmmMode &&other)
{
    if (this != &other) {
        _base::operator=(std::move(other));
        forw_ = std::move(other.forw_);
        back_ = std::move(other.back_);
        post_ = std::move(other.post_);
        path_ = std::move(other.path_);
    }
    return *this;
}

//virtual bool PolyAHmmMode::read(const std::string& filename)
bool PolyAHmmMode::read(const std::string &filename)
{
    bool ret = _base::read(filename);
    // any chance of overwriting
    //        init_[States::POLYA] = 0.8;
    //        init_[States::NONPOLYA] = 1 - init_[States::POLYA];
    //        tran_(States::POLYA, States::POLYA)       = 0.7;
    //        tran_(States::POLYA, States::NONPOLYA)    = 1.0 - tran_(States::POLYA, States::POLYA);
    //        tran_(States::NONPOLYA, States::POLYA)    = 0.0;
    //        tran_(States::NONPOLYA, States::NONPOLYA) = 1.0 - tran_(States::NONPOLYA, States::POLYA);
    return ret;
}

//virtual bool PolyAHmmMode::write(const std::string& filename)
bool PolyAHmmMode::write(const std::string &filename)
{
    //		value_type t1 = tran_(States::POLYA, States::POLYA);
    //		value_type t2 = tran_(States::NONPOLYA, States::POLYA);
    // any chance of overwriting
    //		tran_(States::POLYA, States::POLYA)       = 0.7;
    //		tran_(States::POLYA, States::NONPOLYA)    = 1.0 - tran_(States::POLYA, States::POLYA);
    //		tran_(States::NONPOLYA, States::POLYA)    = 0.0;
    //		tran_(States::NONPOLYA, States::NONPOLYA) = 1.0 - tran_(States::NONPOLYA, States::POLYA);
    bool ret = _base::write(filename);
    // restore
    //		tran_(States::POLYA, States::POLYA)       = t1;
    //		tran_(States::POLYA, States::NONPOLYA)    = 1.0 - tran_(States::POLYA, States::POLYA);
    //		tran_(States::NONPOLYA, States::POLYA)    = t2;
    //		tran_(States::NONPOLYA, States::NONPOLYA) = 1.0 - tran_(States::NONPOLYA, States::POLYA);
    return ret;
}
