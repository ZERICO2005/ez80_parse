#ifndef EZ80_LIBCALL_H
#define EZ80_LIBCALL_H

#include "common_std.h"

enum class ez80_known_function {
	/* 8 bit */
	__bbitrev,
	__bctlz,
	__bcttz,
	__bdivs,
	__bdivu,
	__bpopcnt,
	__brems,
	__bremu,
	__bshl,
	__bshrs,
	__bshru,

	/* 16 bit */
	__sand,
	__sbitrev,
	__sbswap,
	__scmpzero,
	__sctlz,
	__scttz,
	__sdivs,
	__sdivu,
	__smulu,
	__sneg,
	__snot,
	__sor,
	__spopcnt,
	__srems,
	__sremu,
	__sshl,
	__sshrs,
	__sshru,
	__sxor,

	/* 24 bit */
	__iand,
	__ibitrev,
	__ibswap,
	__icmpzero,
	__ictlz,
	__icttz,
	__idivs,
	__idivu,
	__imulu,
	__ineg,
	__inot,
	__ior,
	__ipopcnt,
	__irems,
	__iremu,
	__ishl,
	__ishrs,
	__ishru,
	__ixor,

	/* 32 bit */
	__ladd,
	__land,
	__lbitrev,
	__lbswap,
	__lcmps,
	__lcmpu,
	__lcmpzero,
	__lctlz,
	__lcttz,
	__ldivs,
	__ldivu,
	__lmulu,
	__lneg,
	__lnot,
	__lor,
	__lpopcnt,
	__lrems,
	__lremu,
	__lshl,
	__lshrs,
	__lshru,
	__lsub,
	__lxor,

	/* 48 bit */
	__i48add,
	__i48and,
	__i48bitrev,
	__i48bswap,
	__i48cmps,
	__i48cmpu,
	__i48cmpzero,
	__i48ctlz,
	__i48cttz,
	__i48divs,
	__i48divu,
	__i48mulu,
	__i48neg,
	__i48not,
	__i48or,
	__i48popcnt,
	__i48rems,
	__i48remu,
	__i48shl,
	__i48shrs,
	__i48shru,
	__i48sub,
	__i48xor,

	/* 64 bit */
	__lladd,
	__lland,
	__llbitrev,
	__llbswap,
	__llcmps,
	__llcmpu,
	__llcmpzero,
	__llctlz,
	__llcttz,
	__lldivs,
	__lldivu,
	__llmulu,
	__llneg,
	__llnot,
	__llor,
	__llpopcnt,
	__llrems,
	__llremu,
	__llshl,
	__llshrs,
	__llshru,
	__llsub,
	__llxor,

	/* float/double */
	__fadd,
	__fcmp,
	__fcmpo,
	__fcmpu,
	__fdiv,
	__fmul,
	__fneg,
	__frem,
	__fsub,
	__ftod,
	__ftol,
	__ftoll,
	__ftoul,
	__ftoull,
	__lltof,
	__ltof,
	__ulltof,
	__ultof,

	/* long double */
	__dadd,
	__dcmp,
	__dcmpo,
	__dcmpu,
	__ddiv,
	__dmul,
	__dneg,
	__drem,
	__dsub,
	__dtof,
	__dtol,
	__dtoll,
	__dtoul,
	__dtoull,
	__lltod,
	__ltod,
	__ulltod,
	__ultod,

	/* CRT routines */
	__frameset,
	__frameset0,
	__setflag,

	/* <string.h> */
	_memcpy,
	_memmove,
	_memset,
	_bzero,
	_memcmp,
	_memchr,
	_memrchr,
	_memmem,
	_memccpy,
	_mempcpy,
	_strcpy,
	_strncpy,
	_stpcpy,
	_stpncpy,
	_strlcpy,
	_strcat,
	_strncat,
	_strlcat,
	_strchr,
	_strrchr,
	_strpbrk,
	_strstr,
	_strcasestr,
	_strtok,
	_strdup,
	_strndup,
	_strcspn,
	_strspn,
	_strlen,
	_strnlen,
	_strcmp,
	_strncmp,
	_strcasecmp,
	_strncasecmp,

	/* <ctype.h> */
	_isalnum,
	_isalpha,
	_isblank,
	_iscntrl,
	_isdigit,
	_isgraph,
	_islower,
	_isprint,
	_ispunct,
	_isspace,
	_isupper,
	_tolower,
	_toupper,
	_isascii,
	_sxdigit,

	/* <math.h> */
	_copysign,
	_copysignf,
	_copysignl,
	_fabs,
	_fabsf,
	_fabsl,

	/* <stdlib.h> */
	_abs,
	_labs,
	_i48abs,
	_llabs,
	_imaxabs,
	_div,
	_ldiv,
	_i48div,
	_lldiv,
	_imaxdiv,
};

char const * const ez80_known_function_name[] {
	/* 8 bit */
	"__bbitrev",
	"__bctlz",
	"__bcttz",
	"__bdivs",
	"__bdivu",
	"__bpopcnt",
	"__brems",
	"__bremu",
	"__bshl",
	"__bshrs",
	"__bshru",

	/* 16 bit */
	"__sand",
	"__sbitrev",
	"__sbswap",
	"__scmpzero",
	"__sctlz",
	"__scttz",
	"__sdivs",
	"__sdivu",
	"__smulu",
	"__sneg",
	"__snot",
	"__sor",
	"__spopcnt",
	"__srems",
	"__sremu",
	"__sshl",
	"__sshrs",
	"__sshru",
	"__sxor",

	/* 24 bit */
	"__iand",
	"__ibitrev",
	"__ibswap",
	"__icmpzero",
	"__ictlz",
	"__icttz",
	"__idivs",
	"__idivu",
	"__imulu",
	"__ineg",
	"__inot",
	"__ior",
	"__ipopcnt",
	"__irems",
	"__iremu",
	"__ishl",
	"__ishrs",
	"__ishru",
	"__ixor",

	/* 32 bit */
	"__ladd",
	"__land",
	"__lbitrev",
	"__lbswap",
	"__lcmps",
	"__lcmpu",
	"__lcmpzero",
	"__lctlz",
	"__lcttz",
	"__ldivs",
	"__ldivu",
	"__lmulu",
	"__lneg",
	"__lnot",
	"__lor",
	"__lpopcnt",
	"__lrems",
	"__lremu",
	"__lshl",
	"__lshrs",
	"__lshru",
	"__lsub",
	"__lxor",

	/* 48 bit */
	"__i48add",
	"__i48and",
	"__i48bitrev",
	"__i48bswap",
	"__i48cmps",
	"__i48cmpu",
	"__i48cmpzero",
	"__i48ctlz",
	"__i48cttz",
	"__i48divs",
	"__i48divu",
	"__i48mulu",
	"__i48neg",
	"__i48not",
	"__i48or",
	"__i48popcnt",
	"__i48rems",
	"__i48remu",
	"__i48shl",
	"__i48shrs",
	"__i48shru",
	"__i48sub",
	"__i48xor",

	/* 64 bit */
	"__lladd",
	"__lland",
	"__llbitrev",
	"__llbswap",
	"__llcmps",
	"__llcmpu",
	"__llcmpzero",
	"__llctlz",
	"__llcttz",
	"__lldivs",
	"__lldivu",
	"__llmulu",
	"__llneg",
	"__llnot",
	"__llor",
	"__llpopcnt",
	"__llrems",
	"__llremu",
	"__llshl",
	"__llshrs",
	"__llshru",
	"__llsub",
	"__llxor",

	/* float/double */
	"__fadd",
	"__fcmp",
	"__fcmpo",
	"__fcmpu",
	"__fdiv",
	"__fmul",
	"__fneg",
	"__frem",
	"__fsub",
	"__ftod",
	"__ftol",
	"__ftoll",
	"__ftoul",
	"__ftoull",
	"__lltof",
	"__ltof",
	"__ulltof",
	"__ultof",

	/* long double */
	"__dadd",
	"__dcmp",
	"__dcmpo",
	"__dcmpu",
	"__ddiv",
	"__dmul",
	"__dneg",
	"__drem",
	"__dsub",
	"__dtof",
	"__dtol",
	"__dtoll",
	"__dtoul",
	"__dtoull",
	"__lltod",
	"__ltod",
	"__ulltod",
	"__ultod",

	/* CRT routines */
	"__frameset",
	"__frameset0",
	"__setflag",

	/* <string.h> */
	"_memcpy",
	"_memmove",
	"_memset",
	"_bzero",
	"_memcmp",
	"_memchr",
	"_memrchr",
	"_memmem",
	"_memccpy",
	"_mempcpy",
	"_strcpy",
	"_strncpy",
	"_stpcpy",
	"_stpncpy",
	"_strlcpy",
	"_strcat",
	"_strncat",
	"_strlcat",
	"_strchr",
	"_strrchr",
	"_strpbrk",
	"_strstr",
	"_strcasestr",
	"_strtok",
	"_strdup",
	"_strndup",
	"_strcspn",
	"_strspn",
	"_strlen",
	"_strnlen",
	"_strcmp",
	"_strncmp",
	"_strcasecmp",
	"_strncasecmp",

	/* <ctype.h> */
	"_isalnum",
	"_isalpha",
	"_isblank",
	"_iscntrl",
	"_isdigit",
	"_isgraph",
	"_islower",
	"_isprint",
	"_ispunct",
	"_isspace",
	"_isupper",
	"_tolower",
	"_toupper",
	"_isascii",
	"_sxdigit",

	/* <math.h> */
	"_copysign",
	"_copysignf",
	"_copysignl",
	"_fabs",
	"_fabsf",
	"_fabsl",

	/* <stdlib.h> */
	"_abs",
	"_labs",
	"_i48abs",
	"_llabs",
	"_imaxabs",
	"_div",
	"_ldiv",
	"_i48div",
	"_lldiv",
	"_imaxdiv",
};

inline bool is_crt_function(ez80_known_function func) {
	using enum ez80_known_function;
	switch (func) {
		case __bbitrev:
		case __bctlz:
		case __bcttz:
		case __bdivs:
		case __bdivu:
		case __bpopcnt:
		case __brems:
		case __bremu:
		case __bshl:
		case __bshrs:
		case __bshru:
		case __sand:
		case __sbitrev:
		case __sbswap:
		case __scmpzero:
		case __sctlz:
		case __scttz:
		case __sdivs:
		case __sdivu:
		case __smulu:
		case __sneg:
		case __snot:
		case __sor:
		case __spopcnt:
		case __srems:
		case __sremu:
		case __sshl:
		case __sshrs:
		case __sshru:
		case __sxor:
		case __iand:
		case __ibitrev:
		case __ibswap:
		case __icmpzero:
		case __ictlz:
		case __icttz:
		case __idivs:
		case __idivu:
		case __imulu:
		case __ineg:
		case __inot:
		case __ior:
		case __ipopcnt:
		case __irems:
		case __iremu:
		case __ishl:
		case __ishrs:
		case __ishru:
		case __ixor:
		case __ladd:
		case __land:
		case __lbitrev:
		case __lbswap:
		case __lcmps:
		case __lcmpu:
		case __lcmpzero:
		case __lctlz:
		case __lcttz:
		case __ldivs:
		case __ldivu:
		case __lmulu:
		case __lneg:
		case __lnot:
		case __lor:
		case __lpopcnt:
		case __lrems:
		case __lremu:
		case __lshl:
		case __lshrs:
		case __lshru:
		case __lsub:
		case __lxor:
		case __i48add:
		case __i48and:
		case __i48bitrev:
		case __i48bswap:
		case __i48cmps:
		case __i48cmpu:
		case __i48cmpzero:
		case __i48ctlz:
		case __i48cttz:
		case __i48divs:
		case __i48divu:
		case __i48mulu:
		case __i48neg:
		case __i48not:
		case __i48or:
		case __i48popcnt:
		case __i48rems:
		case __i48remu:
		case __i48shl:
		case __i48shrs:
		case __i48shru:
		case __i48sub:
		case __i48xor:
		case __lladd:
		case __lland:
		case __llbitrev:
		case __llbswap:
		case __llcmps:
		case __llcmpu:
		case __llcmpzero:
		case __llctlz:
		case __llcttz:
		case __lldivs:
		case __lldivu:
		case __llmulu:
		case __llneg:
		case __llnot:
		case __llor:
		case __llpopcnt:
		case __llrems:
		case __llremu:
		case __llshl:
		case __llshrs:
		case __llshru:
		case __llsub:
		case __llxor:
		case __fadd:
		case __fcmp:
		case __fcmpo:
		case __fcmpu:
		case __fdiv:
		case __fmul:
		case __fneg:
		case __frem:
		case __fsub:
		case __ftod:
		case __ftol:
		case __ftoll:
		case __ftoul:
		case __ftoull:
		case __lltof:
		case __ltof:
		case __ulltof:
		case __ultof:
		case __dadd:
		case __dcmp:
		case __dcmpo:
		case __dcmpu:
		case __ddiv:
		case __dmul:
		case __dneg:
		case __drem:
		case __dsub:
		case __dtof:
		case __dtol:
		case __dtoll:
		case __dtoul:
		case __dtoull:
		case __lltod:
		case __ltod:
		case __ulltod:
		case __ultod:
		case __frameset:
		case __frameset0:
		case __setflag:
			return true;
		default:
			return false;
	}
}

#endif /* EZ80_LIBCALL_H */
