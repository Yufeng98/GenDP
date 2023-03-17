#include "comp_decoder.h"

comp_decoder::comp_decoder() {}
comp_decoder::~comp_decoder() {}

void comp_decoder::execute(long instruction, int* op, int* in_addr, int* out_addr, int* PC) {

    // instruction: op[0] op[1] op[2] in_addr[0] in_addr[1] in_addr[2] in_addr[3] in_addr[4] in_addr[5] out_addr

    int i;
    unsigned long out_addr_mask, in_addr_mask[6], op_mask[3];

    out_addr_mask = (1 << REGFILE_ADDR_WIDTH) - 1;
    *out_addr = (int)(out_addr_mask & instruction);

    for (i = 0; i < 6; i++) {
        in_addr_mask[i] = (unsigned long)((1 << REGFILE_ADDR_WIDTH) - 1) << (6 - i) * REGFILE_ADDR_WIDTH;
        // printf("%d %lx %lx\n", i, in_addr_mask[i], instruction);
        in_addr[i] = (in_addr_mask[i] & instruction) >> (6 - i) * REGFILE_ADDR_WIDTH;
    }

    for (i = 0; i < 3; i++) {
        op_mask[i] = (unsigned long)((1 << COMP_OPCODE_WIDTH) - 1) << (7 * REGFILE_ADDR_WIDTH + (2 - i) * COMP_OPCODE_WIDTH);
        op[i] = (op_mask[i] & instruction) >> (7 * REGFILE_ADDR_WIDTH + (2 - i) * COMP_OPCODE_WIDTH);
    }

    if (op[0] < HALT) (*PC)++;
    else if (op[0] == HALT) (*PC) = (*PC);
    else {
        fprintf(stderr, "compute opcode %d error.\n", op[0]);
        exit(-1);
    }

    // printf("%d %d %d %d %d %d %d %d %d %d\t", op[0], op[1], op[2], in_addr[0], in_addr[1], in_addr[2], in_addr[3], in_addr[4], in_addr[5], *out_addr);
#ifdef PROFILE
    printf("%d %d %d %d %d %d %d\t", in_addr[0], in_addr[1], in_addr[2], in_addr[3], in_addr[4], in_addr[5], *out_addr);
#endif
}