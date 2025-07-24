#ifndef CURRENT_STATE_HPP
#define CURRENT_STATE_HPP

#include "../ez80_type.h"
#include "../common_std.h"
#include "../ez80_reg.h"
#include "../ez80_instruction.h"
#include "../ez80_known_function.h"

enum class flag_state {
	unknown,
	known_true,
	known_false
};

inline bool isknown(flag_state f) {
	using enum flag_state;
	return (f != unknown);
}

inline bool isknown_true(flag_state f) {
	using enum flag_state;
	return (f == known_true);
}

inline bool isknown_false(flag_state f) {
	using enum flag_state;
	return (f == known_false);
}

inline flag_state set_flag_state(bool b) {
	using enum flag_state;
	return (b ? known_true : known_false);
}

inline flag_state set_flag_state(flag_state f) {
	return f;
}

inline flag_state and_flag_state(flag_state x, flag_state y) {
	using enum flag_state;
	/*
	| AND | 0 | 1 | ?
	|-----|-----------
	| 0   | 0 : 0 : 0
	| 1   | 0 : 1 : ?
	| ?   | 0 : ? : ?
	*/
	if (x == known_false || y == known_false) {
		return known_false;
	}
	if (x == known_true && y == known_true) {
		return known_true;
	}
	return unknown;
}

inline flag_state and_flag_state(flag_state x, flag_state y, flag_state z) {
	return and_flag_state(and_flag_state(x, y), z);
}

inline flag_state or_flag_state(flag_state x, flag_state y) {
	using enum flag_state;
	/*
	| OR  | 0 | 1 | ?
	|-----|-----------
	| 0   | 0 : 1 : ?
	| 1   | 1 : 1 : 1
	| ?   | ? : 1 : ?
	*/
	if (x == known_true || y == known_true) {
		return known_true;
	}
	if (x == known_false && y == known_false) {
		return known_false;
	}
	return unknown;
}

inline flag_state or_flag_state(flag_state x, flag_state y, flag_state z) {
	return or_flag_state(or_flag_state(x, y), z);
}

inline flag_state xor_flag_state(flag_state x, flag_state y) {
	using enum flag_state;
	/*
	| XOR | 0 | 1 | ?
	|-----|-----------
	| 0   | 0 : 1 : ?
	| 1   | 1 : 0 : ?
	| ?   | ? : ? : ?
	*/
	if (x == unknown || y == unknown) {
		return unknown;
	}
	if (x == y) {
		return known_false;
	}
	return known_true;
}

inline flag_state xor_flag_state(flag_state x, flag_state y, flag_state z) {
	return xor_flag_state(xor_flag_state(x, y), z);
}

inline flag_state cpl_flag_state(flag_state x) {
	using enum flag_state;
	if (x == known_true) {
		return known_false;
	}
	if (x == known_false) {
		return known_true;
	}
	return unknown;
}

inline const char* to_str_flag_state(flag_state x) {
	using enum flag_state;
	if (x == known_true) {
		return "true";
	}
	if (x == known_false) {
		return "false";
	}
	return "unknown";
}

template<typename T>
struct reg_pair {
public:
	T bits;
	T mask;

private:
	T get_bit_index(int b) const {
		if (b < bit_width_of_type<T>()) {
			return (static_cast<T>(1) << b);
		}
		// out of range
		return 0;
	}

public:

	reg_pair<T>() = default;

	/* canonical */

	void set_canonical() {
		// set unknown bits to zero
		bits &= mask;
	}
	bool is_canonical() const {
		// return false if any unknown bits are non-zero
		return ((bits & ~mask) == 0);
	}

	/* setters */

	void set_unknown() {
		mask = 0;
		bits = 0;
	}
	void set_value(T x) {
		bits = x;
		mask = get_all_ones<T>();
	}
	void set_value(reg_pair<T> x) {
		bits = x.bits;
		mask = x.mask;
	}

	/* set to flag */

	void set_to_zero_or_one(flag_state f) {
		using enum flag_state;
		set_value(0);
		if (f == known_false) {
			return;
		}
		if (f == known_true) {
			bit_set(0);
			return;
		}
		bit_unknown(0);
	}
	void set_to_zero_or_one(bool f) {
		using enum flag_state;
		set_to_zero_or_one(f ? known_true : known_false);
	}
	void set_to_zero_or_cpl(flag_state f) {
		using enum flag_state;
		if (f == known_false) {
			set_value(0);
			return;
		}
		if (f == known_true) {
			set_value(get_all_ones<T>());
			return;
		}
		set_unknown();
	}
	void set_to_zero_or_cpl(bool f) {
		using enum flag_state;
		set_to_zero_or_cpl(f ? known_true : known_false);
	}

	/* bit operations */

	void complement() {
		bits = ~bits;
		bits &= mask;
	}

	flag_state bit_test(int b) const {
		using enum flag_state;
		const T index = get_bit_index(b);
		if (mask & index) {
			return (bits & index) ? known_true : known_false;
		}
		return unknown;
	}

	bool isknown_bit(int b) const {
		using enum flag_state;
		return (bit_test(b) != unknown);
	}
	bool isknown_bit_set(int b) const {
		using enum flag_state;
		return (bit_test(b) == known_true);
	}
	bool isknown_bit_clear(int b) const {
		using enum flag_state;
		return (bit_test(b) == known_false);
	}

	void bit_set(int b) {
		const T index = get_bit_index(b);
		mask |= index; // bit is known
		bits |= index; // set bit
	}
	void bit_clear(int b) {
		const T index = get_bit_index(b);
		mask |= index; // bit is known
		bits &= ~index; // clear bit
	}
	void bit_flip(int b) {
		const T index = get_bit_index(b);
		bits ^= index; // flip bit
		bits &= mask; // canonicalize
	}
	void bit_unknown(int b) {
		const T index = get_bit_index(b);
		mask &= ~index; // bit is unknown
		bits &= ~index; // canonicalize to zero
	}
	void bit_copy(int b, flag_state f) {
		using enum flag_state;
		switch (f) {
			case unknown: bit_unknown(b); return;
			case known_true: bit_set(b); return;
			case known_false: bit_clear(b); return;
		}
	}

	/* tests */

	bool isknown_fully() const {
		T temp = ~mask;
		return (temp == 0);
	}
	bool isknown_zero() const {
		return (isknown_fully() && (bits == 0));
	}
	bool isknown_nonzero() const {
		/* test if any bits are known to set */
		return ((mask & bits) != 0);
	}
	flag_state is_zero() const {
		using enum flag_state;
		if (isknown_zero()) {
			return known_true;
		}
		if (isknown_nonzero()) {
			return known_false;
		}
		return unknown;
	}
	flag_state is_nonzero() const {
		using enum flag_state;
		if (isknown_zero()) {
			return known_false;
		}
		if (isknown_nonzero()) {
			return known_true;
		}
		return unknown;
	}

	bool isknown_equal(T x) const {
		return (isknown_fully() && (bits == x));
	}
	bool isknown_notequal(T x) const {
		return (((mask & (bits ^ x))) != 0);
	}
	flag_state is_equal(T x) const {
		using enum flag_state;
		if (isknown_equal(x)) {
			return known_true;
		}
		if (isknown_notequal(x)) {
			return known_false;
		}
		return unknown;
	}
	flag_state is_notequal(T x) const {
		using enum flag_state;
		if (isknown_equal(x)) {
			return known_false;
		}
		if (isknown_notequal(x)) {
			return known_true;
		}
		return unknown;
	}

	/* query */

	flag_state is_parity_even() const {
		using enum flag_state;
		if (isknown_fully()) {
			return ((std::popcount(bits) & 1) == 0) ? known_true : known_false;
		}
		return unknown;
	}

	flag_state test_signbit() const {
		return bit_test(bit_width_of_type<T>() - 1);
	}

	/* shift */

	void shift_left_logical(uint8_t shift) {
		if (shift >= bit_width_of_type<T>()) {
			set_unknown();
			return;
		}
		bits = (bits << shift);
		mask = ~(~mask << shift);
	}

	void shift_right_logical(uint8_t shift) {
		if (shift >= bit_width_of_type<T>()) {
			set_unknown();
			return;
		}
		bits = (bits >> shift);
		mask = ~(~mask >> shift);
	}

	void shift_left_ones(uint8_t shift) {
		if (shift >= bit_width_of_type<T>()) {
			set_unknown();
			return;
		}
		bits = ~(~bits << shift);
		mask = ~(~mask << shift);
	}

	void shift_right_ones(uint8_t shift) {
		if (shift >= bit_width_of_type<T>()) {
			set_unknown();
			return;
		}
		bits = ~(~bits >> shift);
		mask = ~(~mask >> shift);
	}

	void shift_right_arithmetic(uint8_t shift) {
		if (shift >= bit_width_of_type<T>()) {
			set_unknown();
			return;
		}
		if (isknown(test_signbit())) {
			if (isknown_true(test_signbit())) {
				shift_right_ones(shift);
				return;
			}
			shift_right_logical(shift);
			return;
		}
		bits >>= shift;
		mask >>= shift;
	}

	reg_pair<T> increment() {
		using enum flag_state;
		flag_state carry = known_true;
		for (size_t i = 0; i < bit_width_of_type<T>(); i++) {
			const flag_state X = this->bit_test(i);
			// SUM = X ^ CIN
			// COUT = X & CIN
			this->bit_copy(i, xor_flag_state(X, carry));
			carry = and_flag_state(carry, X);
		}
		return *this;
	}

	reg_pair<T> decrement() {
		using enum flag_state;
		flag_state carry = known_true;
		for (size_t i = 0; i < bit_width_of_type<T>(); i++) {
			const flag_state X = this->bit_test(i);
			// DIFF = X ^ CIN
			// COUT = ~X & CIN
			this->bit_copy(i, xor_flag_state(X, carry));
			carry = and_flag_state(cpl_flag_state(X), carry);
		}
		return *this;
	}
};

template<typename T>
reg_pair<T> operator|(reg_pair<T> x, reg_pair<T> y) {
	/*
	| OR  | 0 | 1 | ?
	|-----|-----------
	| 0   | 0 : 1 : ?
	| 1   | 1 : 1 : 1
	| ?   | ? : 1 : ?
	*/
	reg_pair<T> result;
	x.bits &= x.mask;
	y.bits &= y.mask;
	T known_ones = (x.bits & x.mask) | (y.bits & y.mask);
	result.bits = x.bits | y.bits;
	result.mask = x.mask & y.mask;
	result.mask |= known_ones;
	result.bits &= result.mask;
	return result;
}

template<typename T>
reg_pair<T> operator&(reg_pair<T> x, reg_pair<T> y) {
	/*
	| AND | 0 | 1 | ?
	|-----|-----------
	| 0   | 0 : 0 : 0
	| 1   | 0 : 1 : ?
	| ?   | 0 : ? : ?
	*/
	reg_pair<T> result;
	x.bits &= x.mask;
	y.bits &= y.mask;
	T known_zeros = (~x.bits & x.mask) | (~y.bits & y.mask);
	result.bits = x.bits & y.bits;
	result.mask = x.mask & y.mask;
	result.mask |= known_zeros;
	result.bits &= result.mask;
	return result;
}

template<typename T>
reg_pair<T> operator^(reg_pair<T> x, reg_pair<T> y) {
	/*
	| XOR | 0 | 1 | ?
	|-----|-----------
	| 0   | 0 : 1 : ?
	| 1   | 1 : 0 : ?
	| ?   | ? : ? : ?
	*/
	reg_pair<T> result;
	x.bits &= x.mask;
	y.bits &= y.mask;
	result.bits = x.bits ^ y.bits;
	result.mask = x.mask & y.mask;
	result.bits &= result.mask;
	return result;
}

template<typename T>
reg_pair<T> add_with_carry(reg_pair<T> x, reg_pair<T> y, flag_state& carry) {
	using enum flag_state;
	for (size_t i = 0; i < bit_width_of_type<T>(); i++) {
		// 1 bit full adder
		const flag_state X = x.bit_test(i);
		const flag_state Y = y.bit_test(i);
		// SUM = X ^ Y ^ CIN
		// COUT = (X & Y) | (CIN & (X ^ Y))
		x.bit_copy(i, xor_flag_state(X, Y, carry));
		carry = or_flag_state(
			and_flag_state(X, Y),
			and_flag_state(carry, xor_flag_state(X, Y))
		);
	}
	return x;
}

template<typename T>
reg_pair<T> add_ignore_carry(reg_pair<T> x, reg_pair<T> y) {
	using enum flag_state;
	flag_state carry = known_false;
	return add_with_carry(x, y, carry);
}

template<typename T>
reg_pair<T> subtract_with_carry(reg_pair<T> x, reg_pair<T> y, flag_state& carry) {
	using enum flag_state;
	for (size_t i = 0; i < bit_width_of_type<T>(); i++) {
		// 1 bit full subtractor
		flag_state X = x.bit_test(i);
		flag_state Y = y.bit_test(i);
		// DIFF = X ^ Y ^ CIN
		// COUT = X < (Y + C)
		// COUT = (~X & Y) | (CIN & ~(X ^ Y))
		x.bit_copy(i, xor_flag_state(X, Y, carry));
		carry = or_flag_state(
			and_flag_state(cpl_flag_state(X), Y),
			and_flag_state(carry, cpl_flag_state(xor_flag_state(X, Y)))
		);
	}
	return x;
}

template<typename T>
reg_pair<T> subtract_ignore_carry(reg_pair<T> x, reg_pair<T> y) {
	using enum flag_state;
	flag_state carry = known_false;
	return subtract_with_carry(x, y, carry);
}

template<typename T>
void unsigned_compare(flag_state& carry, flag_state& zero, reg_pair<T> x, reg_pair<T> y) {
	using enum flag_state;
	carry = known_false;
	zero = subtract_with_carry(x, y, carry).is_zero();
}

template<typename T>
flag_state unsigned_less_than(reg_pair<T> x, reg_pair<T> y) {
	using enum flag_state;
	flag_state carry, zero;
	unsigned_compare(carry, zero, x, y);
	if (carry == known_true) {
		return known_true;
	}
	if ((carry == known_false) || (zero == known_true)) {
		return known_false;
	}
	return unknown;
}

template<typename T>
flag_state unsigned_greater_equal(reg_pair<T> x, reg_pair<T> y) {
	return cpl_flag_state(unsigned_less_than(x, y));
}

template<typename T>
flag_state unsigned_greater_than(reg_pair<T> x, reg_pair<T> y) {
	return unsigned_less_than(y, x);
}

template<typename T>
flag_state unsigned_less_equal(reg_pair<T> x, reg_pair<T> y) {
	return cpl_flag_state(unsigned_less_than(y, x));
}

template<typename T>
flag_state compare_equal(reg_pair<T> x, reg_pair<T> y) {
	#if 0
		using enum flag_state;
		if (x.isknown_fully() && y.isknown_fully()) {
			return ((x.bits == y.bits) ? known_true : known_false);
		}
	#else
		flag_state carry, zero;
		unsigned_compare(carry, zero, x, y);
		return zero;
	#endif
}

template<typename T>
flag_state compare_notequal(reg_pair<T> x, reg_pair<T> y) {
	return cpl_flag_state(compare_equal(x, y));
}

template<typename T>
reg_pair<T> multiply_same_width(reg_pair<T> x, reg_pair<T> y) {
	using enum flag_state;
	reg_pair<T> result;
	result.set_value(0);
	for (size_t i = bit_width_of_type<T>(); i --> 0;) {
		result.shift_left_logical(1);
		reg_pair<T> mult_mask;
		mult_mask.set_to_zero_or_cpl(y.bit_test(i));
		result = add_ignore_carry(result, (mult_mask & x));
	}
	return result;
}

template<typename T>
void divrem_unsigned(
	reg_pair<T>& quo, reg_pair<T>& rem,
	reg_pair<T> num, reg_pair<T> den
) {
	using enum flag_state;
	if (!den.isknown_nonzero()) {
		// We have not proved that a division by zero will not occur
		rem.set_unknown();
		quo.set_unknown();
		return;
	}
	quo.set_value(0);
	rem.set_value(0);
	for (size_t i = bit_width_of_type<T>(); i --> 0;) {
		rem.shift_left_logical(1);
		rem.bit_copy(0, num.bit_test(i));
		flag_state rem_ge_den = unsigned_greater_equal(rem, den);
		reg_pair<T> cmp_mask;
		cmp_mask.set_to_zero_or_cpl(rem_ge_den);
		rem = subtract_ignore_carry(rem, (cmp_mask & den));
		quo.bit_copy(i, rem_ge_den);
	}
	if (den.isknown_fully()) {
		// the remainder will be less than the quotient
		T rem_mask = ~((bit_floor(static_cast<T>(den.bits - 1)) << 1) - 1);
		rem.bits &= ~rem_mask;
		rem.mask |= rem_mask;
	}
}

template<typename T>
reg_pair<T> div_unsigned(reg_pair<T> num, reg_pair<T> den) {
	reg_pair<T> quo, rem;
	divrem_unsigned(quo, rem, num, den);
	return quo;
}

template<typename T>
reg_pair<T> rem_unsigned(reg_pair<T> num, reg_pair<T> den) {
	reg_pair<T> quo, rem;
	divrem_unsigned(quo, rem, num, den);
	return rem;
}

template<typename T>
reg_pair<T> operator~(reg_pair<T> x) {
	x.complement();
	return x;
}

template<typename T>
reg_pair<T> operator-(reg_pair<T> x) {
	x.complement();
	x.increment();
	return x;
}

template<typename T>
reg_pair<T> operator+(reg_pair<T> x, reg_pair<T> y) {
	#if 0
		if (x.isknown_fully() && y.isknown_fully()) {
			reg_pair<T> result;
			result.set_value(x.bits + y.bits);
			return result;
		}
	#endif
	return add_ignore_carry(x, y);
}

template<typename T>
reg_pair<T> operator-(reg_pair<T> x, reg_pair<T> y) {
	#if 0
		if (x.isknown_fully() && y.isknown_fully()) {
			reg_pair<T> result;
			result.set_value(x.bits - y.bits);
			return result;
		}
	#endif
	return subtract_ignore_carry(x, y);
}

template<typename T>
reg_pair<T> operator*(reg_pair<T> x, reg_pair<T> y) {
	return multiply_same_width(x, y);
}

template<typename T>
reg_pair<T>& operator++(reg_pair<T>& x) {
	x.increment();
	return x;
}

template<typename T>
reg_pair<T>& operator--(reg_pair<T>& x) {
	x.decrement();
	return x;
}

template<typename T>
void merge_tst_was_zero_bits(reg_pair<T>& x, reg_pair<T>& y) {
	using enum flag_state;
	for (size_t i = 0; i < bit_width_of_type<T>(); i++) {
		// ((X & Y) == 0) so one of them must be zero
		if (x.isknown_bit_set(i) && !y.isknown_bit(i)) {
			y.bit_clear(i);
			continue;
		}
		if (y.isknown_bit_set(i) && !x.isknown_bit(i)) {
			x.bit_clear(i);
			continue;
		}
	}
}

template<typename T>
void merge_cp_was_zero_bits(reg_pair<T>& x, reg_pair<T>& y) {
	using enum flag_state;
	for (size_t i = 0; i < bit_width_of_type<T>(); i++) {
		// ((X ^ Y) == 0) so both bits are the same
		if (x.isknown_bit(i) && !y.isknown_bit(i)) {
			y.bit_copy(i, x.bit_test(i));
			continue;
		}
		if (y.isknown_bit(i) && !x.isknown_bit(i)) {
			x.bit_copy(i, y.bit_test(i));
			continue;
		}
	}
}

typedef reg_pair<uint8_t> reg8_pair;

struct reg16_pair : public reg_pair<uint16_t> {
	reg16_pair() {
		bits = 0;
		mask = 0;
	}
	reg16_pair(reg_pair<uint16_t> src) {
		bits = src.bits;
		mask = src.mask;
	}
	void set_value(reg_pair<uint16_t> src) {
		bits = src.bits;
		mask = src.mask;
	}
	void set_value(reg16_pair src) {
		bits = src.bits;
		mask = src.mask;
	}
	void set_value(uint16_t src) {
		bits = src;
		mask = get_all_ones<uint16_t>();
	}
	void set_value(reg8_pair hi, reg8_pair lo) {
		bits = ((uint16_t)hi.bits << 8) | lo.bits;
		mask = ((uint16_t)hi.mask << 8) | lo.mask;
	}
	reg8_pair get_lo() const {
		reg8_pair ret;
		ret.bits = (uint8_t)(bits);
		ret.mask = (uint8_t)(mask);
		return ret;
	}
	reg8_pair get_hi() const {
		reg8_pair ret;
		ret.bits = (uint8_t)(bits >> 8);
		ret.mask = (uint8_t)(mask >> 8);
		return ret;
	}
	void split_value(reg8_pair& hi, reg8_pair& lo) const {
		hi = get_hi();
		lo = get_lo();
	}
	void split_value(reg8_pair& up, reg8_pair& hi, reg8_pair& lo) const {
		up.set_value(0);
		hi = get_hi();
		lo = get_lo();
	}
	void set_zero_extend(reg8_pair src) {
		bits = (uint16_t)src.bits;
		mask = (uint16_t)src.bits;
		bits &= UINT16_C(0x00FF);
		mask |= UINT16_C(0xFF00);
		bits &= mask;
	}
	void set_sign_extend(reg8_pair x) {
		using enum flag_state;
		mask = (uint24_t)x.mask;
		bits = (uint24_t)x.bits;
		bits &= mask;
		flag_state sign_bit = x.test_signbit();
		if (sign_bit == unknown) {
			// unable to sign extend
			return;
		}
		mask |= UINT16_C(0xFF00);
		if (sign_bit == known_false) {
			// zero extend
			bits &= UINT16_C(0x00FF);
			return;
		}
		// one extend
		bits |= UINT16_C(0xFF00);
		uint8_t value = x.bits;
	}
};

struct reg24_pair : public reg_pair<uint24_t> {
	reg24_pair() {
		bits = 0;
		mask = 0;
	}
	reg24_pair(reg_pair<uint24_t> src) {
		bits = src.bits;
		mask = src.mask;
	}
	void set_value(reg_pair<uint24_t> src) {
		bits = src.bits;
		mask = src.mask;
	}
	void set_value(uint24_t src) {
		bits = src;
		mask = get_all_ones<uint24_t>();
	}
	void set_value(reg8_pair hi, reg8_pair lo) {
		bits = ((uint24_t)hi.bits << 8) | lo.bits;
		mask = ((uint24_t)hi.mask << 8) | lo.mask;
	}
	void set_value(reg8_pair up, reg8_pair hi, reg8_pair lo) {
		bits = ((uint24_t)up.bits << 16) | ((uint24_t)hi.bits << 8) | lo.bits;
		mask = ((uint24_t)up.mask << 16) | ((uint24_t)hi.mask << 8) | lo.mask;
	}
	reg8_pair get_lo() const {
		reg8_pair ret;
		ret.bits = (uint8_t)(bits);
		ret.mask = (uint8_t)(mask);
		return ret;
	}
	reg8_pair get_hi() const {
		reg8_pair ret;
		ret.bits = (uint8_t)(bits >> 8);
		ret.mask = (uint8_t)(mask >> 8);
		return ret;
	}
	reg8_pair get_upper() const {
		reg8_pair ret;
		ret.bits = (uint8_t)(bits >> 16);
		ret.mask = (uint8_t)(mask >> 16);
		return ret;
	}
	void split_value(reg8_pair& hi, reg8_pair& lo) const {
		hi = get_hi();
		lo = get_lo();
	}
	void split_value(reg8_pair& up, reg8_pair& hi, reg8_pair& lo) const {
		up = get_upper();
		hi = get_hi();
		lo = get_lo();
	}
	void clear_upper() {
		bits &= 0x00FFFF;
		mask |= 0xFF0000;
	}
	void set_upper_unknown() {
		bits &= 0x00FFFF;
		mask &= 0x00FFFF;
	}
	void set_zero_extend(reg8_pair src) {
		bits = (uint24_t)src.bits;
		mask = (uint24_t)src.mask;
		bits &= UINT24_C(0x0000FF);
		mask |= UINT24_C(0xFFFF00);
		bits &= mask;
	}
	void set_zero_extend(reg16_pair src) {
		bits = (uint24_t)src.bits;
		mask = (uint24_t)src.mask;
		bits &= UINT24_C(0x00FFFF);
		mask |= UINT24_C(0xFF0000);
		bits &= mask;
	}
	void set_sign_extend(reg8_pair x) {
		using enum flag_state;
		bits = (uint24_t)x.bits;
		mask = (uint24_t)x.mask;
		bits &= mask;
		flag_state sign_bit = x.test_signbit();
		if (sign_bit == unknown) {
			// unable to sign extend
			return;
		}
		mask |= UINT24_C(0xFFFF00);
		if (sign_bit == known_false) {
			// zero extend
			bits &= UINT24_C(0x0000FF);
			return;
		}
		// one extend
		bits |= UINT24_C(0xFFFF00);
		uint8_t value = x.bits;
	}
};

struct reg32_pair : public reg_pair<uint32_t> {
	reg32_pair() {
		bits = 0;
		mask = 0;
	}
	reg32_pair(reg_pair<uint32_t> src) {
		bits = src.bits;
		mask = src.mask;
	}
	reg8_pair get_hi8() const {
		reg8_pair ret;
		ret.bits = (uint8_t)(bits >> 24);
		ret.mask = (uint8_t)(mask >> 24);
		return ret;
	}
	reg24_pair get_lo24() const {
		reg24_pair ret;
		ret.bits = (uint24_t)(bits);
		ret.mask = (uint24_t)(mask);
		return ret;
	}
	void set_value(reg8_pair tp, reg8_pair up, reg8_pair hi, reg8_pair lo) {
		bits = ((uint32_t)tp.bits << 24) | ((uint32_t)up.bits << 16) | ((uint32_t)hi.bits << 8) | lo.bits;
		mask = ((uint32_t)tp.mask << 24) | ((uint32_t)up.mask << 16) | ((uint32_t)hi.mask << 8) | lo.mask;
	}
	void set_sign_extend(reg24_pair x) {
		using enum flag_state;
		mask = (uint24_t)x.mask;
		bits = (uint24_t)x.bits;
		bits &= mask;
		flag_state sign_bit = x.test_signbit();
		if (sign_bit == unknown) {
			// unable to sign extend
			return;
		}
		mask |= UINT32_C(0xFF000000);
		if (sign_bit == known_false) {
			// zero extend
			bits &= UINT32_C(0x00FFFFFF);
			return;
		}
		// one extend
		bits |= UINT24_C(0xFF000000);
		uint8_t value = x.bits;
	}
	void set_zero_extend(reg24_pair src) {
		bits = (uint32_t)src.bits;
		mask = (uint32_t)src.bits;
		bits &= UINT32_C(0x00FFFFFF);
		mask |= UINT32_C(0xFF000000);
		bits &= mask;
	}
};

class flag_pair {
	ez80_flag bits;
	ez80_flag mask;
private:
	flag_state flag_getter(bool known, bool flag) const {
		using enum flag_state;
		if (known) {
			return flag ? known_true : known_false;
		}
		return unknown;
	}
	void flag_setter(flag_state f, uint8_t flag_bit) {
		using enum flag_state;
		if (f == unknown) {
			mask.raw &= ~flag_bit;
			bits.raw &= ~flag_bit;
			return;
		}
		mask.raw |= flag_bit;
		if (f == known_true) {
			bits.raw |= flag_bit;
			return;
		}
		bits.raw &= ~flag_bit;
	}
public:

	void export_flag_pair(reg8_pair& dst) const {
		dst.bits = bits.raw;
		dst.mask = mask.raw;
	}

	void import_flag_pair(reg8_pair src) {
		bits.raw = src.bits;
		mask.raw = src.mask;
	}

	/* setters */

	void set_carry(bool f) {
		mask.carry = 1;
		bits.carry = f;
	}
	void set_zero(bool f) {
		mask.zero = 1;
		bits.zero = f;
	}
	void set_sign(bool f) {
		mask.sign = 1;
		bits.sign = f;
	}
	void set_overflow(bool f) {
		mask.overflow = 1;
		bits.overflow = f;
	}

	void set_carry(flag_state f) {
		flag_setter(f, (1 << 0));
	}
	void set_zero(flag_state f) {
		flag_setter(f, (1 << 6));
	}
	void set_sign(flag_state f) {
		flag_setter(f, (1 << 7));
	}
	void set_overflow(flag_state f) {
		flag_setter(f, (1 << 5));
	}

	void set_flags_unknown() {
		mask.raw = 0;
		bits.raw = 0;
	}

	void set_all_but_carry_unknown() {
		mask.raw &= (1 << 0);
		bits.raw &= (1 << 0);
	}

	void set_carry_unknown(void) {
		mask.carry = 0;
		bits.carry = 0;
	}
	void set_zero_unknown(void) {
		mask.zero = 0;
		bits.zero = 0;
	}
	void set_sign_unknown(void) {
		mask.sign = 0;
		bits.sign = 0;
	}
	void set_overflow_unknown(void) {
		mask.overflow = 0;
		bits.overflow = 0;
	}

	/* getters */

	flag_state get_carry() const {
		return flag_getter(mask.carry, bits.carry);
	}
	flag_state get_zero() const {
		return flag_getter(mask.zero, bits.zero);
	}
	flag_state get_sign() const {
		return flag_getter(mask.sign, bits.sign);
	}
	flag_state get_overflow() const {
		return flag_getter(mask.overflow, bits.overflow);
	}

	bool isknown_carry() const {
		return mask.carry;
	}
	bool isknown_zero() const {
		return mask.zero;
	}
	bool isknown_sign() const {
		return mask.sign;
	}
	bool isknown_overflow() const {
		return mask.overflow;
	}

	bool isknown_carry_set() const {
		return (mask.carry && bits.carry);
	}
	bool isknown_zero_set() const {
		return (mask.zero && bits.zero);
	}
	bool isknown_sign_set() const {
		return (mask.sign && bits.sign);
	}
	bool isknown_overflow_set() const {
		return (mask.overflow && bits.overflow);
	}

	bool isknown_carry_clear() const {
		return (mask.carry && !bits.carry);
	}
	bool isknown_zero_clear() const {
		return (mask.zero && !bits.zero);
	}
	bool isknown_sign_clear() const {
		return (mask.sign && !bits.sign);
	}
	bool isknown_overflow_clear() const {
		return (mask.overflow && !bits.overflow);
	}

	/* inverters */

	flag_state invert_flag(flag_state f) {
		using enum flag_state;
		if (f == unknown) {
			return unknown;
		}
		if (f == known_true) {
			return known_false;
		}
		return known_true;
	}

	void invert_carry() {
		set_carry(invert_flag(get_carry()));
	}
	void invert_zero() {
		set_zero(invert_flag(get_zero()));
	}
	void invert_sign() {
		set_sign(invert_flag(get_sign()));
	}
	void invert_overflow() {
		set_overflow(invert_flag(get_overflow()));
	}

};

class current_state {
	public:

	ez80_instruction previous_instruction;

	flag_pair F;
	reg8_pair A;
	reg8_pair C;
	reg8_pair B;
	reg8_pair UBC;
	reg8_pair E;
	reg8_pair D;
	reg8_pair UDE;
	reg8_pair L;
	reg8_pair H;
	reg8_pair UHL;
	reg8_pair IXL;
	reg8_pair IXH;
	reg8_pair UIX;
	reg8_pair IYL;
	reg8_pair IYH;
	reg8_pair UIY;

	static constexpr size_t stack_size = 4;
	reg24_pair STACK[stack_size];

	void set_pointers_invalid() {
		for (size_t i = 0; i < stack_size; i++) {
			STACK[i].set_unknown();
		}
	}

	void set_previous_instructions_invalid() {
		using enum ez80_op_code;
		previous_instruction.op_code = UNKNOWN;
	}

	void set_just_cxx_reg_unknown() {
		A.set_unknown();
		C.set_unknown();
		B.set_unknown();
		UBC.set_unknown();
		E.set_unknown();
		D.set_unknown();
		UDE.set_unknown();
		L.set_unknown();
		H.set_unknown();
		UHL.set_unknown();
		IYL.set_unknown();
		IYH.set_unknown();
		UIY.set_unknown();
		set_pointers_invalid();
		F.set_flags_unknown();
	}

	void set_just_reg_unknown() {
		A.set_unknown();
		C.set_unknown();
		B.set_unknown();
		UBC.set_unknown();
		E.set_unknown();
		D.set_unknown();
		UDE.set_unknown();
		L.set_unknown();
		H.set_unknown();
		UHL.set_unknown();
		IXL.set_unknown();
		IXH.set_unknown();
		UIX.set_unknown();
		IYL.set_unknown();
		IYH.set_unknown();
		UIY.set_unknown();
	}

	void set_all_reg_unknown() {
		set_pointers_invalid();
		set_just_reg_unknown();
		F.set_flags_unknown();
	}

	void set_addition_overflow_flags(
		flag_state result, flag_state x, flag_state y
	) {
		// signed overflow occurs when (pos + pos = neg) or (neg + neg = pos)
		if (isknown(x) && isknown(y)) {
			if (x != y) {
				// signed overflow will never occur
				F.set_overflow(false);
				return;
			}
		}
		if (isknown(result)) {
			if (isknown(x) && (x != result)) {
				// overflow when (pos + pos = neg) or (neg + neg = pos)
				F.set_overflow(true);
				return;
			}
			if (isknown(y) && (y != result)) {
				// overflow when (pos + pos = neg) or (neg + neg = pos)
				F.set_overflow(true);
				return;
			}
		}
		F.set_overflow_unknown();
	}

	void set_subtraction_overflow_flags(
		flag_state result, flag_state x, flag_state y
	) {
		// signed overflow occurs when (pos - neg = neg) or (neg - pos = pos)
		set_addition_overflow_flags(result, x, cpl_flag_state(y));
	}

	reg24_pair reg24_add(reg24_pair dst, reg24_pair src) {
		using enum flag_state;
		flag_state carry = known_false;
		dst = add_with_carry(dst, src, carry);
		F.set_carry(carry);
		return dst;
	}
	reg16_pair reg16_add(reg16_pair dst, reg16_pair src) {
		using enum flag_state;
		flag_state carry = known_false;
		dst = add_with_carry(dst, src, carry);
		F.set_carry(carry);
		return dst;
	}
	reg24_pair reg24_adc(reg24_pair dst, reg24_pair src) {
		using enum flag_state;
		flag_state dst_sign = dst.test_signbit();
		flag_state src_sign = src.test_signbit();
		flag_state carry = F.get_carry();
		dst = add_with_carry(dst, src, carry);
		F.set_carry(carry);
		F.set_zero(dst.is_zero());
		flag_state result_sign = dst.test_signbit();
		F.set_sign(result_sign);
		set_addition_overflow_flags(result_sign, dst_sign, src_sign);
		return dst;
	}
	reg16_pair reg16_adc(reg16_pair dst, reg16_pair src) {
		using enum flag_state;
		flag_state dst_sign = dst.test_signbit();
		flag_state src_sign = src.test_signbit();
		flag_state carry = F.get_carry();
		dst = add_with_carry(dst, src, carry);
		F.set_carry(carry);
		F.set_zero(dst.is_zero());
		flag_state result_sign = dst.test_signbit();
		F.set_sign(result_sign);
		set_addition_overflow_flags(result_sign, dst_sign, src_sign);
		return dst;
	}
	reg24_pair reg24_sbc(reg24_pair dst, reg24_pair src) {
		using enum flag_state;
		if (src.isknown_zero() && F.isknown_carry_clear()) {
			F.set_zero(dst.is_zero());
			F.set_sign(dst.test_signbit());
			F.set_overflow(false);
			return dst;
		}
		flag_state dst_sign = dst.test_signbit();
		flag_state src_sign = src.test_signbit();
		flag_state carry = F.get_carry();
		dst = subtract_with_carry(dst, src, carry);
		F.set_carry(carry);
		F.set_zero(dst.is_zero());
		flag_state result_sign = dst.test_signbit();
		F.set_sign(result_sign);
		set_subtraction_overflow_flags(result_sign, dst_sign, src_sign);
		return dst;
	}
	reg16_pair reg16_sbc(reg16_pair dst, reg16_pair src) {
		using enum flag_state;
		if (src.isknown_zero() && F.isknown_carry_clear()) {
			F.set_zero(dst.is_zero());
			F.set_sign(dst.test_signbit());
			F.set_overflow(false);
			return dst;
		}
		flag_state dst_sign = dst.test_signbit();
		flag_state src_sign = src.test_signbit();
		flag_state carry = F.get_carry();
		dst = subtract_with_carry(dst, src, carry);
		F.set_carry(carry);
		F.set_zero(dst.is_zero());
		flag_state result_sign = dst.test_signbit();
		F.set_sign(result_sign);
		set_subtraction_overflow_flags(result_sign, dst_sign, src_sign);
		return dst;
	}
	void acc_add(reg8_pair src) {
		reg8_pair& dst = A;
		using enum flag_state;
		flag_state dst_sign = dst.test_signbit();
		flag_state src_sign = src.test_signbit();
		flag_state carry = known_false;
		dst = add_with_carry(dst, src, carry);
		F.set_carry(carry);
		F.set_zero(dst.is_zero());
		flag_state result_sign = dst.test_signbit();
		F.set_sign(result_sign);
		set_addition_overflow_flags(result_sign, dst_sign, src_sign);
	}
	void acc_adc(reg8_pair src) {
		reg8_pair& dst = A;
		using enum flag_state;
		flag_state dst_sign = dst.test_signbit();
		flag_state src_sign = src.test_signbit();
		flag_state carry = F.get_carry();
		dst = add_with_carry(dst, src, carry);
		F.set_carry(carry);
		F.set_zero(dst.is_zero());
		flag_state result_sign = dst.test_signbit();
		F.set_sign(result_sign);
		set_addition_overflow_flags(result_sign, dst_sign, src_sign);
	}
	void acc_sbc(reg8_pair src) {
		reg8_pair& dst = A;
		using enum flag_state;
		flag_state dst_sign = dst.test_signbit();
		flag_state src_sign = src.test_signbit();
		flag_state carry = F.get_carry();
		dst = subtract_with_carry(dst, src, carry);
		F.set_carry(carry);
		F.set_zero(dst.is_zero());
		flag_state result_sign = dst.test_signbit();
		F.set_sign(result_sign);
		set_subtraction_overflow_flags(result_sign, dst_sign, src_sign);
	}
	void acc_sub(reg8_pair src) {
		reg8_pair& dst = A;
		using enum flag_state;
		flag_state dst_sign = dst.test_signbit();
		flag_state src_sign = src.test_signbit();
		flag_state carry = known_false;
		dst = subtract_with_carry(dst, src, carry);
		F.set_carry(carry);
		F.set_zero(dst.is_zero());
		flag_state result_sign = dst.test_signbit();
		F.set_sign(result_sign);
		set_subtraction_overflow_flags(result_sign, dst_sign, src_sign);
	}
	void acc_cp(reg8_pair src) {
		reg8_pair dst = A;
		using enum flag_state;
		flag_state dst_sign = dst.test_signbit();
		flag_state src_sign = src.test_signbit();
		flag_state carry = known_false;
		dst = subtract_with_carry(dst, src, carry);
		F.set_carry(carry);
		F.set_zero(dst.is_zero());
		flag_state result_sign = dst.test_signbit();
		F.set_sign(result_sign);
		set_subtraction_overflow_flags(result_sign, dst_sign, src_sign);
	}
	void acc_xor(reg8_pair src) {
		reg8_pair& dst = A;
		F.set_carry(false);
		dst = dst ^ src;
		F.set_zero(dst.is_zero());
		F.set_sign(dst.test_signbit());
		F.set_overflow(dst.is_parity_even());
	}
	void acc_or(reg8_pair src) {
		reg8_pair& dst = A;
		F.set_carry(false);
		dst = dst | src;
		F.set_zero(dst.is_zero());
		F.set_sign(dst.test_signbit());
		F.set_overflow(dst.is_parity_even());
	}
	void acc_and(reg8_pair src) {
		reg8_pair& dst = A;
		F.set_carry(false);
		dst = dst & src;
		F.set_zero(dst.is_zero());
		F.set_sign(dst.test_signbit());
		F.set_overflow(dst.is_parity_even());
	}
	void acc_tst(reg8_pair src) {
		reg8_pair dst = A;
		F.set_carry(false);
		dst = dst & src;
		F.set_zero(dst.is_zero());
		F.set_sign(dst.test_signbit());
		F.set_overflow(dst.is_parity_even());
	}
	void acc_or_was_zero(reg8_pair& arg) {
		F.set_sign(false);
		F.set_overflow(true);
		A.set_value(0);
		arg.set_value(0);
	}
	void acc_xor_was_zero(reg8_pair& arg) {
		F.set_sign(false);
		F.set_overflow(true);
		A.set_value(0);
	}
	void acc_and_was_zero(reg8_pair& arg) {
		F.set_sign(false);
		F.set_overflow(true);
		A.set_value(0);
	}
	void acc_tst_was_zero(reg8_pair& arg) {
		F.set_sign(false);
		F.set_overflow(true);
		merge_tst_was_zero_bits(A, arg);
	}
	reg24_pair adc24_was_zero(reg24_pair arg) {
		set_HL(0);
		F.set_sign(false);
		if (F.isknown_carry_clear()) {
			// carry is only cleared when doing 0 += 0
			arg.set_value(0);
			F.set_overflow(false);
			return arg;
		}
		if (arg.isknown_nonzero()) {
			// carry is always set when arg is non-zero
			F.set_carry(true);
		}
		return arg;
	}
	reg24_pair adc16_was_zero(reg24_pair arg) {
		set_HL(0);
		F.set_sign(false);
		if (F.isknown_carry_clear()) {
			// carry is only cleared when doing 0 += 0
			arg.bits &= 0xFF0000;
			arg.mask |= 0x00FFFF;
			F.set_overflow(false);
			return arg;
		}
		if (arg.isknown_nonzero()) {
			// carry is always set when arg is non-zero
			F.set_carry(true);
		}
		return arg;
	}
	void acc_add_was_zero(reg8_pair& arg) {
		A.set_value(0);
		F.set_sign(false);
		if (F.isknown_carry_clear() || arg.isknown_zero()) {
			// carry is only cleared when doing 0 += 0
			arg.set_value(0);
			F.set_overflow(false);
			F.set_carry(false);
			return;
		}
		if (arg.isknown_nonzero()) {
			// carry is always set when arg is non-zero
			F.set_carry(true);
		}
	}
	void acc_adc_was_zero(reg8_pair& arg) {
		A.set_value(0);
		F.set_sign(false);
		if (F.isknown_carry_clear()) {
			// carry is only cleared when doing 0 += 0
			arg.set_value(0);
			F.set_overflow(false);
			return;
		}
		if (arg.isknown_nonzero()) {
			// carry is always set when arg is non-zero
			F.set_carry(true);
		}
		return;
	}
	void acc_sbc_was_zero(reg8_pair arg) {
		A.set_value(0);
		F.set_sign(false);
		F.set_overflow(false);
		F.set_carry(false);
	}
	void acc_sub_was_zero(reg8_pair arg) {
		A.set_value(0);
		F.set_sign(false);
		F.set_overflow(false);
		F.set_carry(false);
	}
	void acc_cp_was_zero(reg8_pair& arg) {
		F.set_sign(false);
		F.set_overflow(false);
		F.set_carry(false);
		merge_cp_was_zero_bits(A, arg);
	}
	void sbc24_was_zero() {
		set_HL(0);
		F.set_sign(false);
		F.set_overflow(false);
		F.set_carry(false);
	}
	void sbc16_was_zero() {
		set_HL(0);
		F.set_sign(false);
		F.set_overflow(false);
		F.set_carry(false);
	}

	void acc_cpl() {
		A.complement();
	}
	void acc_neg() {
		F.set_carry(A.is_nonzero());
		A = -A;
		F.set_zero(A.is_zero());
		F.set_sign(A.test_signbit());
		F.set_overflow(A.is_equal(0x80));
		if (A.isknown_zero()) {
			A.set_unknown();
		}
	}
	void acc_rla() {
		flag_state new_carry = A.test_signbit();
		flag_state old_carry = F.get_carry();
		A.bits <<= 1;
		A.mask <<= 1;
		A.bit_copy(0, old_carry);
		F.set_carry(new_carry);
	}
	void acc_rlca() {
		flag_state new_carry = A.test_signbit();
		A.bits <<= 1;
		A.mask <<= 1;
		A.bit_copy(0, new_carry);
		F.set_carry(new_carry);
	}
	void acc_rra() {
		flag_state new_carry = A.bit_test(0);
		flag_state old_carry = F.get_carry();
		A.bits >>= 1;
		A.mask >>= 1;
		A.bit_copy(7, old_carry);
		F.set_carry(new_carry);
	}
	void acc_rrca() {
		flag_state new_carry = A.bit_test(0);
		A.bits >>= 1;
		A.mask >>= 1;
		A.bit_copy(7, new_carry);
		F.set_carry(new_carry);
	}
	void reg8_shift_set_flags(reg8_pair src) {
		F.set_zero(src.is_zero());
		F.set_sign(src.test_signbit());
		F.set_overflow(src.is_parity_even());
	}
	void reg8_shift_to_zero(reg8_pair& arg) {
		arg.set_value(0);
		F.set_sign(false);
		F.set_overflow(true);
	}
	void reg_rl(reg8_pair dst) {
		flag_state new_carry = dst.test_signbit();
		flag_state old_carry = F.get_carry();
		dst.shift_left_logical(1);
		dst.bit_copy(0, old_carry);
		F.set_carry(new_carry);
		F.set_zero(dst.is_zero());
		F.set_sign(dst.test_signbit());
		F.set_overflow(dst.is_parity_even());
	}
	void reg_rlc(reg8_pair dst) {
		flag_state new_carry = dst.test_signbit();
		dst.shift_left_logical(1);
		dst.bit_copy(0, new_carry);
		F.set_carry(new_carry);
		F.set_zero(dst.is_zero());
		F.set_sign(dst.test_signbit());
		F.set_overflow(dst.is_parity_even());
	}
	void reg_sla(reg8_pair dst) {
		F.set_carry(dst.test_signbit());
		dst.shift_left_logical(1);
		dst.bit_clear(0);
		F.set_zero(dst.is_zero());
		F.set_sign(dst.test_signbit());
		F.set_overflow(dst.is_parity_even());
	}
	void reg_rr(reg8_pair dst) {
		flag_state new_carry = dst.bit_test(0);
		flag_state old_carry = F.get_carry();
		dst.shift_right_logical(1);
		dst.bit_copy(7, old_carry);
		F.set_carry(new_carry);
		F.set_zero(dst.is_zero());
		F.set_sign(dst.test_signbit());
		F.set_overflow(dst.is_parity_even());
	}
	void reg_rrc(reg8_pair dst) {
		flag_state new_carry = dst.bit_test(0);
		dst.shift_right_logical(1);
		dst.bit_copy(7, new_carry);
		F.set_carry(new_carry);
		F.set_zero(dst.is_zero());
		F.set_sign(dst.test_signbit());
		F.set_overflow(dst.is_parity_even());
	}
	void reg_srl(reg8_pair dst) {
		F.set_carry(dst.bit_test(0));
		dst.shift_right_logical(1);
		dst.bit_clear(7);
		F.set_zero(dst.is_zero());
		F.set_sign(dst.test_signbit());
		F.set_overflow(dst.is_parity_even());
	}
	void reg_sra(reg8_pair dst) {
		F.set_carry(dst.bit_test(0));
		dst.shift_right_arithmetic(1);
		F.set_zero(dst.is_zero());
		F.set_sign(dst.test_signbit());
		F.set_overflow(dst.is_parity_even());
	}
	void add_a_a() {
		using enum flag_state;
		flag_state old_sign = A.test_signbit();
		flag_state new_carry = A.test_signbit();
		F.set_carry(old_sign);
		A.shift_left_logical(1);
		A.bit_clear(0);
		flag_state new_sign = A.test_signbit();
		F.set_carry(new_carry);
		F.set_zero(A.is_zero());
		F.set_sign(new_sign);
		if (old_sign == unknown || new_sign == unknown) {
			F.set_overflow_unknown();
			return;
		}
		F.set_overflow(old_sign != new_sign);
	}
	void adc_a_a() {
		using enum flag_state;
		flag_state old_carry = F.get_carry();
		flag_state old_sign = A.test_signbit();
		flag_state new_carry = A.test_signbit();
		F.set_carry(old_sign);
		A.shift_left_logical(1);
		A.bit_copy(0, old_carry);
		flag_state new_sign = A.test_signbit();
		F.set_carry(new_carry);
		F.set_zero(A.is_zero());
		F.set_sign(new_sign);
		if (old_sign == unknown || new_sign == unknown) {
			F.set_overflow_unknown();
			return;
		}
		F.set_overflow(old_sign != new_sign);
	}
	void sbc_a_a() {
		F.set_overflow(false);
		if (F.isknown_carry_set()) {
			A.set_value(0xFF);
			F.set_zero(false);
			F.set_sign(true);
			return;
		}
		if (F.isknown_carry_clear()) {
			A.set_value(0x00);
			F.set_zero(true);
			F.set_sign(false);
			return;
		}
		F.set_zero_unknown();
		F.set_sign_unknown();
		A.set_unknown();
	}
	reg24_pair sbc24_hl_hl(reg24_pair dst) {
		F.set_overflow(false);
		if (F.isknown_carry_set()) {
			dst.set_value(0xFFFFFF);
			F.set_zero(false);
			F.set_sign(true);
			return dst;
		}
		if (F.isknown_carry_clear()) {
			dst.set_value(0x000000);
			F.set_zero(true);
			F.set_sign(false);
			return dst;
		}
		F.set_zero_unknown();
		F.set_sign_unknown();
		dst.set_unknown();
		return dst;
	}
	reg16_pair sbc16_hl_hl(reg16_pair dst) {
		F.set_overflow(false);
		if (F.isknown_carry_set()) {
			dst.set_value(0xFFFF);
			F.set_zero(false);
			F.set_sign(true);
			return dst;
		}
		if (F.isknown_carry_clear()) {
			dst.set_value(0x0000);
			F.set_zero(true);
			F.set_sign(false);
			return dst;
		}
		F.set_zero_unknown();
		F.set_sign_unknown();
		dst.set_unknown();
		return dst;
	}
	void sub_a_a() {
		A.set_value(0x00);
		F.set_carry(false);
		F.set_zero(true);
		F.set_sign(false);
		F.set_overflow(false);
		return;
	}
	void cp_a_a() {
		F.set_carry(false);
		F.set_zero(true);
		F.set_sign(false);
		F.set_overflow(false);
		return;
	}
	void xor_a_a() {
		A.set_value(0x00);
		F.set_carry(false);
		F.set_zero(true);
		F.set_sign(false);
		F.set_overflow(true);
		return;
	}
	void and_a_a() {
		F.set_carry(false);
		F.set_zero(A.is_zero());
		F.set_sign(A.test_signbit());
		F.set_overflow(A.is_parity_even());
	}
	void or_a_a() {
		F.set_carry(false);
		F.set_zero(A.is_zero());
		F.set_sign(A.test_signbit());
		F.set_overflow(A.is_parity_even());
	}
	void tst_a_a() {
		F.set_carry(false);
		F.set_zero(A.is_zero());
		F.set_sign(A.test_signbit());
		F.set_overflow(A.is_parity_even());
	}
	reg24_pair add24_hl_hl(reg24_pair dst) {
		F.set_carry(dst.test_signbit());
		dst.shift_left_logical(1);
		dst.bit_clear(0);
		return dst;
	}
	reg16_pair add16_hl_hl(reg16_pair dst) {
		F.set_carry(dst.test_signbit());
		dst.shift_left_logical(1);
		dst.bit_clear(0);
		return dst;
	}
	reg24_pair adc24_hl_hl(reg24_pair dst) {
		using enum flag_state;
		flag_state old_carry = F.get_carry();
		flag_state new_carry = dst.test_signbit();
		flag_state old_sign = dst.test_signbit();
		dst.shift_left_logical(1);
		dst.bit_copy(0, old_carry);
		F.set_carry(new_carry);
		F.set_zero(dst.is_zero());
		flag_state new_sign = dst.test_signbit();
		F.set_sign(new_sign);
		if (new_sign == unknown || old_sign == unknown) {
			F.set_overflow_unknown();
			return dst;
		}
		F.set_overflow(new_sign != old_sign);
		return dst;
	}
	reg16_pair adc16_hl_hl(reg16_pair dst) {
		using enum flag_state;
		flag_state old_carry = F.get_carry();
		flag_state new_carry = dst.test_signbit();
		flag_state old_sign = dst.test_signbit();
		dst.shift_left_logical(1);
		dst.bit_copy(0, old_carry);
		F.set_carry(new_carry);
		F.set_zero(dst.is_zero());
		flag_state new_sign = dst.test_signbit();
		F.set_sign(new_sign);
		if (new_sign == unknown || old_sign == unknown) {
			F.set_overflow_unknown();
			return dst;
		}
		F.set_overflow(new_sign != old_sign);
		return dst;
	}
	reg16_pair reg_mlt(reg8_pair hi, reg8_pair lo) {
		reg16_pair x, y;
		x.set_zero_extend(hi);
		y.set_zero_extend(lo);
		x = x * y;
		return x;
	}
	reg24_pair reg24_lea(reg24_pair dst, reg24_pair src, reg8_pair offset) {
		reg24_pair extend;
		extend.set_sign_extend(offset);
		dst = (src + extend);
		return dst;
	}
	reg16_pair reg16_lea(reg16_pair dst, reg16_pair src, reg8_pair offset) {
		reg16_pair extend;
		extend.set_sign_extend(offset);
		dst = (src + extend);
		return dst;
	}
	void reg_bit_test(reg8_pair src, int b) {
		F.set_zero(src.bit_test(b));
		F.set_sign_unknown();
		F.set_overflow_unknown();
	}
	void reg8_inc_dec_to_zero(reg8_pair& arg) {
		arg.set_value(0);
		F.set_sign(false);
		F.set_overflow(false);
	}
	void reg8_bitwise_flag_set(reg8_pair arg) {
		F.set_overflow(arg.is_parity_even());
		F.set_zero(arg.is_zero());
		F.set_sign(arg.test_signbit());
	}
	void reg8_inc_flag_set(reg8_pair arg) {
		F.set_overflow(arg.is_equal(0x80));
		F.set_zero(arg.is_zero());
		F.set_sign(arg.test_signbit());
	}
	void reg8_dec_flag_set(reg8_pair arg) {
		F.set_overflow(arg.is_equal(0x7F));
		F.set_zero(arg.is_zero());
		F.set_sign(arg.test_signbit());
	}
	void reg8_inc(reg8_pair dst) {
		F.set_overflow(dst.is_equal(0x7F));
		dst.increment();
		F.set_zero(dst.is_zero());
		F.set_sign(dst.test_signbit());
	}
	void reg8_dec(reg8_pair dst) {
		F.set_overflow(dst.is_equal(0x80));
		dst.decrement();
		F.set_zero(dst.is_zero());
		F.set_sign(dst.test_signbit());
	}
	void push_stack(reg24_pair src) {
		for (size_t i = 1; i < stack_size; i++) {
			STACK[i] = STACK[i - 1];
		}
		STACK[0] = src;
	}
	reg24_pair pop_stack() {
		reg24_pair ret = STACK[0];
		for (size_t i = 1; i < stack_size; i++) {
			STACK[i - 1] = STACK[i];
		}
		STACK[stack_size].set_unknown();
		return ret;
	}
	void pea_stack(reg24_pair src, reg8_pair offset) {
		reg24_pair dst;
		reg24_pair extend;
		extend.set_sign_extend(offset);
		dst = (src + extend);
		push_stack(dst);
	}
	reg24_pair ex_stack(reg24_pair arg) {
		reg24_pair ret = STACK[0];
		STACK[0] = arg;
		return ret;
	}
/* getters */

	reg24_pair get_AF() const {
		reg8_pair upper;
		reg8_pair flags;
		reg24_pair ret;
		upper.set_unknown();
		F.export_flag_pair(flags);
		ret.set_value(upper, A, flags);
		return ret;
	}
	reg24_pair get_HL() const {
		reg24_pair ret;
		ret.set_value(UHL, H, L);
		return ret;
	}
	reg24_pair get_DE() const {
		reg24_pair ret;
		ret.set_value(UDE, D, E);
		return ret;
	}
	reg24_pair get_BC() const {
		reg24_pair ret;
		ret.set_value(UBC, B, C);
		return ret;
	}
	reg24_pair get_IX() const {
		reg24_pair ret;
		ret.set_value(UIX, IXH, IXL);
		return ret;
	}
	reg24_pair get_IY() const {
		reg24_pair ret;
		ret.set_value(UIY, IYH, IYL);
		return ret;
	}
	reg24_pair get_SP() const {
		/* SP is unimplemented currently */
		reg24_pair ret;
		ret.set_unknown();
		return ret;
	}

/* CRT getters */
	private:
	reg16_pair get16_HL() const {
		reg16_pair ret;
		ret.set_value(H, L);
		return ret;
	}
	reg16_pair get16_DE() const {
		reg16_pair ret;
		ret.set_value(D, E);
		return ret;
	}
	reg16_pair get16_BC() const {
		reg16_pair ret;
		ret.set_value(B, C);
		return ret;
	}
	reg16_pair get16_IX() const {
		reg16_pair ret;
		ret.set_value(IXH, IXL);
		return ret;
	}
	reg16_pair get16_IY() const {
		reg16_pair ret;
		ret.set_value(IYH, IYL);
		return ret;
	}
	reg16_pair get16_SP() const {
		/* SP is unimplemented currently */
		reg16_pair ret;
		ret.set_unknown();
		return ret;
	}

	public:
	reg32_pair get32_EUHL() const {
		reg32_pair ret;
		ret.set_value(E, UHL, H, L);
		return ret;
	}

	reg32_pair get32_AUBC() const {
		reg32_pair ret;
		ret.set_value(A, UBC, B, C);
		return ret;
	}

/* setters */

	void set_AF(reg24_pair val) {
		reg8_pair flags;
		val.split_value(A, flags);
		F.import_flag_pair(flags);
	}

	void set_HL(reg24_pair val) {
		val.split_value(UHL, H, L);
	}
	void set_DE(reg24_pair val) {
		val.split_value(UDE, D, E);
	}
	void set_BC(reg24_pair val) {
		val.split_value(UBC, B, C);
	}
	void set_IX(reg24_pair val) {
		val.split_value(UIX, IXH, IXL);
	}
	void set_IY(reg24_pair val) {
		val.split_value(UIY, IYH, IYL);
	}
	void set_SP(__attribute__((unused)) reg24_pair val) {
		set_pointers_invalid();
		/* SP is unimplemented currently */
		return;
	}

	void set_HL(uint24_t val) {
		reg24_pair dst;
		dst.set_value(val);
		set_HL(dst);
	}
	void set_DE(uint24_t val) {
		reg24_pair dst;
		dst.set_value(val);
		set_DE(dst);
	}
	void set_BC(uint24_t val) {
		reg24_pair dst;
		dst.set_value(val);
		set_BC(dst);
	}
	void set_IX(uint24_t val) {
		reg24_pair dst;
		dst.set_value(val);
		set_IX(dst);
	}
	void set_IY(uint24_t val) {
		reg24_pair dst;
		dst.set_value(val);
		set_IY(dst);
	}
	void set_SP(uint24_t val) {
		set_pointers_invalid();
		reg24_pair dst;
		dst.set_value(val);
		set_SP(dst);
	}

/* CRT setters */
	private:
	void set16_zero_HL(reg16_pair val) {
		val.split_value(UHL, H, L);
	}
	void set16_zero_DE(reg16_pair val) {
		val.split_value(UDE, D, E);
	}
	void set16_zero_BC(reg16_pair val) {
		val.split_value(UBC, B, C);
	}
	void set16_zero_IX(reg16_pair val) {
		val.split_value(UIX, IXH, IXL);
	}
	void set16_zero_IY(reg16_pair val) {
		val.split_value(UIY, IYH, IYL);
	}
	void set16_zero_SP(__attribute__((unused)) reg16_pair val) {
		set_pointers_invalid();
		/* SP is unimplemented currently */
		return;
	}

	void set16_partial_HL(reg16_pair val) {
		val.split_value(H, L);
		UHL.set_unknown();
	}
	void set16_partial_DE(reg16_pair val) {
		val.split_value(D, E);
		UDE.set_unknown();
	}
	void set16_partial_BC(reg16_pair val) {
		val.split_value(B, C);
		UBC.set_unknown();
	}
	void set16_partial_IX(reg16_pair val) {
		val.split_value(IXH, IXL);
		UIX.set_unknown();
	}
	void set16_partial_IY(reg16_pair val) {
		val.split_value(IYH, IYL);
		UIY.set_unknown();
	}
	void set16_partial_SP(__attribute__((unused)) reg16_pair val) {
		set_pointers_invalid();
		/* SP is unimplemented currently */
		return;
	}

	void set16_preserve_HL(reg16_pair val) {
		val.split_value(H, L);
	}
	void set16_preserve_DE(reg16_pair val) {
		val.split_value(D, E);
	}
	void set16_preserve_BC(reg16_pair val) {
		val.split_value(B, C);
	}
	void set16_preserve_IX(reg16_pair val) {
		val.split_value(IXH, IXL);
	}
	void set16_preserve_IY(reg16_pair val) {
		val.split_value(IYH, IYL);
	}
	void set16_preserve_SP(__attribute__((unused)) reg16_pair val) {
		set_pointers_invalid();
		/* SP is unimplemented currently */
		return;
	}
	public:

	void set32_EUHL(reg32_pair val) {
		E.set_value(val.get_hi8());
		set_HL(val.get_lo24());
	}

	void set32_AUBC(reg32_pair val) {
		A.set_value(val.get_hi8());
		set_BC(val.get_lo24());
	}

/* set unknown */
	void HL_set_unknown() {
		UHL.set_unknown();
		H.set_unknown();
		L.set_unknown();
	}
	void DE_set_unknown() {
		UDE.set_unknown();
		D.set_unknown();
		E.set_unknown();
	}
	void BC_set_unknown() {
		UBC.set_unknown();
		B.set_unknown();
		C.set_unknown();
	}
	void IX_set_unknown() {
		UIX.set_unknown();
		IXH.set_unknown();
		IXL.set_unknown();
	}
	void IY_set_unknown() {
		UIY.set_unknown();
		IYH.set_unknown();
		IYL.set_unknown();
	}
	void SP_set_unknown() {
		set_pointers_invalid();
		/* SP is unimplemented currently */
		return;
	}

	void next_instruction(ez80_instruction instruction);

	void next_known_func(ez80_known_function func);

	void check_previous_instruction(ez80_instruction current_instruction);

	string print_flag(flag_state f, string when_known_true) const {
		using enum flag_state;
		if (f == known_true) {
			return when_known_true;
		}
		if (f == known_false) {
			return "-";
		}
		return "?";
	}

	string print_reg8(reg8_pair dst) const {
		const char table[16] = {
			'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
		};
		char buf[3] = "--"; 
		uint8_t hi_mask = (dst.mask & 0xF0) >> 4;
		uint8_t lo_mask = (dst.mask & 0x0F);
		uint8_t hi_bits = (dst.bits & 0xF0) >> 4;
		uint8_t lo_bits = (dst.bits & 0x0F);
		if (hi_mask == 0xF) {
			buf[0] = table[hi_bits];
		} else if (hi_mask != 0x0) {
			buf[0] = '*';
		}
		if (lo_mask == 0xF) {
			buf[1] = table[lo_bits];
		} else if (lo_mask != 0x0) {
			buf[1] = '*';
		}
		return string(buf);
	}

	string print_reg24(reg8_pair up, reg8_pair hi, reg8_pair lo) const {
		return print_reg8(up) + print_reg8(hi) + print_reg8(lo);
	}
	string print_reg24(reg24_pair x) const {
		reg8_pair up, hi, lo;
		x.split_value(up, hi, lo);
		return print_reg24(up, hi, lo);
	}

	string print_state() const {
		string output = (
			"S1 " + print_reg24(STACK[1]) +
			" | S0 " + print_reg24(STACK[0]) +
			" | IX " + print_reg24(UIX, IXH, IXL) +
			" | IY " + print_reg24(UIY, IYH, IYL) +
			" | A " + print_reg8(A) +
			" | BC " + print_reg24(UBC, B, C) + 
			" | DE " + print_reg24(UDE, D, E) +
			" | HL " + print_reg24(UHL, H, L) + 
			" | " +
			print_flag(F.get_sign(), "S") + 
			print_flag(F.get_zero(), "Z") +
			print_flag(F.get_overflow(), "V") +
			print_flag(F.get_carry(), "C")
		);
		return output;
	}
};

#endif /* CURRENT_STATE_HPP */
