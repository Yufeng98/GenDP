#include "alu_32.h"

class compute_unit_32 {

	public:

		compute_unit_32();
		~compute_unit_32();

		int execute(int* op, int* input);
		int execute_8bit(int* op, int* input);

	private:

		int match_score_poa[5][5] = {{7, -1,  0,  0, -3},
									{-1, 13, -6, -2, -5},
									{ 0, -6,  9, -3, -1},
									{ 0, -2, -3,  8,  0},
									{-3, -5, -1,  0,  9}};

		int match_score_phmm[5][5] = {{1,-1, -1, -1,  1},
									{-1,  1, -1, -1,  1},
									{-1, -1,  1, -1,  1},
									{-1, -1, -1,  1,  1},
									{ 1,  1,  1,  1,  1}};

		int8_t match_score_bsw_scalar[5][5] = {{1, -4, -4, -4, -1},
									{-4,  1, -4, -4, -1},
									{-4, -4,  1, -4, -1},
									{-4, -4, -4,  1, -1},
									{-1, -1, -1, -1, -1}};

		int QUAL2PROB_TABLE[256];
		int QUAL2ERROR_DIV3_TABLE[256];

		void LUT_init();
		// int8_t match_score_bsw_simd[5][5][SIMD_WIDTH8];
		// int match_score_bsw_32[5][5];

		alu_32 alu;
		
		int8_t input_0_simd[SIMD_WIDTH8], input_1_simd[SIMD_WIDTH8], input_2_simd[SIMD_WIDTH8], input_3_simd[SIMD_WIDTH8], input_4_simd[SIMD_WIDTH8], input_5_simd[SIMD_WIDTH8], alu_out_1_simd[SIMD_WIDTH8], output_simd[SIMD_WIDTH8];

};