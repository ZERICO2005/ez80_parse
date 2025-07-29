#ifndef EZ80_TYPE_H
#define EZ80_TYPE_H

#include "common_util.h"

#include <stdint.h>

#include <bit>

using std::countl_zero;
using std::countl_one;
using std::countr_zero;
using std::countr_one;
using std::has_single_bit;
using std::bit_floor;
using std::popcount;

#if 1

typedef unsigned _BitInt(24) uint24_t;
typedef signed _BitInt(24) int24_t;
typedef unsigned _BitInt(48) uint48_t;
typedef signed _BitInt(48) int48_t;

#define UINT24_C(x) (static_cast<uint24_t>(UINT32_C(x)))
#define INT24_C(x) (static_cast<int24_t>(INT32_C(x)))
#define UINT48_C(x) (static_cast<uint48_t>(UINT64_C(x)))
#define INT48_C(x) (static_cast<int48_t>(INT64_C(x)))

inline int countl_zero(uint24_t x) {
	return (x == 0) ? 24 : (countl_zero<uint32_t>(x) - 8);
}
inline int countl_zero(uint48_t x) {
	return (x == 0) ? 48 : (countl_zero<uint64_t>(x) - 16);
}

inline int countl_one(uint24_t x) {
	return countl_zero(~x);
}
inline int countl_one(uint48_t x) {
	return countl_zero(~x);
}

inline int countr_zero(uint24_t x) {
	return (x == 0) ? 24 : countr_zero<uint32_t>(x);
}
inline int countr_zero(uint48_t x) {
	return (x == 0) ? 48 : countr_zero<uint64_t>(x);
}

inline int countr_one(uint24_t x) {
	return countr_one<uint32_t>(x);
}
inline int countr_one(uint48_t x) {
	return countr_one<uint64_t>(x);
}

inline int popcount(uint24_t x) {
	return popcount(static_cast<uint32_t>(x));
}
inline int popcount(uint48_t x) {
	return popcount(static_cast<uint64_t>(x));
}

inline bool has_single_bit(uint24_t x) {
	return (x && !(x & (x - 1)));
}
inline bool has_single_bit(uint64_t x) {
	return (x && !(x & (x - 1)));
}

inline uint24_t bit_floor(uint24_t x) {
	return static_cast<uint24_t>(bit_floor(static_cast<uint32_t>(x)));
}
inline uint48_t bit_floor(uint48_t x) {
	return static_cast<uint48_t>(bit_floor(static_cast<uint64_t>(x)));
}

#else

typedef uint32_t uint24_t;
typedef int32_t int24_t;
typedef uint64_t uint48_t;
typedef int64_t int48_t;

#endif

#endif /* EZ80_TYPE_H */
