#include "sys_def.h"

class alu_32 {

	public:

		alu_32();
		~alu_32();

        int execute(int input_0, int input_1, int op);
        int execute_8bit(int input_0, int input_1, int op);

        int execute_4input(int input_0, int input_1, int input_2, int input_3, int op);
        int execute_4input_8bit(int input_0, int input_1, int input_2, int input_3, int op);

	private:

        char LogTable256[256] = {
			#define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
			0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
			LT(4), LT(5), LT(5), LT(6), LT(6), LT(6), LT(6),
			LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7)
		};

		int ilog2_32(unsigned int v);

        int logSumTable[17];

        void logSumTable_init();

		int8_t input_0_simd[SIMD_WIDTH8], input_1_simd[SIMD_WIDTH8], input_3_simd[SIMD_WIDTH8], input_2_simd[SIMD_WIDTH8], output_simd[SIMD_WIDTH8];
};