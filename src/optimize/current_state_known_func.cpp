#include "current_state.hpp"

template<typename T>
void crt_cmpzero(flag_pair& f, T x) {
	f.set_flags_unknown();
	f.set_zero(x.is_zero());
}

template<typename T>
void crt_cmpu(flag_pair& f, __attribute__((unused)) T x, __attribute__((unused)) T y) {
	f.set_flags_unknown();
}

template<typename T>
void crt_cmps(flag_pair& f, __attribute__((unused)) T x, __attribute__((unused)) T y) {
	f.set_flags_unknown();
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
T crt_divs(T dst, __attribute__((unused)) T src) {
	dst.set_unknown();
	return dst;
}

template<typename T>
T crt_remu(T dst, __attribute__((unused)) T src) {
	dst = rem_unsigned(dst, src);
	return dst;
}

template<typename T>
T crt_rems(T dst, __attribute__((unused)) T src) {
	dst.set_unknown();
	return dst;
}

template<typename T>
void crt_ctlz(reg8_pair& dst, __attribute__((unused)) T src) {
	dst.set_unknown();
}

template<typename T>
void crt_popcnt(reg8_pair& dst, __attribute__((unused)) T src) {
	dst.set_unknown();
}

template<typename T>
T crt_bswap(T dst) {
	dst.set_unknown();
	return dst;
}

template<typename T>
T crt_bitrev(T dst) {
	dst.set_unknown();
	return dst;
}

void current_state::next_known_func(ez80_known_function func) {
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
		case __bbitrev: {
			A.set_value(crt_bitrev(A));
		} break;
		case __bctlz: {
			crt_ctlz(A, A);
		} break;
		case __bpopcnt: {
			crt_popcnt(A, A);
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

		} break;
		case __i48cmpu: {

		} break;
		case __i48cmps: {

		} break;
		case __i48not: {
			HL_set_unknown();
			DE_set_unknown();
		} break;
		case __i48neg: {
			HL_set_unknown();
			DE_set_unknown();
		} break;
		case __i48and: {
			HL_set_unknown();
			DE_set_unknown();
		} break;
		case __i48or: {
			HL_set_unknown();
			DE_set_unknown();
		} break;
		case __i48xor: {
			HL_set_unknown();
			DE_set_unknown();
		} break;
		case __i48shl: {
			HL_set_unknown();
			DE_set_unknown();
		} break;
		case __i48shru: {
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __i48shrs: {
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __i48add: {
			HL_set_unknown();
			DE_set_unknown();
		} break;
		case __i48sub: {
			HL_set_unknown();
			DE_set_unknown();
		} break;
		case __i48mulu: {
			HL_set_unknown();
			DE_set_unknown();
		} break;
		case __i48divu: {
			HL_set_unknown();
			DE_set_unknown();
		} break;
		case __i48divs: {
			HL_set_unknown();
			DE_set_unknown();
		} break;
		case __i48remu: {
			HL_set_unknown();
			DE_set_unknown();
		} break;
		case __i48rems: {
			HL_set_unknown();
			DE_set_unknown();
		} break;
		case __i48ctlz: {
			A.set_unknown();
		} break;
		case __i48popcnt: {
			A.set_unknown();
		} break;
		case __i48bswap: {
			HL_set_unknown();
			DE_set_unknown();
		} break;
		case __i48bitrev: {
			HL_set_unknown();
			DE_set_unknown();
		} break;

		/* 64 bit */

		case __llcmpzero: {

		} break;
		case __llcmpu: {

		} break;
		case __llcmps: {

		} break;
		case __llnot: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __llneg: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __lland: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __llor: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __llxor: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __llshl: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __llshru: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __llshrs: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __lladd: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __llsub: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __llmulu: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __lldivu: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __lldivs: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __llremu: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __llrems: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __llctlz: {
			A.set_unknown();
		} break;
		case __llpopcnt: {
			A.set_unknown();
		} break;
		case __llbswap: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __llbitrev: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
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
			E.set_unknown();
			HL_set_unknown();
		} break;
		case __ftoll: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __ftoul: {
			E.set_unknown();
			HL_set_unknown();
		} break;
		case __ftoull: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __ltof: {
			E.set_unknown();
			HL_set_unknown();
		} break;
		case __lltof: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
		case __ultof: {
			E.set_unknown();
			HL_set_unknown();
		} break;
		case __ulltof: {
			BC_set_unknown();
			DE_set_unknown();
			HL_set_unknown();
		} break;
	
		/* long double */

		case __dcmp: {
			
		} break;
		case __dcmpo: {
			
		} break;
		case __dcmpu: {
			
		} break;
		case __dneg: {
			B.bit_unknown(7);
			UBC.set_unknown();
		} break;
		case __dadd: {
			set_all_reg_unknown();
		} break;
		case __dsub: {
			set_all_reg_unknown();
		} break;
		case __dmul: {
			set_all_reg_unknown();
		} break;
		case __ddiv: {
			set_all_reg_unknown();
		} break;
		case __drem: {
			set_all_reg_unknown();
		} break;
		case __dtol: {
			set_all_reg_unknown();
		} break;
		case __dtoll: {
			set_all_reg_unknown();
		} break;
		case __dtoul: {
			set_all_reg_unknown();
		} break;
		case __dtoull: {
			set_all_reg_unknown();
		} break;
		case __dtof: {
			set_all_reg_unknown();
		} break;
		case __ltod: {
			set_all_reg_unknown();
		} break;
		case __lltod: {
			set_all_reg_unknown();
		} break;
		case __ultod: {
			set_all_reg_unknown();
		} break;
		case __ulltod: {
			set_all_reg_unknown();
		} break;
		
		/* CRT routines */

		case __frameset: {
			set_all_reg_unknown();
		} break;
		case __frameset0: {
			set_all_reg_unknown();
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
			set_all_reg_unknown();
		} break;
		case _memmove: {
			set_all_reg_unknown();
		} break;
		case _memset: {
			set_all_reg_unknown();
		} break;
		case _bzero: {
			set_all_reg_unknown();
		} break;
		case _memcmp: {
			set_all_reg_unknown();
		} break;
		case _memchr: {
			set_all_reg_unknown();
		} break;
		case _memrchr: {
			set_all_reg_unknown();
		} break;
		case _memmem: {
			set_all_reg_unknown();
		} break;
		case _memccpy: {
			set_all_reg_unknown();
		} break;
		case _mempcpy: {
			set_all_reg_unknown();
		} break;
		case _strcpy: {
			set_all_reg_unknown();
		} break;
		case _strncpy: {
			set_all_reg_unknown();
		} break;
		case _stpcpy: {
			set_all_reg_unknown();
		} break;
		case _stpncpy: {
			set_all_reg_unknown();
		} break;
		case _strlcpy: {
			set_all_reg_unknown();
		} break;
		case _strcat: {
			set_all_reg_unknown();
		} break;
		case _strncat: {
			set_all_reg_unknown();
		} break;
		case _strlcat: {
			set_all_reg_unknown();
		} break;
		case _strchr: {
			set_all_reg_unknown();
		} break;
		case _strrchr: {
			set_all_reg_unknown();
		} break;
		case _strpbrk: {
			set_all_reg_unknown();
		} break;
		case _strstr: {
			set_all_reg_unknown();
		} break;
		case _strcasestr: {
			set_all_reg_unknown();
		} break;
		case _strtok: {
			set_all_reg_unknown();
		} break;
		case _strdup: {
			set_all_reg_unknown();
		} break;
		case _strndup: {
			set_all_reg_unknown();
		} break;
		case _strcspn: {
			set_all_reg_unknown();
		} break;
		case _strspn: {
			set_all_reg_unknown();
		} break;
		case _strlen: {
			set_all_reg_unknown();
		} break;
		case _strnlen: {
			set_all_reg_unknown();
		} break;
		case _strcmp: {
			set_all_reg_unknown();
		} break;
		case _strncmp: {
			set_all_reg_unknown();
		} break;
		case _strcasecmp: {
			set_all_reg_unknown();
		} break;
		case _strncasecmp: {
			set_all_reg_unknown();
		} break;
	
		/* <ctype.h> */

		case _isalnum: {
			set_all_reg_unknown();
		} break;
		case _isalpha: {
			set_all_reg_unknown();
		} break;
		case _isblank: {
			set_all_reg_unknown();
		} break;
		case _iscntrl: {
			set_all_reg_unknown();
		} break;
		case _isdigit: {
			set_all_reg_unknown();
		} break;
		case _isgraph: {
			set_all_reg_unknown();
		} break;
		case _islower: {
			set_all_reg_unknown();
		} break;
		case _isprint: {
			set_all_reg_unknown();
		} break;
		case _ispunct: {
			set_all_reg_unknown();
		} break;
		case _isspace: {
			set_all_reg_unknown();
		} break;
		case _isupper: {
			set_all_reg_unknown();
		} break;
		case _tolower: {
			set_all_reg_unknown();
		} break;
		case _toupper: {
			set_all_reg_unknown();
		} break;
		case _isascii: {
			set_all_reg_unknown();
		} break;
		case _sxdigit: {
			set_all_reg_unknown();
		} break;
	
		/* <math.h> */

		case _copysign: {
			set_all_reg_unknown();
		} break;
		case _copysignf: {
			set_all_reg_unknown();
		} break;
		case _copysignl: {
			set_all_reg_unknown();
		} break;
		case _fabs: {
			set_all_reg_unknown();
		} break;
		case _fabsf: {
			set_all_reg_unknown();
		} break;
		case _fabsl: {
			set_all_reg_unknown();
		} break;
	
		/* <stdlib.h> */

		case _abs: {
			set_all_reg_unknown();
		} break;
		case _labs: {
			set_all_reg_unknown();
		} break;
		case _i48abs: {
			set_all_reg_unknown();
		} break;
		case _llabs: {
			set_all_reg_unknown();
		} break;
		case _imaxabs: {
			set_all_reg_unknown();
		} break;
		case _div: {
			set_all_reg_unknown();
		} break;
		case _ldiv: {
			set_all_reg_unknown();
		} break;
		case _i48div: {
			set_all_reg_unknown();
		} break;
		case _lldiv: {
			set_all_reg_unknown();
		} break;
		case _imaxdiv: {
			set_all_reg_unknown();
		} break;	
	}
}
