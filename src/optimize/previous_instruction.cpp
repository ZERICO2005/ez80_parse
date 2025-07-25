#include "current_state.hpp"

template <typename T>
void cmpu_was_nc(__attribute__((unused)) flag_pair& f, T& x, T& y) {
	// (X >= Y)
	T x_mask, y_mask;
	x_mask.set_to_unsigned_range(
		y.get_unsigned_minimum(),
		x.get_unsigned_maximum()
	);
	y_mask.set_to_unsigned_range(
		y.get_unsigned_minimum(),
		x.get_unsigned_maximum()
	);
	x.merge_bits(x_mask);
	y.merge_bits(y_mask);
}

template <typename T>
void cmpu_was_c(flag_pair& f, T& x, T& y) {
	// (X < Y) && (Y >= 1) && (X < UINT(N)_MAX)
	// X is [0, UINT(N)_MAX - 1]
	// Y is [1, UINT(N)_MAX]
	f.set_zero(false);
	T x_mask, y_mask;
	x_mask.set_to_unsigned_range(
		x.get_unsigned_minimum(),
		y.get_unsigned_maximum() - 1
	);
	y_mask.set_to_unsigned_range(
		x.get_unsigned_minimum() + 1,
		y.get_unsigned_maximum()
	);
	x.merge_bits(x_mask);
	y.merge_bits(y_mask);
}

void current_state::check_previous_instruction(ez80_instruction current_instruction) {
	using enum ez80_op_code;

	reg8_pair imm8;
	reg8_pair unknown8;
	unknown8.set_unknown();
	uint24_t imm_value = previous_instruction.value;
	imm8.set_value((uint8_t)previous_instruction.value);
	if (!previous_instruction.known_value) {
		imm8.set_unknown();
	}

	if (previous_was_known_func) {
		switch (current_instruction.op_code) {
			using enum ez80_known_function;
			case JR_C:
			case JP_C: {
				F.set_carry(false);
				switch (previous_known_func) {
					case __lcmpu:
					{ 
						reg32_pair x = get32_EUHL();
						reg32_pair y = get32_AUBC();
						cmpu_was_nc(F, x, y);
						set32_EUHL(x);
						set32_AUBC(y);
					} break;
					case __i48cmpu:
					{ 
						reg48_pair x = get48_UDEUHL();
						reg48_pair y = get48_UIYUBC();
						cmpu_was_nc(F, x, y);
						set48_UDEUHL(x);
						set48_UIYUBC(y);
					} break;
					case __llcmpu:
					{ 
						reg64_pair x = get64_BCUDEUHL();
						reg64_pair y = get64_STACK(0);
						cmpu_was_nc(F, x, y);
						set64_partial_BCUDEUHL(x);
						set64_STACK(y, 0);
					} break;
					default: break;
				}
			} break;
			case JR_NC:
			case JP_NC: {
				F.set_carry(true);
				switch (previous_known_func) {
					case __lcmpu:
					{ 
						reg32_pair x = get32_EUHL();
						reg32_pair y = get32_AUBC();
						cmpu_was_c(F, x, y);
						set32_EUHL(x);
						set32_AUBC(y);
					} break;
					case __i48cmpu:
					{ 
						reg48_pair x = get48_UDEUHL();
						reg48_pair y = get48_UIYUBC();
						cmpu_was_c(F, x, y);
						set48_UDEUHL(x);
						set48_UIYUBC(y);
					} break;
					case __llcmpu:
					{ 
						reg64_pair x = get64_BCUDEUHL();
						reg64_pair y = get64_STACK(0);
						cmpu_was_c(F, x, y);
						set64_partial_BCUDEUHL(x);
						set64_STACK(y, 0);
					} break;
					default: break;
				}
			} break;
			case JR_Z:
			case JP_Z: {
				F.set_zero(false);
			} break;
			case JR_NZ:
			case JP_NZ: {
				F.set_zero(true);
				switch (previous_known_func) {
					case __scmpzero:
					{ H.set_to_zero(); L.set_to_zero(); } break;
					case __icmpzero:
					{ set_HL(0); } break;
					case __lcmpzero:
					case __lcmpu:
					case __lcmps:
					{ set_HL(0); E.set_to_zero(); } break;
					case __i48cmpzero:
					case __i48cmpu:
					case __i48cmps:
					{ set_HL(0); set_DE(0); } break;
					case __llcmpzero:
					case __llcmpu:
					case __llcmps:
					{ B.set_to_zero(); C.set_to_zero(); set_HL(0); set_DE(0); } break;
					default: break;
				}
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
			default: break;
		}
		goto finish;
	}
	switch (current_instruction.op_code) {
		case JR_C:
		case JP_C: {
			F.set_carry(false);
			switch (previous_instruction.op_code) {
				case SBC_A_A: { sbc_a_a(); } break;
				case SBC_HL_HL: { set_HL(sbc24_hl_hl(get_HL())); } break;
				case SBC_HL_HL_SIS: { set16_zero_HL(sbc16_hl_hl(get16_HL())); } break;
				case NEG: { A.set_value(0); acc_neg(); } break;
				case RLCA: { A.bit_clear(0); } break;
				case RRCA: { A.bit_clear(7); } break;
				case RLC_A: { A.bit_clear(0); reg8_shift_set_flags(A); } break;
				case RLC_B: { B.bit_clear(0); reg8_shift_set_flags(B); } break;
				case RLC_C: { C.bit_clear(0); reg8_shift_set_flags(C); } break;
				case RLC_D: { D.bit_clear(0); reg8_shift_set_flags(D); } break;
				case RLC_E: { E.bit_clear(0); reg8_shift_set_flags(E); } break;
				case RLC_H: { H.bit_clear(0); reg8_shift_set_flags(H); } break;
				case RLC_L: { L.bit_clear(0); reg8_shift_set_flags(L); } break;
				case RRC_A: { A.bit_clear(7); reg8_shift_set_flags(A); } break;
				case RRC_B: { B.bit_clear(7); reg8_shift_set_flags(B); } break;
				case RRC_C: { C.bit_clear(7); reg8_shift_set_flags(C); } break;
				case RRC_D: { D.bit_clear(7); reg8_shift_set_flags(D); } break;
				case RRC_E: { E.bit_clear(7); reg8_shift_set_flags(E); } break;
				case RRC_H: { H.bit_clear(7); reg8_shift_set_flags(H); } break;
				case RRC_L: { L.bit_clear(7); reg8_shift_set_flags(L); } break;

				case CP_A_N  : { acc_cp_was_nc(imm8); } break;
				case CP_A_B  : { acc_cp_was_nc(B  ); } break;
				case CP_A_C  : { acc_cp_was_nc(C  ); } break;
				case CP_A_D  : { acc_cp_was_nc(D  ); } break;
				case CP_A_E  : { acc_cp_was_nc(E  ); } break;
				case CP_A_H  : { acc_cp_was_nc(H  ); } break;
				case CP_A_L  : { acc_cp_was_nc(L  ); } break;
				case CP_A_IXH: { acc_cp_was_nc(IXH); } break;
				case CP_A_IXL: { acc_cp_was_nc(IXL); } break;
				case CP_A_IYH: { acc_cp_was_nc(IYH); } break;
				case CP_A_IYL: { acc_cp_was_nc(IYL); } break;
				case CP_A_PHL: { acc_cp_was_nc(unknown8); } break;
				case CP_A_PIX: { acc_cp_was_nc(unknown8); } break;
				case CP_A_PIY: { acc_cp_was_nc(unknown8); } break;
				default: break;
			}
		} break;

		case JR_NC:
		case JP_NC: {
			F.set_carry(true);
			switch (previous_instruction.op_code) {
				case SBC_A_A: { sbc_a_a(); } break;
				case SBC_HL_HL: { set_HL(sbc24_hl_hl(get_HL())); } break;
				case SBC_HL_HL_SIS: { set16_zero_HL(sbc16_hl_hl(get16_HL())); } break;
				case RLCA: { A.bit_set(0); } break;
				case RRCA: { A.bit_set(7); } break;
				case RLC_A: { A.bit_set(0); reg8_shift_set_flags(A); } break;
				case RLC_B: { B.bit_set(0); reg8_shift_set_flags(B); } break;
				case RLC_C: { C.bit_set(0); reg8_shift_set_flags(C); } break;
				case RLC_D: { D.bit_set(0); reg8_shift_set_flags(D); } break;
				case RLC_E: { E.bit_set(0); reg8_shift_set_flags(E); } break;
				case RLC_H: { H.bit_set(0); reg8_shift_set_flags(H); } break;
				case RLC_L: { L.bit_set(0); reg8_shift_set_flags(L); } break;
				case RRC_A: { A.bit_set(7); reg8_shift_set_flags(A); } break;
				case RRC_B: { B.bit_set(7); reg8_shift_set_flags(B); } break;
				case RRC_C: { C.bit_set(7); reg8_shift_set_flags(C); } break;
				case RRC_D: { D.bit_set(7); reg8_shift_set_flags(D); } break;
				case RRC_E: { E.bit_set(7); reg8_shift_set_flags(E); } break;
				case RRC_H: { H.bit_set(7); reg8_shift_set_flags(H); } break;
				case RRC_L: { L.bit_set(7); reg8_shift_set_flags(L); } break;

				case CP_A_N  : { acc_cp_was_c(imm8); } break;
				case CP_A_B  : { acc_cp_was_c(B  ); } break;
				case CP_A_C  : { acc_cp_was_c(C  ); } break;
				case CP_A_D  : { acc_cp_was_c(D  ); } break;
				case CP_A_E  : { acc_cp_was_c(E  ); } break;
				case CP_A_H  : { acc_cp_was_c(H  ); } break;
				case CP_A_L  : { acc_cp_was_c(L  ); } break;
				case CP_A_IXH: { acc_cp_was_c(IXH); } break;
				case CP_A_IXL: { acc_cp_was_c(IXL); } break;
				case CP_A_IYH: { acc_cp_was_c(IYH); } break;
				case CP_A_IYL: { acc_cp_was_c(IYL); } break;
				case CP_A_PHL: { acc_cp_was_c(unknown8); } break;
				case CP_A_PIX: { acc_cp_was_c(unknown8); } break;
				case CP_A_PIY: { acc_cp_was_c(unknown8); } break;
				default: break;
			}
		} break;

		case JR_Z:
		case JP_Z: {
			F.set_zero(false);
			switch (previous_instruction.op_code) {
				case CPIR: { set_BC(0); F.set_overflow(false); } break;
				case CPDR: { set_BC(0); F.set_overflow(false); } break;
				case BIT_0_A: { A.bit_set(0); } break;
				case BIT_0_B: { B.bit_set(0); } break;
				case BIT_0_C: { C.bit_set(0); } break;
				case BIT_0_D: { D.bit_set(0); } break;
				case BIT_0_E: { E.bit_set(0); } break;
				case BIT_0_H: { H.bit_set(0); } break;
				case BIT_0_L: { L.bit_set(0); } break;
				case BIT_1_A: { A.bit_set(1); } break;
				case BIT_1_B: { B.bit_set(1); } break;
				case BIT_1_C: { C.bit_set(1); } break;
				case BIT_1_D: { D.bit_set(1); } break;
				case BIT_1_E: { E.bit_set(1); } break;
				case BIT_1_H: { H.bit_set(1); } break;
				case BIT_1_L: { L.bit_set(1); } break;
				case BIT_2_A: { A.bit_set(2); } break;
				case BIT_2_B: { B.bit_set(2); } break;
				case BIT_2_C: { C.bit_set(2); } break;
				case BIT_2_D: { D.bit_set(2); } break;
				case BIT_2_E: { E.bit_set(2); } break;
				case BIT_2_H: { H.bit_set(2); } break;
				case BIT_2_L: { L.bit_set(2); } break;
				case BIT_3_A: { A.bit_set(3); } break;
				case BIT_3_B: { B.bit_set(3); } break;
				case BIT_3_C: { C.bit_set(3); } break;
				case BIT_3_D: { D.bit_set(3); } break;
				case BIT_3_E: { E.bit_set(3); } break;
				case BIT_3_H: { H.bit_set(3); } break;
				case BIT_3_L: { L.bit_set(3); } break;
				case BIT_4_A: { A.bit_set(4); } break;
				case BIT_4_B: { B.bit_set(4); } break;
				case BIT_4_C: { C.bit_set(4); } break;
				case BIT_4_D: { D.bit_set(4); } break;
				case BIT_4_E: { E.bit_set(4); } break;
				case BIT_4_H: { H.bit_set(4); } break;
				case BIT_4_L: { L.bit_set(4); } break;
				case BIT_5_A: { A.bit_set(5); } break;
				case BIT_5_B: { B.bit_set(5); } break;
				case BIT_5_C: { C.bit_set(5); } break;
				case BIT_5_D: { D.bit_set(5); } break;
				case BIT_5_E: { E.bit_set(5); } break;
				case BIT_5_H: { H.bit_set(5); } break;
				case BIT_5_L: { L.bit_set(5); } break;
				case BIT_6_A: { A.bit_set(6); } break;
				case BIT_6_B: { B.bit_set(6); } break;
				case BIT_6_C: { C.bit_set(6); } break;
				case BIT_6_D: { D.bit_set(6); } break;
				case BIT_6_E: { E.bit_set(6); } break;
				case BIT_6_H: { H.bit_set(6); } break;
				case BIT_6_L: { L.bit_set(6); } break;
				case BIT_7_A: { A.bit_set(7); } break;
				case BIT_7_B: { B.bit_set(7); } break;
				case BIT_7_C: { C.bit_set(7); } break;
				case BIT_7_D: { D.bit_set(7); } break;
				case BIT_7_E: { E.bit_set(7); } break;
				case BIT_7_H: { H.bit_set(7); } break;
				case BIT_7_L: { L.bit_set(7); } break;
				default: break;
			}
		} break;

		case JR_NZ:
		case JP_NZ: {
			F.set_zero(true);
			switch (previous_instruction.op_code) {
				/* special cases */

				case SBC_A_A: { F.set_carry(true); sbc_a_a(); } break;
				case SBC_HL_HL: { F.set_carry(true); set_HL(sbc24_hl_hl(get_HL())); } break;
				case SBC_HL_HL_SIS: { F.set_carry(true); set16_zero_HL(sbc16_hl_hl(get16_HL())); } break;
				case NEG: { A.set_value(0); acc_neg(); } break;

				case OR_A_A :
				case AND_A_A:
				case TST_A_A:
				{ A.set_value(0); F.set_sign(false); F.set_overflow(true); } break;

				case ADD_A_A:
				case ADC_A_A:
				{ A.set_value(0); F.set_sign(false); } break;

				case ADC_HL_HL    :
				case ADC_HL_HL_SIS:
				{ set_HL(0); F.set_sign(false); } break;

				/* constant */

				case CP_A_N : { acc_cp_was_zero(imm8); } break;
				case TST_A_N: { acc_tst_was_zero(imm8); } break;

				/* general cases */

				case ADD_A_B  : { acc_add_was_zero(B   ); } break;
				case ADD_A_C  : { acc_add_was_zero(C   ); } break;
				case ADD_A_D  : { acc_add_was_zero(D   ); } break;
				case ADD_A_E  : { acc_add_was_zero(E   ); } break;
				case ADD_A_H  : { acc_add_was_zero(H   ); } break;
				case ADD_A_L  : { acc_add_was_zero(L   ); } break;
				case ADD_A_IXH: { acc_add_was_zero(IXH ); } break;
				case ADD_A_IXL: { acc_add_was_zero(IXL ); } break;
				case ADD_A_IYH: { acc_add_was_zero(IYH ); } break;
				case ADD_A_IYL: { acc_add_was_zero(IYL ); } break;
				case ADD_A_PHL: { acc_add_was_zero(unknown8); } break;
				case ADD_A_PIX: { acc_add_was_zero(unknown8); } break;
				case ADD_A_PIY: { acc_add_was_zero(unknown8); } break;

				case ADC_A_B  : { acc_adc_was_zero(B   ); } break;
				case ADC_A_C  : { acc_adc_was_zero(C   ); } break;
				case ADC_A_D  : { acc_adc_was_zero(D   ); } break;
				case ADC_A_E  : { acc_adc_was_zero(E   ); } break;
				case ADC_A_H  : { acc_adc_was_zero(H   ); } break;
				case ADC_A_L  : { acc_adc_was_zero(L   ); } break;
				case ADC_A_IXH: { acc_adc_was_zero(IXH ); } break;
				case ADC_A_IXL: { acc_adc_was_zero(IXL ); } break;
				case ADC_A_IYH: { acc_adc_was_zero(IYH ); } break;
				case ADC_A_IYL: { acc_adc_was_zero(IYL ); } break;
				case ADC_A_PHL: { acc_adc_was_zero(unknown8); } break;
				case ADC_A_PIX: { acc_adc_was_zero(unknown8); } break;
				case ADC_A_PIY: { acc_adc_was_zero(unknown8); } break;

				case SBC_A_B  : { acc_sbc_was_zero(B   ); } break;
				case SBC_A_C  : { acc_sbc_was_zero(C   ); } break;
				case SBC_A_D  : { acc_sbc_was_zero(D   ); } break;
				case SBC_A_E  : { acc_sbc_was_zero(E   ); } break;
				case SBC_A_H  : { acc_sbc_was_zero(H   ); } break;
				case SBC_A_L  : { acc_sbc_was_zero(L   ); } break;
				case SBC_A_IXH: { acc_sbc_was_zero(IXH ); } break;
				case SBC_A_IXL: { acc_sbc_was_zero(IXL ); } break;
				case SBC_A_IYH: { acc_sbc_was_zero(IYH ); } break;
				case SBC_A_IYL: { acc_sbc_was_zero(IYL ); } break;
				case SBC_A_PHL: { acc_sbc_was_zero(unknown8); } break;
				case SBC_A_PIX: { acc_sbc_was_zero(unknown8); } break;
				case SBC_A_PIY: { acc_sbc_was_zero(unknown8); } break;

				case SUB_A_B  : { acc_sub_was_zero(B   ); } break;
				case SUB_A_C  : { acc_sub_was_zero(C   ); } break;
				case SUB_A_D  : { acc_sub_was_zero(D   ); } break;
				case SUB_A_E  : { acc_sub_was_zero(E   ); } break;
				case SUB_A_H  : { acc_sub_was_zero(H   ); } break;
				case SUB_A_L  : { acc_sub_was_zero(L   ); } break;
				case SUB_A_IXH: { acc_sub_was_zero(IXH ); } break;
				case SUB_A_IXL: { acc_sub_was_zero(IXL ); } break;
				case SUB_A_IYH: { acc_sub_was_zero(IYH ); } break;
				case SUB_A_IYL: { acc_sub_was_zero(IYL ); } break;
				case SUB_A_PHL: { acc_sub_was_zero(unknown8); } break;
				case SUB_A_PIX: { acc_sub_was_zero(unknown8); } break;
				case SUB_A_PIY: { acc_sub_was_zero(unknown8); } break;

				case CP_A_B  : { acc_cp_was_zero(B  ); } break;
				case CP_A_C  : { acc_cp_was_zero(C  ); } break;
				case CP_A_D  : { acc_cp_was_zero(D  ); } break;
				case CP_A_E  : { acc_cp_was_zero(E  ); } break;
				case CP_A_H  : { acc_cp_was_zero(H  ); } break;
				case CP_A_L  : { acc_cp_was_zero(L  ); } break;
				case CP_A_IXH: { acc_cp_was_zero(IXH); } break;
				case CP_A_IXL: { acc_cp_was_zero(IXL); } break;
				case CP_A_IYH: { acc_cp_was_zero(IYH); } break;
				case CP_A_IYL: { acc_cp_was_zero(IYL); } break;
				case CP_A_PHL: { acc_cp_was_zero(unknown8); } break;
				case CP_A_PIX: { acc_cp_was_zero(unknown8); } break;
				case CP_A_PIY: { acc_cp_was_zero(unknown8); } break;

				case OR_A_B  : { acc_or_was_zero(B  ); } break;
				case OR_A_C  : { acc_or_was_zero(C  ); } break;
				case OR_A_D  : { acc_or_was_zero(D  ); } break;
				case OR_A_E  : { acc_or_was_zero(E  ); } break;
				case OR_A_H  : { acc_or_was_zero(H  ); } break;
				case OR_A_L  : { acc_or_was_zero(L  ); } break;
				case OR_A_IXH: { acc_or_was_zero(IXH); } break;
				case OR_A_IXL: { acc_or_was_zero(IXL); } break;
				case OR_A_IYH: { acc_or_was_zero(IYH); } break;
				case OR_A_IYL: { acc_or_was_zero(IYL); } break;
				case OR_A_PHL: { acc_or_was_zero(unknown8); } break;
				case OR_A_PIX: { acc_or_was_zero(unknown8); } break;
				case OR_A_PIY: { acc_or_was_zero(unknown8); } break;

				case XOR_A_B  : { acc_xor_was_zero(B  ); } break;
				case XOR_A_C  : { acc_xor_was_zero(C  ); } break;
				case XOR_A_D  : { acc_xor_was_zero(D  ); } break;
				case XOR_A_E  : { acc_xor_was_zero(E  ); } break;
				case XOR_A_H  : { acc_xor_was_zero(H  ); } break;
				case XOR_A_L  : { acc_xor_was_zero(L  ); } break;
				case XOR_A_IXH: { acc_xor_was_zero(IXH); } break;
				case XOR_A_IXL: { acc_xor_was_zero(IXL); } break;
				case XOR_A_IYH: { acc_xor_was_zero(IYH); } break;
				case XOR_A_IYL: { acc_xor_was_zero(IYL); } break;
				case XOR_A_PHL: { acc_xor_was_zero(unknown8); } break;
				case XOR_A_PIX: { acc_xor_was_zero(unknown8); } break;
				case XOR_A_PIY: { acc_xor_was_zero(unknown8); } break;

				case AND_A_B  : { acc_and_was_zero(B  ); } break;
				case AND_A_C  : { acc_and_was_zero(C  ); } break;
				case AND_A_D  : { acc_and_was_zero(D  ); } break;
				case AND_A_E  : { acc_and_was_zero(E  ); } break;
				case AND_A_H  : { acc_and_was_zero(H  ); } break;
				case AND_A_L  : { acc_and_was_zero(L  ); } break;
				case AND_A_IXH: { acc_and_was_zero(IXH); } break;
				case AND_A_IXL: { acc_and_was_zero(IXL); } break;
				case AND_A_IYH: { acc_and_was_zero(IYH); } break;
				case AND_A_IYL: { acc_and_was_zero(IYL); } break;
				case AND_A_PHL: { acc_and_was_zero(unknown8); } break;
				case AND_A_PIX: { acc_and_was_zero(unknown8); } break;
				case AND_A_PIY: { acc_and_was_zero(unknown8); } break;

				case TST_A_B: { acc_tst_was_zero(B); } break;
				case TST_A_C: { acc_tst_was_zero(C); } break;
				case TST_A_D: { acc_tst_was_zero(D); } break;
				case TST_A_E: { acc_tst_was_zero(E); } break;
				case TST_A_H: { acc_tst_was_zero(H); } break;
				case TST_A_L: { acc_tst_was_zero(L); } break;
				case TST_A_PHL: { acc_tst_was_zero(unknown8); } break;

				case ADC_HL_BC    : { set_BC(adc24_was_zero(get_BC())); } break;
				case ADC_HL_DE    : { set_DE(adc24_was_zero(get_DE())); } break;
				case ADC_HL_SP    : { set_SP(adc24_was_zero(get_SP())); } break;
				case ADC_HL_BC_SIS: { set_BC(adc16_was_zero(get_BC())); } break;
				case ADC_HL_DE_SIS: { set_DE(adc16_was_zero(get_DE())); } break;
				case ADC_HL_SP_SIS: { set_SP(adc16_was_zero(get_SP())); } break;

				case SBC_HL_BC    :
				case SBC_HL_DE    :
				case SBC_HL_SP    :
				{ sbc24_was_zero(); } break;
				case SBC_HL_BC_SIS:
				case SBC_HL_DE_SIS:
				case SBC_HL_SP_SIS:
				{ sbc16_was_zero(); } break;

				case INC_A  : { reg8_inc_dec_to_zero(A  ); } break;
				case INC_B  : { reg8_inc_dec_to_zero(B  ); } break;
				case INC_C  : { reg8_inc_dec_to_zero(C  ); } break;
				case INC_D  : { reg8_inc_dec_to_zero(D  ); } break;
				case INC_E  : { reg8_inc_dec_to_zero(E  ); } break;
				case INC_H  : { reg8_inc_dec_to_zero(H  ); } break;
				case INC_L  : { reg8_inc_dec_to_zero(L  ); } break;
				case INC_IXH: { reg8_inc_dec_to_zero(IXH); } break;
				case INC_IXL: { reg8_inc_dec_to_zero(IXL); } break;
				case INC_IYH: { reg8_inc_dec_to_zero(IYH); } break;
				case INC_IYL: { reg8_inc_dec_to_zero(IYL); } break;
				case DEC_A  : { reg8_inc_dec_to_zero(A  ); } break;
				case DEC_B  : { reg8_inc_dec_to_zero(B  ); } break;
				case DEC_C  : { reg8_inc_dec_to_zero(C  ); } break;
				case DEC_D  : { reg8_inc_dec_to_zero(D  ); } break;
				case DEC_E  : { reg8_inc_dec_to_zero(E  ); } break;
				case DEC_H  : { reg8_inc_dec_to_zero(H  ); } break;
				case DEC_L  : { reg8_inc_dec_to_zero(L  ); } break;
				case DEC_IXH: { reg8_inc_dec_to_zero(IXH); } break;
				case DEC_IXL: { reg8_inc_dec_to_zero(IXL); } break;
				case DEC_IYH: { reg8_inc_dec_to_zero(IYH); } break;
				case DEC_IYL: { reg8_inc_dec_to_zero(IYL); } break;

				case INC_PHL: { reg8_inc_dec_to_zero(unknown8); } break;
				case INC_PIX: { reg8_inc_dec_to_zero(unknown8); } break;
				case INC_PIY: { reg8_inc_dec_to_zero(unknown8); } break;
				case DEC_PHL: { reg8_inc_dec_to_zero(unknown8); } break;
				case DEC_PIX: { reg8_inc_dec_to_zero(unknown8); } break;
				case DEC_PIY: { reg8_inc_dec_to_zero(unknown8); } break;

				case RLC_A: { reg8_shift_to_zero(A); F.set_carry(false); } break;
				case RLC_B: { reg8_shift_to_zero(B); F.set_carry(false); } break;
				case RLC_C: { reg8_shift_to_zero(C); F.set_carry(false); } break;
				case RLC_D: { reg8_shift_to_zero(D); F.set_carry(false); } break;
				case RLC_E: { reg8_shift_to_zero(E); F.set_carry(false); } break;
				case RLC_H: { reg8_shift_to_zero(H); F.set_carry(false); } break;
				case RLC_L: { reg8_shift_to_zero(L); F.set_carry(false); } break;
				case RRC_A: { reg8_shift_to_zero(A); F.set_carry(false); } break;
				case RRC_B: { reg8_shift_to_zero(B); F.set_carry(false); } break;
				case RRC_C: { reg8_shift_to_zero(C); F.set_carry(false); } break;
				case RRC_D: { reg8_shift_to_zero(D); F.set_carry(false); } break;
				case RRC_E: { reg8_shift_to_zero(E); F.set_carry(false); } break;
				case RRC_H: { reg8_shift_to_zero(H); F.set_carry(false); } break;
				case RRC_L: { reg8_shift_to_zero(L); F.set_carry(false); } break;

				case RLC_PHL:
				case RLC_PIX:
				case RLC_PIY:
				case RRC_PHL:
				case RRC_PIX:
				case RRC_PIY:
				{ reg8_shift_to_zero(unknown8); F.set_carry(false); } break;

				case RL_A : { reg8_shift_to_zero(A); } break;
				case RL_B : { reg8_shift_to_zero(B); } break;
				case RL_C : { reg8_shift_to_zero(C); } break;
				case RL_D : { reg8_shift_to_zero(D); } break;
				case RL_E : { reg8_shift_to_zero(E); } break;
				case RL_H : { reg8_shift_to_zero(H); } break;
				case RL_L : { reg8_shift_to_zero(L); } break;
				case RL_PHL : { reg8_shift_to_zero(unknown8); } break;
				case RL_PIX : { reg8_shift_to_zero(unknown8); } break;
				case RL_PIY : { reg8_shift_to_zero(unknown8); } break;

				case RR_A : { reg8_shift_to_zero(A); } break;
				case RR_B : { reg8_shift_to_zero(B); } break;
				case RR_C : { reg8_shift_to_zero(C); } break;
				case RR_D : { reg8_shift_to_zero(D); } break;
				case RR_E : { reg8_shift_to_zero(E); } break;
				case RR_H : { reg8_shift_to_zero(H); } break;
				case RR_L : { reg8_shift_to_zero(L); } break;
				case RR_PHL : { reg8_shift_to_zero(unknown8); } break;
				case RR_PIX : { reg8_shift_to_zero(unknown8); } break;
				case RR_PIY : { reg8_shift_to_zero(unknown8); } break;

				case SRL_A: { reg8_shift_to_zero(A); } break;
				case SRL_B: { reg8_shift_to_zero(B); } break;
				case SRL_C: { reg8_shift_to_zero(C); } break;
				case SRL_D: { reg8_shift_to_zero(D); } break;
				case SRL_E: { reg8_shift_to_zero(E); } break;
				case SRL_H: { reg8_shift_to_zero(H); } break;
				case SRL_L: { reg8_shift_to_zero(L); } break;
				case SRL_PHL: { reg8_shift_to_zero(unknown8); } break;
				case SRL_PIX: { reg8_shift_to_zero(unknown8); } break;
				case SRL_PIY: { reg8_shift_to_zero(unknown8); } break;

				case SLA_A: { reg8_shift_to_zero(A); } break;
				case SLA_B: { reg8_shift_to_zero(B); } break;
				case SLA_C: { reg8_shift_to_zero(C); } break;
				case SLA_D: { reg8_shift_to_zero(D); } break;
				case SLA_E: { reg8_shift_to_zero(E); } break;
				case SLA_H: { reg8_shift_to_zero(H); } break;
				case SLA_L: { reg8_shift_to_zero(L); } break;
				case SLA_PHL: { reg8_shift_to_zero(unknown8); } break;
				case SLA_PIX: { reg8_shift_to_zero(unknown8); } break;
				case SLA_PIY: { reg8_shift_to_zero(unknown8); } break;

				case SRA_A: { reg8_shift_to_zero(A); } break;
				case SRA_B: { reg8_shift_to_zero(B); } break;
				case SRA_C: { reg8_shift_to_zero(C); } break;
				case SRA_D: { reg8_shift_to_zero(D); } break;
				case SRA_E: { reg8_shift_to_zero(E); } break;
				case SRA_H: { reg8_shift_to_zero(H); } break;
				case SRA_L: { reg8_shift_to_zero(L); } break;
				case SRA_PHL: { reg8_shift_to_zero(unknown8); } break;
				case SRA_PIX: { reg8_shift_to_zero(unknown8); } break;
				case SRA_PIY: { reg8_shift_to_zero(unknown8); } break;

				case BIT_0_A: { A.bit_clear(0); } break;
				case BIT_0_B: { B.bit_clear(0); } break;
				case BIT_0_C: { C.bit_clear(0); } break;
				case BIT_0_D: { D.bit_clear(0); } break;
				case BIT_0_E: { E.bit_clear(0); } break;
				case BIT_0_H: { H.bit_clear(0); } break;
				case BIT_0_L: { L.bit_clear(0); } break;
				case BIT_1_A: { A.bit_clear(1); } break;
				case BIT_1_B: { B.bit_clear(1); } break;
				case BIT_1_C: { C.bit_clear(1); } break;
				case BIT_1_D: { D.bit_clear(1); } break;
				case BIT_1_E: { E.bit_clear(1); } break;
				case BIT_1_H: { H.bit_clear(1); } break;
				case BIT_1_L: { L.bit_clear(1); } break;
				case BIT_2_A: { A.bit_clear(2); } break;
				case BIT_2_B: { B.bit_clear(2); } break;
				case BIT_2_C: { C.bit_clear(2); } break;
				case BIT_2_D: { D.bit_clear(2); } break;
				case BIT_2_E: { E.bit_clear(2); } break;
				case BIT_2_H: { H.bit_clear(2); } break;
				case BIT_2_L: { L.bit_clear(2); } break;
				case BIT_3_A: { A.bit_clear(3); } break;
				case BIT_3_B: { B.bit_clear(3); } break;
				case BIT_3_C: { C.bit_clear(3); } break;
				case BIT_3_D: { D.bit_clear(3); } break;
				case BIT_3_E: { E.bit_clear(3); } break;
				case BIT_3_H: { H.bit_clear(3); } break;
				case BIT_3_L: { L.bit_clear(3); } break;
				case BIT_4_A: { A.bit_clear(4); } break;
				case BIT_4_B: { B.bit_clear(4); } break;
				case BIT_4_C: { C.bit_clear(4); } break;
				case BIT_4_D: { D.bit_clear(4); } break;
				case BIT_4_E: { E.bit_clear(4); } break;
				case BIT_4_H: { H.bit_clear(4); } break;
				case BIT_4_L: { L.bit_clear(4); } break;
				case BIT_5_A: { A.bit_clear(5); } break;
				case BIT_5_B: { B.bit_clear(5); } break;
				case BIT_5_C: { C.bit_clear(5); } break;
				case BIT_5_D: { D.bit_clear(5); } break;
				case BIT_5_E: { E.bit_clear(5); } break;
				case BIT_5_H: { H.bit_clear(5); } break;
				case BIT_5_L: { L.bit_clear(5); } break;
				case BIT_6_A: { A.bit_clear(6); } break;
				case BIT_6_B: { B.bit_clear(6); } break;
				case BIT_6_C: { C.bit_clear(6); } break;
				case BIT_6_D: { D.bit_clear(6); } break;
				case BIT_6_E: { E.bit_clear(6); } break;
				case BIT_6_H: { H.bit_clear(6); } break;
				case BIT_6_L: { L.bit_clear(6); } break;
				case BIT_7_A: { A.bit_clear(7); } break;
				case BIT_7_B: { B.bit_clear(7); } break;
				case BIT_7_C: { C.bit_clear(7); } break;
				case BIT_7_D: { D.bit_clear(7); } break;
				case BIT_7_E: { E.bit_clear(7); } break;
				case BIT_7_H: { H.bit_clear(7); } break;
				case BIT_7_L: { L.bit_clear(7); } break;
				default: break;
			}
		} break;

		case JP_P: {
			F.set_sign(true);
			switch (previous_instruction.op_code) {

				/* special cases */

				case SBC_A_A: { F.set_carry(true); sbc_a_a(); } break;
				case SBC_HL_HL: { F.set_carry(true); set_HL(sbc24_hl_hl(get_HL())); } break;
				case SBC_HL_HL_SIS: { F.set_carry(true); set16_zero_HL(sbc16_hl_hl(get16_HL())); } break;
				case NEG: { A.bit_set(7); acc_neg(); acc_neg(); } break;

				case OR_A_A :
				case AND_A_A:
				case TST_A_A:
				{ A.bit_set(7); reg8_bitwise_flag_set(A); } break;

				case ADD_A_A:
				case ADC_A_A:
				{ A.bit_set(7); F.set_zero(false); } break;

				case ADC_HL_HL    :
				{ UHL.bit_set(7); F.set_zero(false); } break;
				case ADC_HL_HL_SIS:
				{ H.bit_set(7); F.set_zero(false); } break;

				/* general cases */

				case ADD_A_B  :
				case ADD_A_C  :
				case ADD_A_D  :
				case ADD_A_E  :
				case ADD_A_H  :
				case ADD_A_L  :
				case ADD_A_IXH:
				case ADD_A_IXL:
				case ADD_A_IYH:
				case ADD_A_IYL:

				case ADC_A_B  :
				case ADC_A_C  :
				case ADC_A_D  :
				case ADC_A_E  :
				case ADC_A_H  :
				case ADC_A_L  :
				case ADC_A_IXH:
				case ADC_A_IXL:
				case ADC_A_IYH:
				case ADC_A_IYL:

				case SBC_A_B  :
				case SBC_A_C  :
				case SBC_A_D  :
				case SBC_A_E  :
				case SBC_A_H  :
				case SBC_A_L  :
				case SBC_A_IXH:
				case SBC_A_IXL:
				case SBC_A_IYH:
				case SBC_A_IYL:

				case SUB_A_B  :
				case SUB_A_C  :
				case SUB_A_D  :
				case SUB_A_E  :
				case SUB_A_H  :
				case SUB_A_L  :
				case SUB_A_IXH:
				case SUB_A_IXL:
				case SUB_A_IYH:
				case SUB_A_IYL:
				{ A.bit_set(7); F.set_zero(false); } break;

				case CP_A_B  :
				case CP_A_C  :
				case CP_A_D  :
				case CP_A_E  :
				case CP_A_H  :
				case CP_A_L  :
				case CP_A_IXH:
				case CP_A_IXL:
				case CP_A_IYH:
				case CP_A_IYL:
				{ F.set_zero(false); } break;

				case OR_A_B  :
				case OR_A_C  :
				case OR_A_D  :
				case OR_A_E  :
				case OR_A_H  :
				case OR_A_L  :
				case OR_A_IXH:
				case OR_A_IXL:
				case OR_A_IYH:
				case OR_A_IYL:

				{ A.bit_set(7); reg8_bitwise_flag_set(A); } break;

				case XOR_A_B  :
				case XOR_A_C  :
				case XOR_A_D  :
				case XOR_A_E  :
				case XOR_A_H  :
				case XOR_A_L  :
				case XOR_A_IXH:
				case XOR_A_IXL:
				case XOR_A_IYH:
				case XOR_A_IYL:

				{ A.bit_set(7); reg8_bitwise_flag_set(A); } break;

				case AND_A_B  : { A.bit_set(7); B  .bit_set(7); reg8_bitwise_flag_set(A); } break;
				case AND_A_C  : { A.bit_set(7); C  .bit_set(7); reg8_bitwise_flag_set(A); } break;
				case AND_A_D  : { A.bit_set(7); D  .bit_set(7); reg8_bitwise_flag_set(A); } break;
				case AND_A_E  : { A.bit_set(7); E  .bit_set(7); reg8_bitwise_flag_set(A); } break;
				case AND_A_H  : { A.bit_set(7); H  .bit_set(7); reg8_bitwise_flag_set(A); } break;
				case AND_A_L  : { A.bit_set(7); L  .bit_set(7); reg8_bitwise_flag_set(A); } break;
				case AND_A_IXH: { A.bit_set(7); IXH.bit_set(7); reg8_bitwise_flag_set(A); } break;
				case AND_A_IXL: { A.bit_set(7); IXL.bit_set(7); reg8_bitwise_flag_set(A); } break;
				case AND_A_IYH: { A.bit_set(7); IYH.bit_set(7); reg8_bitwise_flag_set(A); } break;
				case AND_A_IYL: { A.bit_set(7); IYL.bit_set(7); reg8_bitwise_flag_set(A); } break;

				case TST_A_B:
				case TST_A_C:
				case TST_A_D:
				case TST_A_E:
				case TST_A_H:
				case TST_A_L:
				{ F.set_zero(false); } break;

				case ADC_HL_BC    :
				case ADC_HL_DE    :
				case ADC_HL_SP    :
				{ UHL.bit_set(7); F.set_zero(false); } break;
				case ADC_HL_BC_SIS:
				case ADC_HL_DE_SIS:
				case ADC_HL_SP_SIS:
				{ H.bit_set(7); F.set_zero(false); } break;

				case SBC_HL_BC    :
				case SBC_HL_DE    :
				case SBC_HL_SP    :
				{ UHL.bit_set(7); F.set_zero(false); } break;
				case SBC_HL_BC_SIS:
				case SBC_HL_DE_SIS:
				case SBC_HL_SP_SIS:
				{ H.bit_set(7); F.set_zero(false); } break;

				case INC_A  : { A  .bit_set(7); reg8_inc_flag_set(A  ); } break;
				case INC_B  : { B  .bit_set(7); reg8_inc_flag_set(B  ); } break;
				case INC_C  : { C  .bit_set(7); reg8_inc_flag_set(C  ); } break;
				case INC_D  : { D  .bit_set(7); reg8_inc_flag_set(D  ); } break;
				case INC_E  : { E  .bit_set(7); reg8_inc_flag_set(E  ); } break;
				case INC_H  : { H  .bit_set(7); reg8_inc_flag_set(H  ); } break;
				case INC_L  : { L  .bit_set(7); reg8_inc_flag_set(L  ); } break;
				case INC_IXH: { IXH.bit_set(7); reg8_inc_flag_set(IXH); } break;
				case INC_IXL: { IXL.bit_set(7); reg8_inc_flag_set(IXL); } break;
				case INC_IYH: { IYH.bit_set(7); reg8_inc_flag_set(IYH); } break;
				case INC_IYL: { IYL.bit_set(7); reg8_inc_flag_set(IYL); } break;

				case DEC_A  : { A  .bit_set(7); reg8_dec_flag_set(A  ); } break;
				case DEC_B  : { B  .bit_set(7); reg8_dec_flag_set(B  ); } break;
				case DEC_C  : { C  .bit_set(7); reg8_dec_flag_set(C  ); } break;
				case DEC_D  : { D  .bit_set(7); reg8_dec_flag_set(D  ); } break;
				case DEC_E  : { E  .bit_set(7); reg8_dec_flag_set(E  ); } break;
				case DEC_H  : { H  .bit_set(7); reg8_dec_flag_set(H  ); } break;
				case DEC_L  : { L  .bit_set(7); reg8_dec_flag_set(L  ); } break;
				case DEC_IXH: { IXH.bit_set(7); reg8_dec_flag_set(IXH); } break;
				case DEC_IXL: { IXL.bit_set(7); reg8_dec_flag_set(IXL); } break;
				case DEC_IYH: { IYH.bit_set(7); reg8_dec_flag_set(IYH); } break;
				case DEC_IYL: { IYL.bit_set(7); reg8_dec_flag_set(IYL); } break;

				case RLC_A: { A.bit_set(7); reg8_shift_set_flags(A); } break;
				case RLC_B: { B.bit_set(7); reg8_shift_set_flags(B); } break;
				case RLC_C: { C.bit_set(7); reg8_shift_set_flags(C); } break;
				case RLC_D: { D.bit_set(7); reg8_shift_set_flags(D); } break;
				case RLC_E: { E.bit_set(7); reg8_shift_set_flags(E); } break;
				case RLC_H: { H.bit_set(7); reg8_shift_set_flags(H); } break;
				case RLC_L: { L.bit_set(7); reg8_shift_set_flags(L); } break;

				case RRC_A: { A.bit_set(7); reg8_shift_set_flags(A); F.set_carry(true); } break;
				case RRC_B: { B.bit_set(7); reg8_shift_set_flags(B); F.set_carry(true); } break;
				case RRC_C: { C.bit_set(7); reg8_shift_set_flags(C); F.set_carry(true); } break;
				case RRC_D: { D.bit_set(7); reg8_shift_set_flags(D); F.set_carry(true); } break;
				case RRC_E: { E.bit_set(7); reg8_shift_set_flags(E); F.set_carry(true); } break;
				case RRC_H: { H.bit_set(7); reg8_shift_set_flags(H); F.set_carry(true); } break;
				case RRC_L: { L.bit_set(7); reg8_shift_set_flags(L); F.set_carry(true); } break;

				case RL_A : { A.bit_set(7); reg8_shift_set_flags(A); } break;
				case RL_B : { B.bit_set(7); reg8_shift_set_flags(B); } break;
				case RL_C : { C.bit_set(7); reg8_shift_set_flags(C); } break;
				case RL_D : { D.bit_set(7); reg8_shift_set_flags(D); } break;
				case RL_E : { E.bit_set(7); reg8_shift_set_flags(E); } break;
				case RL_H : { H.bit_set(7); reg8_shift_set_flags(H); } break;
				case RL_L : { L.bit_set(7); reg8_shift_set_flags(L); } break;

				case RR_A : { A.bit_set(7); reg8_shift_set_flags(A); } break;
				case RR_B : { B.bit_set(7); reg8_shift_set_flags(B); } break;
				case RR_C : { C.bit_set(7); reg8_shift_set_flags(C); } break;
				case RR_D : { D.bit_set(7); reg8_shift_set_flags(D); } break;
				case RR_E : { E.bit_set(7); reg8_shift_set_flags(E); } break;
				case RR_H : { H.bit_set(7); reg8_shift_set_flags(H); } break;
				case RR_L : { L.bit_set(7); reg8_shift_set_flags(L); } break;

				case SLA_A: { A.bit_set(7); reg8_shift_set_flags(A); } break;
				case SLA_B: { B.bit_set(7); reg8_shift_set_flags(B); } break;
				case SLA_C: { C.bit_set(7); reg8_shift_set_flags(C); } break;
				case SLA_D: { D.bit_set(7); reg8_shift_set_flags(D); } break;
				case SLA_E: { E.bit_set(7); reg8_shift_set_flags(E); } break;
				case SLA_H: { H.bit_set(7); reg8_shift_set_flags(H); } break;
				case SLA_L: { L.bit_set(7); reg8_shift_set_flags(L); } break;

				case SRA_A: { A.bit_set(7); A.bit_set(6); reg8_shift_set_flags(A); } break;
				case SRA_B: { B.bit_set(7); B.bit_set(6); reg8_shift_set_flags(B); } break;
				case SRA_C: { C.bit_set(7); C.bit_set(6); reg8_shift_set_flags(C); } break;
				case SRA_D: { D.bit_set(7); D.bit_set(6); reg8_shift_set_flags(D); } break;
				case SRA_E: { E.bit_set(7); E.bit_set(6); reg8_shift_set_flags(E); } break;
				case SRA_H: { H.bit_set(7); H.bit_set(6); reg8_shift_set_flags(H); } break;
				case SRA_L: { L.bit_set(7); L.bit_set(6); reg8_shift_set_flags(L); } break;

				case RRC_PHL:
				case RRC_PIX:
				case RRC_PIY:
				{ F.set_zero(false); F.set_carry(true); } break;

				case ADD_A_PHL:
				case ADD_A_PIX:
				case ADD_A_PIY:
				case ADC_A_PHL:
				case ADC_A_PIX:
				case ADC_A_PIY:
				case SBC_A_PHL:
				case SBC_A_PIX:
				case SBC_A_PIY:
				case SUB_A_PHL:
				case SUB_A_PIX:
				case SUB_A_PIY:
				case OR_A_PHL :
				case OR_A_PIX :
				case OR_A_PIY :
				case XOR_A_PHL:
				case XOR_A_PIX:
				case XOR_A_PIY:
				case AND_A_PHL:
				case AND_A_PIX:
				case AND_A_PIY:
				{ A.bit_set(7); reg8_bitwise_flag_set(A); } break;

				case CP_A_PHL :
				case CP_A_PIX :
				case CP_A_PIY :
				case TST_A_PHL:
				case INC_PHL:
				case INC_PIX:
				case INC_PIY:
				case DEC_PHL:
				case DEC_PIX:
				case DEC_PIY:
				case RLC_PHL:
				case RLC_PIX:
				case RLC_PIY:
				case SRA_PHL:
				case SRA_PIX:
				case SRA_PIY:
				case SLA_PHL:
				case SLA_PIX:
				case SLA_PIY:
				case SRL_PHL:
				case SRL_PIX:
				case SRL_PIY:
				case RR_PHL :
				case RR_PIX :
				case RR_PIY :
				case RL_PHL :
				case RL_PIX :
				case RL_PIY :
				{ F.set_zero(false); } break;

				default: break;
			}
		} break;
		case JP_M: {
			F.set_sign(false);
			switch (previous_instruction.op_code) {

				/* special cases */

				case SBC_A_A: { F.set_carry(false); sbc_a_a(); } break;
				case SBC_HL_HL: { F.set_carry(false); set_HL(sbc24_hl_hl(get_HL())); } break;
				case SBC_HL_HL_SIS: { F.set_carry(false); set16_zero_HL(sbc16_hl_hl(get16_HL())); } break;
				case NEG: { A.bit_clear(7); acc_neg(); acc_neg(); } break;

				case OR_A_A :
				case AND_A_A:
				case TST_A_A:
				{ A.bit_clear(7); reg8_bitwise_flag_set(A); } break;

				case ADD_A_A:
				case ADC_A_A:
				{ A.bit_clear(7); F.set_zero(A.is_zero()); } break;

				case ADC_HL_HL    :
				{ UHL.bit_clear(7); F.set_zero(get_HL().is_zero()); } break;
				case ADC_HL_HL_SIS:
				{ H.bit_clear(7); F.set_zero(get_HL().is_zero()); } break;

				/* general cases */

				case ADD_A_B  :
				case ADD_A_C  :
				case ADD_A_D  :
				case ADD_A_E  :
				case ADD_A_H  :
				case ADD_A_L  :
				case ADD_A_IXH:
				case ADD_A_IXL:
				case ADD_A_IYH:
				case ADD_A_IYL:

				case ADC_A_B  :
				case ADC_A_C  :
				case ADC_A_D  :
				case ADC_A_E  :
				case ADC_A_H  :
				case ADC_A_L  :
				case ADC_A_IXH:
				case ADC_A_IXL:
				case ADC_A_IYH:
				case ADC_A_IYL:

				case SBC_A_B  :
				case SBC_A_C  :
				case SBC_A_D  :
				case SBC_A_E  :
				case SBC_A_H  :
				case SBC_A_L  :
				case SBC_A_IXH:
				case SBC_A_IXL:
				case SBC_A_IYH:
				case SBC_A_IYL:

				case SUB_A_B  :
				case SUB_A_C  :
				case SUB_A_D  :
				case SUB_A_E  :
				case SUB_A_H  :
				case SUB_A_L  :
				case SUB_A_IXH:
				case SUB_A_IXL:
				case SUB_A_IYH:
				case SUB_A_IYL:
				{ A.bit_clear(7); F.set_zero(A.is_zero()); } break;

				case OR_A_B  : { A.bit_clear(7); B  .bit_clear(7); reg8_bitwise_flag_set(A); } break;
				case OR_A_C  : { A.bit_clear(7); C  .bit_clear(7); reg8_bitwise_flag_set(A); } break;
				case OR_A_D  : { A.bit_clear(7); D  .bit_clear(7); reg8_bitwise_flag_set(A); } break;
				case OR_A_E  : { A.bit_clear(7); E  .bit_clear(7); reg8_bitwise_flag_set(A); } break;
				case OR_A_H  : { A.bit_clear(7); H  .bit_clear(7); reg8_bitwise_flag_set(A); } break;
				case OR_A_L  : { A.bit_clear(7); L  .bit_clear(7); reg8_bitwise_flag_set(A); } break;
				case OR_A_IXH: { A.bit_clear(7); IXH.bit_clear(7); reg8_bitwise_flag_set(A); } break;
				case OR_A_IXL: { A.bit_clear(7); IXL.bit_clear(7); reg8_bitwise_flag_set(A); } break;
				case OR_A_IYH: { A.bit_clear(7); IYH.bit_clear(7); reg8_bitwise_flag_set(A); } break;
				case OR_A_IYL: { A.bit_clear(7); IYL.bit_clear(7); reg8_bitwise_flag_set(A); } break;

				case XOR_A_B  :
				case XOR_A_C  :
				case XOR_A_D  :
				case XOR_A_E  :
				case XOR_A_H  :
				case XOR_A_L  :
				case XOR_A_IXH:
				case XOR_A_IXL:
				case XOR_A_IYH:
				case XOR_A_IYL:

				{ A.bit_clear(7); reg8_bitwise_flag_set(A); } break;

				case AND_A_B  :
				case AND_A_C  :
				case AND_A_D  :
				case AND_A_E  :
				case AND_A_H  :
				case AND_A_L  :
				case AND_A_IXH:
				case AND_A_IXL:
				case AND_A_IYH:
				case AND_A_IYL:

				{ A.bit_clear(7); reg8_bitwise_flag_set(A); } break;

				case ADC_HL_BC    :
				case ADC_HL_DE    :
				case ADC_HL_SP    :
				{ UHL.bit_clear(7); F.set_zero(get_HL().is_zero()); } break;
				case ADC_HL_BC_SIS:
				case ADC_HL_DE_SIS:
				case ADC_HL_SP_SIS:
				{ H.bit_clear(7); F.set_zero(get_HL().is_zero()); } break;

				case SBC_HL_BC    :
				case SBC_HL_DE    :
				case SBC_HL_SP    :
				{ UHL.bit_clear(7); F.set_zero(get_HL().is_zero()); } break;
				case SBC_HL_BC_SIS:
				case SBC_HL_DE_SIS:
				case SBC_HL_SP_SIS:
				{ H.bit_clear(7); F.set_zero(get_HL().is_zero()); } break;

				case INC_A  : { A  .bit_clear(7); reg8_inc_flag_set(A  ); } break;
				case INC_B  : { B  .bit_clear(7); reg8_inc_flag_set(B  ); } break;
				case INC_C  : { C  .bit_clear(7); reg8_inc_flag_set(C  ); } break;
				case INC_D  : { D  .bit_clear(7); reg8_inc_flag_set(D  ); } break;
				case INC_E  : { E  .bit_clear(7); reg8_inc_flag_set(E  ); } break;
				case INC_H  : { H  .bit_clear(7); reg8_inc_flag_set(H  ); } break;
				case INC_L  : { L  .bit_clear(7); reg8_inc_flag_set(L  ); } break;
				case INC_IXH: { IXH.bit_clear(7); reg8_inc_flag_set(IXH); } break;
				case INC_IXL: { IXL.bit_clear(7); reg8_inc_flag_set(IXL); } break;
				case INC_IYH: { IYH.bit_clear(7); reg8_inc_flag_set(IYH); } break;
				case INC_IYL: { IYL.bit_clear(7); reg8_inc_flag_set(IYL); } break;

				case DEC_A  : { A  .bit_clear(7); reg8_dec_flag_set(A  ); } break;
				case DEC_B  : { B  .bit_clear(7); reg8_dec_flag_set(B  ); } break;
				case DEC_C  : { C  .bit_clear(7); reg8_dec_flag_set(C  ); } break;
				case DEC_D  : { D  .bit_clear(7); reg8_dec_flag_set(D  ); } break;
				case DEC_E  : { E  .bit_clear(7); reg8_dec_flag_set(E  ); } break;
				case DEC_H  : { H  .bit_clear(7); reg8_dec_flag_set(H  ); } break;
				case DEC_L  : { L  .bit_clear(7); reg8_dec_flag_set(L  ); } break;
				case DEC_IXH: { IXH.bit_clear(7); reg8_dec_flag_set(IXH); } break;
				case DEC_IXL: { IXL.bit_clear(7); reg8_dec_flag_set(IXL); } break;
				case DEC_IYH: { IYH.bit_clear(7); reg8_dec_flag_set(IYH); } break;
				case DEC_IYL: { IYL.bit_clear(7); reg8_dec_flag_set(IYL); } break;

				case RLC_A: { A.bit_clear(7); reg8_shift_set_flags(A); } break;
				case RLC_B: { B.bit_clear(7); reg8_shift_set_flags(B); } break;
				case RLC_C: { C.bit_clear(7); reg8_shift_set_flags(C); } break;
				case RLC_D: { D.bit_clear(7); reg8_shift_set_flags(D); } break;
				case RLC_E: { E.bit_clear(7); reg8_shift_set_flags(E); } break;
				case RLC_H: { H.bit_clear(7); reg8_shift_set_flags(H); } break;
				case RLC_L: { L.bit_clear(7); reg8_shift_set_flags(L); } break;

				case RRC_A: { A.bit_clear(7); reg8_shift_set_flags(A); F.set_carry(false); } break;
				case RRC_B: { B.bit_clear(7); reg8_shift_set_flags(B); F.set_carry(false); } break;
				case RRC_C: { C.bit_clear(7); reg8_shift_set_flags(C); F.set_carry(false); } break;
				case RRC_D: { D.bit_clear(7); reg8_shift_set_flags(D); F.set_carry(false); } break;
				case RRC_E: { E.bit_clear(7); reg8_shift_set_flags(E); F.set_carry(false); } break;
				case RRC_H: { H.bit_clear(7); reg8_shift_set_flags(H); F.set_carry(false); } break;
				case RRC_L: { L.bit_clear(7); reg8_shift_set_flags(L); F.set_carry(false); } break;

				case RL_A : { A.bit_clear(7); reg8_shift_set_flags(A); } break;
				case RL_B : { B.bit_clear(7); reg8_shift_set_flags(B); } break;
				case RL_C : { C.bit_clear(7); reg8_shift_set_flags(C); } break;
				case RL_D : { D.bit_clear(7); reg8_shift_set_flags(D); } break;
				case RL_E : { E.bit_clear(7); reg8_shift_set_flags(E); } break;
				case RL_H : { H.bit_clear(7); reg8_shift_set_flags(H); } break;
				case RL_L : { L.bit_clear(7); reg8_shift_set_flags(L); } break;

				case RR_A : { A.bit_clear(7); reg8_shift_set_flags(A); } break;
				case RR_B : { B.bit_clear(7); reg8_shift_set_flags(B); } break;
				case RR_C : { C.bit_clear(7); reg8_shift_set_flags(C); } break;
				case RR_D : { D.bit_clear(7); reg8_shift_set_flags(D); } break;
				case RR_E : { E.bit_clear(7); reg8_shift_set_flags(E); } break;
				case RR_H : { H.bit_clear(7); reg8_shift_set_flags(H); } break;
				case RR_L : { L.bit_clear(7); reg8_shift_set_flags(L); } break;

				case SLA_A: { A.bit_clear(7); reg8_shift_set_flags(A); } break;
				case SLA_B: { B.bit_clear(7); reg8_shift_set_flags(B); } break;
				case SLA_C: { C.bit_clear(7); reg8_shift_set_flags(C); } break;
				case SLA_D: { D.bit_clear(7); reg8_shift_set_flags(D); } break;
				case SLA_E: { E.bit_clear(7); reg8_shift_set_flags(E); } break;
				case SLA_H: { H.bit_clear(7); reg8_shift_set_flags(H); } break;
				case SLA_L: { L.bit_clear(7); reg8_shift_set_flags(L); } break;

				case SRA_A: { A.bit_clear(7); A.bit_clear(6); reg8_shift_set_flags(A); } break;
				case SRA_B: { B.bit_clear(7); B.bit_clear(6); reg8_shift_set_flags(B); } break;
				case SRA_C: { C.bit_clear(7); C.bit_clear(6); reg8_shift_set_flags(C); } break;
				case SRA_D: { D.bit_clear(7); D.bit_clear(6); reg8_shift_set_flags(D); } break;
				case SRA_E: { E.bit_clear(7); E.bit_clear(6); reg8_shift_set_flags(E); } break;
				case SRA_H: { H.bit_clear(7); H.bit_clear(6); reg8_shift_set_flags(H); } break;
				case SRA_L: { L.bit_clear(7); L.bit_clear(6); reg8_shift_set_flags(L); } break;

				case RRC_PHL:
				case RRC_PIX:
				case RRC_PIY:
				{ F.set_carry(false); } break;

				case ADD_A_PHL:
				case ADD_A_PIX:
				case ADD_A_PIY:
				case ADC_A_PHL:
				case ADC_A_PIX:
				case ADC_A_PIY:
				case SBC_A_PHL:
				case SBC_A_PIX:
				case SBC_A_PIY:
				case SUB_A_PHL:
				case SUB_A_PIX:
				case SUB_A_PIY:
				case OR_A_PHL :
				case OR_A_PIX :
				case OR_A_PIY :
				case XOR_A_PHL:
				case XOR_A_PIX:
				case XOR_A_PIY:
				case AND_A_PHL:
				case AND_A_PIX:
				case AND_A_PIY:
				{ A.bit_clear(7); reg8_bitwise_flag_set(A); } break;

				default: break;
			}
		} break;

		case JP_PO: {
			F.set_overflow(true);
			switch (previous_instruction.op_code) {
				case INC_A  : { A  .set_value(0x80); reg8_inc_flag_set(A  ); } break;
				case INC_B  : { B  .set_value(0x80); reg8_inc_flag_set(B  ); } break;
				case INC_C  : { C  .set_value(0x80); reg8_inc_flag_set(C  ); } break;
				case INC_D  : { D  .set_value(0x80); reg8_inc_flag_set(D  ); } break;
				case INC_E  : { E  .set_value(0x80); reg8_inc_flag_set(E  ); } break;
				case INC_H  : { H  .set_value(0x80); reg8_inc_flag_set(H  ); } break;
				case INC_L  : { L  .set_value(0x80); reg8_inc_flag_set(L  ); } break;
				case INC_IXH: { IXH.set_value(0x80); reg8_inc_flag_set(IXH); } break;
				case INC_IXL: { IXL.set_value(0x80); reg8_inc_flag_set(IXL); } break;
				case INC_IYH: { IYH.set_value(0x80); reg8_inc_flag_set(IYH); } break;
				case INC_IYL: { IYL.set_value(0x80); reg8_inc_flag_set(IYL); } break;

				case DEC_A  : { A  .set_value(0x7F); reg8_dec_flag_set(A  ); } break;
				case DEC_B  : { B  .set_value(0x7F); reg8_dec_flag_set(B  ); } break;
				case DEC_C  : { C  .set_value(0x7F); reg8_dec_flag_set(C  ); } break;
				case DEC_D  : { D  .set_value(0x7F); reg8_dec_flag_set(D  ); } break;
				case DEC_E  : { E  .set_value(0x7F); reg8_dec_flag_set(E  ); } break;
				case DEC_H  : { H  .set_value(0x7F); reg8_dec_flag_set(H  ); } break;
				case DEC_L  : { L  .set_value(0x7F); reg8_dec_flag_set(L  ); } break;
				case DEC_IXH: { IXH.set_value(0x7F); reg8_dec_flag_set(IXH); } break;
				case DEC_IXL: { IXL.set_value(0x7F); reg8_dec_flag_set(IXL); } break;
				case DEC_IYH: { IYH.set_value(0x7F); reg8_dec_flag_set(IYH); } break;
				case DEC_IYL: { IYL.set_value(0x7F); reg8_dec_flag_set(IYL); } break;
				default: break;
			}
		} break;
		case JP_PE: {
			F.set_overflow(false);
			switch (previous_instruction.op_code) {
				case CPI:
				case CPD:
				case LDI:
				case LDD:
				{ set_BC(0); } break;
				default: break;
			}
		} break;
		default: break;
		/* won't touch registers or flags */
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
		case LD_ADDR_A:
		case LD_PBC_A:
		case LD_PDE_A:
		case LD_PHL_A:
		case LD_PHL_N:
		case LD_PHL_B:
		case LD_PHL_C:
		case LD_PHL_D:
		case LD_PHL_E:
		case LD_PHL_H:
		case LD_PHL_L:
		case LD_PIX_N:
		case LD_PIX_A:
		case LD_PIX_B:
		case LD_PIX_C:
		case LD_PIX_D:
		case LD_PIX_E:
		case LD_PIX_H:
		case LD_PIX_L:
		case LD_PIY_N:
		case LD_PIY_A:
		case LD_PIY_B:
		case LD_PIY_C:
		case LD_PIY_D:
		case LD_PIY_E:
		case LD_PIY_H:
		case LD_PIY_L:
		case LD_PHL_BC:
		case LD_PHL_DE:
		case LD_PHL_HL:
		case LD_PHL_IX:
		case LD_PHL_IY:
		case LD_PIX_BC:
		case LD_PIX_DE:
		case LD_PIX_HL:
		case LD_PIX_IX:
		case LD_PIX_IY:
		case LD_PIY_BC:
		case LD_PIY_DE:
		case LD_PIY_HL:
		case LD_PIY_IX:
		case LD_PIY_IY:
		case LD_ADDR_HL:
		case LD_ADDR_BC:
		case LD_ADDR_DE:
		case LD_ADDR_IX:
		case LD_ADDR_IY:
		case LD_ADDR_SP:
		case PEA_IX:
		case PEA_IY:
		case PUSH_AF:
		case PUSH_BC:
		case PUSH_DE:
		case PUSH_HL:
		case PUSH_IX:
		case PUSH_IY:
		/* NOPS */
		case NOP:
		case LD_A_A:
		case LD_H_H:
		case LD_L_L:
		case LD_IXL_IXL:
		case LD_IXH_IXH:
		case LD_IYL_IYL:
		case LD_IYH_IYH:
		case COUNT:
			return;
	}
finish:
	set_previous_instruction(current_instruction);
	return;
}
