#ifndef EZ80_PARSE
#define EZ80_PARSE

/**
 * @file
 * This header contains the major function declarations
 */

#include "ez80_type.h"

#include "common_std.h"

#include "ez80_instruction.h"
#include "ez80_asm.h"

string instruction_to_string(ez80_instruction x);

void parse_asm(ez80_program& output, const string& input);

void optimize_asm(ez80_code_section& program);

void process_asm(ez80_code_section& output, const ez80_program& input);

void print_program(const ez80_program& program);

string code_section_to_string(const ez80_code_section& x);

/**
 * tests the arithmetic logic used for reg_pair
 */
bool test_reg_pair();

#endif /* EZ80_PARSE */
