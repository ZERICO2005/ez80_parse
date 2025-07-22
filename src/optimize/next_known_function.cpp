#include "known_state.h"

void ez80_known_state::next_known_function(ez80_known_function func) {
	using enum ez80_known_function;
	ez80_state state = history.back();
	using enum ez80_op_code;
	using enum ez80_flag_state;
	using enum ez80_reg;
	state.flags_unknown();
	switch (func) {
		/* 8 bit */
		case __bbitrev: {
			state.get_reg8(A).set_unknown();
		} break;
		case __bctlz: {
			state.get_reg8(A).set_unknown();
		} break;
		case __bdivs: {
			state.get_reg8(A).set_destroyed();
			state.get_reg8(B).set_observed(true);
			state.get_reg8(C).set_observed(true);
		} break;
		case __bdivu: {
			state.get_reg8(A).set_destroyed();
			state.get_reg8(B).set_observed(true);
			state.get_reg8(C).set_observed(true);
		} break;
		case __bpopcnt: {
			state.get_reg8(A).set_unknown();
		} break;
		case __brems: {
			state.get_reg8(A).set_unknown();
			state.get_reg8(C).set_observed(true);
		} break;
		case __bremu: {
			state.get_reg8(A).set_unknown();
			state.get_reg8(C).set_observed(true);
		} break;
		case __bshl: {
			state.get_reg8(A).set_unknown();
			state.get_reg8(B).set_observed(true);
		} break;
		case __bshrs: {
			state.get_reg8(A).set_unknown();
			state.get_reg8(B).set_observed(true);
		} break;
		case __bshru: {
			state.get_reg8(A).set_unknown();
			state.get_reg8(B).set_observed(true);
		} break;
	
		/* 16 bit */
		case __sand: {
			state.get_reg24(HL).set_unknown();
			state.get_reg24(BC).set_observed(true);
		} break;
		case __sbitrev: {
			state.get_reg24(HL).set_unknown();
		} break;
		case __sbswap: {
			state.get_reg24(HL).set_unknown();
		} break;
		case __scmpzero: {
			state.get_reg24(HL).set_observed(true);
		} break;
		case __sctlz: {
			state.get_reg24(HL).set_observed(true);
			state.get_reg8(A).set_destroyed();
		} break;
		case __sdivs: {
			state.get_reg24(HL).set_unknown();
			state.get_reg24(BC).set_observed(true);
		} break;
		case __sdivu: {
			state.get_reg24(HL).set_unknown();
			state.get_reg24(BC).set_observed(true);
		} break;
		case __smulu: {
			state.get_reg24(HL).set_unknown();
			state.get_reg24(BC).set_observed(true);
		} break;
		case __sneg: {
			state.get_reg24(HL).set_unknown();
			state.get_reg24(BC).set_observed(true);
		} break;
		case __snot: {
			state.get_reg24(HL).set_unknown();
			state.get_reg24(BC).set_observed(true);
		} break;
		case __sor: {
			state.get_reg24(HL).set_unknown();
			state.get_reg24(BC).set_observed(true);
		} break;
		case __spopcnt: {
			state.get_reg24(HL).set_unknown();
			state.get_reg24(BC).set_observed(true);
		} break;
		case __srems: {
			state.get_reg24(HL).set_unknown();
			state.get_reg24(BC).set_observed(true);
		} break;
		case __sremu: {
			state.get_reg24(HL).set_unknown();
			state.get_reg24(BC).set_observed(true);
		} break;
		case __sshl: {
			state.crt_sshl();
		} break;
		case __sshrs: {
			state.get_reg24(HL).set_unknown();
			state.get_reg8(C).set_observed(true);
		} break;
		case __sshru: {
			state.crt_sshru();
		} break;
		case __sxor: {
			state.get_reg24(HL).set_unknown();
			state.get_reg24(BC).set_observed(true);
		} break;
		
		/* 24 bit */
		case __iand: {
			state.get_reg24(HL).set_unknown();
			state.get_reg24(BC).set_observed(true);
		} break;
		case __ibitrev: {
			state.get_reg24(HL).set_unknown();
		} break;
		case __ibswap: {
			state.get_reg24(HL).set_unknown();
		} break;
		case __icmpzero: {
			state.get_reg24(HL).set_observed(true);
		} break;
		case __ictlz: {
			state.get_reg24(HL).set_observed(true);
			state.get_reg8(A).set_destroyed();
		} break;
		case __idivs: {
			state.get_reg24(HL).set_unknown();
			state.get_reg24(BC).set_observed(true);
		} break;
		case __idivu: {
			state.get_reg24(HL).set_unknown();
			state.get_reg24(BC).set_observed(true);
		} break;
		case __imulu: {
			state.get_reg24(HL).set_unknown();
			state.get_reg24(BC).set_observed(true);
		} break;
		case __ineg: {
			state.get_reg24(HL).set_unknown();
		} break;
		case __inot: {
			state.get_reg24(HL).set_unknown();
		} break;
		case __ior: {
			state.get_reg24(HL).set_unknown();
			state.get_reg24(BC).set_observed(true);
		} break;
		case __ipopcnt: {
			state.get_reg24(HL).set_observed(true);
			state.get_reg8(A).set_destroyed();
		} break;
		case __irems: {
			state.get_reg24(HL).set_unknown();
			state.get_reg24(BC).set_observed(true);
		} break;
		case __iremu: {
			state.get_reg24(HL).set_unknown();
			state.get_reg24(BC).set_observed(true);
		} break;
		case __ishl: {
			state.crt_ishl();
		} break;
		case __ishrs: {
			state.get_reg24(HL).set_unknown();
			state.get_reg8(C).set_observed(true);
		} break;
		case __ishru: {
			state.crt_ishru();
		} break;
		case __ixor: {
			state.get_reg24(HL).set_unknown();
			state.get_reg24(BC).set_observed(true);
		} break;
		
		/* 32 bit */
		case __ladd: {
			state.get_reg24(HL).set_unknown();
			state.get_reg8(E).set_unknown();
			state.get_reg24(BC).set_observed(true);
			state.get_reg8(A).set_observed(true);
		} break;
		case __land: {
			state.get_reg24(HL).set_unknown();
			state.get_reg8(E).set_unknown();
			state.get_reg24(BC).set_observed(true);
			state.get_reg8(A).set_observed(true);
		} break;
		case __lbitrev: {
			state.get_reg24(HL).set_unknown();
			state.get_reg8(E).set_unknown();
		} break;
		case __lbswap: {
			state.get_reg24(HL).set_unknown();
			state.get_reg8(E).set_unknown();
		} break;
		case __lcmps: {
			state.get_reg24(HL).set_observed(true);
			state.get_reg8(E).set_observed(true);
			state.get_reg24(BC).set_observed(true);
			state.get_reg8(A).set_observed(true);
		} break;
		case __lcmpu: {
			state.get_reg24(HL).set_observed(true);
			state.get_reg8(E).set_observed(true);
			state.get_reg24(BC).set_observed(true);
			state.get_reg8(A).set_observed(true);
		} break;
		case __lcmpzero: {
			state.get_reg24(HL).set_observed(true);
			state.get_reg8(E).set_observed(true);
		} break;
		case __lctlz: {
			state.get_reg24(HL).set_unknown();
			state.get_reg8(E).set_unknown();
			state.get_reg8(A).set_destroyed();
		} break;
		case __ldivs: {
			state.get_reg24(HL).set_unknown();
			state.get_reg8(E).set_unknown();
			state.get_reg24(BC).set_observed(true);
			state.get_reg8(A).set_observed(true);
		} break;
		case __ldivu: {
			state.get_reg24(HL).set_unknown();
			state.get_reg8(E).set_unknown();
			state.get_reg24(BC).set_observed(true);
			state.get_reg8(A).set_observed(true);
		} break;
		case __lmulu: {
			state.get_reg24(HL).set_unknown();
			state.get_reg8(E).set_unknown();
			state.get_reg24(BC).set_observed(true);
			state.get_reg8(A).set_observed(true);
		} break;
		case __lneg: {
			state.get_reg24(HL).set_unknown();
			state.get_reg8(E).set_unknown();
		} break;
		case __lnot: {
			state.get_reg24(HL).set_unknown();
			state.get_reg8(E).set_unknown();
		} break;
		case __lor: {
			state.get_reg24(HL).set_unknown();
			state.get_reg8(E).set_unknown();
		} break;
		case __lpopcnt: {
			state.get_reg24(HL).set_unknown();
			state.get_reg8(E).set_unknown();
			state.get_reg8(A).set_destroyed();
		} break;
		case __lrems: {
			state.get_reg24(HL).set_unknown();
			state.get_reg8(E).set_unknown();
			state.get_reg24(BC).set_observed(true);
			state.get_reg8(A).set_observed(true);
		} break;
		case __lremu: {
			state.get_reg24(HL).set_unknown();
			state.get_reg8(E).set_unknown();
			state.get_reg24(BC).set_observed(true);
			state.get_reg8(A).set_observed(true);
		} break;
		case __lshl: {
			state.get_reg24(BC).set_unknown();
			state.get_reg8(A).set_unknown();
			state.get_reg8(L).set_observed(true);
		} break;
		case __lshrs: {
			state.get_reg24(BC).set_unknown();
			state.get_reg8(A).set_unknown();
			state.get_reg8(L).set_observed(true);
		} break;
		case __lshru: {
			state.get_reg24(BC).set_unknown();
			state.get_reg8(A).set_unknown();
			state.get_reg8(L).set_observed(true);
		} break;
		case __lsub: {
			state.get_reg24(HL).set_unknown();
			state.get_reg8(E).set_unknown();
			state.get_reg24(BC).set_observed(true);
			state.get_reg8(A).set_observed(true);
		} break;
		case __lxor: {
			state.get_reg24(HL).set_unknown();
			state.get_reg8(E).set_unknown();
			state.get_reg24(BC).set_observed(true);
			state.get_reg8(A).set_observed(true);
		} break;
		
		/* 48 bit */
		case __i48add: {
			state.state_unknown();
		} break;
		case __i48and: {
			state.state_unknown();
		} break;
		case __i48bitrev: {
			state.state_unknown();
		} break;
		case __i48bswap: {
			state.state_unknown();
		} break;
		case __i48cmps: {
			state.get_reg24(HL).set_observed(true);
			state.get_reg24(DE).set_observed(true);
			state.get_reg24(BC).set_observed(true);
			state.get_reg24(IY).set_observed(true);
		} break;
		case __i48cmpu: {
			state.get_reg24(HL).set_observed(true);
			state.get_reg24(DE).set_observed(true);
			state.get_reg24(BC).set_observed(true);
			state.get_reg24(IY).set_observed(true);
		} break;
		case __i48cmpzero: {
			state.get_reg24(HL).set_observed(true);
			state.get_reg24(DE).set_observed(true);
		} break;
		case __i48ctlz: {
			state.state_unknown();
		} break;
		case __i48divs: {
			state.state_unknown();
		} break;
		case __i48divu: {
			state.state_unknown();
		} break;
		case __i48mulu: {
			state.state_unknown();
		} break;
		case __i48neg: {
			state.state_unknown();
		} break;
		case __i48not: {
			state.state_unknown();
		} break;
		case __i48or: {
			state.state_unknown();
		} break;
		case __i48popcnt: {
			state.state_unknown();
		} break;
		case __i48rems: {
			state.state_unknown();
		} break;
		case __i48remu: {
			state.state_unknown();
		} break;
		case __i48shl: {
			state.state_unknown();
		} break;
		case __i48shrs: {
			state.state_unknown();
		} break;
		case __i48shru: {
			state.state_unknown();
		} break;
		case __i48sub: {
			state.state_unknown();
		} break;
		case __i48xor: {
			state.state_unknown();
		} break;
		
		/* 64 bit */
		case __lladd: {
			state.state_unknown();
		} break;
		case __lland: {
			state.state_unknown();
		} break;
		case __llbitrev: {
			state.state_unknown();
		} break;
		case __llbswap: {
			state.state_unknown();
		} break;
		case __llcmps: {
			state.get_reg24(HL).set_observed(true);
			state.get_reg24(DE).set_observed(true);
			state.get_reg24(BC).set_observed(true);
		} break;
		case __llcmpu: {
			state.get_reg24(HL).set_observed(true);
			state.get_reg24(DE).set_observed(true);
			state.get_reg24(BC).set_observed(true);
		} break;
		case __llcmpzero: {
			state.get_reg24(HL).set_observed(true);
			state.get_reg24(DE).set_observed(true);
			state.get_reg24(BC).set_observed(true);
		} break;
		case __llctlz: {
			state.state_unknown();
		} break;
		case __lldivs: {
			state.state_unknown();
		} break;
		case __lldivu: {
			state.state_unknown();
		} break;
		case __llmulu: {
			state.state_unknown();
		} break;
		case __llneg: {
			state.state_unknown();
		} break;
		case __llnot: {
			state.state_unknown();
		} break;
		case __llor: {
			state.state_unknown();
		} break;
		case __llpopcnt: {
			state.state_unknown();
		} break;
		case __llrems: {
			state.state_unknown();
		} break;
		case __llremu: {
			state.state_unknown();
		} break;
		case __llshl: {
			state.state_unknown();
		} break;
		case __llshrs: {
			state.state_unknown();
		} break;
		case __llshru: {
			state.state_unknown();
		} break;
		case __llsub: {
			state.state_unknown();
		} break;
		case __llxor: {
			state.state_unknown();
		} break;
	
		/* float/double */
		case __fadd: {
			state.get_reg8(A).set_unknown();
			state.get_reg24(BC).set_unknown();
			state.get_reg8(E).set_observed(true);
			state.get_reg24(HL).set_observed(true);
		} break;
		case __fcmp: {
			state.get_reg8(A).set_observed(true);
			state.get_reg24(BC).set_observed(true);
			state.get_reg8(E).set_observed(true);
			state.get_reg24(HL).set_observed(true);
		} break;
		case __fcmpo: {
			state.get_reg8(A).set_observed(true);
			state.get_reg24(BC).set_observed(true);
			state.get_reg8(E).set_observed(true);
			state.get_reg24(HL).set_observed(true);
		} break;
		case __fcmpu: {
			state.get_reg8(A).set_observed(true);
			state.get_reg24(BC).set_observed(true);
			state.get_reg8(E).set_observed(true);
			state.get_reg24(HL).set_observed(true);
		} break;
		case __fdiv: {
			state.get_reg8(A).set_unknown();
			state.get_reg24(BC).set_unknown();
			state.get_reg8(E).set_observed(true);
			state.get_reg24(HL).set_observed(true);
		} break;
		case __fmul: {
			state.get_reg8(A).set_unknown();
			state.get_reg24(BC).set_unknown();
			state.get_reg8(E).set_observed(true);
			state.get_reg24(HL).set_observed(true);
		} break;
		case __fneg: {
			state.get_reg8(A).set_unknown();
			state.get_reg24(BC).set_observed(true);
		} break;
		case __frem: {
			state.get_reg8(A).set_unknown();
			state.get_reg24(BC).set_unknown();
			state.get_reg8(E).set_observed(true);
			state.get_reg24(HL).set_observed(true);
		} break;
		case __fsub: {
			state.get_reg8(A).set_unknown();
			state.get_reg24(BC).set_unknown();
			state.get_reg8(E).set_observed(true);
			state.get_reg24(HL).set_observed(true);
		} break;
		case __ftod: {
			state.get_reg24(BC).set_unknown();
			state.get_reg24(DE).set_unknown();
			state.get_reg24(HL).set_unknown();
		} break;
		case __ftol: {
			state.get_reg8(E).set_unknown();
			state.get_reg24(HL).set_unknown();
		} break;
		case __ftoll: {
			state.get_reg24(BC).set_unknown();
			state.get_reg24(DE).set_unknown();
			state.get_reg24(HL).set_unknown();
		} break;
		case __ftoul: {
			state.get_reg8(E).set_unknown();
			state.get_reg24(HL).set_unknown();
		} break;
		case __ftoull: {
			state.get_reg24(BC).set_unknown();
			state.get_reg24(DE).set_unknown();
			state.get_reg24(HL).set_unknown();
		} break;
		case __lltof: {
			state.get_reg24(BC).set_observed(true);
			state.get_reg24(DE).set_unknown();
			state.get_reg24(HL).set_unknown();
		} break;
		case __ltof: {
			state.get_reg8(E).set_unknown();
			state.get_reg24(HL).set_unknown();
		} break;
		case __ulltof: {
			state.get_reg24(BC).set_observed(true);
			state.get_reg24(DE).set_unknown();
			state.get_reg24(HL).set_unknown();
		} break;
		case __ultof: {
			state.get_reg8(E).set_unknown();
			state.get_reg24(HL).set_unknown();
		} break;
	
		/* long double */
		case __dadd: {
			state.state_unknown();
		} break;
		case __dcmp: {
			state.get_reg24(BC).set_observed(true);
			state.get_reg24(DE).set_observed(true);
			state.get_reg24(HL).set_observed(true);
		} break;
		case __dcmpo: {
			state.get_reg24(BC).set_observed(true);
			state.get_reg24(DE).set_observed(true);
			state.get_reg24(HL).set_observed(true);
		} break;
		case __dcmpu: {
			state.get_reg24(BC).set_observed(true);
			state.get_reg24(DE).set_observed(true);
			state.get_reg24(HL).set_observed(true);
		} break;
		case __ddiv: {
			state.state_unknown();
		} break;
		case __dmul: {
			state.state_unknown();
		} break;
		case __dneg: {
			state.state_unknown();
		} break;
		case __drem: {
			state.state_unknown();
		} break;
		case __dsub: {
			state.state_unknown();
		} break;
		case __dtof: {
			state.state_unknown();
		} break;
		case __dtol: {
			state.state_unknown();
		} break;
		case __dtoll: {
			state.state_unknown();
		} break;
		case __dtoul: {
			state.state_unknown();
		} break;
		case __dtoull: {
			state.state_unknown();
		} break;
		case __lltod: {
			state.state_unknown();
		} break;
		case __ltod: {
			state.state_unknown();
		} break;
		case __ulltod: {
			state.state_unknown();
		} break;
		case __ultod: {
			state.state_unknown();
		} break;
	
		/* CRT routines */
		case __frameset: {
			state.state_unknown();
		} break;
		case __frameset0: {
			state.state_unknown();
		} break;
		case __setflags: {
		} break;
	
		/* <string.h> */
		case _memcpy: {
			state.state_unknown();
		} break;
		case _memmove: {
			state.state_unknown();
		} break;
		case _memset: {
			state.state_unknown();
		} break;
		case _bzero: {
			state.state_unknown();
		} break;
		case _memcmp: {
			state.state_unknown();
		} break;
		case _memchr: {
			state.state_unknown();
		} break;
		case _memrchr: {
			state.state_unknown();
		} break;
		case _memmem: {
			state.state_unknown();
		} break;
		case _memccpy: {
			state.state_unknown();
		} break;
		case _mempcpy: {
			state.state_unknown();
		} break;
		case _strcpy: {
			state.state_unknown();
		} break;
		case _strncpy: {
			state.state_unknown();
		} break;
		case _stpcpy: {
			state.state_unknown();
		} break;
		case _stpncpy: {
			state.state_unknown();
		} break;
		case _strlcpy: {
			state.state_unknown();
		} break;
		case _strcat: {
			state.state_unknown();
		} break;
		case _strncat: {
			state.state_unknown();
		} break;
		case _strlcat: {
			state.state_unknown();
		} break;
		case _strchr: {
			state.state_unknown();
		} break;
		case _strrchr: {
			state.state_unknown();
		} break;
		case _strpbrk: {
			state.state_unknown();
		} break;
		case _strstr: {
			state.state_unknown();
		} break;
		case _strcasestr: {
			state.state_unknown();
		} break;
		case _strtok: {
			state.state_unknown();
		} break;
		case _strdup: {
			state.state_unknown();
		} break;
		case _strndup: {
			state.state_unknown();
		} break;
		case _strcspn: {
			state.state_unknown();
		} break;
		case _strspn: {
			state.state_unknown();
		} break;
		case _strlen: {
			state.state_unknown();
		} break;
		case _strnlen: {
			state.state_unknown();
		} break;
		case _strcmp: {
			state.state_unknown();
		} break;
		case _strncmp: {
			state.state_unknown();
		} break;
		case _strcasecmp: {
			state.state_unknown();
		} break;
		case _strncasecmp: {
			state.state_unknown();
		} break;
	
		/* <ctype.h> */
		case _isalnum: {
			state.state_unknown();
		} break;
		case _isalpha: {
			state.state_unknown();
		} break;
		case _isblank: {
			state.state_unknown();
		} break;
		case _iscntrl: {
			state.state_unknown();
		} break;
		case _isdigit: {
			state.state_unknown();
		} break;
		case _isgraph: {
			state.state_unknown();
		} break;
		case _islower: {
			state.state_unknown();
		} break;
		case _isprint: {
			state.state_unknown();
		} break;
		case _ispunct: {
			state.state_unknown();
		} break;
		case _isspace: {
			state.state_unknown();
		} break;
		case _isupper: {
			state.state_unknown();
		} break;
		case _tolower: {
			state.state_unknown();
		} break;
		case _toupper: {
			state.state_unknown();
		} break;
		case _isascii: {
			state.state_unknown();
		} break;
		case _sxdigit: {
			state.state_unknown();
		} break;
	
		/* <math.h> */
		case _copysign: {
			state.state_unknown();
		} break;
		case _copysignf: {
			state.state_unknown();
		} break;
		case _copysignl: {
			state.state_unknown();
		} break;
		case _fabs: {
			state.state_unknown();
		} break;
		case _fabsf: {
			state.state_unknown();
		} break;
		case _fabsl: {
			state.state_unknown();
		} break;
	
		/* <stdlib.h> */
		case _abs: {
			state.state_unknown();
		} break;
		case _labs: {
			state.state_unknown();
		} break;
		case _i48abs: {
			state.state_unknown();
		} break;
		case _llabs: {
			state.state_unknown();
		} break;
		case _imaxabs: {
			state.state_unknown();
		} break;
		case _div: {
			state.state_unknown();
		} break;
		case _ldiv: {
			state.state_unknown();
		} break;
		case _i48div: {
			state.state_unknown();
		} break;
		case _lldiv: {
			state.state_unknown();
		} break;
		case _imaxdiv: {
			state.state_unknown();
		} break;	
	}
}
