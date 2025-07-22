#ifndef KNOWN_STATE_H
#define KNOWN_STATE_H

#include <vector>
#include <string.h>
#include "../ez80_instruction.h"
#include <bit>
#include "../ez80_instruction_info.h"
#include "../ez80_asm.h"

#include "../common_std.h"

using std::countl_zero;
using std::countl_one;
using std::countr_zero;
using std::countr_one;

inline int countl_zero(uint24_t x) {
	return countl_zero<uint32_t>(x);
}
inline int countl_one(uint24_t x) {
	return countl_one<uint32_t>(x);
}
inline int countr_zero(uint24_t x) {
	return countr_zero<uint32_t>(x);
}
inline int countr_one(uint24_t x) {
	return countr_one<uint32_t>(x);
}

inline int safe_clz(unsigned int x) {
	return __builtin_ffs(x) - 1;
}

using std::vector;

enum class ez80_flag_state {
	unknown,
	known_true,
	known_false
};

struct ez80_reg8_pair {
	uint8_t mask;
	uint8_t bits;
	bool observed;


	void set_observed(bool b) {
		observed = b;
	}
	bool get_observed() {
		return observed;
	}
	uint8_t get_bits() {
		return bits;
	}
	uint8_t get_mask() {
		return mask;
	}


	bool isknown_fully() const {
		return (mask + 1 == 0);
	}
	bool isknown_zero() const {
		return (isknown_fully() && (bits == 0));
	}
	bool isknown_nonzero() const {
		/* test if any bits are known to set */
		return ((mask & bits) != 0);
	}
	bool isknown_value(uint8_t x) const {
		return (isknown_fully() && (bits == x));
	}
	ez80_flag_state is_zero() const {
		using enum ez80_flag_state;
		if (isknown_zero()) {
			return known_true;
		}
		if (isknown_nonzero()) {
			return known_false;
		}
		return unknown;
	}
	ez80_flag_state is_nonzero() const {
		using enum ez80_flag_state;
		if (isknown_zero()) {
			return known_false;
		}
		if (isknown_nonzero()) {
			return known_true;
		}
		return unknown;
	}
	void set_unknown(bool observable = false) {
		mask = 0x00;
		bits = 0x00;
		if (observable) {
			observed = true;
		}
	}
	void set_destroyed() {
		set_unknown();
		observed = false;
	}
	void set_value(uint8_t x, bool observable = false) {
		bits = x;
		mask = 0xFF;
		observed = observable;
	}
	void set_value(ez80_reg8_pair x, bool observable = false) {
		bits = x.bits;
		mask = x.mask;
		observed = observable;
	}
	void increment() {
		int ffs = countr_one(mask) - 1;
		if (ffs < 0) {
			/* nothing is known */
			return;
		}
		mask &= (1u << ffs) - 1;
		bits++;
		bits &= mask;
	}
	void decrement() {
		int ffs = countr_one(mask) - 1;
		if (ffs < 0) {
			/* nothing is known */
			return;
		}
		mask &= (1u << ffs) - 1;
		bits++;
		bits &= mask;
	}
	ez80_flag_state shift_lift() {
		using enum ez80_flag_state;
		ez80_flag_state new_carry = unknown;
		if (mask & 0x80) {
			new_carry = (bits & 0x80) ? known_true : known_false;
		}
		mask <<= 1;
		mask |= 1;
		bits <<= 1;
		return new_carry;
	}
	ez80_flag_state shift_right_logical() {
		using enum ez80_flag_state;
		ez80_flag_state new_carry = unknown;
		if (mask & 0x01) {
			new_carry = (bits & 0x01) ? known_true : known_false;
		}
		mask >>= 1;
		mask |= 0x80;
		bits >>= 1;
		return new_carry;
	}
	ez80_flag_state shift_right_arithmetic() {
		using enum ez80_flag_state;
		ez80_flag_state new_carry = unknown;
		if (mask & 0x01) {
			new_carry = (bits & 0x01) ? known_true : known_false;
		}
		if (mask & 0x80) {
			mask >>= 1;
			mask |= 0x80;
			bits = (uint8_t)((int8_t)bits >> 1);
			return new_carry;
		}
		mask >>= 1;
		bits >>= 1;
		return new_carry;
	}
	ez80_flag_state rotate_left(ez80_flag_state carry) {
		using enum ez80_flag_state;
		ez80_flag_state new_carry = unknown;
		if (mask & 0x80) {
			new_carry = (bits & 0x80) ? known_true : known_false;
		}
		if (carry == unknown) {
			mask <<= 1;
			bits <<= 1;
		} else {
			mask <<= 1;
			mask |= 0x01;
			bits <<= 1;
			if (carry == known_true) {
				bits |= 0x01;
			}
		}
		return new_carry;
	}
	ez80_flag_state rotate_right(ez80_flag_state carry) {
		using enum ez80_flag_state;
		ez80_flag_state new_carry = unknown;
		if (mask & 1) {
			new_carry = (bits & 1) ? known_true : known_false;
		}
		if (carry == unknown) {
			mask >>= 1;
			bits >>= 1;
		} else {
			mask >>= 1;
			mask |= 0x80;
			bits >>= 1;
			if (carry == known_true) {
				bits |= 0x80;
			}
		}
		return new_carry;
	}
	ez80_flag_state rotate_carry_left() {
		using enum ez80_flag_state;
		ez80_flag_state new_carry = unknown;
		if (mask & 0x80) {
			new_carry = (bits & 0x80) ? known_true : known_false;
			if (bits & 0x80) {
				bits <<= 1;
				bits |= 0x01;
			} else {
				bits <<= 1;
			}
			mask <<= 1;
			mask |= 0x01;
		} else {
			mask <<= 1;
			bits <<= 1;
		}
		return new_carry;
	}
	ez80_flag_state rotate_carry_right() {
		using enum ez80_flag_state;
		ez80_flag_state new_carry = unknown;
		if (mask & 0x01) {
			new_carry = (bits & 0x01) ? known_true : known_false;
			if (bits & 0x01) {
				bits >>= 1;
				bits |= 0x80;
			} else {
				bits >>= 1;
			}
			mask >>= 1;
			mask |= 0x80;
		} else {
			mask >>= 1;
			bits >>= 1;
		}
		return new_carry;
	}
	void set_bit(uint8_t b) {
		mask |= (1 << b);
		bits |= (1 << b);
	}
	void res_bit(uint8_t b) {
		mask |= (1 << b);
		bits &= ~(1 << b);
	}
	ez80_flag_state test_bit(uint8_t b) {
		using enum ez80_flag_state;
		if (mask & (1 << b)) {
			return (bits & (1 << b)) ? known_true : known_false;
		}
		return unknown;
	}
	void complement() {
		bits = ~bits;
		bits &= mask;
	}
};

struct ez80_reg24_pair {
	ez80_reg8_pair pair[3];
	void set_observed(bool b) {
		pair[0].observed = b;
		pair[1].observed = b;
		pair[2].observed = b;
	}
	void set_unknown(bool observable = false) {
		pair[0].set_unknown(observable);
		pair[1].set_unknown(observable);
		pair[2].set_unknown(observable);
	}
	void set_destroyed() {
		pair[0].set_destroyed();
		pair[1].set_destroyed();
		pair[2].set_destroyed();
	}
	bool get_observed() {
		return (pair[0].observed || pair[1].observed || pair[2].observed);
	}
	bool isknown_24() {
		return (pair[0].isknown_fully() && pair[1].isknown_fully() && pair[2].isknown_fully());
	}
	bool isknown_16() {
		return (pair[0].isknown_fully() && pair[1].isknown_fully());
	}
	bool isknown_zero24() {
		return (pair[0].isknown_zero() && pair[1].isknown_zero() && pair[2].isknown_zero());
	}
	bool isknown_zero16() {
		return (pair[0].isknown_zero() && pair[1].isknown_zero());
	}
	bool isknown_nonzero24() {
		return (pair[0].isknown_nonzero() && pair[1].isknown_nonzero() && pair[2].isknown_nonzero());
	}
	bool isknown_nonzero16() {
		return (pair[0].isknown_nonzero() && pair[1].isknown_nonzero());
	}
	uint24_t get_bits() {
		return (pair[0].bits) | (pair[1].bits << 8) | (pair[2].bits << 16);
	}
	uint24_t get_mask() {
		return (pair[0].mask) | (pair[1].mask << 8) | (pair[2].mask << 16);
	}
	void set_bits(uint24_t x) {
		pair[0].bits = (uint8_t)((x >>  0) & 0xFF);
		pair[1].bits = (uint8_t)((x >>  8) & 0xFF);
		pair[2].bits = (uint8_t)((x >> 16) & 0xFF);
	}
	void set_mask(uint24_t x) {
		pair[0].bits = (uint8_t)((x >>  0) & 0xFF);
		pair[1].bits = (uint8_t)((x >>  8) & 0xFF);
		pair[2].bits = (uint8_t)((x >> 16) & 0xFF);
	}
	void set_bits(uint24_t x, uint24_t mask) {
		x &= mask;
		set_bits(x);
		set_mask(mask);
	}
	void set_value(uint24_t x, bool observable = false) {
		set_bits(x);
		set_mask(0xFFFFFF);
		set_observed(observable);
	}
	void set_value(ez80_reg24_pair x) {
		set_bits(x.get_bits());
		set_mask(x.get_mask());
		set_observed(false);
	}
	void unknown_upper() {
		pair[2].bits = 0;
		pair[2].mask = 0;
	}
	void clear_upper() {
		pair[2].bits = 0;
		pair[2].mask = 0xFF;
	}
	void increment24() {
		uint24_t mask = get_mask();
		int ffs = countr_one(mask) - 1;
		if (ffs < 0) {
			/* nothing is known */
			return;
		}
		mask &= (1u << ffs) - 1;
		uint24_t bits = get_bits();
		bits++;
		set_bits(bits, mask);
	}
	void decrement24() {
		uint24_t mask = get_mask();
		int ffs = countr_one(mask) - 1;
		if (ffs < 0) {
			/* nothing is known */
			return;
		}
		mask &= (1u << ffs) - 1;
		uint24_t bits = get_bits();
		bits--;
		set_bits(bits, mask);
	}
	void increment16() {
		uint24_t mask = get_mask();
		int ffs = countr_one(mask) - 1;
		if (ffs < 0) {
			clear_upper();
			/* nothing is known */
			return;
		}
		mask &= (1u << ffs) - 1;
		uint24_t bits = get_bits();
		bits++;
		clear_upper();
		set_bits(bits, mask);
	}
	void decrement16() {
		uint24_t mask = get_mask();
		int ffs = countr_one(mask) - 1;
		if (ffs < 0) {
			clear_upper();
			/* nothing is known */
			return;
		}
		mask &= (1u << ffs) - 1;
		uint24_t bits = get_bits();
		bits--;
		clear_upper();
		set_bits(bits, mask);
	}
	ez80_flag_state left_shift24(ez80_flag_state carry) {
		using enum ez80_flag_state;
		ez80_flag_state new_carry = unknown;
		uint24_t mask = get_mask();
		uint24_t bits = get_bits();
		if (mask & 0x800000) {
			new_carry = (bits & 0x800000) ? known_true : known_false;
		}
		if (carry == unknown) {
			mask <<= 1;
			bits <<= 1;
		} else {
			mask <<= 1;
			mask |= 0x01;
			bits <<= 1;
			if (carry == known_true) {
				bits |= 0x01;
			}
		}
		set_bits(bits, mask);
		return new_carry;
	}
	ez80_flag_state left_shift16(ez80_flag_state carry) {
		using enum ez80_flag_state;
		ez80_flag_state new_carry = unknown;
		uint24_t mask = get_mask();
		uint24_t bits = get_bits();
		if (mask & 0x8000) {
			new_carry = (bits & 0x8000) ? known_true : known_false;
		}
		if (carry == unknown) {
			mask <<= 1;
			bits <<= 1;
		} else {
			mask <<= 1;
			mask |= 0x01;
			bits <<= 1;
			if (carry == known_true) {
				bits |= 0x01;
			}
		}
		clear_upper();
		set_bits(bits, mask);
		return new_carry;
	}
	void multiply() {
		if (pair[0].isknown_fully() && pair[1].isknown_fully()) {
			uint24_t result = pair[0].bits * pair[1].bits;
			set_value(result);
			return;
		}
		if (pair[0].isknown_value(1) || pair[1].isknown_value(1)) {
			set_bits(0x000000, 0xFFFF00);
			return;
		}
		if (pair[0].isknown_value(0) || pair[1].isknown_value(0)) {
			set_value(0);
			return;
		}
		set_bits(0x000000, 0xFF0000);
	}
};

struct ez80_state {
	public:
	ez80_reg24_pair HL;
	ez80_reg24_pair BC;
	ez80_reg24_pair DE;
	ez80_reg24_pair IX;
	ez80_reg24_pair IY;
	ez80_reg24_pair SP;
	ez80_reg8_pair A;

	uint24_t HL_mask;
	uint24_t HL_bits;
	uint24_t DE_mask;
	uint24_t DE_bits;
	uint24_t BC_mask;
	uint24_t BC_bits;
	uint24_t IX_mask;
	uint24_t IX_bits;
	uint24_t IY_mask;
	uint24_t IY_bits;
	uint8_t A_mask;
	uint8_t A_bits;
	ez80_flags known_flags;
	ez80_flags flags;

	private:
	ez80_flag_state flag_getter(bool known, bool flag) {
		if (known) {
			return flag ? ez80_flag_state::known_true : ez80_flag_state::known_false;
		}
		return ez80_flag_state::unknown;
	}
	void flag_setter(ez80_flag_state f, uint8_t mask) {
		using enum ez80_flag_state;
		if (f == unknown) {
			known_flags.raw &= ~mask;
			flags.raw &= ~mask;
			return;
		}
		known_flags.raw |= mask;
		if (f == known_true) {
			flags.raw |= mask;
			return;
		}
		flags.raw &= ~mask;
	}
	int count_known_reg_bits(uint24_t reg) {
		uint32_t r = (uint32_t)reg;
		return std::popcount(r);
	}
	int count_known_reg_bits(uint8_t reg) {
		return std::popcount(reg);
	}
	void set_reg(uint24_t& mask, uint24_t& reg, uint24_t value) {
		mask = 0xFFFFFF;
		reg = value;
	}
	void set_reg(uint8_t& mask, uint8_t& reg, uint8_t value) {
		mask = 0xFF;
		reg = value;
	}

	public:
	void state_unknown() {
		memset(this, 0, sizeof(ez80_state));
	}
	ez80_state() {
		state_unknown();
	}

	ez80_flag_state invert_flag(ez80_flag_state f) {
		using enum ez80_flag_state;
		if (f == unknown) {
			return unknown;
		}
		if (f == known_true) {
			return known_false;
		}
		return known_true;
	}

	/* unknown state */

	void flags_unknown() {
		known_flags.raw = 0;
		flags.raw = 0;
	}
	void carry_unknown() {
		known_flags.carry = 0;
		flags.carry = 0;
	}
	void zero_unknown() {
		known_flags.zero = 0;
		flags.zero = 0;
	}
	void sign_unknown() {
		known_flags.sign = 0;
		flags.sign = 0;
	}
	void overflow_unknown() {
		known_flags.overflow = 0;
		flags.overflow = 0;
	}
	void addsub_unknown() {
		known_flags.addsub = 0;
		flags.addsub = 0;
	}
	void half_unknown() {
		known_flags.half = 0;
		flags.half = 0;
	}

	void only_carry_preserved() {
		known_flags.raw &= (1 << 0);
		flags.raw &= (1 << 0);
	}

	void registers_unknown() {
		HL.set_unknown();
		DE.set_unknown();
		BC.set_unknown();
		IX.set_unknown();
		IY.set_unknown();
		A.set_unknown();
	}

	/* setters */

	void set_carry(bool f) {
		known_flags.carry = f;
		flags.carry = f;
	}
	void set_zero(bool f) {
		known_flags.zero = 1;
		flags.zero = f;
	}
	void set_sign(bool f) {
		known_flags.sign = 1;
		flags.sign = f;
	}
	void set_overflow(bool f) {
		known_flags.overflow = 1;
		flags.overflow = f;
	}
	void set_addsub(bool f) {
		known_flags.addsub = 1;
		flags.addsub = f;
	}
	void set_half(bool f) {
		known_flags.half = 1;
		flags.half = f;
	}
	void set_carry(ez80_flag_state f) {
		flag_setter(f, (1 << 0));
	}
	void set_zero(ez80_flag_state f) {
		flag_setter(f, (1 << 6));
	}
	void set_sign(ez80_flag_state f) {
		flag_setter(f, (1 << 7));
	}
	void set_overflow(ez80_flag_state f) {
		flag_setter(f, (1 << 5));
	}
	void set_addsub(ez80_flag_state f) {
		flag_setter(f, (1 << 1));
	}
	void set_half(ez80_flag_state f) {
		flag_setter(f, (1 << 3));
	}

	/* getters */

	ez80_flag_state get_carry() {
		return flag_getter(known_flags.carry, flags.carry);
	}
	ez80_flag_state get_zero() {
		return flag_getter(known_flags.zero, flags.zero);
	}
	ez80_flag_state get_sign() {
		return flag_getter(known_flags.sign, flags.sign);
	}
	ez80_flag_state get_overflow() {
		return flag_getter(known_flags.overflow, flags.overflow);
	}
	ez80_flag_state get_addsub() {
		return flag_getter(known_flags.addsub, flags.addsub);
	}
	ez80_flag_state get_half() {
		return flag_getter(known_flags.half, flags.half);
	}

	/* query */

	bool isknown_carry_set() {
		return (known_flags.carry && flags.carry);
	}
	bool isknown_carry_clear() {
		return (known_flags.carry && !flags.carry);
	}

	bool isknown_carry() {
		return known_flags.carry;
	}
	bool isknown_zero() {
		return known_flags.zero;
	}
	bool isknown_sign() {
		return known_flags.sign;
	}
	bool isknown_overflow() {
		return known_flags.overflow;
	}
	bool isknown_addsub() {
		return known_flags.addsub;
	}
	bool isknown_half() {
		return known_flags.half;
	}

	/* reg width */
	int get_reg_width(ez80_reg x) {
		switch (x) {
			case ez80_reg::A:
			case ez80_reg::B:
			case ez80_reg::C:
			case ez80_reg::D:
			case ez80_reg::E:
			case ez80_reg::H:
			case ez80_reg::L:
			case ez80_reg::IXL:
			case ez80_reg::IXH:
			case ez80_reg::IYL:
			case ez80_reg::IYH:
			case ez80_reg::UBC:
			case ez80_reg::UDE:
			case ez80_reg::UHL:
			case ez80_reg::UIX:
			case ez80_reg::UIY:
				return 8;
			case ez80_reg::BC:
			case ez80_reg::DE:
			case ez80_reg::HL:
			case ez80_reg::IX:
			case ez80_reg::IY:
			case ez80_reg::SP:
				return 24;
			default:
				return 0;
		}
	}

	ez80_reg8_pair& get_reg8(ez80_reg x) {
		switch (x) {
			case ez80_reg::A: return A;
			case ez80_reg::B: return BC.pair[1];
			case ez80_reg::C: return BC.pair[0];
			case ez80_reg::D: return DE.pair[1];
			case ez80_reg::E: return DE.pair[0];
			case ez80_reg::H: return HL.pair[1];
			case ez80_reg::L: return HL.pair[0];
			case ez80_reg::IXH: return IX.pair[1];
			case ez80_reg::IXL: return IX.pair[0];
			case ez80_reg::IYH: return IY.pair[1];
			case ez80_reg::IYL: return IY.pair[0];
			default:
				return A;
		}
	}

	/* get register */

	ez80_reg24_pair& get_reg24(ez80_reg x) {
		switch (x) {
			case ez80_reg::HL: return HL;
			case ez80_reg::BC: return BC;
			case ez80_reg::DE: return DE;
			case ez80_reg::IX: return IX;
			case ez80_reg::IY: return IY;
			case ez80_reg::SP: return SP;
			default:
				return HL;
		}
	}

	void load_reg8(ez80_reg dst, ez80_reg src) {
		ez80_reg8_pair x = get_reg8(dst);
		ez80_reg8_pair y = get_reg8(src);
		y.observed = true;
		x.set_value(y);
	}

	void load_reg24(ez80_reg dst, ez80_reg src) {
		ez80_reg24_pair x = get_reg24(dst);
		ez80_reg24_pair y = get_reg24(src);
		y.set_observed(true);
		x.set_value(y);
	}

	void set_reg8(ez80_reg dst, uint8_t value) {
		get_reg8(dst).set_value(value);
	}

	void set_reg24(ez80_reg dst, uint24_t value) {
		get_reg24(dst).set_value(value);
	}

	void test_bit(ez80_reg dst, uint8_t b) {
		set_zero(get_reg8(dst).test_bit(b));
		sign_unknown();
		overflow_unknown();
		set_half(true);
		set_addsub(false);
	}

	void add_reg24(ez80_reg dst, ez80_reg src) {
		ez80_reg24_pair x = get_reg24(dst);
		ez80_reg24_pair y = get_reg24(src);
		x.set_unknown();
		y.set_observed(true);
		carry_unknown();
	}
	void add_reg16(ez80_reg dst, ez80_reg src) {
		ez80_reg24_pair x = get_reg24(dst);
		ez80_reg24_pair y = get_reg24(src);
		x.set_unknown();
		x.clear_upper();
		y.set_observed(true);
		carry_unknown();
	}
	void adc_reg24(ez80_reg dst, ez80_reg src) {
		ez80_reg24_pair x = get_reg24(dst);
		ez80_reg24_pair y = get_reg24(src);
		x.set_unknown();
		y.set_observed(true);
		flags_unknown();
	}
	void adc_reg16(ez80_reg dst, ez80_reg src) {
		ez80_reg24_pair x = get_reg24(dst);
		ez80_reg24_pair y = get_reg24(src);
		x.set_unknown();
		x.clear_upper();
		y.set_observed(true);
		flags_unknown();
	}
	void sbc_reg24(ez80_reg dst, ez80_reg src) {
		ez80_reg24_pair x = get_reg24(dst);
		ez80_reg24_pair y = get_reg24(src);
		x.set_unknown();
		y.set_observed(true);
		flags_unknown();
	}
	void sbc_reg16(ez80_reg dst, ez80_reg src) {
		ez80_reg24_pair x = get_reg24(dst);
		ez80_reg24_pair y = get_reg24(src);
		x.set_unknown();
		x.clear_upper();
		y.set_observed(true);
		flags_unknown();
	}

	void accumulate_arithmetic(ez80_reg src) {
		using enum ez80_reg;
		get_reg8(A).set_unknown();
		get_reg8(src).observed = true;
		flags_unknown();
	}
	void accumulate_logical(ez80_reg src) {
		using enum ez80_reg;
		get_reg8(A).set_unknown();
		get_reg8(src).observed = true;
		flags_unknown();
		set_carry(false);
	}
	void complement() {
		using enum ez80_reg;
		get_reg8(A).complement();
		set_addsub(true);
		set_half(true);
	}

	void negate() {
		using enum ez80_reg;
		ez80_reg8_pair& acc = get_reg8(A);
		if (acc.isknown_zero()) {
			set_carry(false);
			set_zero(true);
			set_overflow(false);
			set_sign(false);
		} else if (acc.isknown_nonzero()) {
			set_carry(true);
			set_zero(false);
			set_overflow(true);
			if (acc.isknown_value(128)) {
				set_sign(true);
			} else {
				set_sign(invert_flag(acc.test_bit(7)));
			}
		}
		acc.complement();
		acc.increment();
	}

	void sbc_a_a() {
		using enum ez80_reg;
		using enum ez80_flag_state;
		ez80_reg8_pair& acc = get_reg8(A);
		acc.set_destroyed();
		if (isknown_carry_set()) {
			set_zero(false);
			set_sign(true);
			set_overflow(true);
			acc.set_value(0xFF);
		} else if (isknown_carry_clear()) {
			set_zero(true);
			set_sign(false);
			set_overflow(false);
			acc.set_value(0x00);
		}
		zero_unknown();
		sign_unknown();
		overflow_unknown();
	}
	void sbc_hl_hl() {
		using enum ez80_reg;
		using enum ez80_flag_state;
		ez80_reg24_pair& hl = get_reg24(HL);
		hl.set_destroyed();
		if (isknown_carry_clear()) {
			set_zero(true);
			set_sign(false);
			set_overflow(false);
			hl.set_value(0x000000);
			return;
		}
		if (isknown_carry_set()) {
			set_zero(false);
			set_sign(true);
			set_overflow(true);
			hl.set_value(0xFFFFFF);
			return;
		}
		zero_unknown();
		sign_unknown();
		overflow_unknown();
	}

	void lea_reg24(ez80_reg dst, ez80_reg src, int8_t value) {
		ez80_reg24_pair x = get_reg24(dst);
		ez80_reg24_pair y = get_reg24(src);
		y.set_observed(true);
		if (value == 0) {
			x.set_value(y);
			return;
		}
		if (y.isknown_24()) {
			x.set_value(y.get_bits() + (int24_t)value);
			return;
		}
		x.set_unknown();
	}

	void lea_reg16(ez80_reg dst, ez80_reg src, int8_t value) {
		ez80_reg24_pair x = get_reg24(dst);
		ez80_reg24_pair y = get_reg24(src);
		y.set_observed(true);
		if (value == 0) {
			x.set_value(y);
			x.clear_upper();
			return;
		}
		if (y.isknown_16()) {
			x.set_value(y.get_bits() + (int24_t)value);
			x.clear_upper();
			return;
		}
		x.set_unknown();
	}

	
	void crt_ishl() {
		using enum ez80_reg;
		ez80_reg24_pair hl = get_reg24(HL);
		ez80_reg8_pair c = get_reg8(C);
		c.set_observed(true);
		if (!c.isknown_fully()) {
			hl.set_unknown();
			return;
		}
		uint8_t shift = c.get_bits();
		if (shift == 0) {
			return;
		}
		if (shift >= 24) {
			hl.set_unknown();
			return;
		}
		uint24_t bits = hl.get_bits();
		uint24_t mask = hl.get_mask();
		bits <<= shift;
		mask = ~(~mask << shift);
		hl.set_bits(bits, mask);
	}

	void crt_ishru() {
		using enum ez80_reg;
		ez80_reg24_pair hl = get_reg24(HL);
		ez80_reg8_pair c = get_reg8(C);
		c.set_observed(true);
		if (!c.isknown_fully()) {
			hl.set_unknown();
			return;
		}
		uint8_t shift = c.get_bits();
		if (shift == 0) {
			return;
		}
		if (shift >= 24) {
			hl.set_unknown();
			return;
		}
		uint24_t bits = hl.get_bits();
		uint24_t mask = hl.get_mask();
		bits >>= shift;
		mask = ~(~mask >> shift);
		hl.set_bits(bits, mask);
	}

	void crt_sshl() {
		using enum ez80_reg;
		ez80_reg24_pair hl = get_reg24(HL);
		ez80_reg8_pair c = get_reg8(C);
		c.set_observed(true);
		if (!c.isknown_fully()) {
			hl.set_unknown();
			return;
		}
		uint8_t shift = c.get_bits();
		if (shift == 0) {
			hl.unknown_upper();
			return;
		}
		if (shift >= 16) {
			hl.set_unknown();
			return;
		}
		uint24_t bits = hl.get_bits();
		uint24_t mask = hl.get_mask();
		bits <<= shift;
		mask = ~(~mask << shift);
		hl.set_bits(bits, mask);
		hl.unknown_upper();
	}

	void crt_sshru() {
		using enum ez80_reg;
		ez80_reg24_pair hl = get_reg24(HL);
		ez80_reg8_pair c = get_reg8(C);
		c.set_observed(true);
		if (!c.isknown_fully()) {
			hl.set_unknown();
			return;
		}
		uint8_t shift = c.get_bits();
		if (shift == 0) {
			hl.unknown_upper();
			return;
		}
		if (shift >= 16) {
			hl.set_unknown();
			return;
		}
		hl.clear_upper();
		uint24_t bits = hl.get_bits();
		uint24_t mask = hl.get_mask();
		bits >>= shift;
		mask = ~(~mask >> shift);
		hl.set_bits(bits, mask);
		hl.unknown_upper();
	}

};

class ez80_known_state {
	vector<ez80_state> history;
	void next_instruction(ez80_op_code op_code, uint24_t value = 0);
	void next_known_function(ez80_known_function func);
};

#endif /* KNOWN_STATE_H */
