/*
**	Author: zerico2005 (2024)
**	Project: 
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include <stdint.h>
#include <stdio.h>
#include <thread>
#include <limits>
#include <algorithm>
#include <vector>
#include <atomic>
#include <string.h>
#include <inttypes.h>
#include <random>
#include <fenv.h>
#include <bit>
#include <string>
#include <iostream>
#include <unordered_map>
#include <sstream>
#include <ctype.h>
#include <string.h>

#include <math.h>

#include "instruction.h"
#include "ez80_type.h"
#include "ez80_instruction_info.h"
#include "text.h"

#include "process_asm.h"

#include "known_state.h"

#include "instruction_printer.h"

using std::string;
using std::vector;
using std::array;
using std::min;
using std::max;

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

struct instruction_text_pair {
	char const * str;
	void(*handler)(asm_line&, char const *);
};

vector<asm_line> asm_text;

char str_retrive(const string& line, size_t index) {
	if (index >= line.size()) {
		return '\0';
	}
	return line[index];
}

static void clean_string(string& line) {
	char const * const whitespace_list = " \t\n\r\f\v";
	/* remove comments */ {
		size_t pos = line.find(';');
		if (pos != std::string::npos) {
			line.erase(pos);
		}
	}
	/* remove leading/trailing whitespace */ {
		size_t end = line.find_last_not_of(whitespace_list);
		if (end != std::string::npos) {
			line.erase(end + 1);
		}
	}
	#if 1
	/* remove leading whitespace */ {
		size_t start = line.find_first_not_of(whitespace_list);
		if (start != std::string::npos) {
			line.erase(0, start);
		}
	}
	#endif
	/* remove redundant whitespace */ 
	if (line.size() >= 1) {
		string result = "";
		for (size_t i = 0; i < line.size(); i++) {
			if (ispunct(line[i]) && line[i] != '_' && line[i] != '$' && isspace(line[i + 1])) {
				result += line[i];
				i++;
				continue;
			}
			result += line[i];
		}
		line = result;
	}
	if (line.size() >= 1) {
		string result = "";
		for (size_t i = 0; i < line.size(); i++) {
			if (isspace(line[i]) && ispunct(line[i + 1]) && line[i + 1] != '_'  && line[i + 1] != '$') {
				continue;
			}
			result += line[i];
		}
		line = result;
	}
	/* replace or add spaces to ',' '+' '-' '(' ')' */ {
		string result = "";
		for (size_t i = 0; i < line.size(); i++) {
			if (line[i] == ',') {
				result += ' ';
				continue;
			}
			if (line[i] == '+' || line[i] == '-' || line[i] == '(') {
				result += ' ';
			}
			result += line[i];
			if (line[i] == ')') {
				result += ' ';
			}
		}
		line = result;
	}
	/* remove redundant spaces */
	if (line.size() >= 2) {
		string result = "";
		for (size_t i = 0; i < line.size(); i++) {
			if (
				(str_retrive(line, i - 1) == '(') &&
				isspace(str_retrive(line, i)) &&
				(str_retrive(line, i + 1) == '-' || str_retrive(line, i + 1) == '+')
			) {
				continue;
			}
			result += line[i];
		}
		line = result;
	}
	/* remove spaces around symbols */
	if (line.size() >= 3) {
		string result = "";
		for (size_t i = 0; i < line.size(); i++) {
			const char c_m3 = str_retrive(line, i - 3);
			const char c_m2 = toupper(str_retrive(line, i - 2));
			const char c_m1 = toupper(str_retrive(line, i - 1));
			bool is_index_reg = (c_m3 == '(' || isspace(c_m3)) && (c_m2 == 'I') && (c_m1 == 'X' || c_m1 == 'Y');
			if (
				!is_index_reg &&
				isspace(str_retrive(line, i)) &&
				(str_retrive(line, i + 1) == '-' || str_retrive(line, i + 1) == '+')
			) {
				continue;
			}
			result += line[i];
		}
		line = result;
	}
	/* check if there are no printable characters */ {
		bool has_graph = false;
		for (size_t i = 0; i < line.size(); i++) {
			if (isgraph(line[i])) {
				has_graph = true;
				break;
			}
		}
		if (has_graph == false) {
			line = "";
		}
	}
}

string get_first_word(string& line) {
	string firstWord = "";
	bool wordStarted = false;

	for (char ch : line) {
		if (isalpha(ch)) {
			firstWord += toupper(ch);
			wordStarted = true;
			continue;
		}
		if (wordStarted) {
			break; // end of word
		}
	}
	return firstWord;
}

bool is_label_valid(const string& str) {
	if (str.length() <= 1) {
		return false;
	}
	return true;
	#if 0
		// currently just checks if the first invalid character is ':'
		char const * const valid_chars =
			"abcdefghijklmnopqrstuvwxyz" \
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
			"0123456789" \
			"_-+.()";
		size_t result = strcspn(str.c_str(), valid_chars);
		if (str[result] == ':') {
			return true;
		}
		return false;
	#endif
}

string visualize_escape_codes(const string& str) {
	string ret = "";
    for (char ch : str) {
		if (ch == '\'') {
			ret += "\\\'";
			continue;
		}
		if (ch == '\"') {
			ret += "\\\"";
			continue;
		}
		if (ch == '\\') {
			ret += "\\\\";
			continue;
		}
        if (isprint(ch)) {
            ret += ch;
			continue;
        }
		switch (ch) {
			case '\0': { ret += "\\0"; } break;
			case '\t': { ret += "\\t"; } break;
			case '\n': { ret += "\\n"; } break;
			case '\r': { ret += "\\r"; } break;
			case '\f': { ret += "\\f"; } break;
			case '\v': { ret += "\\v"; } break;
			default: {
				char buf[10];
				snprintf(buf, sizeof(buf), "\\x%02X", ch);
				ret += buf;
			} break;
		}
    }
	return ret;
}

asm_line_type process_first_word(size_t& index, string str) {
	bool found;
	found = find_in_list(index, str, asm_directives, ARRAY_LEN(asm_directives));
	if (found) {
		return asm_line_type::directive;
	}
	found = find_in_list(index, str, instruction_name_start, ARRAY_LEN(instruction_name_start));
	if (found) {
		return asm_line_type::instruction;
	}
	return asm_line_type::blank;
}

vector<string> split_string(const string& input) {
    vector<string> words;
    std::istringstream stream(input);
    string word;

    while (stream >> word) {
		if (!word.empty()) {
        	words.push_back(word);
		}
    }

    return words;
}

void parse_line(const string& line, ez80_instruction& output) {
	output.offset = 0;
	output.value = 0;
	output.op_code = ez80_op_code::UNKNOWN;
	vector<string> words = split_string(line);
	
	if (words.empty()) {
		return;
	}
	#if 0
		for (size_t i = 0; i < words.size(); i++) {
			printf("\"%s\", ", visualize_escape_codes(words[i]).c_str());
		}
		printf("\n");
	#endif
	size_t index = 0;
	process_first_word(index, words[0]);
	using enum instruction_enum_start;
	instruction_enum_start start = (instruction_enum_start)index;
	if (words.size() == 1) {
		switch (start) {
			case NOP  : { output.op_code = ez80_op_code::NOP ; } break;
			case CCF  : { output.op_code = ez80_op_code::CCF ; } break;
			case SCF  : { output.op_code = ez80_op_code::SCF ; } break;
			case RLA  : { output.op_code = ez80_op_code::RLA ; } break;
			case RRA  : { output.op_code = ez80_op_code::RRA ; } break;
			case RLCA : { output.op_code = ez80_op_code::RLCA; } break;
			case RRCA : { output.op_code = ez80_op_code::RRCA; } break;
			case RLD  : { output.op_code = ez80_op_code::RLD ; } break;
			case RRD  : { output.op_code = ez80_op_code::RRD ; } break;
			case DAA  : { output.op_code = ez80_op_code::DAA ; } break;
			case CPL  : { output.op_code = ez80_op_code::CPL ; } break;
			case NEG  : { output.op_code = ez80_op_code::NEG ; } break;
			case EXX  : { output.op_code = ez80_op_code::EXX ; } break;
			case CPI  : { output.op_code = ez80_op_code::CPI ; } break;
			case CPIR : { output.op_code = ez80_op_code::CPIR; } break;
			case CPD  : { output.op_code = ez80_op_code::CPD ; } break;
			case CPDR : { output.op_code = ez80_op_code::CPDR; } break;
			case LDI  : { output.op_code = ez80_op_code::LDI ; } break;
			case LDIR : { output.op_code = ez80_op_code::LDIR; } break;
			case LDD  : { output.op_code = ez80_op_code::LDD ; } break;
			case LDDR : { output.op_code = ez80_op_code::LDDR; } break;
			case RET  : { output.op_code = ez80_op_code::RET ; } break;
			case RETI : { output.op_code = ez80_op_code::RETI; } break;
			case RETN : { output.op_code = ez80_op_code::RETN; } break;
			case DI   : { output.op_code = ez80_op_code::DI  ; } break;
			case EI   : { output.op_code = ez80_op_code::EI  ; } break;
			default: break;
		}
		return;
	}
	switch (start) {
		case INC     : handle_inc_dec(output, words, start); break;
		case DEC     : handle_inc_dec(output, words, start); break;
		case INC_SIS : handle_inc_dec(output, words, start); break;
		case DEC_SIS : handle_inc_dec(output, words, start); break;
		case MLT     : handle_mlt(output, words, start); break;
		case ADC     : handle_acc(output, words, start); break;
		case ADD     : handle_acc(output, words, start); break;
		case ADC_SIS : handle_acc(output, words, start); break;
		case ADD_SIS : handle_acc(output, words, start); break;
		case AND     : handle_acc8(output, words, start); break;
		case BIT     : handle_bit_manipulation(output, words, start); break;
		case CALL    : handle_call_jump(output, words, start); break;
		case CP      : handle_acc8(output, words, start); break;
		case DJNZ    : handle_call_jump(output, words, start); break;
		case EX      : handle_ex(output, words, start); break;
		case JP      : handle_call_jump(output, words, start); break;
		case JQ      : handle_call_jump(output, words, start); break;
		case JR      : handle_call_jump(output, words, start); break;
		case LD      : handle_load(output, words, start); break;
		case LD_SIS  : handle_load(output, words, start); break;
		case LEA     : handle_lea(output, words, start); break;
		case OR      : handle_acc8(output, words, start); break;
		case PEA     : handle_pea(output, words, start); break;
		case POP     : handle_push_pop(output, words, start); break;
		case PUSH    : handle_push_pop(output, words, start); break;
		case RES     : handle_bit_manipulation(output, words, start); break;
		case RET     : handle_ret_cc(output, words, start); break;
		case RL      : handle_shift8(output, words, start); break;
		case RLC     : handle_shift8(output, words, start); break;
		case RR      : handle_shift8(output, words, start); break;
		case RRC     : handle_shift8(output, words, start); break;
		case SBC     : handle_acc(output, words, start); break;
		case SBC_SIS : handle_acc(output, words, start); break;
		case SET     : handle_bit_manipulation(output, words, start); break;
		case SLA     : handle_shift8(output, words, start); break;
		case SRA     : handle_shift8(output, words, start); break;
		case SRL     : handle_shift8(output, words, start); break;
		case SUB     : handle_acc8(output, words, start); break;
		case TST     : handle_acc8(output, words, start); break;
		case XOR     : handle_acc8(output, words, start); break;
		default: break;
	}
} 

bool construct_asm_line(string& line, size_t line_number, asm_line& output) {
	// is this line a label?
	if (line.back() == ':') {
		if (!is_label_valid(line)) {
			printf(
				"Warning(L%zu): Invalid label: \"%s\"\n",
				line_number,
				visualize_escape_codes(line).c_str()
			);
			return false;
		}
		output.line_type = asm_line_type::label;
		output.text = line;
		// printf("Label(%zu)\n", line_number);
		return true;
	}
	// is this line an instruction?
	string first_word = get_first_word(line);
	if (first_word.empty()) {
		printf("Empty(%zu)\n", line_number);
		return false;
	}
	size_t index;
	asm_line_type line_type = process_first_word(index, line);
	if (line_type == asm_line_type::blank) {
		printf(
			"Warning(L%zu): Blank: \"%s\"\n",
			line_number,
			visualize_escape_codes(line).c_str()
		);
		return false;
	}
	if (line_type == asm_line_type::directive) {
		// printf("Directive: %s\n", asm_directives[index]);
		return true;
	}
	if (line_type == asm_line_type::instruction) {
		parse_line(line, output.instruction);
		if (output.instruction.op_code != ez80_op_code::UNKNOWN) {
			// printf("\t%s\n", instruction_to_string(output.instruction).c_str());
			return true;
		} else {
			#if 1
				vector<string> words = split_string(line);
				for (size_t i = 0; i < words.size(); i++) {
					printf("\"%s\", ", visualize_escape_codes(words[i]).c_str());
				}
				printf("\n");
			#endif
			printf(
				"Warning(L%zu): Unknown instruction: \"%s\"\n",
				line_number,
				visualize_escape_codes(line).c_str()
			);
			return false;
		}
	}
	return false;
}

void parse_asm(const string& input, vector<asm_line>& output) {
	std::istringstream stream(input);
	string line;
	size_t line_number = 0;

	while (std::getline(stream, line)) {
		++line_number;
		clean_string(line);
		if (line.empty()) {
			continue;
		}
		asm_line new_line;
		new_line.line_number = line_number;
		bool valid = construct_asm_line(line, line_number, new_line);
		if (!valid) {
			continue;
		}
		output.push_back(new_line);
	}
}

enum Instruction_Type {
	/* multiple width arithmetic */
	ADD_8,
	ADD_16,
	ADD_24,
	ADC_8,
	ADC_16,
	ADC_24,
	SBC_8,
	SBC_16,
	SBC_24,
	/* accumulator */
	SUB,
	CP,
	AND,
	OR,
	XOR,
	TST,
	/* increment/decrement */
	INC_8,
	INC_16,
	INC_24,
	DEC_8,
	DEC_16,
	DEC_24,
	/* bitwise */
	BIT,
	SET,
	RES,
	/* unary */
	NOP,
	CCF,
	SCF,
	CPL,
	DAA,
	NEG,
	RLA,
	RRA,
	RLCA,
	RRCA,
	/* multiply */
	MLT,
	/* shifts */
	SLA,
	SRA,
	SRL,
	RL,
	RR,
	RLC,
	RRC,
	RLD,
	RRD,
	/* block functions */
	CPI,
	CPIR,
	CPD,
	CPDR,
	LDI,
	LDIR,
	LDD,
	LDDR,
	/* control flow */
	JP_REG,
	JP,
	JP_CC,
	CALL,
	CALL_CC,
	JR,
	JR_CC,
	DJNZ,
	/* return */
	RET,
	RET_CC,
	/* exchange */
	EX_DE_HL,
	EXX,
	EX_AF,
	/* stack operations */
	PUSH,
	POP,
	EX_SP,
	PEA,
	/* load */
	LEA,
	LD_REG_8,
	LD_REG_16,
	LD_REG_24,
	LD_ADDR_16,
	LD_ADDR_24,
	LD_INDR,
};

#include "input_asm.h"

// string input_asm = "111";

int main(void) {
	printf("asm parse\n");
	parse_asm(input_asm, asm_text);
	printf("finished\n");
	return 0;
}
