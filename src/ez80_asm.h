#ifndef EZ80_ASM
#define EZ80_ASM

#include "common_std.h"
#include "ez80_instruction.h"
#include "ez80_asm.h"
#include "ez80_known_function.h"

enum class asm_line_type {
	blank,
	directive,
	label,
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

struct ez80_program {
	vector<asm_line> prog;
};

enum class ez80_calling_convention {
	UNKNOWN,
	LOCAL,
	CXX,
	LIBCALL,
};

enum class ez80_code_type {
	code,
	branch,	
	known_func,
	label,
	directive,
};

enum class ez80_directive {
	ASSUME_ADL0,
	ASSUME_ADL1,
	IDENT,
	EXTERN,
	PRIVATE,
	PUBLIC,
	SECTION_TEXT,
	SECTION_DATA,
	SECTION_RODATA,
};

struct ez80_label {
	string text;
	ez80_calling_convention convention;
};

struct ez80_code {
	size_t original_line; /* 0 if the line was auto generated */
	ez80_code_type type;
	ez80_instruction instruction;
	ez80_known_function known_func;
	ez80_label label;
};

struct ez80_code_section {
	list<ez80_code> prog;
};

struct ez80_application {
	vector<ez80_code_section> section;
};

#endif /* EZ80_ASM */
