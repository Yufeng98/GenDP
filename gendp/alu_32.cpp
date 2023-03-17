#include "alu_32.h"

alu_32::alu_32() {
    
    logSumTable_init();

}

alu_32::~alu_32() {}

int alu_32::execute_8bit(int input_0, int input_1, int op) {

	// int8_t *input_0_simd, *input_1_simd, *output_simd;
	// input_0_simd = (int8_t*) malloc (SIMD_WIDTH8 * sizeof(int8_t));
	// input_1_simd = (int8_t*) malloc (SIMD_WIDTH8 * sizeof(int8_t));
	// output_simd = (int8_t*) malloc (SIMD_WIDTH8 * sizeof(int8_t));
	// int8_t input_0_simd[SIMD_WIDTH8], input_1_simd[SIMD_WIDTH8], output_simd[SIMD_WIDTH8];
	int zero = 0;
	memcpy(output_simd, &zero, SIMD_WIDTH8 * sizeof(int8_t));
	memcpy(input_0_simd, &input_0, SIMD_WIDTH8 * sizeof(int8_t));
	memcpy(input_1_simd, &input_1, SIMD_WIDTH8 * sizeof(int8_t));
	int i, output;

	switch(op) {
		case ADDITION: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = input_0_simd[i] + input_1_simd[i];
			break;
		}
		case SUBTRACTION: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = input_0_simd[i] - input_1_simd[i];
			break;
		}
		case MAXIMUM: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = (input_0_simd[i] - input_1_simd[i]) >= 0 ? input_0_simd[i] : input_1_simd[i];
			break;
		}
		case MINIMUM: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = (input_0_simd[i] - input_1_simd[i]) >= 0 ? input_1_simd[i] : input_0_simd[i];
			break;
		}
		case COPY: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = input_0_simd[i];
			break;
		}
		case INVALID: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = 0;
			break;
		}
	}
	memcpy(&output, output_simd, SIMD_WIDTH8 * sizeof(int8_t));
	return output;
}

int alu_32::execute(int input_0, int input_1, int op) {
	int out, log_sum_in, tmp_shift, tmp_minus;
	long tmp, in_0_tmp, in_1_tmp, sum; 
	if (input_0 < 0) in_0_tmp = pow(2, 32) + input_0;
	else in_0_tmp = input_0;
	if (input_1 < 0) in_1_tmp = pow(2, 32) + input_1;
	else in_1_tmp = input_1;
	
	switch(op){
		case ADDITION: {
			out = input_0 + input_1;
			break;
		}
		case SUBTRACTION: {
			out = input_0 - input_1;
			break;
		}
		case CARRY: {
			sum = in_0_tmp + in_1_tmp;
			if (sum >= pow(2, 32)) out = 1;
			else out = 0;
			break;
		}
		case BORROW: {
			// out = (input_0 >= input_1) ? 0 : 1;
			out = (input_0 - input_1) >= 0 ? 0 : 1;
			break;
		}
		case MAXIMUM: {
			// out = std::max(input_0, input_1);
			out = (input_0 - input_1) >= 0 ? input_0 : input_1;
			break;
		}
		case MINIMUM: {
			// out = std::min(input_0, input_1);
			out = (input_0 - input_1) >= 0 ? input_1 : input_0;
			break;
		}
		case LEFT_SHIFT: {
			if (input_0 < 0) tmp = pow(2, 32) + input_0;
			else tmp = input_0;
			out = tmp << 16;
			break;
		}
		case RIGHT_SHIFT: {
			if (input_0 < 0) tmp = pow(2, 32) + input_0;
			else tmp = input_0;
			out = tmp >> 16;
			break;
		}
		case COPY: {
			out = input_0;
			break;
		}
		case LOG2_LUT: {
			out = ilog2_32(input_0) >> 1;
			break;
		}
		case LOG_SUM_LUT: {
			tmp_minus = -input_0;
			tmp_shift = input_0 < 0 ? (tmp_minus >> 16) : (input_0 >> 16);
			log_sum_in = tmp_shift > 16 ? 16 : tmp_shift;

			if (log_sum_in % 32 < 0) tmp = 32 + log_sum_in % 32;
			else tmp = log_sum_in % 32;
			if (tmp >= 16) out = logSumTable[16];
			else out = logSumTable[tmp];
			break;
		}
        case INVALID: {
			out = 0;
			break;
		}
		default: {
			out = 0;
			break;
		}
	}
	return out;
}


int alu_32::execute_4input_8bit(int input_0, int input_1, int input_2, int input_3, int op) {

	// int8_t *input_0_simd, *input_1_simd, *input_2_simd, *input_3_simd, *output_simd;
	// input_0_simd = (int8_t*) malloc (SIMD_WIDTH8 * sizeof(int8_t));
	// input_1_simd = (int8_t*) malloc (SIMD_WIDTH8 * sizeof(int8_t));
	// input_2_simd = (int8_t*) malloc (SIMD_WIDTH8 * sizeof(int8_t));
	// input_3_simd = (int8_t*) malloc (SIMD_WIDTH8 * sizeof(int8_t));
	// output_simd = (int8_t*) malloc (SIMD_WIDTH8 * sizeof(int8_t));
	// int8_t input_0_simd[SIMD_WIDTH8], input_1_simd[SIMD_WIDTH8], input_3_simd[SIMD_WIDTH8], input_2_simd[SIMD_WIDTH8], output_simd[SIMD_WIDTH8];
	int zero = 0;
	memcpy(output_simd, &zero, SIMD_WIDTH8 * sizeof(int8_t));
	memcpy(input_0_simd, &input_0, SIMD_WIDTH8 * sizeof(int8_t));
	memcpy(input_1_simd, &input_1, SIMD_WIDTH8 * sizeof(int8_t));
	memcpy(input_2_simd, &input_2, SIMD_WIDTH8 * sizeof(int8_t));
	memcpy(input_3_simd, &input_3, SIMD_WIDTH8 * sizeof(int8_t));
	int i, output;

	switch(op) {
		case ADDITION: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = input_0_simd[i] + input_1_simd[i];
			break;
		}
		case SUBTRACTION: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = input_0_simd[i] - input_1_simd[i];
			break;
		}
		case MAXIMUM: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = (input_0_simd[i] - input_1_simd[i]) >= 0 ? input_0_simd[i] : input_1_simd[i];
			break;
		}
		case MINIMUM: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = (input_0_simd[i] - input_1_simd[i]) >= 0 ? input_1_simd[i] : input_0_simd[i];
			break;
		}
		case COPY: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = input_0_simd[i];
			break;
		}
		case COMP_LARGER: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = (input_0_simd[i] > input_1_simd[i]) ? input_2_simd[i] : input_3_simd[i];
			break;
		}
		case COMP_EQUAL: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = (input_0_simd[i] == input_1_simd[i]) ? input_2_simd[i] : input_3_simd[i];
			break;
		}
		case INVALID: {
			for (i=0; i<SIMD_WIDTH8; i++)
				output_simd[i] = 0;
			break;
		}
	}
	memcpy(&output, output_simd, SIMD_WIDTH8 * sizeof(int8_t));
	return output;
}


int alu_32::execute_4input(int input_0, int input_1, int input_2, int input_3, int op) {
	int out, tmp_minus, tmp_shift, log_sum_in;
	long tmp, in_0_tmp, in_1_tmp, sum; 
	if (input_0 < 0) in_0_tmp = pow(2, 32) + input_0;
	else in_0_tmp = input_0;
	if (input_1 < 0) in_1_tmp = pow(2, 32) + input_1;
	else in_1_tmp = input_1;

	switch(op){
		case ADDITION: {
			out = input_0 + input_1;
			break;
		}
		case SUBTRACTION: {
			out = input_0 - input_1;
			break;
		}
		case CARRY: {
			sum = in_0_tmp + in_1_tmp;
			if (sum >= pow(2, 32)) out = 1;
			else out = 0;
			break;
		}
		case BORROW: {
			out = (input_0 > input_1) ? 0 : 1;
			break;
		}
		case MAXIMUM: {
			out = (input_0 - input_1) > 0 ? input_0 : input_1;
			break;
		}
		case MINIMUM: {
			out = (input_0 - input_1) > 0 ? input_1 : input_0;
			break;
		}
		case LEFT_SHIFT: {
			if (input_0 < 0) tmp = pow(2, 32) + input_0;
			else tmp = input_0;
			out = tmp << 16;
			break;
		}
		case RIGHT_SHIFT: {
			if (input_0 < 0) tmp = pow(2, 32) + input_0;
			else tmp = input_0;
			out = tmp >> 16;
			break;
		}
		case COPY: {
			out = input_0;
			break;
		}
		case LOG2_LUT: {
			out = ilog2_32(input_0) >> 1;
			break;
		}
		case LOG_SUM_LUT: {
			tmp_minus = -input_0;
			tmp_shift = input_0 < 0 ? (tmp_minus >> 16) : (input_0 >> 16);
			log_sum_in = tmp_shift > 16 ? 16 : tmp_shift;

			if (log_sum_in % 32 < 0) tmp = 32 + log_sum_in % 32;
			else tmp = log_sum_in % 32;
			if (tmp >= 16) out = logSumTable[16];
			else out = logSumTable[tmp];
			break;
		}
		case COMP_LARGER: {
			out = (input_0 > input_1) ? input_2 : input_3;
			break;
		}
		case COMP_EQUAL: {
			out = (input_0 == input_1) ? input_2 : input_3;
			break;
		}
		case INVALID: {
			out = 0;
			break;
		}
		default: {
			out = 0;
			break;
		}
	}
	return out;
}

void alu_32::logSumTable_init() {
    logSumTable[0] = 0b00000000000000010000000000000000;
    logSumTable[1] = 0b00000000000000001001010111000001;
    logSumTable[2] = 0b00000000000000000101001001101010;
    logSumTable[3] = 0b00000000000000000010101110000001;
    logSumTable[4] = 0b00000000000000000001011001100100;
    logSumTable[5] = 0b00000000000000000000101101011110;
    logSumTable[6] = 0b00000000000000000000010110111010;
    logSumTable[7] = 0b00000000000000000000001011100000;
    logSumTable[8] = 0b00000000000000000000000101110001;
    logSumTable[9] = 0b00000000000000000000000010111001;
    logSumTable[10] = 0b00000000000000000000000001011101;
    logSumTable[11] = 0b00000000000000000000000000101111;
    logSumTable[12] = 0b00000000000000000000000000011000;
    logSumTable[13] = 0b00000000000000000000000000001100;
    logSumTable[14] = 0b00000000000000000000000000000110;
    logSumTable[15] = 0b00000000000000000000000000000011;
    logSumTable[16] = 0b00000000000000000000000000000010;
}

int alu_32::ilog2_32(unsigned int v) {
    unsigned int t, tt, log2;
    if ((tt = v>>16)) log2 = (t = tt>>8) ? 24 + LogTable256[t] : 16 + LogTable256[tt];
    log2 = (t = v>>8) ? 8 + LogTable256[t] : LogTable256[v];
	return log2;
}