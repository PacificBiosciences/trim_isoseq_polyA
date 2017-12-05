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
#ifndef sequence_h
#define sequence_h

#include "char_traits.hpp"
#include "type_policy.h"

using caseInsensitiveString = std::basic_string<char, CaseInsensitiveCharTrait<char>, std::allocator<char> >;

template<>
struct strsize<caseInsensitiveString>
{
    static size_t size(const caseInsensitiveString &s)
    { return s.size(); }
};

template<class T = caseInsensitiveString>
struct Sequence
{
    // type
    using seq_type = T;
    // methods
    Sequence()
        : seq_{""}
    {
    }
    Sequence(const seq_type &other)
        : seq_(other)
    {
    }
    //    template<class Iter> Sequence(Iter a, Iter b) : seq_(a, b) { }
    Sequence(const Sequence<T> &other)
        : seq_(other.seq_)
    {
    }
    Sequence(Sequence<T> &&other)
        : seq_(std::move(other.seq_))
    {
    }
    Sequence &operator=(const Sequence &other)
    {
        assert(this != &other);
        seq_ = other.seq_;
        return *this;
    }
    Sequence &operator=(Sequence &&other)
    {
        if (this != &other) {
            seq_ = std::move(other.seq_);
        }
        return *this;
    }

    Sequence &reverse();
    Sequence &complement();
    Sequence &reverse_complement();

    Sequence reverse_copy() const;
    Sequence complement_copy() const;
    Sequence reverse_complement_copy() const;
    size_t size() const
    { return seq_.size(); }

    typename seq_type::iterator begin()
    { return seq_.begin(); }
    typename seq_type::const_iterator cbegin() const
    { return seq_.cbegin(); }
    typename seq_type::iterator end()
    { return seq_.end(); }
    typename seq_type::const_iterator cend() const
    { return seq_.cend(); }

    // data
    seq_type seq_;
};

template<class T>
Sequence<T> &Sequence<T>::reverse()
{
    auto begin = seq_.begin();
    auto end = seq_.end();
    auto distance = std::distance(begin, end);
    std::advance(end, -1);
    while (distance > 0) {
        std::swap(*begin, *end);
        std::advance(begin, 1);
        std::advance(end, -1);
        distance -= 2;
    }
    return *this;
}

template<class T>
Sequence<T> &Sequence<T>::complement()
{
    for (auto &x : seq_) {
        switch (x) {
            case 'A':
            case 'a':
                x = 'T';
                break;
            case 'C':
            case 'c':
                x = 'G';
                break;
            case 'G':
            case 'g':
                x = 'C';
                break;
            case 'T':
            case 't':
            case 'U':
            case 'u':
                x = 'A';
                break;
            case 'N':
            case 'n':
                x = 'N';
                break;
            default:
                break;
        }
    }
    return *this;
}

template<class T>
Sequence<T> &Sequence<T>::reverse_complement()
{
    this->reverse().complement();
    return *this;
}

template<class T>
Sequence<T> Sequence<T>::reverse_copy() const
{
    return Sequence<T>{seq_type{seq_.crbegin(), seq_.crend()}};
}

template<class T>
Sequence<T> Sequence<T>::complement_copy() const
{
    Sequence<T> ret{*this};
    ret.complement();
    return ret;
}

template<class T>
Sequence<T> Sequence<T>::reverse_complement_copy() const
{
    Sequence<T> ret{seq_type{seq_.crbegin(), seq_.crend()}};
    ret.complement();
    return ret;
}

template<class T, class U>
bool operator==(const Sequence<T> &lhs, const Sequence<U> &rhs)
{
    return lhs.seq_ == rhs.seq_;
}

template<class T>
struct strsize<Sequence<T> >
{
    static size_t size(const Sequence<T> &s)
    { return s.size(); }
};

namespace std
{
template<class T>
auto begin(Sequence<T> &s) -> decltype(s.begin())
{
    return s.begin();
}

template<class T>
auto begin(const Sequence<T> &s) -> decltype(s.cbegin())
const
{
return s.
cbegin();
}
} // namespace std

#endif /* sequence_h */
