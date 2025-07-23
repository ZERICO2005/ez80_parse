#include "../ez80_instruction.h"
#include "../ez80_type.h"
#include "../ez80_instruction_info.h"
#include "../text.h"

#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <cmath>

#include "../ez80_asm.h"
#include "../ez80_parse.h"
#include "../common_std.h"

inline string symbol_str(const string& str) {
	if (str.empty()) {
		return "unknown";
	}
	return str;
}

inline string imm8_offset_str(const ez80_instruction& x) {
	if (!x.known_value) {
		return " + " + symbol_str(x.symbol);
	}
	if (x.offset == 0) {
		return "";
	}
	char buf[20];
	snprintf(buf, sizeof(buf), " %c %d", (x.offset < 0) ? '-' : '+', std::abs(x.offset));
	return buf;
}

inline string imm8_jump_relative_str(const ez80_instruction& x) {
	if (!x.known_value) {
		return symbol_str(x.symbol);
	}
	if (x.offset == 0) {
		return "$";
	}
	char buf[20];
	snprintf(buf, sizeof(buf), "$ %c %d", (x.offset < 0) ? '-' : '+', std::abs(x.offset));
	return buf;
}

inline string imm8_value_str(const ez80_instruction& x) {
	if (!x.known_value) {
		return symbol_str(x.symbol);
	}
	char buf[20];
	snprintf(buf, sizeof(buf), "$%02x", (uint32_t)x.value);
	return buf;
}

inline string imm16_value_str(const ez80_instruction& x) {
	if (!x.known_value) {
		return symbol_str(x.symbol);
	}
	char buf[20];
	snprintf(buf, sizeof(buf), "$%04x", (uint32_t)x.value);
	return buf;
}

inline string imm24_value_str(const ez80_instruction& x) {
	if (!x.known_value) {
		return symbol_str(x.symbol);
	}
	char buf[20];
	snprintf(buf, sizeof(buf), "$%06x", (uint32_t)x.value);
	return buf;
}

string instruction_to_string(ez80_instruction x) {
	using enum ez80_op_code;
	switch (x.op_code) {
		case NOP: return "nop";
		case SCF: return "scf";
		case CCF: return "ccf";
		case RLA: return "rla";
		case RLCA: return "rlca";
		case RRA: return "rra";
		case RRCA: return "rrca";
		case CPL: return "cpl";
	
		case NEG: return "neg";
		case DAA: return "daa";
		case RLD: return "rld";
		case RRD: return "rrd";
	
		case EX_AF_AF: return "ex\taf, af\'";
		case EX_DE_HL: return "ex\tde, hl";
		case EX_DE_HL_SIS: return "ex.sis\tde, hl";
		case EXX: return "exx";
	
		/* Processor Control */
	
		case RSMIX: return "rsmix";
		case SLP: return "slp";
		case STMIX: return "stmix";
		case DI: return "di";
		case EI: return "ei";
		case HALT: return "halt";
		case IM_0: return "im_0";
		case IM_1: return "im_1";
		case IM_2: return "im_2";
	
		/* Special Registers */
	
		case LD_HL_I: return "ld\thl, i";
		case LD_A_I: return "ld\ta, i";
		case LD_A_R: return "ld\ta, r";
		case LD_A_MB: return "ld\ta, mb";
	
		case LD_MB_A: return "ld\tmb, a";
		case LD_R_A: return "ld\tr, a";
		case LD_I_A: return "ld\ti, a";
		case LD_I_HL: return "ld\ti, hl";
	
		/* Jump Register */
	
		case JP_HL: return "jp\t(hl)";
		case JP_IX: return "jp\t(ix)";
		case JP_IY: return "jp\t(iy)";
	
		/* Jump Absolute */
	
		case JP: return "jp\t" + imm24_value_str(x);
		case JP_C: return "jp\tc, " + imm24_value_str(x);
		case JP_NC: return "jp\tnc, " + imm24_value_str(x);
		case JP_Z: return "jp\tz, " + imm24_value_str(x);
		case JP_NZ: return "jp\tnz, " + imm24_value_str(x);
		case JP_P: return "jp\tp, " + imm24_value_str(x);
		case JP_M: return "jp\tm, " + imm24_value_str(x);
		case JP_PO: return "jp\tpo, " + imm24_value_str(x);
		case JP_PE: return "jp\tpe, " + imm24_value_str(x);
	
		/* Jump Relative */
	
		case JR: return "jr\t" + imm8_jump_relative_str(x);
		case JR_C: return "jr\tc, " + imm8_jump_relative_str(x);
		case JR_NC: return "jr\tnc, " + imm8_jump_relative_str(x);
		case JR_Z: return "jr\tz, " + imm8_jump_relative_str(x);
		case JR_NZ: return "jr\tnz, " + imm8_jump_relative_str(x);
		case DJNZ: return "djnz\t" + imm8_jump_relative_str(x);
	
		/* Call */
	
		case CALL: return "call\t" + imm24_value_str(x);
		case CALL_C: return "call\tc, " + imm24_value_str(x);
		case CALL_NC: return "call\tnc, " + imm24_value_str(x);
		case CALL_Z: return "call\tz, " + imm24_value_str(x);
		case CALL_NZ: return "call\tnz, " + imm24_value_str(x);
		case CALL_P: return "call\tp, " + imm24_value_str(x);
		case CALL_M: return "call\tm, " + imm24_value_str(x);
		case CALL_PO: return "call\tpo, " + imm24_value_str(x);
		case CALL_PE: return "call\tpe, " + imm24_value_str(x);
	
		case RST_0: return "rst\t(0)";
		case RST_8: return "rst\t(8)";
		case RST_16: return "rst\t(16)";
		case RST_24: return "rst\t(24)";
		case RST_32: return "rst\t(32)";
		case RST_40: return "rst\t(40)";
		case RST_48: return "rst\t(48)";
		case RST_56: return "rst\t(56)";
	
		/* Return */
	
		case RET: return "ret";
		case RET_C: return "ret\tc";
		case RET_NC: return "ret\tnc";
		case RET_Z: return "ret\tz";
		case RET_NZ: return "ret\tnz";
		case RET_P: return "ret\tp";
		case RET_M: return "ret\tm";
		case RET_PO: return "ret\tpo";
		case RET_PE: return "ret\tpe";
	
		case RETI: return "reti";
		case RETN: return "retn";
	
		/* Stack operations */
	
		case PEA_IX: return "pea\tix";
		case PEA_IY: return "pea\tiy";
	
		case POP_AF: return "pop\taf";
		case POP_BC: return "pop\tbc";
		case POP_DE: return "pop\tde";
		case POP_HL: return "pop\thl";
		case POP_IX: return "pop\tix";
		case POP_IY: return "pop\tiy";
	
		case PUSH_AF: return "push\taf";
		case PUSH_BC: return "push\tbc";
		case PUSH_DE: return "push\tde";
		case PUSH_HL: return "push\thl";
		case PUSH_IX: return "push\tix";
		case PUSH_IY: return "push\tiy";
	
		case EX_SP_HL: return "ex\t(sp), hl";
		case EX_SP_IX: return "ex\t(sp), ix";
		case EX_SP_IY: return "ex\t(sp), iy";
	
		/* Block Transfer and Compare */
	
		case CPD: return "cpd";
		case CPDR: return "cpdr";
		case CPI: return "cpi";
		case CPIR: return "cpir";
	
		case LDD: return "ldd";
		case LDDR: return "lddr";
		case LDI: return "ldi";
		case LDIR: return "ldir";
	
		/* Increment */
	
		case INC_A: return "inc\ta";
		case INC_B: return "inc\tb";
		case INC_C: return "inc\tc";
		case INC_D: return "inc\td";
		case INC_E: return "inc\te";
		case INC_H: return "inc\th";
		case INC_L: return "inc\tl";
		case INC_IXH: return "inc\tixh";
		case INC_IXL: return "inc\tixl";
		case INC_IYH: return "inc\tiyh";
		case INC_IYL: return "inc\tiyl";
		case INC_PHL: return "inc\t(hl)";
		case INC_PIX: return "inc\t(ix" + imm8_offset_str(x) + ")";
		case INC_PIY: return "inc\t(iy" + imm8_offset_str(x) + ")";
	
		case INC_BC: return "inc\tbc";
		case INC_DE: return "inc\tde";
		case INC_HL: return "inc\thl";
		case INC_SP: return "inc\tsp";
		case INC_IX: return "inc\tix";
		case INC_IY: return "inc\tiy";
	
		case INC_BC_SIS: return "inc.sis\tbc";
		case INC_DE_SIS: return "inc.sis\tde";
		case INC_HL_SIS: return "inc.sis\thl";
		case INC_SP_SIS: return "inc.sis\tsp";
		case INC_IX_SIS: return "inc.sis\tix";
		case INC_IY_SIS: return "inc.sis\tiy";
	
		/* Decrement */
	
		case DEC_A: return "dec\ta";
		case DEC_B: return "dec\tb";
		case DEC_C: return "dec\tc";
		case DEC_D: return "dec\td";
		case DEC_E: return "dec\te";
		case DEC_H: return "dec\th";
		case DEC_L: return "dec\tl";
		case DEC_IXH: return "dec\tixh";
		case DEC_IXL: return "dec\tixl";
		case DEC_IYH: return "dec\tiyh";
		case DEC_IYL: return "dec\tiyl";
		case DEC_PHL: return "dec\t(hl)";
		case DEC_PIX: return "dec\t(ix" + imm8_offset_str(x) + ")";
		case DEC_PIY: return "dec\t(iy" + imm8_offset_str(x) + ")";
	
		case DEC_BC: return "dec\tbc";
		case DEC_DE: return "dec\tde";
		case DEC_HL: return "dec\thl";
		case DEC_SP: return "dec\tsp";
		case DEC_IX: return "dec\tix";
		case DEC_IY: return "dec\tiy";
	
		case DEC_BC_SIS: return "dec.sis\tbc";
		case DEC_DE_SIS: return "dec.sis\tde";
		case DEC_HL_SIS: return "dec.sis\thl";
		case DEC_SP_SIS: return "dec.sis\tsp";
		case DEC_IX_SIS: return "dec.sis\tix";
		case DEC_IY_SIS: return "dec.sis\tiy";
	
		/* Aritmetic Reg24 */
	
		case ADD_HL_BC: return "add\thl, bc";
		case ADD_HL_DE: return "add\thl, de";
		case ADD_HL_HL: return "add\thl, hl";
		case ADD_HL_SP: return "add\thl, sp";
	
		case ADD_HL_BC_SIS: return "add.sis\thl, bc";
		case ADD_HL_DE_SIS: return "add.sis\thl, de";
		case ADD_HL_HL_SIS: return "add.sis\thl, hl";
		case ADD_HL_SP_SIS: return "add.sis\thl, sp";
	
		case ADD_IX_BC: return "add\tix, bc";
		case ADD_IX_DE: return "add\tix, de";
		case ADD_IX_IX: return "add\tix, ix";
		case ADD_IX_SP: return "add\tix, sp";
		case ADD_IY_BC: return "add\tiy, bc";
		case ADD_IY_DE: return "add\tiy, de";
		case ADD_IY_IY: return "add\tiy, iy";
		case ADD_IY_SP: return "add\tiy, sp";
	
		case ADD_IX_BC_SIS: return "add.sis\tix, bc";
		case ADD_IX_DE_SIS: return "add.sis\tix, de";
		case ADD_IX_IX_SIS: return "add.sis\tix, ix";
		case ADD_IX_SP_SIS: return "add.sis\tix, sp";
		case ADD_IY_BC_SIS: return "add.sis\tiy, bc";
		case ADD_IY_DE_SIS: return "add.sis\tiy, de";
		case ADD_IY_IY_SIS: return "add.sis\tiy, iy";
		case ADD_IY_SP_SIS: return "add.sis\tiy, sp";
	
		case ADC_HL_BC: return "adc\thl, bc";
		case ADC_HL_DE: return "adc\thl, de";
		case ADC_HL_HL: return "adc\thl, hl";
		case ADC_HL_SP: return "adc\thl, sp";
	
		case ADC_HL_BC_SIS: return "adc.sis\thl, bc";
		case ADC_HL_DE_SIS: return "adc.sis\thl, de";
		case ADC_HL_HL_SIS: return "adc.sis\thl, hl";
		case ADC_HL_SP_SIS: return "adc.sis\thl, sp";
	
		case SBC_HL_BC: return "sbc\thl, bc";
		case SBC_HL_DE: return "sbc\thl, de";
		case SBC_HL_HL: return "sbc\thl, hl";
		case SBC_HL_SP: return "sbc\thl, sp";
	
		case SBC_HL_BC_SIS: return "sbc.sis\thl, bc";
		case SBC_HL_DE_SIS: return "sbc.sis\thl, de";
		case SBC_HL_HL_SIS: return "sbc.sis\thl, hl";
		case SBC_HL_SP_SIS: return "sbc.sis\thl, sp";
	
		case MLT_BC: return "mlt\tbc";
		case MLT_DE: return "mlt\tde";
		case MLT_HL: return "mlt\thl";
		case MLT_SP: return "mlt\tsp";
	
		/* Accumulate Arithmetic */
	
		case ADD_A_N: return "add\ta, " + imm8_value_str(x);
		case ADD_A_A: return "add\ta, a";
		case ADD_A_B: return "add\ta, b";
		case ADD_A_C: return "add\ta, c";
		case ADD_A_D: return "add\ta, d";
		case ADD_A_E: return "add\ta, e";
		case ADD_A_H: return "add\ta, h";
		case ADD_A_L: return "add\ta, l";
		case ADD_A_IXH: return "add\ta, ixh";
		case ADD_A_IXL: return "add\ta, ixl";
		case ADD_A_IYH: return "add\ta, iyh";
		case ADD_A_IYL: return "add\ta, iyl";
		case ADD_A_PHL: return "add\ta, (hl)";
		case ADD_A_PIX: return "add\ta, (ix" + imm8_offset_str(x) + ")";
		case ADD_A_PIY: return "add\ta, (iy" + imm8_offset_str(x) + ")";
	
		case ADC_A_N: return "adc\ta, " + imm8_value_str(x);
		case ADC_A_A: return "adc\ta, a";
		case ADC_A_B: return "adc\ta, b";
		case ADC_A_C: return "adc\ta, c";
		case ADC_A_D: return "adc\ta, d";
		case ADC_A_E: return "adc\ta, e";
		case ADC_A_H: return "adc\ta, h";
		case ADC_A_L: return "adc\ta, l";
		case ADC_A_IXH: return "adc\ta, ixh";
		case ADC_A_IXL: return "adc\ta, ixl";
		case ADC_A_IYH: return "adc\ta, iyh";
		case ADC_A_IYL: return "adc\ta, iyl";
		case ADC_A_PHL: return "adc\ta, (hl)";
		case ADC_A_PIX: return "adc\ta, (ix" + imm8_offset_str(x) + ")";
		case ADC_A_PIY: return "adc\ta, (iy" + imm8_offset_str(x) + ")";
	
		case SUB_A_N: return "sub\ta, " + imm8_value_str(x);
		case SUB_A_A: return "sub\ta, a";
		case SUB_A_B: return "sub\ta, b";
		case SUB_A_C: return "sub\ta, c";
		case SUB_A_D: return "sub\ta, d";
		case SUB_A_E: return "sub\ta, e";
		case SUB_A_H: return "sub\ta, h";
		case SUB_A_L: return "sub\ta, l";
		case SUB_A_IXH: return "sub\ta, ixh";
		case SUB_A_IXL: return "sub\ta, ixl";
		case SUB_A_IYH: return "sub\ta, iyh";
		case SUB_A_IYL: return "sub\ta, iyl";
		case SUB_A_PHL: return "sub\ta, (hl)";
		case SUB_A_PIX: return "sub\ta, (ix" + imm8_offset_str(x) + ")";
		case SUB_A_PIY: return "sub\ta, (iy" + imm8_offset_str(x) + ")";
	
		case SBC_A_N: return "sbc\ta, " + imm8_value_str(x);
		case SBC_A_A: return "sbc\ta, a";
		case SBC_A_B: return "sbc\ta, b";
		case SBC_A_C: return "sbc\ta, c";
		case SBC_A_D: return "sbc\ta, d";
		case SBC_A_E: return "sbc\ta, e";
		case SBC_A_H: return "sbc\ta, h";
		case SBC_A_L: return "sbc\ta, l";
		case SBC_A_IXH: return "sbc\ta, ixh";
		case SBC_A_IXL: return "sbc\ta, ixl";
		case SBC_A_IYH: return "sbc\ta, iyh";
		case SBC_A_IYL: return "sbc\ta, iyl";
		case SBC_A_PHL: return "sbc\ta, (hl)";
		case SBC_A_PIX: return "sbc\ta, (ix" + imm8_offset_str(x) + ")";
		case SBC_A_PIY: return "sbc\ta, (iy" + imm8_offset_str(x) + ")";
	
		case CP_A_N: return "cp\ta, " + imm8_value_str(x);
		case CP_A_A: return "cp\ta, a";
		case CP_A_B: return "cp\ta, b";
		case CP_A_C: return "cp\ta, c";
		case CP_A_D: return "cp\ta, d";
		case CP_A_E: return "cp\ta, e";
		case CP_A_H: return "cp\ta, h";
		case CP_A_L: return "cp\ta, l";
		case CP_A_IXH: return "cp\ta, ixh";
		case CP_A_IXL: return "cp\ta, ixl";
		case CP_A_IYH: return "cp\ta, iyh";
		case CP_A_IYL: return "cp\ta, iyl";
		case CP_A_PHL: return "cp\ta, (hl)";
		case CP_A_PIX: return "cp\ta, (ix" + imm8_offset_str(x) + ")";
		case CP_A_PIY: return "cp\ta, (iy" + imm8_offset_str(x) + ")";
	
		/* Accumulate Logical */
	
		case AND_A_N: return "and\ta, " + imm8_value_str(x);
		case AND_A_A: return "and\ta, a";
		case AND_A_B: return "and\ta, b";
		case AND_A_C: return "and\ta, c";
		case AND_A_D: return "and\ta, d";
		case AND_A_E: return "and\ta, e";
		case AND_A_H: return "and\ta, h";
		case AND_A_L: return "and\ta, l";
		case AND_A_IXH: return "and\ta, ixh";
		case AND_A_IXL: return "and\ta, ixl";
		case AND_A_IYH: return "and\ta, iyh";
		case AND_A_IYL: return "and\ta, iyl";
		case AND_A_PHL: return "and\ta, (hl)";
		case AND_A_PIX: return "and\ta, (ix" + imm8_offset_str(x) + ")";
		case AND_A_PIY: return "and\ta, (iy" + imm8_offset_str(x) + ")";
	
		case OR_A_N: return "or\ta, " + imm8_value_str(x);
		case OR_A_A: return "or\ta, a";
		case OR_A_B: return "or\ta, b";
		case OR_A_C: return "or\ta, c";
		case OR_A_D: return "or\ta, d";
		case OR_A_E: return "or\ta, e";
		case OR_A_H: return "or\ta, h";
		case OR_A_L: return "or\ta, l";
		case OR_A_IXH: return "or\ta, ixh";
		case OR_A_IXL: return "or\ta, ixl";
		case OR_A_IYH: return "or\ta, iyh";
		case OR_A_IYL: return "or\ta, iyl";
		case OR_A_PHL: return "or\ta, (hl)";
		case OR_A_PIX: return "or\ta, (ix" + imm8_offset_str(x) + ")";
		case OR_A_PIY: return "or\ta, (iy" + imm8_offset_str(x) + ")";
	
		case XOR_A_N: return "xor\ta, " + imm8_value_str(x);
		case XOR_A_A: return "xor\ta, a";
		case XOR_A_B: return "xor\ta, b";
		case XOR_A_C: return "xor\ta, c";
		case XOR_A_D: return "xor\ta, d";
		case XOR_A_E: return "xor\ta, e";
		case XOR_A_H: return "xor\ta, h";
		case XOR_A_L: return "xor\ta, l";
		case XOR_A_IXH: return "xor\ta, ixh";
		case XOR_A_IXL: return "xor\ta, ixl";
		case XOR_A_IYH: return "xor\ta, iyh";
		case XOR_A_IYL: return "xor\ta, iyl";
		case XOR_A_PHL: return "xor\ta, (hl)";
		case XOR_A_PIX: return "xor\ta, (ix" + imm8_offset_str(x) + ")";
		case XOR_A_PIY: return "xor\ta, (iy" + imm8_offset_str(x) + ")";
	
		/* Test A */
	
		case TST_A_N: return "tst\ta, " + imm8_value_str(x);
		case TST_A_A: return "tst\ta, a";
		case TST_A_B: return "tst\ta, b";
		case TST_A_C: return "tst\ta, c";
		case TST_A_D: return "tst\ta, d";
		case TST_A_E: return "tst\ta, e";
		case TST_A_H: return "tst\ta, h";
		case TST_A_L: return "tst\ta, l";
		case TST_A_PHL: return "tst\ta, (hl)";
	
		/* Shift Reg8 */
	
		case RL_A: return "rl\ta";
		case RL_B: return "rl\tb";
		case RL_C: return "rl\tc";
		case RL_D: return "rl\td";
		case RL_E: return "rl\te";
		case RL_H: return "rl\th";
		case RL_L: return "rl\tl";
		case RL_PHL: return "rl\t(hl)";
		case RL_PIX: return "rl\t(ix" + imm8_offset_str(x) + ")";
		case RL_PIY: return "rl\t(iy" + imm8_offset_str(x) + ")";
	
		case RLC_A: return "rlc\ta";
		case RLC_B: return "rlc\tb";
		case RLC_C: return "rlc\tc";
		case RLC_D: return "rlc\td";
		case RLC_E: return "rlc\te";
		case RLC_H: return "rlc\th";
		case RLC_L: return "rlc\tl";
		case RLC_PHL: return "rlc\t(hl)";
		case RLC_PIX: return "rlc\t(ix" + imm8_offset_str(x) + ")";
		case RLC_PIY: return "rlc\t(iy" + imm8_offset_str(x) + ")";
	
		case RR_A: return "rr\ta";
		case RR_B: return "rr\tb";
		case RR_C: return "rr\tc";
		case RR_D: return "rr\td";
		case RR_E: return "rr\te";
		case RR_H: return "rr\th";
		case RR_L: return "rr\tl";
		case RR_PHL: return "rr\t(hl)";
		case RR_PIX: return "rr\t(ix" + imm8_offset_str(x) + ")";
		case RR_PIY: return "rr\t(iy" + imm8_offset_str(x) + ")";
	
		case RRC_A: return "rrc\ta";
		case RRC_B: return "rrc\tb";
		case RRC_C: return "rrc\tc";
		case RRC_D: return "rrc\td";
		case RRC_E: return "rrc\te";
		case RRC_H: return "rrc\th";
		case RRC_L: return "rrc\tl";
		case RRC_PHL: return "rrc\t(hl)";
		case RRC_PIX: return "rrc\t(ix" + imm8_offset_str(x) + ")";
		case RRC_PIY: return "rrc\t(iy" + imm8_offset_str(x) + ")";
	
		case SLA_A: return "sla\ta";
		case SLA_B: return "sla\tb";
		case SLA_C: return "sla\tc";
		case SLA_D: return "sla\td";
		case SLA_E: return "sla\te";
		case SLA_H: return "sla\th";
		case SLA_L: return "sla\tl";
		case SLA_PHL: return "sla\t(hl)";
		case SLA_PIX: return "sla\t(ix" + imm8_offset_str(x) + ")";
		case SLA_PIY: return "sla\t(iy" + imm8_offset_str(x) + ")";
	
		case SRA_A: return "sra\ta";
		case SRA_B: return "sra\tb";
		case SRA_C: return "sra\tc";
		case SRA_D: return "sra\td";
		case SRA_E: return "sra\te";
		case SRA_H: return "sra\th";
		case SRA_L: return "sra\tl";
		case SRA_PHL: return "sra\t(hl)";
		case SRA_PIX: return "sra\t(ix" + imm8_offset_str(x) + ")";
		case SRA_PIY: return "sra\t(iy" + imm8_offset_str(x) + ")";
		
		case SRL_A: return "srl\ta";
		case SRL_B: return "srl\tb";
		case SRL_C: return "srl\tc";
		case SRL_D: return "srl\td";
		case SRL_E: return "srl\te";
		case SRL_H: return "srl\th";
		case SRL_L: return "srl\tl";
		case SRL_PHL: return "srl\t(hl)";
		case SRL_PIX: return "srl\t(ix" + imm8_offset_str(x) + ")";
		case SRL_PIY: return "srl\t(iy" + imm8_offset_str(x) + ")";
	
		/* load register A */
	
		case LD_A_N: return "ld\ta, " + imm8_value_str(x);
		case LD_A_A: return "ld\ta, a";
		case LD_A_B: return "ld\ta, b";
		case LD_A_C: return "ld\ta, c";
		case LD_A_D: return "ld\ta, d";
		case LD_A_E: return "ld\ta, e";
		case LD_A_H: return "ld\ta, h";
		case LD_A_L: return "ld\ta, l";
		case LD_A_IXH: return "ld\ta, ixh";
		case LD_A_IXL: return "ld\ta, ixl";
		case LD_A_IYH: return "ld\ta, iyh";
		case LD_A_IYL: return "ld\ta, iyl";
	
		case LD_A_ADDR: return "ld\ta, " + imm24_value_str(x);
		case LD_A_PBC: return "ld\ta, (bc)";
		case LD_A_PDE: return "ld\ta, (de)";
		case LD_A_PHL: return "ld\ta, (hl)";
		case LD_A_PIX: return "ld\ta, (ix" + imm8_offset_str(x) + ")";
		case LD_A_PIY: return "ld\ta, (iy" + imm8_offset_str(x) + ")";
	
		/* load B C D E */
	
		case LD_B_N: return "ld\tb, " + imm8_value_str(x);
		case LD_B_A: return "ld\tb, a";
		case LD_B_C: return "ld\tb, c";
		case LD_B_D: return "ld\tb, d";
		case LD_B_E: return "ld\tb, e";
		case LD_B_H: return "ld\tb, h";
		case LD_B_L: return "ld\tb, l";
		case LD_B_IXH: return "ld\tb, ixh";
		case LD_B_IXL: return "ld\tb, ixl";
		case LD_B_IYH: return "ld\tb, iyh";
		case LD_B_IYL: return "ld\tb, iyl";
		case LD_B_PHL: return "ld\tb, (hl)";
		case LD_B_PIX: return "ld\tb, (ix" + imm8_offset_str(x) + ")";
		case LD_B_PIY: return "ld\tb, (iy" + imm8_offset_str(x) + ")";
	
		case LD_C_N: return "ld\tc, " + imm8_value_str(x);
		case LD_C_A: return "ld\tc, a";
		case LD_C_B: return "ld\tc, b";
		case LD_C_D: return "ld\tc, d";
		case LD_C_E: return "ld\tc, e";
		case LD_C_H: return "ld\tc, h";
		case LD_C_L: return "ld\tc, l";
		case LD_C_IXH: return "ld\tc, ixh";
		case LD_C_IXL: return "ld\tc, ixl";
		case LD_C_IYH: return "ld\tc, iyh";
		case LD_C_IYL: return "ld\tc, iyl";
		case LD_C_PHL: return "ld\tc, (hl)";
		case LD_C_PIX: return "ld\tc, (ix" + imm8_offset_str(x) + ")";
		case LD_C_PIY: return "ld\tc, (iy" + imm8_offset_str(x) + ")";
	
		case LD_D_N: return "ld\td, " + imm8_value_str(x);
		case LD_D_A: return "ld\td, a";
		case LD_D_B: return "ld\td, b";
		case LD_D_C: return "ld\td, c";
		case LD_D_E: return "ld\td, e";
		case LD_D_H: return "ld\td, h";
		case LD_D_L: return "ld\td, l";
		case LD_D_IXH: return "ld\td, ixh";
		case LD_D_IXL: return "ld\td, ixl";
		case LD_D_IYH: return "ld\td, iyh";
		case LD_D_IYL: return "ld\td, iyl";
		case LD_D_PHL: return "ld\td, (hl)";
		case LD_D_PIX: return "ld\td, (ix" + imm8_offset_str(x) + ")";
		case LD_D_PIY: return "ld\td, (iy" + imm8_offset_str(x) + ")";
	
		case LD_E_N: return "ld\te, " + imm8_value_str(x);
		case LD_E_A: return "ld\te, a";
		case LD_E_B: return "ld\te, b";
		case LD_E_C: return "ld\te, c";
		case LD_E_D: return "ld\te, d";
		case LD_E_H: return "ld\te, h";
		case LD_E_L: return "ld\te, l";
		case LD_E_IXH: return "ld\te, ixh";
		case LD_E_IXL: return "ld\te, ixl";
		case LD_E_IYH: return "ld\te, iyh";
		case LD_E_IYL: return "ld\te, iyl";
		case LD_E_PHL: return "ld\te, (hl)";
		case LD_E_PIX: return "ld\te, (ix" + imm8_offset_str(x) + ")";
		case LD_E_PIY: return "ld\te, (iy" + imm8_offset_str(x) + ")";
	
		/* load H L */
	
		case LD_H_N: return "ld\th, " + imm8_value_str(x);
		case LD_H_A: return "ld\th, a";
		case LD_H_B: return "ld\th, b";
		case LD_H_C: return "ld\th, c";
		case LD_H_D: return "ld\th, d";
		case LD_H_E: return "ld\th, e";
		case LD_H_H: return "ld\th, h";
		case LD_H_L: return "ld\th, l";
		case LD_H_PHL: return "ld\th, (hl)";
		case LD_H_PIX: return "ld\th, (ix" + imm8_offset_str(x) + ")";
		case LD_H_PIY: return "ld\th, (iy" + imm8_offset_str(x) + ")";
	
		case LD_L_N: return "ld\tl, " + imm8_value_str(x);
		case LD_L_A: return "ld\tl, a";
		case LD_L_B: return "ld\tl, b";
		case LD_L_C: return "ld\tl, c";
		case LD_L_D: return "ld\tl, d";
		case LD_L_E: return "ld\tl, e";
		case LD_L_H: return "ld\tl, h";
		case LD_L_L: return "ld\tl, l";
		case LD_L_PHL: return "ld\tl, (hl)";
		case LD_L_PIX: return "ld\tl, (ix" + imm8_offset_str(x) + ")";
		case LD_L_PIY: return "ld\tl, (iy" + imm8_offset_str(x) + ")";
	
		/* load IXL IXH IYL IYH */
	
		case LD_IXH_N: return "ld\tixh, " + imm8_value_str(x);
		case LD_IXH_A: return "ld\tixh, a";
		case LD_IXH_B: return "ld\tixh, b";
		case LD_IXH_C: return "ld\tixh, c";
		case LD_IXH_D: return "ld\tixh, d";
		case LD_IXH_E: return "ld\tixh, e";
		case LD_IXH_IXH: return "ld\tixh, ixh";
		case LD_IXH_IXL: return "ld\tixh, ixl";
	
		case LD_IXL_N: return "ld\tixl, " + imm8_value_str(x);
		case LD_IXL_A: return "ld\tixl, a";
		case LD_IXL_B: return "ld\tixl, b";
		case LD_IXL_C: return "ld\tixl, c";
		case LD_IXL_D: return "ld\tixl, d";
		case LD_IXL_E: return "ld\tixl, e";
		case LD_IXL_IXH: return "ld\tixl, ixh";
		case LD_IXL_IXL: return "ld\tixl, ixl";
	
		case LD_IYH_N: return "ld\tiyh, " + imm8_value_str(x);
		case LD_IYH_A: return "ld\tiyh, a";
		case LD_IYH_B: return "ld\tiyh, b";
		case LD_IYH_C: return "ld\tiyh, c";
		case LD_IYH_D: return "ld\tiyh, d";
		case LD_IYH_E: return "ld\tiyh, e";
		case LD_IYH_IYH: return "ld\tiyh, iyh";
		case LD_IYH_IYL: return "ld\tiyh, iyl";
	
		case LD_IYL_N: return "ld\tiyl, " + imm8_value_str(x);
		case LD_IYL_A: return "ld\tiyl, a";
		case LD_IYL_B: return "ld\tiyl, b";
		case LD_IYL_C: return "ld\tiyl, c";
		case LD_IYL_D: return "ld\tiyl, d";
		case LD_IYL_E: return "ld\tiyl, e";
		case LD_IYL_IYH: return "ld\tiyl, iyh";
		case LD_IYL_IYL: return "ld\tiyl, iyl";
	
		/* load HL */
	
		case LD_HL_N: return "ld\thl, " + imm24_value_str(x);
		case LD_HL_N_SIS: return "ld.sis\thl, " + imm16_value_str(x);
		case LD_HL_ADDR: return "ld\thl, (" + imm24_value_str(x) + ")";
		case LD_HL_PHL: return "ld\thl, (hl)";
		case LD_HL_PIX: return "ld\thl, (ix" + imm8_offset_str(x) + ")";
		case LD_HL_PIY: return "ld\thl, (iy" + imm8_offset_str(x) + ")";
	
		/* load reg24 */
	
		case LD_BC_N: return "ld\tbc, " + imm24_value_str(x);
		case LD_BC_N_SIS: return "ld.sis\tbc, " + imm16_value_str(x);
		case LD_BC_ADDR: return "ld\tbc, (" + imm24_value_str(x) + ")";
		case LD_BC_PHL: return "ld\tbc, (hl)";
		case LD_BC_PIX: return "ld\tbc, (ix" + imm8_offset_str(x) + ")";
		case LD_BC_PIY: return "ld\tbc, (iy" + imm8_offset_str(x) + ")";
	
		case LD_DE_N: return "ld\tde, " + imm24_value_str(x);
		case LD_DE_N_SIS: return "ld.sis\tde, " + imm16_value_str(x);
		case LD_DE_ADDR: return "ld\tde, (" + imm24_value_str(x) + ")";
		case LD_DE_PHL: return "ld\tde, (hl)";
		case LD_DE_PIX: return "ld\tde, (ix" + imm8_offset_str(x) + ")";
		case LD_DE_PIY: return "ld\tde, (iy" + imm8_offset_str(x) + ")";
	
		/* load SP */
	
		case LD_SP_N: return "ld\tsp, " + imm24_value_str(x);
		case LD_SP_N_SIS: return "ld.sis\tsp, " + imm16_value_str(x);
		case LD_SP_HL: return "ld\tsp, hl";
		case LD_SP_IX: return "ld\tsp, ix";
		case LD_SP_IY: return "ld\tsp, iy";
		case LD_SP_ADDR: return "ld\tsp, (" + imm24_value_str(x) + ")";
	
		/* load IX IY */
	
		case LD_IX_N: return "ld\tix, " + imm24_value_str(x);
		case LD_IX_N_SIS: return "ld.sis\tix, " + imm16_value_str(x);
		case LD_IX_ADDR: return "ld\tix, (" + imm24_value_str(x) + ")";
		case LD_IX_PHL: return "ld\tix, (hl)";
		case LD_IX_PIX: return "ld\tix, (ix" + imm8_offset_str(x) + ")";
		case LD_IX_PIY: return "ld\tix, (iy" + imm8_offset_str(x) + ")";
	
		case LD_IY_N: return "ld\tiy, " + imm24_value_str(x);
		case LD_IY_N_SIS: return "ld.sis\tiy, " + imm16_value_str(x);
		case LD_IY_ADDR: return "ld\tiy, (" + imm24_value_str(x) + ")";
		case LD_IY_PHL: return "ld\tiy, (hl)";
		case LD_IY_PIX: return "ld\tiy, (ix" + imm8_offset_str(x) + ")";
		case LD_IY_PIY: return "ld\tiy, (iy" + imm8_offset_str(x) + ")";
	
		/* store register A */
	
		case LD_ADDR_A: return "ld\t(" + imm24_value_str(x) + "), a";
	
		case LD_PBC_A: return "ld\t(bc), a";
		case LD_PDE_A: return "ld\t(de), a";
		case LD_PHL_A: return "ld\t(hl), a";
	
		/* store reg8 */
	
		case LD_PHL_N: return "ld\t(hl), " + imm8_value_str(x);
		case LD_PHL_B: return "ld\t(hl), b";
		case LD_PHL_C: return "ld\t(hl), c";
		case LD_PHL_D: return "ld\t(hl), d";
		case LD_PHL_E: return "ld\t(hl), e";
		case LD_PHL_H: return "ld\t(hl), h";
		case LD_PHL_L: return "ld\t(hl), l";
	
		case LD_PIX_N: return "ld\t(ix" + imm8_offset_str(x) + "), " + imm8_value_str(x);
		case LD_PIX_A: return "ld\t(ix" + imm8_offset_str(x) + "), a";
		case LD_PIX_B: return "ld\t(ix" + imm8_offset_str(x) + "), b";
		case LD_PIX_C: return "ld\t(ix" + imm8_offset_str(x) + "), c";
		case LD_PIX_D: return "ld\t(ix" + imm8_offset_str(x) + "), d";
		case LD_PIX_E: return "ld\t(ix" + imm8_offset_str(x) + "), e";
		case LD_PIX_H: return "ld\t(ix" + imm8_offset_str(x) + "), h";
		case LD_PIX_L: return "ld\t(ix" + imm8_offset_str(x) + "), l";
	
		case LD_PIY_N: return "ld\t(iy" + imm8_offset_str(x) + "), " + imm8_value_str(x);
		case LD_PIY_A: return "ld\t(iy" + imm8_offset_str(x) + "), a";
		case LD_PIY_B: return "ld\t(iy" + imm8_offset_str(x) + "), b";
		case LD_PIY_C: return "ld\t(iy" + imm8_offset_str(x) + "), c";
		case LD_PIY_D: return "ld\t(iy" + imm8_offset_str(x) + "), d";
		case LD_PIY_E: return "ld\t(iy" + imm8_offset_str(x) + "), e";
		case LD_PIY_H: return "ld\t(iy" + imm8_offset_str(x) + "), h";
		case LD_PIY_L: return "ld\t(iy" + imm8_offset_str(x) + "), l";
	
		/* write reg24 to address */
	
		case LD_ADDR_HL: return "ld\t(" + imm24_value_str(x) + "), hl";

		case LD_ADDR_BC: return "ld\t(" + imm24_value_str(x) + "), bc";
		case LD_ADDR_DE: return "ld\t(" + imm24_value_str(x) + "), de";
		case LD_ADDR_IX: return "ld\t(" + imm24_value_str(x) + "), ix";
		case LD_ADDR_IY: return "ld\t(" + imm24_value_str(x) + "), iy";
		case LD_ADDR_SP: return "ld\t(" + imm24_value_str(x) + "), sp";
	
		/* store reg24 */
	
		case LD_PHL_BC: return "ld\t(hl), bc";
		case LD_PHL_DE: return "ld\t(hl), de";
		case LD_PHL_HL: return "ld\t(hl), hl";
		case LD_PHL_IX: return "ld\t(hl), ix";
		case LD_PHL_IY: return "ld\t(hl), iy";
	
		case LD_PIX_BC: return "ld\t(ix" + imm8_offset_str(x) + "), bc";
		case LD_PIX_DE: return "ld\t(ix" + imm8_offset_str(x) + "), de";
		case LD_PIX_HL: return "ld\t(ix" + imm8_offset_str(x) + "), hl";
		case LD_PIX_IX: return "ld\t(ix" + imm8_offset_str(x) + "), ix";
		case LD_PIX_IY: return "ld\t(ix" + imm8_offset_str(x) + "), iy";
	
		case LD_PIY_BC: return "ld\t(iy" + imm8_offset_str(x) + "), bc";
		case LD_PIY_DE: return "ld\t(iy" + imm8_offset_str(x) + "), de";
		case LD_PIY_HL: return "ld\t(iy" + imm8_offset_str(x) + "), hl";
		case LD_PIY_IX: return "ld\t(iy" + imm8_offset_str(x) + "), ix";
		case LD_PIY_IY: return "ld\t(iy" + imm8_offset_str(x) + "), iy";
	
		/* load effective address */
	
		case LEA_BC_IX: return "lea\tbc, ix" + imm8_offset_str(x);
		case LEA_BC_IY: return "lea\tbc, iy" + imm8_offset_str(x);
		case LEA_DE_IX: return "lea\tde, ix" + imm8_offset_str(x);
		case LEA_DE_IY: return "lea\tde, iy" + imm8_offset_str(x);
		case LEA_HL_IX: return "lea\thl, ix" + imm8_offset_str(x);
		case LEA_HL_IY: return "lea\thl, iy" + imm8_offset_str(x);
		case LEA_IX_IX: return "lea\tix, ix" + imm8_offset_str(x);
		case LEA_IX_IY: return "lea\tix, iy" + imm8_offset_str(x);
		case LEA_IY_IX: return "lea\tiy, ix" + imm8_offset_str(x);
		case LEA_IY_IY: return "lea\tiy, iy" + imm8_offset_str(x);
	
		case LEA_BC_IX_SIS: return "lea.sis\tbc, ix" + imm8_offset_str(x);
		case LEA_BC_IY_SIS: return "lea.sis\tbc, iy" + imm8_offset_str(x);
		case LEA_DE_IX_SIS: return "lea.sis\tde, ix" + imm8_offset_str(x);
		case LEA_DE_IY_SIS: return "lea.sis\tde, iy" + imm8_offset_str(x);
		case LEA_HL_IX_SIS: return "lea.sis\thl, ix" + imm8_offset_str(x);
		case LEA_HL_IY_SIS: return "lea.sis\thl, iy" + imm8_offset_str(x);
		case LEA_IX_IX_SIS: return "lea.sis\tix, ix" + imm8_offset_str(x);
		case LEA_IX_IY_SIS: return "lea.sis\tix, iy" + imm8_offset_str(x);
		case LEA_IY_IX_SIS: return "lea.sis\tiy, ix" + imm8_offset_str(x);
		case LEA_IY_IY_SIS: return "lea.sis\tiy, iy" + imm8_offset_str(x);
	
		/* Test bits */
	
		case BIT_0_A: return "bit\t0, a";
		case BIT_0_B: return "bit\t0, b";
		case BIT_0_C: return "bit\t0, c";
		case BIT_0_D: return "bit\t0, d";
		case BIT_0_E: return "bit\t0, e";
		case BIT_0_H: return "bit\t0, h";
		case BIT_0_L: return "bit\t0, l";
		case BIT_0_PHL: return "bit\t0, (hl)";
		case BIT_0_PIX: return "bit\t0, (ix" + imm8_offset_str(x) + ")";
		case BIT_0_PIY: return "bit\t0, (iy" + imm8_offset_str(x) + ")";
		case BIT_1_A: return "bit\t1, a";
		case BIT_1_B: return "bit\t1, b";
		case BIT_1_C: return "bit\t1, c";
		case BIT_1_D: return "bit\t1, d";
		case BIT_1_E: return "bit\t1, e";
		case BIT_1_H: return "bit\t1, h";
		case BIT_1_L: return "bit\t1, l";
		case BIT_1_PHL: return "bit\t1, (hl)";
		case BIT_1_PIX: return "bit\t1, (ix" + imm8_offset_str(x) + ")";
		case BIT_1_PIY: return "bit\t1, (iy" + imm8_offset_str(x) + ")";
		case BIT_2_A: return "bit\t2, a";
		case BIT_2_B: return "bit\t2, b";
		case BIT_2_C: return "bit\t2, c";
		case BIT_2_D: return "bit\t2, d";
		case BIT_2_E: return "bit\t2, e";
		case BIT_2_H: return "bit\t2, h";
		case BIT_2_L: return "bit\t2, l";
		case BIT_2_PHL: return "bit\t2, (hl)";
		case BIT_2_PIX: return "bit\t2, (ix" + imm8_offset_str(x) + ")";
		case BIT_2_PIY: return "bit\t2, (iy" + imm8_offset_str(x) + ")";
		case BIT_3_A: return "bit\t3, a";
		case BIT_3_B: return "bit\t3, b";
		case BIT_3_C: return "bit\t3, c";
		case BIT_3_D: return "bit\t3, d";
		case BIT_3_E: return "bit\t3, e";
		case BIT_3_H: return "bit\t3, h";
		case BIT_3_L: return "bit\t3, l";
		case BIT_3_PHL: return "bit\t3, (hl)";
		case BIT_3_PIX: return "bit\t3, (ix" + imm8_offset_str(x) + ")";
		case BIT_3_PIY: return "bit\t3, (iy" + imm8_offset_str(x) + ")";
		case BIT_4_A: return "bit\t4, a";
		case BIT_4_B: return "bit\t4, b";
		case BIT_4_C: return "bit\t4, c";
		case BIT_4_D: return "bit\t4, d";
		case BIT_4_E: return "bit\t4, e";
		case BIT_4_H: return "bit\t4, h";
		case BIT_4_L: return "bit\t4, l";
		case BIT_4_PHL: return "bit\t4, (hl)";
		case BIT_4_PIX: return "bit\t4, (ix" + imm8_offset_str(x) + ")";
		case BIT_4_PIY: return "bit\t4, (iy" + imm8_offset_str(x) + ")";
		case BIT_5_A: return "bit\t5, a";
		case BIT_5_B: return "bit\t5, b";
		case BIT_5_C: return "bit\t5, c";
		case BIT_5_D: return "bit\t5, d";
		case BIT_5_E: return "bit\t5, e";
		case BIT_5_H: return "bit\t5, h";
		case BIT_5_L: return "bit\t5, l";
		case BIT_5_PHL: return "bit\t5, (hl)";
		case BIT_5_PIX: return "bit\t5, (ix" + imm8_offset_str(x) + ")";
		case BIT_5_PIY: return "bit\t5, (iy" + imm8_offset_str(x) + ")";
		case BIT_6_A: return "bit\t6, a";
		case BIT_6_B: return "bit\t6, b";
		case BIT_6_C: return "bit\t6, c";
		case BIT_6_D: return "bit\t6, d";
		case BIT_6_E: return "bit\t6, e";
		case BIT_6_H: return "bit\t6, h";
		case BIT_6_L: return "bit\t6, l";
		case BIT_6_PHL: return "bit\t6, (hl)";
		case BIT_6_PIX: return "bit\t6, (ix" + imm8_offset_str(x) + ")";
		case BIT_6_PIY: return "bit\t6, (iy" + imm8_offset_str(x) + ")";
		case BIT_7_A: return "bit\t7, a";
		case BIT_7_B: return "bit\t7, b";
		case BIT_7_C: return "bit\t7, c";
		case BIT_7_D: return "bit\t7, d";
		case BIT_7_E: return "bit\t7, e";
		case BIT_7_H: return "bit\t7, h";
		case BIT_7_L: return "bit\t7, l";
		case BIT_7_PHL: return "bit\t7, (hl)";
		case BIT_7_PIX: return "bit\t7, (ix" + imm8_offset_str(x) + ")";
		case BIT_7_PIY: return "bit\t7, (iy" + imm8_offset_str(x) + ")";
	
		/* Set bits */
		
		case SET_0_A: return "set\t0, a";
		case SET_0_B: return "set\t0, b";
		case SET_0_C: return "set\t0, c";
		case SET_0_D: return "set\t0, d";
		case SET_0_E: return "set\t0, e";
		case SET_0_H: return "set\t0, h";
		case SET_0_L: return "set\t0, l";
		case SET_0_PHL: return "set\t0, (hl)";
		case SET_0_PIX: return "set\t0, (ix" + imm8_offset_str(x) + ")";
		case SET_0_PIY: return "set\t0, (iy" + imm8_offset_str(x) + ")";
		case SET_1_A: return "set\t1, a";
		case SET_1_B: return "set\t1, b";
		case SET_1_C: return "set\t1, c";
		case SET_1_D: return "set\t1, d";
		case SET_1_E: return "set\t1, e";
		case SET_1_H: return "set\t1, h";
		case SET_1_L: return "set\t1, l";
		case SET_1_PHL: return "set\t1, (hl)";
		case SET_1_PIX: return "set\t1, (ix" + imm8_offset_str(x) + ")";
		case SET_1_PIY: return "set\t1, (iy" + imm8_offset_str(x) + ")";
		case SET_2_A: return "set\t2, a";
		case SET_2_B: return "set\t2, b";
		case SET_2_C: return "set\t2, c";
		case SET_2_D: return "set\t2, d";
		case SET_2_E: return "set\t2, e";
		case SET_2_H: return "set\t2, h";
		case SET_2_L: return "set\t2, l";
		case SET_2_PHL: return "set\t2, (hl)";
		case SET_2_PIX: return "set\t2, (ix" + imm8_offset_str(x) + ")";
		case SET_2_PIY: return "set\t2, (iy" + imm8_offset_str(x) + ")";
		case SET_3_A: return "set\t3, a";
		case SET_3_B: return "set\t3, b";
		case SET_3_C: return "set\t3, c";
		case SET_3_D: return "set\t3, d";
		case SET_3_E: return "set\t3, e";
		case SET_3_H: return "set\t3, h";
		case SET_3_L: return "set\t3, l";
		case SET_3_PHL: return "set\t3, (hl)";
		case SET_3_PIX: return "set\t3, (ix" + imm8_offset_str(x) + ")";
		case SET_3_PIY: return "set\t3, (iy" + imm8_offset_str(x) + ")";
		case SET_4_A: return "set\t4, a";
		case SET_4_B: return "set\t4, b";
		case SET_4_C: return "set\t4, c";
		case SET_4_D: return "set\t4, d";
		case SET_4_E: return "set\t4, e";
		case SET_4_H: return "set\t4, h";
		case SET_4_L: return "set\t4, l";
		case SET_4_PHL: return "set\t4, (hl)";
		case SET_4_PIX: return "set\t4, (ix" + imm8_offset_str(x) + ")";
		case SET_4_PIY: return "set\t4, (iy" + imm8_offset_str(x) + ")";
		case SET_5_A: return "set\t5, a";
		case SET_5_B: return "set\t5, b";
		case SET_5_C: return "set\t5, c";
		case SET_5_D: return "set\t5, d";
		case SET_5_E: return "set\t5, e";
		case SET_5_H: return "set\t5, h";
		case SET_5_L: return "set\t5, l";
		case SET_5_PHL: return "set\t5, (hl)";
		case SET_5_PIX: return "set\t5, (ix" + imm8_offset_str(x) + ")";
		case SET_5_PIY: return "set\t5, (iy" + imm8_offset_str(x) + ")";
		case SET_6_A: return "set\t6, a";
		case SET_6_B: return "set\t6, b";
		case SET_6_C: return "set\t6, c";
		case SET_6_D: return "set\t6, d";
		case SET_6_E: return "set\t6, e";
		case SET_6_H: return "set\t6, h";
		case SET_6_L: return "set\t6, l";
		case SET_6_PHL: return "set\t6, (hl)";
		case SET_6_PIX: return "set\t6, (ix" + imm8_offset_str(x) + ")";
		case SET_6_PIY: return "set\t6, (iy" + imm8_offset_str(x) + ")";
		case SET_7_A: return "set\t7, a";
		case SET_7_B: return "set\t7, b";
		case SET_7_C: return "set\t7, c";
		case SET_7_D: return "set\t7, d";
		case SET_7_E: return "set\t7, e";
		case SET_7_H: return "set\t7, h";
		case SET_7_L: return "set\t7, l";
		case SET_7_PHL: return "set\t7, (hl)";
		case SET_7_PIX: return "set\t7, (ix" + imm8_offset_str(x) + ")";
		case SET_7_PIY: return "set\t7, (iy" + imm8_offset_str(x) + ")";
	
		/* Reset bits */
	
		case RES_0_A: return "res\t0, a";
		case RES_0_B: return "res\t0, b";
		case RES_0_C: return "res\t0, c";
		case RES_0_D: return "res\t0, d";
		case RES_0_E: return "res\t0, e";
		case RES_0_H: return "res\t0, h";
		case RES_0_L: return "res\t0, l";
		case RES_0_PHL: return "res\t0, (hl)";
		case RES_0_PIX: return "res\t0, (ix" + imm8_offset_str(x) + ")";
		case RES_0_PIY: return "res\t0, (iy" + imm8_offset_str(x) + ")";
		case RES_1_A: return "res\t1, a";
		case RES_1_B: return "res\t1, b";
		case RES_1_C: return "res\t1, c";
		case RES_1_D: return "res\t1, d";
		case RES_1_E: return "res\t1, e";
		case RES_1_H: return "res\t1, h";
		case RES_1_L: return "res\t1, l";
		case RES_1_PHL: return "res\t1, (hl)";
		case RES_1_PIX: return "res\t1, (ix" + imm8_offset_str(x) + ")";
		case RES_1_PIY: return "res\t1, (iy" + imm8_offset_str(x) + ")";
		case RES_2_A: return "res\t2, a";
		case RES_2_B: return "res\t2, b";
		case RES_2_C: return "res\t2, c";
		case RES_2_D: return "res\t2, d";
		case RES_2_E: return "res\t2, e";
		case RES_2_H: return "res\t2, h";
		case RES_2_L: return "res\t2, l";
		case RES_2_PHL: return "res\t2, (hl)";
		case RES_2_PIX: return "res\t2, (ix" + imm8_offset_str(x) + ")";
		case RES_2_PIY: return "res\t2, (iy" + imm8_offset_str(x) + ")";
		case RES_3_A: return "res\t3, a";
		case RES_3_B: return "res\t3, b";
		case RES_3_C: return "res\t3, c";
		case RES_3_D: return "res\t3, d";
		case RES_3_E: return "res\t3, e";
		case RES_3_H: return "res\t3, h";
		case RES_3_L: return "res\t3, l";
		case RES_3_PHL: return "res\t3, (hl)";
		case RES_3_PIX: return "res\t3, (ix" + imm8_offset_str(x) + ")";
		case RES_3_PIY: return "res\t3, (iy" + imm8_offset_str(x) + ")";
		case RES_4_A: return "res\t4, a";
		case RES_4_B: return "res\t4, b";
		case RES_4_C: return "res\t4, c";
		case RES_4_D: return "res\t4, d";
		case RES_4_E: return "res\t4, e";
		case RES_4_H: return "res\t4, h";
		case RES_4_L: return "res\t4, l";
		case RES_4_PHL: return "res\t4, (hl)";
		case RES_4_PIX: return "res\t4, (ix" + imm8_offset_str(x) + ")";
		case RES_4_PIY: return "res\t4, (iy" + imm8_offset_str(x) + ")";
		case RES_5_A: return "res\t5, a";
		case RES_5_B: return "res\t5, b";
		case RES_5_C: return "res\t5, c";
		case RES_5_D: return "res\t5, d";
		case RES_5_E: return "res\t5, e";
		case RES_5_H: return "res\t5, h";
		case RES_5_L: return "res\t5, l";
		case RES_5_PHL: return "res\t5, (hl)";
		case RES_5_PIX: return "res\t5, (ix" + imm8_offset_str(x) + ")";
		case RES_5_PIY: return "res\t5, (iy" + imm8_offset_str(x) + ")";
		case RES_6_A: return "res\t6, a";
		case RES_6_B: return "res\t6, b";
		case RES_6_C: return "res\t6, c";
		case RES_6_D: return "res\t6, d";
		case RES_6_E: return "res\t6, e";
		case RES_6_H: return "res\t6, h";
		case RES_6_L: return "res\t6, l";
		case RES_6_PHL: return "res\t6, (hl)";
		case RES_6_PIX: return "res\t6, (ix" + imm8_offset_str(x) + ")";
		case RES_6_PIY: return "res\t6, (iy" + imm8_offset_str(x) + ")";
		case RES_7_A: return "res\t7, a";
		case RES_7_B: return "res\t7, b";
		case RES_7_C: return "res\t7, c";
		case RES_7_D: return "res\t7, d";
		case RES_7_E: return "res\t7, e";
		case RES_7_H: return "res\t7, h";
		case RES_7_L: return "res\t7, l";
		case RES_7_PHL: return "res\t7, (hl)";
		case RES_7_PIX: return "res\t7, (ix" + imm8_offset_str(x) + ")";
		case RES_7_PIY: return "res\t7, (iy" + imm8_offset_str(x) + ")";
		
		/* IO instructions */
	
		case IN_A_PN: return "in\ta, (n)";
		case IN_A_PBC: return "in\ta, (bc)";
		case IN_B_PBC: return "in\tb, (bc)";
		case IN_C_PBC: return "in\tc, (bc)";
		case IN_D_PBC: return "in\td, (bc)";
		case IN_E_PBC: return "in\te, (bc)";
		case IN_H_PBC: return "in\th, (bc)";
		case IN_L_PBC: return "in\tl, (bc)";
	
		case IN0_A_PN: return "in0\ta, (n)";
		case IN0_B_PN: return "in0\tb, (n)";
		case IN0_C_PN: return "in0\tc, (n)";
		case IN0_D_PN: return "in0\td, (n)";
		case IN0_E_PN: return "in0\te, (n)";
		case IN0_H_PN: return "in0\th, (n)";
		case IN0_L_PN: return "in0\tl, (n)";
	
		case IND: return "ind";
		case IND2: return "ind2";
		case IND2R: return "ind2r";
		case INDM: return "indm";
		case INDMR: return "indmr";
		case INDR: return "indr";
		case INDRX: return "indrx";
	
		case INI: return "ini";
		case INI2: return "ini2";
		case INI2R: return "ini2r";
		case INIM: return "inim";
		case INIMR: return "inimr";
		case INIR: return "inir";
		case INIRX: return "inirx";
	
		case OTD2R: return "otd2r";
		case OTDM: return "otdm";
		case OTDMR: return "otdmr";
		case OTDR: return "otdr";
		case OTDRX: return "otdrx";
	
		case OTI2R: return "oti2r";
		case OTIM: return "otim";
		case OTIMR: return "otimr";
		case OTIR: return "otir";
		case OTIRX: return "otirx";
	
		case OUT_PBC_A: return "out\t(bc), a";
		case OUT_PBC_B: return "out\t(bc), b";
		case OUT_PBC_C: return "out\t(bc), c";
		case OUT_PBC_D: return "out\t(bc), d";
		case OUT_PBC_E: return "out\t(bc), e";
		case OUT_PBC_H: return "out\t(bc), h";
		case OUT_PBC_L: return "out\t(bc), l";
		case OUT_PN_A: return "out\t(n), a";
	
		case OUT0_PN_A: return "out0\t(n), a";
		case OUT0_PN_B: return "out0\t(n), b";
		case OUT0_PN_C: return "out0\t(n), c";
		case OUT0_PN_D: return "out0\t(n), d";
		case OUT0_PN_E: return "out0\t(n), e";
		case OUT0_PN_H: return "out0\t(n), h";
		case OUT0_PN_L: return "out0\t(n), l";
	
		case OUTD: return "outd";
		case OUTD2: return "outd2";
		case OUTI: return "outi";
		case OUTI2: return "outi2";

		case TSTIO: return "tstio\tn";

		default: return "unknown";
	}
}

void print_program(const ez80_program& x) {
	const vector<asm_line>& code = x.prog;
	for (size_t i = 0; i < code.size(); i++) {
		const asm_line& line = code[i];
		switch (line.line_type) {
			case asm_line_type::blank: {
				break;
			} break;
			case asm_line_type::instruction: {
				printf("\t%s\n", instruction_to_string(line.instruction).c_str());
			} break;
			default: {
				printf("%s\n", line.text.c_str());
			} break;
		}
	}
}

string code_section_to_string(const ez80_code_section& x) {
	string output = "";
	const list<ez80_code>& prog = x.prog;
	for (ez80_code code : prog) {
		switch (code.type) {
			case ez80_code_type::known_func:
			case ez80_code_type::branch:
			case ez80_code_type::code: {
				output += "\t" + instruction_to_string(code.instruction) + "\n";
			} break;
			case ez80_code_type::label: {
				output += code.label.text + "\n";
			} break;
			case ez80_code_type::directive: {
				output += "directive\n";
			} break;
			default: break;
		}
	}
	return output;
}
