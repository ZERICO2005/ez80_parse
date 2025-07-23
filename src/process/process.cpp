#include "../ez80_parse.h"
#include "../ez80_known_function.h"
#include "../ez80_instruction.h"
#include "../text_util.h"

bool setup_code(ez80_code& code, const asm_line& line) {
	code.original_line = line.line_number;
	switch (line.line_type) {
		using enum asm_line_type;
		default: return false;
		case directive: {
			code.type = ez80_code_type::directive;
			code.label.text = line.text;
		} break;
		case label: {
			code.type = ez80_code_type::label;
			code.label.text = line.text;
		} break;
		case instruction: {
			code.type = ez80_code_type::code;
			code.instruction = line.instruction;
			switch (code.instruction.op_code) {
				using enum ez80_op_code;
				default: break;
				case JR:
				case JR_C:
				case JR_NC:
				case JR_Z:
				case JR_NZ:
				case JP:
				case JP_C:
				case JP_NC:
				case JP_Z:
				case JP_NZ:
				case JP_P:
				case JP_M:
				case JP_PO:
				case JP_PE:
				case CALL:
				case CALL_C:
				case CALL_NC:
				case CALL_Z:
				case CALL_NZ:
				case CALL_P:
				case CALL_M:
				case CALL_PO:
				case CALL_PE:
				code.type = ez80_code_type::branch;
			}
		} break;
	}
	return true;
}

void set_known_function(ez80_code& code) {
	using enum ez80_op_code;
	if (code.instruction.op_code != CALL) {
		return;
	}
	size_t index;
	bool found;
	const string& str = code.instruction.symbol;
	if (str.empty()) {
		return;
	}
	found = find_in_list(index, str, ez80_known_function_name, ARRAY_LEN(ez80_known_function_name));
	if (found) {
		code.type = ez80_code_type::known_func;
		code.known_func = (ez80_known_function)index;
		// printf("[%s]\n", ez80_known_function_name[index]);
	}
}

void process_asm(ez80_code_section& output, const ez80_program& program) {
	const vector<asm_line>& prog = program.prog;
	for (size_t i = 0; i < prog.size(); i++) {
		const asm_line& line = prog[i];
		ez80_code code;
		bool worked = setup_code(code, line);
		if (worked == false) {
			printf("L%zu: failed to process\n", line.line_number);
			continue;
		}
		set_known_function(code);
		output.prog.push_back(code);
	}
}
