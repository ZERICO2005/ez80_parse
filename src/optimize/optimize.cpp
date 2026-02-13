#include "../ez80_parse.h"
#include "../ez80_known_function.h"
#include "../ez80_input_reg.h"
#include "../ez80_output_reg.h"
#include "current_state.hpp"
#include "addmul_chain.h"

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

bool is_input_reg(ez80_op_code op_code, ez80_reg_field_mask r) {
	for (size_t i = 0; i < ARRAY_LEN(input_reg); i++) {
		if (input_reg[i].op_code == op_code) {
			return (input_reg[i].field.raw & (uint32_t)r);
		}
	}
	return false;
}

bool is_output_reg(ez80_op_code op_code, ez80_reg_field_mask r) {
	for (size_t i = 0; i < ARRAY_LEN(output_reg); i++) {
		if (output_reg[i].op_code == op_code) {
			return (output_reg[i].field.raw & (uint32_t)r);
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

char const * const reg_timeline_text[] = {
	"unknown",
	"destroyed",
	"observed",
	"modified",
	"preserved",
};

const char* reg_timeline_str(reg_timeline x) {
	return reg_timeline_text[(size_t)x];
}

reg_timeline get_reg_future(
	ez80_reg_field_mask r,
	const list<ez80_code>& prog,
	list<ez80_code>::iterator& it,
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
	ez80_reg_field_mask r,
	const list<ez80_code>& prog,
	list<ez80_code>::iterator& it
) {
	return get_reg_future(r, prog, it, prog.end());
}

reg_timeline get_reg_past(
	ez80_reg_field_mask r,
	const list<ez80_code>& prog,
	list<ez80_code>::iterator& it,
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
	ez80_reg_field_mask r,
	const list<ez80_code>& prog,
	list<ez80_code>::iterator& it
) {
	return get_reg_past(r, prog, it, prog.begin());
}

ez80_code make_code_from_op_code(ez80_op_code op_code) {
	ez80_code code;
	code.original_line = 0;
	code.type = ez80_code_type::code;
	code.instruction.op_code = op_code;
	return code;
}

void insert_op_code(
	list<ez80_code>& prog,
	list<ez80_code>::iterator& it,
	initializer_list<ez80_op_code> replacement
) {
	for (const ez80_op_code& op_code : replacement) {
		ez80_code code;
		code.original_line = 0;
		code.type = ez80_code_type::code;
		code.instruction.op_code = op_code;
		it = prog.insert(it, code);
		++it;
	}
}

void instruction_insert_repeat(
	list<ez80_code>& prog,
	list<ez80_code>::iterator& it,
	ez80_code instruction,
	int count
) {
	for (uint8_t i = 0; i < count; i++) {
		it = prog.insert(it, instruction);
		++it;
	}
}

void optimize_ishl(
	ez80_code current,
	list<ez80_code>& prog,
	list<ez80_code>::iterator& it,
	current_state state
) {
	if (!state.C.isknown_fully()) {
		/* nothing we can do */
		return;
	}
	const uint8_t shift = state.C.bits;
	if (shift > 6) {
		/* too large to inline */
		return;
	}

	using enum reg_timeline;
	list<ez80_code>::iterator past_it = it;
	list<ez80_code>::iterator future_it = it;
	reg_timeline shift_past = get_reg_past(ez80_reg_field_mask::C, prog, past_it);
	reg_timeline shift_future = get_reg_future(ez80_reg_field_mask::C, prog, future_it);
	uint8_t budget = 4;
	using enum ez80_op_code;
	if ((shift_past == destroyed) && (shift_future == destroyed)) {
		bool removed_code = true;
		switch (past_it->instruction.op_code) {
			case LD_C_N:
			case LD_C_IXL:
			case LD_C_IXH:
			case LD_C_IYL:
			case LD_C_IYH:
			{ budget = 6; } break;
			case LD_C_A:
			case LD_C_B:
			case LD_C_D:
			case LD_C_E:
			{ budget = 5; } break;
			default: { removed_code = false; } break;
		}
		if (shift > budget) {
			return;
		}
		if (removed_code) {
			prog.erase(past_it);
		}
	}
	if (shift > budget) {
		return;
	}
	it = prog.erase(it);
	ez80_code instruction = make_code_from_op_code(ADD_HL_HL);
	instruction_insert_repeat(prog, it, instruction, shift);
}

void optimize_imulu(
	ez80_code current,
	list<ez80_code>& prog,
	list<ez80_code>::iterator& it,
	current_state state
) {
	if (!state.get_BC().isknown_fully()) {
		/* nothing we can do */
		return;
	}
	const uint24_t mult = state.get_BC().bits;
	const int cost = 2 + get_addmul_cost(mult);
	/* trivial cases and powers of two shouldn't be here anyways */
	if (mult == 0 || has_single_bit(mult)) {
		/* use __ishl instead or etc */
		return;
	}
	if (cost > 8) {
		return;
	}
	using enum reg_timeline;
	list<ez80_code>::iterator past_it = it;
	list<ez80_code>::iterator future_it = it;
	reg_timeline shift_past = get_reg_past(ez80_reg_field_mask::BC, prog, past_it);
	reg_timeline shift_future = get_reg_future(ez80_reg_field_mask::BC, prog, future_it);
	uint8_t budget = 4;
	using enum ez80_op_code;
	if ((shift_past == destroyed) && (shift_future == destroyed)) {
		bool removed_code = true;
		switch (past_it->instruction.op_code) {
			case LD_BC_N:
			case LEA_BC_IX_SIS:
			case LEA_BC_IY_SIS:
			{ budget = 8; } break;
			case LEA_BC_IX:
			case LEA_BC_IY:
			{ budget = 7; } break;
			default: { removed_code = false; } break;
		}
		if (cost > budget) {
			return;
		}
		if (removed_code) {
			prog.erase(past_it);
		}
	}
	if (cost > budget) {
		return;
	}

	it = prog.erase(it);
	if (state.UBC.isknown_zero() && state.UHL.isknown_zero()) {
		insert_op_code(prog, it,
			{LD_B_H, LD_C_L}
		);
	} else {
		insert_op_code(prog, it,
			{PUSH_HL, POP_BC}
		);
	}
	int test_cost = 2;
	uint32_t m = mult;
	uint32_t b = UINT32_C(1) << countr_zero(std::bit_floor(m));
	b >>= 1;
	while (b != 0) {
		insert_op_code(prog, it, {ADD_HL_HL});
		test_cost++;
		if (m & b) {
			insert_op_code(prog, it, {ADD_HL_BC});
			test_cost++;
		}
		b >>= 1;
	}
	if (test_cost != cost) {
		printf("Cost: T %d != G %d\n", cost, test_cost);
	}
}

#if 0
void optimize_iand(
	ez80_code current,
	list<ez80_code>& prog,
	list<ez80_code>::iterator& it,
	current_state state
) {
	if (!state.get_BC().isknown_fully()) {
		return;
	}

	using enum reg_timeline;
	list<ez80_code>::iterator past_it = it;
	list<ez80_code>::iterator future_it = it;
	reg_timeline value_past = get_reg_past(ez80_reg_field_mask::BC, prog, past_it);
	reg_timeline value_future = get_reg_future(ez80_reg_field_mask::BC, prog, future_it);
	using enum ez80_op_code;
	if ((value_past == destroyed) && (value_future == destroyed)) {
		printf(
			"%6zu: %s\n",
			current.original_line,
			state.print_state().c_str()
		);
	}
}
#endif

void optimize_asm(ez80_code_section& program) {
	list<ez80_code>& prog = program.prog;
	current_state state;
	state.set_all_reg_unknown();
	for (list<ez80_code>::iterator it = prog.begin(); it != prog.end();) {
		using enum ez80_code_type;
		using enum ez80_op_code;
		ez80_code current = *it;

		switch (current.type) {
			case known_func: {
				using enum ez80_known_function;
				switch (current.known_func) {
					case _fabs:
					case _fabsf:
					{
						// it = prog.erase(it);
						// insert_op_code(prog, it, {POP_DE, RES_7_E, PUSH_DE});
						// continue;
					} break;
					case _fabsl:
					{
						// it = prog.erase(it);
						// insert_op_code(prog, it, {POP_BC, RES_7_B, PUSH_BC});
						// continue;
					} break;
					case __imulu:
					{
						optimize_imulu(current, prog, it, state);
					} break;
					case __ishl:
					{
						optimize_ishl(current, prog, it, state);
					} break;
					case __iand:
					{
						// optimize_iand(current, prog, it, state);
					} break;
					default: break;
				}
				// printf(
				// 	"%6zu: %s\n",
				// 	current.original_line,
				// 	ez80_known_function_name[(size_t)current.known_func]
				// );
				state.next_known_func(current.instruction, current.known_func);
				printf(
					"%6zu: %s | %s\n",
					current.original_line,
					state.print_state().c_str(),
					instruction_to_string(current.instruction).c_str()
				);
			} break;
			case branch:
			{
				switch (current.instruction.op_code) {
					using enum ez80_op_code;
					case CALL:
					case CALL_C:
					case CALL_NC:
					case CALL_Z:
					case CALL_NZ:
					case CALL_P:
					case CALL_M:
					case CALL_PO:
					case CALL_PE:
					{ state.set_just_cxx_reg_unknown(); } break;
					default: break;
				}
			}
			[[fallthrough]];
			case code:
			{
				state.next_instruction(current.instruction);
				printf(
					"%6zu: %s | %s\n",
					current.original_line,
					state.print_state().c_str(),
					instruction_to_string(current.instruction).c_str()
				);
			} break;
			case label:
			{
				state.set_all_reg_unknown();

				printf("%6zu: %s:\n", current.original_line, current.label.text.c_str());
			} break;
			default: break;
		}
		++it;
	}
}
