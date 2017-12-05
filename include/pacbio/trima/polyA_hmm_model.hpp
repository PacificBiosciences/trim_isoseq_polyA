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
#ifndef polyA_hmm_model_hpp
#define polyA_hmm_model_hpp

#include <string>
#include <iostream>
#include <cmath>
#include "sequence.hpp" // policy strsize<>::size()
#include "hmm_model.hpp"
#include "hmm_utilities.h"

class PolyAHmmMode: public HmmModeBase
{
    // types
private:
    using _base = HmmModeBase;
    using _self = PolyAHmmMode;

protected:
    using value_type    = typename HmmModeBase::value_type;
    using pointer       = typename HmmModeBase::pointer;
    using matrix_type   = typename HmmModeBase::matrix_type;
    using path_type     = Matrix<int>;
    using const_pointer = typename std::add_const<pointer>::type;

public:
    constexpr static size_t nStates = 2;
    constexpr static size_t nSymbol = 4;

    enum States: int
    {
        POLYA = 0,
        NONPOLYA = 1,
        UNKNOWN = 2
    };
    // methods
public:
    PolyAHmmMode();

    virtual ~PolyAHmmMode()
    { }

    PolyAHmmMode(const PolyAHmmMode &other);

    PolyAHmmMode(PolyAHmmMode &&other);

    PolyAHmmMode &operator=(const PolyAHmmMode &) = delete;

    PolyAHmmMode &operator=(PolyAHmmMode &&other);

    virtual bool read(const std::string &filename);

    virtual bool write(const std::string &filename);

/* evaluating algorithms */
public:
    template<class TSequence>
    const matrix_type &calculateForward(const TSequence &) const;

    template<class TIter>
    const matrix_type &calculateForward(TIter, size_t) const;

    template<class TSequence>
    const matrix_type &calculateBackward(const TSequence &) const;

    template<class TIter>
    const matrix_type &calculateBackward(TIter, size_t) const;

    template<class TSequence>
    const matrix_type &calculatePosterior(const TSequence &) const;

/* decoding algorithms */
public:
    template<class TSequence>
    const path_type &calculateVirtabi(const TSequence &) const;

    template<class TIter>
    const path_type &calculateVirtabi(TIter, size_t) const;

/* training algorithms */
public:
    // estimate init_, emit_ & tran_ by taking a bunch of sequences
    template<class TSeqIterator>
    void maximumLikelihoodEstimation
        (TSeqIterator b_polya, TSeqIterator e_polya, TSeqIterator b_nonpolya, TSeqIterator e_nonpolya);

protected:
    template<class TSeqIterator>
    std::pair<size_t, size_t>
        maximumLikelihoodEstimationAux_(TSeqIterator, TSeqIterator, std::underlying_type<States>::type);

// data
protected:
    mutable matrix_type forw_;
    mutable matrix_type back_;
    mutable matrix_type post_;
    mutable path_type path_;
};

// -----------------------------------------------
// Virtabi
// answer the question: what is the most possible
// state chain that generate a given sequence?
// -----------------------------------------------
template<class TIterator>
auto PolyAHmmMode::calculateVirtabi(TIterator striter, size_t N) const -> const path_type &
{
    matrix_type prob(no_states_, N);
    prob = 0.0;
    double curmax, tmp;
    for (size_t i = 0; i < no_states_; ++i) {
        prob(i, 0) = std::log2(init_(i, 0) * emit_(i, to_idx[size_t(*striter)]));
    }
    // dynamically fill d
    ++striter; // at seq[1]
    for (size_t j = 1; j < N; ++j, ++striter) {
        // j is the observe sequence index
        for (size_t i = 0; i < no_states_; ++i) {
            // current state index i
            curmax = -INFINITY;
            for (size_t k = 0; k < no_states_; ++k) { // previous state index k
                tmp = prob(k, j - 1) + std::log2(tran_(k, i));
                if (tmp > curmax) {
                    curmax = tmp;
                }
            }
            prob(i, j) = curmax + std::log2(emit_(i, to_idx[size_t(*striter)]));
        }
    }
    path_.reSize(1, N);
    path_ = States::UNKNOWN;
    // determine the max ending
    curmax = -INFINITY;
    for (size_t i = 0; i < no_states_; ++i) {
        if (prob(i, N - 1) > curmax) {
            curmax = prob(i, N - 1);
            path_[N - 1] = i;
        }
    }
    for (int j = int(N - 2); j >= 0; --j) {
        curmax = -INFINITY;
        for (size_t i = 0; i < no_states_; ++i) {
            // from pos j (state: i) to pos j + 1 (state: path_(0, j+1) )
            tmp = prob(i, j) + std::log2(tran_(i, path_[j + 1]));
            if (tmp > curmax) {
                curmax = tmp;
                path_[j] = i;
            }
        }
    }
    return path_;
}

template<class TSequence>
auto PolyAHmmMode::calculateVirtabi(const TSequence &seq) const -> const path_type &
{
    size_t N = strsize<TSequence>::size(seq);
    return PolyAHmmMode::calculateVirtabi(std::begin(seq), N);
}

// -----------------------------------------------
// forward
// answer the question: what is the probability of
// sequence X(1)X(2)..X(i), given X(i) is
// in state of k?
// -----------------------------------------------
template<class TIterator>
auto PolyAHmmMode::calculateForward(TIterator striter, size_t N) const -> const matrix_type &
{
    forw_.reSize(no_states_, N);
    forw_ = 0.0;
    // fill first sequence
    for (size_t i = 0; i < no_states_; ++i) {
        forw_(i, 0) = std::log2(init_(i, 0) * emit_(i, to_idx[size_t(*striter)]));
    }
    // dynamically fill d
    ++striter; // at seq[1]
    value_type logsum, temp;
    for (size_t j = 1; j < N; ++j, ++striter) { // j is the observe sequence index
        for (size_t i = 0; i < no_states_; ++i) { // current state index i
            logsum = -INFINITY;
            for (size_t k = 0; k < no_states_; ++k) { // previous state index
                temp = forw_(k, j - 1) + std::log2(tran_(k, i));
                if (temp > -INFINITY) {
                    logsum = temp + std::log2(1 + std::exp2(logsum - temp));
                }
            }
            forw_(i, j) = std::log2(emit_(i, to_idx[size_t(*striter)])) + logsum;
        }
    }
    return forw_;
}

template<class TSequence>
auto PolyAHmmMode::calculateForward(const TSequence &seq) const -> const matrix_type &
{
    size_t N = strsize<TSequence>::size(seq);
    return PolyAHmmMode::calculateForward(std::begin(seq), N);
}

// -----------------------------------------------
// backward
// answer the question: what is the probability of
// X(i+1)X(i+2)...X(l), given X(i) is in state k?
// -----------------------------------------------
template<class TIterator>
auto PolyAHmmMode::calculateBackward(TIterator strriter, size_t N) const -> const matrix_type &
{
    back_.reSize(no_states_, N);
    back_ = 0.0;
    // fill first sequence
    for (size_t i = 0; i < no_states_; ++i) {
        back_(i, N - 1) = 0.0 /* std::log2(1.) */;
    }
    value_type logsum, temp;
    for (int j = int(N - 2); j >= 0; --j, --strriter) {
        // state at j
        for (size_t i = 0; i < no_states_; ++i) {
            // i is on position j
            logsum = -INFINITY;
            for (size_t k = 0; k < no_states_; ++k) {
                // k is on position j + 1
                temp = back_(k, j + 1) + std::log2(tran_(i, k) * emit_(k, to_idx[size_t(*strriter)]));
                if (temp > -INFINITY) {
                    logsum = temp + std::log2(1 + std::exp2(logsum - temp));
                }
            }
            back_(i, j) = logsum;
        }
    }
    return back_;
}

template<class TSequence>
auto PolyAHmmMode::calculateBackward(const TSequence &seq) const -> const matrix_type &
{
    size_t N = strsize<TSequence>::size(seq);
    // no corespoding std::rbegin()
    // for container, should use rbegin()
    // but for char*, has to overwrite operator++... TODO
    auto iter = std::begin(seq);
    std::advance(iter, N - 1);
    return PolyAHmmMode::calculateBackward(iter, N);
}

// -----------------------------------------------
// posterior
// answer the question: what is the probability of X(i)
// in state of k, given the sequence X(1)X(2)..X(i)..X(l)?
// -----------------------------------------------
template<class TSequence>
auto PolyAHmmMode::calculatePosterior(const TSequence &seq) const -> const matrix_type &
{
    size_t N = strsize<TSequence>::size(seq);
    calculateForward(seq);
    calculateBackward(seq);
    post_.reSize(no_states_, N);
    value_type prob = forw_(States::POLYA, N - 1);
    for (size_t i = 1; i < no_states_; ++i) {
        if (forw_(i, N - 1) > -INFINITY) {
            prob = forw_(i, N - 1) + std::log2(1.0 + std::exp2(prob - forw_(i, N - 1)));
        }
    }
    // prob now is the probability of observed the whole sequence
    for (size_t i = 0; i < no_states_; ++i) {
        for (size_t j = 0; j < N; ++j) {
            post_(i, j) = std::exp2(forw_(i, j) + back_(i, j) - prob);
        }
    }
    return post_;
}

// -----------------------------------------------
// MLE
// obtain the initial, transition and emission
// probabilities for the HMM model, given a training
// data with answer
// -----------------------------------------------

template<class TSeqIterator>
void PolyAHmmMode::maximumLikelihoodEstimation(TSeqIterator b_polya,
                                               TSeqIterator e_polya,
                                               TSeqIterator b_nonpolya,
                                               TSeqIterator e_nonpolya)
{
    auto count_A = maximumLikelihoodEstimationAux_(b_polya, e_polya, States::POLYA);
    auto count_B = maximumLikelihoodEstimationAux_(b_nonpolya, e_nonpolya, States::NONPOLYA);

    // calculate initial probability based on the number of entries in polyA and nonpolyA training file
    initialProb(States::POLYA) = static_cast<double>(count_A.first) / (count_A.first + count_B.first);
    // TODO: test whether hardcoding this is better
    //	initialProb(States::POLYA) = 0.5;
    initialProb(States::NONPOLYA) = 1.0 - init_[States::POLYA];

    // transition probability, assuming geometric duration distribution
    tran_(States::POLYA, States::POLYA) = 1.0 - 1.0 / static_cast<value_type>(count_A.second);
    tran_(States::POLYA, States::NONPOLYA) = 1.0 - tran_(States::POLYA, States::POLYA);

    tran_(States::NONPOLYA, States::NONPOLYA) = 1.0 - 1.0 / static_cast<value_type>(count_B.second);
    tran_(States::NONPOLYA, States::POLYA) = 1.0 - tran_(States::NONPOLYA, States::NONPOLYA);
}

template<class TSeqIterator>
std::pair<size_t, size_t> PolyAHmmMode::maximumLikelihoodEstimationAux_(TSeqIterator b,
                                                                        TSeqIterator e,
                                                                        std::underlying_type<States>::type s)
{ // s is POLYA(0) or NONPOLYA(1), init_[s], emit_(s, ?)
    if (b == e) {
        fprintf(stderr, "[ERROR] Invalid iterator, possible empty file\n");
        exit(EXIT_FAILURE);
    }
    std::pair<size_t, size_t> ret{0, 0}; // ret.first: # of entries; ret.second: # of nucleotide
    Matrix<int> new_emit(1, nSymbol);
    new_emit = 0;
    for (; b != e; ++b) {
        // *b: a Fasta;
        // b->seq_: a sequence
        for (auto ntiter = b->seq_.cbegin(); ntiter != b->seq_.cend(); ++ntiter) {
            ++new_emit[to_idx[size_t(*ntiter)]];
            ++ret.second; // counting the # of nt
        }
        ++ret.first; // count the # of entries
    }
    // sum
    value_type sum = 0.0;
    for (size_t i = 0; i < nSymbol; ++i) {
        sum += static_cast<value_type>(new_emit[i]);
    }
    // copy
    for (size_t i = 0; i < nSymbol; ++i) {
        emit_(s, i) = static_cast<value_type>(new_emit[i]) / sum;
    }
    return ret;
}

#endif
