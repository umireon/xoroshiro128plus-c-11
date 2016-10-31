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
#include <cstdint>

/**
 * @class splitmix64
 * @brief A C++11 PRNG implementation of the splitmix64 algorithm
 * This is a C++11 implementation of the splitmix64 algorithm.
 *
 * This code is based on the reference implementation written by Sebastiano
 * Vigna, which is available at
 * <http://xoroshiro.di.unimi.it/splitmix64.c>.
 */
class splitmix64 {
public:
  // concept Uniform Random Number Generator
  typedef std::uint64_t result_type;

  constexpr static result_type default_seed = UINT64_C(0);

  result_type operator()() {
    result_type z = (state += UINT64_C(0x9E3779B97F4A7C15));
    z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
    return z ^ (z >> 31);
  }

  constexpr static result_type min() { return UINT64_C(0); }

  constexpr static result_type max() { return UINT64_C(0xFFFFFFFFFFFFFFFF); }

  // concept Pseudo-Random Number Generator
  explicit splitmix64(result_type i = default_seed) { seed(i); }

  template <class SeedSeq> splitmix64(SeedSeq &s) { seed(s); }

  void seed(result_type i = default_seed) { state = i; }

  template <class SeedSeq> void seed(SeedSeq &s) {
    auto *p = reinterpret_cast<std::uint32_t *>(&state);
    s.generate(p, p + 2);
  }

  void discard(unsigned long long j) {
    for (auto i = j; i < j; i++)
      this->operator()();
  }

  // concept Seed Sequence
  template <class InputIt> void generate(InputIt begin, InputIt end) {
    auto it = begin;
    while (it != end) {
      result_type u = this->operator()();
      *it++ = u & UINT32_C(0xFFFFFFFF);
      if (it == end)
        break;
      *it++ = (u >> 32);
    }
  }

private:
  result_type state;
};
