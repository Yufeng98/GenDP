#include "pe_array.h"

#define NUM_FRACTION_BITS 16
#define MAX_RANGE NUM_FRACTION_BITS
#define NUM_INTEGER_BITS 5

pe_array::pe_array(int input_size, int output_size) {

    int i;
    input_buffer_size = input_size;
    output_buffer_size = output_size;

    input_buffer = (int*)calloc(input_buffer_size, sizeof(int));
    output_buffer = (int*)calloc(output_buffer_size, sizeof(int));

    main_addressing_register[0] = 0;
    main_PC = 0;
    for (i = 0; i < PE_NUM; i++)
        pe_unit[i] = new pe(i);
    load_data = 0;
    store_data = 0;
    from_fifo = 0;
}

pe_array::~pe_array() {
    int i;
    free(input_buffer);
    free(output_buffer);
    for (i = 0; i < PE_NUM; i++)
        delete pe_unit[i];
}

void pe_array::buffer_reset(int* buffer, int num) {
    int i;
    for (i = 0; i < num; i++)
        buffer[i] = 0;
}

void pe_array::input_buffer_write_from_ddr(int addr, int* data) {

    if (addr >= 0 && addr < input_buffer_size) {
        input_buffer[addr] = *data;
    } else {
        fprintf(stderr, "data buffer write addr %d is out of bound\n", addr);
        exit(-1);
    }
}

void pe_array::input_buffer_write_from_ddr_unsigned(int addr, unsigned int* data) {

    if (addr >= 0 && addr < input_buffer_size) {
        input_buffer[addr] = *data;
    } else {
        fprintf(stderr, "data buffer write addr %d is out of bound\n", addr);
        exit(-1);
    }
}

void pe_array::compute_instruction_buffer_write_from_ddr(int addr, unsigned long data[]) {

    if (addr >= 0 && addr < COMP_INSTR_BUFFER_GROUP_NUM) {
        compute_instruction_buffer[addr][0] = data[0];
        compute_instruction_buffer[addr][1] = data[1];
    } else {
        fprintf(stderr, "PE instruction buffer write addr %d is out of bound\n", addr);
        exit(-1);
    }
}

void pe_array::main_instruction_buffer_write_from_ddr(int addr, unsigned long data[]) {

    if (addr >= 0 && addr < CTRL_INSTR_BUFFER_NUM) {
        main_instruction_buffer[addr][0] = data[0];
        main_instruction_buffer[addr][1] = data[1];
    } else {
        fprintf(stderr, "main instruction buffer write addr %d is out of bound\n", addr);
        exit(-1);
    }
}

void pe_array::pe_instruction_buffer_write_from_ddr(int addr, unsigned long data[], int id) {

    pe_unit[id]->ctrl_instr_load_from_ddr(addr, data);

};


int pe_array::load(int source_pos, int reg_immBar_flag, int rs1, int rs2, int simd) {

    int data = 0;
    int source_addr = 0;
    
    if (reg_immBar_flag) source_addr = main_addressing_register[rs1] + main_addressing_register[rs2];
    else source_addr = rs1 + main_addressing_register[rs2];


#ifdef DEBUG
    printf("src: %d reg_immBar_flag: %d reg_imm_1: %d reg_1: %d src_addr: %d\n", source_pos, reg_immBar_flag, rs1, main_addressing_register[rs2], source_addr);
#endif

    if (source_pos == 1) {
        data = main_addressing_register[source_addr];
#ifdef PROFILE
    if (simd)
        printf("%lx from main addr reg[%d] to ", data, source_addr);
    else
        printf("%d from main addr reg[%d] to ", data, source_addr);
#endif
    } else if (source_pos == 3) {
        PE_instruction[0] = compute_instruction_buffer[source_addr][0];
        PE_instruction[1] = compute_instruction_buffer[source_addr][1];
#ifdef PROFILE
        printf("%lx %lx from main comp instr buffer[%d] to ", PE_instruction[0], PE_instruction[1], source_addr);
#endif
    } else if (source_pos == 5) {
        if (source_addr >= 0 && source_addr < input_buffer_size) {
            data = input_buffer[source_addr];
#ifdef PROFILE
    if (simd)
        printf("%lx from input buffer[%d] to ", data, source_addr);
    else
        printf("%d from input buffer[%d] to ", data, source_addr);
#endif
        } else {
            fprintf(stderr, "main load input buffer addr %d error.\n", source_addr);
            exit(-1);
        }
    } else if (source_pos == 7) {
        data = load_data;
#ifdef PROFILE
    if (simd)
        printf("%lx from last PE to ", data);
    else
        printf("%d from last PE to ", data);
#endif
    } else if (source_pos >= 11 && source_pos <=14) {
        data = fifo_unit[0][source_pos - 11].pop();
        from_fifo = 1;
#ifdef PROFILE
    if (simd)
        printf("%lx from fifo[%d] to ", data, source_pos - 11);
    else {
        printf("%d from fifo[%d] to (size is %d)", data, source_pos - 11, fifo_unit[0][source_pos - 11].size());
        fifo_unit[0][source_pos - 11].show();
    }
#endif
    } else {
        fprintf(stderr, "source_pos error.\n");
        exit(-1);
    }
    return data;
}

void pe_array::store(int dest_pos, int reg_immBar_flag, int rs1, int rs2, int data, int simd) {

    int dest_addr = 0;

    if (reg_immBar_flag) dest_addr = main_addressing_register[rs1] + main_addressing_register[rs2];
    else dest_addr = rs1 + main_addressing_register[rs2];

    // if (dest_pos == 1 && dest_addr == 0) printf("%d\n", data);
    // if (main_addressing_register[0] != 0) printf("!");
    // if (rs2 == 0 && rs1 == 0) {
    //     printf("%lx %d\n", main_addressing_register[rs2], data);
    //     if (main_addressing_register[rs2] != 0) 
    //         printf("!");
    // }

#ifdef DEBUG
    printf("dest: %d reg_immBar_flag: %d reg_imm_1: %d reg_1: %d gr[reg_1]: %d dest_addr: %d\n", dest_pos, reg_immBar_flag, rs1, rs2, main_addressing_register[rs2], dest_addr);
#endif

    if (dest_pos == 1) {
        main_addressing_register[dest_addr] = data;
        if (dest_addr == 0) printf("%d\n", data);
#ifdef PROFILE
        printf("main addr register[%d].\n", dest_addr);
#endif
    } else if(dest_pos == 6) {
        if (dest_addr >= 0 && dest_addr < output_buffer_size) {
            output_buffer[dest_addr] = data;
#ifdef PROFILE
            printf("output buffer[%d].\n", dest_addr);
#endif
        } else {
            fprintf(stderr, "main store output buffer addr %d error.\n", dest_addr);
            exit(-1);
        }
    } else if (dest_pos == 9) {
        store_data = data;
#ifdef PROFILE
        printf("PE[0].\n");
#endif
    } else if (dest_pos >= 11 && dest_pos <= 14) {
        // fprintf(stderr, "fifo[0] ");
        fifo_unit[0][dest_pos - 11].push(data);
#ifdef PROFILE
    printf("fifo[%d]. size is %d\n", dest_pos - 11, fifo_unit[0][dest_pos - 11].size());
    fifo_unit[0][dest_pos - 11].show();
#endif
    }
}

int pe_array::decode(unsigned long instruction, int* PC, int simd, int setting, int main_instruction_setting) {

#ifdef PROFILE
    // printf("main j=%d\t", main_addressing_register[12]);
    // printf("main j=%d\t", main_addressing_register[4]);
    printf("main i=%d j=%d\t", main_addressing_register[8]/20 - 1, main_addressing_register[5]);
#endif

    // pe_array position:   
    // src - 1/3/4/5/6/7/10
    // dest - 1/3/4/5/6/8/9
    // 0 - Compute register
    // 1 - Addressing register
    // 2 - Scratchpad memory
    // 3-6 FIFO[0-3]
    // 7 - Input buffer
    // 8 - Output buffer
    // 9 - In data port
    // 10 - Out data port
    // 11 - imm
    // 12 - none

    int i, rd, rs1, rs2, imm, data, comp_0 = 0, comp_1 = 0, sum = 0, add_a = 0, add_b = 0;

    int8_t rs[4];
        
    unsigned long dest_mask = (unsigned long)((1 << MEMORY_COMPONENTS_ADDR_WIDTH) - 1) << (INSTRUCTION_WIDTH - MEMORY_COMPONENTS_ADDR_WIDTH);
    unsigned long src_mask = (unsigned long)((1 << MEMORY_COMPONENTS_ADDR_WIDTH) - 1) << (INSTRUCTION_WIDTH - 2*MEMORY_COMPONENTS_ADDR_WIDTH);
    unsigned long reg_immBar_flag_0_mask = (unsigned long)1 << (INSTRUCTION_WIDTH - 2*MEMORY_COMPONENTS_ADDR_WIDTH - 1);
    unsigned long reg_auto_increasement_flag_0_mask = (unsigned long)1 << (INSTRUCTION_WIDTH - 2*MEMORY_COMPONENTS_ADDR_WIDTH - 2);
    unsigned long reg_imm_0_sign_bit_mask = (unsigned long)1 << (INSTRUCTION_WIDTH - 2*MEMORY_COMPONENTS_ADDR_WIDTH - 3);
    unsigned long reg_imm_0_mask = (unsigned long)((1 << IMMEDIATE_WIDTH) - 1) << (2 + IMMEDIATE_WIDTH + 2 * GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH);
    unsigned long reg_0_mask = (unsigned long)((1 << GLOBAL_REGISTER_ADDR_WIDTH) - 1) << (2 + IMMEDIATE_WIDTH + GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH);
    unsigned long reg_immBar_flag_1_mask = (unsigned long)1 << (1 + IMMEDIATE_WIDTH + GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH);
    unsigned long reg_auto_increasement_flag_1_mask = (unsigned long)1 << (IMMEDIATE_WIDTH + GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH);
    unsigned long reg_imm_1_sign_bit_mask = (unsigned long)1 << (IMMEDIATE_WIDTH + GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH - 1);
    unsigned long reg_imm_1_mask = (unsigned long)((1 << IMMEDIATE_WIDTH) - 1) << (GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH);
    unsigned long reg_1_mask = (unsigned long)((1 << GLOBAL_REGISTER_ADDR_WIDTH) - 1) << CTRL_OPCODE_WIDTH;
    unsigned long opcode_mask = (unsigned long)((1 << CTRL_OPCODE_WIDTH) - 1);

    int dest = (instruction & dest_mask) >> (INSTRUCTION_WIDTH - MEMORY_COMPONENTS_ADDR_WIDTH);
    int src = (instruction & src_mask) >> (INSTRUCTION_WIDTH - 2*MEMORY_COMPONENTS_ADDR_WIDTH);
    int reg_immBar_flag_0 = (instruction & reg_immBar_flag_0_mask) >> (INSTRUCTION_WIDTH - 2*MEMORY_COMPONENTS_ADDR_WIDTH - 1);
    int reg_auto_increasement_flag_0 = (instruction & reg_auto_increasement_flag_0_mask) >> (INSTRUCTION_WIDTH - 2*MEMORY_COMPONENTS_ADDR_WIDTH - 2);
    int reg_imm_0 = (instruction & reg_imm_0_mask) >> (2 + IMMEDIATE_WIDTH + 2 * GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH);
    int reg_imm_0_sign_bit = (instruction & reg_imm_0_sign_bit_mask) >> (INSTRUCTION_WIDTH - 2*MEMORY_COMPONENTS_ADDR_WIDTH - 3);
    int sext_imm_0 = reg_imm_0 | (reg_imm_0_sign_bit ? 0xFFFFFC00 : 0);
    int reg_0 = (instruction & reg_0_mask) >> (2 + IMMEDIATE_WIDTH + GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH);
    int reg_immBar_flag_1 = (instruction & reg_immBar_flag_1_mask) >> (1 + IMMEDIATE_WIDTH + GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH);
    int reg_auto_increasement_flag_1 = (instruction & reg_auto_increasement_flag_1_mask) >> (IMMEDIATE_WIDTH + GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH);
    int reg_imm_1 = (instruction & reg_imm_1_mask) >> (GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH);
    int reg_imm_1_sign_bit = (instruction & reg_imm_1_sign_bit_mask) >> (IMMEDIATE_WIDTH + GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH - 1);
    int sext_imm_1 = reg_imm_1 | (reg_imm_1_sign_bit ? 0xFFFFFC00 : 0);
    int reg_1 = (instruction & reg_1_mask) >> CTRL_OPCODE_WIDTH;
    int opcode = instruction & opcode_mask;

#ifdef PROFILE
    printf("PC = %d\t", *PC);
#endif
    if (main_instruction_setting == MAIN_INSTRUCTION_2) {
        if (((opcode == 4 || opcode == 5) && (dest == 5 || dest == 6 || dest == 11 || dest == 12 || dest == 13 || dest == 14)) || opcode == 14) {
            (*PC)++;
#ifdef PROFILE
            printf("\n");
#endif
            return 0;
        }
    } else if (main_instruction_setting == MAIN_INSTRUCTION_1) {
        if (dest == 5 || dest == 6 || dest == 11 || dest == 12 || dest == 13 || dest == 14) {
            (*PC)++;
#ifdef PROFILE
            printf("\n");
#endif
            return 0;
        }
    }

#ifdef DEBUG
    printf("dest: %d src: %d reg_immBar_flag_0: %d reg_auto_increasement_flag_0: %d reg_imm_0_sign_bit: %d sext_imm_0: %d, reg_0: %d reg_immBar_flag_1: %d reg_auto_increasement_flag_1: %d reg_imm_1_sign_bit: %d sext_imm_1: %d reg_1: %d opcode: %d\n", dest, src, reg_immBar_flag_0, reg_auto_increasement_flag_0, reg_imm_0_sign_bit, sext_imm_0, reg_0, reg_immBar_flag_1, reg_auto_increasement_flag_1, reg_imm_1_sign_bit, sext_imm_1, reg_1, opcode);
#endif

    if (opcode == 0) {              // add rd rs1 rs2
        rd = reg_imm_0;
        rs1 = reg_imm_1;
        rs2 = reg_1;
        add_a = main_addressing_register[rs1];
        add_b = main_addressing_register[rs2];
        sum = add_a + add_b;
        main_addressing_register[rd] = sum;
#ifdef PROFILE
        printf("add gr[%d] gr[%d] gr[%d] (%d %d %d)\n", rd, rs1, rs2, sum, add_a, add_b);
#endif
        (*PC)++;
    } else if (opcode == 1) {       // sub rd rs1 rs2
        rd = reg_imm_0;
        rs1 = reg_imm_1;
        rs2 = reg_1;
        add_a = main_addressing_register[rs1];
        add_b = main_addressing_register[rs2];
        sum = add_a - add_b;
        main_addressing_register[rd] = sum;
#ifdef PROFILE
        printf("sub gr[%d] gr[%d] gr[%d] (%d %d %d)\n", rd, rs1, rs2, sum, add_a, add_b);
#endif
        (*PC)++;
    } else if (opcode == 2) {       // addi rd rs2 imm
        rd = reg_imm_0;
        imm = sext_imm_1;
        rs2 = reg_1;
        add_a = imm;
        add_b = main_addressing_register[rs2];
        sum = add_a + add_b;
        main_addressing_register[rd] = sum;
#ifdef PROFILE
        printf("addi gr[%d] %d gr[%d] (%d %d %d)\n", rd, imm, rs2, sum, add_a, add_b);
#endif
        (*PC)++;
    } else if (opcode == 3) {       // set_8 rd rs2
        rd = reg_imm_0;
        rs2 = reg_1;
        memcpy(rs, &main_addressing_register[rs2], 4*sizeof(int8_t));

        for (i = 0; i < 4; i++) {
            rs[i] = main_addressing_register[rs2] & 0xFF;
        }
        memcpy(&main_addressing_register[rd], rs, 4*sizeof(int8_t));
#ifdef PROFILE
        printf("set_8 gr[%d] gr[%d] (%d %lx)\n", rd, rs2, main_addressing_register[rs2], main_addressing_register[rd]);
#endif
        (*PC)++;
    } else if (opcode == 4) {       // li dest imm/reg(reg(++))
#ifdef PROFILE
    if (simd)
        printf("Store %lx to ", sext_imm_1);
    else
        printf("Store %d to ", sext_imm_1);
#endif
        store(dest, reg_immBar_flag_0, sext_imm_0, reg_0, sext_imm_1, simd);
        if (reg_auto_increasement_flag_0)
            main_addressing_register[reg_0]++;
        (*PC)++;
    } else if (opcode == 5) {       // mv dest src imm/reg(reg(++)) imm/reg(reg(++))
#ifdef PROFILE
        printf("Move ");
#endif
        data = load(src, reg_immBar_flag_1, sext_imm_1, reg_1, simd);
        store(dest, reg_immBar_flag_0, sext_imm_0, reg_0, data, simd);
        if (reg_auto_increasement_flag_0)
            main_addressing_register[reg_0]++;
        if (reg_auto_increasement_flag_1)
            main_addressing_register[reg_1]++;
        (*PC)++;
//     } else if (opcode == 6) {       // add_8 rd rs1 rs2
//         rd = reg_imm_0;
//         rs1 = reg_imm_1;
//         rs2 = reg_1;
//         memcpy(rs, &main_addressing_register[rs1], 4 * sizeof(int8_t));
//         memcpy(rs_, &main_addressing_register[rs2], 4 * sizeof(int8_t));
//         for (i = 0; i < 4; i++) rd_[i] = rs[i] + rs_[i];
//         memcpy(&main_addressing_register[rd], rd_, 4 * sizeof(int8_t));
// #ifdef PROFILE
//         printf("add_8 gr[%d] gr[%d] gr[%d] (%lx %lx %lx)\n", rd, rs1, rs2, main_addressing_register[rd], main_addressing_register[rs1], main_addressing_register[rs2]);
// #endif
//         (*PC)++;
//     } else if (opcode == 7) {       // addi_8 rd imm rs2
//         rd = reg_imm_0;
//         rs2 = reg_1;
//         memcpy(rs_, &main_addressing_register[rs2], 4 * sizeof(int8_t));
//         for (i = 0; i < 4; i++) rd_[i] = reg_imm_1 && 0xFF + rs_[i];
//         memcpy(&main_addressing_register[rd], rd_, 4 * sizeof(int8_t));
// #ifdef PROFILE
//         printf("addi_8 gr[%d] %d gr[%d] (%lx %d %lx)\n", rd, sext_imm_1, rs2, main_addressing_register[rd], sext_imm_1, main_addressing_register[rs2]);
// #endif
//         (*PC)++;
    } else if (opcode == 8) {       // bne rs1 rs2 offset
        rs1 = sext_imm_1;
        rs2 = reg_1;
#ifdef PROFILE
        printf("bne %d %d %d", rs1, rs2, sext_imm_0);
#endif
        if (reg_immBar_flag_1) comp_0 = main_addressing_register[rs1];
        else comp_0 = sext_imm_1;
        comp_1 = main_addressing_register[rs2];
#ifdef PROFILE
        printf(" (%d %d)", comp_0, comp_1);
#endif
        if (comp_0 != comp_1) {
            *PC = *PC + sext_imm_0;
#ifdef PROFILE
            printf(" jump.\n");
#endif
        } else {
            (*PC)++;
#ifdef PROFILE
            printf(" not jump.\n");
#endif
        }
    } else if (opcode == 9) {       // beq rs1 rs2 offset
        rs1 = sext_imm_1;
        rs2 = reg_1;
#ifdef PROFILE
        printf("beq %d %d %d", rs1, rs2, sext_imm_0);
#endif
        if (reg_immBar_flag_1) comp_0 = main_addressing_register[rs1];
        else comp_0 = sext_imm_1;
        comp_1 = main_addressing_register[rs2];
#ifdef PROFILE
        printf(" (%d %d)", comp_0, comp_1);
#endif
        if (comp_0 == comp_1) {
            *PC = *PC + sext_imm_0;
#ifdef PROFILE
            printf(" jump.\n");
#endif
        } else {
            (*PC)++;
#ifdef PROFILE
            printf(" not jump.\n");
#endif
        }
    } else if (opcode == 10) {       // bge rs1 rs2 offset
        rs1 = sext_imm_1;
        rs2 = reg_1;
#ifdef PROFILE
        printf("bge %d %d %d", rs1, rs2, sext_imm_0);
#endif
        if (reg_immBar_flag_1) comp_0 = main_addressing_register[rs1];
        else comp_0 = sext_imm_1;
        comp_1 = main_addressing_register[rs2];
#ifdef PROFILE
        printf(" (%d %d)", comp_0, comp_1);
#endif
        if (comp_0 >= comp_1) {
            *PC = *PC + sext_imm_0;
#ifdef PROFILE
            printf(" jump.\n");
#endif
        } else {
            (*PC)++;
#ifdef PROFILE
            printf(" not jump.\n");
#endif
        }
    } else if (opcode == 11) {       // blt rs1 rs2 offset
        rs1 = sext_imm_1;
        rs2 = reg_1;
#ifdef PROFILE
        printf("blt %d %d %d", rs1, rs2, sext_imm_0);
#endif
        if (reg_immBar_flag_1) comp_0 = main_addressing_register[rs1];
        else comp_0 = sext_imm_1;
        comp_1 = main_addressing_register[rs2];
#ifdef PROFILE
        printf(" (%d %d)", comp_0, comp_1);
#endif
        if (comp_0 < comp_1) {
            *PC = *PC + sext_imm_0;
#ifdef PROFILE
            printf(" jump.\n");
#endif
        } else {
            (*PC)++;
#ifdef PROFILE
            printf(" not jump.\n");
#endif
        }
    } else if (opcode == 12) {      // jump
        *PC = *PC + sext_imm_0;
#ifdef PROFILE
        printf("jump %d\n", sext_imm_0);
#endif
    } else if (opcode == 13) {      // set PE_PC
        for (i = 0; i < setting; i++) {
            pe_unit[i]->PC[0] = sext_imm_0;
            pe_unit[i]->PC[1] = sext_imm_0;
        }
#ifdef PROFILE
        printf("set PE PC to %d.\n", sext_imm_0);
#endif
        (*PC)++;
    } else if (opcode == 14) {      // None
        (*PC)++;
#ifdef PROFILE
        printf("No-op.\n");
#endif
    } else if (opcode == 15) {      // halt
#ifdef PROFILE
        printf("halt.\n");
#endif
        return -1;
    } else {
        fprintf(stderr, "main control instruction opcode error.\n");
        exit(-1);
    }
    return 0;
}

int pe_array::decode_output(unsigned long instruction, int* PC, int simd, int setting, int main_instruction_setting) {

#ifdef PROFILE
    printf("main\t");
#endif
    int i, rd, rs1, rs2, imm, data, sum = 0, add_a = 0, add_b = 0;
    int8_t rs[4];
        
    unsigned long dest_mask = (unsigned long)((1 << MEMORY_COMPONENTS_ADDR_WIDTH) - 1) << (INSTRUCTION_WIDTH - MEMORY_COMPONENTS_ADDR_WIDTH);
    unsigned long src_mask = (unsigned long)((1 << MEMORY_COMPONENTS_ADDR_WIDTH) - 1) << (INSTRUCTION_WIDTH - 2*MEMORY_COMPONENTS_ADDR_WIDTH);
    unsigned long reg_immBar_flag_0_mask = (unsigned long)1 << (INSTRUCTION_WIDTH - 2*MEMORY_COMPONENTS_ADDR_WIDTH - 1);
    unsigned long reg_auto_increasement_flag_0_mask = (unsigned long)1 << (INSTRUCTION_WIDTH - 2*MEMORY_COMPONENTS_ADDR_WIDTH - 2);
    unsigned long reg_imm_0_sign_bit_mask = (unsigned long)1 << (INSTRUCTION_WIDTH - 2*MEMORY_COMPONENTS_ADDR_WIDTH - 3);
    unsigned long reg_imm_0_mask = (unsigned long)((1 << IMMEDIATE_WIDTH) - 1) << (2 + IMMEDIATE_WIDTH + 2 * GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH);
    unsigned long reg_0_mask = (unsigned long)((1 << GLOBAL_REGISTER_ADDR_WIDTH) - 1) << (2 + IMMEDIATE_WIDTH + GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH);
    unsigned long reg_immBar_flag_1_mask = (unsigned long)1 << (1 + IMMEDIATE_WIDTH + GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH);
    unsigned long reg_auto_increasement_flag_1_mask = (unsigned long)1 << (IMMEDIATE_WIDTH + GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH);
    unsigned long reg_imm_1_sign_bit_mask = (unsigned long)1 << (IMMEDIATE_WIDTH + GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH - 1);
    unsigned long reg_imm_1_mask = (unsigned long)((1 << IMMEDIATE_WIDTH) - 1) << (GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH);
    unsigned long reg_1_mask = (unsigned long)((1 << GLOBAL_REGISTER_ADDR_WIDTH) - 1) << CTRL_OPCODE_WIDTH;
    unsigned long opcode_mask = (unsigned long)((1 << CTRL_OPCODE_WIDTH) - 1);

    int dest = (instruction & dest_mask) >> (INSTRUCTION_WIDTH - MEMORY_COMPONENTS_ADDR_WIDTH);
    int src = (instruction & src_mask) >> (INSTRUCTION_WIDTH - 2*MEMORY_COMPONENTS_ADDR_WIDTH);
    int reg_immBar_flag_0 = (instruction & reg_immBar_flag_0_mask) >> (INSTRUCTION_WIDTH - 2*MEMORY_COMPONENTS_ADDR_WIDTH - 1);
    int reg_auto_increasement_flag_0 = (instruction & reg_auto_increasement_flag_0_mask) >> (INSTRUCTION_WIDTH - 2*MEMORY_COMPONENTS_ADDR_WIDTH - 2);
    int reg_imm_0 = (instruction & reg_imm_0_mask) >> (2 + IMMEDIATE_WIDTH + 2 * GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH);
    int reg_imm_0_sign_bit = (instruction & reg_imm_0_sign_bit_mask) >> (INSTRUCTION_WIDTH - 2*MEMORY_COMPONENTS_ADDR_WIDTH - 3);
    int sext_imm_0 = reg_imm_0 | (reg_imm_0_sign_bit ? 0xFFFFFC00 : 0);
    int reg_0 = (instruction & reg_0_mask) >> (2 + IMMEDIATE_WIDTH + GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH);
    int reg_immBar_flag_1 = (instruction & reg_immBar_flag_1_mask) >> (1 + IMMEDIATE_WIDTH + GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH);
    int reg_auto_increasement_flag_1 = (instruction & reg_auto_increasement_flag_1_mask) >> (IMMEDIATE_WIDTH + GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH);
    int reg_imm_1 = (instruction & reg_imm_1_mask) >> (GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH);
    int reg_imm_1_sign_bit = (instruction & reg_imm_1_sign_bit_mask) >> (IMMEDIATE_WIDTH + GLOBAL_REGISTER_ADDR_WIDTH + CTRL_OPCODE_WIDTH - 1);
    int sext_imm_1 = reg_imm_1 | (reg_imm_1_sign_bit ? 0xFFFFFC00 : 0);
    int reg_1 = (instruction & reg_1_mask) >> CTRL_OPCODE_WIDTH;
    int opcode = instruction & opcode_mask;

#ifdef PROFILE
    printf("PC = %d\t", *PC);
#endif
    if (main_instruction_setting == MAIN_INSTRUCTION_2) {
        if (((opcode == 4 || opcode == 5) && (dest != 5 && dest != 6 && dest != 11 && dest != 12 && dest != 13 && dest != 14)) || opcode == 14) {
#ifdef PROFILE
            printf("\n");
#endif
            return 0;
        }
    } else if (main_instruction_setting == MAIN_INSTRUCTION_1) {
        if (dest != 5 && dest != 6 && dest != 11 && dest != 12 && dest != 13 && dest != 14) {
#ifdef PROFILE
            printf("\n");
#endif
            return 0;
        }
    }

#ifdef DEBUG
    printf("dest: %d src: %d reg_immBar_flag_0: %d reg_auto_increasement_flag_0: %d reg_imm_0_sign_bit: %d sext_imm_0: %d, reg_0: %d reg_immBar_flag_1: %d reg_auto_increasement_flag_1: %d reg_imm_1_sign_bit: %d sext_imm_1: %d reg_1: %d opcode: %d\n", dest, src, reg_immBar_flag_0, reg_auto_increasement_flag_0, reg_imm_0_sign_bit, sext_imm_0, reg_0, reg_immBar_flag_1, reg_auto_increasement_flag_1, reg_imm_1_sign_bit, sext_imm_1, reg_1, opcode);
#endif

    if (opcode == 0) {              // add rd rs1 rs2
        rd = reg_imm_0;
        rs1 = reg_imm_1;
        rs2 = reg_1;
        add_a = main_addressing_register[rs1];
        add_b = main_addressing_register[rs2];
        sum = add_a + add_b;
        main_addressing_register[rd] = sum;
#ifdef PROFILE
        printf("add gr[%d] gr[%d] gr[%d] (%d %d %d)\n", rd, rs1, rs2, sum, add_a, add_b);
#endif
    } else if (opcode == 1) {       // sub rd rs1 rs2
        rd = reg_imm_0;
        rs1 = reg_imm_1;
        rs2 = reg_1;
        add_a = main_addressing_register[rs1];
        add_b = main_addressing_register[rs2];
        sum = add_a - add_b;
        main_addressing_register[rd] = sum;
#ifdef PROFILE
        printf("sub gr[%d] gr[%d] gr[%d] (%d %d %d)\n", rd, rs1, rs2, sum, add_a, add_b);
#endif
    } else if (opcode == 2) {       // addi rd rs2 imm
        rd = reg_imm_0;
        imm = sext_imm_1;
        rs2 = reg_1;
        add_a = imm;
        add_b = main_addressing_register[rs2];
        sum = add_a + add_b;
        main_addressing_register[rd] = sum;
#ifdef PROFILE
        printf("addi gr[%d] %d gr[%d] (%d %d %d)\n", rd, imm, rs2, sum, add_a, add_b);
#endif
    } else if (opcode == 3) {       // set_8 rd rs2
        rd = reg_imm_0;
        rs2 = reg_1;
        memcpy(rs, &main_addressing_register[rs2], 4*sizeof(int8_t));

        for (i = 0; i < 4; i++) {
            rs[i] = main_addressing_register[rs2] & 0xFF;
        }
        memcpy(&main_addressing_register[rd], rs, 4*sizeof(int8_t));
#ifdef PROFILE
        printf("set_8 gr[%d] gr[%d] (%d %lx)\n", rd, rs2, main_addressing_register[rs2], main_addressing_register[rd]);
#endif
    } else if (opcode == 4) {       // li dest imm/reg(reg(++))
#ifdef PROFILE
    if (simd)
        printf("Store %lx to ", sext_imm_1);
    else
        printf("Store %d to ", sext_imm_1);
#endif
        store(dest, reg_immBar_flag_0, sext_imm_0, reg_0, sext_imm_1, simd);
        if (reg_auto_increasement_flag_0)
            main_addressing_register[reg_0]++;
    } else if (opcode == 5) {       // mv dest src imm/reg(reg(++)) imm/reg(reg(++))
#ifdef PROFILE
        printf("Move ");
#endif
        data = load(src, reg_immBar_flag_1, sext_imm_1, reg_1, simd);
        store(dest, reg_immBar_flag_0, sext_imm_0, reg_0, data, simd);
        if (reg_auto_increasement_flag_0)
            main_addressing_register[reg_0]++;
        if (reg_auto_increasement_flag_1)
            main_addressing_register[reg_1]++;
    }
    // } else fprintf(stderr, "decode_output opcode error %d.\n", opcode);
    return 0;
}

void pe_array::show_gr() {
    int i;
    for (i = 0; i < ADDR_REGISTER_NUM; i++)
        printf("main gr[%d] = %d\n", i, main_addressing_register[i]);
}

void pe_array::show_compute_instruction_buffer() {
    int i, j;
    for (i = 0; i < COMP_INSTR_BUFFER_GROUP_NUM; i++)
        for (j = 0; j < COMP_INSTR_BUFFER_GROUP_SIZE; j++)
            printf("compute instruction buffer[%d][%d] = %lx\n", i, j, compute_instruction_buffer[i][j]);
}

int Float2Fix(float exact_value) {
    int MIN_INTEGER = -pow(2, NUM_FRACTION_BITS+NUM_INTEGER_BITS);
    if (exact_value == - std::numeric_limits<float>::infinity())
        return MIN_INTEGER;
    int result = (int)ceil(exact_value * pow(2, NUM_FRACTION_BITS));
    return result;
}

float Fix2Float(int integer) {
    float result = (float)(integer / pow(2, NUM_FRACTION_BITS));
    return result;
}

int Upper_LOG2_accurate(float num){
    float numLog2 = log(num) / log(2);
    int result = Float2Fix(numLog2);
    return result;
}

void pe_array::phmm_show_output_buffer(FILE* fp) {
    float INITIAL_CONDITION_UP = (float)pow(2, 127);
    // float result = log10(pow(2, Fix2Float(output_buffer[0] - Upper_LOG2_accurate(INITIAL_CONDITION_UP))));
    fprintf(fp, "%d\n", output_buffer[0]);
}

void pe_array::chain_show_output_buffer(int n, FILE* fp) {
    int j;
    for (j = 0; j < n; j++) {
        fprintf(fp, "%d\n", output_buffer[j]);
    }
}

void pe_array::bsw_show_output_buffer(FILE* fp) {
    int i, j;
    int8_t output[6][4];
    for (i = 0; i < 6; i++)
        memcpy(output[i], output_buffer+i, 4*sizeof(int8_t));
    for (j = 0; j < 4; j++) {
        fprintf(fp, "%d ", output[0][j]);
        fprintf(fp, "%d ", output[3][j]);
        fprintf(fp, "%d ", output[4][j]);
        fprintf(fp, "%d ", output[1][j]);
        fprintf(fp, "%d ", output[2][j]);
        fprintf(fp, "%d\n", output[5][j]);
    }
}

void pe_array::poa_show_output_buffer(int len_y, int len_x, FILE* fp) {
    int num = len_y * len_x * 2;
    // printf("Output: %d\n", num);
    fprintf(fp, "Output: %d\n", num);
    int i, j, k;
    int iter = len_x * 8;

    for (i = 0; i < len_y/4; i++) {
        fprintf(fp, "%d ", i*iter);
        fprintf(fp, "x x x x x x %d %d \n", output_buffer[i*iter+6], output_buffer[i*iter+7]);
        fprintf(fp, "%d ", i*iter+8);
        fprintf(fp, "x x x x %d %d %d %d \n", output_buffer[i*iter+8+4], output_buffer[i*iter+8+5], output_buffer[i*iter+8+6], output_buffer[i*iter+8+7]);
        fprintf(fp, "%d ", i*iter+16);
        fprintf(fp, "x x %d %d %d %d %d %d \n", output_buffer[i*iter+16+2], output_buffer[i*iter+16+3], output_buffer[i*iter+16+4], output_buffer[i*iter+16+5], output_buffer[i*iter+16+6], output_buffer[i*iter+16+7]);

        for (j = 3; j < len_x-3; j++) {
            fprintf(fp, "%d ", i*iter+j*8);
            for (k = 0; k < 8; k++)
                fprintf(fp, "%d ", output_buffer[i*iter + j*8 + k]);
            fprintf(fp, "\n");
        }

        fprintf(fp, "%d ", i*iter+(len_x-3)*8);
        fprintf(fp, "%d %d %d %d %d %d x x \n", output_buffer[i*iter+(len_x-3)*8], output_buffer[i*iter+(len_x-3)*8+1], output_buffer[i*iter+(len_x-3)*8+2], output_buffer[i*iter+(len_x-3)*8+3], output_buffer[i*iter+(len_x-3)*8+4], output_buffer[i*iter+(len_x-3)*8+5]);
        fprintf(fp, "%d ", i*iter+(len_x-2)*8);
        fprintf(fp, "%d %d %d %d x x x x \n", output_buffer[i*iter+(len_x-2)*8], output_buffer[i*iter+(len_x-2)*8+1], output_buffer[i*iter+(len_x-2)*8+2], output_buffer[i*iter+(len_x-2)*8+3]);
        fprintf(fp, "%d ", i*iter+(len_x-1)*8);
        fprintf(fp, "%d %d x x x x x x \n", output_buffer[i*iter+(len_x-1)*8], output_buffer[i*iter+(len_x-1)*8+1]);
    }
}



void pe_array::run(int cycle_limit, int simd, int setting, int main_instruction_setting) {
    int i, j, cycle = 0, flag, old_PC;

    while (1) {
        cycle++;
        old_PC = main_PC;
        flag = decode(main_instruction_buffer[main_PC][1], &main_PC, simd, setting, main_instruction_setting);
        pe_unit[0]->load_data = store_data;
        pe_unit[0]->load_instruction[0] = PE_instruction[0];
        pe_unit[0]->load_instruction[1] = PE_instruction[1];

        if (setting == PE_4_SETTING) {
            for (i = 0; i < 4; i++) {
#ifdef PROFILE
                printf("PE[%d]\t", i);
#endif
                pe_unit[i]->run(simd);
                if (i < 3) {
                    pe_unit[i+1]->load_data = pe_unit[i]->store_data;
                    pe_unit[i+1]->load_instruction[0] = pe_unit[i]->store_instruction[0];
                    pe_unit[i+1]->load_instruction[1] = pe_unit[i]->store_instruction[1];
                } else if (i == 3) {
                    load_data = pe_unit[3]->store_data;
                }
            }
        } else if (setting == PE_64_SETTING) {
            for (j = 0; j < 16; j++) {
                if (j > 0) {
                    if (from_fifo) pe_unit[j*4]->load_data = store_data;
                    else pe_unit[j*4]->load_data = pe_unit[j*4-1]->store_data;
                    pe_unit[j*4]->load_instruction[0] = pe_unit[j*4-1]->store_instruction[0];
                    pe_unit[j*4]->load_instruction[1] = pe_unit[j*4-1]->store_instruction[1];
                }

                for (i = 0; i < 4; i++) {
#ifdef PROFILE
                    printf("PE[%d]\t", j*4+i);
#endif
                    pe_unit[j*4+i]->run(simd);
                    if (i < 3) {
                        pe_unit[j*4+i+1]->load_data = pe_unit[j*4+i]->store_data;
                        pe_unit[j*4+i+1]->load_instruction[0] = pe_unit[j*4+i]->store_instruction[0];
                        pe_unit[j*4+i+1]->load_instruction[1] = pe_unit[j*4+i]->store_instruction[1];
                    } else if (j*4+i == 63) {
                        load_data = pe_unit[63]->store_data;
                    }
                }
            }
        }
        from_fifo = 0;
        
        if (main_instruction_setting == MAIN_INSTRUCTION_1)
            decode_output(main_instruction_buffer[old_PC][1], &old_PC, simd, setting, main_instruction_setting);
        else if (main_instruction_setting == MAIN_INSTRUCTION_2)
            decode_output(main_instruction_buffer[old_PC][0], &old_PC, simd, setting, main_instruction_setting);

        main_addressing_register[13] = pe_unit[0]->get_gr_10() && pe_unit[1]->get_gr_10();
        for (i = 2; i < setting; i++)
            main_addressing_register[13] = main_addressing_register[13] && pe_unit[i]->get_gr_10();
        if (flag == -1 || cycle == cycle_limit) {
            printf("cycle %d\n", cycle);
            break;
        }
    }

    // fprintf(stderr, "Finish simulation.\n");
}