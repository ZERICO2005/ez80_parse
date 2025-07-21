#ifndef EZ80_INSTRUCTION_H
#define EZ80_INSTRUCTION_H

#include "ez80_op_code.h"

enum class ez80_modifier {
	NONE,
	AUTO = NONE,
	S,
	L,
	IS,
	IL,
	SIS,
	LIL,
	SIL,
	LIS,
};

struct ez80_instruction {
	ez80_op_code op_code;
	// ez80_modifier modifer;
	int8_t offset;
	uint24_t value;
	bool known_value;
	string symbol;
	ez80_instruction() {
		op_code = ez80_op_code::UNKNOWN;
		// modifer = ez80_modifier::NONE;
		offset = 0;
		value = 0;
		known_value = false;
	}
};

#endif /* EZ80_INSTRUCTION_H */
