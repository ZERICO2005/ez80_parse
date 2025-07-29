#include "../common_std.h"
#include "../known_bit_integer/ez80_kbi.hpp"

#include <random>
#include <stdio.h>

class kbi8_test {
private:
	std::random_device rd;
	std::mt19937 gen;
	std::uniform_int_distribution<uint8_t> dist_reg;
	size_t random_test_iter;
public:
	kbi8_test(size_t test_count) {
		this->random_test_iter = test_count;
		gen = std::mt19937(rd());
	}
	size_t random_test_count() {
		return random_test_iter;
	}
	kbi8 get_rand() {
		kbi8 ret;
		ret.get_bits_raw() = dist_reg(gen);
		ret.get_mask_raw() = dist_reg(gen);
		ret.set_canonical();
		return ret;
	}
	kbi8 get_rand_known() {
		kbi8 ret;
		ret.set_value(dist_reg(gen));
		return ret;
	}
	kbi::bit_state get_rand_carry() {
		using enum kbi::bit_state;
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

bool test_add_with_carry(kbi8_test& test) {
#if 1
	for (size_t i = 0; i < test.random_test_count(); i++) {
		kbi8 x = test.get_rand_known();
		kbi8 y = test.get_rand_known();
		uint8_t carry = test.get_rand_carry_known();

		uint16_t acc = ((uint16_t)x.get_bits_raw() + (uint16_t)y.get_bits_raw()) + (uint16_t)carry;
		uint8_t truth = (uint8_t)acc;
		bit_state truth_carry = set_bit_state(acc >= 256);

		bit_state guess_carry = set_bit_state(carry);
		kbi8 guess = add_with_carry(x, y, guess_carry);

		if (!((truth == guess.get_bits_raw()) && (truth_carry == guess_carry))) {
			TEST_FAILED();
			return false;
		}
	}
	return true;
#else
	for (size_t i = 0; i < test.random_test_count(); i++) {
		kbi8 x = test.get_rand_known();
		kbi8 y = test.get_rand_known();
		uint8_t carry = test.get_rand_carry_known();

		// set all unknown bits to zero
		uint8_t x_lo = (x.get_bits_raw() & x.get_mask_raw());
		uint8_t y_lo = (y.get_bits_raw() & y.get_mask_raw());
		uint16_t acc_lo = ((uint16_t)x_lo + (uint16_t)y_lo) + 0;
		uint8_t truth_lo = (uint8_t)acc_lo;
		bit_state truth_lo_carry = set_bit_state(acc_lo >= 256);

		// set all unknown bits to one
		uint8_t x_hi = (x.get_bits_raw() | ~x.get_mask_raw());
		uint8_t y_hi = (y.get_bits_raw() | ~y.get_mask_raw());
		uint16_t acc_hi = ((uint16_t)x_hi + (uint16_t)y_hi) + 1;
		uint8_t truth_hi = (uint8_t)acc_hi;
		bit_state truth_hi_carry = set_bit_state(acc_hi >= 256);

		bit_state guess_carry = set_bit_state(carry);
		kbi8 guess = add_with_carry(x, y, guess_carry);

	
		if (!((truth == guess.get_bits_raw()) && (truth_carry == guess_carry))) {
			TEST_FAILED();
			return false;
		}
	}
	return true;
#endif
}

bool test_sub_with_carry(kbi8_test& test) {
	for (size_t i = 0; i < test.random_test_count(); i++) {
		kbi8 x = test.get_rand_known();
		kbi8 y = test.get_rand_known();

		uint16_t acc = ((uint16_t)x.get_bits_raw() - (uint16_t)y.get_bits_raw());
		uint8_t truth = (uint8_t)acc;
		bit_state truth_carry = set_bit_state(acc >= 256);

		bit_state guess_carry = bit_state::known_false;
		kbi8 guess = sub_with_carry(x, y, guess_carry);

		if (!((truth == guess.get_bits_raw()) && (truth_carry == guess_carry))) {
			printf(
				"%3zu: %02X != %02X [%02X] | %d != %d \n",
				i, truth, guess.get_bits_raw(), guess.get_mask_raw(), truth_carry, guess_carry
			);
			TEST_FAILED();
			return false;
		}
	}

	for (size_t i = 0; i < test.random_test_count(); i++) {
		kbi8 x = test.get_rand_known();
		kbi8 y = test.get_rand_known();
		uint8_t carry = test.get_rand_carry_known();

		uint16_t acc = ((uint16_t)x.get_bits_raw() - (uint16_t)y.get_bits_raw()) - (uint16_t)carry;
		uint8_t truth = (uint8_t)acc;
		bit_state truth_carry = set_bit_state(acc >= 256);

		bit_state guess_carry = set_bit_state(carry);
		kbi8 guess = sub_with_carry(x, y, guess_carry);

		if (!((truth == guess.get_bits_raw()) && (truth_carry == guess_carry))) {
			TEST_FAILED();
			return false;
		}
	}
	return true;
}

bool test_logical(kbi8_test& test) {
	for (size_t i = 0; i < test.random_test_count(); i++) {
		kbi8 x = test.get_rand_known();
		kbi8 y = test.get_rand_known();

		uint8_t truth_and = (x.get_bits_raw() & y.get_bits_raw());
		uint8_t truth_orr = (x.get_bits_raw() | y.get_bits_raw());
		uint8_t truth_xor = (x.get_bits_raw() ^ y.get_bits_raw());

		kbi8 guess_and = (x & y);
		kbi8 guess_orr = (x | y);
		kbi8 guess_xor = (x ^ y);

		if (!(truth_and == guess_and.get_bits_raw())) {
			TEST_FAILED();
			return false;
		}

		if (!(truth_orr == guess_orr.get_bits_raw())) {
			TEST_FAILED();
			return false;
		}

		if (!(truth_xor == guess_xor.get_bits_raw())) {
			TEST_FAILED();
			return false;
		}
	}
	return true;
}

bool test_inc_dec(kbi8_test& test) {
	kbi8 g_inc, g_dec;
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
				i, t_inc, g_inc.get_bits_raw(), g_inc.get_mask_raw()
			);
			TEST_FAILED();
			return false;
		}
		if (!(g_dec.isknown_equal(t_dec))) {
			printf(
				"%3zu: dec: T %02X != G %02X | %02X\n",
				i, t_inc, g_inc.get_bits_raw(), g_inc.get_mask_raw()
			);
			TEST_FAILED();
			return false;
		}
	}

	for (size_t i = 0; i < test.random_test_count(); i++) {
		// (~x + 1) == ~(x - 1)
		kbi8 value = test.get_rand();
		kbi8 cpl_inc = value;
		kbi8 dec_cpl = value;

		cpl_inc = ~cpl_inc;
		++cpl_inc;

		--dec_cpl;
		dec_cpl = ~dec_cpl;

		if (!(cpl_inc.is_canonical() && dec_cpl.is_canonical())) {
			printf(
				"%3zu: ~+ %02X %02X != -~ %02X %02X\n",
				i, cpl_inc.get_bits_raw(), cpl_inc.get_mask_raw(), dec_cpl.get_bits_raw(), dec_cpl.get_mask_raw()
			);
			TEST_FAILED();
			return false;
		}

		if (!((cpl_inc.get_bits_raw() == dec_cpl.get_bits_raw()) && (cpl_inc.get_bits_raw() == dec_cpl.get_bits_raw()))) {
			printf(
				"%3zu: ~+ %02X %02X != -~ %02X %02X\n",
				i, cpl_inc.get_bits_raw(), cpl_inc.get_mask_raw(), dec_cpl.get_bits_raw(), dec_cpl.get_mask_raw()
			);
			TEST_FAILED();
			return false;
		}
	}
	return true;
}

bool test_mul(kbi8_test& test) {
	for (size_t i = 0; i < test.random_test_count(); i++) {
		kbi8 x = test.get_rand_known();
		kbi8 y = test.get_rand_known();

		uint8_t truth = (x.get_bits_raw() * y.get_bits_raw());

		kbi8 guess = (x * y);

		if (!(truth == guess.get_bits_raw())) {
			printf(
				"%3zu: %02X * %02X --> %02X != %02X\n",
				i, x.get_bits_raw(), y.get_bits_raw(), truth, guess.get_bits_raw()
			);
			TEST_FAILED();
			return false;
		}
	}

	for (size_t i = 0; i < test.random_test_count(); i++) {
		kbi8 x = test.get_rand();
		kbi8 y;
		y.set_value(16);

		kbi8 guess = (x * y);

		if (!(((guess.get_mask_raw() & 0xF) == 0xF) && ((guess.get_bits_raw() & 0xF) == 0x0))) {
			printf(
				"%3zu: %02X %02X\n",
				i, guess.get_bits_raw(), guess.get_mask_raw()
			);
			TEST_FAILED();
			return false;
		}
	}
	return true;
}

bool test_divrem_unsigned(kbi8_test& test) {
	for (size_t i = 0; i < test.random_test_count(); i++) {
		kbi8 x = test.get_rand_known();
		kbi8 y = test.get_rand_known();
		if (!y.isknown_nonzero()) {
			y.set_value(1);
		}

		uint8_t t_quo = (x.get_bits_raw() / y.get_bits_raw());
		uint8_t t_rem = (x.get_bits_raw() % y.get_bits_raw());

		kbi8 g_quo, g_rem;
		divrem_trunc_unsigned(g_quo, g_rem, x, y);

		if (!((t_quo == g_quo.get_bits_raw()) && (t_rem == g_rem.get_bits_raw()))) {
			printf(
				"%3zu: %02X / %02X --> %02X R %02X != %02X R %02X\n",
				i, x.get_bits_raw(), y.get_bits_raw(), t_quo, t_rem, g_quo.get_bits_raw(), g_rem.get_bits_raw()
			);
			TEST_FAILED();
			return false;
		}
	}
	for (size_t i = 0; i < test.random_test_count(); i++) {
		kbi8 x = test.get_rand();
		kbi8 y;
		y.set_value(18);

		kbi8 guess = div_trunc_unsigned(x, y);

		if (!(((guess.get_mask_raw() & 0xF0) == 0xF0) && ((guess.get_bits_raw() & 0xF0) == 0x00))) {
			printf(
				"%3zu: %02X %02X\n",
				i, guess.get_bits_raw(), guess.get_mask_raw()
			);
			TEST_FAILED();
			return false;
		}
	}
	return true;
}

bool test_reg_pair() {
	// set the random test count
	kbi8_test test(1000);
	bool passed = true;

	// run the tests
	passed &= test_add_with_carry(test);
	passed &= test_sub_with_carry(test);
	passed &= test_mul(test);
	passed &= test_logical(test);
	passed &= test_inc_dec(test);
	passed &= test_divrem_unsigned(test);

	return passed;
}



