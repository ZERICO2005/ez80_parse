#include "../optimize/current_state.hpp"
#include "../ez80_type.h"

#include <random>
#include <stdio.h>

class reg8_pair_test {
private:
	std::random_device rd;
	std::mt19937 gen;
	std::uniform_int_distribution<uint8_t> dist_reg;
	size_t random_test_iter;
public:
	reg8_pair_test(size_t test_count) {
		this->random_test_iter = test_count;
		gen = std::mt19937(rd());
	}
	size_t random_test_count() {
		return random_test_iter;
	}
	reg8_pair get_rand() {
		reg8_pair ret;
		ret.mask = dist_reg(gen);
		ret.bits = dist_reg(gen);
		return ret;
	}
	reg8_pair get_rand_known() {
		reg8_pair ret;
		ret.set_value(dist_reg(gen));
		return ret;
	}
	flag_state get_rand_carry() {
		using enum flag_state;
		switch(std::uniform_int_distribution(0, 2)(gen)) {
			default:
			case 0: return unknown;
			case 1: return known_true;
			case 2: return known_false;
		}
	}
	uint8_t get_rand_carry_known() {
		return std::uniform_int_distribution(0, 1)(gen);
	}
};

template<typename T>
string to_binary_str(T x) {
	char buf[bit_width_of_type<T>() + 1];
	for (size_t i = bit_width_of_type<T>(); i --> 0;) {
		buf[i] = (x & 1) ? '1' : '0';
		x >>= 1;
	}
	buf[sizeof(buf) - 1] = '\0';
	return string(buf);
}

#define TEST_FAILED() \
printf("test failed: %ld\n", static_cast<long>(__LINE__))

bool test_add_with_carry(reg8_pair_test& test) {
#if 1
	for (size_t i = 0; i < test.random_test_count(); i++) {
		reg8_pair x = test.get_rand_known();
		reg8_pair y = test.get_rand_known();
		uint8_t carry = test.get_rand_carry_known();

		uint16_t acc = ((uint16_t)x.bits + (uint16_t)y.bits) + (uint16_t)carry;
		uint8_t truth = (uint8_t)acc;
		flag_state truth_carry = set_flag_state(acc >= 256);

		flag_state guess_carry = set_flag_state(carry);
		reg8_pair guess = add_with_carry(x, y, guess_carry);

		if (!((truth == guess.bits) && (truth_carry == guess_carry))) {
			TEST_FAILED();
			return false;
		}
	}
	return true;
#else
	for (size_t i = 0; i < test.random_test_count(); i++) {
		reg8_pair x = test.get_rand_known();
		reg8_pair y = test.get_rand_known();
		uint8_t carry = test.get_rand_carry_known();

		// set all unknown bits to zero
		uint8_t x_lo = (x.bits & x.mask);
		uint8_t y_lo = (y.bits & y.mask);
		uint16_t acc_lo = ((uint16_t)x_lo + (uint16_t)y_lo) + 0;
		uint8_t truth_lo = (uint8_t)acc_lo;
		flag_state truth_lo_carry = set_flag_state(acc_lo >= 256);

		// set all unknown bits to one
		uint8_t x_hi = (x.bits | ~x.mask);
		uint8_t y_hi = (y.bits | ~y.mask);
		uint16_t acc_hi = ((uint16_t)x_hi + (uint16_t)y_hi) + 1;
		uint8_t truth_hi = (uint8_t)acc_hi;
		flag_state truth_hi_carry = set_flag_state(acc_hi >= 256);

		flag_state guess_carry = set_flag_state(carry);
		reg8_pair guess = add_with_carry(x, y, guess_carry);

	
		if (!((truth == guess.bits) && (truth_carry == guess_carry))) {
			TEST_FAILED();
			return false;
		}
	}
	return true;
#endif
}

bool test_subtract_with_carry(reg8_pair_test& test) {
	for (size_t i = 0; i < test.random_test_count(); i++) {
		reg8_pair x = test.get_rand_known();
		reg8_pair y = test.get_rand_known();

		uint16_t acc = ((uint16_t)x.bits - (uint16_t)y.bits);
		uint8_t truth = (uint8_t)acc;
		flag_state truth_carry = set_flag_state(acc >= 256);

		flag_state guess_carry = flag_state::known_false;
		reg8_pair guess = subtract_with_carry(x, y, guess_carry);

		if (!((truth == guess.bits) && (truth_carry == guess_carry))) {
			TEST_FAILED();
			return false;
		}
	}

	for (size_t i = 0; i < test.random_test_count(); i++) {
		reg8_pair x = test.get_rand_known();
		reg8_pair y = test.get_rand_known();
		uint8_t carry = test.get_rand_carry_known();

		uint16_t acc = ((uint16_t)x.bits - (uint16_t)y.bits) - (uint16_t)carry;
		uint8_t truth = (uint8_t)acc;
		flag_state truth_carry = set_flag_state(acc >= 256);

		flag_state guess_carry = set_flag_state(carry);
		reg8_pair guess = subtract_with_carry(x, y, guess_carry);

		if (!((truth == guess.bits) && (truth_carry == guess_carry))) {
			TEST_FAILED();
			return false;
		}
	}
	return true;
}

bool test_logical(reg8_pair_test& test) {
	for (size_t i = 0; i < test.random_test_count(); i++) {
		reg8_pair x = test.get_rand_known();
		reg8_pair y = test.get_rand_known();

		uint8_t truth_and = (x.bits & y.bits);
		uint8_t truth_orr = (x.bits | y.bits);
		uint8_t truth_xor = (x.bits ^ y.bits);

		reg8_pair guess_and = (x & y);
		reg8_pair guess_orr = (x | y);
		reg8_pair guess_xor = (x ^ y);

		if (!(truth_and == guess_and.bits)) {
			TEST_FAILED();
			return false;
		}

		if (!(truth_orr == guess_orr.bits)) {
			TEST_FAILED();
			return false;
		}

		if (!(truth_xor == guess_xor.bits)) {
			TEST_FAILED();
			return false;
		}
	}
	return true;
}

bool test_inc_dec(reg8_pair_test& test) {
	reg8_pair g_inc, g_dec;
	uint8_t t_inc, t_dec;
	t_inc = 0x32;
	t_dec = 0xAA;
	g_inc.set_value(0x32);
	g_dec.set_value(0xAA);
	for (size_t i = 0; i < 256; i++) {
		++g_inc;
		++t_inc;
		--g_dec;
		--t_dec;
		if (!(g_inc.isknown_equal(t_inc))) {
			printf(
				"%3zu: inc: T %02X != G %02X | %02X\n",
				i, t_inc, g_inc.bits, g_inc.mask
			);
			TEST_FAILED();
			return false;
		}
		if (!(g_dec.isknown_equal(t_dec))) {
			printf(
				"%3zu: dec: T %02X != G %02X | %02X\n",
				i, t_inc, g_inc.bits, g_inc.mask
			);
			TEST_FAILED();
			return false;
		}
	}

	for (size_t i = 0; i < test.random_test_count(); i++) {
		// (~x + 1) == ~(x - 1)
		reg8_pair value = test.get_rand();
		reg8_pair cpl_inc = value;
		reg8_pair dec_cpl = value;

		cpl_inc = ~cpl_inc;
		++cpl_inc;

		--dec_cpl;
		dec_cpl = ~dec_cpl;

		if (!(cpl_inc.is_canonical() && dec_cpl.is_canonical())) {
			TEST_FAILED();
			return false;
		}

		if (!((cpl_inc.bits == dec_cpl.bits) && (cpl_inc.bits == dec_cpl.bits))) {
			printf(
				"%3zu: ~+ %02X %02X != -~ %02X %02X\n",
				i, cpl_inc.bits, cpl_inc.mask, dec_cpl.bits, dec_cpl.mask
			);
			TEST_FAILED();
			return false;
		}
	}
	return true;
}

bool test_mul(reg8_pair_test& test) {
	for (size_t i = 0; i < test.random_test_count(); i++) {
		reg8_pair x = test.get_rand_known();
		reg8_pair y = test.get_rand_known();

		uint8_t truth = (x.bits * y.bits);

		reg8_pair guess = (x * y);

		if (!(truth == guess.bits)) {
			printf(
				"%3zu: %02X * %02X --> %02X != %02X\n",
				i, x.bits, y.bits, truth, guess.bits
			);
			TEST_FAILED();
			return false;
		}
	}

	for (size_t i = 0; i < test.random_test_count(); i++) {
		reg8_pair x = test.get_rand();
		reg8_pair y;
		y.set_value(16);

		reg8_pair guess = (x * y);

		if (!(((guess.mask & 0xF) == 0xF) && ((guess.bits & 0xF) == 0x0))) {
			printf(
				"%3zu: %02X %02X\n",
				i, guess.bits, guess.mask
			);
			TEST_FAILED();
			return false;
		}
	}
	return true;
}

bool test_divrem_unsigned(reg8_pair_test& test) {
	for (size_t i = 0; i < test.random_test_count(); i++) {
		reg8_pair x = test.get_rand_known();
		reg8_pair y = test.get_rand_known();
		if (!y.isknown_nonzero()) {
			y.set_value(1);
		}

		uint8_t t_quo = (x.bits / y.bits);
		uint8_t t_rem = (x.bits % y.bits);

		reg8_pair g_quo, g_rem;
		divrem_unsigned(g_quo, g_rem, x, y);

		if (!((t_quo == g_quo.bits) && (t_rem == g_rem.bits))) {
			printf(
				"%3zu: %02X / %02X --> %02X R %02X != %02X R %02X\n",
				i, x.bits, y.bits, t_quo, t_rem, g_quo.bits, g_rem.bits
			);
			TEST_FAILED();
			return false;
		}
	}
	for (size_t i = 0; i < test.random_test_count(); i++) {
		reg8_pair x = test.get_rand();
		reg8_pair y;
		y.set_value(18);

		reg8_pair guess = div_unsigned(x, y);

		if (!(((guess.mask & 0xF0) == 0xF0) && ((guess.bits & 0xF0) == 0x00))) {
			printf(
				"%3zu: %02X %02X\n",
				i, guess.bits, guess.mask
			);
			TEST_FAILED();
			return false;
		}
	}
	return true;
}

bool test_reg_pair() {
	// set the random test count
	reg8_pair_test test(100);
	bool passed = true;

	// run the tests
	passed &= test_add_with_carry(test);
	passed &= test_subtract_with_carry(test);
	passed &= test_mul(test);
	passed &= test_logical(test);
	passed &= test_inc_dec(test);
	passed &= test_divrem_unsigned(test);

	return passed;
}



