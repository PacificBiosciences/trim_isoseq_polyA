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
#ifndef fasta_h
#define fasta_h

#include <string>
#include <iostream>
#include <cassert>
#include <fstream>
#include <memory>
#include "format.hpp"
#include "sequence.hpp"
#include "type_policy.h"

template<class T = caseInsensitiveString>
struct Fasta: public Sequence<T>
{
    using seq_type = Sequence<T>;
    using iterator = FormatReaderIter<Fasta>;
    std::string
        name_; // sequence can take different representation such as char*, std::string or twoBits; name has to be string
};

/* reading policy */
template<class T>
struct read_policy<Fasta<T> >
{
    using string_type = T;
    using fasta_type = Fasta<T>;

    // reading policy for fasta in the ifstream
    static fasta_type read(std::istream *ins)
    {
        fasta_type fa{};
        if (ins->peek() != '>' || ins->eof()) {
            return fa; // returning an empty Fa meant EOF or ill-formated file
        }
        ins->get(); // consume '>'
        std::getline(*ins, fa.name_); // read name, which is always std::string
        constexpr size_t bufsize = 20480;
        char buffer[bufsize];
        while (ins->peek() != '>' && ins->good()) {
            do {
                ins->clear();
                ins->getline(buffer, bufsize, '\n');
                fa.seq_.append(buffer);
            } while ((bufsize == ins->gcount() + 1) && (ins->rdstate() == std::ios::failbit));
        }
        return fa;
    }
};

template<class T>
struct FastaSupported
{
    const static bool value = false;
};

template<>
struct FastaSupported<std::string>
{
    const static bool value = true;
}; // currently string is supported

template<>
struct FastaSupported<caseInsensitiveString>
{
    const static bool value = true;
}; // currently caseInsensitiveString is supported

template<class T = caseInsensitiveString, class = typename std::enable_if<FastaSupported<T>::value>::type>
using FastaReader = FormatReader<Fasta<T> >;

#endif
