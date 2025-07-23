#include "current_state.hpp"

void current_state::next_instruction(ez80_instruction instruction) {
	using enum ez80_op_code;
	using enum flag_state;
	ez80_op_code op_code = instruction.op_code;
	reg8_pair unknown8;
	unknown8.set_unknown();
	reg24_pair unknown24;
	unknown24.set_unknown();
	bool known_value = instruction.known_value;
	uint24_t imm_value = instruction.value;
	reg8_pair imm_offset;
	imm_offset.set_value(instruction.offset);
	reg8_pair imm8;
	reg24_pair imm16;
	reg24_pair imm24;
	imm8.set_value((uint8_t)imm_value);
	imm16.set_value((uint16_t)imm_value);
	imm24.set_value((uint24_t)imm_value);
	if (!known_value) {
		imm8.set_unknown();
		imm16.set_unknown();
		imm24.set_unknown();
	}
	imm16.clear_upper();
	switch (op_code) {

		/* special cases */

		case ADD_A_A: { add_a_a(); } break;
		case ADC_A_A: { adc_a_a(); } break;
		case SBC_A_A: { sbc_a_a(); } break;
		case SUB_A_A: { sub_a_a(); } break;
		case CP_A_A : {  cp_a_a(); } break;

		case XOR_A_A: { xor_a_a(); } break;
		case OR_A_A : {  or_a_a(); } break;
		case AND_A_A: { and_a_a(); } break;
		case TST_A_A: { tst_a_a(); } break;

		case ADD_HL_HL: { set_HL(add24_hl_hl(get_HL())); } break;
		case ADD_IX_IX: { set_IX(add24_hl_hl(get_IX())); } break;
		case ADD_IY_IY: { set_IY(add24_hl_hl(get_IY())); } break;
	
		case ADD_HL_HL_SIS: { set_HL(add16_hl_hl(get_HL())); } break;
		case ADD_IX_IX_SIS: { set_IX(add16_hl_hl(get_IX())); } break;
		case ADD_IY_IY_SIS: { set_IY(add16_hl_hl(get_IY())); } break;

		case ADC_HL_HL: { set_HL(adc24_hl_hl(get_HL())); } break;
		case ADC_HL_HL_SIS: { set_HL(adc16_hl_hl(get_HL())); } break;

		case SBC_HL_HL: { set_HL(sbc24_hl_hl(get_HL())); } break;
		case SBC_HL_HL_SIS: { set_HL(sbc16_hl_hl(get_HL())); } break;

		/* unique instructions */

		case SCF: { F.set_carry(true); } break;
		case CCF: { F.invert_carry(); } break;

		case CPL: { acc_cpl(); } break;
		case NEG: { acc_neg(); } break;

		case RLA: { acc_rla(); } break;
		case RRA: { acc_rra(); } break;
		case RLCA: { acc_rlca(); } break;
		case RRCA: { acc_rrca(); } break;

		/* general cases */

		case EX_AF_AF: {
			F.set_flags_unknown();
			A.set_unknown();
		} break;

		case EX_DE_HL: {
			std::swap(UHL, UDE);
			std::swap(H, D);
			std::swap(L, E);
		} break;
		case EX_DE_HL_SIS: {
			std::swap(H, D);
			std::swap(L, E);
			UHL.set_value(0);
			UDE.set_value(0);
		} break;
		case EXX: {
			UHL.set_unknown();
			H.set_unknown();
			L.set_unknown();
			UDE.set_unknown();
			D.set_unknown();
			E.set_unknown();
			UBC.set_unknown();
			B.set_unknown();
			C.set_unknown();
		} break;

		case POP_AF: {
			A.set_unknown();
			F.set_flags_unknown();
		} break;
		case POP_BC: {
			UBC.set_unknown();
			B.set_unknown();
			C.set_unknown();
		} break;
		case POP_DE: {
			UDE.set_unknown();
			D.set_unknown();
			E.set_unknown();
		} break;
		case POP_HL: {
			UHL.set_unknown();
			H.set_unknown();
			L.set_unknown();
		} break;
		case POP_IX: {
			UIX.set_unknown();
			IXH.set_unknown();
			IXL.set_unknown();
		} break;
		case POP_IY: {
			UIY.set_unknown();
			IYH.set_unknown();
			IYL.set_unknown();
		} break;
		case EX_SP_HL: {
			UHL.set_unknown();
			H.set_unknown();
			L.set_unknown();
		} break;
		case EX_SP_IX: {
			UIX.set_unknown();
			IXH.set_unknown();
			IXL.set_unknown();
		} break;
		case EX_SP_IY: {
			UIY.set_unknown();
			IYH.set_unknown();
			IYL.set_unknown();
		} break;
		case INC_BC: {
			reg24_pair reg;
			reg.set_value(UBC, B, C);
			++reg;
			reg.split_value(UBC, B, C);
		} break;
		case INC_DE: {
			reg24_pair reg;
			reg.set_value(UDE, D, E);
			++reg;
			reg.split_value(UDE, D, E);
		} break;
		case INC_HL: {
			reg24_pair reg;
			reg.set_value(UHL, H, L);
			++reg;
			reg.split_value(UHL, H, L);
		} break;
		case INC_SP: {
			SP_set_unknown();
		} break;
		case INC_IX: {
			reg24_pair reg;
			reg.set_value(UIX, IXH, IXL);
			++reg;
			reg.split_value(UIX, IXH, IXL);
		} break;
		case INC_IY: {
			reg24_pair reg;
			reg.set_value(UIY, IYH, IYL);
			++reg;
			reg.split_value(UIY, IYH, IYL);
		} break;
		case INC_BC_SIS: {
			reg24_pair reg;
			reg.set_value(UBC, B, C);
			++reg;
			reg.clear_upper();
			reg.split_value(UBC, B, C);
		} break;
		case INC_DE_SIS: {
			reg24_pair reg;
			reg.set_value(UDE, D, E);
			++reg;
			reg.clear_upper();
			reg.split_value(UDE, D, E);
		} break;
		case INC_HL_SIS: {
			reg24_pair reg;
			reg.set_value(UHL, H, L);
			++reg;
			reg.clear_upper();
			reg.split_value(UHL, H, L);
		} break;
		case INC_SP_SIS: {
			SP_set_unknown();
		} break;
		case INC_IX_SIS: {
			reg24_pair reg;
			reg.set_value(UIX, IXH, IXL);
			++reg;
			reg.clear_upper();
			reg.split_value(UIX, IXH, IXL);
		} break;
		case INC_IY_SIS: {
			reg24_pair reg;
			reg.set_value(UIY, IYH, IYL);
			++reg;
			reg.clear_upper();
			reg.split_value(UIY, IYH, IYL);
		} break;
		case DEC_BC: {
			reg24_pair reg;
			reg.set_value(UBC, B, C);
			--reg;
			reg.split_value(UBC, B, C);
		} break;
		case DEC_DE: {
			reg24_pair reg;
			reg.set_value(UDE, D, E);
			--reg;
			reg.split_value(UDE, D, E);
		} break;
		case DEC_HL: {
			reg24_pair reg;
			reg.set_value(UHL, H, L);
			--reg;
			reg.split_value(UHL, H, L);
		} break;
		case DEC_SP: {
			SP_set_unknown();
		} break;
		case DEC_IX: {
			reg24_pair reg;
			reg.set_value(UIX, IXH, IXL);
			--reg;
			reg.split_value(UIX, IXH, IXL);
		} break;
		case DEC_IY: {
			reg24_pair reg;
			reg.set_value(UIY, IYH, IYL);
			--reg;
			reg.split_value(UIY, IYH, IYL);
		} break;
		case DEC_BC_SIS: {
			reg24_pair reg;
			reg.set_value(UBC, B, C);
			--reg;
			reg.clear_upper();
			reg.split_value(UBC, B, C);
		} break;
		case DEC_DE_SIS: {
			reg24_pair reg;
			reg.set_value(UDE, D, E);
			--reg;
			reg.clear_upper();
			reg.split_value(UDE, D, E);
		} break;
		case DEC_HL_SIS: {
			reg24_pair reg;
			reg.set_value(UHL, H, L);
			--reg;
			reg.clear_upper();
			reg.split_value(UHL, H, L);
		} break;
		case DEC_SP_SIS: {
			SP_set_unknown();
		} break;
		case DEC_IX_SIS: {
			reg24_pair reg;
			reg.set_value(UIX, IXH, IXL);
			--reg;
			reg.clear_upper();
			reg.split_value(UIX, IXH, IXL);
		} break;
		case DEC_IY_SIS: {
			reg24_pair reg;
			reg.set_value(UIY, IYH, IYL);
			--reg;
			reg.clear_upper();
			reg.split_value(UIY, IYH, IYL);
		} break;
		case MLT_BC: {
			reg24_pair reg;
			reg.set_value(UBC, B, C);
			reg_mlt(reg);
			reg.split_value(UBC, B, C);
		} break;
		case MLT_DE: {
			reg24_pair reg;
			reg.set_value(UDE, D, E);
			reg_mlt(reg);
			reg.split_value(UDE, D, E);
		} break;
		case MLT_HL: {
			reg24_pair reg;
			reg.set_value(UHL, H, L);
			reg_mlt(reg);
			reg.split_value(UHL, H, L);
		} break;
		case MLT_SP: {
			SP_set_unknown();
		} break;
		case LD_A_N: { A.set_value(imm8); } break;
		case LD_B_N: { B.set_value(imm8); } break;
		case LD_C_N: { C.set_value(imm8); } break;
		case LD_D_N: { D.set_value(imm8); } break;
		case LD_E_N: { E.set_value(imm8); } break;
		case LD_H_N: { H.set_value(imm8); } break;
		case LD_L_N: { L.set_value(imm8); } break;
		case LD_IXL_N: { IXL.set_value(imm8); } break;
		case LD_IXH_N: { IXH.set_value(imm8); } break;
		case LD_IYL_N: { IYL.set_value(imm8); } break;
		case LD_IYH_N: { IYH.set_value(imm8); } break;
		case LD_A_B: { A.set_value(B); } break;
		case LD_A_C: { A.set_value(C); } break;
		case LD_A_D: { A.set_value(D); } break;
		case LD_A_E: { A.set_value(E); } break;
		case LD_A_H: { A.set_value(H); } break;
		case LD_A_L: { A.set_value(L); } break;
		case LD_A_IXH: { A.set_value(IXH); } break;
		case LD_A_IXL: { A.set_value(IXL); } break;
		case LD_A_IYH: { A.set_value(IYH); } break;
		case LD_A_IYL: { A.set_value(IYL); } break;
		case LD_A_ADDR:
		case LD_A_PBC:
		case LD_A_PDE:
		case LD_A_PHL:
		case LD_A_PIX:
		case LD_A_PIY: {
			A.set_unknown();
		} break;
		case LD_B_PHL:
		case LD_B_PIX:
		case LD_B_PIY: {
			B.set_unknown();
		} break;
		case LD_C_PHL:
		case LD_C_PIX:
		case LD_C_PIY: {
			C.set_unknown();
		} break;
		case LD_D_PHL:
		case LD_D_PIX:
		case LD_D_PIY: {
			D.set_unknown();
		} break;
		case LD_E_PHL:
		case LD_E_PIX:
		case LD_E_PIY: {
			E.set_unknown();
		} break;
		case LD_H_PHL:
		case LD_H_PIX:
		case LD_H_PIY: {
			H.set_unknown();
		} break;
		case LD_L_PHL:
		case LD_L_PIX:
		case LD_L_PIY: {
			L.set_unknown();
		} break;
		case LD_B_A: {
			B.set_value(A);
		} break;
		case LD_B_C: {
			B.set_value(C);
		} break;
		case LD_B_D: {
			B.set_value(D);
		} break;
		case LD_B_E: {
			B.set_value(E);
		} break;
		case LD_B_H: {
			B.set_value(H);
		} break;
		case LD_B_L: {
			B.set_value(L);
		} break;
		case LD_B_IXH: {
			B.set_value(IXH);
		} break;
		case LD_B_IXL: {
			B.set_value(IXL);
		} break;
		case LD_B_IYH: {
			B.set_value(IYH);
		} break;
		case LD_B_IYL: {
			B.set_value(IYL);
		} break;

		case LD_C_A: {
			C.set_value(A);
		} break;
		case LD_C_B: {
			C.set_value(B);
		} break;
		case LD_C_D: {
			C.set_value(D);
		} break;
		case LD_C_E: {
			C.set_value(E);
		} break;
		case LD_C_H: {
			C.set_value(H);
		} break;
		case LD_C_L: {
			C.set_value(L);
		} break;
		case LD_C_IXH: {
			C.set_value(IXH);
		} break;
		case LD_C_IXL: {
			C.set_value(IXL);
		} break;
		case LD_C_IYH: {
			C.set_value(IYH);
		} break;
		case LD_C_IYL: {
			C.set_value(IYL);
		} break;

		case LD_D_A: {
			D.set_value(A);
		} break;
		case LD_D_B: {
			D.set_value(B);
		} break;
		case LD_D_C: {
			D.set_value(C);
		} break;
		case LD_D_E: {
			D.set_value(E);
		} break;
		case LD_D_H: {
			D.set_value(H);
		} break;
		case LD_D_L: {
			D.set_value(L);
		} break;
		case LD_D_IXH: {
			D.set_value(IXH);
		} break;
		case LD_D_IXL: {
			D.set_value(IXL);
		} break;
		case LD_D_IYH: {
			D.set_value(IYH);
		} break;
		case LD_D_IYL: {
			D.set_value(IYL);
		} break;

		case LD_E_A: {
			E.set_value(A);
		} break;
		case LD_E_B: {
			E.set_value(B);
		} break;
		case LD_E_C: {
			E.set_value(C);
		} break;
		case LD_E_D: {
			E.set_value(D);
		} break;
		case LD_E_H: {
			E.set_value(H);
		} break;
		case LD_E_L: {
			E.set_value(L);
		} break;
		case LD_E_IXH: {
			E.set_value(IXH);
		} break;
		case LD_E_IXL: {
			E.set_value(IXL);
		} break;
		case LD_E_IYH: {
			E.set_value(IYH);
		} break;
		case LD_E_IYL: {
			E.set_value(IYL);
		} break;

		case LD_H_A: {
			H.set_value(A);
		} break;
		case LD_H_B: {
			H.set_value(B);
		} break;
		case LD_H_C: {
			H.set_value(C);
		} break;
		case LD_H_D: {
			H.set_value(D);
		} break;
		case LD_H_E: {
			H.set_value(E);
		} break;
		case LD_H_L: {
			H.set_value(L);
		} break;

		case LD_L_A: {
			L.set_value(A);
		} break;
		case LD_L_B: {
			L.set_value(B);
		} break;
		case LD_L_C: {
			L.set_value(C);
		} break;
		case LD_L_D: {
			L.set_value(D);
		} break;
		case LD_L_E: {
			L.set_value(E);
		} break;
		case LD_L_H: {
			L.set_value(H);
		} break;

		case LD_IXH_A: {
			IXH.set_value(A);
		} break;
		case LD_IXH_B: {
			IXH.set_value(B);
		} break;
		case LD_IXH_C: {
			IXH.set_value(C);
		} break;
		case LD_IXH_D: {
			IXH.set_value(D);
		} break;
		case LD_IXH_E: {
			IXH.set_value(E);
		} break;
		case LD_IXH_IXL: {
			IXH.set_value(IXL);
		} break;

		case LD_IXL_A: {
			IXL.set_value(A);
		} break;
		case LD_IXL_B: {
			IXL.set_value(B);
		} break;
		case LD_IXL_C: {
			IXL.set_value(C);
		} break;
		case LD_IXL_D: {
			IXL.set_value(D);
		} break;
		case LD_IXL_E: {
			IXL.set_value(E);
		} break;
		case LD_IXL_IXH: {
			IXL.set_value(IXH);
		} break;

		case LD_IYH_A: {
			IYH.set_value(A);
		} break;
		case LD_IYH_B: {
			IYH.set_value(B);
		} break;
		case LD_IYH_C: {
			IYH.set_value(C);
		} break;
		case LD_IYH_D: {
			IYH.set_value(D);
		} break;
		case LD_IYH_E: {
			IYH.set_value(E);
		} break;
		case LD_IYH_IYL: {
			IYH.set_value(IYL);
		} break;

		case LD_IYL_A: {
			IYL.set_value(A);
		} break;
		case LD_IYL_B: {
			IYL.set_value(B);
		} break;
		case LD_IYL_C: {
			IYL.set_value(C);
		} break;
		case LD_IYL_D: {
			IYL.set_value(D);
		} break;
		case LD_IYL_E: {
			IYL.set_value(E);
		} break;
		case LD_IYL_IYH: {
			IYL.set_value(IYH);
		} break;
		case LD_HL_N: {
			imm24.split_value(UHL, H, L);
		} break;
		case LD_HL_N_SIS: {
			imm16.split_value(UHL, H, L);
		} break;
		case LD_HL_ADDR:
		case LD_HL_PHL:
		case LD_HL_PIX:
		case LD_HL_PIY: {
			HL_set_unknown();
		} break;
		case LD_DE_N: {
			imm24.split_value(UDE, D, E);
		} break;
		case LD_DE_N_SIS: {
			imm16.split_value(UDE, D, E);
		} break;
		case LD_DE_ADDR:
		case LD_DE_PHL:
		case LD_DE_PIX:
		case LD_DE_PIY: {
			DE_set_unknown();
		} break;
		case LD_BC_N: {
			imm24.split_value(UBC, B, C);
		} break;
		case LD_BC_N_SIS: {
			imm16.split_value(UBC, B, C);
		} break;
		case LD_BC_ADDR:
		case LD_BC_PHL:
		case LD_BC_PIX:
		case LD_BC_PIY: {
			BC_set_unknown();
		} break;
		case LD_SP_N:
		case LD_SP_N_SIS: {

		} break;
		case LD_SP_HL:
		case LD_SP_IX:
		case LD_SP_IY:
		case LD_SP_ADDR: {

		} break;
		case LD_IX_N: {
			imm24.split_value(UIX, IXH, IXL);
		} break;
		case LD_IX_N_SIS: {
			imm16.split_value(UIX, IXH, IXL);
		} break;
		case LD_IX_ADDR:
		case LD_IX_PHL:
		case LD_IX_PIX:
		case LD_IX_PIY: {
			IX_set_unknown();
		} break;
		case LD_IY_N: {
			imm24.split_value(UIY, IYH, IYL);
		} break;
		case LD_IY_N_SIS: {
			imm16.split_value(UIY, IYH, IYL);
		} break;
		case LD_IY_ADDR:
		case LD_IY_PHL:
		case LD_IY_PIX:
		case LD_IY_PIY: {
			IY_set_unknown();
		} break;

		case LD_ADDR_A:
		case LD_PBC_A:
		case LD_PDE_A:
		case LD_PHL_A:
		case LD_PIX_A:
		case LD_PIY_A: {
			
		} break;
		
		case LD_PHL_B:
		case LD_PIX_B:
		case LD_PIY_B: {
			
		} break;
		case LD_PHL_C:
		case LD_PIX_C:
		case LD_PIY_C: {
			
		} break;
		case LD_PHL_D:
		case LD_PIX_D:
		case LD_PIY_D: {
			
		} break;
		case LD_PHL_E:
		case LD_PIX_E:
		case LD_PIY_E: {
			
		} break;
		case LD_PHL_H:
		case LD_PIX_H:
		case LD_PIY_H: {
			
		} break;
		case LD_PHL_L:
		case LD_PIX_L:
		case LD_PIY_L: {
			
		} break;

		case LD_ADDR_HL:
		case LD_PHL_HL:
		case LD_PIX_HL:
		case LD_PIY_HL: {
			
		} break;
		case LD_ADDR_DE:
		case LD_PHL_DE:
		case LD_PIX_DE:
		case LD_PIY_DE: {
			
		} break;
		case LD_ADDR_BC:
		case LD_PHL_BC:
		case LD_PIX_BC:
		case LD_PIY_BC: {
			
		} break;
		case LD_ADDR_IX:
		case LD_PHL_IX:
		case LD_PIX_IX:
		case LD_PIY_IX: {
			
		} break;
		case LD_ADDR_IY:
		case LD_PHL_IY:
		case LD_PIX_IY:
		case LD_PIY_IY: {
			
		} break;
		case LD_ADDR_SP: {
			
		} break;

		case LEA_BC_IX: { set_BC(reg24_lea(get_BC(), get_IX(), imm_offset)); } break;
		case LEA_BC_IY: { set_BC(reg24_lea(get_BC(), get_IY(), imm_offset)); } break;
		case LEA_DE_IX: { set_DE(reg24_lea(get_DE(), get_IX(), imm_offset)); } break;
		case LEA_DE_IY: { set_DE(reg24_lea(get_DE(), get_IY(), imm_offset)); } break;
		case LEA_HL_IX: { set_HL(reg24_lea(get_HL(), get_IX(), imm_offset)); } break;
		case LEA_HL_IY: { set_HL(reg24_lea(get_HL(), get_IY(), imm_offset)); } break;
		case LEA_IX_IX: { set_IX(reg24_lea(get_IX(), get_IX(), imm_offset)); } break;
		case LEA_IX_IY: { set_IX(reg24_lea(get_IX(), get_IY(), imm_offset)); } break;
		case LEA_IY_IX: { set_IY(reg24_lea(get_IY(), get_IX(), imm_offset)); } break;
		case LEA_IY_IY: { set_IY(reg24_lea(get_IY(), get_IY(), imm_offset)); } break;

		case LEA_BC_IX_SIS: { set_BC(reg16_lea(get_BC(), get_IX(), imm_offset)); } break;
		case LEA_BC_IY_SIS: { set_BC(reg16_lea(get_BC(), get_IY(), imm_offset)); } break;
		case LEA_DE_IX_SIS: { set_DE(reg16_lea(get_DE(), get_IX(), imm_offset)); } break;
		case LEA_DE_IY_SIS: { set_DE(reg16_lea(get_DE(), get_IY(), imm_offset)); } break;
		case LEA_HL_IX_SIS: { set_HL(reg16_lea(get_HL(), get_IX(), imm_offset)); } break;
		case LEA_HL_IY_SIS: { set_HL(reg16_lea(get_HL(), get_IY(), imm_offset)); } break;
		case LEA_IX_IX_SIS: { set_IX(reg16_lea(get_IX(), get_IX(), imm_offset)); } break;
		case LEA_IX_IY_SIS: { set_IX(reg16_lea(get_IX(), get_IY(), imm_offset)); } break;
		case LEA_IY_IX_SIS: { set_IY(reg16_lea(get_IY(), get_IX(), imm_offset)); } break;
		case LEA_IY_IY_SIS: { set_IY(reg16_lea(get_IY(), get_IY(), imm_offset)); } break;

		case SET_0_A: { A.bit_set(0); } break;
		case SET_0_B: { B.bit_set(0); } break;
		case SET_0_C: { C.bit_set(0); } break;
		case SET_0_D: { D.bit_set(0); } break;
		case SET_0_E: { E.bit_set(0); } break;
		case SET_0_H: { H.bit_set(0); } break;
		case SET_0_L: { L.bit_set(0); } break;
		case SET_1_A: { A.bit_set(1); } break;
		case SET_1_B: { B.bit_set(1); } break;
		case SET_1_C: { C.bit_set(1); } break;
		case SET_1_D: { D.bit_set(1); } break;
		case SET_1_E: { E.bit_set(1); } break;
		case SET_1_H: { H.bit_set(1); } break;
		case SET_1_L: { L.bit_set(1); } break;
		case SET_2_A: { A.bit_set(2); } break;
		case SET_2_B: { B.bit_set(2); } break;
		case SET_2_C: { C.bit_set(2); } break;
		case SET_2_D: { D.bit_set(2); } break;
		case SET_2_E: { E.bit_set(2); } break;
		case SET_2_H: { H.bit_set(2); } break;
		case SET_2_L: { L.bit_set(2); } break;
		case SET_3_A: { A.bit_set(3); } break;
		case SET_3_B: { B.bit_set(3); } break;
		case SET_3_C: { C.bit_set(3); } break;
		case SET_3_D: { D.bit_set(3); } break;
		case SET_3_E: { E.bit_set(3); } break;
		case SET_3_H: { H.bit_set(3); } break;
		case SET_3_L: { L.bit_set(3); } break;
		case SET_4_A: { A.bit_set(4); } break;
		case SET_4_B: { B.bit_set(4); } break;
		case SET_4_C: { C.bit_set(4); } break;
		case SET_4_D: { D.bit_set(4); } break;
		case SET_4_E: { E.bit_set(4); } break;
		case SET_4_H: { H.bit_set(4); } break;
		case SET_4_L: { L.bit_set(4); } break;
		case SET_5_A: { A.bit_set(5); } break;
		case SET_5_B: { B.bit_set(5); } break;
		case SET_5_C: { C.bit_set(5); } break;
		case SET_5_D: { D.bit_set(5); } break;
		case SET_5_E: { E.bit_set(5); } break;
		case SET_5_H: { H.bit_set(5); } break;
		case SET_5_L: { L.bit_set(5); } break;
		case SET_6_A: { A.bit_set(6); } break;
		case SET_6_B: { B.bit_set(6); } break;
		case SET_6_C: { C.bit_set(6); } break;
		case SET_6_D: { D.bit_set(6); } break;
		case SET_6_E: { E.bit_set(6); } break;
		case SET_6_H: { H.bit_set(6); } break;
		case SET_6_L: { L.bit_set(6); } break;
		case SET_7_A: { A.bit_set(7); } break;
		case SET_7_B: { B.bit_set(7); } break;
		case SET_7_C: { C.bit_set(7); } break;
		case SET_7_D: { D.bit_set(7); } break;
		case SET_7_E: { E.bit_set(7); } break;
		case SET_7_H: { H.bit_set(7); } break;
		case SET_7_L: { L.bit_set(7); } break;			

		case RES_0_A: { A.bit_clear(0); } break;
		case RES_0_B: { B.bit_clear(0); } break;
		case RES_0_C: { C.bit_clear(0); } break;
		case RES_0_D: { D.bit_clear(0); } break;
		case RES_0_E: { E.bit_clear(0); } break;
		case RES_0_H: { H.bit_clear(0); } break;
		case RES_0_L: { L.bit_clear(0); } break;
		case RES_1_A: { A.bit_clear(1); } break;
		case RES_1_B: { B.bit_clear(1); } break;
		case RES_1_C: { C.bit_clear(1); } break;
		case RES_1_D: { D.bit_clear(1); } break;
		case RES_1_E: { E.bit_clear(1); } break;
		case RES_1_H: { H.bit_clear(1); } break;
		case RES_1_L: { L.bit_clear(1); } break;
		case RES_2_A: { A.bit_clear(2); } break;
		case RES_2_B: { B.bit_clear(2); } break;
		case RES_2_C: { C.bit_clear(2); } break;
		case RES_2_D: { D.bit_clear(2); } break;
		case RES_2_E: { E.bit_clear(2); } break;
		case RES_2_H: { H.bit_clear(2); } break;
		case RES_2_L: { L.bit_clear(2); } break;
		case RES_3_A: { A.bit_clear(3); } break;
		case RES_3_B: { B.bit_clear(3); } break;
		case RES_3_C: { C.bit_clear(3); } break;
		case RES_3_D: { D.bit_clear(3); } break;
		case RES_3_E: { E.bit_clear(3); } break;
		case RES_3_H: { H.bit_clear(3); } break;
		case RES_3_L: { L.bit_clear(3); } break;
		case RES_4_A: { A.bit_clear(4); } break;
		case RES_4_B: { B.bit_clear(4); } break;
		case RES_4_C: { C.bit_clear(4); } break;
		case RES_4_D: { D.bit_clear(4); } break;
		case RES_4_E: { E.bit_clear(4); } break;
		case RES_4_H: { H.bit_clear(4); } break;
		case RES_4_L: { L.bit_clear(4); } break;
		case RES_5_A: { A.bit_clear(5); } break;
		case RES_5_B: { B.bit_clear(5); } break;
		case RES_5_C: { C.bit_clear(5); } break;
		case RES_5_D: { D.bit_clear(5); } break;
		case RES_5_E: { E.bit_clear(5); } break;
		case RES_5_H: { H.bit_clear(5); } break;
		case RES_5_L: { L.bit_clear(5); } break;
		case RES_6_A: { A.bit_clear(6); } break;
		case RES_6_B: { B.bit_clear(6); } break;
		case RES_6_C: { C.bit_clear(6); } break;
		case RES_6_D: { D.bit_clear(6); } break;
		case RES_6_E: { E.bit_clear(6); } break;
		case RES_6_H: { H.bit_clear(6); } break;
		case RES_6_L: { L.bit_clear(6); } break;
		case RES_7_A: { A.bit_clear(7); } break;
		case RES_7_B: { B.bit_clear(7); } break;
		case RES_7_C: { C.bit_clear(7); } break;
		case RES_7_D: { D.bit_clear(7); } break;
		case RES_7_E: { E.bit_clear(7); } break;
		case RES_7_H: { H.bit_clear(7); } break;
		case RES_7_L: { L.bit_clear(7); } break;
		
		case ADD_HL_BC: { set_HL(reg24_add(get_HL(), get_BC())); } break;
		case ADD_HL_DE: { set_HL(reg24_add(get_HL(), get_DE())); } break;
		case ADD_HL_SP: { set_HL(reg24_add(get_HL(), get_SP())); } break;
		case ADD_IX_BC: { set_IX(reg24_add(get_IX(), get_BC())); } break;
		case ADD_IX_DE: { set_IX(reg24_add(get_IX(), get_DE())); } break;
		case ADD_IX_SP: { set_IX(reg24_add(get_IX(), get_SP())); } break;
		case ADD_IY_BC: { set_IY(reg24_add(get_IY(), get_BC())); } break;
		case ADD_IY_DE: { set_IY(reg24_add(get_IY(), get_DE())); } break;
		case ADD_IY_SP: { set_IY(reg24_add(get_IY(), get_SP())); } break;

		case ADD_HL_BC_SIS: { set_HL(reg16_add(get_HL(), get_BC())); } break;
		case ADD_HL_DE_SIS: { set_HL(reg16_add(get_HL(), get_DE())); } break;
		case ADD_HL_SP_SIS: { set_HL(reg16_add(get_HL(), get_SP())); } break;
		case ADD_IX_BC_SIS: { set_IX(reg16_add(get_IX(), get_BC())); } break;
		case ADD_IX_DE_SIS: { set_IX(reg16_add(get_IX(), get_DE())); } break;
		case ADD_IX_SP_SIS: { set_IX(reg16_add(get_IX(), get_SP())); } break;
		case ADD_IY_BC_SIS: { set_IY(reg16_add(get_IY(), get_BC())); } break;
		case ADD_IY_DE_SIS: { set_IY(reg16_add(get_IY(), get_DE())); } break;
		case ADD_IY_SP_SIS: { set_IY(reg16_add(get_IY(), get_SP())); } break;

		case ADC_HL_BC: { set_HL(reg24_adc(get_HL(), get_BC())); } break;
		case ADC_HL_DE: { set_HL(reg24_adc(get_HL(), get_DE())); } break;
		case ADC_HL_SP: { set_HL(reg24_adc(get_HL(), get_SP())); } break;

		case ADC_HL_BC_SIS: { set_HL(reg16_adc(get_HL(), get_BC())); } break;
		case ADC_HL_DE_SIS: { set_HL(reg16_adc(get_HL(), get_DE())); } break;
		case ADC_HL_SP_SIS: { set_HL(reg16_adc(get_HL(), get_SP())); } break;

		case SBC_HL_BC: { set_HL(reg24_sbc(get_HL(), get_BC())); } break;
		case SBC_HL_DE: { set_HL(reg24_sbc(get_HL(), get_DE())); } break;
		case SBC_HL_SP: { set_HL(reg24_sbc(get_HL(), get_SP())); } break;

		case SBC_HL_BC_SIS: { set_HL(reg16_sbc(get_HL(), get_BC())); } break;
		case SBC_HL_DE_SIS: { set_HL(reg16_sbc(get_HL(), get_DE())); } break;
		case SBC_HL_SP_SIS: { set_HL(reg16_sbc(get_HL(), get_SP())); } break;

		case BIT_0_A: { reg_bit_test(A, 0); } break;
		case BIT_0_B: { reg_bit_test(B, 0); } break;
		case BIT_0_C: { reg_bit_test(C, 0); } break;
		case BIT_0_D: { reg_bit_test(D, 0); } break;
		case BIT_0_E: { reg_bit_test(E, 0); } break;
		case BIT_0_H: { reg_bit_test(H, 0); } break;
		case BIT_0_L: { reg_bit_test(L, 0); } break;
		case BIT_1_A: { reg_bit_test(A, 1); } break;
		case BIT_1_B: { reg_bit_test(B, 1); } break;
		case BIT_1_C: { reg_bit_test(C, 1); } break;
		case BIT_1_D: { reg_bit_test(D, 1); } break;
		case BIT_1_E: { reg_bit_test(E, 1); } break;
		case BIT_1_H: { reg_bit_test(H, 1); } break;
		case BIT_1_L: { reg_bit_test(L, 1); } break;
		case BIT_2_A: { reg_bit_test(A, 2); } break;
		case BIT_2_B: { reg_bit_test(B, 2); } break;
		case BIT_2_C: { reg_bit_test(C, 2); } break;
		case BIT_2_D: { reg_bit_test(D, 2); } break;
		case BIT_2_E: { reg_bit_test(E, 2); } break;
		case BIT_2_H: { reg_bit_test(H, 2); } break;
		case BIT_2_L: { reg_bit_test(L, 2); } break;
		case BIT_3_A: { reg_bit_test(A, 3); } break;
		case BIT_3_B: { reg_bit_test(B, 3); } break;
		case BIT_3_C: { reg_bit_test(C, 3); } break;
		case BIT_3_D: { reg_bit_test(D, 3); } break;
		case BIT_3_E: { reg_bit_test(E, 3); } break;
		case BIT_3_H: { reg_bit_test(H, 3); } break;
		case BIT_3_L: { reg_bit_test(L, 3); } break;
		case BIT_4_A: { reg_bit_test(A, 4); } break;
		case BIT_4_B: { reg_bit_test(B, 4); } break;
		case BIT_4_C: { reg_bit_test(C, 4); } break;
		case BIT_4_D: { reg_bit_test(D, 4); } break;
		case BIT_4_E: { reg_bit_test(E, 4); } break;
		case BIT_4_H: { reg_bit_test(H, 4); } break;
		case BIT_4_L: { reg_bit_test(L, 4); } break;
		case BIT_5_A: { reg_bit_test(A, 5); } break;
		case BIT_5_B: { reg_bit_test(B, 5); } break;
		case BIT_5_C: { reg_bit_test(C, 5); } break;
		case BIT_5_D: { reg_bit_test(D, 5); } break;
		case BIT_5_E: { reg_bit_test(E, 5); } break;
		case BIT_5_H: { reg_bit_test(H, 5); } break;
		case BIT_5_L: { reg_bit_test(L, 5); } break;
		case BIT_6_A: { reg_bit_test(A, 6); } break;
		case BIT_6_B: { reg_bit_test(B, 6); } break;
		case BIT_6_C: { reg_bit_test(C, 6); } break;
		case BIT_6_D: { reg_bit_test(D, 6); } break;
		case BIT_6_E: { reg_bit_test(E, 6); } break;
		case BIT_6_H: { reg_bit_test(H, 6); } break;
		case BIT_6_L: { reg_bit_test(L, 6); } break;
		case BIT_7_A: { reg_bit_test(A, 7); } break;
		case BIT_7_B: { reg_bit_test(B, 7); } break;
		case BIT_7_C: { reg_bit_test(C, 7); } break;
		case BIT_7_D: { reg_bit_test(D, 7); } break;
		case BIT_7_E: { reg_bit_test(E, 7); } break;
		case BIT_7_H: { reg_bit_test(H, 7); } break;
		case BIT_7_L: { reg_bit_test(L, 7); } break;			

		case BIT_0_PHL:
		case BIT_0_PIX:
		case BIT_0_PIY:
		case BIT_1_PHL:
		case BIT_1_PIX:
		case BIT_1_PIY:
		case BIT_2_PHL:
		case BIT_2_PIX:
		case BIT_2_PIY:
		case BIT_3_PHL:
		case BIT_3_PIX:
		case BIT_3_PIY:
		case BIT_4_PHL:
		case BIT_4_PIX:
		case BIT_4_PIY:
		case BIT_5_PHL:
		case BIT_5_PIX:
		case BIT_5_PIY:
		case BIT_6_PHL:
		case BIT_6_PIX:
		case BIT_6_PIY:
		case BIT_7_PHL:
		case BIT_7_PIX:
		case BIT_7_PIY:
		/* test bit */ {
			F.set_zero_unknown();
			F.set_sign_unknown();
			F.set_overflow_unknown();
		} break;

		case CPIR:
		case CPDR: {
			HL_set_unknown();
			BC_set_unknown();
			F.set_all_but_carry_unknown();
		} break;

		case LDIR:{
			set_HL(get_HL() + get_BC());
			set_DE(get_DE() + get_BC());
			set_BC(0);
			F.set_overflow(false);
		} break;

		case LDDR:{
			set_HL(get_HL() - get_BC());
			set_DE(get_DE() - get_BC());
			set_BC(0);
			F.set_overflow(false);
		} break;

		case CPI: {
			set_HL(--get_HL());
			set_BC(--get_BC());
			F.set_zero_unknown();
			F.set_sign_unknown();
			F.set_overflow(get_BC().is_nonzero());
		} break;
		case CPD: {
			set_HL(--get_HL());
			set_BC(--get_BC());
			F.set_zero_unknown();
			F.set_sign_unknown();
			F.set_overflow(get_BC().is_nonzero());
		} break;
		case LDI: {
			set_HL(++get_HL());
			set_HL(++get_DE());
			set_BC(--get_BC());
			F.set_zero(get_BC().is_zero());
		} break;
		case LDD: {
			set_HL(--get_HL());
			set_HL(--get_DE());
			set_BC(--get_BC());
			F.set_zero(get_BC().is_zero());
		} break;

		case INC_A  : { reg8_inc(A  ); } break;
		case INC_B  : { reg8_inc(B  ); } break;
		case INC_C  : { reg8_inc(C  ); } break;
		case INC_D  : { reg8_inc(D  ); } break;
		case INC_E  : { reg8_inc(E  ); } break;
		case INC_H  : { reg8_inc(H  ); } break;
		case INC_L  : { reg8_inc(L  ); } break;
		case INC_IXH: { reg8_inc(IXH); } break;
		case INC_IXL: { reg8_inc(IXL); } break;
		case INC_IYH: { reg8_inc(IYH); } break;
		case INC_IYL: { reg8_inc(IYL); } break;
		case DEC_A  : { reg8_dec(A  ); } break;
		case DEC_B  : { reg8_dec(B  ); } break;
		case DEC_C  : { reg8_dec(C  ); } break;
		case DEC_D  : { reg8_dec(D  ); } break;
		case DEC_E  : { reg8_dec(E  ); } break;
		case DEC_H  : { reg8_dec(H  ); } break;
		case DEC_L  : { reg8_dec(L  ); } break;
		case DEC_IXH: { reg8_dec(IXH); } break;
		case DEC_IXL: { reg8_dec(IXL); } break;
		case DEC_IYH: { reg8_dec(IYH); } break;
		case DEC_IYL: { reg8_dec(IYL); } break;

		case INC_PHL:
		case INC_PIX:
		case INC_PIY:
		case DEC_PHL:
		case DEC_PIX:
		case DEC_PIY:
		/* only carry preserved */ {
			F.set_all_but_carry_unknown();
		} break;

		/* 8 bit accumulate */

		case ADD_A_N  : { acc_add(imm8); } break;
		case ADD_A_B  : { acc_add(B); } break;
		case ADD_A_C  : { acc_add(C); } break;
		case ADD_A_D  : { acc_add(D); } break;
		case ADD_A_E  : { acc_add(E); } break;
		case ADD_A_H  : { acc_add(H); } break;
		case ADD_A_L  : { acc_add(L); } break;
		case ADD_A_IXH: { acc_add(IXH); } break;
		case ADD_A_IXL: { acc_add(IXL); } break;
		case ADD_A_IYH: { acc_add(IYH); } break;
		case ADD_A_IYL: { acc_add(IYL); } break;
		case ADD_A_PHL: { acc_add(unknown8); } break;
		case ADD_A_PIX: { acc_add(unknown8); } break;
		case ADD_A_PIY: { acc_add(unknown8); } break;
		
		case ADC_A_N  : { acc_adc(imm8); } break;
		case ADC_A_B  : { acc_adc(B); } break;
		case ADC_A_C  : { acc_adc(C); } break;
		case ADC_A_D  : { acc_adc(D); } break;
		case ADC_A_E  : { acc_adc(E); } break;
		case ADC_A_H  : { acc_adc(H); } break;
		case ADC_A_L  : { acc_adc(L); } break;
		case ADC_A_IXH: { acc_adc(IXH); } break;
		case ADC_A_IXL: { acc_adc(IXL); } break;
		case ADC_A_IYH: { acc_adc(IYH); } break;
		case ADC_A_IYL: { acc_adc(IYL); } break;
		case ADC_A_PHL: { acc_adc(unknown8); } break;
		case ADC_A_PIX: { acc_adc(unknown8); } break;
		case ADC_A_PIY: { acc_adc(unknown8); } break;

		case SUB_A_N  : { acc_sub(imm8); } break;
		case SUB_A_B  : { acc_sub(B); } break;
		case SUB_A_C  : { acc_sub(C); } break;
		case SUB_A_D  : { acc_sub(D); } break;
		case SUB_A_E  : { acc_sub(E); } break;
		case SUB_A_H  : { acc_sub(H); } break;
		case SUB_A_L  : { acc_sub(L); } break;
		case SUB_A_IXH: { acc_sub(IXH); } break;
		case SUB_A_IXL: { acc_sub(IXL); } break;
		case SUB_A_IYH: { acc_sub(IYH); } break;
		case SUB_A_IYL: { acc_sub(IYL); } break;
		case SUB_A_PHL: { acc_sub(unknown8); } break;
		case SUB_A_PIX: { acc_sub(unknown8); } break;
		case SUB_A_PIY: { acc_sub(unknown8); } break;

		case SBC_A_N  : { acc_sbc(imm8); } break;
		case SBC_A_B  : { acc_sbc(B); } break;
		case SBC_A_C  : { acc_sbc(C); } break;
		case SBC_A_D  : { acc_sbc(D); } break;
		case SBC_A_E  : { acc_sbc(E); } break;
		case SBC_A_H  : { acc_sbc(H); } break;
		case SBC_A_L  : { acc_sbc(L); } break;
		case SBC_A_IXH: { acc_sbc(IXH); } break;
		case SBC_A_IXL: { acc_sbc(IXL); } break;
		case SBC_A_IYH: { acc_sbc(IYH); } break;
		case SBC_A_IYL: { acc_sbc(IYL); } break;
		case SBC_A_PHL: { acc_sbc(unknown8); } break;
		case SBC_A_PIX: { acc_sbc(unknown8); } break;
		case SBC_A_PIY: { acc_sbc(unknown8); } break;

		case CP_A_N  : { acc_cp(imm8); } break;
		case CP_A_B  : { acc_cp(B); } break;
		case CP_A_C  : { acc_cp(C); } break;
		case CP_A_D  : { acc_cp(D); } break;
		case CP_A_E  : { acc_cp(E); } break;
		case CP_A_H  : { acc_cp(H); } break;
		case CP_A_L  : { acc_cp(L); } break;
		case CP_A_IXH: { acc_cp(IXH); } break;
		case CP_A_IXL: { acc_cp(IXL); } break;
		case CP_A_IYH: { acc_cp(IYH); } break;
		case CP_A_IYL: { acc_cp(IYL); } break;
		case CP_A_PHL: { acc_cp(unknown8); } break;
		case CP_A_PIX: { acc_cp(unknown8); } break;
		case CP_A_PIY: { acc_cp(unknown8); } break;

		case OR_A_N  : { acc_or(imm8); } break;
		case OR_A_B  : { acc_or(B  ); } break;
		case OR_A_C  : { acc_or(C  ); } break;
		case OR_A_D  : { acc_or(D  ); } break;
		case OR_A_E  : { acc_or(E  ); } break;
		case OR_A_H  : { acc_or(H  ); } break;
		case OR_A_L  : { acc_or(L  ); } break;
		case OR_A_IXH: { acc_or(IXH); } break;
		case OR_A_IXL: { acc_or(IXL); } break;
		case OR_A_IYH: { acc_or(IYH); } break;
		case OR_A_IYL: { acc_or(IYL); } break;
		case OR_A_PHL: { acc_or(unknown8); } break;
		case OR_A_PIX: { acc_or(unknown8); } break;
		case OR_A_PIY: { acc_or(unknown8); } break;
	
		case XOR_A_N  : { acc_xor(imm8); } break;
		case XOR_A_B  : { acc_xor(B  ); } break;
		case XOR_A_C  : { acc_xor(C  ); } break;
		case XOR_A_D  : { acc_xor(D  ); } break;
		case XOR_A_E  : { acc_xor(E  ); } break;
		case XOR_A_H  : { acc_xor(H  ); } break;
		case XOR_A_L  : { acc_xor(L  ); } break;
		case XOR_A_IXH: { acc_xor(IXH); } break;
		case XOR_A_IXL: { acc_xor(IXL); } break;
		case XOR_A_IYH: { acc_xor(IYH); } break;
		case XOR_A_IYL: { acc_xor(IYL); } break;
		case XOR_A_PHL: { acc_xor(unknown8); } break;
		case XOR_A_PIX: { acc_xor(unknown8); } break;
		case XOR_A_PIY: { acc_xor(unknown8); } break;

		case AND_A_N  : { acc_and(imm8); } break;
		case AND_A_B  : { acc_and(B  ); } break;
		case AND_A_C  : { acc_and(C  ); } break;
		case AND_A_D  : { acc_and(D  ); } break;
		case AND_A_E  : { acc_and(E  ); } break;
		case AND_A_H  : { acc_and(H  ); } break;
		case AND_A_L  : { acc_and(L  ); } break;
		case AND_A_IXH: { acc_and(IXH); } break;
		case AND_A_IXL: { acc_and(IXL); } break;
		case AND_A_IYH: { acc_and(IYH); } break;
		case AND_A_IYL: { acc_and(IYL); } break;
		case AND_A_PHL: { acc_and(unknown8); } break;
		case AND_A_PIX: { acc_and(unknown8); } break;
		case AND_A_PIY: { acc_and(unknown8); } break;

		case TST_A_N  : { acc_tst(imm8); } break;
		case TST_A_B  : { acc_tst(B); } break;
		case TST_A_C  : { acc_tst(C); } break;
		case TST_A_D  : { acc_tst(D); } break;
		case TST_A_E  : { acc_tst(E); } break;
		case TST_A_H  : { acc_tst(H); } break;
		case TST_A_L  : { acc_tst(L); } break;
		case TST_A_PHL: { acc_tst(unknown8); } break;

		/* 8 bit shift */
		
		case RL_A  : { reg_rl(A); } break;
		case RL_B  : { reg_rl(B); } break;
		case RL_C  : { reg_rl(C); } break;
		case RL_D  : { reg_rl(D); } break;
		case RL_E  : { reg_rl(E); } break;
		case RL_H  : { reg_rl(H); } break;
		case RL_L  : { reg_rl(L); } break;
		case RL_PHL: { reg_rl(unknown8); } break;
		case RL_PIX: { reg_rl(unknown8); } break;
		case RL_PIY: { reg_rl(unknown8); } break;

		case RLC_A  : { reg_rlc(A); } break;
		case RLC_B  : { reg_rlc(B); } break;
		case RLC_C  : { reg_rlc(C); } break;
		case RLC_D  : { reg_rlc(D); } break;
		case RLC_E  : { reg_rlc(E); } break;
		case RLC_H  : { reg_rlc(H); } break;
		case RLC_L  : { reg_rlc(L); } break;
		case RLC_PHL: { reg_rlc(unknown8); } break;
		case RLC_PIX: { reg_rlc(unknown8); } break;
		case RLC_PIY: { reg_rlc(unknown8); } break;

		case RR_A  : { reg_rr(A); } break;
		case RR_B  : { reg_rr(B); } break;
		case RR_C  : { reg_rr(C); } break;
		case RR_D  : { reg_rr(D); } break;
		case RR_E  : { reg_rr(E); } break;
		case RR_H  : { reg_rr(H); } break;
		case RR_L  : { reg_rr(L); } break;
		case RR_PHL: { reg_rr(unknown8); } break;
		case RR_PIX: { reg_rr(unknown8); } break;
		case RR_PIY: { reg_rr(unknown8); } break;

		case RRC_A  : { reg_rrc(A); } break;
		case RRC_B  : { reg_rrc(B); } break;
		case RRC_C  : { reg_rrc(C); } break;
		case RRC_D  : { reg_rrc(D); } break;
		case RRC_E  : { reg_rrc(E); } break;
		case RRC_H  : { reg_rrc(H); } break;
		case RRC_L  : { reg_rrc(L); } break;
		case RRC_PHL: { reg_rrc(unknown8); } break;
		case RRC_PIX: { reg_rrc(unknown8); } break;
		case RRC_PIY: { reg_rrc(unknown8); } break;

		case SRL_A  : { reg_srl(A); } break;
		case SRL_B  : { reg_srl(B); } break;
		case SRL_C  : { reg_srl(C); } break;
		case SRL_D  : { reg_srl(D); } break;
		case SRL_E  : { reg_srl(E); } break;
		case SRL_H  : { reg_srl(H); } break;
		case SRL_L  : { reg_srl(L); } break;
		case SRL_PHL: { reg_srl(unknown8); } break;
		case SRL_PIX: { reg_srl(unknown8); } break;
		case SRL_PIY: { reg_srl(unknown8); } break;

		case SLA_A  : { reg_sla(A); } break;
		case SLA_B  : { reg_sla(B); } break;
		case SLA_C  : { reg_sla(C); } break;
		case SLA_D  : { reg_sla(D); } break;
		case SLA_E  : { reg_sla(E); } break;
		case SLA_H  : { reg_sla(H); } break;
		case SLA_L  : { reg_sla(L); } break;
		case SLA_PHL: { reg_sla(unknown8); } break;
		case SLA_PIX: { reg_sla(unknown8); } break;
		case SLA_PIY: { reg_sla(unknown8); } break;

		case SRA_A  : { reg_sra(A); } break;
		case SRA_B  : { reg_sra(B); } break;
		case SRA_C  : { reg_sra(C); } break;
		case SRA_D  : { reg_sra(D); } break;
		case SRA_E  : { reg_sra(E); } break;
		case SRA_H  : { reg_sra(H); } break;
		case SRA_L  : { reg_sra(L); } break;
		case SRA_PHL: { reg_sra(unknown8); } break;
		case SRA_PIX: { reg_sra(unknown8); } break;
		case SRA_PIY: { reg_sra(unknown8); } break;

		/* BCD */

		case RLD:
		case RRD:
		case DAA: {
			F.set_flags_unknown();
			A.set_unknown();
		} break;

		/* push to stack */

		case PEA_IX:
		case PEA_IY:
		case PUSH_AF:
		case PUSH_BC:
		case PUSH_DE:
		case PUSH_HL:
		case PUSH_IX:
		case PUSH_IY: {
			
		} break;

		/* call */
		case CALL_C:
		case CALL_NC:
		case CALL_Z:
		case CALL_NZ:
		case CALL_P:
		case CALL_M:
		case CALL_PO:
		case CALL_PE:
		case CALL: {

		} break;

		/* jump uncondintional */

		case JP_HL:
		case JP_IX:
		case JP_IY:
		case JR:
		case JP: {

		} break;


		/* jump conditional (assuming false) */

		case JR_C:
		case JP_C: {
			F.set_carry(false);
		} break;

		case JR_NC:
		case JP_NC: {
			F.set_carry(true);
		} break;
		
		case JR_Z:
		case JP_Z: {
			F.set_zero(false);
		} break;
	
		case JR_NZ:
		case JP_NZ: {
			F.set_zero(true);
		} break;

		case JP_P: {
			F.set_sign(true);
		} break;
		case JP_M: {
			F.set_sign(false);
		} break;

		case JP_PO: {
			F.set_overflow(true);
		} break;
		case JP_PE: {
			F.set_overflow(false);
		} break;

		case DJNZ: {
			B.set_value(0);
		} break;

		/* return */

		case RET: {

		} break;

		/* return conditional (assuming false) */

		case RET_C: {
			F.set_carry(false);
		} break;
		case RET_NC: {
			F.set_carry(true);
		} break;
		case RET_Z: {
			F.set_zero(false);
		} break;
		case RET_NZ: {
			F.set_zero(true);
		} break;
		case RET_P: {
			F.set_sign(true);
		} break;
		case RET_M: {
			F.set_sign(false);
		} break;
		case RET_PO: {
			F.set_overflow(true);
		} break;
		case RET_PE: {
			F.set_overflow(false);
		} break;

		case LD_PHL_N:
		case LD_PIX_N:
		case LD_PIY_N:
		case SET_0_PHL:
		case SET_0_PIX:
		case SET_0_PIY:
		case SET_1_PHL:
		case SET_1_PIX:
		case SET_1_PIY:
		case SET_2_PHL:
		case SET_2_PIX:
		case SET_2_PIY:
		case SET_3_PHL:
		case SET_3_PIX:
		case SET_3_PIY:
		case SET_4_PHL:
		case SET_4_PIX:
		case SET_4_PIY:
		case SET_5_PHL:
		case SET_5_PIX:
		case SET_5_PIY:
		case SET_6_PHL:
		case SET_6_PIX:
		case SET_6_PIY:
		case SET_7_PHL:
		case SET_7_PIX:
		case SET_7_PIY:
		case RES_0_PHL:
		case RES_0_PIX:
		case RES_0_PIY:
		case RES_1_PHL:
		case RES_1_PIX:
		case RES_1_PIY:
		case RES_2_PHL:
		case RES_2_PIX:
		case RES_2_PIY:
		case RES_3_PHL:
		case RES_3_PIX:
		case RES_3_PIY:
		case RES_4_PHL:
		case RES_4_PIX:
		case RES_4_PIY:
		case RES_5_PHL:
		case RES_5_PIX:
		case RES_5_PIY:
		case RES_6_PHL:
		case RES_6_PIX:
		case RES_6_PIY:
		case RES_7_PHL:
		case RES_7_PIX:
		case RES_7_PIY:
		/* No state or flag change */
		break;

		case NOP:
		case LD_A_A:
		case LD_H_H:
		case LD_L_L:
		case LD_IXL_IXL:
		case LD_IXH_IXH:
		case LD_IYL_IYL:
		case LD_IYH_IYH:
		case COUNT:
		/* NOPS */
		break;

		case RST_0:
		case RST_8:
		case RST_16:
		case RST_24:
		case RST_32:
		case RST_40:
		case RST_48:
		case RST_56:
		case RETI:
		case RETN:
		case RSMIX:
		case SLP:
		case STMIX:
		case DI:
		case EI:
		case HALT:
		case IM_0:
		case IM_1:
		case IM_2:
		case LD_HL_I:
		case LD_A_I:
		case LD_A_R:
		case LD_A_MB:
		case LD_MB_A:
		case LD_R_A:
		case LD_I_A:
		case LD_I_HL:
		case IN_A_PBC:
		case IN_A_PN:
		case IN_B_PBC:
		case IN_C_PBC:
		case IN_D_PBC:
		case IN_E_PBC:
		case IN_H_PBC:
		case IN_L_PBC:
		case IN0_A_PN:
		case IN0_B_PN:
		case IN0_C_PN:
		case IN0_D_PN:
		case IN0_E_PN:
		case IN0_H_PN:
		case IN0_L_PN:
		case IND:
		case IND2:
		case IND2R:
		case INDM:
		case INDMR:
		case INDR:
		case INDRX:
		case INI:
		case INI2:
		case INI2R:
		case INIM:
		case INIMR:
		case INIR:
		case INIRX:
		case OTD2R:
		case OTDM:
		case OTDMR:
		case OTDR:
		case OTDRX:
		case OTI2R:
		case OTIM:
		case OTIMR:
		case OTIR:
		case OTIRX:
		case OUT_PBC_A:
		case OUT_PBC_B:
		case OUT_PBC_C:
		case OUT_PBC_D:
		case OUT_PBC_E:
		case OUT_PBC_H:
		case OUT_PBC_L:
		case OUT_PN_A:
		case OUT0_PN_A:
		case OUT0_PN_B:
		case OUT0_PN_C:
		case OUT0_PN_D:
		case OUT0_PN_E:
		case OUT0_PN_H:
		case OUT0_PN_L:
		case OUTD:
		case OUTD2:
		case OUTI:
		case OUTI2:
		case TSTIO:
		case UNKNOWN: {
			F.set_flags_unknown();
			set_all_reg_unknown();
		} break;
	}
}
