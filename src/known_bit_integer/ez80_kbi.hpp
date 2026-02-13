#ifndef EZ80_KBI_HPP
#define EZ80_KBI_HPP

#include "../ez80_type.h"

#include "known_bit_integer.hpp"

namespace kbi {

template<> constexpr
size_t bit_width_of_type<uint24_t>() { return 24; }
template<> constexpr
size_t bit_width_of_type<uint48_t>() { return 48; }

template<> constexpr
uint24_t get_all_ones<uint24_t>() { return UINT24_C(0xFFFFFF); }
template<> constexpr
uint48_t get_all_ones<uint48_t>() { return UINT48_C(0xFFFFFFFFFFFF); }

inline known_bit_integer<uint24_t> reverse_bits(known_bit_integer<uint24_t> x) {
	uint32_t temp_bits = static_cast<uint32_t>(x.bits);
	uint32_t temp_mask = static_cast<uint32_t>(x.mask);
	temp_bits = __builtin_bitreverse32(temp_bits) >> 8;
	temp_mask = __builtin_bitreverse32(temp_mask) >> 8;
	x.bits = static_cast<uint24_t>(temp_bits);
	x.mask = static_cast<uint24_t>(temp_mask);
	return x;
}

inline known_bit_integer<uint48_t> reverse_bits(known_bit_integer<uint48_t> x) {
	uint64_t temp_bits = static_cast<uint64_t>(x.bits);
	uint64_t temp_mask = static_cast<uint64_t>(x.mask);
	temp_bits = __builtin_bitreverse64(temp_bits) >> 16;
	temp_mask = __builtin_bitreverse64(temp_mask) >> 16;
	x.bits = static_cast<uint48_t>(temp_bits);
	x.mask = static_cast<uint48_t>(temp_mask);
	return x;
}

inline known_bit_integer<uint24_t> swap_byte_order(known_bit_integer<uint24_t> x) {
	uint32_t temp_bits = static_cast<uint32_t>(x.bits);
	uint32_t temp_mask = static_cast<uint32_t>(x.mask);
	temp_bits = __builtin_bswap32(temp_bits) >> 8;
	temp_mask = __builtin_bswap32(temp_mask) >> 8;
	x.bits = static_cast<uint24_t>(temp_bits);
	x.mask = static_cast<uint24_t>(temp_mask);
	return x;
}

inline known_bit_integer<uint48_t> swap_byte_order(known_bit_integer<uint48_t> x) {
	uint64_t temp_bits = static_cast<uint64_t>(x.bits);
	uint64_t temp_mask = static_cast<uint64_t>(x.mask);
	temp_bits = __builtin_bswap64(temp_bits) >> 16;
	temp_mask = __builtin_bswap64(temp_mask) >> 16;
	x.bits = static_cast<uint48_t>(temp_bits);
	x.mask = static_cast<uint48_t>(temp_mask);
	return x;
}

} // namespace kbi

typedef kbi::known_bit_integer<uint8_t> kbi8;
typedef kbi::known_bit_integer<uint16_t> kbi16;
typedef kbi::known_bit_integer<uint24_t> kbi24;
typedef kbi::known_bit_integer<uint32_t> kbi32;
typedef kbi::known_bit_integer<uint48_t> kbi48;
typedef kbi::known_bit_integer<uint64_t> kbi64;
using namespace kbi;

#endif /* EZ80_KBI_HPP */
