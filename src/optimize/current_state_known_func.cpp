#include "current_state.hpp"

template<typename T>
void crt_cmpzero(flag_pair& f, T x) {
	f.set_flags_unknown();
	f.set_zero(x.is_zero());
}

template<typename T>
void crt_cmpu(flag_pair& f, T x, T y) {
	flag_state carry, zero;
	unsigned_compare(carry, zero, x, y);
	f.set_carry(carry);
	f.set_zero(zero);
}

template<typename T>
void crt_cmps(flag_pair& f, T x, T y) {
	f.set_flags_unknown();
	f.set_zero(compare_equal(x, y));
}

template<typename T>
T crt_not(T dst) {
	dst = ~dst;
	return dst;
}

template<typename T>
T crt_neg(T dst) {
	dst = -dst;
	return dst;
}

template<typename T>
T crt_and(T dst, T src) {
	dst = (dst & src);
	return dst;
}

template<typename T>
T crt_or(T dst, T src) {
	dst = (dst | src);
	return dst;
}

template<typename T>
T crt_xor(T dst, T src) {
	dst = (dst ^ src);
	return dst;
}

template<typename T>
T crt_shl(T dst, reg8_pair shift) {
	if (shift.isknown_fully()) {
		dst.shift_left_logical(shift.bits);
		return dst;
	}
	dst.set_unknown();
	return dst;
}

template<typename T>
T crt_shru(T dst, reg8_pair shift) {
	if (shift.isknown_fully()) {
		dst.shift_right_logical(shift.bits);
		return dst;
	}
	dst.set_unknown();
	return dst;
}

template<typename T>
T crt_shrs(T dst, reg8_pair shift) {
	if (shift.isknown_fully()) {
		dst.shift_right_arithmetic(shift.bits);
		return dst;
	}
	dst.set_unknown();
	return dst;
}

template<typename T>
T crt_add(T dst, T src) {
	dst = (dst + src);
	return dst;
}

template<typename T>
T crt_sub(T dst, T src) {
	dst = (dst - src);
	return dst;
}

template<typename T>
T crt_mul(T dst, T src) {
	dst = (dst * src);
	return dst;
}

template<typename T>
T crt_divu(T dst, T src) {
	dst = div_unsigned(dst, src);
	return dst;
}

template<typename T>
T crt_divs(T dst, T src) {
	if (isknown_false(dst.test_signbit()) && isknown_false(src.test_signbit())) {
		return crt_divu(dst, src);
	}
	dst.set_unknown();
	return dst;
}

template<typename T>
T crt_remu(T dst, T src) {
	dst = rem_unsigned(dst, src);
	return dst;
}

template<typename T>
T crt_rems(T dst, T src) {
	if (isknown_false(dst.test_signbit()) && isknown_false(src.test_signbit())) {
		return crt_remu(dst, src);
	}
	dst.set_unknown();
	return dst;
}

template<typename T>
void crt_ctlz(reg8_pair& dst, T src) {
	uint8_t min_bound, max_bound;
	reg_pair_leading_zeros(min_bound,max_bound, src);
	dst.set_to_unsigned_range(min_bound, max_bound);
}

template<typename T>
void crt_cttz(reg8_pair& dst, T src) {
	uint8_t min_bound, max_bound;
	reg_pair_trailing_zeros(min_bound,max_bound, src);
	dst.set_to_unsigned_range(min_bound, max_bound);
}

template<typename T>
void crt_popcnt(reg8_pair& dst, __attribute__((unused)) T src) {
	uint8_t min_bound, max_bound;
	reg_pair_count_ones(min_bound,max_bound, src);
	dst.set_to_unsigned_range(min_bound, max_bound);
}

template<typename T>
T crt_bswap(T dst) {
	return swap_byte_order(dst);
}

template<typename T>
T crt_bitrev(T dst) {
	return bit_reverse(dst);
}

void current_state::next_known_func(ez80_instruction instruction, ez80_known_function func) {
	using enum ez80_known_function;
	using enum ez80_op_code;
	using enum flag_state;
	F.set_flags_unknown();
	switch (func) {

		/* 8 bit */

		case __bshl: {
			A.set_value(crt_shl(A, B));
		} break;
		case __bshru: {
			A.set_value(crt_shru(A, B));
		} break;
		case __bshrs: {
			A.set_value(crt_shru(A, B));
		} break;
		case __bdivu: {
			A.set_value(C);
			A.set_value(crt_shru(A, C));
		} break;
		case __bdivs: {
			A.set_value(B);
			A.set_value(crt_shru(A, C));
		} break;
		case __bremu: {
			A.set_value(crt_remu(A, C));
		} break;
		case __brems: {
			A.set_value(crt_rems(A, C));
		} break;
		case __bctlz: {
			crt_ctlz(A, A);
		} break;
		case __bcttz: {
			crt_ctlz(A, A);
		} break;
		case __bpopcnt: {
			crt_popcnt(A, A);
		} break;
		case __bbitrev: {
			A.set_value(crt_bitrev(A));
		} break;
	
		/* 16 bit */

		case __scmpzero: {
			crt_cmpzero(F, get16_HL());
		} break;
		case __snot: {
			set16_partial_HL(crt_not(get16_HL()));
		} break;
		case __sneg: {
			set16_partial_HL(crt_neg(get16_HL()));
		} break;
		case __sand: {
			set16_partial_HL(crt_and(get16_HL(), get16_BC()));
		} break;
		case __sor: {
			set16_partial_HL(crt_or(get16_HL(), get16_BC()));
		} break;
		case __sxor: {
			set16_partial_HL(crt_xor(get16_HL(), get16_BC()));
		} break;
		case __sshl: {
			set16_partial_HL(crt_shl(get16_HL(), C));
		} break;
		case __sshru: {
			set16_partial_HL(crt_shru(get16_HL(), C));
		} break;
		case __sshrs: {
			set16_partial_HL(crt_shrs(get16_HL(), C));
		} break;
		case __smulu: {
			set16_partial_HL(crt_mul(get16_HL(), get16_BC()));
		} break;
		case __sdivu: {
			set16_partial_HL(crt_divu(get16_HL(), get16_BC()));
		} break;
		case __sdivs: {
			set16_partial_HL(crt_divs(get16_HL(), get16_BC()));
		} break;
		case __sremu: {
			set16_partial_HL(crt_remu(get16_HL(), get16_BC()));
		} break;
		case __srems: {
			set16_partial_HL(crt_rems(get16_HL(), get16_BC()));
		} break;
		case __sctlz: {
			crt_ctlz(A, get16_HL());
		} break;
		case __scttz: {
			crt_cttz(A, get16_HL());
		} break;
		case __spopcnt: {
			crt_popcnt(A, get16_HL());
		} break;
		case __sbswap: {
			set16_partial_HL(crt_bswap(get16_HL()));
		} break;
		case __sbitrev: {
			set16_partial_HL(crt_bitrev(get16_HL()));
		} break;

		/* 24 bit */

		case __icmpzero: {
			crt_cmpzero(F, get_HL());
		} break;
		case __inot: {
			set_HL(crt_not(get_HL()));
		} break;
		case __ineg: {
			set_HL(crt_neg(get_HL()));
		} break;
		case __iand: {
			set_HL(crt_and(get_HL(), get_BC()));
		} break;
		case __ior: {
			set_HL(crt_or(get_HL(), get_BC()));
		} break;
		case __ixor: {
			set_HL(crt_xor(get_HL(), get_BC()));
		} break;
		case __ishl: {
			set_HL(crt_shl(get_HL(), C));
		} break;
		case __ishru: {
			set_HL(crt_shru(get_HL(), C));
		} break;
		case __ishrs: {
			set_HL(crt_shrs(get_HL(), C));
		} break;
		case __imulu: {
			set_HL(crt_mul(get_HL(), get_BC()));
		} break;
		case __idivu: {
			set_HL(crt_divu(get_HL(), get_BC()));
		} break;
		case __idivs: {
			set_HL(crt_divs(get_HL(), get_BC()));
		} break;
		case __iremu: {
			set_HL(crt_remu(get_HL(), get_BC()));
		} break;
		case __irems: {
			set_HL(crt_rems(get_HL(), get_BC()));
		} break;
		case __ictlz: {
			crt_ctlz(A, get_HL());
		} break;
		case __icttz: {
			crt_cttz(A, get_HL());
		} break;
		case __ipopcnt: {
			crt_popcnt(A, get_HL());
		} break;
		case __ibswap: {
			set_HL(crt_bswap(get_HL()));
		} break;
		case __ibitrev: {
			set_HL(crt_bitrev(get_HL()));
		} break;
		
		/* 32 bit */

		case __lcmpzero: {
			crt_cmpzero(F, get32_EUHL());
		} break;
		case __lcmpu: {
			crt_cmpu(F, get32_EUHL(), get32_AUBC());
		} break;
		case __lcmps: {
			crt_cmps(F, get32_EUHL(), get32_AUBC());
		} break;
		case __lnot: {
			set32_EUHL(crt_not(get32_EUHL()));
		} break;
		case __lneg: {
			set32_EUHL(crt_neg(get32_EUHL()));
		} break;
		case __land: {
			set32_EUHL(crt_and(get32_EUHL(), get32_AUBC()));
		} break;
		case __lor: {
			set32_EUHL(crt_or(get32_EUHL(), get32_AUBC()));
		} break;
		case __lxor: {
			set32_EUHL(crt_xor(get32_EUHL(), get32_AUBC()));
		} break;
		case __lshl: {
			set32_AUBC(crt_shl(get32_AUBC(), L));
		} break;
		case __lshru: {
			set32_AUBC(crt_shru(get32_AUBC(), L));
		} break;
		case __lshrs: {
			set32_AUBC(crt_shrs(get32_AUBC(), L));
		} break;
		case __ladd: {
			set32_EUHL(crt_add(get32_EUHL(), get32_AUBC()));
		} break;
		case __lsub: {
			set32_EUHL(crt_sub(get32_EUHL(), get32_AUBC()));
		} break;
		case __lmulu: {
			set32_EUHL(crt_mul(get32_EUHL(), get32_AUBC()));
		} break;
		case __ldivu: {
			set32_EUHL(crt_divu(get32_EUHL(), get32_AUBC()));
		} break;
		case __ldivs: {
			set32_EUHL(crt_divs(get32_EUHL(), get32_AUBC()));
		} break;
		case __lremu: {
			set32_EUHL(crt_remu(get32_EUHL(), get32_AUBC()));
		} break;
		case __lrems: {
			set32_EUHL(crt_rems(get32_EUHL(), get32_AUBC()));
		} break;
		case __lctlz: {
			crt_ctlz(A, get32_EUHL());
		} break;
		case __lcttz: {
			crt_ctlz(A, get32_EUHL());
		} break;
		case __lpopcnt: {
			crt_popcnt(A, get32_EUHL());
		} break;
		case __lbswap: {
			set32_EUHL(crt_bswap(get32_EUHL()));
		} break;
		case __lbitrev: {
			set32_EUHL(crt_bitrev(get32_EUHL()));
		} break;
		
		/* 48 bit */
	
		case __i48cmpzero: {
			crt_cmpzero(F, get48_UDEUHL());
		} break;
		case __i48cmpu: {
			crt_cmpu(F, get48_UDEUHL(), get48_UIYUBC());
		} break;
		case __i48cmps: {
			crt_cmps(F, get48_UDEUHL(), get48_UIYUBC());
		} break;
		case __i48not: {
			set48_UDEUHL(crt_not(get48_UDEUHL()));
		} break;
		case __i48neg: {
			set48_UDEUHL(crt_neg(get48_UDEUHL()));
		} break;
		case __i48and: {
			set48_UDEUHL(crt_and(get48_UDEUHL(), get48_UIYUBC()));
		} break;
		case __i48or: {
			set48_UDEUHL(crt_or(get48_UDEUHL(), get48_UIYUBC()));
		} break;
		case __i48xor: {
			set48_UDEUHL(crt_xor(get48_UDEUHL(), get48_UIYUBC()));
		} break;
		case __i48shl: {
			set48_UDEUHL(crt_shl(get48_UDEUHL(), C));
		} break;
		case __i48shru: {
			set48_UDEUHL(crt_shru(get48_UDEUHL(), C));
		} break;
		case __i48shrs: {
			set48_UDEUHL(crt_shrs(get48_UDEUHL(), C));
		} break;
		case __i48add: {
			set48_UDEUHL(crt_add(get48_UDEUHL(), get48_UIYUBC()));
		} break;
		case __i48sub: {
			set48_UDEUHL(crt_sub(get48_UDEUHL(), get48_UIYUBC()));
		} break;
		case __i48mulu: {
			set48_UDEUHL(crt_mul(get48_UDEUHL(), get48_UIYUBC()));
		} break;
		case __i48divu: {
			set48_UDEUHL(crt_divu(get48_UDEUHL(), get48_UIYUBC()));
		} break;
		case __i48divs: {
			set48_UDEUHL(crt_divs(get48_UDEUHL(), get48_UIYUBC()));
		} break;
		case __i48remu: {
			set48_UDEUHL(crt_remu(get48_UDEUHL(), get48_UIYUBC()));
		} break;
		case __i48rems: {
			set48_UDEUHL(crt_rems(get48_UDEUHL(), get48_UIYUBC()));
		} break;
		case __i48ctlz: {
			crt_ctlz(A, get48_UDEUHL());
		} break;
		case __i48cttz: {
			crt_ctlz(A, get48_UDEUHL());
		} break;
		case __i48popcnt: {
			crt_popcnt(A, get48_UDEUHL());
		} break;
		case __i48bswap: {
			set48_UDEUHL(crt_bswap(get48_UDEUHL()));
		} break;
		case __i48bitrev: {
			set48_UDEUHL(crt_bitrev(get48_UDEUHL()));
		} break;

		/* 64 bit */

		case __llcmpzero: {
			crt_cmpzero(F, get64_BCUDEUHL());
		} break;
		case __llcmpu: {
			crt_cmpu(F, get64_BCUDEUHL(), get64_STACK());
		} break;
		case __llcmps: {
			crt_cmps(F, get64_BCUDEUHL(), get64_STACK());
		} break;
		case __llnot: {
			set64_partial_BCUDEUHL(crt_not(get64_BCUDEUHL()));
		} break;
		case __llneg: {
			set64_partial_BCUDEUHL(crt_neg(get64_BCUDEUHL()));
		} break;
		case __lland: {
			set64_partial_BCUDEUHL(crt_and(get64_BCUDEUHL(), get64_STACK()));
		} break;
		case __llor: {
			set64_partial_BCUDEUHL(crt_or(get64_BCUDEUHL(), get64_STACK()));
		} break;
		case __llxor: {
			set64_partial_BCUDEUHL(crt_xor(get64_BCUDEUHL(), get64_STACK()));
		} break;
		case __llshl: {
			set64_partial_BCUDEUHL(crt_shl(get64_BCUDEUHL(), get8_STACK(0)));
		} break;
		case __llshru: {
			set64_partial_BCUDEUHL(crt_shru(get64_BCUDEUHL(), get8_STACK(0)));
		} break;
		case __llshrs: {
			set64_partial_BCUDEUHL(crt_shrs(get64_BCUDEUHL(), get8_STACK(0)));
		} break;
		case __lladd: {
			set64_partial_BCUDEUHL(crt_add(get64_BCUDEUHL(), get64_STACK()));
		} break;
		case __llsub: {
			set64_partial_BCUDEUHL(crt_sub(get64_BCUDEUHL(), get64_STACK()));
		} break;
		case __llmulu: {
			set64_partial_BCUDEUHL(crt_mul(get64_BCUDEUHL(), get64_STACK()));
		} break;
		case __lldivu: {
			set64_partial_BCUDEUHL(crt_divu(get64_BCUDEUHL(), get64_STACK()));
		} break;
		case __lldivs: {
			set64_partial_BCUDEUHL(crt_divs(get64_BCUDEUHL(), get64_STACK()));
		} break;
		case __llremu: {
			set64_partial_BCUDEUHL(crt_remu(get64_BCUDEUHL(), get64_STACK()));
		} break;
		case __llrems: {
			set64_partial_BCUDEUHL(crt_rems(get64_BCUDEUHL(), get64_STACK()));
		} break;
		case __llctlz: {
			crt_ctlz(A, get64_BCUDEUHL());
		} break;
		case __llcttz: {
			crt_ctlz(A, get64_BCUDEUHL());
		} break;
		case __llpopcnt: {
			crt_popcnt(A, get64_BCUDEUHL());
		} break;
		case __llbswap: {
			set64_partial_BCUDEUHL(crt_bswap(get64_BCUDEUHL()));
		} break;
		case __llbitrev: {
			set64_partial_BCUDEUHL(crt_bitrev(get64_BCUDEUHL()));
		} break;
	
		/* float/double */

		case __fcmp: {
		} break;
		case __fcmpo: {
		} break;
		case __fcmpu: {
		} break;
		case __fneg: {
			A.bit_unknown(7);
		} break;
		case __fadd: {
			A.set_unknown();
			BC_set_unknown();
		} break;
		case __fsub: {
			A.set_unknown();
			BC_set_unknown();
		} break;
		case __fmul: {
			A.set_unknown();
			BC_set_unknown();
		} break;
		case __fdiv: {
			A.set_unknown();
			BC_set_unknown();
		} break;
		case __frem: {
			A.set_unknown();
			BC_set_unknown();
		} break;
		case __ftod: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __ftol: {
			A.set_unknown();
			BC_set_unknown();
		} break;
		case __ftoll: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __ftoul: {
			A.set_unknown();
			BC_set_unknown();
		} break;
		case __ftoull: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __ltof: {
			A.set_unknown();
			BC_set_unknown();
		} break;
		case __lltof: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __ultof: {
			A.set_unknown();
			BC_set_unknown();
		} break;
		case __ulltof: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
	
		/* long double */

		case __dcmp: {
			stack.set_stack_base_unknown(9);
		} break;
		case __dcmpo: {
			stack.set_stack_base_unknown(9);
		} break;
		case __dcmpu: {
			stack.set_stack_base_unknown(9);
		} break;
		case __dneg: {
			B.bit_unknown(7);
			UBC.set_unknown();
		} break;
		case __dadd: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
			stack.set_stack_base_unknown(9);
		} break;
		case __dsub: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
			stack.set_stack_base_unknown(9);
		} break;
		case __dmul: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
			stack.set_stack_base_unknown(9);
		} break;
		case __ddiv: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
			stack.set_stack_base_unknown(9);
		} break;
		case __drem: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
			stack.set_stack_base_unknown(9);
		} break;
		case __dtol: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __dtoll: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __dtoul: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __dtoull: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __dtof: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __ltod: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __lltod: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __ultod: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __ulltod: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		
		/* CRT routines */

		case __frameset: {
			reg24_pair offset = get_HL();
			set_all_reg_unknown();
			if (offset.isknown_fully()) {
				stack.frameset((int)((int24_t)offset.bits));
			}
		} break;
		case __frameset0: {
			set_all_reg_unknown();
			stack.frameset0();
		} break;
		case __setflag: {
			if (F.isknown_overflow()) {
				if (F.isknown_overflow_set()) {
					F.invert_sign();
				}
			} else {
				F.set_sign_unknown();
			}
		} break;
	
		/* <string.h> */

		case _memcpy: {
			reg24_pair ret = get24_STACK(0);
			set_just_libc_reg_unknown(9);
			set_HL(ret);
		} break;
		case _memmove: {
			reg24_pair ret = get24_STACK(0);
			set_just_libc_reg_unknown(9);
			set_HL(ret);
		} break;
		case _memset: {
			reg24_pair ret = get24_STACK(0);
			set_just_libc_reg_unknown(9);
			set_HL(ret);
		} break;
		case _bzero: {
			set_just_libc_reg_unknown(9);
		} break;
		case _memcmp: {
			set_just_libc_reg_unknown(9);
		} break;
		case _memchr: {
			set_just_libc_reg_unknown(9);
		} break;
		case _memrchr: {
			set_just_libc_reg_unknown(9);
		} break;
		case _memmem: {
			set_just_libc_reg_unknown(12);
		} break;
		case _memccpy: {
			set_just_libc_reg_unknown(12);
		} break;
		case _mempcpy: {
			reg24_pair ptr = get24_STACK(0);
			reg24_pair len = get24_STACK(3);
			set_just_libc_reg_unknown(9);
			set_HL(ptr + len);
		} break;
		case _strcpy: {
			reg24_pair ret = get24_STACK(0);
			set_just_libc_reg_unknown(6);
			set_HL(ret);
		} break;
		case _strncpy: {
			reg24_pair ret = get24_STACK(0);
			set_just_libc_reg_unknown(9);
			set_HL(ret);
		} break;
		case _stpcpy: {
			set_just_libc_reg_unknown(9);
		} break;
		case _stpncpy: {
			set_just_libc_reg_unknown(9);
		} break;
		case _strlcpy: {
			set_just_libc_reg_unknown(9);
		} break;
		case _strcat: {
			reg24_pair ret = get24_STACK(0);
			set_just_libc_reg_unknown(6);
			set_HL(ret);
		} break;
		case _strncat: {
			reg24_pair ret = get24_STACK(0);
			set_just_libc_reg_unknown(9);
			set_HL(ret);
		} break;
		case _strlcat: {
			set_just_libc_reg_unknown(9);
		} break;
		case _strchr: {
			set_just_libc_reg_unknown(6);
		} break;
		case _strrchr: {
			set_just_libc_reg_unknown(6);
		} break;
		case _strpbrk: {
			set_just_libc_reg_unknown(6);
		} break;
		case _strstr: {
			set_just_libc_reg_unknown(6);
		} break;
		case _strcasestr: {
			set_just_libc_reg_unknown(6);
		} break;
		case _strtok: {
			set_just_cxx_reg_unknown();
		} break;
		case _strdup: {
			set_just_libc_reg_unknown(3);
		} break;
		case _strndup: {
			set_just_libc_reg_unknown(6);
		} break;
		case _strcspn: {
			set_just_libc_reg_unknown(6);
		} break;
		case _strspn: {
			set_just_libc_reg_unknown(6);
		} break;
		case _strlen: {
			set_just_libc_reg_unknown(3);
		} break;
		case _strnlen: {
			reg24_pair max_len = get24_STACK(6);
			set_just_libc_reg_unknown(6);
			max_len.set_to_unsigned_range(0, max_len.get_unsigned_maximum());
			set_HL(max_len);
		} break;
		case _strcmp: {
			set_just_libc_reg_unknown(6);
		} break;
		case _strncmp: {
			set_just_libc_reg_unknown(9);
		} break;
		case _strcasecmp: {
			set_just_libc_reg_unknown(6);
		} break;
		case _strncasecmp: {
			set_just_libc_reg_unknown(6);
		} break;
	
		/* <ctype.h> */

		case _isalnum: {
			set_just_libc_reg_unknown(3);
		} break;
		case _isalpha: {
			set_just_libc_reg_unknown(3);
		} break;
		case _isblank: {
			set_just_libc_reg_unknown(3);
		} break;
		case _iscntrl: {
			set_just_libc_reg_unknown(3);
		} break;
		case _isdigit: {
			set_just_libc_reg_unknown(3);
		} break;
		case _isgraph: {
			set_just_libc_reg_unknown(3);
		} break;
		case _islower: {
			set_just_libc_reg_unknown(3);
		} break;
		case _isprint: {
			set_just_libc_reg_unknown(3);
		} break;
		case _ispunct: {
			set_just_libc_reg_unknown(3);
		} break;
		case _isspace: {
			set_just_libc_reg_unknown(3);
		} break;
		case _isupper: {
			set_just_libc_reg_unknown(3);
		} break;
		case _tolower: {
			set_just_libc_reg_unknown(3);
		} break;
		case _toupper: {
			set_just_libc_reg_unknown(3);
		} break;
		case _isascii: {
			set_just_libc_reg_unknown(3);
		} break;
		case _sxdigit: {
			set_just_libc_reg_unknown(3);
		} break;
	
		/* <math.h> */

		case _copysign:
		case _copysignf: {
			reg32_pair ret = get32_STACK(0);
			flag_state sign = get32_STACK(6).test_signbit();
			set_just_libc_reg_unknown(12);
			ret.bit_copy(31, sign);
			set32_EUHL(ret);
		} break;
		case _copysignl: {
			reg64_pair ret = get64_STACK(0);
			flag_state sign = get64_STACK(9).test_signbit();
			set_just_libc_reg_unknown(18);
			ret.bit_copy(63, sign);
			set64_partial_BCUDEUHL(ret);
		} break;
		case _fabs:
		case _fabsf: {
			reg32_pair ret = get32_STACK(0);
			set_just_libc_reg_unknown(6);
			ret.bit_clear(31);
			set32_EUHL(ret);
		} break;
		case _fabsl: {
			reg64_pair ret = get64_STACK(0);
			set_just_libc_reg_unknown(9);
			ret.bit_clear(63);
			set64_partial_BCUDEUHL(ret);
		} break;

		/* <stdlib.h> */

		case _abs: {
			reg24_pair ret = get24_STACK(0);
			set_just_libc_reg_unknown(3);
			set_HL(absolute_value(ret));
		} break;
		case _labs: {
			reg32_pair ret = get32_STACK(0);
			set_just_libc_reg_unknown(6);
			set32_EUHL(absolute_value(ret));
		} break;
		case _i48abs: {
			reg48_pair ret = get48_STACK(0);
			set_just_libc_reg_unknown(6);
			set48_UDEUHL(absolute_value(ret));
		} break;
		case _llabs:
		case _imaxabs: {
			reg64_pair ret = get64_STACK(0);
			set_just_libc_reg_unknown(9);
			set64_partial_BCUDEUHL(absolute_value(ret));
		} break;
		case _div: {
			set_just_cxx_reg_unknown();
		} break;
		case _ldiv: {
			set_just_cxx_reg_unknown();
		} break;
		case _i48div: {
			set_just_cxx_reg_unknown();
		} break;
		case _lldiv:
		case _imaxdiv: {
			set_just_cxx_reg_unknown();
		} break;
	}
	set_previous_instruction(instruction, func);
}
