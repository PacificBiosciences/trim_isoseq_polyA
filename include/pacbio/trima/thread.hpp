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

#ifndef TRIMISOSEQPOLYA_THREAD_H
#define TRIMISOSEQPOLYA_THREAD_H

#include <mutex>
#include "type_policy.h"

/* multi-threading safe queue to produce bulk of data to process
 * data type should
 * 1. provide an forward_iterator to generate/obtain data from certain source
 * container type should
 * 1. has specialized linear_container_policy which provides "reserve" "add_to_right" "empty" policies
 * */
template<class T, template<class...> class Container = std::vector>
class MultiThreadSafeQueue
{
public:
    using iterator = typename T::iterator;
    using container_type = Container<T>;
    using policies = linear_container_policy<Container, T>;
public:
    MultiThreadSafeQueue(iterator &iter, iterator &end, int size)
        : iter_(iter), end_(end), size_(size)
    { }

    container_type get()
    {
        std::lock_guard<std::mutex> lock(mx_);
        container_type ret;
        policies::reserve(ret, size_);
        int i;
        for (i = 0; i < size_; ++i) {
            if (iter_ == end_) break;
            policies::add_to_right(ret, std::move(*iter_));
            ++iter_;
        }
        return ret;
    }

private:
    iterator &iter_;
    iterator &end_;
    int size_;
    std::mutex mx_;
};

#endif //TRIMISOSEQPOLYA_THREAD_H
