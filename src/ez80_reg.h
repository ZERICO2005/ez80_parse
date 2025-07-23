#ifndef EZ80_REG
#define EZ80_REG

#include "ez80_type.h"

enum class ez80_reg {
	NONE,
	A,
	B,
	C,
	D,
	E,
	H,
	L,
	IXL,
	IXH,
	IYL,
	IYH,
	UBC,
	UDE,
	UHL,
	UIX,
	UIY,
	BC,
	DE,
	HL,
	IX,
	IY,
	SP,
};

union ez80_flag {
	struct {
		uint8_t carry : 1;
		uint8_t addsub : 1;
		uint8_t three : 1;
		uint8_t half : 1;
		uint8_t five : 1;
		uint8_t overflow : 1;
		uint8_t zero : 1;
		uint8_t sign : 1;
	};
	uint8_t raw;
};

enum class ez80_flag_bit {
	carry    =  0,
	addsub   =  1,
	three    =  2,
	half     =  3,
	five     =  4,
	overflow =  5,
	zero     =  6,
	sign     =  7,
};

struct ez80_reg_field {
	ez80_flag flag;
	union {
		struct {
			uint32_t A   : 1;
			uint32_t UBC : 1;
			uint32_t B   : 1;
			uint32_t C   : 1;
			uint32_t UDE : 1;
			uint32_t D   : 1;
			uint32_t E   : 1;
			uint32_t UHL : 1;
			uint32_t H   : 1;
			uint32_t L   : 1;
			uint32_t UIX : 1;
			uint32_t IXH : 1;
			uint32_t IXL : 1;
			uint32_t UIY : 1;
			uint32_t IYH : 1;
			uint32_t IYL : 1;
			uint32_t SP  : 1;
		};
		uint32_t raw;
	};
};

enum class ez80_reg_field_bit {
	A   =  0,
	UBC =  1,
	B   =  2,
	C   =  3,
	UDE =  4,
	D   =  5,
	E   =  6,
	UHL =  7,
	H   =  8,
	L   =  9,
	UIX = 10,
	IXH = 11,
	IXL = 12,
	UIY = 13,
	IYH = 14,
	IYL = 15,
	SP  = 16,
};


#endif /* EZ80_REG */
