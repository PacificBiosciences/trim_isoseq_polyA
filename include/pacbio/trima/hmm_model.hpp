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
#ifndef hmm_model_hpp
#define hmm_model_hpp

#include "matrix.hpp"
#include <fstream>

class HmmModeBase
{
    // type
protected:
    using value_type    = double;
    using matrix_type   = Matrix<value_type>;
    using pointer       = value_type *;
    using const_pointer = const pointer;
    using reference     = value_type &;

    // methods
public:
    explicit HmmModeBase(int sta = 0, int sym = 0);

    virtual ~HmmModeBase()
    { }

    HmmModeBase(const HmmModeBase &other);

    HmmModeBase(HmmModeBase &&other);

    HmmModeBase &operator=(const HmmModeBase &) = delete;

    HmmModeBase &operator=(HmmModeBase &&other);

    size_t states() const;
    size_t symbols() const;

    reference initialProb(size_t i);

    void initialProb(size_t i, value_type v);

    reference transProb(size_t i, size_t j);

    void transProb(size_t i, size_t j, value_type v);

    reference emitProb(size_t i, size_t j);

    void emitProb(size_t i, size_t j, value_type v);

    virtual bool read(const std::string &filename);

    virtual bool write(const std::string &filename);

    // data
protected:
    size_t no_states_;
    size_t no_symbol_;
    matrix_type init_;
    matrix_type tran_;
    matrix_type emit_;
};

#endif
