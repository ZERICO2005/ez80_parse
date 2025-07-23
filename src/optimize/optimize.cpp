#include "../ez80_parse.h"
#include "../ez80_known_function.h"
#include "../ez80_input_reg.h"
#include "../ez80_output_reg.h"
#include "current_state.hpp"

bool is_input_flag(ez80_op_code op_code, ez80_flag_bit f) {
	for (size_t i = 0; i < ARRAY_LEN(input_reg); i++) {
		if (input_reg[i].op_code == op_code) {
			return (input_reg[i].field.flag.raw & (1 << (int)f));
		}
	}
	return false;
}

bool is_output_flag(ez80_op_code op_code, ez80_flag_bit f) {
	for (size_t i = 0; i < ARRAY_LEN(output_reg); i++) {
		if (output_reg[i].op_code == op_code) {
			return (output_reg[i].field.flag.raw & (1 << (int)f));
		}
	}
	return false;
}

bool is_input_reg(ez80_op_code op_code, ez80_reg_field_bit r) {
	for (size_t i = 0; i < ARRAY_LEN(input_reg); i++) {
		if (input_reg[i].op_code == op_code) {
			return (input_reg[i].field.raw & (1 << (int)r));
		}
	}
	return false;
}

bool is_output_reg(ez80_op_code op_code, ez80_reg_field_bit r) {
	for (size_t i = 0; i < ARRAY_LEN(output_reg); i++) {
		if (output_reg[i].op_code == op_code) {
			return (output_reg[i].field.raw & (1 << (int)r));
		}
	}
	return false;
}

enum class reg_timeline {
	unknown,
	destroyed,
	observed,
	modified,
	preserved,
};

reg_timeline get_reg_future(
	ez80_reg_field_bit r,
	const list<ez80_code>& prog,
	list<ez80_code>::const_iterator it,
	const list<ez80_code>::const_iterator it_end
) {
	using enum reg_timeline;
	for (; it != prog.end() && it != it_end;) {
		++it;
		ez80_code c = *it;
		if (c.type != ez80_code_type::code) {
			return unknown;
		}
		bool is_output = is_output_reg(c.instruction.op_code, r);
		bool is_input = is_input_reg(c.instruction.op_code, r);
		if (is_output || is_input) {
			if (is_output) {
				if (is_input) {
					return modified;
				}
				return destroyed;
			}
			return observed;
		}
	}
	return preserved;
}

reg_timeline get_reg_future(
	ez80_reg_field_bit r,
	const list<ez80_code>& prog,
	list<ez80_code>::const_iterator it
) {
	return get_reg_future(r, prog, it, prog.end());
}

reg_timeline get_reg_past(
	ez80_reg_field_bit r,
	const list<ez80_code>& prog,
	list<ez80_code>::const_iterator it,
	const list<ez80_code>::const_iterator it_end
) {
	using enum reg_timeline;
	for (; it != prog.begin() && it != it_end;) {
		--it;
		ez80_code c = *it;
		if (c.type != ez80_code_type::code) {
			return unknown;
		}
		bool is_output = is_output_reg(c.instruction.op_code, r);
		bool is_input = is_input_reg(c.instruction.op_code, r);
		if (is_output || is_input) {
			if (is_output) {
				if (is_input) {
					return modified;
				}
				return destroyed;
			}
			return observed;
		}
	}
	return preserved;
}

reg_timeline get_reg_past(
	ez80_reg_field_bit r,
	const list<ez80_code>& prog,
	list<ez80_code>::const_iterator it
) {
	return get_reg_past(r, prog, it, prog.begin());
}

void insert_op_code(
	list<ez80_code>& prog,
	list<ez80_code>::iterator& it,
	initializer_list<ez80_op_code> replacement
) {
	for (const ez80_op_code& op_code : replacement) {
		ez80_code code;
		code.type = ez80_code_type::code;
		code.instruction.op_code = op_code;
		it = prog.insert(it, code);
		++it;
	}
}

void optimize_asm(ez80_code_section& program) {
	list<ez80_code>& prog = program.prog;
	for (list<ez80_code>::iterator it = prog.begin(); it != prog.end();) {
		using enum ez80_code_type;
		using enum ez80_op_code;
		ez80_code current = *it;
		switch (current.type) {
			case known_func: {
				using enum ez80_known_function;
				switch (current.known_func) {
					case _fabs:
					case _fabsf: {
						it = prog.erase(it);
						insert_op_code(prog, it, {POP_DE, RES_7_E, PUSH_DE});
						continue;
					} break;
					case _fabsl: {
						it = prog.erase(it);
						insert_op_code(prog, it, {POP_BC, RES_7_B, PUSH_BC});
						continue;
					} break;
					default: break;
				}
			} break;
			default: break;
		}
		++it;
	}
}
