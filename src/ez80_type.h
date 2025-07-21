#ifndef EZ80_TYPE_H
#define EZ80_TYPE_H

#include "common_util.h"

#include <stdint.h>

#if 1

typedef unsigned _BitInt(24) uint24_t;
typedef signed _BitInt(24) int24_t;
typedef unsigned _BitInt(48) uint48_t;
typedef signed _BitInt(48) int48_t;

#else

typedef uint32_t uint24_t;
typedef int32_t int24_t;
typedef uint64_t uint48_t;
typedef int64_t int48_t;

#endif

#endif /* EZ80_TYPE_H */
