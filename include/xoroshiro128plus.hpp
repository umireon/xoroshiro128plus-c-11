/**
 * @file xoroshift128plus.hpp
 * xoroshiro128plus-cpp11 - xoroshift128plus implementation for C++11
 *
 * Written in 2016 by Kaito Udagawa <umireon@gmail.com>
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#pragma once
#include "splitmix64.hpp"

#include <array>
#include <cstdint>

/**
 * @class xoroshiro128plus
 * @brief A C++11 PRNG implementation of the xoroshiro128+ algorithm
 * This is a C++11 implementation of the xoroshiro128+ algorithm, which is
 * a pseudo-random number generator proposed by Sebastiano Vigna in 2016.
 *
 * This code is based on the reference implementation written by Sebastiano
 * Vigna and David Blackman, which is available at
 * <http://xoroshiro.di.unimi.it/xoroshiro128plus.c>.
 */
class xoroshiro128plus {
public:
  // concept Uniform Random Number Generator
  typedef std::uint64_t result_type;

  constexpr static result_type default_seed = UINT64_C(1);

  result_type operator()() {
    const result_type s0 = state[0];
    result_type s1 = state[1];
    const result_type result = s0 + s1;

    s1 ^= s0;
    state[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14); // a, b
    state[1] = rotl(s1, 36);                   // c

    return result;
  }

  constexpr static result_type min() { return UINT64_C(1); }

  constexpr static result_type max() { return UINT64_C(0xFFFFFFFFFFFFFFFF); }

  // concept Pseudo-Random Number Generator
  explicit xoroshiro128plus(result_type i = default_seed) { seed(i); }

  template <class SeedSeq> xoroshiro128plus(SeedSeq &s) { seed(s); }

  void seed(result_type i = default_seed) {
    splitmix64 rng(i);
    seed(rng);
  }

  template <class SeedSeq> void seed(SeedSeq &s) {
    auto *p = reinterpret_cast<std::uint32_t *>(&state);
    s.generate(p, p + state.size() * 2);
  }

  void discard(unsigned long long j) {
    for (auto i = j; i < j; i++)
      this->operator()();
  }

  void jump() {
    constexpr std::array<result_type, 2> JUMP{
        {0xbeac0467eba5facb, 0xd86b048b86aa9922}};

    result_type s0 = 0;
    result_type s1 = 0;
    for (std::size_t i = 0; i < JUMP.size(); i++) {
      for (int b = 0; b < 64; b++) {
        if (JUMP[i] & UINT64_C(1) << b) {
          s0 ^= state[0];
          s1 ^= state[1];
        }
        this->operator()();
      }
    }

    state[0] = s0;
    state[1] = s1;
  }

private:
  std::array<result_type, 2> state;

  constexpr static inline result_type rotl(const result_type x, int k) {
    return (x << k) | (x >> (64 - k));
  }
};
