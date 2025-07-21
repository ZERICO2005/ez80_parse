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

void parse_asm(const string& input, vector<asm_line>& output);

#endif /* EZ80_PARSE */
