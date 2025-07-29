#ifndef KNOWN_BIT_INTEGER_HPP
#define KNOWN_BIT_INTEGER_HPP

#include <cstdint>
#include <algorithm>
#include <bit>
#include <climits>
#include <limits>

#include "known_bit_integer_fwd.hpp"

namespace kbi {

//------------------------------------------------------------------------------
// bitwise operations
//------------------------------------------------------------------------------

template<typename T>
known_bit_integer<T> complement(known_bit_integer<T> x) {
	known_bit_integer<T> ret;
	ret.bits = ~x.bits;
	ret.mask &= x.mask;
	return ret;
}

template<typename T>
known_bit_integer<T> bitwise_and(known_bit_integer<T> x, known_bit_integer<T> y) {
	known_bit_integer<T> ret;
	T known_zeros = (~x.bits & x.mask) | (~y.bits & y.mask);
	ret.bits = x.bits & y.bits;
	ret.mask = x.mask & y.mask;
	ret.mask |= known_zeros;
	ret.bits &= ret.mask;
	return ret;
}

template<typename T>
known_bit_integer<T> bitwise_or(known_bit_integer<T> x, known_bit_integer<T> y) {
	known_bit_integer<T> ret;
	T known_ones = (x.bits & x.mask) | (y.bits & y.mask);
	ret.bits = x.bits | y.bits;
	ret.mask = x.mask & y.mask;
	ret.mask |= known_ones;
	ret.bits &= ret.mask;
	return ret;
}

template<typename T>
known_bit_integer<T> bitwise_xor(known_bit_integer<T> x, known_bit_integer<T> y) {
	known_bit_integer<T> ret;
	ret.bits = x.bits ^ y.bits;
	ret.mask = x.mask & y.mask;
	ret.bits &= ret.mask;
	return ret;
}

//------------------------------------------------------------------------------
// shift
//------------------------------------------------------------------------------

template<typename T>
known_bit_integer<T> shift_left_logical(known_bit_integer<T> x, int shift) {
	known_bit_integer<T> ret;
	if (!ret.shift_logical_in_range(shift)) {
		return ret;
	}
	ret.bits = (x.bits << shift);
	ret.mask = ~(~x.mask << shift);
	return ret;
}

template<typename T>
known_bit_integer<T> shift_right_logical(known_bit_integer<T> x, int shift) {
	known_bit_integer<T> ret;
	if (!ret.shift_logical_in_range(shift)) {
		ret.set_unknown();
		return ret;
	}
	ret.bits = (x.bits >> shift);
	ret.mask = ~(~x.mask >> shift);
}

template<typename T>
known_bit_integer<T> shift_left_ones(known_bit_integer<T> x, int shift) {
	known_bit_integer<T> ret;
	if (!ret.shift_ones_in_range(shift)) {
		return ret;
	}
	ret.bits = ~(~x.bits << shift);
	ret.mask = ~(~x.mask << shift);
}

template<typename T>
known_bit_integer<T> shift_right_ones(known_bit_integer<T> x, int shift) {
	known_bit_integer<T> ret;
	if (!ret.shift_ones_in_range(shift)) {
		return ret;
	}
	ret.bits = ~(~x.bits >> shift);
	ret.mask = ~(~x.mask >> shift);
}

template<typename T>
known_bit_integer<T> shift_left_unknown_bits(known_bit_integer<T> x, int shift) {
	known_bit_integer<T> ret;
	if (!ret.shift_ones_in_range(shift)) {
		return ret;
	}
	ret.bits = (x.bits << shift);
	ret.mask = (x.mask << shift);
}

template<typename T>
known_bit_integer<T> shift_right_unknown_bits(known_bit_integer<T> x, int shift) {
	known_bit_integer<T> ret;
	if (!ret.shift_ones_in_range(shift)) {
		return ret;
	}
	ret.bits = (x.bits >> shift);
	ret.mask = (x.mask >> shift);
}

template<typename T>
known_bit_integer<T> shift_right_arithmetic(known_bit_integer<T> x, int shift) {
	bit_state sign = x.test_signbit();
	if (sign == known_true) {
		// negative
		return shift_right_ones(x, shift);
	}
	if (sign == known_false) {
		// positive
		return shift_right_logical(x, shift);
	}
	return shift_right_unknown_bits(x, shift);
}

//------------------------------------------------------------------------------
// rotate
//------------------------------------------------------------------------------

template<typename T>
void rotate_left(known_bit_integer<T> x, int shift) {
	if (!x.rotate_in_range(shift)) {
		return x;
	}
	for (int i = 0; i < shift; i++) {
		// MSB becomes LSB
		bit_state bit = x.test_signbit();
		x = shift_left_logical(x, 1);
		x.bit_copy(0, bit);
	}
	return x;
}

template<typename T>
void rotate_left_with_carry(known_bit_integer<T> x, int shift, bit_state& carry_inout) {
	if (!x.rotate_with_carry_in_range(shift)) {
		return x;
	}
	for (int i = 0; i < shift; i++) {
		// carry shifted into LSB
		// MSB shifted out to carry
		bit_state bit = x.test_signbit();
		x = shift_left_logical(x, 1);
		x.bit_copy(0, carry_inout);
		carry_inout = bit;
	}
	return x;
}

template<typename T>
void rotate_right(known_bit_integer<T> x, int shift) {
	if (!x.rotate_in_range(shift)) {
		return x;
	}
	for (int i = 0; i < shift; i++) {
		// LSB becomes MSB
		bit_state bit = x.bit_test(0);
		x = shift_right_logical(x, 1);
		x.bit_copy(x.get_signbit_index(), bit);
	}
	return x;
}

template<typename T>
void rotate_right_with_carry(known_bit_integer<T> x, int shift, bit_state& carry_inout) {
	if (!x.rotate_with_carry_in_range(shift)) {
		return x;
	}
	for (int i = 0; i < shift; i++) {
		// carry shifted into MSB
		// LSB shifted out to carry
		bit_state bit = x.bit_test(0);
		x = shift_right_logical(x, 1);
		x.bit_copy(x.get_signbit_index(), carry_inout);
		carry_inout = bit;
	}
	return x;
}

//------------------------------------------------------------------------------
// increment/decrement
//------------------------------------------------------------------------------

template<typename T>
known_bit_integer<T> increment_by_carry(known_bit_integer<T> x, bit_state carry) {
	for (size_t i = 0; i < bit_width_of_type<T>(); i++) {
		const bit_state X = x.bit_test(i);
		// SUM = X ^ CIN
		// COUT = X & CIN
		x.bit_copy(i, xor_bit_state(X, carry));
		carry = and_bit_state(carry, X);
	}
	return x;
}

template<typename T>
known_bit_integer<T> increment(known_bit_integer<T> x) {
	return increment_by_carry(x, known_true);
}

template<typename T>
known_bit_integer<T> decrement_by_carry(known_bit_integer<T> x, bit_state carry) {
	for (size_t i = 0; i < bit_width_of_type<T>(); i++) {
		const bit_state X = x.bit_test(i);
		// DIFF = X ^ CIN
		// COUT = ~X & CIN
		x.bit_copy(i, xor_bit_state(X, carry));
		carry = and_bit_state(invert_bit_state(X), carry);
	}
	return x;
}

template<typename T>
known_bit_integer<T> decrement(known_bit_integer<T> x) {
	return decrement_by_carry(x, known_true);
}

template<typename T>
known_bit_integer<T> negate(known_bit_integer<T> x) {
	// (-x) == (~x + 1) == ~(x - 1)
	return increment(complement(x));
}

template<typename T>
known_bit_integer<T> decrement_if_true_increment_if_false(known_bit_integer<T> x, bit_state cond) {
	known_bit_integer<T> x_inc = increment(x);
	known_bit_integer<T> x_dec = decrement(x);
	if (cond == known_false) {
		return x_inc;
	}
	if (cond == known_true) {
		return x_dec;
	}
	x_inc.merge_bits_intersection(x_dec);
	return x_inc;
}

template<typename T>
known_bit_integer<T> increment_if_true_decrement_if_false(known_bit_integer<T> x, bit_state cond) {
	return decrement_if_true_increment_if_false(x, invert_bit_state(cond));
}

template<typename T>
known_bit_integer<T> conditional_negate(known_bit_integer<T> x, bit_state cond) {
	if (cond == known_false) {
		return x;
	}
	known_bit_integer<T> neg = negate(x);
	if (cond == known_true) {
		return neg;
	}
	x.merge_bits_intersection(neg);
	return x;
}

template<typename T>
known_bit_integer<T> absolute_value(known_bit_integer<T> x) {
	return conditional_negate(x, x.test_signbit());
}

//------------------------------------------------------------------------------
// rounding
//------------------------------------------------------------------------------

template<typename T>
known_bit_integer<T> shift_right_ceil_unsigned(known_bit_integer<T> x, int shift) {
	if (shift >= bit_width_of_type<T>()) {
		x.set_to_zero_or_one(x.isknown_nonzero());
		return x;
	}
	bit_state sticky_bits = known_false;
	for (int i = 0; i < shift; i++) {
		sticky_bits = or_bit_state(sticky_bits, x.bit_test(0));
		x = shift_right_logical(x, 1);
	}
	x = increment_by_carry(x, sticky_bits);
	return x;
}

template<typename T>
known_bit_integer<T> shift_right_round_to_even_unsigned(known_bit_integer<T> x, int shift) {
	shift = std::min(shift, bit_width_of_type<T>() + 1);
	bit_state sticky_bits = known_false;
	bit_state round_bit = known_false;
	for (int i = 0; i < shift; i++) {
		sticky_bits = or_bit_state(sticky_bits, round_bit);
		round_bit = x.bit_test(0);
		x = shift_right_logical(x, 1);
	}
	bit_state guard_bit = x.bit_test(0);
	// round up to even if (round && (guard || sticky))
	bit_state round_upwards = and_bit_state(round_bit, or_bit_state(guard_bit, sticky_bits));
	x = increment_by_carry(x, round_upwards);
	return x;
}

//------------------------------------------------------------------------------
// addition/subtract
//------------------------------------------------------------------------------

template<typename T>
known_bit_integer<T> addition_with_carry_and_flags(
	known_bit_integer<T> x,
	known_bit_integer<T> y,
	bit_state& carry_inout,
	arithmetic_flags& flags
) {
	const bit_state sign_x = x.test_signbit();
	const bit_state sign_y = y.test_signbit();
	known_bit_integer<T> ret;
	for (size_t i = 0; i < bit_width_of_type<T>(); i++) {
		// 1 bit full adder
		const bit_state X = x.bit_test(i);
		const bit_state Y = y.bit_test(i);
		// SUM = X ^ Y ^ CIN
		// COUT = (X & Y) | (CIN & (X ^ Y))
		ret.bit_copy(i, xor_bit_state(X, Y, carry_inout));
		carry_inout = or_bit_state(
			and_bit_state(X, Y),
			and_bit_state(carry_inout, xor_bit_state(X, Y))
		);
	}
	flags.carry = carry_inout;
	flags.zero = ret.is_zero();
	const bit_state sign_r = ret.test_signbit();
	flags.sign = ret.test_signbit();
	flags.overflow = arithmetic_flags::test_addition_overflow(sign_r, sign_x, sign_y);
	if (x.isknown_zero() || y.isknown_zero()) {
		flags.overflow = known_false;
	}
	flags.parity_even = is_pairity_even(x);
	return ret;
}

template<typename T>
known_bit_integer<T> addition_with_carry(
	known_bit_integer<T> x,
	known_bit_integer<T> y,
	bit_state& carry_inout
) {
	arithmetic_flags flags;
	return addition_with_carry_and_flags(x, y, carry_inout, flags);
}

template<typename T>
known_bit_integer<T> addition(
	known_bit_integer<T> x,
	known_bit_integer<T> y
) {
	bit_state carry_inout = known_false;
	return addition_with_carry_and_flags(x, y, carry_inout);
}

template<typename T>
known_bit_integer<T> subtract_with_carry_and_flags(
	known_bit_integer<T> x,
	known_bit_integer<T> y,
	bit_state& carry_inout,
	arithmetic_flags& flags
) {
	const bit_state sign_x = x.test_signbit();
	const bit_state sign_y = y.test_signbit();
	known_bit_integer<T> ret;
	for (size_t i = 0; i < bit_width_of_type<T>(); i++) {
		// 1 bit full subtractor
		const bit_state X = x.bit_test(i);
		const bit_state Y = y.bit_test(i);
		// DIFF = X ^ Y ^ CIN
		// COUT = X < (Y + C)
		// COUT = (~X & Y) | (CIN & ~(X ^ Y))
		x.bit_copy(i, xor_bit_state(X, Y, carry_inout));
		carry_inout = or_bit_state(
			and_bit_state(invert_bit_state(X), Y),
			and_bit_state(carry_inout, invert_bit_state(xor_bit_state(X, Y)))
		);
	}
	flags.carry = carry_inout;
	flags.zero = ret.is_zero();
	const bit_state sign_r = ret.test_signbit();
	flags.sign = ret.test_signbit();
	flags.overflow = arithmetic_flags::test_subtraction_overflow(sign_r, sign_x, sign_y);
	if (x.isknown_zero() || y.isknown_zero()) {
		flags.overflow = known_false;
	}
	flags.parity_even = is_pairity_even(x);
	return ret;
}

template<typename T>
known_bit_integer<T> subtract_with_carry(
	known_bit_integer<T> x,
	known_bit_integer<T> y,
	bit_state& carry_inout
) {
	arithmetic_flags flags;
	return subtract_with_carry_and_flags(x, y, carry_inout, flags);
}

template<typename T>
known_bit_integer<T> subtract(
	known_bit_integer<T> x,
	known_bit_integer<T> y
) {
	bit_state carry_inout = known_false;
	return subtract_with_carry_and_flags(x, y, carry_inout);
}

//------------------------------------------------------------------------------
// comparisons
//------------------------------------------------------------------------------

template<typename T>
bit_state compare_equal(known_bit_integer<T> x, known_bit_integer<T> y) {
	if (x.isknown_fully() && y.isknown_fully()) {
		return ((x.bits == y.bits) ? known_true : known_false);
	}
	// are any bits known to be different
	if (((x.bits ^ y.bits) & (x.mask & y.mask)) != 0) {
		return known_false;
	}
	return unknown;
}

template<typename T>
bit_state compare_notequal(known_bit_integer<T> x, known_bit_integer<T> y) {
	return invert_bit_state(compare_equal(x, y));
}

template<typename T>
void compare(known_bit_integer<T> x, known_bit_integer<T> y, arithmetic_flags& flags) {
	bit_state carry_inout = known_false;
	subtract_with_carry_and_flags(x, y, carry_inout, flags);
}

template<typename T>
bit_state unsigned_less_than(known_bit_integer<T> x, known_bit_integer<T> y) {
	arithmetic_flags flags;
	unsigned_compare(x, y, flags);
	if (flags.carry == known_true) {
		return known_true;
	}
	if ((flags.carry == known_false) || (flags.zero == known_true)) {
		return known_false;
	}
	return unknown;
}

template<typename T>
bit_state unsigned_greater_equal(known_bit_integer<T> x, known_bit_integer<T> y) {
	return invert_bit_state(unsigned_less_than(x, y));
}

template<typename T>
bit_state unsigned_greater_than(known_bit_integer<T> x, known_bit_integer<T> y) {
	return unsigned_less_than(y, x);
}

template<typename T>
bit_state unsigned_less_equal(known_bit_integer<T> x, known_bit_integer<T> y) {
	return invert_bit_state(unsigned_less_than(y, x));
}

template<typename T>
bit_state signed_less_than(known_bit_integer<T> x, known_bit_integer<T> y) {
	arithmetic_flags flags;
	unsigned_compare(x, y, flags);
	bit_state is_less = xor_bit_state(flags.sign, flags.overflow);
	if (is_less == known_true) {
		return known_true;
	}
	if ((is_less == known_false) || (flags.zero == known_true)) {
		return known_false;
	}
	return unknown;
}

template<typename T>
bit_state signed_greater_equal(known_bit_integer<T> x, known_bit_integer<T> y) {
	return invert_bit_state(signed_less_than(x, y));
}

template<typename T>
bit_state signed_greater_than(known_bit_integer<T> x, known_bit_integer<T> y) {
	return signed_less_than(y, x);
}

template<typename T>
bit_state signed_less_equal(known_bit_integer<T> x, known_bit_integer<T> y) {
	return invert_bit_state(signed_less_than(y, x));
}

//------------------------------------------------------------------------------
// multiplication
//------------------------------------------------------------------------------

template<typename T>
known_bit_integer<T> multiply(known_bit_integer<T> x, known_bit_integer<T> y) {
	known_bit_integer<T> result;
	result.set_to_zero();
	for (size_t i = bit_width_of_type<T>(); i --> 0;) {
		result = shift_left_logical(result, 1);
		known_bit_integer<T> mult_mask;
		mult_mask.set_to_zero_or_all_ones(y.bit_test(i));
		result = addition(result, bitwise_and(mult_mask, x));
	}
	return result;
}

template<typename T>
void multiply_hilo_unsigned(known_bit_integer<T>& hi, known_bit_integer<T>& lo, known_bit_integer<T> x, known_bit_integer<T> y) {
	hi.set_to_zero();
	lo.set_to_zero();
	for (size_t i = bit_width_of_type<T>(); i --> 0;) {
		bit_state middle_bit = lo.test_signbit();
		lo = shift_left_logical(lo, 1);
		hi = shift_left_logical(hi, 1);
		hi.bit_copy(0, middle_bit);

		known_bit_integer<T> mult_mask;
		mult_mask.set_to_zero_or_all_ones(y.bit_test(i));
		bit_state carry_inout = known_false;
		lo = addition_with_carry(lo, bitwise_and(mult_mask, x), carry_inout);
		hi = increment_by_carry(hi, carry_inout);
	}
}

template<typename T>
known_bit_integer<T> multiply_hi_unsigned(known_bit_integer<T> x, known_bit_integer<T> y) {
	known_bit_integer<T> hi, lo;
	multiply_hilo_unsigned(hi, lo, x, y);
	return hi;
}

template<typename T>
void multiply_hilo_signed(known_bit_integer<T>& hi, known_bit_integer<T>& lo, known_bit_integer<T> x, known_bit_integer<T> y) {
	multiply_hilo_unsigned(hi, lo, absolute_value(x), absolute_value(y));
	bit_state sign = xor_bit_state(x.test_signbit(), y.test_signbit());
	if (sign == known_false) {
		return;
	}
	known_bit_integer<T> neg_hi;
	neg_hi = complement(hi);
	neg_hi = increment_by_carry(neg_hi, lo.is_zero());
	lo = multiply(x, y);
	if (sign == known_true) {
		hi = neg_hi;
		return;
	}
	hi.merge_bits_intersection(neg_hi);
}

template<typename T>
known_bit_integer<T> multiply_hi_signed(known_bit_integer<T> x, known_bit_integer<T> y) {
	known_bit_integer<T> hi, lo;
	multiply_hilo_signed(hi, lo, x, y);
	return hi;
}

template<typename T>
known_bit_integer<T> square(known_bit_integer<T> x) {
	/* the lower 4 bits can only be 0, 1, 4, or 9 */
	/* 
	| 0: 0000
	| 1: 0001
	| 4: 0100
	| 9: 1001
	*/
	/* 
	| 00: 00000
	| 01: 00001
	| 04: 00100
	| 09: 01001
	| 10: 10000
	| 11: 10001
	| 19: 11001
	*/
	/* 
	| 00: 000000
	| 01: 000001
	| 04: 000100
	| 09: 001001
	| 10: 010000
	| 11: 010001
	| 19: 011001
	| 21: 100001
	| 24: 100100
	| 29: 101001
	| 31: 110001
	| 39: 111001
	*/
	known_bit_integer<T> ret = multiply(x, x);
	ret.bit_clear(1); // guaranteed to be zero
	if (x.isknown_bit_set(0)) {
		ret.bit_clear(2);
	}
	if (x.isknown_bit_clear(0)) {
		ret.bit_clear(3);
	}
	if (ret.isknown_bit_set(2)) {
		ret.bit_clear(3);
		ret.bit_clear(4);
	}
	return ret;
}

template<typename T>
void square_hilo_unsigned(
	known_bit_integer<T>& hi,
	known_bit_integer<T>& lo,
	known_bit_integer<T> x
) {
	multiply_hilo_unsigned(hi, lo, x, x);
	lo = square(x);
}

template<typename T>
known_bit_integer<T> square_hi_unsigned(known_bit_integer<T> x) {
	known_bit_integer<T> hi, lo;
	square_hilo_unsigned(hi, lo, x);
	return hi;
}

template<typename T>
void square_hilo_signed(
	known_bit_integer<T>& hi,
	known_bit_integer<T>& lo,
	known_bit_integer<T> x
) {
	known_bit_integer<T> x_abs = absolute_value(x);
	multiply_hilo_unsigned(hi, lo, x_abs, x_abs);
	lo = square(x);
	hi.bit_clear(hi.get_signbit_index());
}

template<typename T>
known_bit_integer<T> square_hi_signed(known_bit_integer<T> x) {
	known_bit_integer<T> hi, lo;
	square_hilo_signed(hi, lo, x);
	return hi;
}

//------------------------------------------------------------------------------
// division and remainder
//------------------------------------------------------------------------------

template<typename T>
void divrem_trunc_unsigned(
	known_bit_integer<T>& quo,
	known_bit_integer<T>& rem,
	known_bit_integer<T> num,
	known_bit_integer<T> den
) {
	if (!den.isknown_nonzero()) {
		// We have not proved that a division by zero will not occur
		rem.set_unknown();
		quo.set_unknown();
		return;
	}
	quo.set_to_zero();
	rem.set_to_zero();
	for (size_t i = bit_width_of_type<T>(); i --> 0;) {
		rem.shift_left_logical(1);
		rem.bit_copy(0, num.bit_test(i));
		bit_state rem_ge_den = unsigned_greater_equal(rem, den);
		known_bit_integer<T> cmp_mask;
		cmp_mask.set_to_zero_or_all_ones(rem_ge_den);
		rem = subtract_ignore_carry(rem, (cmp_mask & den));
		quo.bit_copy(i, rem_ge_den);
	}
	rem.set_known_unsigned_range(0, den.get_unsigned_maximum() - 1);
}

template<typename T>
known_bit_integer<T> div_trunc_unsigned(known_bit_integer<T> num, known_bit_integer<T> den) {
	known_bit_integer<T> quo, rem;
	divrem_trunc_unsigned(quo, rem, num, den);
	return quo;
}

template<typename T>
known_bit_integer<T> rem_trunc_unsigned(known_bit_integer<T> num, known_bit_integer<T> den) {
	known_bit_integer<T> quo, rem;
	divrem_trunc_unsigned(quo, rem, num, den);
	return rem;
}

template<typename T>
void divrem_trunc_signed(
	known_bit_integer<T>& quo,
	known_bit_integer<T>& rem,
	known_bit_integer<T> num,
	known_bit_integer<T> den
) {
	divrem_trunc_unsigned(quo, rem, absolute_value(num), absolute_value(den));
	quo = conditional_negate(quo, xor_bit_state(num.test_signbit(), den.test_signbit()));
	rem = conditional_negate(rem, num.test_signbit());
}

template<typename T>
known_bit_integer<T> div_trunc_signed(known_bit_integer<T> num, known_bit_integer<T> den) {
	known_bit_integer<T> quo, rem;
	divrem_trunc_signed(quo, rem, num, den);
	return quo;
}

template<typename T>
known_bit_integer<T> rem_trunc_signed(known_bit_integer<T> num, known_bit_integer<T> den) {
	known_bit_integer<T> quo, rem;
	divrem_trunc_signed(quo, rem, num, den);
	return rem;
}

template<typename T>
void divrem_floor_signed(
	known_bit_integer<T>& quo,
	known_bit_integer<T>& rem,
	known_bit_integer<T> num,
	known_bit_integer<T> den
) {
	divrem_trunc_signed(quo, rem, num, den);
	bit_state modify_result = or_bit_state(
		and_bit_state(rem.is_greater_than_zero(), den.is_less_than_zero()),
		and_bit_state(rem.is_less_than_zero(), den.is_greater_than_zero())
	);
	quo = decrement_by_carry(quo, modify_result);
	known_bit_integer<T> den_mask;
	den_mask.set_to_zero_or_all_ones(modify_result);
	rem = addition(rem, (bitwise_and(den_mask, den)));
}

template<typename T>
known_bit_integer<T> div_floor_signed(known_bit_integer<T> num, known_bit_integer<T> den) {
	known_bit_integer<T> quo, rem;
	divrem_floor_signed(quo, rem, num, den);
	return quo;
}

template<typename T>
known_bit_integer<T> rem_floor_signed(known_bit_integer<T> num, known_bit_integer<T> den) {
	known_bit_integer<T> quo, rem;
	divrem_floor_signed(quo, rem, num, den);
	return rem;
}

template<typename T>
void divrem_euclidean_signed(
	known_bit_integer<T>& quo,
	known_bit_integer<T>& rem,
	known_bit_integer<T> num,
	known_bit_integer<T> den
) {
	divrem_trunc_signed(quo, rem, num, den);
	bit_state rem_lt_zero = rem.is_less_than_zero();
	bit_state den_lt_zero = den.is_less_than_zero();
	if (rem_lt_zero == known_false) {
		return;
	}
	rem = addition(rem, conditional_negate(den, den_lt_zero));
	quo = increment_if_true_decrement_if_false(quo, den_lt_zero);
}

template<typename T>
known_bit_integer<T> div_euclidean_signed(known_bit_integer<T> num, known_bit_integer<T> den) {
	known_bit_integer<T> quo, rem;
	divrem_euclidean_signed(quo, rem, num, den);
	return quo;
}

template<typename T>
known_bit_integer<T> rem_euclidean_signed(known_bit_integer<T> num, known_bit_integer<T> den) {
	known_bit_integer<T> quo, rem;
	divrem_euclidean_signed(quo, rem, num, den);
	return rem;
}

//------------------------------------------------------------------------------
// C23 <stdbit.h>
//------------------------------------------------------------------------------

template<typename T>
void leading_zeros(int& min_bound, int& max_bound, known_bit_integer<T> x) {
	min_bound = std::countl_zero(x.get_unsigned_maximum());
	max_bound = std::countl_zero(x.get_unsigned_minimum());
}

template<typename T>
void leading_ones(int& min_bound, int& max_bound, known_bit_integer<T> x) {
	return leading_zeros(min_bound, max_bound, complement(x));
}

template<typename T>
void trailing_zeros(int& min_bound, int& max_bound, known_bit_integer<T> x) {
	min_bound = std::countr_zero(x.get_unsigned_maximum());
	max_bound = std::countr_zero(x.get_unsigned_minimum());
}

template<typename T>
void trailing_ones(int& min_bound, int& max_bound, known_bit_integer<T> x) {
	return trailing_zeros(min_bound, max_bound, complement(x));
}

template<typename T>
void first_leading_one(int& min_bound, int& max_bound, known_bit_integer<T> x) {
	min_bound = std::countl_zero(x.get_unsigned_minimum()) + 1;
	max_bound = std::countl_zero(x.get_unsigned_maximum()) + 1;
	if (x.get_unsigned_minimum() == 0) {
		min_bound = 0;
	}
	if (x.get_unsigned_maximum() == 0) {
		max_bound = 0;
	}
}

template<typename T>
void first_leading_zero(int& min_bound, int& max_bound, known_bit_integer<T> x) {
	return first_leading_one(min_bound, max_bound, complement(x));
}

template<typename T>
void first_trailing_one(int& min_bound, int& max_bound, known_bit_integer<T> x) {
	min_bound = std::countr_zero(x.get_unsigned_minimum()) + 1;
	max_bound = std::countr_zero(x.get_unsigned_maximum()) + 1;
	if (x.get_unsigned_minimum() == 0) {
		min_bound = 0;
	}
	if (x.get_unsigned_maximum() == 0) {
		max_bound = 0;
	}
}

template<typename T>
void first_trailing_zero(int& min_bound, int& max_bound, known_bit_integer<T> x) {
	return first_trailing_one(min_bound, max_bound, complement(x));
}

template<typename T>
void count_ones(int& min_bound, int& max_bound, known_bit_integer<T> x) {
	min_bound = std::popcount(x.get_unsigned_minimum());
	max_bound = std::popcount(x.get_unsigned_maximum());
}

template<typename T>
void count_zeros(int& min_bound, int& max_bound, known_bit_integer<T> x) {
	return count_ones(min_bound, max_bound, complement(x));
}

template<typename T>
bit_state has_single_bit(known_bit_integer<T> x) {
	// (x && !(x & (x - 1)))
	// ((x != 0) & ((x & (x - 1)) == 0))
	return and_bit_state(x.is_nonzero(), bitwise_and(x, decrement(x)).is_zero());
}

template<typename T>
void bit_width(int& min_bound, int& max_bound, known_bit_integer<T> x) {
	leading_zeros(min_bound, max_bound, x);
	min_bound = bit_width_of_type<T>() - min_bound;
	max_bound = bit_width_of_type<T>() - max_bound;
}

template<typename T>
known_bit_integer<T> bit_floor(known_bit_integer<T> x) {
	// return (x == 0) ? 0 : (1 << (bit_width(x) - 1))
	known_bit_integer<T> ret;
	if (x.isknown_zero()) {
		ret.set_to_zero();
		return ret;
	}
	if (!x.isknown_nonzero()) {
		ret.set_unknown();
		return ret;
	}
	int min_bound, max_bound;
	bit_width(min_bound, max_bound, x);
	min_bound--;
	max_bound--;
	if (min_bound == max_bound) {
		ret.set_value(1);
		ret = shift_left_logical(ret, min_bound);
		return ret;
	}
	ret.set_unknown();
	ret = shift_left_logical(ret, min_bound);
	return ret;
}

template<typename T>
known_bit_integer<T> bit_ceil(known_bit_integer<T> x) {
	// ((x < 2) ? 1 : (2 << (bit_width(x - 1) - 1)))
	known_bit_integer<T> ret;
	if (x.get_unsigned_maximum() < 2) {
		ret.set_value(1);
		return ret;
	}
	if (x.get_unsigned_minimum() < 2) {
		ret.set_unknown();
		return ret;
	}
	int min_bound, max_bound;
	bit_width(min_bound, max_bound, decrement(x));
	min_bound--;
	max_bound--;
	if (min_bound == max_bound) {
		ret.set_value(2);
		ret = shift_left_logical(ret, min_bound);
		return ret;
	}
	ret.set_unknown();
	ret.bit_clear(0);
	ret = shift_left_logical(ret, min_bound);
	return ret;
}

/* other bit functions */

template<typename T>
bit_state is_pairity_even(known_bit_integer<T> x) {
	int min_bound, max_bound;
	count_ones(min_bound, max_bound, x);
	if (min_bound == max_bound) {
		return ((min_bound & 1) == 0) ? known_true : known_false;
	}
	return unknown;
}

template<typename T>
bit_state is_pairity_odd(known_bit_integer<T> x) {
	return invert_bit_state(is_pairity_even(x));
}

template<typename T>
void leading_signbits(int& min_bound, int& max_bound, known_bit_integer<T> x) {
	bit_state sign = x.test_signbit();
	if (sign == known_true) {
		leading_ones(min_bound, max_bound, x);
		return;
	}
	if (sign == known_false) {
		leading_zeros(min_bound, max_bound, x);
		return;
	}
	int min_pos_bound, max_pos_bound;
	int min_neg_bound, max_neg_bound;
	known_bit_integer<T> pos = x;
	known_bit_integer<T> neg = x;
	pos.bit_clear(pos.get_signbit_index());
	neg.bit_set(neg.get_signbit_index());
	leading_zeros(min_pos_bound, max_pos_bound, x);
	leading_ones(min_neg_bound, max_neg_bound, x);
	min_bound = std::min(min_pos_bound, min_neg_bound);
	max_bound = std::min(max_pos_bound, max_neg_bound);
}

//------------------------------------------------------------------------------
// reverse_bits
//------------------------------------------------------------------------------

template<typename T>
known_bit_integer<T> reverse_bits(known_bit_integer<T> x) {
	known_bit_integer<T> ret;
	size_t r = 0;
	for (size_t f = bit_width_of_type<T>(); f --> 0;) {
		ret.bit_copy(r, x.bit_test(f));
		r++;
	}
	return ret;
}

#if !KBI_NOBUILTINS

inline known_bit_integer<uint8_t> reverse_bits(known_bit_integer<uint8_t> x) {
	x.bits = __builtin_bitreverse8(x.bits);
	x.mask = __builtin_bitreverse8(x.mask);
	return x;
}

inline known_bit_integer<uint16_t> reverse_bits(known_bit_integer<uint16_t> x) {
	x.bits = __builtin_bitreverse16(x.bits);
	x.mask = __builtin_bitreverse16(x.mask);
	return x;
}

inline known_bit_integer<uint32_t> reverse_bits(known_bit_integer<uint32_t> x) {
	x.bits = __builtin_bitreverse32(x.bits);
	x.mask = __builtin_bitreverse32(x.mask);
	return x;
}

inline known_bit_integer<uint64_t> reverse_bits(known_bit_integer<uint64_t> x) {
	x.bits = __builtin_bitreverse64(x.bits);
	x.mask = __builtin_bitreverse64(x.mask);
	return x;
}

#endif /* CHAR_BIT == 8 */

//------------------------------------------------------------------------------
// swap_byte_order
//------------------------------------------------------------------------------

#if !KBI_NOBUILTINS

inline known_bit_integer<uint8_t> swap_byte_order(known_bit_integer<uint8_t> x) {
	return x;
}

inline known_bit_integer<uint16_t> swap_byte_order(known_bit_integer<uint16_t> x) {
	x.bits = __builtin_bswap16(x.bits);
	x.mask = __builtin_bswap16(x.mask);
	return x;
}

inline known_bit_integer<uint32_t> swap_byte_order(known_bit_integer<uint32_t> x) {
	x.bits = __builtin_bswap32(x.bits);
	x.mask = __builtin_bswap32(x.mask);
	return x;
}

inline known_bit_integer<uint64_t> swap_byte_order(known_bit_integer<uint64_t> x) {
	x.bits = __builtin_bswap64(x.bits);
	x.mask = __builtin_bswap64(x.mask);
	return x;
}

#endif /* CHAR_BIT == 8 */

} // namespace kbi

#endif /* KNOWN_BIT_INTEGER_HPP */
