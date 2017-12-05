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
#ifndef type_policy_h
#define type_policy_h

#include <string.h>
#include <vector>
#include <list>
#include <deque>
#include <string>

template<class T>
struct read_policy
{
};

template<class T>
struct write_policy
{
};

/* Sequence size calculation */
template<class T>
struct strsize
{
};

template<>
struct strsize<std::string>
{
    static size_t size(const std::string &s)
    { return s.size(); }
};

template<size_t N>
struct strsize<char[N]>
{
    static size_t size(const char *s)
    { return strlen(s) /* not necessarilly N-1 */; }
};

template<>
struct strsize<const char *>
{
    static size_t size(const char *s)
    { return strlen(s); }
};

template<template<class...> class Container, class T, class... Args>
struct linear_container_policy
{
};

// policies for vector
template<class T, class... Args>
struct linear_container_policy<std::vector, T, Args...>
{
    using container_t = std::vector<T, Args...>;
    using size_type = typename container_t::size_type;
    static void reserve(container_t &c, size_type n)
    {
        c.reserve(n);
    }

    static void add_to_right(container_t &c, T &&item)
    {
        c.emplace_back(std::forward<T>(item));
    }

    static void add_to_right(container_t &c, const T &item)
    {
        c.push_back(item);
    }

    template<class... I>
    static void add_to_right(container_t &c, I &&... args)
    {
        c.emplace_back(std::forward<I>(args)...);
    }

    static bool empty(const container_t &c)
    {
        return c.empty();
    }
};

// policies for list
template<class T, class... Args>
struct linear_container_policy<std::list, T, Args...>
{
    using container_t = std::list<T, Args...>;
    using size_type = typename container_t::size_type;
    static void reserve(container_t &c, size_type n)
    { /* list does not need to reserve */}

    static void add_to_right(container_t &c, T &&item)
    {
        c.emplace_back(std::forward<T>(item));
    }

    static void add_to_right(container_t &c, const T &item)
    {
        c.push_back(item);
    }

    template<class... I>
    static void add_to_right(container_t &c, I &&... args)
    {
        c.emplace_back(std::forward<I>(args)...);
    }

    static bool empty(const container_t &c)
    {
        return c.empty();
    }
};

// policies for deque
template<class T, class... Args>
struct linear_container_policy<std::deque, T, Args...>
{
    using container_t = std::deque<T, Args...>;
    using size_type = typename container_t::size_type;
    static void reserve(container_t &c, size_type n)
    { /* deque doesn't need to reserve */ }

    static void add_to_right(container_t &c, T &&item)
    {
        c.emplace_back(std::forward<T>(item));
    }

    static void add_to_right(container_t &c, const T &item)
    {
        c.push_back(item);
    }

    template<class... I>
    static void add_to_right(container_t &c, I &&... args)
    {
        c.emplace_back(std::forward<I>(args)...);
    }

    static bool empty(const container_t &c)
    {
        return c.empty();
    }
};

#endif /* type_policy_h */
