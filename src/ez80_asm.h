#ifndef EZ80_ASM
#define EZ80_ASM

#include "common_std.h"
#include "ez80_instruction.h"
#include "ez80_asm.h"

enum class asm_line_type {
	blank,
	directive,
	label,
	compiler_label,
	instruction,
};

struct asm_line {
	size_t line_number; /* the original line number */
	string text;
	asm_line_type line_type;
	ez80_instruction instruction;

	asm_line(size_t line_number = 0) {
		this->line_number = line_number;
	}
};

#endif /* EZ80_ASM */
