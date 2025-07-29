#ifndef KNOWN_BIT_INTEGER_FWD_HPP
#define KNOWN_BIT_INTEGER_FWD_HPP

#include <cstddef>
#include <cstdint>
#include <algorithm>

#ifndef KBI_USE_BUILTINS
#define KBI_USE_BUILTINS 0
#endif /* KBI_USE_BUILTINS */

#if !((KBI_USE_BUILTINS) == 0 || (KBI_USE_BUILTINS) == 1)
#error "KBI_USE_BUILTINS must be defined to 0 or 1"
#endif

namespace kbi {

//------------------------------------------------------------------------------
// helpers
//------------------------------------------------------------------------------

template<typename T>
constexpr size_t bit_width_of_type();

template<> constexpr
size_t bit_width_of_type<uint8_t>() { return 8; }
template<> constexpr
size_t bit_width_of_type<uint16_t>() { return 16; }
template<> constexpr
size_t bit_width_of_type<uint32_t>() { return 32; }
template<> constexpr
size_t bit_width_of_type<uint64_t>() { return 64; }

template<typename T>
constexpr T get_all_ones();

template<> constexpr
uint8_t get_all_ones<uint8_t>() { return UINT8_C(0xFF); }
template<> constexpr
uint16_t get_all_ones<uint16_t>() { return UINT16_C(0xFFFF); }
template<> constexpr
uint32_t get_all_ones<uint32_t>() { return UINT32_C(0xFFFFFFFF); }
template<> constexpr
uint64_t get_all_ones<uint64_t>() { return UINT64_C(0xFFFFFFFFFFFFFFFF); }

//------------------------------------------------------------------------------
// bit_state
//------------------------------------------------------------------------------

enum class bit_state {
	unknown,
	known_true,
	known_false,
};

using enum bit_state;

inline bit_state invert_bit_state(bit_state x) {
	if (x == known_true) {
		return known_false;
	}
	if (x == known_false) {
		return known_true;
	}
	return unknown;
}

inline bit_state and_bit_state(bit_state x, bit_state y) {
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

inline bit_state and_bit_state(bit_state x, bit_state y, bit_state z) {
	return and_bit_state(and_bit_state(x, y), z);
}

inline bit_state or_bit_state(bit_state x, bit_state y) {
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

inline bit_state or_bit_state(bit_state x, bit_state y, bit_state z) {
	return or_bit_state(or_bit_state(x, y), z);
}

inline bit_state xor_bit_state(bit_state x, bit_state y) {
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

inline bit_state xor_bit_state(bit_state x, bit_state y, bit_state z) {
	return xor_bit_state(xor_bit_state(x, y), z);
}

inline bool isknown(bit_state x) {
	return (x != unknown);
}

inline bool isknown_true(bit_state x) {
	return (x == known_true);
}

inline bool isknown_false(bit_state x) {
	return (x == known_false);
}

inline bit_state set_bit_state(bool b) {
	return (b ? known_true : known_false);
}

inline bit_state set_bit_state(bit_state f) {
	return f;
}

//------------------------------------------------------------------------------
// known_bit_integer
//------------------------------------------------------------------------------

struct arithmetic_flags {
	bit_state carry;
	bit_state zero;
	bit_state sign;
	bit_state overflow;
	bit_state parity_even;

	static bit_state test_addition_overflow(bit_state result, bit_state x, bit_state y) {
		/*
		Signed Overflow Truth Table
		Overflow occurs when (pos + pos = neg) or (neg + neg = pos)
		+---+---+---+---+---+
		| X | Y |   | R |   |
		+---+---+---+---+---+
		|   |   | 0 | 1 | ? |
		+---+---+---+---+---+
		| 0 | 0 | 0 : 1 : ? :
		| 0 | 1 | 0 : 0 : 0 :
		| 0 | ? | 0 : ? : ? :
		+---+---+---+---+---+
		| 1 | 0 | 0 : 0 : 0 :
		| 1 | 1 | 1 : 0 : ? :
		| 1 | ? | ? : 0 : ? :
		+---+---+---+---+---+
		| ? | 0 | 0 : ? : ? :
		| ? | 1 | ? : 0 : ? :
		| ? | ? | ? : ? : ? :
		+---+---+---+---+---+
		*/
		if (isknown(x) && isknown(y)) {
			if (x != y) {
				// never overflows
				return known_false;
			}
			if (isknown(result)) {
				if (x == result) {
					// inputs and outputs are the same sign
					return known_false;
				}
				// (pos + pos = neg) or (neg + neg = pos)
				return known_true;
			}
			// overflow cannot be determined
			return unknown;
		}
		if (isknown(result)) {
			if (isknown(x)) {
				if (x == result) {
					// inputs and outputs are the same sign
					return known_false;
				}
			}
			if (isknown(y)) {
				if (y == result) {
					// inputs and outputs are the same sign
					return known_false;
				}
			}
		}
		return unknown;
	}
	static bit_state test_subtraction_overflow(bit_state result, bit_state x, bit_state y) {
		return test_addition_overflow(result, x, invert_bit_state(y));
	}
};

template<typename T>
class known_bit_integer {
private:
	T bits;
	T mask;

	T get_bit_mask(int b) const {
		if (b < bit_width_of_type<T>()) {
			return (static_cast<T>(1) << b);
		}
		// out of range
		return 0;
	}

public:

	T get_raw_bits() const {
		return bits;
	}
	T get_raw_mask() const {
		return mask;
	}
	void set_raw_bits(T b) {
		bits = b;
	}
	void set_raw_mask(T m) {
		mask = m;
	}
	
	void set_canonical() {
		// set unknown bits to zero
		bits &= mask;
	}
	bool is_canonical() const {
		// return false if any unknown bits are non-zero
		return ((bits & ~mask) == 0);
	}

	void set_value(T x) {
		bits = x;
		mask = get_all_ones<T>();
	}
	void set_value(known_bit_integer<T> x) {
		bits = x.bits;
		mask = x.mask;
	}

	void set_unknown() {
		bits = 0;
		mask = 0;
	}

	void set_to_zero() {
		set_value(0);
	}

	void set_to_all_ones() {
		set_value(get_all_ones<T>());
	}

	known_bit_integer(T x) {
		bits = x;
		mask = get_all_ones<T>();
	}

	known_bit_integer() {
		set_unknown();
	}


	bool isknown_fully() const {
		T temp = ~mask;
		return (temp == 0);
	}
	bool isunknown_fully() const {
		return (mask == 0);
	}
	bool isknown_zero() const {
		return (isknown_fully() && (bits == 0));
	}
	bool isknown_nonzero() const {
		/* test if any bits are known to set */
		return ((mask & bits) != 0);
	}
	bit_state is_zero() const {
		if (isknown_zero()) {
			return known_true;
		}
		if (isknown_nonzero()) {
			return known_false;
		}
		return unknown;
	}
	bit_state is_nonzero() const {
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
		/* test if any known bits differ */
		return (((mask & (bits ^ x))) != 0);
	}
	bit_state is_equal(T x) const {
		if (isknown_equal(x)) {
			return known_true;
		}
		if (isknown_notequal(x)) {
			return known_false;
		}
		return unknown;
	}
	bit_state is_notequal(T x) const {
		if (isknown_equal(x)) {
			return known_false;
		}
		if (isknown_notequal(x)) {
			return known_true;
		}
		return unknown;
	}

	T get_unsigned_minimum() const {
		T ret = (bits & mask);
		return ret;
	}

	T get_unsigned_maximum() const {
		T ret = (bits & mask);
		ret |= (~mask);
		return ret;
	}

	/* bit operations */

	bit_state bit_test(int b) const {
		const T index = get_bit_mask(b);
		if (mask & index) {
			return (bits & index) ? known_true : known_false;
		}
		return unknown;
	}

	bool isknown_bit(int b) const {
		return (bit_test(b) != unknown);
	}
	bool isknown_bit_set(int b) const {
		return (bit_test(b) == known_true);
	}
	bool isknown_bit_clear(int b) const {
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
	void bit_copy(int b, bit_state f) {
		switch (f) {
			case unknown: bit_unknown(b); return;
			case known_true: bit_set(b); return;
			case known_false: bit_clear(b); return;
		}
	}
	void bit_copy(int b, bool f) {
		bit_copy(b, set_bit_state(f));
	}

	void bit_copy_if_unknown(int b, bit_state f) {
		if (bit_test(b) != unknown) {
			return;
		}
		switch (f) {
			case unknown: bit_unknown(b); return;
			case known_true: bit_set(b); return;
			case known_false: bit_clear(b); return;
		}
	}
	void bit_copy_if_unknown(int b, bool f) {
		bit_copy(b, set_bit_state(f));
	}


	/* set to bit */

	void set_to_zero_or_one(bit_state f) {
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
		set_to_zero_or_one(set_bit_state(f));
	}
	void set_to_zero_or_all_ones(bit_state f) {
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
	void set_to_zero_or_all_ones(bool f) {
		set_to_zero_or_one(set_bit_state(f));
	}

	/* set to range */

	void set_known_unsigned_range(T min_bound, T max_bound) {
		if (min_bound == max_bound) {
			set_value(min_bound);
			return;
		}
		if (min_bound > max_bound) {
			std::swap(min_bound, max_bound);
		}
		if (get_unsigned_minimum() < min_bound) {
			set_value(min_bound);
			return;
		}
		if (get_unsigned_maximum() > max_bound) {
			set_value(max_bound);
			return;
		}
		for (size_t i = bit_width_of_type<T>(); i --> 0;) {
			bool x = (min_bound & (static_cast<T>(1) << i));
			bool y = (max_bound & (static_cast<T>(1) << i));
			if (x != y) {
				break;
			}
			bit_copy_if_unknown(i, x);
		}
	}

	constexpr int get_signbit_index() const {
		return (bit_width_of_type<T>() - 1);
	}

	bit_state test_signbit() const {
		return bit_test(get_signbit_index());
	}


/* bitwise operations */

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

/* bitwise operations */

known_bit_integer<T> friend complement(known_bit_integer<T> x);

known_bit_integer<T> friend bitwise_and(known_bit_integer<T> x, known_bit_integer<T> y);

known_bit_integer<T> friend bitwise_or(known_bit_integer<T> x, known_bit_integer<T> y);

known_bit_integer<T> friend bitwise_xor(known_bit_integer<T> x, known_bit_integer<T> y);


known_bit_integer<T> friend shift_left_logical(known_bit_integer<T> x, int shift);

known_bit_integer<T> friend shift_right_logical(known_bit_integer<T> x, int shift);

known_bit_integer<T> friend shift_left_ones(known_bit_integer<T> x, int shift);

known_bit_integer<T> friend shift_right_ones(known_bit_integer<T> x, int shift);

known_bit_integer<T> friend shift_left_unknown_bits(known_bit_integer<T> x, int shift);

known_bit_integer<T> friend shift_right_unknown_bits(known_bit_integer<T> x, int shift);

known_bit_integer<T> friend shift_right_arithmetic(known_bit_integer<T> x, int shift);

/* rotates */

known_bit_integer<T> friend rotate_left(known_bit_integer<T> x, int shift);

known_bit_integer<T> friend rotate_right(known_bit_integer<T> x, int shift);

known_bit_integer<T> friend rotate_left_with_carry(known_bit_integer<T> x, bit_state& carry_inout, int shift);

known_bit_integer<T> friend rotate_right_with_carry(known_bit_integer<T> x, bit_state& carry_inout, int shift);

/* rounding */

known_bit_integer<T> friend shift_right_round_to_even_unsigned(known_bit_integer<T> x, int shift);

known_bit_integer<T> friend shift_right_ceil_unsigned(known_bit_integer<T> x, int shift);

/* addition and subtraction */

known_bit_integer<T> friend increment_by_carry(known_bit_integer<T> x, bit_state carry);

known_bit_integer<T> friend decrement_by_carry(known_bit_integer<T> x, bit_state carry);

known_bit_integer<T> friend increment(known_bit_integer<T> x);

known_bit_integer<T> friend decrement(known_bit_integer<T> x);

known_bit_integer<T> friend negate(known_bit_integer<T> x);

known_bit_integer<T> friend decrement_if_true_increment_if_false(known_bit_integer<T> x, bit_state cond);

known_bit_integer<T> friend increment_if_true_decrement_if_false(known_bit_integer<T> x, bit_state cond);

known_bit_integer<T> friend conditional_negate(known_bit_integer<T> x, bit_state cond);

known_bit_integer<T> friend absolute_value(known_bit_integer<T> x);

known_bit_integer<T> friend addition_with_carry_and_flags(
	known_bit_integer<T> x,
	known_bit_integer<T> y,
	bit_state& carry_inout,
	arithmetic_flags& flags
);

known_bit_integer<T> friend addition_with_carry(
	known_bit_integer<T> x,
	known_bit_integer<T> y,
	bit_state& carry_inout
);

known_bit_integer<T> friend addition(known_bit_integer<T> x, known_bit_integer<T> y);

known_bit_integer<T> friend subtract_with_carry_and_flags(
	known_bit_integer<T> x,
	known_bit_integer<T> y,
	bit_state& carry_inout,
	arithmetic_flags& flags
);

known_bit_integer<T> friend subtract_with_carry(
	known_bit_integer<T> x,
	known_bit_integer<T> y,
	bit_state& carry_inout
);

known_bit_integer<T> friend subtract(known_bit_integer<T> x, known_bit_integer<T> y);

/* compare */

bit_state friend compare_equal(known_bit_integer<T> x, known_bit_integer<T> y);

bit_state friend compare_notequal(known_bit_integer<T> x, known_bit_integer<T> y);


void friend unsigned_compare(known_bit_integer<T> x, known_bit_integer<T> y, arithmetic_flags& flags);


void friend signed_compare(known_bit_integer<T> x, known_bit_integer<T> y, arithmetic_flags& flags);


bit_state friend unsigned_less_than(known_bit_integer<T> x, known_bit_integer<T> y);


bit_state friend unsigned_less_equal(known_bit_integer<T> x, known_bit_integer<T> y);


bit_state friend unsigned_greater_than(known_bit_integer<T> x, known_bit_integer<T> y);


bit_state friend signed_greater_equal(known_bit_integer<T> x, known_bit_integer<T> y);


bit_state friend signed_less_than(known_bit_integer<T> x, known_bit_integer<T> y);


bit_state friend signed_less_equal(known_bit_integer<T> x, known_bit_integer<T> y);


bit_state friend signed_greater_than(known_bit_integer<T> x, known_bit_integer<T> y);


bit_state friend signed_greater_equal(known_bit_integer<T> x, known_bit_integer<T> y);

/* multiplication */

known_bit_integer<T> friend multiply(known_bit_integer<T> x, known_bit_integer<T> y);

void friend multiply_hilo_unsigned(
	known_bit_integer<T>& hi,
	known_bit_integer<T>& lo,
	known_bit_integer<T> x,
	known_bit_integer<T> y
);

void friend multiply_hilo_signed(
	known_bit_integer<T>& hi,
	known_bit_integer<T>& lo,
	known_bit_integer<T> x,
	known_bit_integer<T> y
);

/**
 * @brief Uses the knowledge that x == y to resolve a few more known bits.
 * For example, all squared integers have bit 1 cleared.
 */
known_bit_integer<T> friend square(known_bit_integer<T> x);

void friend square_hilo_unsigned(
	known_bit_integer<T>& hi,
	known_bit_integer<T>& lo,
	known_bit_integer<T> x
);

void friend square_hilo_signed(
	known_bit_integer<T>& hi,
	known_bit_integer<T>& lo,
	known_bit_integer<T> x
);

/* division and remainder */

/** @brief Same as C99/C++11 */
known_bit_integer<T> friend divrem_trunc_unsigned(
	known_bit_integer<T>& quo,
	known_bit_integer<T>& rem,
	known_bit_integer<T> num,
	known_bit_integer<T> den
);

/** @brief Same as C99/C++11 */
known_bit_integer<T> friend divrem_trunc_signed(
	known_bit_integer<T>& quo,
	known_bit_integer<T>& rem,
	known_bit_integer<T> num,
	known_bit_integer<T> den
);

known_bit_integer<T> friend divrem_floor_signed(
	known_bit_integer<T>& quo,
	known_bit_integer<T>& rem,
	known_bit_integer<T> num,
	known_bit_integer<T> den
);

known_bit_integer<T> friend divrem_euclidean_signed(
	known_bit_integer<T>& quo,
	known_bit_integer<T>& rem,
	known_bit_integer<T> num,
	known_bit_integer<T> den
);

/** @brief Same as C99/C++11 */
known_bit_integer<T> friend div_trunc_unsigned(known_bit_integer<T> num, known_bit_integer<T> den);

/** @brief Same as C99/C++11 */
known_bit_integer<T> friend rem_trunc_unsigned(known_bit_integer<T> num, known_bit_integer<T> den);

/** @brief Same as C99/C++11 */
known_bit_integer<T> friend div_trunc_signed(known_bit_integer<T> num, known_bit_integer<T> den);

/** @brief Same as C99/C++11 */
known_bit_integer<T> friend rem_trunc_signed(known_bit_integer<T> num, known_bit_integer<T> den);

known_bit_integer<T> friend div_floor_signed(known_bit_integer<T> num, known_bit_integer<T> den);

known_bit_integer<T> friend rem_floor_signed(known_bit_integer<T> num, known_bit_integer<T> den);

known_bit_integer<T> friend div_euclidean_signed(known_bit_integer<T> num, known_bit_integer<T> den);

known_bit_integer<T> friend rem_euclidean_signed(known_bit_integer<T> num, known_bit_integer<T> den);

/* C23 <stdbit.h> */

/**
 * @brief similar to __builtin_clz/stdc_leading_zeros/std::countl_zero
 */
void friend leading_zeros(int& min_bound, int& max_bound, known_bit_integer<T> x);

/**
 * @brief similar to stdc_leading_ones/std::countl_one
 */
void friend leading_ones(int& min_bound, int& max_bound, known_bit_integer<T> x);

/**
 * @brief similar to __builtin_ctz/stdc_trailing_zeros/std::countr_zero
 */
void friend trailing_zeros(int& min_bound, int& max_bound, known_bit_integer<T> x);

/**
 * @brief similar to stdc_trailing_ones/std::countr_one
 */
void friend trailing_ones(int& min_bound, int& max_bound, known_bit_integer<T> x);

/**
 * @brief similar to stdc_first_leading_zero
 */
void friend first_leading_zero(int& min_bound, int& max_bound, known_bit_integer<T> x);

/**
 * @brief similar to stdc_first_leading_one
 */
void friend first_leading_one(int& min_bound, int& max_bound, known_bit_integer<T> x);

/**
 * @brief similar to stdc_first_trailing_zero
 */
void friend first_trailing_zero(int& min_bound, int& max_bound, known_bit_integer<T> x);

/**
 * @brief similar to __builtin_ffs/stdc_first_trailing_one
 */
void friend first_trailing_one(int& min_bound, int& max_bound, known_bit_integer<T> x);

/**
 * @brief similar to stdc_count_zeros
 */
void friend count_zeros(int& min_bound, int& max_bound, known_bit_integer<T> x);

/**
 * @brief similar to stdc_count_ones/std::popcount
 */
void friend count_ones(int& min_bound, int& max_bound, known_bit_integer<T> x);

/**
 * @brief similar to stdc_has_single_bit/std::has_single_bit
 */
bit_state friend has_single_bit(known_bit_integer<T> x);

/**
 * @brief similar to stdc_bit_width/std::bit_width
 */
void friend bit_width(int& min_bound, int& max_bound, known_bit_integer<T> x);

/**
 * @brief similar to stdc_bit_floor/std::bit_floor
 */
known_bit_integer<T> friend bit_floor(known_bit_integer<T> x);

/**
 * @brief follows C23 stdc_bit_ceil, which returns zero if the result does not
 * fit the return type. C++20 std::bit_ceil leaves this behaviour undefined.
 */
known_bit_integer<T> friend bit_ceil(known_bit_integer<T> x);

/* other functions */

known_bit_integer<T> friend swap_byte_order(known_bit_integer<T> x);

/**
 * @brief similar to __builtin_bitreverse
 */
known_bit_integer<T> friend reverse_bits(known_bit_integer<T> x);

/**
 * @brief similar to __builtin_clrsb (count leading redundant sign bits)
 */
void friend leading_signbits(int& min_bound, int& max_bound, known_bit_integer<T> x);

bit_state friend is_pairity_even(known_bit_integer<T> x);

bit_state friend is_pairity_odd(known_bit_integer<T> x);

//------------------------------------------------------------------------------
// Merge bits
//------------------------------------------------------------------------------

/** 
 * @brief sets bits to unknown on conflict
 */
void merge_bits_favor_unknown(known_bit_integer<T> y) {
	for (size_t i = 0; i < bit_width_of_type<T>(); i++) {
		bit_state X = this->bit_test(i);
		bit_state Y = y.bit_test(i);
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
void merge_bits_preserve(known_bit_integer<T> y) {
	for (size_t i = 0; i < bit_width_of_type<T>(); i++) {
		bit_state X = this->bit_test(i);
		bit_state Y = y.bit_test(i);
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
void merge_bits_overwrite(known_bit_integer<T> y) {
	for (size_t i = 0; i < bit_width_of_type<T>(); i++) {
		bit_state X = this->bit_test(i);
		bit_state Y = y.bit_test(i);
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

/** 
 * @brief sets any bits that differ to unknown
 */
void merge_bits_intersection(known_bit_integer<T> y) {
	for (size_t i = 0; i < bit_width_of_type<T>(); i++) {
		bit_state X = this->bit_test(i);
		bit_state Y = y.bit_test(i);
		if (X != Y) {
			this->bit_unknown(i);
		}
	}
}

bit_state is_strictly_positive() const {
	bit_state sign = test_signbit();
	if (sign == known_false) {
		if (isknown_nonzero()) {
			return known_true;
		}
		if (isknown_zero()) {
			return known_false;
		}
		return unknown;
	}
	if (sign == known_true) {
		return known_false;
	}
	return unknown;
}

bit_state is_negative() const {
	return test_signbit();
}

bit_state is_less_than_zero() const {
	return is_negative();
}

bit_state is_less_equal_zero() const {
	return or_bit_state(is_negative(), is_zero());
}

bit_state is_greater_than_zero() const {
	return is_strictly_positive();
}

bit_state is_greater_equal_zero() const {
	return invert_bit_state(is_negative());
}

bool isknown_strictly_positive() const {
	return isknown_true(is_negative());
}

bool isknown_negative() const {
	return isknown_true(is_negative());
}

bool isknown_less_than_zero() const {
	return isknown_true(is_less_than_zero());
}

bool isknown_less_equal_zero() const {
	return isknown_true(is_less_equal_zero());
}

bool isknown_greater_than_zero() const {
	return isknown_true(is_greater_than_zero());
}

bool isknown_greater_equal_zero() const {
	return isknown_true(is_greater_equal_zero());
}

};

} // namespace kbi

#endif /* KNOWN_BIT_INTEGER_FWD_HPP */
