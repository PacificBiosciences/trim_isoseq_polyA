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
#ifndef fastq_h
#define fastq_h

#include <string>
#include <iostream>
#include <cassert>
#include <fstream>
#include <memory>
#include "format.hpp"
#include "sequence.hpp"
#include "type_policy.h"

template<class T = caseInsensitiveString>
struct Fastq: public Sequence<T>
{
    using seq_type = Sequence<T>;
    using iterator = FormatReaderIter<Fastq>;
    std::string name_;
    std::string quality_;
};

/* reading policy */
template<class T>
struct read_policy<Fastq<T> >
{
    using traits_type = typename T::traits_type;
    using string_type = T;
    using fastq_type  = Fastq<T>;
    // reading policy for fasta in the ifstream
    static fastq_type read(std::istream *ins)
    {
        fastq_type fq{};
        if (ins->peek() != '@' || ins->eof()) {
            return fq; // returning an empty Fa meant EOF or ill-formated file
        }
        ins->get(); // consume '@'
        std::getline(*ins, fq.name_); // read name, which is always std::string
        char c;
        while (true) {
            ins->get(c);
            if (c == '\n') break;
            fq.seq_ += c;
        }
        ins->ignore(std::numeric_limits<int>::max(), '\n');
        std::getline(*ins, fq.quality_);
        if (fq.seq_.size() != fq.quality_.size()) {
            fprintf(stderr, "[warning] the length of sequence and quality does not match for %s\n",
                    fq.name_.c_str());
        }
        return fq;
    }
};

template<class T>
struct FastqSupported
{
    const static bool value = false;
};

template<>
struct FastqSupported<std::string>
{
    const static bool value = true;
}; // currently string is supported

template<>
struct FastqSupported<caseInsensitiveString>
{
    const static bool value = true;
}; // currently caseInsensitiveString is supported

template<class T = caseInsensitiveString, class = typename std::enable_if<FastqSupported<T>::value>::type>
using FastqReader = FormatReader<Fastq<T> >;

#endif
