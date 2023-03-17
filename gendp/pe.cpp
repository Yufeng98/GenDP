#include "pe.h"

pe::pe(int _id) {

    id = _id;
    comp_reg_load = 0, comp_reg_store = 0, addr_reg_load = 0, addr_reg_store = 0, SPM_load = 0, SPM_store = 0,
    comp_instr_load = 0, comp_instr_store = 0,
    comp_reg_load_addr = 0, comp_reg_store_addr = 0, addr_reg_load_addr = 0, addr_reg_store_addr = 0, SPM_load_addr = 0, SPM_store_addr = 0,
    comp_instr_load_addr = 0, comp_instr_store_addr = 0,
    store_instruction[0] = 0; store_instruction[1] = 0;
    load_instruction[0] = 0; load_instruction[1] = 0;
    instruction[0] = 0; instruction[1] = 0;
    comp_PC = COMP_INSTR_BUFFER_GROUP_NUM - 1;
    PC[0] = 0;
    PC[1] = 0;
}
pe::~pe() {
    delete comp_instr_buffer_unit;
    delete ctrl_instr_buffer_unit;
    delete SPM_unit;
    delete addr_regfile_unit;
    delete regfile_unit;
}

void pe::reset() {
    SPM_unit->reset();
    addr_regfile_unit->reset();
    regfile_unit->reset();
    comp_reg_load = 0, comp_reg_store = 0, addr_reg_load = 0, addr_reg_store = 0, SPM_load = 0, SPM_store = 0,
    comp_instr_load = 0, comp_instr_store = 0,
    comp_reg_load_addr = 0, comp_reg_store_addr = 0, addr_reg_load_addr = 0, addr_reg_store_addr = 0, SPM_load_addr = 0, SPM_store_addr = 0,
    comp_instr_load_addr = 0, comp_instr_store_addr = 0,
    store_instruction[0] = 0; store_instruction[1] = 0;
    load_instruction[0] = 0; load_instruction[1] = 0;
    instruction[0] = 0; instruction[1] = 0;
    comp_PC = COMP_INSTR_BUFFER_GROUP_NUM - 1;
    PC[0] = 0;
    PC[1] = 0;
}

void pe::run(int simd) {
    int i, op[2][3], input_addr[2][6], output_addr[2], ctrl_op[2];

    // Compute
    instruction[0] = comp_instr_buffer_unit->buffer[comp_PC][0];
    instruction[1] = comp_instr_buffer_unit->buffer[comp_PC][1];
#ifdef PROFILE
    printf("comp_PC = %d\t", comp_PC);
#endif
    comp_decoder_unit.execute(instruction[0], op[0], input_addr[0], &output_addr[0], &comp_PC);
    comp_decoder_unit.execute(instruction[1], op[1], input_addr[1], &output_addr[1], &i);
#ifdef PROFILE
    printf("\n");
#endif
    for (i = 0; i < 6; i++) {
        regfile_unit->read_addr[i] = input_addr[0][i];
        regfile_unit->read_addr[i+6] = input_addr[1][i];
    }
    regfile_unit->read(regfile_unit->read_addr, regfile_unit->read_data);
    regfile_unit->write_addr[0] = output_addr[0];
    regfile_unit->write_addr[1] = output_addr[1];

    if (simd) {
        regfile_unit->write_data[0] = cu_32.execute_8bit(op[0], regfile_unit->read_data);
        regfile_unit->write_data[1] = cu_32.execute_8bit(op[1], regfile_unit->read_data + 6);        
    } else {
        regfile_unit->write_data[0] = cu_32.execute(op[0], regfile_unit->read_data);
        regfile_unit->write_data[1] = cu_32.execute(op[1], regfile_unit->read_data + 6);   
    }


    regfile_unit->write(regfile_unit->write_addr, regfile_unit->write_data, 0);
    regfile_unit->write(regfile_unit->write_addr, regfile_unit->write_data, 1);
#ifdef PROFILE
    printf("\nPE[%d]\t", id);
#endif

    // Control
    decode(ctrl_instr_buffer_unit->buffer[PC[1]][1], &PC[1], src_dest[1], &ctrl_op[1], simd);
    decode(ctrl_instr_buffer_unit->buffer[PC[0]][0], &PC[0], src_dest[0], &ctrl_op[0], simd);
#ifdef PROFILE
    printf("\n");
#endif

    if (ctrl_op[0] == 5 && ctrl_op[1] == 5 && src_dest[0][0] == src_dest[1][0]) {
        fprintf(stderr, "PE[%d] PC[%d %d] source position confliction.\n", id, PC[0], PC[1]);
        exit(-1);
    } else if (ctrl_op[0] == 5 && ctrl_op[1] == 5 && src_dest[0][1] == src_dest[1][1]) {
        fprintf(stderr, "PE[%d] PC[%d %d] dest position confliction.\n", id, PC[0], PC[1]);
        exit(-1);
    }
}

void pe::ctrl_instr_load_from_ddr(int addr, unsigned long data[]) {
    if (addr >= 0 && addr < CTRL_INSTR_BUFFER_NUM) {
        ctrl_instr_buffer_unit->buffer[addr][0] = data[0];
        ctrl_instr_buffer_unit->buffer[addr][1] = data[1];
    } else {
        fprintf(stderr, "PE[%d] ctrl instr buffer write addr %d is out of bound\n", id, addr);
        exit(-1);
    }
}


int pe::load(int source_pos, int reg_immBar_flag, int rs1, int rs2, int simd) {

    int data = 0;
    int source_addr = 0;
    
    if (reg_immBar_flag) source_addr = addr_regfile_unit->buffer[rs1] + addr_regfile_unit->buffer[rs2];
    else source_addr = rs1 + addr_regfile_unit->buffer[rs2];
    

#ifdef DEBUG
    printf("src: %d reg_immBar_flag: %d reg_imm_1: %d reg_1: %d src_addr: %d\n", source_pos, reg_immBar_flag, rs1, rs2, source_addr);
#endif

    if (source_pos == 0) {
        comp_reg_load = 1;
        comp_reg_load_addr = source_addr;
        regfile_unit->read_addr[12] = comp_reg_load_addr;
        regfile_unit->read(regfile_unit->read_addr, regfile_unit->read_data);
        data = regfile_unit->read_data[12];
#ifdef PROFILE
    if (simd)
        printf("%lx from comp reg[%d] to ", data, source_addr);
    else
        printf("%d from comp reg[%d] to ", data, source_addr);
#endif
    } else if (source_pos == 1) {
        if (source_addr >= 0 && source_addr < ADDR_REGISTER_NUM) {
            data = addr_regfile_unit->buffer[source_addr];
#ifdef PROFILE
    if (simd)
        printf("%lx from addr reg[%d] to ", data, source_addr);
    else
        printf("%d from addr reg[%d] to ", data, source_addr);
#endif
        } else {
            fprintf(stderr, "PE[%d] load gr addr %d error.\n", id, source_addr);
            exit(-1);
        }
    } else if (source_pos == 2) {
        if (source_addr >= 0 && source_addr < SPM_ADDR_NUM) {
            data = SPM_unit->buffer[source_addr];
#ifdef PROFILE
    if (simd)
        printf("%lx from SPM[%d] to ", data, source_addr);
    else
        printf("%lx from SPM[%d] to ", data, source_addr);
#endif
        } else {
            fprintf(stderr, "PE[%d] load SPM addr %d error.\n", id, source_addr);
            exit(-1);
        }
    } else if (source_pos == 3) {
        comp_instr_load = 1;
        comp_instr_load_addr = source_addr;
        instruction[0] = comp_instr_buffer_unit->buffer[comp_instr_load_addr][0];
        instruction[1] = comp_instr_buffer_unit->buffer[comp_instr_load_addr][1];
#ifdef PROFILE
        printf("%lx %lx from comp instruction buffer[%d] to ", instruction[0], instruction[1], source_addr);
#endif
    } else if (source_pos == 7) {
        data = load_data;
#ifdef PROFILE
    if (simd)
        printf("%lx from input data port to ", data);
    else
        printf("%d from input data port to ", data);
#endif
    } else if (source_pos == 8) {
        instruction[0] = load_instruction[0];
        instruction[1] = load_instruction[1];
#ifdef PROFILE
        printf("%lx %lx from input comp instruction port to ", instruction[0], instruction[1]);
#endif
    } else {
        fprintf(stderr, "source_pos error.\n");
        exit(-1);
    }
    return data;
}

void pe::store(int dest_pos, int reg_immBar_flag, int rs1, int rs2, int data, int simd) {

    int dest_addr = 0;

    if (reg_immBar_flag) dest_addr = addr_regfile_unit->buffer[rs1] + addr_regfile_unit->buffer[rs2];
    else dest_addr = rs1 + addr_regfile_unit->buffer[rs2];

#ifdef DEBUG
    printf("dest: %d reg_immBar_flag: %d reg_imm_1: %d reg_1: %d src_addr: %d\n", dest_pos, reg_immBar_flag, rs1, rs2, dest_addr);
#endif

    if (dest_pos == 0) {
        comp_reg_store = 1;
        comp_reg_store_addr = dest_addr;
        regfile_unit->write_addr[2] = comp_reg_store_addr;
        regfile_unit->write_data[2] = data;
        regfile_unit->write(regfile_unit->write_addr, regfile_unit->write_data, 2);
#ifdef PROFILE
        printf("comp reg[%d].\t", dest_addr);
#endif
    } else if (dest_pos == 1) {
        if (dest_addr >= 0 && dest_addr < ADDR_REGISTER_NUM) {
            addr_regfile_unit->buffer[dest_addr] = data;
#ifdef PROFILE
            printf("addr reg[%d].\t", dest_addr);
#endif
        } else {
            fprintf(stderr, "PE[%d] store gr addr %d error.\n", id, dest_addr);
            exit(-1);
        }
    } else if (dest_pos == 2) {
        if (dest_addr >= 0 && dest_addr < SPM_ADDR_NUM) {
            SPM_unit->buffer[dest_addr] = data;
#ifdef PROFILE
            printf("SPM[%d].\t", dest_addr);
#endif
        } else {
            fprintf(stderr, "PE[%d] store SPM addr %d error.\n", id, dest_addr);
            exit(-1);
        }
    } else if (dest_pos == 3) {
        comp_instr_store = 1;
        comp_instr_store_addr = dest_addr;
        comp_instr_buffer_unit->buffer[comp_instr_store_addr][0] = instruction[0];
        comp_instr_buffer_unit->buffer[comp_instr_store_addr][1] = instruction[1];
#ifdef PROFILE
        printf("comp instruction buffer[%d].\t", dest_addr);
#endif
    } else if (dest_pos == 9) {
        store_data = data;
#ifdef PROFILE
        printf("output data port.\t");
#endif
    } else if (dest_pos == 10) {
        store_instruction[0] = instruction[0];
        store_instruction[1] = instruction[1];
#ifdef PROFILE
        printf("output comp instruction port.\t");
#endif
    } else { 
        fprintf(stderr, "dest_addr error.\t");
        exit(-1);
    }

}

int pe::decode(unsigned long instruction, int* PC, int src_dest[], int* op, int simd) {

    // pe position:   
    // src - 0/1/2/9
    // dest - 0/1/2/10
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

    int rd, rs1, rs2, imm, data, comp_0 = 0, comp_1 = 0, sum = 0, add_a = 0, add_b = 0;

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

    src_dest[0] = src;
    src_dest[1] = dest;
    *op = opcode;

#ifdef PROFILE
    printf("PC = %d\t", *PC);
#endif

#ifdef DEBUG
    printf("dest: %d src: %d reg_immBar_flag_0: %d reg_auto_increasement_flag_0: %d reg_imm_0_sign_bit: %d sext_imm_0: %d, reg_0: %d reg_immBar_flag_1: %d reg_auto_increasement_flag_1: %d reg_imm_1_sign_bit: %d sext_imm_1: %d reg_1: %d opcode: %d\n", dest, src, reg_immBar_flag_0, reg_auto_increasement_flag_0, reg_imm_0_sign_bit, sext_imm_0, reg_0, reg_immBar_flag_1, reg_auto_increasement_flag_1, reg_imm_1_sign_bit, sext_imm_1, reg_1, opcode);
#endif

    if (opcode == 0) {              // add rd rs1 rs2
        rd = reg_imm_0;
        rs1 = reg_imm_1;
        rs2 = reg_1;
        add_a = addr_regfile_unit->buffer[rs1];
        add_b = addr_regfile_unit->buffer[rs2];
        sum = add_a + add_b;
        addr_regfile_unit->buffer[rd] = sum;
#ifdef PROFILE
        printf("add gr[%d] gr[%d] gr[%d] (%d %d %d)\t", rd, rs1, rs2, sum, add_a, add_b);
#endif
        (*PC)++;
    } else if (opcode == 1) {       // sub rd rs1 rs2
        rd = reg_imm_0;
        rs1 = reg_imm_1;
        rs2 = reg_1;
        add_a = addr_regfile_unit->buffer[rs1];
        add_b = addr_regfile_unit->buffer[rs2];
        sum = add_a - add_b;
        addr_regfile_unit->buffer[rd] = sum;
#ifdef PROFILE
        printf("sub gr[%d] gr[%d] gr[%d] (%d %d %d)\t", rd, rs1, rs2, sum, add_a, add_b);
#endif
        (*PC)++;
    } else if (opcode == 2) {       // addi rd rs2 imm
        rd = reg_imm_0;
        imm = sext_imm_1;
        rs2 = reg_1;
        add_a = imm;
        add_b = addr_regfile_unit->buffer[rs2];
        sum = add_a + add_b;
        addr_regfile_unit->buffer[rd] = sum;
#ifdef PROFILE
        printf("addi gr[%d] %d gr[%d] (%d %d %d)\t", rd, imm, rs2, sum, add_a, add_b);
#endif
        (*PC)++;
//     } else if (opcode == 3) {       // set_8 rd rs2
//         rd = reg_imm_0;
//         rs2 = reg_1;
//         memcpy(rs, &main_addressing_register[rs2], 4*sizeof(int8_t));

//         for (i = 0; i < 4; i++) {
//             rs[i] = main_addressing_register[rs2] & 0xFF;
//         }
//         memcpy(&main_addressing_register[rd], rs, 4*sizeof(int8_t));
// #ifdef PROFILE
//         printf("set_8 gr[%d] gr[%d] (%d %lx)\n", rd, rs2, main_addressing_register[rs2], main_addressing_register[rd]);
// #endif
//         (*PC)++;
    } else if (opcode == 4) {       // li dest imm/reg(reg(++))
#ifdef PROFILE
    if (simd)
        printf("Store %lx to ", sext_imm_1);
    else
        printf("Store %d to ", sext_imm_1);
#endif
        store(dest, reg_immBar_flag_0, sext_imm_0, reg_0, sext_imm_1, simd);
        if (reg_auto_increasement_flag_0)
            addr_regfile_unit->buffer[reg_0]++;
        (*PC)++;
    } else if (opcode == 5) {       // mv dest src imm/reg(reg(++)) imm/reg(reg(++))
#ifdef PROFILE
        printf("Move ");
#endif
        data = load(src, reg_immBar_flag_1, sext_imm_1, reg_1, simd);
        store(dest, reg_immBar_flag_0, sext_imm_0, reg_0, data, simd);
        if (reg_auto_increasement_flag_0)
            addr_regfile_unit->buffer[reg_0]++;
        if (reg_auto_increasement_flag_1)
            addr_regfile_unit->buffer[reg_1]++;
        (*PC)++;
    } else if (opcode == 8) {       // bne rs1 rs2 offset
        rs1 = sext_imm_1;
        rs2 = reg_1;
#ifdef PROFILE
        printf("bne %d %d %d", rs1, rs2, sext_imm_0);
#endif
        if (reg_immBar_flag_1) comp_0 = addr_regfile_unit->buffer[rs1];
        else comp_0 = sext_imm_1;
        comp_1 = addr_regfile_unit->buffer[rs2];
#ifdef PROFILE
        printf(" (%d %d)", comp_0, comp_1);
#endif
        if (comp_0 != comp_1) {
            *PC = *PC + sext_imm_0;
#ifdef PROFILE
            printf(" jump.\t");
#endif
        } else {
            (*PC)++;
#ifdef PROFILE
            printf(" not jump.\t");
#endif
        }
    } else if (opcode == 9) {       // beq rs1 rs2 offset
        rs1 = sext_imm_1;
        rs2 = reg_1;
#ifdef PROFILE
        printf("beq %d %d %d", rs1, rs2, sext_imm_0);
#endif
        if (reg_immBar_flag_1) comp_0 = addr_regfile_unit->buffer[rs1];
        else comp_0 = sext_imm_1;
        comp_1 = addr_regfile_unit->buffer[rs2];
#ifdef PROFILE
        printf(" (%d %d)", comp_0, comp_1);
#endif
        if (comp_0 == comp_1) {
            *PC = *PC + sext_imm_0;
#ifdef PROFILE
            printf(" jump.\t");
#endif
        } else {
            (*PC)++;
#ifdef PROFILE
            printf(" not jump.\t");
#endif
        }
    } else if (opcode == 10) {       // bge rs1 rs2 offset
        rs1 = sext_imm_1;
        rs2 = reg_1;
#ifdef PROFILE
        printf("bge %d %d %d", rs1, rs2, sext_imm_0);
#endif
        if (reg_immBar_flag_1) comp_0 = addr_regfile_unit->buffer[rs1];
        else comp_0 = sext_imm_1;
        comp_1 = addr_regfile_unit->buffer[rs2];
#ifdef PROFILE
        printf(" (%d %d)", comp_0, comp_1);
#endif
        if (comp_0 >= comp_1) {
            *PC = *PC + sext_imm_0;
#ifdef PROFILE
            printf(" jump.\t");
#endif
        } else {
            (*PC)++;
#ifdef PROFILE
            printf(" not jump.\t");
#endif
        }
    } else if (opcode == 11) {       // blt rs1 rs2 offset
        rs1 = sext_imm_1;
        rs2 = reg_1;
#ifdef PROFILE
        printf("blt %d %d %d", rs1, rs2, sext_imm_0);
#endif
        if (reg_immBar_flag_1) comp_0 = addr_regfile_unit->buffer[rs1];
        else comp_0 = sext_imm_1;
        comp_1 = addr_regfile_unit->buffer[rs2];
#ifdef PROFILE
        printf(" (%d %d)", comp_0, comp_1);
#endif
        if (comp_0 < comp_1) {
            *PC = *PC + sext_imm_0;
#ifdef PROFILE
            printf(" jump.\t");
#endif
        } else {
            (*PC)++;
#ifdef PROFILE
            printf(" not jump.\t");
#endif
        }
    } else if (opcode == 12) {      // jump
        *PC = *PC + sext_imm_0;
#ifdef PROFILE
        printf("jump %d\t", sext_imm_0);
#endif
    } else if (opcode == 13) {      // set PE_PC
        comp_PC = sext_imm_0;
#ifdef PROFILE
        printf("set PC to %d.\t", sext_imm_0);
#endif
        (*PC)++;
    } else if (opcode == 14) {      // None
        (*PC)++;
#ifdef PROFILE
        printf("No-op.\t");
#endif
    } else if (opcode == 15) {      // halt
#ifdef PROFILE
        printf("wait.\t");
#endif
    } else {
        fprintf(stderr, "PE[%d] control instruction opcode error.\n", id);
        exit(-1);
    }
    return 0;
}

int pe::get_gr_10() {
    return addr_regfile_unit->buffer[10];
}

void pe::show_comp_reg() {
    int i;
    for (i = 0; i < REGFILE_ADDR_NUM; i++)
        regfile_unit->show_data(i);
}

