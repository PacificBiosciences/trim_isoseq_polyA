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

#ifndef matrix_h
#define matrix_h

#include <algorithm> // for fill_n
#include <iostream> //for memcpy
#include <string.h> //for memcpy
#include <stdlib.h> // for malloc/free
#include <assert.h> // for assert
#include <cmath> // for std::log2
#include <utility> // for swap
#include <initializer_list>

template<class T>
class Matrix
{
    // type
public:
    using value_type    = T;
    using pointer       = T *;
    using const_pointer = const T *;
    using reference     = T &;

    explicit Matrix(size_t r = 0, size_t c = 0)
        : row_(r), col_(c), data_(nullptr)
    {
        if (r > 0 && c > 0)
            data_ = (pointer) malloc(sizeof(value_type) * row_ * col_);
    }

    virtual ~Matrix()
    { free(data_); }

    Matrix(const Matrix &other)
        : row_(other.row_), col_(other.col_)
    {
        if (data_ != nullptr) {
            free(data_);
            data_ = nullptr;
        }
        data_ = (pointer) malloc(sizeof(value_type) * row_ * col_);
        memcpy(data_, other.data_, sizeof(value_type) * row_ * col_);
    }

    Matrix(Matrix &&other)
        : row_(other.row_), col_(other.col_)
    {
        std::swap(data_, other.data_);
    }

    Matrix &operator=(const Matrix &) = delete;
    Matrix &operator=(Matrix &&other)
    {
        if (this != &other) {
            row_ = other.row_;
            col_ = other.col_;
            std::swap(data_, other.data_);
        }
        return *this;
    }

    Matrix &operator=(std::initializer_list<value_type> values)
    {
        assert(row_ * col_ >= values.size());
        int i = 0;
        for (auto val : values) {
            data_[i++] = val;
        }
        return *this;
    }

    size_t row() const
    { return row_; }
    size_t col() const
    { return col_; }
    size_t size() const
    { return row_ * col_; }

    reference operator()(size_t i, size_t j) const
    {
        assert(i < row_);
        assert(j < col_);
        return data_[i * col_ + j];
    }

    reference operator()(size_t i, size_t j)
    {
        assert(i < row_);
        assert(j < col_);
        return data_[i * col_ + j];
    }

    reference operator[](size_t j) // making 1D access easier
    {
        assert(j < col_);
        return data_[j];
    }

    reference operator[](size_t j) const
    {
        assert(j < col_);
        return data_[j];
    }

    void reSize(size_t r, size_t c)
    {
        row_ = r;
        col_ = c;
        if (data_ == nullptr)
            data_ = (pointer) malloc(row_ * col_ * sizeof(value_type));
        else {
            void *t = realloc(data_, row_ * col_ * sizeof(value_type));
            assert(t != nullptr);
            data_ = (pointer) t;
        }
    }

    template<class TFunc, class... TArgs>
    Matrix &apply(TFunc &&func, TArgs &&... args)
    {
        static_assert(
            std::is_same<value_type, decltype(std::forward<TFunc>(func)(
                value_type{},
                std::forward<TArgs...>(args)...))>::value,
            "the callable argument of apply should takes value_type as the first "
                "argument and have value_type as the return type");
        for (size_t i = 0; i < row_ * col_; ++i) {
            data_[i] = std::forward<TFunc>(func)(data_[i], std::forward<TArgs...>(args)...);
            // TODO: optimize with SIMD
        }
        return *this;
    }

    Matrix &operator=(value_type val)
    {
        // TODO: optimize this function with memset if std::is_same<std::is_integral<value_type>::type, std::true_type>::value
        std::fill_n(data_, row_ * col_, val);
        return *this;
        // return apply([&](value_type a, value_type b) -> value_type { return b; },
        // val);
    }

    Matrix &operator+=(value_type val)
    {
        return apply(
            [&](value_type a, value_type b) -> value_type {
                return a
                    + b;
            }, val);
    }

    Matrix &operator-=(value_type val)
    {
        return apply(
            [&](value_type a, value_type b) -> value_type {
                return a
                    - b;
            }, val);
    }

    Matrix &operator*=(value_type val)
    {
        return apply(
            [&](value_type a, value_type b) -> value_type {
                return a
                    * b;
            }, val);
    }

    Matrix &operator/=(value_type val)
    {
        assert(val != 0);
        return apply(
            [&](value_type a, value_type b) -> value_type {
                return a
                    / b;
            }, val);
    }

    Matrix &log2()
    {
        return apply([&](value_type a) -> value_type {
            assert(a > 0);
            return std::log2(a);
        });
    }

    value_type rowSum(size_t target_row) const
    {
        assert(target_row < row_);
        value_type sum = 0.0;
        pointer p = data_ + col_ * target_row;
        for (size_t c = 0; c < col_; ++c) {
            sum += *p++;
        }
        return sum;
    }

    value_type colSum(size_t target_col) const
    {
        assert(target_col < col_);
        value_type sum = 0.0;
        pointer p = data_ + target_col;
        for (size_t r = 0; r < row_; ++r) {
            sum += *p;
            p += col_;
        }
        return sum;
    }

    /* most generic */
    template<class U>
    bool equal(const Matrix<U> &rhs) const
    {
        if (row_ != rhs.row() || col_ != rhs.col())
            return false;
        for (size_t i = 0; i < row_; ++i) {
            for (size_t j = 0; j < col_; ++j) {
                if (this->operator()(i, j) != rhs(i, j))
                    return false;
            }
        }
        return true;
    }

    /* same type, potential optimization with memcmp */
    bool equal(const Matrix<value_type> &rhs) const
    {
        if (row_ != rhs.row_ || col_ != rhs.col_)
            return false;
        return equalAux_(rhs, typename std::is_integral<value_type>::type());
    }

protected:
    bool equalAux_(const Matrix<value_type> &rhs,
                   std::true_type /*is integral*/) const
    {
        if (memcmp(data_, rhs.data_, sizeof(value_type) * row_ * col_) == 0)
            return true;
        else
            return false;
    }

    bool equalAux_(const Matrix<value_type> &rhs,
                   std::false_type /*is not integral*/) const
    {
        for (size_t i = 0; i < row_; ++i) {
            for (size_t j = 0; j < col_; ++j) {
                if (this->operator()(i, j) != rhs(i, j))
                    return false;
            }
        }
        return true;
    }

protected:
    size_t row_;
    size_t col_;
    pointer data_ = nullptr;
};

template<class T, class U>
bool operator==(const Matrix<T> &lhs, const Matrix<U> &rhs)
{
    return lhs.equal(rhs);
}

template<class T, class U>
bool operator!=(const Matrix<T> &lhs, const Matrix<U> &rhs)
{
    return !lhs.equal(rhs);
}

#endif /* matrix_h */
