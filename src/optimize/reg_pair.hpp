#ifndef REG_PAIR_HPP
#define REG_PAIR_HPP

#include "../ez80_type.h"
#include "../common_std.h"

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
	T get_bit_mask(int b) const {
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

	void set_to_zero() {
		set_value(0);
	}

	void set_to_all_ones() {
		set_value(get_all_ones<T>());
	}

	/* set to flag */

	void set_to_zero_or_one(flag_state f) {
		using enum flag_state;
		set_to_zero();
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
			set_to_zero();
			return;
		}
		if (f == known_true) {
			set_to_all_ones();
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
		const T index = get_bit_mask(b);
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
		const T index = get_bit_mask(b);
		mask |= index; // bit is known
		bits |= index; // set bit
	}
	void bit_clear(int b) {
		const T index = get_bit_mask(b);
		mask |= index; // bit is known
		bits &= ~index; // clear bit
	}
	void bit_flip(int b) {
		const T index = get_bit_mask(b);
		bits ^= index; // flip bit
		bits &= mask; // canonicalize
	}
	void bit_unknown(int b) {
		const T index = get_bit_mask(b);
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
	void bit_copy(int b, bool f) {
		bit_copy(b, set_flag_state(f));
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

	T get_unknown_bits_as_zeros() const {
		T ret = (bits & mask);
		return ret;
	}

	T get_unknown_bits_as_ones() const {
		T ret = (bits & mask);
		ret |= (~mask);
		return ret;
	}

	/* signbit */

	constexpr int get_signbit_index() const {
		return (bit_width_of_type<T>() - 1);
	}

	flag_state test_signbit() const {
		return bit_test(get_signbit_index());
	}

	/* set to range */

	void set_to_unsigned_range(T min_bound, T max_bound) {
		if (min_bound == max_bound) {
			set_value(min_bound);
			return;
		}
		if (min_bound > max_bound) {
			std::swap(min_bound, max_bound);
		}
		set_unknown();
		for (size_t i = bit_width_of_type<T>(); i --> 0;) {
			bool x = (min_bound & (static_cast<T>(1) << i));
			bool y = (max_bound & (static_cast<T>(1) << i));
			if (x != y) {
				break;
			}
			bit_copy(i, x);
		}
	}

//------------------------------------------------------------------------------
// Merge bits
//------------------------------------------------------------------------------

/** 
 * @brief assumes that no known bits conflict
 */
void merge_bits(reg_pair<T> y) {
	using enum flag_state;
	for (size_t i = 0; i < bit_width_of_type<T>(); i++) {
		flag_state X = this->bit_test(i);
		flag_state Y = y.bit_test(i);
		if (!isknown(Y)) {
			continue;
		}
		if (isknown(X)) {
			if (X != Y) {
				printf("Error: merge_bits conflict at bit %zu\n", i);
				bit_unknown(i);
				continue;
			}
			continue;
		}
		this->bit_copy(i, Y);
	}
}

/** 
 * @brief sets bits to unknown on conflict
 */
void merge_bits_favor_unknown(reg_pair<T> y) {
	using enum flag_state;
	for (size_t i = 0; i < bit_width_of_type<T>(); i++) {
		flag_state X = this->bit_test(i);
		flag_state Y = y.bit_test(i);
		if (!isknown(Y)) {
			continue;
		}
		if (isknown(X)) {
			if (X != Y) {
				bit_unknown(i);
				continue;
			}
			continue;
		}
		this->bit_copy(i, Y);
	}
}

/** 
 * @brief preserves bits on conflict
 */
void merge_bits_preserve(reg_pair<T> y) {
	using enum flag_state;
	for (size_t i = 0; i < bit_width_of_type<T>(); i++) {
		flag_state X = this->bit_test(i);
		flag_state Y = y.bit_test(i);
		if (!isknown(Y)) {
			continue;
		}
		if (isknown(X)) {
			continue;
		}
		this->bit_copy(i, Y);
	}
}

/** 
 * @brief overwrites bits on conflict
 */
void merge_bits_overwrite(reg_pair<T> y) {
	using enum flag_state;
	for (size_t i = 0; i < bit_width_of_type<T>(); i++) {
		flag_state X = this->bit_test(i);
		flag_state Y = y.bit_test(i);
		if (!isknown(Y)) {
			continue;
		}
		if (isknown(X)) {
			if (X != Y) {
				this->bit_copy(i, Y);
				continue;
			}
			continue;
		}
		this->bit_copy(i, Y);
	}
}

//------------------------------------------------------------------------------
// Undefined Behaviour
//------------------------------------------------------------------------------

private:

/**
 * @brief Validates and corrects the shift amount. You may implement your own
 * custom behavior here.
 * @return `true` if the calling function shall proceed. `false` if it should abort.
 */
bool shift_logical_in_range(uint8_t& shift) {
	if (shift < bit_width_of_type<T>()) {
		return true;
	}
	#if 1
		// default to unknown
		set_unknown();
		return false;
	#elif 0
		// special case for shift == bit_width
		if (shift == bit_width_of_type<T>()) {
			set_to_zero();
		} else {
			set_unknown();
		}
		return false;
	#else
		// modulo shift amount
		shift %= bit_width_of_type<T>();
		return true;
	#endif
}

/**
 * @brief Validates and corrects the shift amount. You may implement your own
 * custom behavior here.
 * @return `true` if the calling function shall proceed. `false` if it should abort.
 */
bool shift_ones_in_range(uint8_t& shift) {
	if (shift < bit_width_of_type<T>()) {
		return true;
	}
	#if 1
		// default to unknown
		set_unknown();
		return false;
	#elif 0
		// special case for shift == bit_width
		if (shift == bit_width_of_type<T>()) {
			set_to_all_ones();
		} else {
			set_unknown();
		}
		return false;
	#else
		// modulo shift amount
		shift %= bit_width_of_type<T>();
		return true;
	#endif
}

/**
 * @brief Validates and corrects the shift amount. You may implement your own
 * custom behavior here.
 * @return `true` if the calling function shall proceed. `false` if it should abort.
 */
bool shift_unknown_bits_in_range(uint8_t& shift) {
	if (shift < bit_width_of_type<T>()) {
		return true;
	}
	#if 1
		// default to unknown
		set_unknown();
		return false;
	#else
		// modulo shift amount
		shift %= bit_width_of_type<T>();
		return true;
	#endif
}

/**
 * @brief Validates and corrects the shift amount. You may implement your own
 * custom behavior here.
 * @return `true` if the calling function shall proceed. `false` if it should abort.
 */
bool rotate_in_range(uint8_t& shift) {
	if (shift < bit_width_of_type<T>()) {
		return true;
	}
	#if 1
		// default to unknown
		set_unknown();
		return false;
	#else
		// modulo shift amount
		shift %= bit_width_of_type<T>();
		return true;
	#endif
}

/**
 * @brief Validates and corrects the shift amount. You may implement your own
 * custom behavior here.
 * @return `true` if the calling function shall proceed. `false` if it should abort.
 */
bool rotate_with_carry_in_range(uint8_t& shift) {
	// the carry adds an additional bit that will be shifted
	constexpr uint8_t bits_that_can_be_shifted = (bit_width_of_type<T>() + 1);
	if (shift < bits_that_can_be_shifted) {
		return true;
	}
	#if 1
		// default to unknown
		set_unknown();
		return false;
	#else
		// modulo shift amount
		shift %= bits_that_can_be_shifted;
		return true;
	#endif
}

public:

//------------------------------------------------------------------------------
// Shift Bits
//------------------------------------------------------------------------------

/**
 * @brief Shift left while shifting in zeros
 */
void shift_left_logical(uint8_t shift) {
	if (!shift_logical_in_range(shift)) {
		return;
	}
	bits = (bits << shift);
	mask = ~(~mask << shift);
}

/**
 * @brief Shift right while shifting in zeros
 */
void shift_right_logical(uint8_t shift) {
	if (!shift_logical_in_range(shift)) {
		return;
	}
	bits = (bits >> shift);
	mask = ~(~mask >> shift);
}

/**
 * @brief Shift left while shifting in ones
 */
void shift_left_ones(uint8_t shift) {
	if (!shift_ones_in_range(shift)) {
		return;
	}
	bits = ~(~bits << shift);
	mask = ~(~mask << shift);
}

/**
 * @brief Shift right while shifting in ones
 */
void shift_right_ones(uint8_t shift) {
	if (!shift_ones_in_range(shift)) {
		return;
	}
	bits = ~(~bits >> shift);
	mask = ~(~mask >> shift);
}

/**
 * @brief Shift left while shifting in unknown bits
 */
void shift_left_unknown_bits(uint8_t shift) {
	if (!shift_unknown_bits_in_range(shift)) {
		return;
	}
	bits = ~(~bits << shift);
	mask = (mask << shift);
}

/**
 * @brief Shift right while shifting in unknown bits
 */
void shift_right_unknown_bits(uint8_t shift) {
	if (!shift_unknown_bits_in_range(shift)) {
		return;
	}
	bits = ~(~bits >> shift);
	mask = (mask >> shift);
}

/**
 * @brief Shift right while shifting in signbits
 */
void shift_right_arithmetic(uint8_t shift) {
	using enum flag_state;
	flag_state sign = test_signbit();
	if (sign == known_true) {
		// negative
		shift_right_ones(shift);
		return;
	}
	if (sign == known_false) {
		// positive
		shift_right_logical(shift);
		return;
	}
	shift_right_unknown_bits(shift);
}

//------------------------------------------------------------------------------
// Rotate Bits
//------------------------------------------------------------------------------

/**
 * @brief N bit rotate left
 */
void rotate_left(uint8_t shift) {
	using enum flag_state;
	if (!rotate_in_range(shift)) {
		return;
	}
	for (uint8_t i = 0; i < shift; i++) {
		// MSB becomes LSB
		flag_state bit = bit_test(get_signbit_index());
		shift_left_logical(1);
		bit_copy(0, bit);
	}
}

/**
 * @brief (N + 1) bit rotate left with carry
 */
void rotate_left_with_carry(uint8_t shift, flag_state& carry) {
	if (!rotate_with_carry_in_range(shift)) {
		return;
	}
	for (uint8_t i = 0; i < shift; i++) {
		// carry shifted into LSB
		// MSB shifted out to carry
		flag_state bit = bit_test(get_signbit_index());
		shift_left_logical(1);
		bit_copy(0, carry);
		carry = bit;
	}
}

/**
 * @brief N bit rotate right
 */
void rotate_right(uint8_t shift) {
	using enum flag_state;
	if (!rotate_in_range(shift)) {
		return;
	}
	for (uint8_t i = 0; i < shift; i++) {
		// LSB becomes MSB
		flag_state bit = bit_test(0);
		shift_right_logical(1);
		bit_copy(get_signbit_index(), bit);
	}
}

/**
 * @brief (N + 1) bit rotate right with carry
 */
void rotate_right_with_carry(uint8_t shift, flag_state& carry) {
	if (!rotate_with_carry_in_range(shift)) {
		return;
	}
	for (uint8_t i = 0; i < shift; i++) {
		// carry shifted into MSB
		// LSB shifted out to carry
		flag_state bit = bit_test(0);
		shift_right_logical(1);
		bit_copy(get_signbit_index(), carry);
		carry = bit;
	}
}

//------------------------------------------------------------------------------
// increment/Decrement
//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------
// Binary operations
//------------------------------------------------------------------------------

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
	using enum flag_state;
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
flag_state compare_notequal(reg_pair<T> x, reg_pair<T> y) {
	return cpl_flag_state(compare_equal(x, y));
}

template<typename T>
reg_pair<T> multiply_same_width(reg_pair<T> x, reg_pair<T> y) {
	using enum flag_state;
	reg_pair<T> result;
	result.set_to_zero();
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
	quo.set_to_zero();
	rem.set_to_zero();
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
		reg_pair<T> rem_mask;
		rem_mask.set_to_unsigned_range(0, den.bits - 1);
		rem.merge_bits(rem_mask);
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

//------------------------------------------------------------------------------
// Logical merge known bits
//------------------------------------------------------------------------------

/**
 * @brief Uses the result from a logical AND to resolve unknown bits.
 */
inline void merge_known_bits_via_and(flag_state result, flag_state& x, flag_state& y) {
	using enum flag_state;
	if (result != known_false) {
		return;
	}
	// ((X & Y) == 0) so at least one of them is zero
	if (isknown_true(x) && !isknown(y)) {
		y = known_false;
		return;
	}
	if (isknown_true(y) && !isknown(x)) {
		x = known_false;
	}
}

/**
 * @brief Uses the result from a logical OR to resolve unknown bits.
 */
inline void merge_known_bits_via_or(flag_state result, flag_state& x, flag_state& y) {
	using enum flag_state;
	if (result != known_true) {
		return;
	}
	// ((X | Y) == 1) so at least one of them is zero
	if (isknown_true(x) && !isknown(y)) {
		y = known_false;
		return;
	}
	if (isknown_true(y) && !isknown(x)) {
		x = known_false;
	}
}

/**
 * @brief Uses the result from a logical XOR to resolve unknown bits.
 */
inline void merge_known_bits_via_xor(flag_state result, flag_state& x, flag_state& y) {
	using enum flag_state;
	if (result == known_false) {
		// ((X ^ Y) == 0) so both bits are the same
		if (isknown(x) && !isknown(y)) {
			y = x;
			return;
		}
		if (isknown(y) && !isknown(x)) {
			x = y;
			return;
		}
	}
	if (result == known_true) {
		// ((X ^ Y) == 1) so both bits are different
		if (isknown(x) && !isknown(y)) {
			y = cpl_flag_state(x);
			return;
		}
		if (isknown(y) && !isknown(x)) {
			x = cpl_flag_state(y);
			return;
		}
	}
}

template<typename T>
void merge_known_bits_via_and(reg_pair<T> result, reg_pair<T>& x, reg_pair<T>& y) {
	for (size_t i = 0; i < bit_width_of_type<T>(); i++) {
		flag_state R = result.bit_test(i);
		flag_state X = x.bit_test(i);
		flag_state Y = y.bit_test(i);
		merge_known_bits_via_and(R, X, Y);
		x.bit_copy(i, X);
		y.bit_copy(i, Y);
	}
}

template<typename T>
void merge_known_bits_via_or(reg_pair<T> result, reg_pair<T>& x, reg_pair<T>& y) {
	for (size_t i = 0; i < bit_width_of_type<T>(); i++) {
		flag_state R = result.bit_test(i);
		flag_state X = x.bit_test(i);
		flag_state Y = y.bit_test(i);
		merge_known_bits_via_or(R, X, Y);
		x.bit_copy(i, X);
		y.bit_copy(i, Y);
	}
}

template<typename T>
void merge_known_bits_via_xor(reg_pair<T> result, reg_pair<T>& x, reg_pair<T>& y) {
	for (size_t i = 0; i < bit_width_of_type<T>(); i++) {
		flag_state R = result.bit_test(i);
		flag_state X = x.bit_test(i);
		flag_state Y = y.bit_test(i);
		merge_known_bits_via_xor(R, X, Y);
		x.bit_copy(i, X);
		y.bit_copy(i, Y);
	}
}

template<typename T>
void merge_assuming_bitwise_and_is_zero(reg_pair<T>& x, reg_pair<T>& y) {
	reg_pair<T> result;
	result.set_to_zero();
	merge_known_bits_via_and(result, x, y);
}

template<typename T>
void merge_assuming_bitwise_or_is_all_ones(reg_pair<T>& x, reg_pair<T>& y) {
	reg_pair<T> result;
	result.set_to_all_ones();
	merge_known_bits_via_or(result, x, y);
}

template<typename T>
void merge_assuming_bitwise_xor_is_zero(reg_pair<T>& x, reg_pair<T>& y) {
	reg_pair<T> result;
	result.set_to_zero();
	merge_known_bits_via_xor(result, x, y);
}

template<typename T>
void merge_assuming_bitwise_xor_is_all_ones(reg_pair<T>& x, reg_pair<T>& y) {
	reg_pair<T> result;
	result.set_to_all_ones();
	merge_known_bits_via_xor(result, x, y);
}

//------------------------------------------------------------------------------
// Swap Byte Order
//------------------------------------------------------------------------------

template<typename T>
reg_pair<T> swap_byte_order(reg_pair<T> x);

template<>
inline reg_pair<uint8_t> swap_byte_order(reg_pair<uint8_t> x) {
	return x;
}

template<>
inline reg_pair<uint16_t> swap_byte_order(reg_pair<uint16_t> x) {
	x.bits = __builtin_bswap16(x.bits);
	x.mask = __builtin_bswap16(x.mask);
	return x;
}

template<>
inline reg_pair<uint24_t> swap_byte_order(reg_pair<uint24_t> x) {
	uint32_t temp_bits = static_cast<uint32_t>(x.bits);
	uint32_t temp_mask = static_cast<uint32_t>(x.mask);
	temp_bits = __builtin_bswap32(temp_bits) >> 8;
	temp_mask = __builtin_bswap32(temp_mask) >> 8;
	x.bits = static_cast<uint24_t>(temp_bits);
	x.mask = static_cast<uint24_t>(temp_mask);
	return x;
}

template<>
inline reg_pair<uint32_t> swap_byte_order(reg_pair<uint32_t> x) {
	x.bits = __builtin_bswap32(x.bits);
	x.mask = __builtin_bswap32(x.mask);
	return x;
}

template<>
inline reg_pair<uint48_t> swap_byte_order(reg_pair<uint48_t> x) {
	uint64_t temp_bits = static_cast<uint64_t>(x.bits);
	uint64_t temp_mask = static_cast<uint64_t>(x.mask);
	temp_bits = __builtin_bswap64(temp_bits) >> 16;
	temp_mask = __builtin_bswap64(temp_mask) >> 16;
	x.bits = static_cast<uint48_t>(temp_bits);
	x.mask = static_cast<uint48_t>(temp_mask);
	return x;
}

template<>
inline reg_pair<uint64_t> swap_byte_order(reg_pair<uint64_t> x) {
	x.bits = __builtin_bswap64(x.bits);
	x.mask = __builtin_bswap64(x.mask);
	return x;
}

//------------------------------------------------------------------------------
// Reverse Bits
//------------------------------------------------------------------------------

template<typename T>
reg_pair<T> bit_reverse(reg_pair<T> x);

template<>
inline reg_pair<uint8_t> bit_reverse(reg_pair<uint8_t> x) {
	x.bits = __builtin_bitreverse8(x.bits);
	x.mask = __builtin_bitreverse8(x.mask);
	return x;
}

template<>
inline reg_pair<uint16_t> bit_reverse(reg_pair<uint16_t> x) {
	x.bits = __builtin_bitreverse16(x.bits);
	x.mask = __builtin_bitreverse16(x.mask);
	return x;
}

template<>
inline reg_pair<uint24_t> bit_reverse(reg_pair<uint24_t> x) {
	uint32_t temp_bits = static_cast<uint32_t>(x.bits);
	uint32_t temp_mask = static_cast<uint32_t>(x.mask);
	temp_bits = __builtin_bitreverse32(temp_bits) >> 8;
	temp_mask = __builtin_bitreverse32(temp_mask) >> 8;
	x.bits = static_cast<uint24_t>(temp_bits);
	x.mask = static_cast<uint24_t>(temp_mask);
	return x;
}

template<>
inline reg_pair<uint32_t> bit_reverse(reg_pair<uint32_t> x) {
	x.bits = __builtin_bitreverse32(x.bits);
	x.mask = __builtin_bitreverse32(x.mask);
	return x;
}

template<>
inline reg_pair<uint48_t> bit_reverse(reg_pair<uint48_t> x) {
	uint64_t temp_bits = static_cast<uint64_t>(x.bits);
	uint64_t temp_mask = static_cast<uint64_t>(x.mask);
	temp_bits = __builtin_bitreverse64(temp_bits) >> 16;
	temp_mask = __builtin_bitreverse64(temp_mask) >> 16;
	x.bits = static_cast<uint48_t>(temp_bits);
	x.mask = static_cast<uint48_t>(temp_mask);
	return x;
}

template<>
inline reg_pair<uint64_t> bit_reverse(reg_pair<uint64_t> x) {
	x.bits = __builtin_bitreverse64(x.bits);
	x.mask = __builtin_bitreverse64(x.mask);
	return x;
}

//------------------------------------------------------------------------------
// Bit Tests (mimics the functions from C23 <stdbit.h>)
//------------------------------------------------------------------------------

template<typename T>
void reg_pair_count_ones(uint8_t& min_bound, uint8_t& max_bound, reg_pair<T> x) {
	min_bound = popcount(x.get_unknown_bits_as_zeros());
	max_bound = popcount(x.get_unknown_bits_as_ones());
}

template<typename T>
void reg_pair_leading_zeros(uint8_t& min_bound, uint8_t& max_bound, reg_pair<T> x) {
	min_bound = countl_zero(x.get_unknown_bits_as_zeros());
	max_bound = countl_zero(x.get_unknown_bits_as_ones());
}

template<typename T>
void reg_pair_trailing_zeros(uint8_t& min_bound, uint8_t& max_bound, reg_pair<T> x) {
	min_bound = countr_zero(x.get_unknown_bits_as_zeros());
	max_bound = countr_zero(x.get_unknown_bits_as_ones());
}

#endif /* REG_PAIR_HPP */
