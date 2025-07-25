#ifndef CURRENT_STATE_HPP
#define CURRENT_STATE_HPP

#include "../ez80_type.h"
#include "../common_std.h"
#include "../ez80_reg.h"
#include "../ez80_instruction.h"
#include "../ez80_known_function.h"
#include "../ez80_parse.h"

#include "reg_pair.hpp"

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

struct reg48_pair : public reg_pair<uint48_t> {
	reg48_pair() {
		bits = 0;
		mask = 0;
	}
	reg48_pair(reg_pair<uint48_t> src) {
		bits = src.bits;
		mask = src.mask;
	}
	reg24_pair get_hi24() const {
		reg24_pair ret;
		ret.bits = (uint24_t)(bits >> 24);
		ret.mask = (uint24_t)(mask >> 24);
		return ret;
	}
	reg24_pair get_lo24() const {
		reg24_pair ret;
		ret.bits = (uint24_t)(bits);
		ret.mask = (uint24_t)(mask);
		return ret;
	}
	void set_value(reg24_pair hi, reg24_pair lo) {
		bits = ((uint48_t)hi.bits << 24) | lo.bits;
		mask = ((uint48_t)hi.mask << 24) | lo.mask;
	}
	void split_value(reg24_pair& hi, reg24_pair& lo) {
		hi = get_hi24();
		lo = get_lo24();
	}
};

struct reg64_pair : public reg_pair<uint64_t> {
	reg64_pair() {
		bits = 0;
		mask = 0;
	}
	reg64_pair(reg_pair<uint64_t> src) {
		bits = src.bits;
		mask = src.mask;
	}
	reg16_pair get_upper16() const {
		reg16_pair ret;
		ret.bits = (uint16_t)(bits >> 48);
		ret.mask = (uint16_t)(mask >> 48);
		return ret;
	}
	reg24_pair get_hi24() const {
		reg24_pair ret;
		ret.bits = (uint24_t)(bits >> 24);
		ret.mask = (uint24_t)(mask >> 24);
		return ret;
	}
	reg24_pair get_lo24() const {
		reg24_pair ret;
		ret.bits = (uint24_t)(bits);
		ret.mask = (uint24_t)(mask);
		return ret;
	}
	void set_value(reg16_pair up, reg24_pair hi, reg24_pair lo) {
		bits = ((uint64_t)up.bits << 48) | ((uint64_t)hi.bits << 24) | lo.bits;
		mask = ((uint64_t)up.mask << 48) | ((uint64_t)hi.mask << 24) | lo.mask;
	}
	void split_value(reg16_pair& up, reg24_pair& hi, reg24_pair& lo) {
		up = get_upper16();
		hi = get_hi24();
		lo = get_lo24();
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


enum class reg_pointer_type {
	UNKNOWN,
	VALUE,
	STACK,
};

class reg_pointer {
public:
	int offset;
	reg_pointer_type type;
	void set_invalid() {
		using enum reg_pointer_type;
		type = UNKNOWN;
	}

	bool is_valid() const {
		using enum reg_pointer_type;
		switch (type) {
			case STACK:
			{ return true; } break;
			default:
			{ return false; } break;
		}
	}

	void set_value(int value) {
		using enum reg_pointer_type;
		type = VALUE;
		offset = value;
	}
	void set_value(reg24_pair value) {
		if (!value.isknown_fully()) {
			set_invalid();
			return;
		}
		set_value(static_cast<int>(static_cast<int24_t>(value.bits)));
	}
	void add_sp() {
		using enum reg_pointer_type;
		switch (type) {
			case VALUE: {
				type = STACK;
			} break;
			default: {
				set_invalid();
			} break;
		}
	}
	void modify_offset(int value) {
		offset += value;
	}
	void modify_offset(reg8_pair value) {
		if (!value.isknown_fully()) {
			set_invalid();
			return;
		}
		modify_offset(static_cast<int>(static_cast<int8_t>(value.bits)));
	}
	void modify_offset(reg24_pair value) {
		if (!value.isknown_fully()) {
			set_invalid();
			return;
		}
		modify_offset(static_cast<int>(static_cast<int24_t>(value.bits)));
	}
	reg_pointer_type get_type() const {
		return type;
	}
	void lea_copy(reg_pointer src, int src_offset = 0) {
		*this = src;
		this->modify_offset(src_offset);
	}
	void lea_copy(reg_pointer src, reg8_pair src_offset) {
		*this = src;
		this->modify_offset(src_offset);
	}
};

class stack_frame {
	static constexpr size_t stack_size = 60;
	static_assert(stack_size >= 15, "invalid stack size");
	reg8_pair stack_data[stack_size];
	int base_offset;

	/* reg data */
public:
	int IX_offset;
	int IY_offset;
	int HL_offset;
	int DE_offset;
	int BC_offset;

	bool IX_valid;
	bool IY_valid;
	bool HL_valid;
	bool DE_valid;
	bool BC_valid;

	/* internal */
private:

	bool adjust_index(int& index) const {
		index = index - base_offset;
		if (!(index >= 0 && static_cast<size_t>(index) < stack_size)) {
			return false;
		}
		return true;
	}

public:
	void write(reg8_pair src, int index) {
		if (!adjust_index(index)) {
			return;
		}
		stack_data[index] = src;
	}

	void write_base(reg8_pair src, size_t index) {
		if (!(index < stack_size)) {
			return;
		}
		stack_data[index] = src;
	}

	reg8_pair read(int index) const {
		reg8_pair ret;
		if (!adjust_index(index)) {
			ret.set_unknown();
			return ret;
		}
		ret = stack_data[index];
		return ret;
	}

	reg8_pair read_base(size_t index) const {
		reg8_pair ret;
		if (!(index < stack_size)) {
			ret.set_unknown();
			return ret;
		}
		ret = stack_data[index];
		return ret;
	}

public:

	void set_stack_invalid() {
		for (size_t i = 0; i < stack_size; i++) {
			stack_data[i].set_unknown();
		}
		IX_offset = 0;
		IY_offset = 0;
		HL_offset = 0;
		DE_offset = 0;
		BC_offset = 0;
		IX_valid = false;
		IY_valid = false;
		HL_valid = false;
		DE_valid = false;
		BC_valid = false;
	}

	stack_frame() {
		set_stack_invalid();
	}

private:

	reg24_pair load24(int offset, int reg_offset, bool reg_valid) const {
		reg24_pair ret;
		if (reg_valid == false) {
			ret.set_unknown();
			return ret;
		}
		int index = offset + reg_offset;
		ret.set_value(
			read(index + 2),
			read(index + 1),
			read(index + 0)
		);
		return ret;
	}
	reg8_pair load8(int offset, int reg_offset, bool reg_valid) const {
		reg8_pair ret;
		if (reg_valid == false) {
			ret.set_unknown();
			return ret;
		}
		int index = offset + reg_offset;
		return read(index);
	}
	void store24(reg24_pair src, int offset, int reg_offset, bool reg_valid) {
		if (reg_valid == false) {
			// we don't know where this write occured
			set_stack_invalid();
			return;
		}
		int index = offset + reg_offset;
		write(src.get_lo()   , index + 0);
		write(src.get_hi()   , index + 1);
		write(src.get_upper(), index + 2);
	}
	void store8(reg8_pair src, int offset, int reg_offset, bool reg_valid) {
		if (reg_valid == false) {
			// we don't know where this write occured
			set_stack_invalid();
			return;
		}
		int index = offset + reg_offset;
		write(src, index);
	}

	/* stack operations */
public:

	void set_stack_base_unknown(size_t bytes) {
		for (size_t i = 0; i < bytes && i < stack_size; i++) {
			stack_data[i].set_unknown();
		}
	}

	void decrement() {
		base_offset++;
		for (size_t i = stack_size; i --> 1;) {
			stack_data[i] = stack_data[i - 1];
		}
		stack_data[0].set_unknown();
	}
	void increment() {
		base_offset--;
		for (size_t i = 1; i < stack_size; i++) {
			stack_data[i - 1] = stack_data[i];
		}
		stack_data[stack_size - 1].set_unknown();
	}
	void push(reg24_pair src) {
		base_offset += 3;
		for (size_t i = stack_size; i --> 3;) {
			stack_data[i] = stack_data[i - 3];
		}
		stack_data[0] = src.get_lo();
		stack_data[1] = src.get_hi();
		stack_data[2] = src.get_upper();
	}
	reg24_pair pop() {
		base_offset -= 3;
		reg24_pair ret;
		ret.set_value(stack_data[2], stack_data[1], stack_data[0]);
		for (size_t i = 3; i < stack_size; i++) {
			stack_data[i - 3] = stack_data[i];
		}
		stack_data[stack_size - 3].set_unknown();
		stack_data[stack_size - 2].set_unknown();
		stack_data[stack_size - 1].set_unknown();
		return ret;
	}
	reg24_pair exchange(reg24_pair arg) {
		reg24_pair ret;
		ret.set_value(stack_data[2], stack_data[1], stack_data[0]);
		stack_data[0] = arg.get_lo();
		stack_data[1] = arg.get_hi();
		stack_data[2] = arg.get_upper();
		return ret;
	}

	void load_SP_reg(int offset, bool valid) {
		if (valid == false) {
			set_stack_invalid();
			return;
		}
		for (int i = 0; i < offset; i++) {
			increment();
		}
	}

/* others */

	void set_IX(int offset) {
		IX_offset = offset;
		IX_valid = true;
	}
	void modifiy_IX(int change) {
		IX_offset += change;
	}
	void invalidate_IX() {
		IX_valid = false;
	}
	void load_SP_IX() {
		load_SP_reg(IX_offset, IX_valid);
	}
	reg24_pair load24_IX(int offset) const {
		return load24(offset, IX_offset, IX_valid);
	}
	reg8_pair load8_IX(int offset) const {
		return load8(offset, IX_offset, IX_valid);
	}
	void store24_IX(reg24_pair src, int offset) {
		store24(src, offset, IX_offset, IX_valid);
	}
	void store8_IX(reg8_pair src, int offset) {
		store8(src, offset, IX_offset, IX_valid);
	}

	reg8_pair read8(reg_pointer ptr, int offset) {
		return load8(offset, ptr.offset, true);
	}
	reg24_pair read24(reg_pointer ptr, int offset) {
		return load24(offset, ptr.offset, true);
	}
	void write8(reg8_pair src, reg_pointer ptr, int offset) {
		store8(src, offset, ptr.offset, true);
	}
	void write24(reg24_pair src, reg_pointer ptr, int offset) {
		store24(src, offset, ptr.offset, true);
	}

	void frameset(int offset) {
		/*
		| sp + 3 = ret
		| sp + 0 = old IX 
		| sp - n = data
		| sp - m = temp
		*/
		// offset by 3 to store IX technically
		set_stack_invalid();
		reg24_pair ret_addr;
		ret_addr.set_unknown();
		exchange(ret_addr);
		reg24_pair ix_val;
		ix_val.set_unknown();
		push(ix_val);
		for (int i = 0; i < -offset; i++) {
			decrement();
		}

		base_offset = offset;
		set_IX(0);
	}
	void frameset0() {
		frameset(0);
	}
};

class current_state {
	public:

	ez80_instruction previous_instruction;
	ez80_known_function previous_known_func;
	bool previous_was_known_func;

	void set_previous_instruction(ez80_instruction instruction) {
		previous_instruction = instruction;
		previous_was_known_func = false;
	}
	void set_previous_instruction(ez80_instruction instruction, ez80_known_function known_func) {
		previous_instruction = instruction;
		previous_known_func = known_func;
		previous_was_known_func = true;
	}

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

	stack_frame stack;

	reg_pointer PHL;
	reg_pointer PDE;
	reg_pointer PBC;
	reg_pointer PIX;
	reg_pointer PIY;

	struct full_reg {
		reg_pointer ptr;
		reg24_pair value;
	};

	full_reg HL;
	full_reg DE;
	full_reg BC;
	full_reg IX;
	full_reg IY;

	full_reg get_full_HL() {
		full_reg ret;
		ret.ptr = PHL;
		ret.value = get_HL();
		return ret;
	}
	full_reg get_full_DE() {
		full_reg ret;
		ret.ptr = PDE;
		ret.value = get_DE();
		return ret;
	}
	full_reg get_full_BC() {
		full_reg ret;
		ret.ptr = PBC;
		ret.value = get_BC();
		return ret;
	}
	full_reg get_full_IX() {
		full_reg ret;
		ret.ptr = PIX;
		ret.value = get_IX();
		return ret;
	}
	full_reg get_full_IY() {
		full_reg ret;
		ret.ptr = PIY;
		ret.value = get_IY();
		return ret;
	}

	void update_PHL() {
		PHL.set_value(get_HL());
	}
	void update_PDE() {
		PDE.set_value(get_DE());
	}
	void update_PBC() {
		PBC.set_value(get_BC());
	}
	void update_PIX() {
		PIX.set_value(get_IX());
	}
	void update_PIY() {
		PIY.set_value(get_IY());
	}

	void set_pointers_invalid() {
		PHL.set_invalid();
		PDE.set_invalid();
		PBC.set_invalid();
		PIX.set_invalid();
		PIY.set_invalid();
		invalidate_stack();
	}

	void unknown_write() {
		set_pointers_invalid();
	}

	void unknown_read() {
		set_pointers_invalid();
	}

	void lost_control_of_pointer() {
		set_pointers_invalid();
	}

	reg8_pair read8(reg_pointer& ptr, int offset = 0) {
		reg8_pair ret;
		using enum reg_pointer_type;
		switch (ptr.get_type()) {
			case STACK: {
				ret.set_value(stack.read8(ptr, offset));
			} break;
			default: {
				ret.set_unknown();
				unknown_read();
			} break;
		}
		return ret;
	}
	reg8_pair read8(reg_pointer& ptr, reg8_pair offset) {
		reg8_pair ret;
		if (!offset.isknown_fully()) {
			ret.set_unknown();
			unknown_read();
			return ret;
		}
		return read8(ptr, static_cast<int>(static_cast<int8_t>(offset.bits)));
	}
	reg24_pair read24(reg_pointer& ptr, int offset = 0) {
		reg24_pair ret;
		using enum reg_pointer_type;
		switch (ptr.get_type()) {
			case STACK: {
				ret.set_value(stack.read24(ptr, offset));
			} break;
			default: {
				ret.set_unknown();
				unknown_read();
			} break;
		}
		return ret;
	}
	reg24_pair read24(reg_pointer& ptr, reg8_pair offset) {
		reg24_pair ret;
		if (!offset.isknown_fully()) {
			ret.set_unknown();
			unknown_read();
			return ret;
		}
		return read24(ptr, static_cast<int>(static_cast<int8_t>(offset.bits)));
	}

	void write8(reg8_pair src, reg_pointer& ptr, int offset = 0) {
		using enum reg_pointer_type;
		switch (ptr.get_type()) {
			case STACK: {
				stack.write8(src, ptr, offset);
			} break;
			default: {
				unknown_write();
			} break;
		}
	}

	void write8(reg8_pair src, reg_pointer& ptr, reg8_pair offset) {
		if (!offset.isknown_fully()) {
			unknown_write();
			return;
		}
		write8(src, ptr, static_cast<int>(static_cast<int8_t>(offset.bits)));
	}

	void write24(reg24_pair src, reg_pointer& src_ptr, reg_pointer& ptr, int offset = 0) {
		using enum reg_pointer_type;
		if (src_ptr.is_valid()) {
			lost_control_of_pointer();
		}
		switch (ptr.get_type()) {
			case STACK: {
				stack.write24(src, ptr, offset);
			} break;
			default: {
				unknown_write();
			} break;
		}
	}

	void write24(reg24_pair src, reg_pointer& src_ptr, reg_pointer& ptr, reg8_pair offset) {
		if (!offset.isknown_fully()) {
			if (src_ptr.is_valid()) {
				lost_control_of_pointer();
			}
			unknown_write();
			return;
		}
		write24(src, src_ptr, ptr, static_cast<int>(static_cast<int8_t>(offset.bits)));
	}

	static constexpr size_t stack_size = 12;
	static_assert(stack_size >= 9, "invalid stack size");
	reg8_pair STACK[stack_size];

	void invalidate_stack() {
		stack.set_stack_invalid();
		for (size_t i = 0; i < stack_size; i++) {
			STACK[i].set_unknown();
		}
	}

	void set_previous_instructions_invalid() {
		using enum ez80_op_code;
		previous_instruction.op_code = UNKNOWN;
		previous_was_known_func = false;
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

	void set_just_libc_reg_unknown(size_t stack_used) {
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
		F.set_flags_unknown();
		stack.set_stack_base_unknown(stack_used);
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

/* stack operations */

	void pea_stack(reg24_pair src, reg8_pair offset) {
		reg24_pair dst;
		reg24_pair extend;
		extend.set_sign_extend(offset);
		dst = (src + extend);
		stack.push(dst);
	}

/* instructions */

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
		merge_assuming_bitwise_and_is_zero(A, arg);
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
		merge_assuming_bitwise_xor_is_zero(A, arg);
	}
	void acc_cp_was_nc(reg8_pair& arg) {
		// (A >= arg)
		reg8_pair arg_mask, acc_mask;
		acc_mask.set_to_unsigned_range(
			arg.get_unsigned_minimum(),
			A.get_unsigned_maximum()
		);
		arg_mask.set_to_unsigned_range(
			arg.get_unsigned_minimum(),
			A.get_unsigned_maximum()
		);
		A.merge_bits(acc_mask);
		arg.merge_bits(arg_mask);
	}
	void acc_cp_was_c(reg8_pair& arg) {
		// (A < arg) && (arg >= 1) && (A < 255)
		// A is [0, 254]
		// arg is [1, 255]
		reg8_pair arg_mask, acc_mask;
		F.set_zero(false);
		acc_mask.set_to_unsigned_range(
			A.get_unsigned_minimum(),
			arg.get_unsigned_maximum() - 1
		);
		arg_mask.set_to_unsigned_range(
			A.get_unsigned_minimum() + 1,
			arg.get_unsigned_maximum()
		);
		A.merge_bits(acc_mask);
		arg.merge_bits(arg_mask);
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

	reg48_pair get48_UDEUHL() const {
		reg48_pair ret;
		ret.set_value(get_DE(), get_HL());
		return ret;
	}

	reg48_pair get48_UIYUBC() const {
		reg48_pair ret;
		ret.set_value(get_IY(), get_BC());
		return ret;
	}

	reg64_pair get64_BCUDEUHL() const {
		reg64_pair ret;
		ret.set_value(get16_BC(), get_DE(), get_HL());
		return ret;
	}

/* retrive from stack */

	reg8_pair get8_STACK(size_t offset = 0) const {
		return stack.read_base(offset);
	}

	reg16_pair get16_STACK(size_t offset = 0) const {
		reg16_pair ret;
		ret.set_value(stack.read_base(offset + 1), stack.read_base(offset + 0));
		return ret;
	}

	reg24_pair get24_STACK(size_t offset = 0) const {
		reg24_pair ret;
		ret.set_value(
			stack.read_base(offset + 2),
			stack.read_base(offset + 1),
			stack.read_base(offset + 0)
		);
		return ret;
	}

	reg32_pair get32_STACK(size_t offset = 0) const {
		reg32_pair ret;
		ret.set_value(
			stack.read_base(offset + 3),
			stack.read_base(offset + 2),
			stack.read_base(offset + 1),
			stack.read_base(offset + 0)
		);
		return ret;
	}

	reg48_pair get48_STACK(size_t offset = 0) const {
		reg48_pair ret;
		ret.set_value(get24_STACK(offset + 3), get24_STACK(offset + 0));
		return ret;
	}

	reg64_pair get64_STACK(size_t offset = 0) const {
		reg64_pair ret;
		ret.set_value(get16_STACK(offset + 6), get24_STACK(offset + 3), get24_STACK(offset + 0));
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
		update_PHL();
	}
	void set_DE(reg24_pair val) {
		val.split_value(UDE, D, E);
		update_PDE();
	}
	void set_BC(reg24_pair val) {
		val.split_value(UBC, B, C);
		update_PBC();
	}
	void set_IX(reg24_pair val) {
		val.split_value(UIX, IXH, IXL);
		update_PIX();
	}
	void set_IY(reg24_pair val) {
		val.split_value(UIY, IYH, IYL);
		update_PIY();
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
		update_PHL();
	}
	void set_DE(uint24_t val) {
		reg24_pair dst;
		dst.set_value(val);
		set_DE(dst);
		update_PDE();
	}
	void set_BC(uint24_t val) {
		reg24_pair dst;
		dst.set_value(val);
		set_BC(dst);
		update_PBC();
	}
	void set_IX(uint24_t val) {
		reg24_pair dst;
		dst.set_value(val);
		set_IX(dst);
		update_PIX();
	}
	void set_IY(uint24_t val) {
		reg24_pair dst;
		dst.set_value(val);
		set_IY(dst);
		update_PIY();
	}
	void set_SP(uint24_t val) {
		reg24_pair dst;
		dst.set_value(val);
		set_SP(dst);
		set_pointers_invalid();
	}

/* CRT setters */
	private:
	void set16_zero_HL(reg16_pair val) {
		val.split_value(UHL, H, L);
		update_PHL();
	}
	void set16_zero_DE(reg16_pair val) {
		val.split_value(UDE, D, E);
		update_PDE();
	}
	void set16_zero_BC(reg16_pair val) {
		val.split_value(UBC, B, C);
		update_PBC();
	}
	void set16_zero_IX(reg16_pair val) {
		val.split_value(UIX, IXH, IXL);
		update_PIX();
	}
	void set16_zero_IY(reg16_pair val) {
		val.split_value(UIY, IYH, IYL);
		update_PIY();
	}
	void set16_zero_SP(__attribute__((unused)) reg16_pair val) {
		set_pointers_invalid();
		/* SP is unimplemented currently */
		return;
	}

	void set16_partial_HL(reg16_pair val) {
		val.split_value(H, L);
		UHL.set_unknown();
		update_PHL();
	}
	void set16_partial_DE(reg16_pair val) {
		val.split_value(D, E);
		UDE.set_unknown();
		update_PDE();
	}
	void set16_partial_BC(reg16_pair val) {
		val.split_value(B, C);
		UBC.set_unknown();
		update_PBC();
	}
	void set16_partial_IX(reg16_pair val) {
		val.split_value(IXH, IXL);
		UIX.set_unknown();
		update_PIX();
	}
	void set16_partial_IY(reg16_pair val) {
		val.split_value(IYH, IYL);
		UIY.set_unknown();
		update_PIY();
	}
	void set16_partial_SP(__attribute__((unused)) reg16_pair val) {
		set_pointers_invalid();
		/* SP is unimplemented currently */
		return;
	}

	void set16_preserve_HL(reg16_pair val) {
		val.split_value(H, L);
		update_PHL();
	}
	void set16_preserve_DE(reg16_pair val) {
		val.split_value(D, E);
		update_PDE();
	}
	void set16_preserve_BC(reg16_pair val) {
		val.split_value(B, C);
		update_PBC();
	}
	void set16_preserve_IX(reg16_pair val) {
		val.split_value(IXH, IXL);
		update_PIX();
	}
	void set16_preserve_IY(reg16_pair val) {
		val.split_value(IYH, IYL);
		update_PIY();
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
		update_PHL();
		update_PDE();
	}

	void set32_AUBC(reg32_pair val) {
		A.set_value(val.get_hi8());
		set_BC(val.get_lo24());
		update_PBC();
	}

	void set48_UDEUHL(reg48_pair val) {
		set_DE(val.get_hi24());
		set_HL(val.get_lo24());
		update_PHL();
		update_PDE();
	}

	void set48_UIYUBC(reg48_pair val) {
		set_IY(val.get_hi24());
		set_BC(val.get_lo24());
		update_PIY();
		update_PBC();
	}

	void set64_zero_BCUDEUHL(reg64_pair val) {
		set_HL(val.get_lo24());
		set_DE(val.get_hi24());
		set16_zero_BC(val.get_upper16());
		update_PHL();
		update_PDE();
		update_PBC();
	}

	void set64_preserve_BCUDEUHL(reg64_pair val) {
		set_HL(val.get_lo24());
		set_DE(val.get_hi24());
		set16_preserve_BC(val.get_upper16());
		update_PHL();
		update_PDE();
		update_PBC();
	}

	void set64_partial_BCUDEUHL(reg64_pair val) {
		set_HL(val.get_lo24());
		set_DE(val.get_hi24());
		set16_partial_BC(val.get_upper16());
		update_PHL();
		update_PDE();
		update_PBC();
	}

	void set64_STACK(reg64_pair val, size_t offset = 0) {
		reg16_pair up;
		reg24_pair hi, lo;
		val.split_value(up, hi, lo);
		stack.write_base(lo.get_lo()   , offset + 0);
		stack.write_base(lo.get_hi()   , offset + 1);
		stack.write_base(lo.get_upper(), offset + 2);
		stack.write_base(hi.get_lo()   , offset + 3);
		stack.write_base(hi.get_hi()   , offset + 4);
		stack.write_base(hi.get_upper(), offset + 5);
		stack.write_base(up.get_lo()   , offset + 6);
		stack.write_base(up.get_hi()   , offset + 7);
	}

/* set unknown */
	void HL_set_unknown() {
		UHL.set_unknown();
		H.set_unknown();
		L.set_unknown();
		update_PHL();
	}
	void DE_set_unknown() {
		UDE.set_unknown();
		D.set_unknown();
		E.set_unknown();
		update_PDE();
	}
	void BC_set_unknown() {
		UBC.set_unknown();
		B.set_unknown();
		C.set_unknown();
		update_PBC();
	}
	void IX_set_unknown() {
		UIX.set_unknown();
		IXH.set_unknown();
		IXL.set_unknown();
		stack.invalidate_IX();
		update_PIX();
	}
	void IY_set_unknown() {
		UIY.set_unknown();
		IYH.set_unknown();
		IYL.set_unknown();
		update_PIY();
	}
	void SP_set_unknown() {
		set_pointers_invalid();
		/* SP is unimplemented currently */
		return;
	}

	void next_instruction(ez80_instruction instruction);

	void next_known_func(ez80_instruction instruction, ez80_known_function func);

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
		#if 0
			"S12 " + print_reg24(stack.read_base(14), stack.read_base(13), stack.read_base(12)) +
			" | S9 " + print_reg24(stack.read_base(11), stack.read_base(10), stack.read_base(9)) +
			" | S6 " + print_reg24(stack.read_base(8), stack.read_base(7), stack.read_base(6)) +
			" | S3 " + print_reg24(stack.read_base(5), stack.read_base(4), stack.read_base(3)) +
		#else
			"S3 " + print_reg24(stack.read_base(5), stack.read_base(4), stack.read_base(3)) +
		#endif
			" | S0 " + print_reg24(stack.read_base(2), stack.read_base(1), stack.read_base(0)) +
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
