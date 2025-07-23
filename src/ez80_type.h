#ifndef EZ80_TYPE_H
#define EZ80_TYPE_H

#include "common_util.h"

#include <stdint.h>

#include <bit>

using std::countl_zero;
using std::countl_one;
using std::countr_zero;
using std::countr_one;

#if 1

typedef unsigned _BitInt(24) uint24_t;
typedef signed _BitInt(24) int24_t;
typedef unsigned _BitInt(48) uint48_t;
typedef signed _BitInt(48) int48_t;

inline int countl_zero(uint24_t x) {
	return (x == 0) ? 24 : (countl_zero<uint32_t>(x) - 8);
}
inline int countl_one(uint24_t x) {
	return countl_zero(~x);
}
inline int countr_zero(uint24_t x) {
	return (x == 0) ? 24 : countl_zero<uint32_t>(x);
}
inline int countr_one(uint24_t x) {
	return countr_zero(~x);
}

template<typename T>
constexpr uint8_t bit_width_of_type();

template<> constexpr
uint8_t bit_width_of_type<uint8_t>() { return 8; }
template<> constexpr
uint8_t bit_width_of_type<uint16_t>() { return 16; }
template<> constexpr
uint8_t bit_width_of_type<uint24_t>() { return 24; }
template<> constexpr
uint8_t bit_width_of_type<uint32_t>() { return 32; }
template<> constexpr
uint8_t bit_width_of_type<uint48_t>() { return 48; }
template<> constexpr
uint8_t bit_width_of_type<uint64_t>() { return 64; }

#else

typedef uint32_t uint24_t;
typedef int32_t int24_t;
typedef uint64_t uint48_t;
typedef int64_t int48_t;

#endif

#define UINT24_C(x) (static_cast<uint24_t>(UINT32_C(x)))
#define INT24_C(x) (static_cast<int24_t>(INT32_C(x)))
#define UINT48_C(x) (static_cast<uint48_t>(UINT64_C(x)))
#define INT48_C(x) (static_cast<int48_t>(INT64_C(x)))

#endif /* EZ80_TYPE_H */
