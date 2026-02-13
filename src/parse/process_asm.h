#ifndef PROCESS_ASM_H
#define PROCESS_ASM_H

#include "../ez80_instruction.h"
#include "../ez80_instruction_info.h"
#include "../text.h"
#include "../text_util.h"


void clean_symbol_str(string& str) {
	if (str.front() == '(' && str.back() == ')') {
		str.erase(std::remove(str.begin(), str.end(), '('), str.end());
		str.erase(std::remove(str.begin(), str.end(), ')'), str.end());
	}
}

void set_index_register_offset(ez80_instruction& output, const vector<string>& str, size_t offset) {
	if (offset >= str.size()) {
		if (offset == str.size()) {
			output.offset = 0;
			output.known_value = true;
		}
		return;
	}
	char * str_start = (char*)str[offset].c_str();
	char * str_end;
	output.offset = (int8_t)strtol(str_start, &str_end, 10);
	if (str_start == str_end) {
		output.known_value = false;
		output.symbol = str_start;
		clean_symbol_str(output.symbol);
		return;
	}
	output.known_value = true;
}


void set_imm_value(ez80_instruction& output, const vector<string>& str, size_t offset) {
	if (offset >= str.size()) {
		return;
	}
	char * str_start = (char*)str[offset].c_str();
	char * str_end;
	output.value = (uint24_t)strtol(str_start, &str_end, 10);
	if (str_start == str_end) {
		output.known_value = false;
		output.symbol = str_start;
		clean_symbol_str(output.symbol);
		return;
	}
	output.known_value = true;
}

void handle_inc_dec(
	ez80_instruction& output,
	const vector<string>& str,
	instruction_enum_start oper
) {
	if (str.size() < 2) {
		return;
	}
	size_t index;
	bool found = find_in_list(index, str[1], argument_name, ARRAY_LEN(argument_name));
	using enum ez80_argument_name;
	using enum instruction_enum_start;
	if (found == false) {
		return;
	}
	bool upward = (oper == INC || oper == INC_SIS);
	bool arg16 = (oper == INC_SIS || oper == DEC_SIS);
	switch ((ez80_argument_name)index) {
		using enum ez80_op_code;
		case A  : { output.op_code = upward ? INC_A   : DEC_A  ; } break;
		case B  : { output.op_code = upward ? INC_B   : DEC_B  ; } break;
		case C  : { output.op_code = upward ? INC_C   : DEC_C  ; } break;
		case D  : { output.op_code = upward ? INC_D   : DEC_D  ; } break;
		case E  : { output.op_code = upward ? INC_E   : DEC_E  ; } break;
		case H  : { output.op_code = upward ? INC_H   : DEC_H  ; } break;
		case L  : { output.op_code = upward ? INC_L   : DEC_L  ; } break;
		case IXL: { output.op_code = upward ? INC_IXL : DEC_IXL; } break;
		case IXH: { output.op_code = upward ? INC_IXH : DEC_IXH; } break;
		case IYL: { output.op_code = upward ? INC_IYL : DEC_IYL; } break;
		case IYH: { output.op_code = upward ? INC_IYH : DEC_IYH; } break;
		case BC : { output.op_code = arg16 ? (upward ? INC_BC_SIS  : DEC_BC_SIS) : (upward ? INC_BC  : DEC_BC); } break;
		case DE : { output.op_code = arg16 ? (upward ? INC_DE_SIS  : DEC_DE_SIS) : (upward ? INC_DE  : DEC_DE); } break;
		case HL : { output.op_code = arg16 ? (upward ? INC_HL_SIS  : DEC_HL_SIS) : (upward ? INC_HL  : DEC_HL); } break;
		case SP : { output.op_code = arg16 ? (upward ? INC_SP_SIS  : DEC_SP_SIS) : (upward ? INC_SP  : DEC_SP); } break;
		case IX : { output.op_code = arg16 ? (upward ? INC_IX_SIS  : DEC_IX_SIS) : (upward ? INC_IX  : DEC_IX); } break;
		case IY : { output.op_code = arg16 ? (upward ? INC_IY_SIS  : DEC_IY_SIS) : (upward ? INC_IY  : DEC_IY); } break;
		case PHL: { output.op_code = upward ? INC_PHL : DEC_PHL; } break;
		case PIX: {
			output.op_code = upward ? INC_PIX : DEC_PIX;
			set_index_register_offset(output, str, 3);
		} break;
		case PIY: {
			output.op_code = upward ? INC_PIX : DEC_PIX;
			set_index_register_offset(output, str, 3);
		} break;
		default: break;
	}
}

void handle_mlt(
	ez80_instruction& output,
	const vector<string>& str,
	__attribute__((unused)) instruction_enum_start oper
) {
	if (str.size() < 2) {
		return;
	}
	size_t index;
	bool found = find_in_list(index, str[1], argument_name, ARRAY_LEN(argument_name));
	using enum ez80_argument_name;
	using enum ez80_op_code;
	if (found == false) {
		return;
	}
	switch ((ez80_argument_name)index) {
		case BC : { output.op_code = MLT_BC; } break;
		case DE : { output.op_code = MLT_DE; } break;
		case HL : { output.op_code = MLT_HL; } break;
		case SP : { output.op_code = MLT_SP; } break;
		default: break;
	}
}

void handle_ex(
	ez80_instruction& output,
	const vector<string>& str,
	__attribute__((unused)) instruction_enum_start oper
) {
	if (str.size() < 3) {
		return;
	}
	size_t index_dst, index_src;
	bool found;
	found = find_in_list(index_dst, str[1], argument_name, ARRAY_LEN(argument_name));
	if (found == false) {
		return;
	}
	using enum ez80_argument_name;
	using enum instruction_enum_start;
	if ((ez80_argument_name)index_dst == DE) {
		using enum ez80_op_code;
		output.op_code = (oper == EX_SIS) ? EX_DE_HL_SIS : EX_DE_HL;
		return;
	}
	found = find_in_list(index_src, str[2], argument_name, ARRAY_LEN(argument_name));
	if (found == false) {
		return;
	}
	switch ((ez80_argument_name)index_src) {
		using enum ez80_op_code;
		case HL : { output.op_code = EX_SP_HL; } break;
		case IX : { output.op_code = EX_SP_IX; } break;
		case IY : { output.op_code = EX_SP_IY; } break;
		default: break;
	}
}

void handle_ret_cc(
	ez80_instruction& output,
	const vector<string>& str,
	__attribute__((unused)) instruction_enum_start oper
) {
	if (str.size() < 2) {
		return;
	}
	size_t index;
	bool found = find_in_list(index, str[1], condition_code_name, ARRAY_LEN(condition_code_name));
	using enum ez80_condition_code;
	using enum ez80_op_code;
	if (found == false) {
		return;
	}
	switch ((ez80_condition_code)index) {
		case C  : { output.op_code = RET_C ; } break;
		case NC : { output.op_code = RET_NC; } break;
		case Z  : { output.op_code = RET_Z ; } break;
		case NZ : { output.op_code = RET_NZ; } break;
		case P  : { output.op_code = RET_P ; } break;
		case M  : { output.op_code = RET_M ; } break;
		case PO : { output.op_code = RET_PO; } break;
		case PE : { output.op_code = RET_PE; } break;
		default: break;
	}
}

void handle_call_jump(
	ez80_instruction& output,
	const vector<string>& str,
	__attribute__((unused)) instruction_enum_start oper
) {
	if (str.size() < 2) {
		return;
	}
	if (str.size() == 2) {
		using enum instruction_enum_start;
		switch (oper) {
			case DJNZ : { output.op_code = ez80_op_code::DJNZ ; } break;
			case JR   : { output.op_code = ez80_op_code::JR   ; } break;
			case JQ   : { output.op_code = ez80_op_code::JP   ; } break;
			case JP   : { output.op_code = ez80_op_code::JP   ; } break;
			case CALL : { output.op_code = ez80_op_code::CALL ; } break;
			default: break;
		}
		set_imm_value(output, str, 1);
		return;
	}
	size_t index;
	bool found = find_in_list(index, str[1], condition_code_name, ARRAY_LEN(condition_code_name));
	using enum ez80_condition_code;
	using enum ez80_op_code;
	if (found == false) {
		return;
	}
	ez80_condition_code cc = (ez80_condition_code)index;
	switch (oper) {
		using enum instruction_enum_start;
		case JR: {
			switch (cc) {
				case C  : { output.op_code = JR_C ; } break;
				case NC : { output.op_code = JR_NC; } break;
				case Z  : { output.op_code = JR_Z ; } break;
				case NZ : { output.op_code = JR_NZ; } break;
				default: break;
			}
			set_imm_value(output, str, 2);
		} break;
		case JQ:
		case JP: {
			switch (cc) {
				case C  : { output.op_code = JP_C ; } break;
				case NC : { output.op_code = JP_NC; } break;
				case Z  : { output.op_code = JP_Z ; } break;
				case NZ : { output.op_code = JP_NZ; } break;
				case P  : { output.op_code = JP_P ; } break;
				case M  : { output.op_code = JP_M ; } break;
				case PO : { output.op_code = JP_PO; } break;
				case PE : { output.op_code = JP_PE; } break;
				default: break;
			}
			set_imm_value(output, str, 2);
		} break;
		case CALL: {
			switch (cc) {
				case C  : { output.op_code = CALL_C ; } break;
				case NC : { output.op_code = CALL_NC; } break;
				case Z  : { output.op_code = CALL_Z ; } break;
				case NZ : { output.op_code = CALL_NZ; } break;
				case P  : { output.op_code = CALL_P ; } break;
				case M  : { output.op_code = CALL_M ; } break;
				case PO : { output.op_code = CALL_PO; } break;
				case PE : { output.op_code = CALL_PE; } break;
				default: break;
			}
			set_imm_value(output, str, 2);
		} break;
		default: break;
	}
}

void handle_push_pop(
	ez80_instruction& output,
	const vector<string>& str,
	instruction_enum_start oper
) {
	if (str.size() < 2) {
		return;
	}
	size_t index;
	bool found = find_in_list(index, str[1], argument_name, ARRAY_LEN(argument_name));
	using enum ez80_argument_name;

	using enum instruction_enum_start;
	if (found == false) {
		return;
	}
	bool pushing = oper == PUSH;
	switch ((ez80_argument_name)index) {
		using enum ez80_op_code;
		case AF : { output.op_code = pushing ? PUSH_AF : POP_AF; } break;
		case BC : { output.op_code = pushing ? PUSH_BC : POP_BC; } break;
		case DE : { output.op_code = pushing ? PUSH_DE : POP_DE; } break;
		case HL : { output.op_code = pushing ? PUSH_HL : POP_HL; } break;
		case IX : { output.op_code = pushing ? PUSH_IX : POP_IX; } break;
		case IY : { output.op_code = pushing ? PUSH_IY : POP_IY; } break;
		default: break;
	}
}

void handle_shift8(
	ez80_instruction& output,
	const vector<string>& str,
	instruction_enum_start oper
) {
	if (str.size() < 2) {
		return;
	}
	size_t index;
	bool found = find_in_list(index, str[1], argument_name, ARRAY_LEN(argument_name));
	using enum ez80_argument_name;
	using enum instruction_enum_start;
	if (found == false) {
		return;
	}
	ez80_argument_name arg = (ez80_argument_name)index;
	switch (arg) {
		using enum ez80_op_code;
		case A: {
			switch (oper) {
				case RL : { output.op_code =  RL_A; } break;
				case RLC: { output.op_code = RLC_A; } break;
				case RR : { output.op_code =  RR_A; } break;
				case RRC: { output.op_code = RRC_A; } break;
				case SLA: { output.op_code = SLA_A; } break;
				case SRA: { output.op_code = SRA_A; } break;
				case SRL: { output.op_code = SRL_A; } break;
				default: break;
			}
		} break;
		case B: {
			switch (oper) {
				case RL : { output.op_code =  RL_B; } break;
				case RLC: { output.op_code = RLC_B; } break;
				case RR : { output.op_code =  RR_B; } break;
				case RRC: { output.op_code = RRC_B; } break;
				case SLA: { output.op_code = SLA_B; } break;
				case SRA: { output.op_code = SRA_B; } break;
				case SRL: { output.op_code = SRL_B; } break;
				default: break;
			}
		} break;
		case C: {
			switch (oper) {
				case RL : { output.op_code =  RL_C; } break;
				case RLC: { output.op_code = RLC_C; } break;
				case RR : { output.op_code =  RR_C; } break;
				case RRC: { output.op_code = RRC_C; } break;
				case SLA: { output.op_code = SLA_C; } break;
				case SRA: { output.op_code = SRA_C; } break;
				case SRL: { output.op_code = SRL_C; } break;
				default: break;
			}
		} break;
		case D: {
			switch (oper) {
				case RL : { output.op_code =  RL_D; } break;
				case RLC: { output.op_code = RLC_D; } break;
				case RR : { output.op_code =  RR_D; } break;
				case RRC: { output.op_code = RRC_D; } break;
				case SLA: { output.op_code = SLA_D; } break;
				case SRA: { output.op_code = SRA_D; } break;
				case SRL: { output.op_code = SRL_D; } break;
				default: break;
			}
		} break;
		case E: {
			switch (oper) {
				case RL : { output.op_code =  RL_E; } break;
				case RLC: { output.op_code = RLC_E; } break;
				case RR : { output.op_code =  RR_E; } break;
				case RRC: { output.op_code = RRC_E; } break;
				case SLA: { output.op_code = SLA_E; } break;
				case SRA: { output.op_code = SRA_E; } break;
				case SRL: { output.op_code = SRL_E; } break;
				default: break;
			}
		} break;
		case H: {
			switch (oper) {
				case RL : { output.op_code =  RL_H; } break;
				case RLC: { output.op_code = RLC_H; } break;
				case RR : { output.op_code =  RR_H; } break;
				case RRC: { output.op_code = RRC_H; } break;
				case SLA: { output.op_code = SLA_H; } break;
				case SRA: { output.op_code = SRA_H; } break;
				case SRL: { output.op_code = SRL_H; } break;
				default: break;
			}
		} break;
		case L: {
			switch (oper) {
				case RL : { output.op_code =  RL_L; } break;
				case RLC: { output.op_code = RLC_L; } break;
				case RR : { output.op_code =  RR_L; } break;
				case RRC: { output.op_code = RRC_L; } break;
				case SLA: { output.op_code = SLA_L; } break;
				case SRA: { output.op_code = SRA_L; } break;
				case SRL: { output.op_code = SRL_L; } break;
				default: break;
			}
		} break;
		case PHL: {
			switch (oper) {
				case RL : { output.op_code =  RL_PHL; } break;
				case RLC: { output.op_code = RLC_PHL; } break;
				case RR : { output.op_code =  RR_PHL; } break;
				case RRC: { output.op_code = RRC_PHL; } break;
				case SLA: { output.op_code = SLA_PHL; } break;
				case SRA: { output.op_code = SRA_PHL; } break;
				case SRL: { output.op_code = SRL_PHL; } break;
				default: break;
			}
		} break;
		case PIX: {
			switch (oper) {
				case RL : { output.op_code =  RL_PIX; } break;
				case RLC: { output.op_code = RLC_PIX; } break;
				case RR : { output.op_code =  RR_PIX; } break;
				case RRC: { output.op_code = RRC_PIX; } break;
				case SLA: { output.op_code = SLA_PIX; } break;
				case SRA: { output.op_code = SRA_PIX; } break;
				case SRL: { output.op_code = SRL_PIX; } break;
				default: break;
			}
			set_index_register_offset(output, str, 2);
		} break;
		case PIY: {
			switch (oper) {
				case RL : { output.op_code =  RL_PIY; } break;
				case RLC: { output.op_code = RLC_PIY; } break;
				case RR : { output.op_code =  RR_PIY; } break;
				case RRC: { output.op_code = RRC_PIY; } break;
				case SLA: { output.op_code = SLA_PIY; } break;
				case SRA: { output.op_code = SRA_PIY; } break;
				case SRL: { output.op_code = SRL_PIY; } break;
				default: break;
			}
			set_index_register_offset(output, str, 2);
		} break;
		default: break;
	}
}

void handle_acc8(
	ez80_instruction& output,
	const vector<string>& str,
	instruction_enum_start oper
) {
	if (str.size() < 3) {
		return;
	}

	using enum ez80_argument_name;
	using enum instruction_enum_start;
	size_t index;
	bool found;
	found = find_in_list(index, str[2], argument_name, ARRAY_LEN(argument_name));
	if (found == false) {
		using enum ez80_op_code;
		set_imm_value(output, str, 2);
		switch (oper) {
			case ADC: { output.op_code = ADC_A_N; } break;
			case ADD: { output.op_code = ADD_A_N; } break;
			case AND: { output.op_code = AND_A_N; } break;
			case CP : { output.op_code =  CP_A_N; } break;
			case OR : { output.op_code =  OR_A_N; } break;
			case SBC: { output.op_code = SBC_A_N; } break;
			case SUB: { output.op_code = SUB_A_N; } break;
			case XOR: { output.op_code = XOR_A_N; } break;
			default: break;
		}
	}
	ez80_argument_name arg = (ez80_argument_name)index;
	switch (arg) {
		using enum ez80_op_code;
		case A: {
			switch (oper) {
				case ADC: { output.op_code = ADC_A_A; } break;
				case ADD: { output.op_code = ADD_A_A; } break;
				case AND: { output.op_code = AND_A_A; } break;
				case CP : { output.op_code =  CP_A_A; } break;
				case OR : { output.op_code =  OR_A_A; } break;
				case SBC: { output.op_code = SBC_A_A; } break;
				case SUB: { output.op_code = SUB_A_A; } break;
				case TST: { output.op_code = TST_A_A; } break;
				case XOR: { output.op_code = XOR_A_A; } break;
				default: break;
			}
		} break;
		case B: {
			switch (oper) {
				case ADC: { output.op_code = ADC_A_B; } break;
				case ADD: { output.op_code = ADD_A_B; } break;
				case AND: { output.op_code = AND_A_B; } break;
				case CP : { output.op_code =  CP_A_B; } break;
				case OR : { output.op_code =  OR_A_B; } break;
				case SBC: { output.op_code = SBC_A_B; } break;
				case SUB: { output.op_code = SUB_A_B; } break;
				case TST: { output.op_code = TST_A_B; } break;
				case XOR: { output.op_code = XOR_A_B; } break;
				default: break;
			}
		} break;
		case C: {
			switch (oper) {
				case ADC: { output.op_code = ADC_A_C; } break;
				case ADD: { output.op_code = ADD_A_C; } break;
				case AND: { output.op_code = AND_A_C; } break;
				case CP : { output.op_code =  CP_A_C; } break;
				case OR : { output.op_code =  OR_A_C; } break;
				case SBC: { output.op_code = SBC_A_C; } break;
				case SUB: { output.op_code = SUB_A_C; } break;
				case TST: { output.op_code = TST_A_C; } break;
				case XOR: { output.op_code = XOR_A_C; } break;
				default: break;
			}
		} break;
		case D: {
			switch (oper) {
				case ADC: { output.op_code = ADC_A_D; } break;
				case ADD: { output.op_code = ADD_A_D; } break;
				case AND: { output.op_code = AND_A_D; } break;
				case CP : { output.op_code =  CP_A_D; } break;
				case OR : { output.op_code =  OR_A_D; } break;
				case SBC: { output.op_code = SBC_A_D; } break;
				case SUB: { output.op_code = SUB_A_D; } break;
				case TST: { output.op_code = TST_A_D; } break;
				case XOR: { output.op_code = XOR_A_D; } break;
				default: break;
			}
		} break;
		case E: {
			switch (oper) {
				case ADC: { output.op_code = ADC_A_E; } break;
				case ADD: { output.op_code = ADD_A_E; } break;
				case AND: { output.op_code = AND_A_E; } break;
				case CP : { output.op_code =  CP_A_E; } break;
				case OR : { output.op_code =  OR_A_E; } break;
				case SBC: { output.op_code = SBC_A_E; } break;
				case SUB: { output.op_code = SUB_A_E; } break;
				case TST: { output.op_code = TST_A_E; } break;
				case XOR: { output.op_code = XOR_A_E; } break;
				default: break;
			}
		} break;
		case H: {
			switch (oper) {
				case ADC: { output.op_code = ADC_A_H; } break;
				case ADD: { output.op_code = ADD_A_H; } break;
				case AND: { output.op_code = AND_A_H; } break;
				case CP : { output.op_code =  CP_A_H; } break;
				case OR : { output.op_code =  OR_A_H; } break;
				case SBC: { output.op_code = SBC_A_H; } break;
				case SUB: { output.op_code = SUB_A_H; } break;
				case TST: { output.op_code = TST_A_H; } break;
				case XOR: { output.op_code = XOR_A_H; } break;
				default: break;
			}
		} break;
		case L: {
			switch (oper) {
				case ADC: { output.op_code = ADC_A_L; } break;
				case ADD: { output.op_code = ADD_A_L; } break;
				case AND: { output.op_code = AND_A_L; } break;
				case CP : { output.op_code =  CP_A_L; } break;
				case OR : { output.op_code =  OR_A_L; } break;
				case SBC: { output.op_code = SBC_A_L; } break;
				case SUB: { output.op_code = SUB_A_L; } break;
				case TST: { output.op_code = TST_A_L; } break;
				case XOR: { output.op_code = XOR_A_L; } break;
				default: break;
			}
		} break;
		case IXH: {
			switch (oper) {
				case ADC: { output.op_code = ADC_A_IXH; } break;
				case ADD: { output.op_code = ADD_A_IXH; } break;
				case AND: { output.op_code = AND_A_IXH; } break;
				case CP : { output.op_code =  CP_A_IXH; } break;
				case OR : { output.op_code =  OR_A_IXH; } break;
				case SBC: { output.op_code = SBC_A_IXH; } break;
				case SUB: { output.op_code = SUB_A_IXH; } break;
				case XOR: { output.op_code = XOR_A_IXH; } break;
				default: break;
			}
		} break;
		case IXL: {
			switch (oper) {
				case ADC: { output.op_code = ADC_A_IXL; } break;
				case ADD: { output.op_code = ADD_A_IXL; } break;
				case AND: { output.op_code = AND_A_IXL; } break;
				case CP : { output.op_code =  CP_A_IXL; } break;
				case OR : { output.op_code =  OR_A_IXL; } break;
				case SBC: { output.op_code = SBC_A_IXL; } break;
				case SUB: { output.op_code = SUB_A_IXL; } break;
				case XOR: { output.op_code = XOR_A_IXL; } break;
				default: break;
			}
		} break;
		case IYH: {
			switch (oper) {
				case ADC: { output.op_code = ADC_A_IYH; } break;
				case ADD: { output.op_code = ADD_A_IYH; } break;
				case AND: { output.op_code = AND_A_IYH; } break;
				case CP : { output.op_code =  CP_A_IYH; } break;
				case OR : { output.op_code =  OR_A_IYH; } break;
				case SBC: { output.op_code = SBC_A_IYH; } break;
				case SUB: { output.op_code = SUB_A_IYH; } break;
				case XOR: { output.op_code = XOR_A_IYH; } break;
				default: break;
			}
		} break;
		case IYL: {
			switch (oper) {
				case ADC: { output.op_code = ADC_A_IYL; } break;
				case ADD: { output.op_code = ADD_A_IYL; } break;
				case AND: { output.op_code = AND_A_IYL; } break;
				case CP : { output.op_code =  CP_A_IYL; } break;
				case OR : { output.op_code =  OR_A_IYL; } break;
				case SBC: { output.op_code = SBC_A_IYL; } break;
				case SUB: { output.op_code = SUB_A_IYL; } break;
				case XOR: { output.op_code = XOR_A_IYL; } break;
				default: break;
			}
		} break;
		case PHL: {
			switch (oper) {
				case ADC: { output.op_code = ADC_A_PHL; } break;
				case ADD: { output.op_code = ADD_A_PHL; } break;
				case AND: { output.op_code = AND_A_PHL; } break;
				case CP : { output.op_code =  CP_A_PHL; } break;
				case OR : { output.op_code =  OR_A_PHL; } break;
				case SBC: { output.op_code = SBC_A_PHL; } break;
				case SUB: { output.op_code = SUB_A_PHL; } break;
				case TST: { output.op_code = TST_A_PHL; } break;
				case XOR: { output.op_code = XOR_A_PHL; } break;
				default: break;
			}
		} break;
		case PIX: {
			set_index_register_offset(output, str, 3);
			switch (oper) {
				case ADC: { output.op_code = ADC_A_PIX; } break;
				case ADD: { output.op_code = ADD_A_PIX; } break;
				case AND: { output.op_code = AND_A_PIX; } break;
				case CP : { output.op_code =  CP_A_PIX; } break;
				case OR : { output.op_code =  OR_A_PIX; } break;
				case SBC: { output.op_code = SBC_A_PIX; } break;
				case SUB: { output.op_code = SUB_A_PIX; } break;
				case XOR: { output.op_code = XOR_A_PIX; } break;
				default: break;
			}
		} break;
		case PIY: {
			set_index_register_offset(output, str, 3);
			switch (oper) {
				case ADC: { output.op_code = ADC_A_PIY; } break;
				case ADD: { output.op_code = ADD_A_PIY; } break;
				case AND: { output.op_code = AND_A_PIY; } break;
				case CP : { output.op_code =  CP_A_PIY; } break;
				case OR : { output.op_code =  OR_A_PIY; } break;
				case SBC: { output.op_code = SBC_A_PIY; } break;
				case SUB: { output.op_code = SUB_A_PIY; } break;
				case XOR: { output.op_code = XOR_A_PIY; } break;
				default: break;
			}
		} break;
		default: break;
	}
}

void handle_acc(
	ez80_instruction& output,
	const vector<string>& str,
	instruction_enum_start oper
) {
	if (str.size() < 3) {
		return;
	}
	size_t index_dst, index_src;
	bool found;
	found = find_in_list(index_dst, str[1], argument_name, ARRAY_LEN(argument_name));
	if (found == false) {
		return;
	}
	using enum ez80_argument_name;
	using enum instruction_enum_start;
	if ((ez80_argument_name)index_dst == A) {
		handle_acc8(output, str, oper);
		return;
	}
	found = find_in_list(index_src, str[2], argument_name, ARRAY_LEN(argument_name));
	if (found == false) {
		return;
	}
	ez80_argument_name dst = (ez80_argument_name)index_dst;
	ez80_argument_name src = (ez80_argument_name)index_src;
	// only used for IX/IY
	bool acc16 = (oper == ADD_SIS);
	switch (dst) {
		using enum ez80_op_code;
		case HL: {
			switch (src) {
				case HL: {
					switch (oper) {
						case ADD: { output.op_code = ADD_HL_HL; } break;
						case ADC: { output.op_code = ADC_HL_HL; } break;
						case SBC: { output.op_code = SBC_HL_HL; } break;
						case ADD_SIS: { output.op_code = ADD_HL_HL_SIS; } break;
						case ADC_SIS: { output.op_code = ADC_HL_HL_SIS; } break;
						case SBC_SIS: { output.op_code = SBC_HL_HL_SIS; } break;
						default: break;
					}
				} break;
				case DE: {
					switch (oper) {
						case ADD: { output.op_code = ADD_HL_DE; } break;
						case ADC: { output.op_code = ADC_HL_DE; } break;
						case SBC: { output.op_code = SBC_HL_DE; } break;
						case ADD_SIS: { output.op_code = ADD_HL_DE_SIS; } break;
						case ADC_SIS: { output.op_code = ADC_HL_DE_SIS; } break;
						case SBC_SIS: { output.op_code = SBC_HL_DE_SIS; } break;
						default: break;
					}
				} break;
				case BC: {
					switch (oper) {
						case ADD: { output.op_code = ADD_HL_BC; } break;
						case ADC: { output.op_code = ADC_HL_BC; } break;
						case SBC: { output.op_code = SBC_HL_BC; } break;
						case ADD_SIS: { output.op_code = ADD_HL_BC_SIS; } break;
						case ADC_SIS: { output.op_code = ADC_HL_BC_SIS; } break;
						case SBC_SIS: { output.op_code = SBC_HL_BC_SIS; } break;
						default: break;
					}
				} break;
				case SP: {
					switch (oper) {
						case ADD: { output.op_code = ADD_HL_SP; } break;
						case ADC: { output.op_code = ADC_HL_SP; } break;
						case SBC: { output.op_code = SBC_HL_SP; } break;
						case ADD_SIS: { output.op_code = ADD_HL_SP_SIS; } break;
						case ADC_SIS: { output.op_code = ADC_HL_SP_SIS; } break;
						case SBC_SIS: { output.op_code = SBC_HL_SP_SIS; } break;
						default: break;
					}
				} break;
				default: break;
			}
		} break;
		case IX: {
			switch (src) {
				case IX: { output.op_code = acc16 ? ADD_IX_IX_SIS : ADD_IX_IX; } break;
				case DE: { output.op_code = acc16 ? ADD_IX_DE_SIS : ADD_IX_DE; } break;
				case BC: { output.op_code = acc16 ? ADD_IX_BC_SIS : ADD_IX_BC; } break;
				case SP: { output.op_code = acc16 ? ADD_IX_SP_SIS : ADD_IX_SP; } break;
				default: break;
			}
		} break;
		case IY: {
			switch (src) {
				case IY: { output.op_code = acc16 ? ADD_IY_IY_SIS : ADD_IY_IY; } break;
				case DE: { output.op_code = acc16 ? ADD_IY_DE_SIS : ADD_IY_DE; } break;
				case BC: { output.op_code = acc16 ? ADD_IY_BC_SIS : ADD_IY_BC; } break;
				case SP: { output.op_code = acc16 ? ADD_IY_SP_SIS : ADD_IY_SP; } break;
				default: break;
			}
		} break;
		default: break;
	}
}

void handle_bit_manipulation(
	ez80_instruction& output,
	const vector<string>& str,
	instruction_enum_start oper
) {
	if (str.size() < 3) {
		return;
	}
	size_t index;
	bool found = find_in_list(index, str[2], argument_name, ARRAY_LEN(argument_name));
	using enum ez80_argument_name;
	using enum instruction_enum_start;
	if (found == false) {
		return;
	}
	int bit = strtol(str[1].c_str(), nullptr, 10);
	ez80_argument_name arg = (ez80_argument_name)index;
	if (arg == PIX || arg == PIY) {
		if (str.size() == 3) {
			output.offset = 0;
			output.known_value = true;
		} else {
			set_index_register_offset(output, str, 4);
		}
	}
	switch (oper) {
		using enum ez80_op_code;
		case SET: {
			switch (arg) {
				case A : {
					switch (bit) {
						case 0: { output.op_code = SET_0_A; } break;
						case 1: { output.op_code = SET_1_A; } break;
						case 2: { output.op_code = SET_2_A; } break;
						case 3: { output.op_code = SET_3_A; } break;
						case 4: { output.op_code = SET_4_A; } break;
						case 5: { output.op_code = SET_5_A; } break;
						case 6: { output.op_code = SET_6_A; } break;
						case 7: { output.op_code = SET_7_A; } break;
						default: break;
					}
				} break;
				case B : {
					switch (bit) {
						case 0: { output.op_code = SET_0_B; } break;
						case 1: { output.op_code = SET_1_B; } break;
						case 2: { output.op_code = SET_2_B; } break;
						case 3: { output.op_code = SET_3_B; } break;
						case 4: { output.op_code = SET_4_B; } break;
						case 5: { output.op_code = SET_5_B; } break;
						case 6: { output.op_code = SET_6_B; } break;
						case 7: { output.op_code = SET_7_B; } break;
						default: break;
					}
				} break;
				case C : {
					switch (bit) {
						case 0: { output.op_code = SET_0_C; } break;
						case 1: { output.op_code = SET_1_C; } break;
						case 2: { output.op_code = SET_2_C; } break;
						case 3: { output.op_code = SET_3_C; } break;
						case 4: { output.op_code = SET_4_C; } break;
						case 5: { output.op_code = SET_5_C; } break;
						case 6: { output.op_code = SET_6_C; } break;
						case 7: { output.op_code = SET_7_C; } break;
						default: break;
					}
				} break;
				case D : {
					switch (bit) {
						case 0: { output.op_code = SET_0_D; } break;
						case 1: { output.op_code = SET_1_D; } break;
						case 2: { output.op_code = SET_2_D; } break;
						case 3: { output.op_code = SET_3_D; } break;
						case 4: { output.op_code = SET_4_D; } break;
						case 5: { output.op_code = SET_5_D; } break;
						case 6: { output.op_code = SET_6_D; } break;
						case 7: { output.op_code = SET_7_D; } break;
						default: break;
					}
				} break;
				case E : {
					switch (bit) {
						case 0: { output.op_code = SET_0_E; } break;
						case 1: { output.op_code = SET_1_E; } break;
						case 2: { output.op_code = SET_2_E; } break;
						case 3: { output.op_code = SET_3_E; } break;
						case 4: { output.op_code = SET_4_E; } break;
						case 5: { output.op_code = SET_5_E; } break;
						case 6: { output.op_code = SET_6_E; } break;
						case 7: { output.op_code = SET_7_E; } break;
						default: break;
					}
				} break;
				case H : {
					switch (bit) {
						case 0: { output.op_code = SET_0_H; } break;
						case 1: { output.op_code = SET_1_H; } break;
						case 2: { output.op_code = SET_2_H; } break;
						case 3: { output.op_code = SET_3_H; } break;
						case 4: { output.op_code = SET_4_H; } break;
						case 5: { output.op_code = SET_5_H; } break;
						case 6: { output.op_code = SET_6_H; } break;
						case 7: { output.op_code = SET_7_H; } break;
						default: break;
					}
				} break;
				case L : {
					switch (bit) {
						case 0: { output.op_code = SET_0_L; } break;
						case 1: { output.op_code = SET_1_L; } break;
						case 2: { output.op_code = SET_2_L; } break;
						case 3: { output.op_code = SET_3_L; } break;
						case 4: { output.op_code = SET_4_L; } break;
						case 5: { output.op_code = SET_5_L; } break;
						case 6: { output.op_code = SET_6_L; } break;
						case 7: { output.op_code = SET_7_L; } break;
						default: break;
					}
				} break;
				case PHL : {
					switch (bit) {
						case 0: { output.op_code = SET_0_PHL; } break;
						case 1: { output.op_code = SET_1_PHL; } break;
						case 2: { output.op_code = SET_2_PHL; } break;
						case 3: { output.op_code = SET_3_PHL; } break;
						case 4: { output.op_code = SET_4_PHL; } break;
						case 5: { output.op_code = SET_5_PHL; } break;
						case 6: { output.op_code = SET_6_PHL; } break;
						case 7: { output.op_code = SET_7_PHL; } break;
						default: break;
					}
				} break;
				case PIX : {
					switch (bit) {
						case 0: { output.op_code = SET_0_PIX; } break;
						case 1: { output.op_code = SET_1_PIX; } break;
						case 2: { output.op_code = SET_2_PIX; } break;
						case 3: { output.op_code = SET_3_PIX; } break;
						case 4: { output.op_code = SET_4_PIX; } break;
						case 5: { output.op_code = SET_5_PIX; } break;
						case 6: { output.op_code = SET_6_PIX; } break;
						case 7: { output.op_code = SET_7_PIX; } break;
						default: break;
					}
				} break;
				case PIY : {
					switch (bit) {
						case 0: { output.op_code = SET_0_PIY; } break;
						case 1: { output.op_code = SET_1_PIY; } break;
						case 2: { output.op_code = SET_2_PIY; } break;
						case 3: { output.op_code = SET_3_PIY; } break;
						case 4: { output.op_code = SET_4_PIY; } break;
						case 5: { output.op_code = SET_5_PIY; } break;
						case 6: { output.op_code = SET_6_PIY; } break;
						case 7: { output.op_code = SET_7_PIY; } break;
						default: break;
					}
				} break;
				default: break;
			}
		} break;
		case RES: {
			switch (arg) {
				case A : {
					switch (bit) {
						case 0: { output.op_code = RES_0_A; } break;
						case 1: { output.op_code = RES_1_A; } break;
						case 2: { output.op_code = RES_2_A; } break;
						case 3: { output.op_code = RES_3_A; } break;
						case 4: { output.op_code = RES_4_A; } break;
						case 5: { output.op_code = RES_5_A; } break;
						case 6: { output.op_code = RES_6_A; } break;
						case 7: { output.op_code = RES_7_A; } break;
						default: break;
					}
				} break;
				case B : {
					switch (bit) {
						case 0: { output.op_code = RES_0_B; } break;
						case 1: { output.op_code = RES_1_B; } break;
						case 2: { output.op_code = RES_2_B; } break;
						case 3: { output.op_code = RES_3_B; } break;
						case 4: { output.op_code = RES_4_B; } break;
						case 5: { output.op_code = RES_5_B; } break;
						case 6: { output.op_code = RES_6_B; } break;
						case 7: { output.op_code = RES_7_B; } break;
						default: break;
					}
				} break;
				case C : {
					switch (bit) {
						case 0: { output.op_code = RES_0_C; } break;
						case 1: { output.op_code = RES_1_C; } break;
						case 2: { output.op_code = RES_2_C; } break;
						case 3: { output.op_code = RES_3_C; } break;
						case 4: { output.op_code = RES_4_C; } break;
						case 5: { output.op_code = RES_5_C; } break;
						case 6: { output.op_code = RES_6_C; } break;
						case 7: { output.op_code = RES_7_C; } break;
						default: break;
					}
				} break;
				case D : {
					switch (bit) {
						case 0: { output.op_code = RES_0_D; } break;
						case 1: { output.op_code = RES_1_D; } break;
						case 2: { output.op_code = RES_2_D; } break;
						case 3: { output.op_code = RES_3_D; } break;
						case 4: { output.op_code = RES_4_D; } break;
						case 5: { output.op_code = RES_5_D; } break;
						case 6: { output.op_code = RES_6_D; } break;
						case 7: { output.op_code = RES_7_D; } break;
						default: break;
					}
				} break;
				case E : {
					switch (bit) {
						case 0: { output.op_code = RES_0_E; } break;
						case 1: { output.op_code = RES_1_E; } break;
						case 2: { output.op_code = RES_2_E; } break;
						case 3: { output.op_code = RES_3_E; } break;
						case 4: { output.op_code = RES_4_E; } break;
						case 5: { output.op_code = RES_5_E; } break;
						case 6: { output.op_code = RES_6_E; } break;
						case 7: { output.op_code = RES_7_E; } break;
						default: break;
					}
				} break;
				case H : {
					switch (bit) {
						case 0: { output.op_code = RES_0_H; } break;
						case 1: { output.op_code = RES_1_H; } break;
						case 2: { output.op_code = RES_2_H; } break;
						case 3: { output.op_code = RES_3_H; } break;
						case 4: { output.op_code = RES_4_H; } break;
						case 5: { output.op_code = RES_5_H; } break;
						case 6: { output.op_code = RES_6_H; } break;
						case 7: { output.op_code = RES_7_H; } break;
						default: break;
					}
				} break;
				case L : {
					switch (bit) {
						case 0: { output.op_code = RES_0_L; } break;
						case 1: { output.op_code = RES_1_L; } break;
						case 2: { output.op_code = RES_2_L; } break;
						case 3: { output.op_code = RES_3_L; } break;
						case 4: { output.op_code = RES_4_L; } break;
						case 5: { output.op_code = RES_5_L; } break;
						case 6: { output.op_code = RES_6_L; } break;
						case 7: { output.op_code = RES_7_L; } break;
						default: break;
					}
				} break;
				case PHL : {
					switch (bit) {
						case 0: { output.op_code = RES_0_PHL; } break;
						case 1: { output.op_code = RES_1_PHL; } break;
						case 2: { output.op_code = RES_2_PHL; } break;
						case 3: { output.op_code = RES_3_PHL; } break;
						case 4: { output.op_code = RES_4_PHL; } break;
						case 5: { output.op_code = RES_5_PHL; } break;
						case 6: { output.op_code = RES_6_PHL; } break;
						case 7: { output.op_code = RES_7_PHL; } break;
						default: break;
					}
				} break;
				case PIX : {
					switch (bit) {
						case 0: { output.op_code = RES_0_PIX; } break;
						case 1: { output.op_code = RES_1_PIX; } break;
						case 2: { output.op_code = RES_2_PIX; } break;
						case 3: { output.op_code = RES_3_PIX; } break;
						case 4: { output.op_code = RES_4_PIX; } break;
						case 5: { output.op_code = RES_5_PIX; } break;
						case 6: { output.op_code = RES_6_PIX; } break;
						case 7: { output.op_code = RES_7_PIX; } break;
						default: break;
					}
				} break;
				case PIY : {
					switch (bit) {
						case 0: { output.op_code = RES_0_PIY; } break;
						case 1: { output.op_code = RES_1_PIY; } break;
						case 2: { output.op_code = RES_2_PIY; } break;
						case 3: { output.op_code = RES_3_PIY; } break;
						case 4: { output.op_code = RES_4_PIY; } break;
						case 5: { output.op_code = RES_5_PIY; } break;
						case 6: { output.op_code = RES_6_PIY; } break;
						case 7: { output.op_code = RES_7_PIY; } break;
						default: break;
					}
				} break;
				default: break;
			}
		} break;
		case BIT: {
			switch (arg) {
				case A : {
					switch (bit) {
						case 0: { output.op_code = BIT_0_A; } break;
						case 1: { output.op_code = BIT_1_A; } break;
						case 2: { output.op_code = BIT_2_A; } break;
						case 3: { output.op_code = BIT_3_A; } break;
						case 4: { output.op_code = BIT_4_A; } break;
						case 5: { output.op_code = BIT_5_A; } break;
						case 6: { output.op_code = BIT_6_A; } break;
						case 7: { output.op_code = BIT_7_A; } break;
						default: break;
					}
				} break;
				case B : {
					switch (bit) {
						case 0: { output.op_code = BIT_0_B; } break;
						case 1: { output.op_code = BIT_1_B; } break;
						case 2: { output.op_code = BIT_2_B; } break;
						case 3: { output.op_code = BIT_3_B; } break;
						case 4: { output.op_code = BIT_4_B; } break;
						case 5: { output.op_code = BIT_5_B; } break;
						case 6: { output.op_code = BIT_6_B; } break;
						case 7: { output.op_code = BIT_7_B; } break;
						default: break;
					}
				} break;
				case C : {
					switch (bit) {
						case 0: { output.op_code = BIT_0_C; } break;
						case 1: { output.op_code = BIT_1_C; } break;
						case 2: { output.op_code = BIT_2_C; } break;
						case 3: { output.op_code = BIT_3_C; } break;
						case 4: { output.op_code = BIT_4_C; } break;
						case 5: { output.op_code = BIT_5_C; } break;
						case 6: { output.op_code = BIT_6_C; } break;
						case 7: { output.op_code = BIT_7_C; } break;
						default: break;
					}
				} break;
				case D : {
					switch (bit) {
						case 0: { output.op_code = BIT_0_D; } break;
						case 1: { output.op_code = BIT_1_D; } break;
						case 2: { output.op_code = BIT_2_D; } break;
						case 3: { output.op_code = BIT_3_D; } break;
						case 4: { output.op_code = BIT_4_D; } break;
						case 5: { output.op_code = BIT_5_D; } break;
						case 6: { output.op_code = BIT_6_D; } break;
						case 7: { output.op_code = BIT_7_D; } break;
						default: break;
					}
				} break;
				case E : {
					switch (bit) {
						case 0: { output.op_code = BIT_0_E; } break;
						case 1: { output.op_code = BIT_1_E; } break;
						case 2: { output.op_code = BIT_2_E; } break;
						case 3: { output.op_code = BIT_3_E; } break;
						case 4: { output.op_code = BIT_4_E; } break;
						case 5: { output.op_code = BIT_5_E; } break;
						case 6: { output.op_code = BIT_6_E; } break;
						case 7: { output.op_code = BIT_7_E; } break;
						default: break;
					}
				} break;
				case H : {
					switch (bit) {
						case 0: { output.op_code = BIT_0_H; } break;
						case 1: { output.op_code = BIT_1_H; } break;
						case 2: { output.op_code = BIT_2_H; } break;
						case 3: { output.op_code = BIT_3_H; } break;
						case 4: { output.op_code = BIT_4_H; } break;
						case 5: { output.op_code = BIT_5_H; } break;
						case 6: { output.op_code = BIT_6_H; } break;
						case 7: { output.op_code = BIT_7_H; } break;
						default: break;
					}
				} break;
				case L : {
					switch (bit) {
						case 0: { output.op_code = BIT_0_L; } break;
						case 1: { output.op_code = BIT_1_L; } break;
						case 2: { output.op_code = BIT_2_L; } break;
						case 3: { output.op_code = BIT_3_L; } break;
						case 4: { output.op_code = BIT_4_L; } break;
						case 5: { output.op_code = BIT_5_L; } break;
						case 6: { output.op_code = BIT_6_L; } break;
						case 7: { output.op_code = BIT_7_L; } break;
						default: break;
					}
				} break;
				case PHL : {
					switch (bit) {
						case 0: { output.op_code = BIT_0_PHL; } break;
						case 1: { output.op_code = BIT_1_PHL; } break;
						case 2: { output.op_code = BIT_2_PHL; } break;
						case 3: { output.op_code = BIT_3_PHL; } break;
						case 4: { output.op_code = BIT_4_PHL; } break;
						case 5: { output.op_code = BIT_5_PHL; } break;
						case 6: { output.op_code = BIT_6_PHL; } break;
						case 7: { output.op_code = BIT_7_PHL; } break;
						default: break;
					}
				} break;
				case PIX : {
					switch (bit) {
						case 0: { output.op_code = BIT_0_PIX; } break;
						case 1: { output.op_code = BIT_1_PIX; } break;
						case 2: { output.op_code = BIT_2_PIX; } break;
						case 3: { output.op_code = BIT_3_PIX; } break;
						case 4: { output.op_code = BIT_4_PIX; } break;
						case 5: { output.op_code = BIT_5_PIX; } break;
						case 6: { output.op_code = BIT_6_PIX; } break;
						case 7: { output.op_code = BIT_7_PIX; } break;
						default: break;
					}
				} break;
				case PIY : {
					switch (bit) {
						case 0: { output.op_code = BIT_0_PIY; } break;
						case 1: { output.op_code = BIT_1_PIY; } break;
						case 2: { output.op_code = BIT_2_PIY; } break;
						case 3: { output.op_code = BIT_3_PIY; } break;
						case 4: { output.op_code = BIT_4_PIY; } break;
						case 5: { output.op_code = BIT_5_PIY; } break;
						case 6: { output.op_code = BIT_6_PIY; } break;
						case 7: { output.op_code = BIT_7_PIY; } break;
						default: break;
					}
				} break;
				default: break;
			}
		} break;
		default: break;
	}
}

void handle_lea(
	ez80_instruction& output,
	const vector<string>& str,
	__attribute__((unused)) instruction_enum_start oper
) {
	if (str.size() < 3) {
		return;
	}
	using enum ez80_argument_name;
	using enum instruction_enum_start;
	size_t index_dst, index_src;
	bool found;
	found = find_in_list(index_dst, str[1], argument_name, ARRAY_LEN(argument_name));
	if (found == false) {
		return;
	}
	found = find_in_list(index_src, str[2], argument_name, ARRAY_LEN(argument_name));
	if (found == false) {
		return;
	}
	ez80_argument_name dst = (ez80_argument_name)index_dst;
	bool use_ix = ((ez80_argument_name)index_src == IX);
	bool arg16 = (oper == LEA_SIS);
	set_index_register_offset(output, str, 3);
	switch (dst) {
		using enum ez80_op_code;
		case HL: { output.op_code = use_ix ? (arg16 ? LEA_HL_IX_SIS : LEA_HL_IX) : (arg16 ? LEA_HL_IY_SIS : LEA_HL_IY); } break;
		case DE: { output.op_code = use_ix ? (arg16 ? LEA_DE_IX_SIS : LEA_DE_IX) : (arg16 ? LEA_DE_IY_SIS : LEA_DE_IY); } break;
		case BC: { output.op_code = use_ix ? (arg16 ? LEA_BC_IX_SIS : LEA_BC_IX) : (arg16 ? LEA_BC_IY_SIS : LEA_BC_IY); } break;
		case IX: { output.op_code = use_ix ? (arg16 ? LEA_IX_IX_SIS : LEA_IX_IX) : (arg16 ? LEA_IX_IY_SIS : LEA_IX_IY); } break;
		case IY: { output.op_code = use_ix ? (arg16 ? LEA_IY_IX_SIS : LEA_IY_IX) : (arg16 ? LEA_IY_IY_SIS : LEA_IY_IY); } break;
		default: break;
	}
}

void handle_pea(
	ez80_instruction& output,
	const vector<string>& str,
	__attribute__((unused)) instruction_enum_start oper
) {
	if (str.size() < 2) {
		return;
	}
	using enum ez80_argument_name;
	using enum instruction_enum_start;
	size_t index;
	bool found;
	found = find_in_list(index, str[1], argument_name, ARRAY_LEN(argument_name));
	if (found == false) {
		return;
	}
	output.op_code =
		((ez80_argument_name)index == IX) ?
		ez80_op_code::PEA_IX : ez80_op_code::PEA_IY;
	if (str.size() == 2) {
		output.offset = 0;
		output.known_value = true;
		return;
	}
	set_index_register_offset(output, str, 2);
}

void handle_load(
	ez80_instruction& output,
	const vector<string>& str,
	__attribute__((unused)) instruction_enum_start oper
) {
	if (str.size() < 3) {
		return;
	}
	using enum ez80_argument_name;
	using enum instruction_enum_start;
	bool arg16 = (oper == LD_SIS);
	size_t index_dst, index_src, index_alt = 0;
	bool found_dst = find_in_list(index_dst, str[1], argument_name, ARRAY_LEN(argument_name));
	bool found_src = find_in_list(index_src, str[2], argument_name, ARRAY_LEN(argument_name));
	__attribute__((unused)) bool found_alt = false;
	if (str.size() >= 4) {
		found_alt = find_in_list(index_alt, str[3], argument_name, ARRAY_LEN(argument_name));
	}
	ez80_argument_name src = (ez80_argument_name)index_src;
	ez80_argument_name dst = (ez80_argument_name)index_dst;
	ez80_argument_name alt = (ez80_argument_name)index_alt;
	if (found_dst == false && found_src == true) {
		using enum ez80_op_code;
		set_imm_value(output, str, 1);
		switch (src) {
			case A  : { output.op_code = LD_ADDR_A ; } break;
			case BC : { output.op_code = LD_ADDR_BC; } break;
			case DE : { output.op_code = LD_ADDR_DE; } break;
			case HL : { output.op_code = LD_ADDR_HL; } break;
			case SP : { output.op_code = LD_ADDR_SP; } break;
			case IX : { output.op_code = LD_ADDR_IX; } break;
			case IY : { output.op_code = LD_ADDR_IY; } break;
			default: break;
		}
		return;
	}
	if (found_dst == true && found_src == false) {
		using enum ez80_op_code;
		if (dst == PIX || dst == PIY) {
			set_index_register_offset(output, str, 2);
			bool use_ix = (dst == PIX);
			switch (alt) {
				case A  : { output.op_code = use_ix ? LD_PIX_A  : LD_PIY_A ; } break;
				case B  : { output.op_code = use_ix ? LD_PIX_B  : LD_PIY_B ; } break;
				case C  : { output.op_code = use_ix ? LD_PIX_C  : LD_PIY_C ; } break;
				case D  : { output.op_code = use_ix ? LD_PIX_D  : LD_PIY_D ; } break;
				case E  : { output.op_code = use_ix ? LD_PIX_E  : LD_PIY_E ; } break;
				case H  : { output.op_code = use_ix ? LD_PIX_H  : LD_PIY_H ; } break;
				case L  : { output.op_code = use_ix ? LD_PIX_L  : LD_PIY_L ; } break;
				case BC : { output.op_code = use_ix ? LD_PIX_BC : LD_PIY_BC; } break;
				case DE : { output.op_code = use_ix ? LD_PIX_DE : LD_PIY_DE; } break;
				case HL : { output.op_code = use_ix ? LD_PIX_HL : LD_PIY_HL; } break;
				case IX : { output.op_code = use_ix ? LD_PIX_IX : LD_PIY_IX; } break;
				case IY : { output.op_code = use_ix ? LD_PIX_IY : LD_PIY_IY; } break;
				default: {
					set_imm_value(output, str, 3);
					output.op_code = use_ix ? LD_PIX_N : LD_PIY_N;
				} break;
			}
			return;
		}
		set_imm_value(output, str, 2);
		bool is_address = (str[2].front() == '(');
		switch (dst) {
			case A   : { output.op_code = is_address ?  LD_A_ADDR : LD_A_N; } break;
			case BC  : { output.op_code = is_address ? LD_BC_ADDR : (arg16 ? LD_BC_N_SIS : LD_BC_N); } break;
			case DE  : { output.op_code = is_address ? LD_DE_ADDR : (arg16 ? LD_DE_N_SIS : LD_DE_N); } break;
			case HL  : { output.op_code = is_address ? LD_HL_ADDR : (arg16 ? LD_HL_N_SIS : LD_HL_N); } break;
			case SP  : { output.op_code = is_address ? LD_SP_ADDR : (arg16 ? LD_SP_N_SIS : LD_SP_N); } break;
			case IX  : { output.op_code = is_address ? LD_IX_ADDR : (arg16 ? LD_IX_N_SIS : LD_IX_N); } break;
			case IY  : { output.op_code = is_address ? LD_IY_ADDR : (arg16 ? LD_IY_N_SIS : LD_IY_N); } break;
			case B   : { output.op_code = LD_B_N  ; } break;
			case C   : { output.op_code = LD_C_N  ; } break;
			case D   : { output.op_code = LD_D_N  ; } break;
			case E   : { output.op_code = LD_E_N  ; } break;
			case H   : { output.op_code = LD_H_N  ; } break;
			case L   : { output.op_code = LD_L_N  ; } break;
			case IXH : { output.op_code = LD_IXH_N; } break;
			case IXL : { output.op_code = LD_IXL_N; } break;
			case IYH : { output.op_code = LD_IXH_N; } break;
			case IYL : { output.op_code = LD_IXL_N; } break;
			case PHL : { output.op_code = LD_PHL_N; } break;
			case PIX : { output.op_code = LD_PIX_N; } break;
			case PIY : { output.op_code = LD_PIY_N; } break;
			default: break;
		}
		return;
	}
	if (dst == PIX || dst == PIY) {
		if (str.size() == 3) {
			output.offset = 0;
			output.known_value = true;
		}
	}
	switch (src) {
		using enum ez80_op_code;
		case A: {
			switch (dst) {
				case A   : { output.op_code =   LD_A_A; } break;
				case B   : { output.op_code =   LD_B_A; } break;
				case C   : { output.op_code =   LD_C_A; } break;
				case D   : { output.op_code =   LD_D_A; } break;
				case E   : { output.op_code =   LD_E_A; } break;
				case H   : { output.op_code =   LD_H_A; } break;
				case L   : { output.op_code =   LD_L_A; } break;
				case I   : { output.op_code =   LD_I_A; } break;
				case R   : { output.op_code =   LD_R_A; } break;
				case MB  : { output.op_code =  LD_MB_A; } break;
				case IXL : { output.op_code = LD_IXL_A; } break;
				case IXH : { output.op_code = LD_IXH_A; } break;
				case IYL : { output.op_code = LD_IYL_A; } break;
				case IYH : { output.op_code = LD_IYH_A; } break;
				case PBC : { output.op_code = LD_PBC_A; } break;
				case PDE : { output.op_code = LD_PDE_A; } break;
				case PHL : { output.op_code = LD_PHL_A; } break;
				case PIX : { output.op_code = LD_PIX_A; } break;
				case PIY : { output.op_code = LD_PIY_A; } break;
				default: break;
			}
		} break;
		case B: {
			switch (dst) {
				case A   : { output.op_code =   LD_A_B; } break;
				case C   : { output.op_code =   LD_C_B; } break;
				case D   : { output.op_code =   LD_D_B; } break;
				case E   : { output.op_code =   LD_E_B; } break;
				case H   : { output.op_code =   LD_H_B; } break;
				case L   : { output.op_code =   LD_L_B; } break;
				case IXL : { output.op_code = LD_IXL_B; } break;
				case IXH : { output.op_code = LD_IXH_B; } break;
				case IYL : { output.op_code = LD_IYL_B; } break;
				case IYH : { output.op_code = LD_IYH_B; } break;
				case PHL : { output.op_code = LD_PHL_B; } break;
				case PIX : { output.op_code = LD_PIX_B; } break;
				case PIY : { output.op_code = LD_PIY_B; } break;
				default: break;
			}
		} break;
		case C: {
			switch (dst) {
				case A   : { output.op_code =   LD_A_C; } break;
				case B   : { output.op_code =   LD_B_C; } break;
				case D   : { output.op_code =   LD_D_C; } break;
				case E   : { output.op_code =   LD_E_C; } break;
				case H   : { output.op_code =   LD_H_C; } break;
				case L   : { output.op_code =   LD_L_C; } break;
				case IXL : { output.op_code = LD_IXL_C; } break;
				case IXH : { output.op_code = LD_IXH_C; } break;
				case IYL : { output.op_code = LD_IYL_C; } break;
				case IYH : { output.op_code = LD_IYH_C; } break;
				case PHL : { output.op_code = LD_PHL_C; } break;
				case PIX : { output.op_code = LD_PIX_C; } break;
				case PIY : { output.op_code = LD_PIY_C; } break;
				default: break;
			}
		} break;
		case D: {
			switch (dst) {
				case A   : { output.op_code =   LD_A_D; } break;
				case B   : { output.op_code =   LD_B_D; } break;
				case C   : { output.op_code =   LD_C_D; } break;
				case E   : { output.op_code =   LD_E_D; } break;
				case H   : { output.op_code =   LD_H_D; } break;
				case L   : { output.op_code =   LD_L_D; } break;
				case IXL : { output.op_code = LD_IXL_D; } break;
				case IXH : { output.op_code = LD_IXH_D; } break;
				case IYL : { output.op_code = LD_IYL_D; } break;
				case IYH : { output.op_code = LD_IYH_D; } break;
				case PHL : { output.op_code = LD_PHL_D; } break;
				case PIX : { output.op_code = LD_PIX_D; } break;
				case PIY : { output.op_code = LD_PIY_D; } break;
				default: break;
			}
		} break;
		case E: {
			switch (dst) {
				case A   : { output.op_code =   LD_A_E; } break;
				case B   : { output.op_code =   LD_B_E; } break;
				case C   : { output.op_code =   LD_C_E; } break;
				case D   : { output.op_code =   LD_D_E; } break;
				case H   : { output.op_code =   LD_H_E; } break;
				case L   : { output.op_code =   LD_L_E; } break;
				case IXL : { output.op_code = LD_IXL_E; } break;
				case IXH : { output.op_code = LD_IXH_E; } break;
				case IYL : { output.op_code = LD_IYL_E; } break;
				case IYH : { output.op_code = LD_IYH_E; } break;
				case PHL : { output.op_code = LD_PHL_E; } break;
				case PIX : { output.op_code = LD_PIX_E; } break;
				case PIY : { output.op_code = LD_PIY_E; } break;
				default: break;
			}
		} break;
		case H: {
			switch (dst) {
				case A   : { output.op_code =   LD_A_H; } break;
				case B   : { output.op_code =   LD_B_H; } break;
				case C   : { output.op_code =   LD_C_H; } break;
				case D   : { output.op_code =   LD_D_H; } break;
				case E   : { output.op_code =   LD_E_H; } break;
				case H   : { output.op_code =   LD_H_H; } break;
				case L   : { output.op_code =   LD_L_H; } break;
				case PHL : { output.op_code = LD_PHL_H; } break;
				case PIX : { output.op_code = LD_PIX_H; } break;
				case PIY : { output.op_code = LD_PIY_H; } break;
				default: break;
			}
		} break;
		case L: {
			switch (dst) {
				case A   : { output.op_code =   LD_A_L; } break;
				case B   : { output.op_code =   LD_B_L; } break;
				case C   : { output.op_code =   LD_C_L; } break;
				case D   : { output.op_code =   LD_D_L; } break;
				case E   : { output.op_code =   LD_E_L; } break;
				case H   : { output.op_code =   LD_H_L; } break;
				case L   : { output.op_code =   LD_L_L; } break;
				case PHL : { output.op_code = LD_PHL_L; } break;
				case PIX : { output.op_code = LD_PIX_L; } break;
				case PIY : { output.op_code = LD_PIY_L; } break;
				default: break;
			}
		} break;
		case IXH: {
			switch (dst) {
				case A   : { output.op_code =   LD_A_IXH; } break;
				case B   : { output.op_code =   LD_B_IXH; } break;
				case C   : { output.op_code =   LD_C_IXH; } break;
				case D   : { output.op_code =   LD_D_IXH; } break;
				case E   : { output.op_code =   LD_E_IXH; } break;
				case IXL : { output.op_code = LD_IXL_IXH; } break;
				case IXH : { output.op_code = LD_IXH_IXH; } break;
				default: break;
			}
		} break;
		case IXL: {
			switch (dst) {
				case A   : { output.op_code =   LD_A_IXL; } break;
				case B   : { output.op_code =   LD_B_IXL; } break;
				case C   : { output.op_code =   LD_C_IXL; } break;
				case D   : { output.op_code =   LD_D_IXL; } break;
				case E   : { output.op_code =   LD_E_IXL; } break;
				case IXL : { output.op_code = LD_IXL_IXL; } break;
				case IXH : { output.op_code = LD_IXH_IXL; } break;
				default: break;
			}
		} break;
		case IYH: {
			switch (dst) {
				case A   : { output.op_code =   LD_A_IYH; } break;
				case B   : { output.op_code =   LD_B_IYH; } break;
				case C   : { output.op_code =   LD_C_IYH; } break;
				case D   : { output.op_code =   LD_D_IYH; } break;
				case E   : { output.op_code =   LD_E_IYH; } break;
				case IYL : { output.op_code = LD_IYL_IYH; } break;
				case IYH : { output.op_code = LD_IYH_IYH; } break;
				default: break;
			}
		} break;
		case IYL: {
			switch (dst) {
				case A   : { output.op_code =   LD_A_IYL; } break;
				case B   : { output.op_code =   LD_B_IYL; } break;
				case C   : { output.op_code =   LD_C_IYL; } break;
				case D   : { output.op_code =   LD_D_IYL; } break;
				case E   : { output.op_code =   LD_E_IYL; } break;
				case IYL : { output.op_code = LD_IYL_IYL; } break;
				case IYH : { output.op_code = LD_IYH_IYL; } break;
				default: break;
			}
		} break;
		case BC: {
			switch (dst) {
				case PHL : { output.op_code = LD_PHL_BC; } break;
				case PIX : { output.op_code = LD_PIX_BC; } break;
				case PIY : { output.op_code = LD_PIY_BC; } break;
				default: break;
			}
		} break;
		case DE: {
			switch (dst) {
				case PHL : { output.op_code = LD_PHL_DE; } break;
				case PIX : { output.op_code = LD_PIX_DE; } break;
				case PIY : { output.op_code = LD_PIY_DE; } break;
				default: break;
			}
		} break;
		case HL: {
			switch (dst) {
				case PHL : { output.op_code = LD_PHL_HL; } break;
				case PIX : { output.op_code = LD_PIX_HL; } break;
				case PIY : { output.op_code = LD_PIY_HL; } break;
				case SP  : { output.op_code =  LD_SP_HL; } break;
				case I   : { output.op_code =   LD_I_HL; } break;
				default: break;
			}
		} break;
		case IX: {
			switch (dst) {;
				case PHL : { output.op_code = LD_PHL_IX; } break;
				case PIX : { output.op_code = LD_PIX_IX; } break;
				case PIY : { output.op_code = LD_PIY_IX; } break;
				case SP  : { output.op_code =  LD_SP_IX; } break;
				default: break;
			}
		} break;
		case IY: {
			switch (dst) {;
				case PHL : { output.op_code = LD_PHL_IY; } break;
				case PIX : { output.op_code = LD_PIX_IY; } break;
				case PIY : { output.op_code = LD_PIY_IY; } break;
				case SP  : { output.op_code =  LD_SP_IY; } break;
				default: break;
			}
		} break;
		case PHL: {
			switch (dst) {
				case A  : { output.op_code =    LD_A_PHL; } break;
				case B  : { output.op_code =    LD_B_PHL; } break;
				case C  : { output.op_code =    LD_C_PHL; } break;
				case D  : { output.op_code =    LD_D_PHL; } break;
				case E  : { output.op_code =    LD_E_PHL; } break;
				case H  : { output.op_code =    LD_H_PHL; } break;
				case L  : { output.op_code =    LD_L_PHL; } break;
				case BC : { output.op_code =   LD_BC_PHL; } break;
				case DE : { output.op_code =   LD_DE_PHL; } break;
				case HL : { output.op_code =   LD_HL_PHL; } break;
				case IX : { output.op_code =   LD_IX_PHL; } break;
				case IY : { output.op_code =   LD_IY_PHL; } break;
				default: break;
			}
		} break;
		case PDE: { output.op_code = LD_PDE_A; } break;
		case PBC: { output.op_code = LD_PBC_A; } break;
		case PIX: {
			switch (dst) {
				case A  : { output.op_code =    LD_A_PIX; } break;
				case B  : { output.op_code =    LD_B_PIX; } break;
				case C  : { output.op_code =    LD_C_PIX; } break;
				case D  : { output.op_code =    LD_D_PIX; } break;
				case E  : { output.op_code =    LD_E_PIX; } break;
				case H  : { output.op_code =    LD_H_PIX; } break;
				case L  : { output.op_code =    LD_L_PIX; } break;
				case BC : { output.op_code =   LD_BC_PIX; } break;
				case DE : { output.op_code =   LD_DE_PIX; } break;
				case HL : { output.op_code =   LD_HL_PIX; } break;
				case IX : { output.op_code =   LD_IX_PIX; } break;
				case IY : { output.op_code =   LD_IY_PIX; } break;
				default: break;
			}
			if (str.size() == 3) {
				output.offset = 0;
				output.known_value = true;
				return;
			}
			set_index_register_offset(output, str, 3);
		} break;
		case PIY: {
			switch (dst) {
				case A  : { output.op_code =    LD_A_PIY; } break;
				case B  : { output.op_code =    LD_B_PIY; } break;
				case C  : { output.op_code =    LD_C_PIY; } break;
				case D  : { output.op_code =    LD_D_PIY; } break;
				case E  : { output.op_code =    LD_E_PIY; } break;
				case H  : { output.op_code =    LD_H_PIY; } break;
				case L  : { output.op_code =    LD_L_PIY; } break;
				case BC : { output.op_code =   LD_BC_PIY; } break;
				case DE : { output.op_code =   LD_DE_PIY; } break;
				case HL : { output.op_code =   LD_HL_PIY; } break;
				case IX : { output.op_code =   LD_IX_PIY; } break;
				case IY : { output.op_code =   LD_IY_PIY; } break;
				default: break;
			}
			if (str.size() == 3) {
				output.offset = 0;
				output.known_value = true;
				return;
			}
			set_index_register_offset(output, str, 3);
		} break;
		case I: {
			switch (dst) {
				case A  : { output.op_code =    LD_A_I; } break;
				case HL : { output.op_code =   LD_HL_I; } break;
				default: break;
			}
		} break;
		case R: {
			switch (dst) {
				case A : { output.op_code =    LD_A_R; } break;
				default: break;
			}
		} break;
		case MB: {
			switch (dst) {
				case A : { output.op_code =    LD_A_MB; } break;
				default: break;
			}
		} break;
		default: break;
	}
}

#endif /* PROCESS_ASM_H */
