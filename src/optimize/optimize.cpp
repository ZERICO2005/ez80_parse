#include "../ez80_parse.h"
#include "../ez80_known_function.h"


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
					default:
				}
			} break;
			default:
		}
		++it;
	}
}
