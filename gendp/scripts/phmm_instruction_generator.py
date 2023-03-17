import sys

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
fifo = [11, 12]

add = 0
sub = 1
addi = 2
mod = 3
si = 4
mv = 5
bne = 8
beq = 9
bge = 10
blt = 11
jump = 12
set_PC = 13
none = 14
halt = 15

PHMM_COMPUTE_INSTRUCTION_NUM = 15

PE_INIT_CONSTANT_AND_INSTRUCTION = 1
PE_GROUP = 36+1
PE_RUN_0 = 62+1
PE_RUN_1 = 72+1
PE_RUN_2 = 82+1
PE_RUN_3 = 92+1
PE_RUN_4 = 92+11
PE_RUN_5 = 92+21
PE_RUN_6 = 92+31
PE_LAST = 103+36
PE_END_0 = 108+36
PE_END_1 = 115+36
PE_END_2 = 121+36
PE_END_3 = 125+36
# PE_RUN_LAST = 93+1
# PE_END_0 = 108+1
# PE_END_1 = 115+1
# PE_END_2 = 121+1
# PE_END_3 = 125+1

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
    
    
def phmm_compute():
    
    f = open("../data/phmm/compute_instruction.txt", "w")
    
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))   # instruction 0
    f.write(compute_instruction(10, 15, 2, 13, 14, 15, 0, 0, 0, 16))

    f.write(compute_instruction(0, 9, 0, 16, 7, 0, 0, 2, 0, 17))    # instruction 1
    f.write(compute_instruction(0, 9, 0, 16, 8, 0, 0, 2, 0, 18))

    f.write(compute_instruction(0, 9, 0, 16, 6, 0, 0, 3, 0, 19))    # instruction 2
    f.write(compute_instruction(1, 15, 9, 17, 18, 0, 0, 0, 0, 16))

    f.write(compute_instruction(5, 12, 0, 17, 18, 0, 0, 16, 0, 17)) # instruction 3
    f.write(compute_instruction(0, 0, 1, 23, 5, 0, 0, 25, 1, 18))

    f.write(compute_instruction(1, 15, 9, 19, 17, 0, 0, 0, 0, 16))  # instruction 4
    f.write(compute_instruction(0, 0, 5, 23, 5, 0, 0, 25, 1, 20))

    f.write(compute_instruction(0, 0, 1, 9, 4, 0, 0, 10, 1, 21))    # instruction 5
    f.write(compute_instruction(9, 15, 9, 25, 0, 0, 0, 0, 0, 12)) 

    f.write(compute_instruction(9, 12, 0, 20, 0, 0, 0, 18, 0, 25))  # instruction 6
    f.write(compute_instruction(0, 0, 5, 9, 4, 0, 0, 10, 1, 18))    

    f.write(compute_instruction(9, 15, 9, 24, 0, 0, 0, 0, 0, 26))   # instruction 7
    f.write(compute_instruction(9, 15, 9, 23, 0, 0, 0, 0, 0, 11))
    
    f.write(compute_instruction(9, 12, 0, 18, 0, 0, 0, 21, 0, 24))  # instruction 8
    f.write(compute_instruction(5, 12, 0, 17, 19, 0, 0, 16, 0, 23))
    
    f.write(compute_instruction(16, 15, 15, 0, 0, 0, 0, 0, 0, 0))   # instruction 9
    f.write(compute_instruction(16, 15, 15, 0, 0, 0, 0, 0, 0, 0)) 
    
    f.write(compute_instruction(1, 15, 9, 23, 24, 0, 0, 0, 0, 16))   # instruction 10
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))
    
    f.write(compute_instruction(5, 12, 0, 23, 24, 0, 0, 16, 0, 22))  # instruction 11
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))

    # f.write(compute_instruction(1, 15, 9, 16, 22, 0, 0, 0, 0, 17))  # instruction 12
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))
    
    # f.write(compute_instruction(5, 12, 0, 16, 22, 0, 0, 17, 0, 22)) # instruction 13
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))

    f.write(compute_instruction(16, 15, 15, 0, 0, 0, 0, 0, 0, 0))   # instruction 14
    f.write(compute_instruction(16, 15, 15, 0, 0, 0, 0, 0, 0, 0)) 

    f.close()


# dest, src, flag_0, flag_1, imm/reg_0, reg_0(++), flag_2, flag_3, imm/reg_1, reg_1(++), opcode
def phmm_main_instruction():
    
    f = open("../data/phmm/main_instruction.txt", "w")
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 1, 0, 0, 0, 4, 0, si))                                   # gr[1] = pe_group_size
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 2, 0, 0, 0, 0, 0, si))                                   # gr[2] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, in_buf, 0, 0, 3, 0, 0, 1, 0, 2, mv))                              # gr[3] = input[gr[2]++]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, in_buf, 0, 0, 4, 0, 0, 1, 0, 2, mv))                              # gr[4] = input[gr[2]++]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, in_buf, 0, 0, 9, 0, 0, 1, 0, 2, mv))                              # gr[9] = input[gr[2]++]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, in_buf, 0, 0, 10, 0, 0, 1, 0, 2, mv))                             # gr[10] = input[gr[2]++]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, in_buf, 0, 0, 11, 0, 0, 1, 0, 2, mv))                             # gr[11] = input[gr[2]++]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, PE_INIT_CONSTANT_AND_INSTRUCTION, 0, 0, 0, 0, 0, set_PC)) # PE_PC = consts&instr
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 0, 1, 0, 2, mv));                       # out = input[gr[2]++]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 0, 1, 0, 2, mv));                       # out = input[gr[2]++]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 10, 0, mv));                          # out = gr[10]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 11, 0, mv));                          # out = gr[11]
    for i in range(PHMM_COMPUTE_INSTRUCTION_NUM):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(out_port, comp_ib, 0, 0, 0, 0, 0, 0, i, 0, mv));                  # out = instr[i]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 1, 0, 6, 0, 1, 0, 2, 3, add))                                   # gr[6] = gr[2] + gr[3]
    for i in range(4):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 1, 0, 6, 0, 1, 0, 6, 3, add))                               # gr[6] = gr[6] + gr[3]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 7, 0, 0, 0, -1, 0, si))                                  # gr[7] = -1
    f.write(data_movement_instruction(fifo[0], gr, 0, 0, 0, 0, 0, 0, 0, 0, mv))                             # FIFO_M = gr[0]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(fifo[0], gr, 0, 0, 0, 0, 0, 0, 0, 0, mv))                             # FIFO_M = gr[0]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(fifo[1], gr, 0, 0, 0, 0, 0, 0, 10, 0, mv))                            # FIFO_XY = gr[10]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(fifo[1], gr, 0, 0, 0, 0, 0, 0, 0, 0, mv))                             # FIFO_XY = gr[0]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(fifo[1], gr, 0, 0, 0, 0, 0, 0, 0, 0, mv))                             # FIFO_XY = gr[0]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 1, 0, 7, 0, 0, 0, 1, 7, addi))                                  # gr[7]++
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, -6, 0, 1, 0, 7, 4, bne))                                  # bne gr[7] gr[4] -6
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 1, 0, 7, 0, 1, 0, 3, 3, add))                                   # gr[7] = gr[3] + gr[3]
    for i in range(3):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 1, 0, 7, 0, 1, 0, 7, 3, add))                               # gr[7] = gr[7] + gr[3]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 8, 0, 0, 0, 20, 0, si))                                  # gr[8] = 20
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 1, 0, 4, 0, 0, 0, 1, 4, addi))                                  # gr[4]+=1
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 1, 0, 7, 0, 1, 0, 7, 8, add))                                   # gr[7] = gr[7] + gr[8]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 107, 0, 1, 0, 8, 7, bge))                                 # bge gr[8] gr[7] 107
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, PE_GROUP, 0, 0, 0, 0, 0, set_PC))                         # PE_PC = PE_GROUP

    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 5, 0, 0, 0, 0, 0, si))                                   # gr[5] = 0
    for i in range(3):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 1, 0, 3, 0, 0, 0, -1, 8, addi))                                 # gr[3] = gr[8] - 1
    for i in range(20):
        f.write(data_movement_instruction(0, 0, 1, 0, 3, 0, 0, 0, -1, 3, addi))                             # gr[3]--
        f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 1, 0, 2, 3, mv))                    # out = input[gr[2](gr[3])]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 1, 1, 6, 5, mv))                        # out = input[gr[6](gr[5]++)]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 1, 0, 8, 0, 0, 0, 20, 8, addi))                                 # gr[8] = gr[8] + 20
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[0], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_M
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[0], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_M
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, PE_RUN_0, 0, 0, 0, 0, 0, set_PC))                         # PE_PC = run
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[0], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_M
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(out_port, fifo[0], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_M
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 1, 1, 6, 5, mv))                        # out = input[gr[6](gr[5]++)]
    f.write(data_movement_instruction(fifo[0], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_M = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(fifo[0], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_M = in
    for i in range(3):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, PE_RUN_1, 0, 0, 0, 0, 0, set_PC))                         # PE_PC = run
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[0], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_M
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(out_port, fifo[0], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_M
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 1, 1, 6, 5, mv))                        # out = input[gr[6](gr[5]++)]
    f.write(data_movement_instruction(fifo[0], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_M = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(fifo[0], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_M = in
    for i in range(3):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op

    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, PE_RUN_2, 0, 0, 0, 0, 0, set_PC))                         # PE_PC = run
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[0], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_M
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(out_port, fifo[0], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_M
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 1, 1, 6, 5, mv))                        # out = input[gr[6](gr[5]++)]
    f.write(data_movement_instruction(fifo[0], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_M = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(fifo[0], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_M = in
    for i in range(3):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op

    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, PE_RUN_3, 0, 0, 0, 0, 0, set_PC))                         # PE_PC = run
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[0], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_M
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(out_port, fifo[0], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_M
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 1, 1, 6, 5, mv))                        # out = input[gr[6](gr[5]++)]
    f.write(data_movement_instruction(fifo[0], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_M = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(fifo[0], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_M = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, -9, 0, 1, 0, 4, 5, bne))                                  # bne gr[4] gr[5] -9
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, PE_RUN_4, 0, 0, 0, 0, 0, set_PC))                         # PE_PC = run
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[0], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_M
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(out_port, fifo[0], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_M
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 1, 1, 6, 5, mv))                        # out = input[gr[6](gr[5]++)]
    f.write(data_movement_instruction(fifo[0], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_M = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(fifo[0], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_M = in
    for i in range(3):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op

    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, PE_RUN_5, 0, 0, 0, 0, 0, set_PC))                         # PE_PC = run
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[0], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_M
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(out_port, fifo[0], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_M
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 1, 1, 6, 5, mv))                        # out = input[gr[6](gr[5]++)]
    f.write(data_movement_instruction(fifo[0], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_M = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(fifo[0], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_M = in
    for i in range(3):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op

    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, PE_RUN_6, 0, 0, 0, 0, 0, set_PC))                         # PE_PC = run
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, fifo[0], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_M
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(out_port, fifo[0], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_M
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_XY
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 1, 1, 6, 5, mv))                        # out = input[gr[6](gr[5]++)]
    f.write(data_movement_instruction(fifo[0], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_M = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(fifo[0], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_M = in
    for i in range(3):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_XY = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(fifo[0], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_M = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(fifo[0], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_M = in
    f.write(data_movement_instruction(0, 0, 0, 0, -106, 0, 0, 0, 0, 0, beq))                                # beq 0 gr[0] -106
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, PE_LAST, 0, 0, 0, 0, 0, set_PC))                          # PE_PC = last
    
    for i in range(6):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op

    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 7, 0, 0, 0, 0, 9, bne))                                   # bne 0 gr[9] 7
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, PE_END_0, 0, 0, 0, 0, 0, set_PC))                         # PE_PC = end_0
    for i in range(6):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(out_buf, in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # output[0] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 6, 0, 0, 0, 1, 9, bne))                                   # bne 1 gr[9] 6
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, PE_END_1, 0, 0, 0, 0, 0, set_PC))                         # PE_PC = end_1
    for i in range(4):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(out_buf, in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # output[0] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 5, 0, 0, 0, 2, 9, bne))                                   # bne 2 gr[9] 5
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, PE_END_2, 0, 0, 0, 0, 0, set_PC))                         # PE_PC = end_2
    for i in range(2):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(out_buf, in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # output[0] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, PE_END_3, 0, 0, 0, 0, 0, set_PC))                         # PE_PC = end_3
    f.write(data_movement_instruction(out_buf, in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # output[0] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    f.close()
    
def pe_instruction(i):
    
    f = open("../data/phmm/pe_{}_instruction.txt".format(i), "w")

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
    f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                            # reg[8] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 2, 0, mv))                           # out = reg[2]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 22, 0, 0, 0, 0, 0, mv))                           # reg[22] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 8, 0, mv))                           # out = reg[8]
    f.write(data_movement_instruction(comp_ib, in_instr, 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # ir[0] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 22, 0, mv))                          # out = reg[22]
    for j in range(PHMM_COMPUTE_INSTRUCTION_NUM-1):
        f.write(data_movement_instruction(comp_ib, in_instr, 0, 0, j+1, 0, 0, 0, 0, 0, mv))                 # ir[j+1] = in
        f.write(data_movement_instruction(out_instr, comp_ib, 0, 0, 0, 0, 0, 0, j, 0, mv))                  # out = ir[j]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op                 19 + i
    f.write(data_movement_instruction(out_instr, comp_ib, 0, 0, 0, 0, 0, 0, PHMM_COMPUTE_INSTRUCTION_NUM-1, 0, mv))
    for j in range(2):
        f.write(data_movement_instruction(reg, 0, 0, 0, j+6, 0, 0, 0, 0, 0, si))                            # reg[j+6] = 0
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 2, 0, 0, 0, 1-i, 0, si))                                 # gr[2] = 0-i
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    for j in range(4):
        f.write(data_movement_instruction(reg, 0, 0, 0, j+9, 0, 0, 0, 0, 0, si))                            # reg[j+9] = 0
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    for j in range(3):
        f.write(data_movement_instruction(reg, 0, 0, 0, j+23, 0, 0, 0, 0, 0, si))                           # reg[j+23] = 0
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    for j in range(3-i):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt                  35
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt

    f.write(data_movement_instruction(gr, 0, 0, 0, 2, 0, 0, 0, 0-i, 0, si))                                 # gr[2] = 0-i
    if i == 0:
        f.write(data_movement_instruction(reg, 0, 0, 0, 27, 0, 0, 0, 1, 0, si))                             # reg[27] = 1
    else:
        f.write(data_movement_instruction(reg, 0, 0, 0, 27, 0, 0, 0, 0, 0, si))                             # reg[27] = 0
    f.write(data_movement_instruction(reg, 0, 0, 0, 24, 0, 0, 0, 0, 0, si))                                 # reg[24] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 23, 0, 0, 0, 0, 0, si))                                 # reg[23] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 25, 0, 0, 0, 0, 0, si))                                 # reg[25] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        
    if i == 0:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 15, 0, 0, 0, 0, 0, mv))                       # reg[15] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                       # reg[13] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 15, 0, mv))                      # out = reg[15]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 5, 0, 0, 0, 0, 0, mv))                        # reg[5] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 13, 0, mv))                      # out = reg[13]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 4, 0, 0, 0, 0, 0, mv))                        # reg[4] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 5, 0, mv))                       # out = reg[5]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 3, 0, 0, 0, 0, 0, mv))                        # reg[3] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 4, 0, mv))                       # out = reg[4]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 15, 0, 0, 0, 0, 0, mv))                       # reg[15] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 3, 0, mv))                       # out = reg[3]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                       # reg[13] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 15, 0, mv))                      # out = reg[15]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 5, 0, 0, 0, 0, 0, mv))                        # reg[5] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 13, 0, mv))                      # out = reg[13]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 4, 0, 0, 0, 0, 0, mv))                        # reg[4] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 5, 0, mv))                       # out = reg[5]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 3, 0, 0, 0, 0, 0, mv))                        # reg[3] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 4, 0, mv))                       # out = reg[4]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 15, 0, 0, 0, 0, 0, mv))                       # reg[15] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 3, 0, mv))                       # out = reg[3]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                       # reg[13] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 15, 0, mv))                      # out = reg[15]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 5, 0, 0, 0, 0, 0, mv))                        # reg[5] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 13, 0, mv))                      # out = reg[13]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 4, 0, 0, 0, 0, 0, mv))                        # reg[4] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 5, 0, mv))                       # out = reg[5]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 3, 0, 0, 0, 0, 0, mv))                        # reg[3] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 4, 0, mv))                       # out = reg[4]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 15, 0, 0, 0, 0, 0, mv))                       # reg[15] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 3, 0, mv))                       # out = reg[3]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                       # reg[13] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 5, 0, 0, 0, 0, 0, mv))                        # reg[5] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 4, 0, 0, 0, 0, 0, mv))                        # reg[4] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 3, 0, 0, 0, 0, 0, mv))                        # reg[3] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                       # reg[14] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    elif i == 1:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 15, 0, 0, 0, 0, 0, mv))                       # reg[15] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                       # reg[13] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 15, 0, mv))                      # out = reg[15]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 5, 0, 0, 0, 0, 0, mv))                        # reg[5] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 13, 0, mv))                      # out = reg[13]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 4, 0, 0, 0, 0, 0, mv))                        # reg[4] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 5, 0, mv))                       # out = reg[5]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 3, 0, 0, 0, 0, 0, mv))                        # reg[3] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 4, 0, mv))                       # out = reg[4]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 15, 0, 0, 0, 0, 0, mv))                       # reg[15] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 3, 0, mv))                       # out = reg[3]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                       # reg[13] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 15, 0, mv))                      # out = reg[15]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 5, 0, 0, 0, 0, 0, mv))                        # reg[5] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 13, 0, mv))                      # out = reg[13]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 4, 0, 0, 0, 0, 0, mv))                        # reg[4] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 5, 0, mv))                       # out = reg[5]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 3, 0, 0, 0, 0, 0, mv))                        # reg[3] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 4, 0, mv))                       # out = reg[4]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 15, 0, 0, 0, 0, 0, mv))                       # reg[15] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 3, 0, mv))                       # out = reg[3]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                       # reg[13] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 5, 0, 0, 0, 0, 0, mv))                        # reg[5] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 4, 0, 0, 0, 0, 0, mv))                        # reg[4] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 3, 0, 0, 0, 0, 0, mv))                        # reg[3] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        for j in range(10):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
    elif i == 2:
        for j in range(4):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 15, 0, 0, 0, 0, 0, mv))                       # reg[15] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                       # reg[13] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 15, 0, mv))                      # out = reg[15]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 5, 0, 0, 0, 0, 0, mv))                        # reg[5] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 13, 0, mv))                      # out = reg[13]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 4, 0, 0, 0, 0, 0, mv))                        # reg[4] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 5, 0, mv))                       # out = reg[5]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 3, 0, 0, 0, 0, 0, mv))                        # reg[3] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 4, 0, mv))                       # out = reg[4]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 15, 0, 0, 0, 0, 0, mv))                       # reg[15] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 3, 0, mv))                       # out = reg[3]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                       # reg[13] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 5, 0, 0, 0, 0, 0, mv))                        # reg[5] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 4, 0, 0, 0, 0, 0, mv))                        # reg[4] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 3, 0, 0, 0, 0, 0, mv))                        # reg[3] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        for j in range(18):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
    elif i == 3:
        for j in range(6):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 15, 0, 0, 0, 0, 0, mv))                       # reg[15] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                       # reg[13] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 5, 0, 0, 0, 0, 0, mv))                        # reg[5] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 4, 0, 0, 0, 0, 0, mv))                        # reg[4] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 3, 0, 0, 0, 0, 0, mv))                        # reg[3] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        for j in range(26):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt                  61
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    if i == 0:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    else:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                        # reg[8] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 12, 0, mv))                          # out = reg[12]
    
    if i == 0:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                        # reg[8] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, set_PC))                            # set_PC 0
    else:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    
    if i == 0:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 9, 0, 0, 0, 0, 0, mv))                            # reg[9] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
        for j in range(2):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 1, 0, 2, 0, 0, 0, 1, 2, addi))                                  # gr[2]++
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    elif i != 3:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 9, 0, 0, 0, 0, 0, mv))                            # reg[9] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
        for j in range(2):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 1, 0, 2, 0, 0, 0, 1, 2, addi))                                  # gr[2]++
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    else:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 9, 0, 0, 0, 0, 0, mv))                            # reg[9] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 12, 0, mv))                          # out = reg[12]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
        f.write(data_movement_instruction(0, 0, 1, 0, 2, 0, 0, 0, 1, 2, addi))                                  # gr[2]++
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
        
    f.write(data_movement_instruction(reg, gr, 0, 0, 27, 0, 0, 0, 2, 0, mv))                                    # reg[27] = gr[2]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                      # No-op

    if i == 0:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    else:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                        # reg[8] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 12, 0, mv))                          # out = reg[12]
    
    if i == 0:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                        # reg[8] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, set_PC))                            # set_PC 0
    elif i == 1:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, set_PC))                            # set_PC 0
    else:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    
    if i == 0:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 9, 0, 0, 0, 0, 0, mv))                            # reg[9] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
        for j in range(2):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 1, 0, 2, 0, 0, 0, 1, 2, addi))                                  # gr[2]++
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    elif i != 3:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 9, 0, 0, 0, 0, 0, mv))                            # reg[9] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
        for j in range(2):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 1, 0, 2, 0, 0, 0, 1, 2, addi))                                  # gr[2]++
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    else:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 9, 0, 0, 0, 0, 0, mv))                            # reg[9] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 12, 0, mv))                          # out = reg[12]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
        f.write(data_movement_instruction(0, 0, 1, 0, 2, 0, 0, 0, 1, 2, addi))                                  # gr[2]++
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
        
    f.write(data_movement_instruction(reg, gr, 0, 0, 27, 0, 0, 0, 2, 0, mv))                                    # reg[27] = gr[2]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                      # No-op

    if i == 0:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    else:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                        # reg[8] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 12, 0, mv))                          # out = reg[12]
    
    if i == 0:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                        # reg[8] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, set_PC))                            # set_PC 0
    elif i <= 2:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, set_PC))                            # set_PC 0
    else:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    
    if i == 0:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 9, 0, 0, 0, 0, 0, mv))                            # reg[9] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
        for j in range(2):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 1, 0, 2, 0, 0, 0, 1, 2, addi))                                  # gr[2]++
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    elif i != 3:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 9, 0, 0, 0, 0, 0, mv))                            # reg[9] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
        for j in range(2):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 1, 0, 2, 0, 0, 0, 1, 2, addi))                                  # gr[2]++
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    else:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 9, 0, 0, 0, 0, 0, mv))                            # reg[9] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 12, 0, mv))                          # out = reg[12]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
        f.write(data_movement_instruction(0, 0, 1, 0, 2, 0, 0, 0, 1, 2, addi))                                  # gr[2]++
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
        
    f.write(data_movement_instruction(reg, gr, 0, 0, 27, 0, 0, 0, 2, 0, mv))                                    # reg[27] = gr[2]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                      # No-op

    if i == 0:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    else:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                        # reg[8] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 12, 0, mv))                          # out = reg[12]
    
    if i == 0:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                        # reg[8] = in
    else:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, set_PC))                                # set_PC 0
    
    if i == 0:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 9, 0, 0, 0, 0, 0, mv))                            # reg[9] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
        for j in range(2):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 1, 0, 2, 0, 0, 0, 1, 2, addi))                                  # gr[2]++
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    elif i != 3:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 9, 0, 0, 0, 0, 0, mv))                            # reg[9] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
        for j in range(2):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 1, 0, 2, 0, 0, 0, 1, 2, addi))                                  # gr[2]++
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    else:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 9, 0, 0, 0, 0, 0, mv))                            # reg[9] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 12, 0, mv))                          # out = reg[12]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
        f.write(data_movement_instruction(0, 0, 1, 0, 2, 0, 0, 0, 1, 2, addi))                                  # gr[2]++
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
        
    f.write(data_movement_instruction(reg, gr, 0, 0, 27, 0, 0, 0, 2, 0, mv))                                    # reg[27] = gr[2]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                      # No-op
    
    if i == 0:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    else:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                        # reg[8] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 12, 0, mv))                          # out = reg[12]
    
    if i == 0:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                        # reg[8] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    else:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, set_PC))                            # set_PC 0
    
    if i == 0:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 9, 0, 0, 0, 0, 0, mv))                            # reg[9] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
        for j in range(2):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 1, 0, 2, 0, 0, 0, 1, 2, addi))                                  # gr[2]++
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    elif i != 3:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 9, 0, 0, 0, 0, 0, mv))                            # reg[9] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
        for j in range(2):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 1, 0, 2, 0, 0, 0, 1, 2, addi))                                  # gr[2]++
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    else:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 9, 0, 0, 0, 0, 0, mv))                            # reg[9] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 12, 0, mv))                          # out = reg[12]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
        f.write(data_movement_instruction(0, 0, 1, 0, 2, 0, 0, 0, 1, 2, addi))                                  # gr[2]++
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
        
    f.write(data_movement_instruction(reg, gr, 0, 0, 27, 0, 0, 0, 2, 0, mv))                                    # reg[27] = gr[2]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                      # No-op
    
    if i == 0:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    else:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                        # reg[8] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 12, 0, mv))                          # out = reg[12]
    
    if i == 0:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                        # reg[8] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    elif i < 2:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    else:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, set_PC))                            # set_PC 0
    
    if i == 0:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 9, 0, 0, 0, 0, 0, mv))                            # reg[9] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
        for j in range(2):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 1, 0, 2, 0, 0, 0, 1, 2, addi))                                  # gr[2]++
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    elif i != 3:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 9, 0, 0, 0, 0, 0, mv))                            # reg[9] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
        for j in range(2):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 1, 0, 2, 0, 0, 0, 1, 2, addi))                                  # gr[2]++
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    else:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 9, 0, 0, 0, 0, 0, mv))                            # reg[9] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 12, 0, mv))                          # out = reg[12]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
        f.write(data_movement_instruction(0, 0, 1, 0, 2, 0, 0, 0, 1, 2, addi))                                  # gr[2]++
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
        
    f.write(data_movement_instruction(reg, gr, 0, 0, 27, 0, 0, 0, 2, 0, mv))                                    # reg[27] = gr[2]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                      # No-op
    
    if i == 0:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    else:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                        # reg[8] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 12, 0, mv))                          # out = reg[12]
    
    if i == 0:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                        # reg[8] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    elif i < 3:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    else:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, set_PC))                            # set_PC 0
    
    if i == 0:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 9, 0, 0, 0, 0, 0, mv))                            # reg[9] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
        for j in range(2):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 1, 0, 2, 0, 0, 0, 1, 2, addi))                                  # gr[2]++
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    elif i != 3:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 9, 0, 0, 0, 0, 0, mv))                            # reg[9] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
        for j in range(2):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
        f.write(data_movement_instruction(0, 0, 1, 0, 2, 0, 0, 0, 1, 2, addi))                                  # gr[2]++
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    else:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 9, 0, 0, 0, 0, 0, mv))                            # reg[9] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 12, 0, mv))                          # out = reg[12]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
        f.write(data_movement_instruction(0, 0, 1, 0, 2, 0, 0, 0, 1, 2, addi))                                  # gr[2]++
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
        
    f.write(data_movement_instruction(reg, gr, 0, 0, 27, 0, 0, 0, 2, 0, mv))                                    # reg[27] = gr[2]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                      # No-op

    if i < 3:
        for j in range(10):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    else:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 12, 0, mv))                          # out = reg[12]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # wait
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # wait
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 10, 0, 0, 0, 0, 0, set_PC))                               # set_PC 10
    
    for j in range(10):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    
    if i == 0:
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op 
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 22, 0, mv))                      # out = reg[22]
        for j in range(34):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
    elif i == 1:
        f.write(data_movement_instruction(reg, in_port, 0, 0, 22, 0, 0, 0, 0, 0, mv))                       # reg[22] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op 
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op 
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 22, 0, mv))                      # out = reg[22]
        for j in range(11):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 22, 0, mv))                      # out = reg[22]
        for j in range(20):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
    elif i == 2:
        for j in range(2):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 22, 0, 0, 0, 0, 0, mv))                       # reg[22] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op 
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op 
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 22, 0, mv))                      # out = reg[22]
        for j in range(8):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 22, 0, 0, 0, 0, 0, mv))                       # reg[22] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op 
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op 
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 22, 0, mv))                      # out = reg[22]
        for j in range(9):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 22, 0, mv))                      # out = reg[22]
        for j in range(8):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
    else:
        for j in range(4):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 22, 0, 0, 0, 0, 0, mv))                       # reg[22] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op 
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op 
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 22, 0, mv))                      # out = reg[22]
        for j in range(8):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 22, 0, 0, 0, 0, 0, mv))                       # reg[22] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op 
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op 
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 22, 0, mv))                      # out = reg[22]
        for j in range(6):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 22, 0, 0, 0, 0, 0, mv))                       # reg[22] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op 
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op 
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 22, 0, mv))                      # out = reg[22]
        for j in range(5):
            f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                          # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 22, 0, mv))                      # out = reg[22]
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt

    f.close()


phmm_compute()
phmm_main_instruction()
for i in range(4):
    pe_instruction(i)
