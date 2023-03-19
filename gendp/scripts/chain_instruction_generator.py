import sys
import os

reg = 0
gr = 1
SPM = 2
comp_ib = 3
ctrl_ib = 4
in_buf = 5
out_buf = 6
in_port = 7
in_instr = 8
out_port = 9
out_instr = 10
fifo = [11, 12, 13, 14]

add = 0
sub = 1
addi = 2
set_8 = 3
si = 4
mv = 5
add_8 = 6
addi_8 = 7
bne = 8
beq = 9
bge = 10
blt = 11
jump = 12
set_PC = 13
none = 14
halt = 15

CHAIN_COMPUTE_INSTRUCTION_NUM = 12

PE_INIT_CONSTANT_AND_INSTRUCTION = 1
PE_INIT = 86
PE_RUN = 105



def compute_instruction(op_0, op_1, op_2, in_addr_0, in_addr_1, in_addr_2, in_addr_3, in_addr_4, in_addr_5, out_addr):
    instr = "0" * 14 \
            + "{:0>5b}".format(op_0) \
            + "{:0>5b}".format(op_1) \
            + "{:0>5b}".format(op_2) \
            + "{:0>5b}".format(in_addr_0) \
            + "{:0>5b}".format(in_addr_1) \
            + "{:0>5b}".format(in_addr_2) \
            + "{:0>5b}".format(in_addr_3) \
            + "{:0>5b}".format(in_addr_4) \
            + "{:0>5b}".format(in_addr_5) \
            + "{:0>5b}".format(out_addr)
    value = int(instr, 2)
    return hex(value) + "\n"
    
    
def data_movement_instruction(dest, src, reg_immBar_0, reg_auto_increase_0, imm_0, reg_0, reg_immBar_1, reg_auto_increase_1, imm_1, reg_1, opcode):
    instr = "0" * 20 \
            + "{:0>4b}".format(dest) \
            + "{:0>4b}".format(src) \
            + "{:0>1b}".format(reg_immBar_0) \
            + "{:0>1b}".format(reg_auto_increase_0) \
            + "{:0>10b}".format(imm_0 & 0x3ff) \
            + "{:0>4b}".format(reg_0) \
            + "{:0>1b}".format(reg_immBar_1) \
            + "{:0>1b}".format(reg_auto_increase_1) \
            + "{:0>10b}".format(imm_1 & 0x3ff) \
            + "{:0>4b}".format(reg_1) \
            + "{:0>4b}".format(opcode)
    value = int(instr, 2)
    return hex(value) + "\n"
    
    
def chain_compute():
    
    f = open("instructions/chain/compute_instruction.txt", "w")
    
    f.write(compute_instruction(1, 15, 9, 11, 13, 0, 0, 0, 0, 7))       # 0 ri-11 qi-12 rj-13 qj-14
    f.write(compute_instruction(1, 15, 9, 12, 14, 0, 0, 0, 0, 8))
    
    f.write(compute_instruction(1, 1, 5, 7, 8, 0, 0, 8, 7, 9))          # 1 
    f.write(compute_instruction(14, 15, 9, 7, 0, 1, 0, 0, 0, 10))
    
    f.write(compute_instruction(2, 15, 9, 9, 5, 0, 0, 0, 0, 17))        # 2
    f.write(compute_instruction(13, 15, 9, 8, 0, 10, 1, 0, 0, 10))
    
    f.write(compute_instruction(2, 15, 9, 9, 22, 0, 0, 0, 0, 23))       # 3
    f.write(compute_instruction(13, 15, 9, 8, 3, 1, 10, 0, 0, 10))
    
    f.write(compute_instruction(7, 9, 0, 17, 0, 0, 0, 23, 0, 17))       # 4
    f.write(compute_instruction(13, 15, 9, 9, 4, 1, 10, 0, 0, 10))
    
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))       # 5
    f.write(compute_instruction(11, 15, 9, 9, 0, 0, 0, 0, 0, 18))
    
    f.write(compute_instruction(8, 9, 0, 17, 0, 0, 0, 18, 0, 17))       # 6 qspan-6
    f.write(compute_instruction(6, 9, 6, 7, 8, 0, 0, 6, 0, 18))
    
    f.write(compute_instruction(1, 9, 0, 18, 17, 0, 0, 20, 0, 17))      # 7 score[j]-20
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))

    f.write(compute_instruction(13, 15, 9, 10, 0, 21, 17, 0, 0, 17))    # 8
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))
    
    f.write(compute_instruction(13, 15, 9, 15, 17, 15, 17, 0, 0, 15))   # 9 j-19 score[i]-15 index[i]-16
    f.write(compute_instruction(13, 15, 9, 15, 17, 16, 19, 0, 0, 16))
    
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))
    
    f.write(compute_instruction(16, 15, 15, 0, 0, 0, 0, 0, 0, 0))       # 11
    f.write(compute_instruction(16, 15, 15, 0, 0, 0, 0, 0, 0, 0))
    
    f.close()


# dest, src, flag_0, flag_1, imm/reg_0, reg_0(++), flag_2, flag_3, imm/reg_1, reg_1(++), opcode
def chain_main_instruction():
    
    f = open("instructions/chain/main_instruction.txt", "w")
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 1, 0, 0, 0, 64, 0, si))                                  # gr[1] = pe_group_size
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 2, 0, 0, 0, 0, 0, si))                                   # gr[2] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, in_buf, 0, 0, 3, 0, 0, 1, 0, 2, mv))                              # gr[3] = input[gr[2]++]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, PE_INIT_CONSTANT_AND_INSTRUCTION, 0, 0, 0, 0, 0, set_PC)) # PE_PC = consts&instr
    for i in range(7):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 0, 1, 0, 2, mv))                    # PE[0] = input[gr[2]++]
    for i in range(CHAIN_COMPUTE_INSTRUCTION_NUM):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(out_instr, comp_ib, 0, 0, 0, 0, 0, 0, i, 0, mv))                  # PE[0] = instr[i]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 1, 13, bne))                                  # bne 1 gr[13] 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 4, 0, 0, 0, -64, 0, si))                                 # gr[4] = -64
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, PE_INIT, 0, 0, 0, 0, 0, set_PC))                          # PE_PC = init
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 1, 0, 3, 0, 1, 0, 3, 1, add))                                   # gr[3] = gr[3] + gr[1]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 4, 0, 0, 0, 1, 4, addi))                                  # gr[4]++
    for i in range(3):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 0, 1, 0, 2, mv))                    # PE[0] = input[gr[2]++]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, -4, 0, 0, 0, -1, 4, bge))                                 # bge -1 gr[4] -4
    for i in range(2):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op

    for i in range(2):
        f.write(data_movement_instruction(fifo[i], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                    # FIFO[i] = in
        f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 0, 1, 0, 2, mv))                    # PE[0] = input[gr[2]++]
    for i in range(2):
        f.write(data_movement_instruction(fifo[2+i], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                  # FIFO[i] = in
        f.write(data_movement_instruction(out_port, fifo[i], 0, 0, 0, 0, 0, 0, 0, 0, mv))                   # out = fifo[i]
    f.write(data_movement_instruction(fifo[3], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO[3] = in
    f.write(data_movement_instruction(out_port, fifo[2], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = fifo[2]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[3], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = fifo[3]
    for i in range(6):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(out_buf, in_port, 0, 1, 0, 4, 0, 0, 0, 0, mv))                        # output[gr[4]++] = in
    f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 0, 1, 0, 2, mv))                        # PE[0] = input[gr[2]++]

    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, PE_RUN, 0, 0, 0, 0, 0, set_PC))                           # PE_PC = run
    for i in range(2):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    for i in range(2):
        f.write(data_movement_instruction(fifo[i], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                    # FIFO[i] = in
        f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 0, 1, 0, 2, mv))                    # PE[0] = input[gr[2]++]
    f.write(data_movement_instruction(fifo[2], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO[2] = in
    f.write(data_movement_instruction(out_port, fifo[3], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = fifo[3]
    for i in range(3):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(out_port, fifo[i], 0, 0, 0, 0, 0, 0, 0, 0, mv))                   # out = fifo[i]
    for i in range(8):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(out_buf, in_port, 0, 1, 0, 4, 0, 0, 0, 0, mv))                        # output[gr[4]++] = in
    f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 0, 1, 0, 2, mv))                        # PE[0] = input[gr[2]++]
    f.write(data_movement_instruction(fifo[3], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO[3] = in
    f.write(data_movement_instruction(0, 0, 0, 0, -13, 0, 1, 0, 4, 3, blt))                                 # blt gr[4] gr[3] -13

    # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    f.close()



def pe_instruction(i):
    
    f = open("instructions/chain/pe_{}_instruction.txt".format(i), "w")

    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op                 1
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                  # gr[10] = 0
    
    for j in range(i):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        
    f.write(data_movement_instruction(reg, in_port, 0, 0, 1, 0, 0, 0, 0, 0, mv))                            # reg[1] = in           2 + i
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 2, 0, 0, 0, 0, 0, mv))                            # reg[2] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 1, 0, mv))                           # out = reg[1]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 21, 0, 0, 0, 0, 0, mv))                           # reg[21] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 2, 0, mv))                           # out = reg[2]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 3, 0, 0, 0, 0, 0, mv))                            # reg[3] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 21, 0, mv))                          # out = reg[21]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 4, 0, 0, 0, 0, 0, mv))                            # reg[4] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 3, 0, mv))                           # out = reg[3]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 5, 0, 0, 0, 0, 0, mv))                            # reg[5] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 4, 0, mv))                           # out = reg[4]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 22, 0, 0, 0, 0, 0, mv))                           # reg[22] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 5, 0, mv))                           # out = reg[5]
    f.write(data_movement_instruction(comp_ib, in_instr, 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # ir[0] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 22, 0, mv))                          # out = reg[22]
    for j in range(CHAIN_COMPUTE_INSTRUCTION_NUM-1):
        f.write(data_movement_instruction(comp_ib, in_instr, 0, 0, j+1, 0, 0, 0, 0, 0, mv))                 # ir[j+1] = in
        f.write(data_movement_instruction(out_instr, comp_ib, 0, 0, 0, 0, 0, 0, j, 0, mv))                  # out = ir[j]
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 1, 0, si))                                  # gr[10] = 1            21 + i
    f.write(data_movement_instruction(out_instr, comp_ib, 0, 0, 0, 0, 0, 0, CHAIN_COMPUTE_INSTRUCTION_NUM-1, 0, mv))

    for j in range(63-i):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    # f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 1, 0, si))                                  # gr[10] = 1
    # f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt                  85
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op

    f.write(data_movement_instruction(reg, 0, 0, 0, 16, 0, 0, 0, -1, 0, si))                                # reg[16] = -1
    f.write(data_movement_instruction(gr, 0, 0, 0, 1, 0, 0, 0, -64, 0, si))                                 # gr[1] = -64
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 1, 0, 0, 0, 1, 1, addi))                                  # gr[1]++

    f.write(data_movement_instruction(reg, in_port, 0, 0, 11, 0, 0, 0, 0, 0, mv))                           # reg[11] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 12, 0, 0, 0, 0, 0, mv))                           # reg[12] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 12, 0, mv))                          # out = reg[12]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 6, 0, mv))                           # out = reg[6]
    f.write(data_movement_instruction(0, 0, 0, 0, -4, 0, 0, 0, -1, 1, bge))                                 # bge -1 gr[1] -4
    f.write(data_movement_instruction(0, 0, 0, 0, -4, 0, 0, 0, -1, 1, bge))                                 # bge -1 gr[1] -4

    f.write(data_movement_instruction(reg, reg, 0, 0, 15, 0, 0, 0, 6, 0, mv))                               # reg[15] = reg[6]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op

    f.write(data_movement_instruction(reg, in_port, 0, 0, 11, 0, 0, 0, 0, 0, mv))                           # reg[11] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 12, 0, 0, 0, 0, 0, mv))                           # reg[12] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 12, 0, mv))                          # out = reg[12]

    if (i<63 and i%4 == 0):
        f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                       # reg[13] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                       # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 13, 0, mv))                      # out = reg[13]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 19, 0, 0, 0, 0, 0, mv))                       # reg[19] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                      # out = reg[14]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 20, 0, 0, 0, 0, 0, mv))                       # reg[20] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 19, 0, mv))                      # out = reg[19]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 20, 0, mv))                      # out = reg[20]
        for j in range(4):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
    elif (i<63 and i%4 == 1):
        for j in range(2):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                       # reg[13] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                       # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 13, 0, mv))                      # out = reg[13]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 19, 0, 0, 0, 0, 0, mv))                       # reg[19] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                      # out = reg[14]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 20, 0, 0, 0, 0, 0, mv))                       # reg[20] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 19, 0, mv))                      # out = reg[19]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 20, 0, mv))                      # out = reg[20]
        for j in range(2):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
    elif (i<63 and i%4 == 2):
        for j in range(4):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                       # reg[13] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                       # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 13, 0, mv))                      # out = reg[13]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 19, 0, 0, 0, 0, 0, mv))                       # reg[19] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                      # out = reg[14]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 20, 0, 0, 0, 0, 0, mv))                       # reg[20] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 19, 0, mv))                      # out = reg[19]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 20, 0, mv))                      # out = reg[20]
    elif (i<63 and i%4 == 3):
        for j in range(6):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                       # reg[13] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                       # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 13, 0, mv))                      # out = reg[13]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 19, 0, 0, 0, 0, 0, mv))                       # reg[19] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                      # out = reg[14]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 20, 0, 0, 0, 0, 0, mv))                       # reg[20] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 19, 0, mv))                      # out = reg[19]
    elif i==63:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 1, 0, mv))                        # out = gr[1]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 15, 0, mv))                      # out = reg[15]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 1, 0, 0, 0, 1, 1, addi))                              # gr[1]++
        f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                       # reg[13] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                       # reg[14] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 19, 0, 0, 0, 0, 0, mv))                       # reg[19] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 20, 0, 0, 0, 0, 0, mv))                       # reg[20] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op

    if i < 63:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                        # reg[6] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 6, 0, mv))                       # out = reg[6]
    else:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                        # reg[6] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 15, 0, mv))                      # out = reg[15]

    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt                  104   
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    if i == 0:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op             105
    else:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 15, 0, 0, 0, 0, 0, mv))                       # reg[15] = in      105
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 15, 0, mv))                          # out = reg[15]

    if i == 0:
        f.write(data_movement_instruction(reg, reg, 0, 0, 15, 0, 0, 0, 6, 0, mv))                           # reg[15] = reg[6]
    else:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, set_PC))                                # set_PC 0

    f.write(data_movement_instruction(reg, in_port, 0, 0, 11, 0, 0, 0, 0, 0, mv))                           # reg[11] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 12, 0, 0, 0, 0, 0, mv))                           # reg[12] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 12, 0, mv))                          # out = reg[12]

    if (i<63 and i%4 == 0):
        f.write(data_movement_instruction(reg, in_port, 0, 0, 20, 0, 0, 0, 0, 0, mv))                       # reg[20] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                       # reg[13] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 20, 0, mv))                      # out = reg[20]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                       # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 13, 0, mv))                      # out = reg[13]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 19, 0, 0, 0, 0, 0, mv))                       # reg[19] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                      # out = reg[14]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 19, 0, mv))                      # out = reg[19]
        for j in range(6):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
    elif (i<63 and i%4 == 1):
        for j in range(2):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 20, 0, 0, 0, 0, 0, mv))                       # reg[20] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                       # reg[13] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 20, 0, mv))                      # out = reg[20]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                       # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 13, 0, mv))                      # out = reg[13]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 19, 0, 0, 0, 0, 0, mv))                       # reg[19] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                      # out = reg[14]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 19, 0, mv))                      # out = reg[19]
        for j in range(4):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
    elif (i<63 and i%4 == 2):
        for j in range(4):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 20, 0, 0, 0, 0, 0, mv))                       # reg[20] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                       # reg[13] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 20, 0, mv))                      # out = reg[20]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                       # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 13, 0, mv))                      # out = reg[13]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 19, 0, 0, 0, 0, 0, mv))                       # reg[19] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                      # out = reg[14]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 19, 0, mv))                      # out = reg[19]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    elif (i<63 and i%4 == 3):
        for j in range(6):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 20, 0, 0, 0, 0, 0, mv))                       # reg[20] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                       # reg[13] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 20, 0, mv))                      # out = reg[20]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                       # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 13, 0, mv))                      # out = reg[13]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 19, 0, 0, 0, 0, 0, mv))                       # reg[19] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                      # out = reg[14]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    elif i==63:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 1, 0, mv))                        # out = gr[1]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 1, 0, 0, 0, 1, 1, addi))                              # gr[1]++
        for j in range(2):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 20, 0, 0, 0, 0, 0, mv))                       # reg[20] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                       # reg[13] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 20, 0, mv))                      # out = reg[20]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                       # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 13, 0, mv))                      # out = reg[13]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 19, 0, 0, 0, 0, 0, mv))                       # reg[19] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                      # out = reg[14]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op

    if i < 63:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 6, 0, mv))                           # out = reg[6]
    else:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 15, 0, mv))                          # out = reg[15]

    if i < 63:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 16, 0, 0, 0, 0, 0, mv))                           # reg[16] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 16, 0, mv))                          # out = reg[16]
    else:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 16, 0, 0, 0, 0, 0, mv))                           # reg[16] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 15, 0, mv))                          # out = reg[15]


           
    f.close()

if not os.path.exists("instructions/chain"):
    os.makedirs("instructions/chain")
chain_compute()
chain_main_instruction()
for i in range(64):
    pe_instruction(i)
