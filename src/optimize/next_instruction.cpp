#include "known_state.h"

void ez80_known_state::next_instruction(ez80_op_code op_code, uint24_t value) {
	ez80_state state = history.back();
	using enum ez80_op_code;
	using enum ez80_flag_state;
	using enum ez80_reg;
	switch (op_code) {
		case SBC_HL_HL: {
			state.sbc_hl_hl();
		} break;
		case SBC_A_A: {
			state.sbc_a_a();
		} break;
		case SUB_A_A: {
			state.set_carry(false);
			state.set_zero(true);
			state.set_sign(false);
			state.set_overflow(false);
			state.get_reg8(A).set_value(0x00);
		} break;
		case XOR_A_A: {
			state.set_carry(false);
			state.set_zero(true);
			state.set_sign(false);
			state.set_overflow(true);
			state.get_reg8(A).set_value(0x00);
		} break;
		case TST_A_N:
		case TST_A_A:
		case TST_A_B:
		case TST_A_C:
		case TST_A_D:
		case TST_A_E:
		case TST_A_H:
		case TST_A_L:
		case TST_A_PHL: {
			state.flags_unknown();
			state.set_carry(false);
		} break;
		case AND_A_A:
		case OR_A_A: {
			state.flags_unknown();
			state.set_zero(state.get_reg8(A).is_zero());
			state.set_carry(false);
		} break;
		case CP_A_A: {
			state.set_carry(false);
			state.set_zero(true);
			state.set_sign(false);
			state.set_overflow(false);
		} break;
		case EX_AF_AF: {
			state.flags_unknown();
			state.get_reg8(A).set_unknown(true);
		} break;
		case SCF: {
			state.set_carry(true);
		} break;
		case CCF: {
			if (state.isknown_carry()) {
				state.flags.carry ^= 1;
			}
		} break;

		case CP_A_N:
		case CP_A_B:
		case CP_A_C:
		case CP_A_D:
		case CP_A_E:
		case CP_A_H:
		case CP_A_L:
		case CP_A_IXH:
		case CP_A_IXL:
		case CP_A_IYH:
		case CP_A_IYL:
		case CP_A_PHL:
		case CP_A_PIX:
		case CP_A_PIY:
		/* flags destroyed */ {
			state.flags_unknown();
		} break;

		case EX_DE_HL: {
			ez80_reg24_pair hl = state.get_reg24(HL);
			ez80_reg24_pair de = state.get_reg24(DE);
			std::swap(hl, de);
		} break;
		case EX_DE_HL_SIS: {
			ez80_reg24_pair hl = state.get_reg24(HL);
			ez80_reg24_pair de = state.get_reg24(DE);
			hl.clear_upper();
			de.clear_upper();
			std::swap(hl, de);
		} break;
		case EXX: {
			state.get_reg24(HL).set_unknown(true);
			state.get_reg24(DE).set_unknown(true);
			state.get_reg24(BC).set_unknown(true);
		} break;
		case CPL: {
			state.complement();
		} break;
		case NEG: {
			state.negate();
		} break;
		case POP_BC: {
			state.get_reg24(BC).set_destroyed();
		} break;
		case POP_DE: {
			state.get_reg24(DE).set_destroyed();
		} break;
		case POP_HL: {
			state.get_reg24(HL).set_destroyed();
		} break;
		case POP_IX: {
			state.get_reg24(IX).set_destroyed();
		} break;
		case POP_IY: {
			state.get_reg24(IY).set_destroyed();
		} break;
		case EX_SP_HL: {
			state.get_reg24(HL).set_unknown(true);
		} break;
		case EX_SP_IX: {
			state.get_reg24(IX).set_unknown(true);
		} break;
		case EX_SP_IY: {
			state.get_reg24(IY).set_unknown(true);
		} break;
		case INC_BC: {
			state.get_reg24(BC).increment24();
		} break;
		case INC_DE: {
			state.get_reg24(DE).increment24();
		} break;
		case INC_HL: {
			state.get_reg24(HL).increment24();
		} break;
		case INC_SP: {
			state.get_reg24(SP).increment24();
		} break;
		case INC_IX: {
			state.get_reg24(IX).increment24();
		} break;
		case INC_IY: {
			state.get_reg24(IY).increment24();
		} break;
		case INC_BC_SIS: {
			state.get_reg24(BC).increment16();
		} break;
		case INC_DE_SIS: {
			state.get_reg24(DE).increment16();
		} break;
		case INC_HL_SIS: {
			state.get_reg24(HL).increment16();
		} break;
		case INC_SP_SIS: {
			state.get_reg24(SP).increment16();
		} break;
		case INC_IX_SIS: {
			state.get_reg24(IX).increment16();
		} break;
		case INC_IY_SIS: {
			state.get_reg24(IY).increment16();
		} break;
		case DEC_BC: {
			state.get_reg24(BC).decrement24();
		} break;
		case DEC_DE: {
			state.get_reg24(DE).decrement24();
		} break;
		case DEC_HL: {
			state.get_reg24(HL).decrement24();
		} break;
		case DEC_SP: {
			state.get_reg24(SP).decrement24();
		} break;
		case DEC_IX: {
			state.get_reg24(IX).decrement24();
		} break;
		case DEC_IY: {
			state.get_reg24(IY).decrement24();
		} break;
		case DEC_BC_SIS: {
			state.get_reg24(BC).decrement16();
		} break;
		case DEC_DE_SIS: {
			state.get_reg24(DE).decrement16();
		} break;
		case DEC_HL_SIS: {
			state.get_reg24(HL).decrement16();
		} break;
		case DEC_SP_SIS: {
			state.get_reg24(SP).decrement16();
		} break;
		case DEC_IX_SIS: {
			state.get_reg24(IX).decrement16();
		} break;
		case DEC_IY_SIS: {
			state.get_reg24(IY).decrement16();
		} break;
		case MLT_BC: {
			state.get_reg24(BC).multiply();
		} break;
		case MLT_DE: {
			state.get_reg24(DE).multiply();
		} break;
		case MLT_HL: {
			state.get_reg24(HL).multiply();
		} break;
		case MLT_SP: {
			state.get_reg24(SP).multiply();
		} break;
		case LD_A_N: {
			state.set_reg8(A, (uint8_t)value);
		} break;
		case LD_B_N: {
			state.set_reg8(B, (uint8_t)value);
		} break;
		case LD_C_N: {
			state.set_reg8(C, (uint8_t)value);
		} break;
		case LD_D_N: {
			state.set_reg8(D, (uint8_t)value);
		} break;
		case LD_E_N: {
			state.set_reg8(E, (uint8_t)value);
		} break;
		case LD_H_N: {
			state.set_reg8(H, (uint8_t)value);
		} break;
		case LD_L_N: {
			state.set_reg8(L, (uint8_t)value);
		} break;
		case LD_IXL_N: {
			state.set_reg8(IXL, (uint8_t)value);
		} break;
		case LD_IXH_N: {
			state.set_reg8(IXH, (uint8_t)value);
		} break;
		case LD_IYL_N: {
			state.set_reg8(IYL, (uint8_t)value);
		} break;
		case LD_IYH_N: {
			state.set_reg8(IYH, (uint8_t)value);
		} break;
		case LD_A_B: {
			state.load_reg8(A, B);
		} break;
		case LD_A_C: {
			state.load_reg8(A, C);
		} break;
		case LD_A_D: {
			state.load_reg8(A, D);
		} break;
		case LD_A_E: {
			state.load_reg8(A, E);
		} break;
		case LD_A_H: {
			state.load_reg8(A, H);
		} break;
		case LD_A_L: {
			state.load_reg8(A, L);
		} break;
		case LD_A_IXH: {
			state.load_reg8(A, IXH);
		} break;
		case LD_A_IXL: {
			state.load_reg8(A, IXL);
		} break;
		case LD_A_IYH: {
			state.load_reg8(A, IYH);
		} break;
		case LD_A_IYL: {
			state.load_reg8(A, IYL);
		} break;
		case LD_A_ADDR:
		case LD_A_PBC:
		case LD_A_PDE:
		case LD_A_PHL:
		case LD_A_PIX:
		case LD_A_PIY: {
			state.get_reg8(A).set_destroyed();
		} break;
		case LD_B_PHL:
		case LD_B_PIX:
		case LD_B_PIY: {
			state.get_reg8(B).set_destroyed();
		} break;
		case LD_C_PHL:
		case LD_C_PIX:
		case LD_C_PIY: {
			state.get_reg8(C).set_destroyed();
		} break;
		case LD_D_PHL:
		case LD_D_PIX:
		case LD_D_PIY: {
			state.get_reg8(D).set_destroyed();
		} break;
		case LD_E_PHL:
		case LD_E_PIX:
		case LD_E_PIY: {
			state.get_reg8(E).set_destroyed();
		} break;
		case LD_H_PHL:
		case LD_H_PIX:
		case LD_H_PIY: {
			state.get_reg8(H).set_destroyed();
		} break;
		case LD_L_PHL:
		case LD_L_PIX:
		case LD_L_PIY: {
			state.get_reg8(L).set_destroyed();
		} break;
		case LD_B_A: {
			state.load_reg8(B, A);
		} break;
		case LD_B_C: {
			state.load_reg8(B, C);
		} break;
		case LD_B_D: {
			state.load_reg8(B, D);
		} break;
		case LD_B_E: {
			state.load_reg8(B, E);
		} break;
		case LD_B_H: {
			state.load_reg8(B, H);
		} break;
		case LD_B_L: {
			state.load_reg8(B, L);
		} break;
		case LD_B_IXH: {
			state.load_reg8(B, IXH);
		} break;
		case LD_B_IXL: {
			state.load_reg8(B, IXL);
		} break;
		case LD_B_IYH: {
			state.load_reg8(B, IYH);
		} break;
		case LD_B_IYL: {
			state.load_reg8(B, IYL);
		} break;

		case LD_C_A: {
			state.load_reg8(C, A);
		} break;
		case LD_C_B: {
			state.load_reg8(C, B);
		} break;
		case LD_C_D: {
			state.load_reg8(C, D);
		} break;
		case LD_C_E: {
			state.load_reg8(C, E);
		} break;
		case LD_C_H: {
			state.load_reg8(C, H);
		} break;
		case LD_C_L: {
			state.load_reg8(C, L);
		} break;
		case LD_C_IXH: {
			state.load_reg8(C, IXH);
		} break;
		case LD_C_IXL: {
			state.load_reg8(C, IXL);
		} break;
		case LD_C_IYH: {
			state.load_reg8(C, IYH);
		} break;
		case LD_C_IYL: {
			state.load_reg8(C, IYL);
		} break;

		case LD_D_A: {
			state.load_reg8(D, A);
		} break;
		case LD_D_B: {
			state.load_reg8(D, B);
		} break;
		case LD_D_C: {
			state.load_reg8(D, C);
		} break;
		case LD_D_E: {
			state.load_reg8(D, E);
		} break;
		case LD_D_H: {
			state.load_reg8(D, H);
		} break;
		case LD_D_L: {
			state.load_reg8(D, L);
		} break;
		case LD_D_IXH: {
			state.load_reg8(D, IXH);
		} break;
		case LD_D_IXL: {
			state.load_reg8(D, IXL);
		} break;
		case LD_D_IYH: {
			state.load_reg8(D, IYH);
		} break;
		case LD_D_IYL: {
			state.load_reg8(D, IYL);
		} break;

		case LD_E_A: {
			state.load_reg8(E, A);
		} break;
		case LD_E_B: {
			state.load_reg8(E, B);
		} break;
		case LD_E_C: {
			state.load_reg8(E, C);
		} break;
		case LD_E_D: {
			state.load_reg8(E, D);
		} break;
		case LD_E_H: {
			state.load_reg8(E, H);
		} break;
		case LD_E_L: {
			state.load_reg8(E, L);
		} break;
		case LD_E_IXH: {
			state.load_reg8(E, IXH);
		} break;
		case LD_E_IXL: {
			state.load_reg8(E, IXL);
		} break;
		case LD_E_IYH: {
			state.load_reg8(E, IYH);
		} break;
		case LD_E_IYL: {
			state.load_reg8(E, IYL);
		} break;

		case LD_H_A: {
			state.load_reg8(H, A);
		} break;
		case LD_H_B: {
			state.load_reg8(H, B);
		} break;
		case LD_H_C: {
			state.load_reg8(H, C);
		} break;
		case LD_H_D: {
			state.load_reg8(H, D);
		} break;
		case LD_H_E: {
			state.load_reg8(H, E);
		} break;
		case LD_H_L: {
			state.load_reg8(H, L);
		} break;

		case LD_L_A: {
			state.load_reg8(L, A);
		} break;
		case LD_L_B: {
			state.load_reg8(L, B);
		} break;
		case LD_L_C: {
			state.load_reg8(L, C);
		} break;
		case LD_L_D: {
			state.load_reg8(L, D);
		} break;
		case LD_L_E: {
			state.load_reg8(L, E);
		} break;
		case LD_L_H: {
			state.load_reg8(L, H);
		} break;

		case LD_IXH_A: {
			state.load_reg8(IXH, A);
		} break;
		case LD_IXH_B: {
			state.load_reg8(IXH, B);
		} break;
		case LD_IXH_C: {
			state.load_reg8(IXH, C);
		} break;
		case LD_IXH_D: {
			state.load_reg8(IXH, D);
		} break;
		case LD_IXH_E: {
			state.load_reg8(IXH, E);
		} break;
		case LD_IXH_IXL: {
			state.load_reg8(IXH, IXL);
		} break;

		case LD_IXL_A: {
			state.load_reg8(IXL, A);
		} break;
		case LD_IXL_B: {
			state.load_reg8(IXL, B);
		} break;
		case LD_IXL_C: {
			state.load_reg8(IXL, C);
		} break;
		case LD_IXL_D: {
			state.load_reg8(IXL, D);
		} break;
		case LD_IXL_E: {
			state.load_reg8(IXL, E);
		} break;
		case LD_IXL_IXH: {
			state.load_reg8(IXL, IXH);
		} break;

		case LD_IYH_A: {
			state.load_reg8(IYH, A);
		} break;
		case LD_IYH_B: {
			state.load_reg8(IYH, B);
		} break;
		case LD_IYH_C: {
			state.load_reg8(IYH, C);
		} break;
		case LD_IYH_D: {
			state.load_reg8(IYH, D);
		} break;
		case LD_IYH_E: {
			state.load_reg8(IYH, E);
		} break;
		case LD_IYH_IYL: {
			state.load_reg8(IYH, IYL);
		} break;

		case LD_IYL_A: {
			state.load_reg8(IYL, A);
		} break;
		case LD_IYL_B: {
			state.load_reg8(IYL, B);
		} break;
		case LD_IYL_C: {
			state.load_reg8(IYL, C);
		} break;
		case LD_IYL_D: {
			state.load_reg8(IYL, D);
		} break;
		case LD_IYL_E: {
			state.load_reg8(IYL, E);
		} break;
		case LD_IYL_IYH: {
			state.load_reg8(IYL, IYH);
		} break;
		case LD_HL_N:
		case LD_HL_N_SIS: {
			state.set_reg24(HL, value);
		}
		case LD_HL_ADDR:
		case LD_HL_PHL:
		case LD_HL_PIX:
		case LD_HL_PIY: {
			state.get_reg8(HL).set_destroyed();
		} break;
		case LD_DE_N:
		case LD_DE_N_SIS: {
			state.set_reg24(DE, value);
		}
		case LD_DE_ADDR:
		case LD_DE_PHL:
		case LD_DE_PIX:
		case LD_DE_PIY: {
			state.get_reg8(DE).set_destroyed();
		} break;
		case LD_BC_N:
		case LD_BC_N_SIS: {
			state.set_reg24(BC, value);
		}
		case LD_BC_ADDR:
		case LD_BC_PHL:
		case LD_BC_PIX:
		case LD_BC_PIY: {
			state.get_reg8(BC).set_destroyed();
		} break;
		case LD_SP_N:
		case LD_SP_N_SIS: {
			state.set_reg24(SP, value);
		}
		case LD_SP_HL:
		case LD_SP_IX:
		case LD_SP_IY:
		case LD_SP_ADDR: {
			state.get_reg8(SP).set_destroyed();
		} break;
		case LD_IX_N:
		case LD_IX_N_SIS: {
			state.set_reg24(IX, value);
		}
		case LD_IX_ADDR:
		case LD_IX_PHL:
		case LD_IX_PIX:
		case LD_IX_PIY: {
			state.get_reg8(IX).set_destroyed();
		} break;
		case LD_IY_N:
		case LD_IY_N_SIS: {
			state.set_reg24(IY, value);
		}
		case LD_IY_ADDR:
		case LD_IY_PHL:
		case LD_IY_PIX:
		case LD_IY_PIY: {
			state.get_reg8(IY).set_destroyed();
		} break;

		case LD_ADDR_A:
		case LD_PBC_A:
		case LD_PDE_A:
		case LD_PHL_A:
		case LD_PIX_A:
		case LD_PIY_A: {
			state.get_reg8(A).observed = true;
		} break;
		
		case LD_PHL_B:
		case LD_PIX_B:
		case LD_PIY_B: {
			state.get_reg8(B).observed = true;
		} break;
		case LD_PHL_C:
		case LD_PIX_C:
		case LD_PIY_C: {
			state.get_reg8(C).observed = true;
		} break;
		case LD_PHL_D:
		case LD_PIX_D:
		case LD_PIY_D: {
			state.get_reg8(D).observed = true;
		} break;
		case LD_PHL_E:
		case LD_PIX_E:
		case LD_PIY_E: {
			state.get_reg8(E).observed = true;
		} break;
		case LD_PHL_H:
		case LD_PIX_H:
		case LD_PIY_H: {
			state.get_reg8(H).observed = true;
		} break;
		case LD_PHL_L:
		case LD_PIX_L:
		case LD_PIY_L: {
			state.get_reg8(L).observed = true;
		} break;

		case LD_ADDR_HL:
		case LD_PHL_HL:
		case LD_PIX_HL:
		case LD_PIY_HL: {
			state.get_reg24(HL).set_observed(true);
		} break;
		case LD_ADDR_DE:
		case LD_PHL_DE:
		case LD_PIX_DE:
		case LD_PIY_DE: {
			state.get_reg24(DE).set_observed(true);
		} break;
		case LD_ADDR_BC:
		case LD_PHL_BC:
		case LD_PIX_BC:
		case LD_PIY_BC: {
			state.get_reg24(BC).set_observed(true);
		} break;
		case LD_ADDR_IX:
		case LD_PHL_IX:
		case LD_PIX_IX:
		case LD_PIY_IX: {
			state.get_reg24(IX).set_observed(true);
		} break;
		case LD_ADDR_IY:
		case LD_PHL_IY:
		case LD_PIX_IY:
		case LD_PIY_IY: {
			state.get_reg24(IY).set_observed(true);
		} break;
		case LD_ADDR_SP: {
			state.get_reg24(SP).set_observed(true);
		} break;

		case LEA_BC_IX: {
			state.lea_reg24(BC, IX, (int8_t)value);
		} break;
		case LEA_BC_IY: {
			state.lea_reg24(BC, IY, (int8_t)value);
		} break;
		case LEA_DE_IX: {
			state.lea_reg24(DE, IX, (int8_t)value);
		} break;
		case LEA_DE_IY: {
			state.lea_reg24(DE, IY, (int8_t)value);
		} break;
		case LEA_HL_IX: {
			state.lea_reg24(HL, IX, (int8_t)value);
		} break;
		case LEA_HL_IY: {
			state.lea_reg24(HL, IY, (int8_t)value);
		} break;
		case LEA_IX_IX: {
			state.lea_reg24(IX, IX, (int8_t)value);
		} break;
		case LEA_IX_IY: {
			state.lea_reg24(IX, IY, (int8_t)value);
		} break;
		case LEA_IY_IX: {
			state.lea_reg24(IY, IX, (int8_t)value);
		} break;
		case LEA_IY_IY: {
			state.lea_reg24(IY, IY, (int8_t)value);
		} break;

		case LEA_BC_IX_SIS: {
			state.lea_reg16(BC, IX, (int8_t)value);
		} break;
		case LEA_BC_IY_SIS: {
			state.lea_reg16(BC, IY, (int8_t)value);
		} break;
		case LEA_DE_IX_SIS: {
			state.lea_reg16(DE, IX, (int8_t)value);
		} break;
		case LEA_DE_IY_SIS: {
			state.lea_reg16(DE, IY, (int8_t)value);
		} break;
		case LEA_HL_IX_SIS: {
			state.lea_reg16(HL, IX, (int8_t)value);
		} break;
		case LEA_HL_IY_SIS: {
			state.lea_reg16(HL, IY, (int8_t)value);
		} break;
		case LEA_IX_IX_SIS: {
			state.lea_reg16(IX, IX, (int8_t)value);
		} break;
		case LEA_IX_IY_SIS: {
			state.lea_reg16(IX, IY, (int8_t)value);
		} break;
		case LEA_IY_IX_SIS: {
			state.lea_reg16(IY, IX, (int8_t)value);
		} break;
		case LEA_IY_IY_SIS: {
			state.lea_reg16(IY, IY, (int8_t)value);
		} break;

		case SET_0_A: {
			state.get_reg8(A).set_bit(0);
		} break;
		case SET_0_B: {
			state.get_reg8(B).set_bit(0);
		} break;
		case SET_0_C: {
			state.get_reg8(C).set_bit(0);
		} break;
		case SET_0_D: {
			state.get_reg8(D).set_bit(0);
		} break;
		case SET_0_E: {
			state.get_reg8(E).set_bit(0);
		} break;
		case SET_0_H: {
			state.get_reg8(H).set_bit(0);
		} break;
		case SET_0_L: {
			state.get_reg8(L).set_bit(0);
		} break;
		case SET_1_A: {
			state.get_reg8(A).set_bit(1);
		} break;
		case SET_1_B: {
			state.get_reg8(B).set_bit(1);
		} break;
		case SET_1_C: {
			state.get_reg8(C).set_bit(1);
		} break;
		case SET_1_D: {
			state.get_reg8(D).set_bit(1);
		} break;
		case SET_1_E: {
			state.get_reg8(E).set_bit(1);
		} break;
		case SET_1_H: {
			state.get_reg8(H).set_bit(1);
		} break;
		case SET_1_L: {
			state.get_reg8(L).set_bit(1);
		} break;
		case SET_2_A: {
			state.get_reg8(A).set_bit(2);
		} break;
		case SET_2_B: {
			state.get_reg8(B).set_bit(2);
		} break;
		case SET_2_C: {
			state.get_reg8(C).set_bit(2);
		} break;
		case SET_2_D: {
			state.get_reg8(D).set_bit(2);
		} break;
		case SET_2_E: {
			state.get_reg8(E).set_bit(2);
		} break;
		case SET_2_H: {
			state.get_reg8(H).set_bit(2);
		} break;
		case SET_2_L: {
			state.get_reg8(L).set_bit(2);
		} break;
		case SET_3_A: {
			state.get_reg8(A).set_bit(3);
		} break;
		case SET_3_B: {
			state.get_reg8(B).set_bit(3);
		} break;
		case SET_3_C: {
			state.get_reg8(C).set_bit(3);
		} break;
		case SET_3_D: {
			state.get_reg8(D).set_bit(3);
		} break;
		case SET_3_E: {
			state.get_reg8(E).set_bit(3);
		} break;
		case SET_3_H: {
			state.get_reg8(H).set_bit(3);
		} break;
		case SET_3_L: {
			state.get_reg8(L).set_bit(3);
		} break;
		case SET_4_A: {
			state.get_reg8(A).set_bit(4);
		} break;
		case SET_4_B: {
			state.get_reg8(B).set_bit(4);
		} break;
		case SET_4_C: {
			state.get_reg8(C).set_bit(4);
		} break;
		case SET_4_D: {
			state.get_reg8(D).set_bit(4);
		} break;
		case SET_4_E: {
			state.get_reg8(E).set_bit(4);
		} break;
		case SET_4_H: {
			state.get_reg8(H).set_bit(4);
		} break;
		case SET_4_L: {
			state.get_reg8(L).set_bit(4);
		} break;
		case SET_5_A: {
			state.get_reg8(A).set_bit(5);
		} break;
		case SET_5_B: {
			state.get_reg8(B).set_bit(5);
		} break;
		case SET_5_C: {
			state.get_reg8(C).set_bit(5);
		} break;
		case SET_5_D: {
			state.get_reg8(D).set_bit(5);
		} break;
		case SET_5_E: {
			state.get_reg8(E).set_bit(5);
		} break;
		case SET_5_H: {
			state.get_reg8(H).set_bit(5);
		} break;
		case SET_5_L: {
			state.get_reg8(L).set_bit(5);
		} break;
		case SET_6_A: {
			state.get_reg8(A).set_bit(6);
		} break;
		case SET_6_B: {
			state.get_reg8(B).set_bit(6);
		} break;
		case SET_6_C: {
			state.get_reg8(C).set_bit(6);
		} break;
		case SET_6_D: {
			state.get_reg8(D).set_bit(6);
		} break;
		case SET_6_E: {
			state.get_reg8(E).set_bit(6);
		} break;
		case SET_6_H: {
			state.get_reg8(H).set_bit(6);
		} break;
		case SET_6_L: {
			state.get_reg8(L).set_bit(6);
		} break;
		case SET_7_A: {
			state.get_reg8(A).set_bit(7);
		} break;
		case SET_7_B: {
			state.get_reg8(B).set_bit(7);
		} break;
		case SET_7_C: {
			state.get_reg8(C).set_bit(7);
		} break;
		case SET_7_D: {
			state.get_reg8(D).set_bit(7);
		} break;
		case SET_7_E: {
			state.get_reg8(E).set_bit(7);
		} break;
		case SET_7_H: {
			state.get_reg8(H).set_bit(7);
		} break;
		case SET_7_L: {
			state.get_reg8(L).set_bit(7);
		} break;			

		case RES_0_A: {
			state.get_reg8(A).res_bit(0);
		} break;
		case RES_0_B: {
			state.get_reg8(B).res_bit(0);
		} break;
		case RES_0_C: {
			state.get_reg8(C).res_bit(0);
		} break;
		case RES_0_D: {
			state.get_reg8(D).res_bit(0);
		} break;
		case RES_0_E: {
			state.get_reg8(E).res_bit(0);
		} break;
		case RES_0_H: {
			state.get_reg8(H).res_bit(0);
		} break;
		case RES_0_L: {
			state.get_reg8(L).res_bit(0);
		} break;
		case RES_1_A: {
			state.get_reg8(A).res_bit(1);
		} break;
		case RES_1_B: {
			state.get_reg8(B).res_bit(1);
		} break;
		case RES_1_C: {
			state.get_reg8(C).res_bit(1);
		} break;
		case RES_1_D: {
			state.get_reg8(D).res_bit(1);
		} break;
		case RES_1_E: {
			state.get_reg8(E).res_bit(1);
		} break;
		case RES_1_H: {
			state.get_reg8(H).res_bit(1);
		} break;
		case RES_1_L: {
			state.get_reg8(L).res_bit(1);
		} break;
		case RES_2_A: {
			state.get_reg8(A).res_bit(2);
		} break;
		case RES_2_B: {
			state.get_reg8(B).res_bit(2);
		} break;
		case RES_2_C: {
			state.get_reg8(C).res_bit(2);
		} break;
		case RES_2_D: {
			state.get_reg8(D).res_bit(2);
		} break;
		case RES_2_E: {
			state.get_reg8(E).res_bit(2);
		} break;
		case RES_2_H: {
			state.get_reg8(H).res_bit(2);
		} break;
		case RES_2_L: {
			state.get_reg8(L).res_bit(2);
		} break;
		case RES_3_A: {
			state.get_reg8(A).res_bit(3);
		} break;
		case RES_3_B: {
			state.get_reg8(B).res_bit(3);
		} break;
		case RES_3_C: {
			state.get_reg8(C).res_bit(3);
		} break;
		case RES_3_D: {
			state.get_reg8(D).res_bit(3);
		} break;
		case RES_3_E: {
			state.get_reg8(E).res_bit(3);
		} break;
		case RES_3_H: {
			state.get_reg8(H).res_bit(3);
		} break;
		case RES_3_L: {
			state.get_reg8(L).res_bit(3);
		} break;
		case RES_4_A: {
			state.get_reg8(A).res_bit(4);
		} break;
		case RES_4_B: {
			state.get_reg8(B).res_bit(4);
		} break;
		case RES_4_C: {
			state.get_reg8(C).res_bit(4);
		} break;
		case RES_4_D: {
			state.get_reg8(D).res_bit(4);
		} break;
		case RES_4_E: {
			state.get_reg8(E).res_bit(4);
		} break;
		case RES_4_H: {
			state.get_reg8(H).res_bit(4);
		} break;
		case RES_4_L: {
			state.get_reg8(L).res_bit(4);
		} break;
		case RES_5_A: {
			state.get_reg8(A).res_bit(5);
		} break;
		case RES_5_B: {
			state.get_reg8(B).res_bit(5);
		} break;
		case RES_5_C: {
			state.get_reg8(C).res_bit(5);
		} break;
		case RES_5_D: {
			state.get_reg8(D).res_bit(5);
		} break;
		case RES_5_E: {
			state.get_reg8(E).res_bit(5);
		} break;
		case RES_5_H: {
			state.get_reg8(H).res_bit(5);
		} break;
		case RES_5_L: {
			state.get_reg8(L).res_bit(5);
		} break;
		case RES_6_A: {
			state.get_reg8(A).res_bit(6);
		} break;
		case RES_6_B: {
			state.get_reg8(B).res_bit(6);
		} break;
		case RES_6_C: {
			state.get_reg8(C).res_bit(6);
		} break;
		case RES_6_D: {
			state.get_reg8(D).res_bit(6);
		} break;
		case RES_6_E: {
			state.get_reg8(E).res_bit(6);
		} break;
		case RES_6_H: {
			state.get_reg8(H).res_bit(6);
		} break;
		case RES_6_L: {
			state.get_reg8(L).res_bit(6);
		} break;
		case RES_7_A: {
			state.get_reg8(A).res_bit(7);
		} break;
		case RES_7_B: {
			state.get_reg8(B).res_bit(7);
		} break;
		case RES_7_C: {
			state.get_reg8(C).res_bit(7);
		} break;
		case RES_7_D: {
			state.get_reg8(D).res_bit(7);
		} break;
		case RES_7_E: {
			state.get_reg8(E).res_bit(7);
		} break;
		case RES_7_H: {
			state.get_reg8(H).res_bit(7);
		} break;
		case RES_7_L: {
			state.get_reg8(L).res_bit(7);
		} break;

		/* registers modified, but not flags */ {
			state.registers_unknown();
		} break;
		
		case RLA: {
			state.set_carry(state.get_reg8(A).rotate_left(state.get_carry()));
		} break;
		case RRA: {
			state.set_carry(state.get_reg8(A).rotate_right(state.get_carry()));
		} break;
		case RLCA: {
			state.set_carry(state.get_reg8(A).rotate_carry_left());
		} break;
		case RRCA: {
			state.set_carry(state.get_reg8(A).rotate_carry_right());
		} break;
		case ADD_HL_HL: {
			state.set_carry(state.get_reg24(HL).left_shift24(known_false));
		}
		case ADD_HL_HL_SIS: {
			state.set_carry(state.get_reg24(HL).left_shift16(known_false));
		}
		case ADD_IX_IX: {
			state.set_carry(state.get_reg24(IX).left_shift24(known_false));
		}
		case ADD_IX_IX_SIS: {
			state.set_carry(state.get_reg24(IX).left_shift16(known_false));
		}
		case ADD_IY_IY: {
			state.set_carry(state.get_reg24(IY).left_shift24(known_false));
		}
		case ADD_IY_IY_SIS: {
			state.set_carry(state.get_reg24(IY).left_shift16(known_false));
		}
		case ADD_HL_BC:
		case ADD_HL_DE:
		case ADD_HL_SP:
		case ADD_IX_BC:
		case ADD_IX_DE:
		case ADD_IX_SP:
		case ADD_IY_BC:
		case ADD_IY_DE:
		case ADD_IY_SP:
		case ADD_HL_BC_SIS:
		case ADD_HL_DE_SIS:
		case ADD_HL_SP_SIS:
		case ADD_IX_BC_SIS:
		case ADD_IX_DE_SIS:
		case ADD_IX_SP_SIS:
		case ADD_IY_BC_SIS:
		case ADD_IY_DE_SIS:
		case ADD_IY_SP_SIS:
		/* registers and carry modified, but not other flags */ {
			state.registers_unknown();
			state.carry_unknown();
		} break;
		case BIT_0_A: {
			state.test_bit(A, 0);
		} break;
		case BIT_0_B: {
			state.test_bit(B, 0);
		} break;
		case BIT_0_C: {
			state.test_bit(C, 0);
		} break;
		case BIT_0_D: {
			state.test_bit(D, 0);
		} break;
		case BIT_0_E: {
			state.test_bit(E, 0);
		} break;
		case BIT_0_H: {
			state.test_bit(H, 0);
		} break;
		case BIT_0_L: {
			state.test_bit(L, 0);
		} break;
		case BIT_1_A: {
			state.test_bit(A, 1);
		} break;
		case BIT_1_B: {
			state.test_bit(B, 1);
		} break;
		case BIT_1_C: {
			state.test_bit(C, 1);
		} break;
		case BIT_1_D: {
			state.test_bit(D, 1);
		} break;
		case BIT_1_E: {
			state.test_bit(E, 1);
		} break;
		case BIT_1_H: {
			state.test_bit(H, 1);
		} break;
		case BIT_1_L: {
			state.test_bit(L, 1);
		} break;
		case BIT_2_A: {
			state.test_bit(A, 2);
		} break;
		case BIT_2_B: {
			state.test_bit(B, 2);
		} break;
		case BIT_2_C: {
			state.test_bit(C, 2);
		} break;
		case BIT_2_D: {
			state.test_bit(D, 2);
		} break;
		case BIT_2_E: {
			state.test_bit(E, 2);
		} break;
		case BIT_2_H: {
			state.test_bit(H, 2);
		} break;
		case BIT_2_L: {
			state.test_bit(L, 2);
		} break;
		case BIT_3_A: {
			state.test_bit(A, 3);
		} break;
		case BIT_3_B: {
			state.test_bit(B, 3);
		} break;
		case BIT_3_C: {
			state.test_bit(C, 3);
		} break;
		case BIT_3_D: {
			state.test_bit(D, 3);
		} break;
		case BIT_3_E: {
			state.test_bit(E, 3);
		} break;
		case BIT_3_H: {
			state.test_bit(H, 3);
		} break;
		case BIT_3_L: {
			state.test_bit(L, 3);
		} break;
		case BIT_4_A: {
			state.test_bit(A, 4);
		} break;
		case BIT_4_B: {
			state.test_bit(B, 4);
		} break;
		case BIT_4_C: {
			state.test_bit(C, 4);
		} break;
		case BIT_4_D: {
			state.test_bit(D, 4);
		} break;
		case BIT_4_E: {
			state.test_bit(E, 4);
		} break;
		case BIT_4_H: {
			state.test_bit(H, 4);
		} break;
		case BIT_4_L: {
			state.test_bit(L, 4);
		} break;
		case BIT_5_A: {
			state.test_bit(A, 5);
		} break;
		case BIT_5_B: {
			state.test_bit(B, 5);
		} break;
		case BIT_5_C: {
			state.test_bit(C, 5);
		} break;
		case BIT_5_D: {
			state.test_bit(D, 5);
		} break;
		case BIT_5_E: {
			state.test_bit(E, 5);
		} break;
		case BIT_5_H: {
			state.test_bit(H, 5);
		} break;
		case BIT_5_L: {
			state.test_bit(L, 5);
		} break;
		case BIT_6_A: {
			state.test_bit(A, 6);
		} break;
		case BIT_6_B: {
			state.test_bit(B, 6);
		} break;
		case BIT_6_C: {
			state.test_bit(C, 6);
		} break;
		case BIT_6_D: {
			state.test_bit(D, 6);
		} break;
		case BIT_6_E: {
			state.test_bit(E, 6);
		} break;
		case BIT_6_H: {
			state.test_bit(H, 6);
		} break;
		case BIT_6_L: {
			state.test_bit(L, 6);
		} break;
		case BIT_7_A: {
			state.test_bit(A, 7);
		} break;
		case BIT_7_B: {
			state.test_bit(B, 7);
		} break;
		case BIT_7_C: {
			state.test_bit(C, 7);
		} break;
		case BIT_7_D: {
			state.test_bit(D, 7);
		} break;
		case BIT_7_E: {
			state.test_bit(E, 7);
		} break;
		case BIT_7_H: {
			state.test_bit(H, 7);
		} break;
		case BIT_7_L: {
			state.test_bit(L, 7);
		} break;			

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
			state.zero_unknown();
		} break;

		case CPIR:
		case CPDR: {
			state.get_reg24(HL).set_observed(true);
			state.get_reg24(BC).set_observed(true);
			state.get_reg8(A).observed = true;
			state.only_carry_preserved();
		} break;

		case LDIR:
		case LDDR: {
			state.get_reg24(HL).set_observed(true);
			state.get_reg24(DE).set_observed(true);
			state.get_reg24(BC).set_observed(true);
			state.only_carry_preserved();
			state.set_overflow(false);
		} break;

		case CPI: {
			state.get_reg24(HL).set_observed(true);
			state.get_reg24(BC).set_observed(true);
			state.get_reg8(A).observed = true;
			state.get_reg24(HL).increment24();
			state.get_reg24(BC).decrement24();
			state.only_carry_preserved();
		} break;
		case CPD: {
			state.get_reg24(HL).set_observed(true);
			state.get_reg24(BC).set_observed(true);
			state.get_reg8(A).observed = true;
			state.get_reg24(HL).decrement24();
			state.get_reg24(BC).decrement24();
			state.only_carry_preserved();
		} break;
		case LDI: {
			state.get_reg24(HL).set_observed(true);
			state.get_reg24(DE).set_observed(true);
			state.get_reg24(BC).set_observed(true);
			state.get_reg24(HL).increment24();
			state.get_reg24(DE).increment24();
			state.get_reg24(BC).decrement24();
			state.only_carry_preserved();
		} break;
		case LDD: {
			state.get_reg24(HL).set_observed(true);
			state.get_reg24(DE).set_observed(true);
			state.get_reg24(BC).set_observed(true);
			state.get_reg24(HL).decrement24();
			state.get_reg24(DE).decrement24();
			state.get_reg24(BC).decrement24();
			state.only_carry_preserved();
		} break;

		case INC_A: {
			state.get_reg8(A).increment();
		} break;
		case INC_B: {
			state.get_reg8(B).increment();
		} break;
		case INC_C: {
			state.get_reg8(C).increment();
		} break;
		case INC_D: {
			state.get_reg8(D).increment();
		} break;
		case INC_E: {
			state.get_reg8(E).increment();
		} break;
		case INC_H: {
			state.get_reg8(H).increment();
		} break;
		case INC_L: {
			state.get_reg8(L).increment();
		} break;
		case INC_IXH: {
			state.get_reg8(IXH).increment();
		} break;
		case INC_IXL: {
			state.get_reg8(IXL).increment();
		} break;
		case INC_IYH: {
			state.get_reg8(IYH).increment();
		} break;
		case INC_IYL: {
			state.get_reg8(IYL).increment();
		} break;

		case DEC_A: {
			state.get_reg8(A).decrement();
		} break;
		case DEC_B: {
			state.get_reg8(B).decrement();
		} break;
		case DEC_C: {
			state.get_reg8(C).decrement();
		} break;
		case DEC_D: {
			state.get_reg8(D).decrement();
		} break;
		case DEC_E: {
			state.get_reg8(E).decrement();
		} break;
		case DEC_H: {
			state.get_reg8(H).decrement();
		} break;
		case DEC_L: {
			state.get_reg8(L).decrement();
		} break;
		case DEC_IXH: {
			state.get_reg8(IXH).decrement();
		} break;
		case DEC_IXL: {
			state.get_reg8(IXL).decrement();
		} break;
		case DEC_IYH: {
			state.get_reg8(IYH).decrement();
		} break;
		case DEC_IYL: {
			state.get_reg8(IYL).decrement();
		} break;

		case INC_PHL:
		case INC_PIX:
		case INC_PIY:
		case DEC_PHL:
		case DEC_PIX:
		case DEC_PIY:
		/* only carry preserved */ {
			state.only_carry_preserved();
		}

		case AND_A_N:
		case AND_A_B:
		case AND_A_C:
		case AND_A_D:
		case AND_A_E:
		case AND_A_H:
		case AND_A_L:
		case AND_A_IXH:
		case AND_A_IXL:
		case AND_A_IYH:
		case AND_A_IYL:
		case AND_A_PHL:
		case AND_A_PIX:
		case AND_A_PIY:
		case OR_A_N:
		case OR_A_B:
		case OR_A_C:
		case OR_A_D:
		case OR_A_E:
		case OR_A_H:
		case OR_A_L:
		case OR_A_IXH:
		case OR_A_IXL:
		case OR_A_IYH:
		case OR_A_IYL:
		case OR_A_PHL:
		case OR_A_PIX:
		case OR_A_PIY:
		case XOR_A_N:
		case XOR_A_B:
		case XOR_A_C:
		case XOR_A_D:
		case XOR_A_E:
		case XOR_A_H:
		case XOR_A_L:
		case XOR_A_IXH:
		case XOR_A_IXL:
		case XOR_A_IYH:
		case XOR_A_IYL:
		case XOR_A_PHL:
		case XOR_A_PIX:
		case XOR_A_PIY:
		/* carry is cleared */ {
			state.get_reg8(A).set_unknown();
			state.set_carry(false);
		}
		/* Otherwise */
		default: {
			state.state_unknown();
		} break;
		case NOP:
		case PEA_IX:
		case PEA_IY:
		case PUSH_AF:
		case PUSH_BC:
		case PUSH_DE:
		case PUSH_HL:
		case PUSH_IX:
		case PUSH_IY:
		case LD_A_A:
		case LD_H_H:
		case LD_L_L:
		case LD_IXL_IXL:
		case LD_IXH_IXH:
		case LD_IYL_IYL:
		case LD_IYH_IYH:
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
	}
	history.push_back(state);
}
