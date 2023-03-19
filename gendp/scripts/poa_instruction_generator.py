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

POA_COMPUTE_INSTRUCTION_NUM = 29

PE_INIT_CONSTANT_AND_INSTRUCTION = 1
PE_GROUP = 49
PE_SEND_PRED_INDEX = 70
PE_RUN = 75
PE_OUTPUT = 105

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
    
    
def poa_compute():
    
    f = open("instructions/poa/compute_instruction.txt", "w")
    
    # 0 ~ 10
    f.write(compute_instruction(9, 15, 9, 0, 0, 0, 0, 0, 0, 19))        # 0
    f.write(compute_instruction(9, 15, 9, 18, 0, 0, 0, 0, 0, 23))
    f.write(compute_instruction(9, 15, 9, 0, 0, 0, 0, 0, 0, 24))        # 1
    f.write(compute_instruction(9, 15, 9, 0, 0, 0, 0, 0, 0, 22))
    f.write(compute_instruction(9, 15, 9, 18, 0, 0, 0, 0, 0, 26))       # 2
    f.write(compute_instruction(9, 15, 9, 0, 0, 0, 0, 0, 0, 27))
    f.write(compute_instruction(9, 15, 9, 0, 0, 0, 0, 0, 0, 25))        # 3
    f.write(compute_instruction(9, 15, 9, 0, 0, 0, 0, 0, 0, 28))
    f.write(compute_instruction(9, 15, 9, 0, 0, 0, 0, 0, 0, 29))        # 4
    f.write(compute_instruction(9, 15, 9, 0, 0, 0, 0, 0, 0, 30))
    f.write(compute_instruction(14, 15, 9, 1, 0, 14, 13, 0, 0, 20))     # 5 FIFO / Prev PE upper gap_y
    f.write(compute_instruction(0, 15, 9, 9, 15, 0, 0, 0, 0, 9))
    f.write(compute_instruction(14, 15, 9, 1, 17, 0, 20, 0, 0, 20))     # 6 FIFO / Prev PE upper gap_y
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))
    f.write(compute_instruction(1, 15, 9, 2, 20, 0, 0, 0, 0, 21))       # 7 FIFO / Prev PE upper score
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))
    f.write(compute_instruction(13, 15, 9, 21, 18, 21, 18, 0, 0, 23))   # 8
    f.write(compute_instruction(13, 15, 9, 21, 18, 15, 0, 0, 0, 24))
    f.write(compute_instruction(13, 15, 9, 21, 18, 1, 0, 0, 0, 22))     # 9
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))
    
    # 10 ~ 18
    f.write(compute_instruction(0, 15, 9, 19, 15, 0, 0, 0, 0, 19))      # 10 SPM left gap_x
    f.write(compute_instruction(14, 15, 9, 4, 0, 14, 13, 0, 0, 20))
    f.write(compute_instruction(14, 15, 9, 4, 17, 0, 20, 0, 0, 20))     # 11 
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))
    f.write(compute_instruction(1, 15, 9, 5, 20, 0, 0, 0, 0, 21))       # 12
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))
    f.write(compute_instruction(13, 15, 9, 21, 26, 19, 27, 0, 0, 27))   # 13
    f.write(compute_instruction(13, 15, 9, 3, 28, 19, 29, 0, 0, 29))
    f.write(compute_instruction(13, 15, 9, 21, 26, 4, 25, 0, 0, 25))    # 14 SPM left score
    f.write(compute_instruction(13, 15, 9, 3, 28, 15, 30, 0, 0, 30))
    f.write(compute_instruction(13, 15, 9, 21, 26, 21, 26, 0, 0, 26))   # 15 SPM diag score
    f.write(compute_instruction(13, 15, 9, 3, 28, 3, 28, 0, 0, 28))
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))       # 16
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))       # 17
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))       # 18
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))

    # 19 ~ 28
    f.write(compute_instruction(0, 15, 9, 25, 15, 0, 0, 0, 0, 21))      # 19
    f.write(compute_instruction(0, 15, 9, 22, 15, 0, 0, 0, 0, 20))
    f.write(compute_instruction(13, 15, 9, 16, 25, 21, 25, 0, 0, 21))   # 20
    f.write(compute_instruction(13, 15, 9, 16, 22, 20, 22, 0, 0, 20))
    f.write(compute_instruction(13, 15, 9, 26, 23, 26, 23, 0, 0, 19))   # 21
    f.write(compute_instruction(10, 9, 1, 12, 10, 0, 0, 28, 0, 28))
    f.write(compute_instruction(13, 15, 9, 26, 23, 21, 20, 0, 0, 20))   # 22
    f.write(compute_instruction(9, 15, 9, 2, 0, 0, 0, 0, 0, 3))      # store upper score to diag score
    f.write(compute_instruction(13, 15, 9, 26, 23, 27, 0, 0, 0, 22))    # 23
    f.write(compute_instruction(13, 15, 9, 26, 23, 0, 24, 0, 0, 21))
    f.write(compute_instruction(13, 15, 9, 28, 19, 28, 19, 0, 0, 5))    # 24
    f.write(compute_instruction(13, 15, 9, 28, 19, 0, 20, 0, 0, 4))
    f.write(compute_instruction(13, 15, 9, 28, 19, 0, 20, 0, 0, 1))     # 25
    f.write(compute_instruction(13, 15, 9, 28, 19, 29, 22, 0, 0, 24))
    f.write(compute_instruction(13, 15, 9, 28, 19, 30, 21, 0, 0, 23))   # 26 Best x
    f.write(compute_instruction(13, 15, 9, 5, 6, 9, 7, 0, 0, 7))
    f.write(compute_instruction(13, 15, 9, 5, 6, 11, 8, 0, 0, 8))       # 27 Best y Best score
    f.write(compute_instruction(13, 15, 9, 5, 6, 5, 6, 0, 0, 6))
    f.write(compute_instruction(16, 15, 15, 0, 0, 0, 0, 0, 0, 0))       # 28 halt
    f.write(compute_instruction(16, 15, 15, 0, 0, 0, 0, 0, 0, 0))
    
    f.close()


# dest, src, flag_0, flag_1, imm/reg_0, reg_0(++), flag_2, flag_3, imm/reg_1, reg_1(++), opcode
def poa_main_instruction():
    
    f = open("instructions/poa/main_instruction.txt", "w")
    
    f.write(data_movement_instruction(gr, 0, 0, 0, 1, 0, 0, 0, 4, 0, si))                                   # gr[1] = pe_group_size
    f.write(data_movement_instruction(gr, 0, 0, 0, 2, 0, 0, 0, 0, 0, si))                                   # gr[2] = 0
    f.write(data_movement_instruction(gr, in_buf, 0, 0, 3, 0, 0, 1, 0, 2, mv))                              # gr[3] = input[gr[2]++]
    f.write(data_movement_instruction(gr, in_buf, 0, 0, 4, 0, 0, 0, 0, 2, mv))                              # gr[4] = input[gr[2]]
    f.write(data_movement_instruction(0, 0, 0, 0, PE_INIT_CONSTANT_AND_INSTRUCTION, 0, 0, 0, 0, 0, set_PC))    # PE_PC = consts&instr
    for i in range (8):
        f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 0, 1, 0, 2, mv));                   # out = input[gr[2]++]
    for i in range(POA_COMPUTE_INSTRUCTION_NUM):
        f.write(data_movement_instruction(out_port, comp_ib, 0, 0, 0, 0, 0, 0, i, 0, mv));                  # out = instr[i]
    f.write(data_movement_instruction(0, 0, 1, 0, 5, 0, 0, 0, 0, 1, addi))                                  # gr[5] = gr[1]
    f.write(data_movement_instruction(0, 0, 1, 0, 5, 0, 0, 0, -1, 5, addi))                                 # gr[5]--
    f.write(data_movement_instruction(fifo[0], in_buf, 0, 0, 0, 0, 0, 0, 7, 0, mv))                         # FIFO_gap_y = input[7]
    f.write(data_movement_instruction(fifo[1], in_buf, 0, 0, 0, 0, 0, 0, 2, 0, mv))                         # FIFO_score = input[2]
    f.write(data_movement_instruction(0, 0, 1, 0, 5, 0, 0, 0, 1, 5, addi))                                  # gr[5]++
    f.write(data_movement_instruction(0, 0, 0, 0, -3, 0, 1, 0, 5, 4, bne))                                  # bne gr[5] gr[4] -3
    f.write(data_movement_instruction(gr, 0, 0, 0, 14, 0, 0, 0, 0, 0, si))                                  # gr[14] = 0
    f.write(data_movement_instruction(0, 0, 1, 0, 7, 0, 1, 0, 2, 3, add))                                   # gr[7] = gr[2] + gr[3]
    f.write(data_movement_instruction(0, 0, 1, 0, 3, 0, 1, 0, 3, 1, add))                                   # gr[3] = gr[3] + gr[1]
    f.write(data_movement_instruction(0, 0, 1, 0, 6, 0, 0, 0, 0, 1, addi))                                  # gr[6] = gr[1]
    f.write(data_movement_instruction(0, 0, 0, 0, 58, 0, 1, 0, 6, 3, bge))                                  # bge gr[6] gr[3] 58
    f.write(data_movement_instruction(0, 0, 0, 0, PE_GROUP, 0, 0, 0, 0, 0, set_PC))                            # PE_PC = PE_GROUP
    f.write(data_movement_instruction(0, 0, 1, 0, 5, 0, 0, 0, -1, 6, addi))                                 # gr[5] = gr[6] - 1
    f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 1, 0, 2, 5, mv))                        # out = input[gr[2](gr[5])]
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 5, 0, mv))                            # out = gr[5]
    f.write(data_movement_instruction(0, 0, 1, 0, 5, 0, 0, 0, -1, 5, addi))                                 # gr[5]--
    f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 1, 0, 2, 5, mv))                        # out = input[gr[2](gr[5])]
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 5, 0, mv))                            # out = gr[5]
    f.write(data_movement_instruction(0, 0, 1, 0, 5, 0, 0, 0, -1, 5, addi))                                 # gr[5]--
    f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 1, 0, 2, 5, mv))                        # out = input[gr[2](gr[5])]
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 5, 0, mv))                            # out = gr[5]
    f.write(data_movement_instruction(0, 0, 1, 0, 5, 0, 0, 0, -1, 5, addi))                                 # gr[5]--
    f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 1, 0, 2, 5, mv))                        # out = input[gr[2](gr[5])]
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 5, 0, mv))                            # out = gr[5]
    f.write(data_movement_instruction(0, 0, 1, 0, 6, 0, 1, 0, 6, 1, add))                                   # gr[6] = gr[6] + gr[1]
    f.write(data_movement_instruction(gr, 0, 0, 0, 12, 0, 0, 0, 0, 0, si))                                  # gr[12] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 1, 0, 8, 0, 1, 0, 7, 4, add))                                   # gr[8] = gr[7] + gr[4]
    f.write(data_movement_instruction(0, 0, 1, 0, 9, 0, 1, 0, 8, 4, add))                                   # gr[9] = gr[8] + gr[4]
    f.write(data_movement_instruction(gr, in_buf, 0, 0, 10, 0, 0, 1, 0, 8, mv))                             # gr[10] = input[gr[8]++]
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 10, 0, mv))                           # out = gr[10]
    f.write(data_movement_instruction(0, 0, 1, 0, 11, 0, 1, 0, 9, 10, add))                                 # gr[11] = gr[9] + gr[10]
    
    f.write(data_movement_instruction(0, 0, 0, 0, PE_SEND_PRED_INDEX, 0, 0, 0, 0, 0, set_PC))                  # PE_PC = send_pred_index
    f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 0, 1, 0, 9, mv))                        # out = input[gr[9]++]
    f.write(data_movement_instruction(0, 0, 0, 0, -1, 0, 1, 0, 11, 9, bne))                                 # bne gr[11] gr[9] -1
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 1, 13, bne))                                  # bne 1 gr[13] 0
    
    f.write(data_movement_instruction(0, 0, 0, 0, PE_RUN, 0, 0, 0, 0, 0, set_PC))                              # PE_PC = run
    for i in range(2):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 1, 1, 7, 12, mv))                       # out = input[gr[7](gr[12]++)]
    f.write(data_movement_instruction(out_port, fifo[0], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_gap_y
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_score
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 1, 13, bne))                                  # bne 1 gr[13] 0
    for i in range(3):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op

    f.write(data_movement_instruction(0, 0, 0, 0, PE_OUTPUT, 0, 0, 0, 0, 0, set_PC))                           # PE_PC = output
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, in_buf, 0, 0, 10, 0, 0, 1, 0, 8, mv))                             # gr[10] = input[gr[8]++]
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 10, 0, mv))                           # out = gr[10]
    f.write(data_movement_instruction(0, 0, 1, 0, 11, 0, 1, 0, 9, 10, add))                                 # gr[11] = gr[9] + gr[10]
    for i in range(3):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    # f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 1, 0, 12, 1, blt))                                  # blt gr[12] gr[1] 3
    f.write(data_movement_instruction(fifo[0], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_gap_y = in
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_score = in
    for i in range(8):
        f.write(data_movement_instruction(out_buf, in_port, 0, 1, 0, 14, 0, 0, 0, 0, mv))                   # output[gr[14]++] = in
    f.write(data_movement_instruction(0, 0, 0, 0, -34, 0, 1, 0, 12, 4, bne))                                # bne gr[12] gr[4]  -34
    f.write(data_movement_instruction(0, 0, 0, 0, -57, 0, 0, 0, 0, 0, beq))                                 # beq 0 0 -57
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    
    # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    f.close()
    
def pe_0_instruction():
    
    f = open("instructions/poa/pe_0_instruction.txt", "w")
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt              0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, in_port, 0, 0, 1, 0, 0, 0, 0, 0, mv))                             # gr[1] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                            # reg[0] = in
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 1, 0, mv))                            # out = gr[1]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 15, 0, 0, 0, 0, 0, mv))                           # reg[15] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 0, 0, mv))                           # out = reg[0]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                           # reg[13] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 15, 0, mv))                          # out = reg[15]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 13, 0, mv))                          # out = reg[13]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 16, 0, 0, 0, 0, 0, mv))                           # reg[16] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 17, 0, 0, 0, 0, 0, mv))                           # reg[17] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 16, 0, mv))                          # out = reg[16]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 18, 0, 0, 0, 0, 0, mv))                           # reg[18] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 17, 0, mv))                          # out = reg[17]
    f.write(data_movement_instruction(comp_ib, in_instr, 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # ir[0] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 18, 0, mv))                          # out = reg[18]
    for i in range(POA_COMPUTE_INSTRUCTION_NUM-1):
        f.write(data_movement_instruction(comp_ib, in_instr, 0, 0, i+1, 0, 0, 0, 0, 0, mv))                 # ir[i+1] = in
        f.write(data_movement_instruction(out_instr, comp_ib, 0, 0, 0, 0, 0, 0, i, 0, mv))                  # out = ir[i]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_instr, comp_ib, 0, 0, 0, 0, 0, 0, 28, 0, mv))                     # out = ir[28]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    for i in range(10):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op             45
           
    for i in range(10):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
    f.write(data_movement_instruction(gr, 0, 0, 0, 5, 0, 0, 0, 0, 0, si))                                   # gr[5] = 0
    f.write(data_movement_instruction(reg, in_port, 0, 0, 11, 0, 0, 0, 0, 0, mv))                           # reg[11] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 10, 0, mv))                          # out = reg[10]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                  # gr[10] = 0
    f.write(data_movement_instruction(reg, in_port, 0, 0, 11, 0, 0, 0, 0, 0, mv))                           # reg[11] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 10, 0, mv))                          # out = reg[10]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 11, 0, 0, 0, 0, 0, mv))                           # reg[11] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 10, 0, mv))                          # out = reg[10]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 11, 0, 0, 0, 0, 0, mv))                           # reg[11] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 9, 0, 0, 0, -1, 0, si))                                 # reg[9] = -1
    f.write(data_movement_instruction(gr, 0, 0, 0, 6, 0, 0, 0, 0, 0, si))                                   # gr[6] = 0
    for i in range(2):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(SPM, 0, 0, 0, 271, 0, 0, 0, 0, 0, si))                                # SPM[271] = 0
    f.write(data_movement_instruction(reg, reg, 0, 0, 6, 0, 0, 0, 18, 0, mv))                               # reg[6] = reg[18]
    f.write(data_movement_instruction(SPM, 0, 0, 0, 143, 0, 0, 0, 17, 0, si))                               # SPM[143] = 17
    f.write(data_movement_instruction(reg, reg, 0, 0, 7, 0, 0, 0, -1, 0, si))                               # reg[7] = -1
    f.write(data_movement_instruction(SPM, 0, 0, 0, 399, 0, 0, 0, 0, 0, si))                                # SPM[399] = 0
    f.write(data_movement_instruction(reg, reg, 0, 0, 8, 0, 0, 0, -1, 0, si))                               # reg[8] = -1
    f.write(data_movement_instruction(gr, gr, 0, 0, 4, 0, 0, 0, 2, 0, mv))                                  # gr[4] = gr[2]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, in_port, 0, 0, 2, 0, 0, 0, 0, 0, mv))                             # gr[2] = in
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 4, 0, mv))                            # out = gr[4]
    f.write(data_movement_instruction(gr, 0, 0, 0, 3, 0, 0, 0, 0, 0, si))                                   # gr[3] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op             69

    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(SPM, in_port, 0, 1, 0, 3, 0, 0, 0, 0, mv))                            # SPM[gr[3]++] = in
    f.write(data_movement_instruction(out_port, SPM, 0, 0, 0, 0, 0, 1, 0, 5, mv))                           # out = SPM[gr[5]++]
    f.write(data_movement_instruction(0, 0, 0, 0, -1, 0, 1, 0, 3, 2, blt))                                  # blt gr[3] gr[2] -1
    f.write(data_movement_instruction(0, 0, 0, 0, -1, 0, 1, 0, 5, 4, blt))                                  # blt gr[5] gr[4] -1
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 1, 0, si))                                  # gr[10] = 1
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op             74
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, set_PC))                                   # set_PC 0
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                  # gr[10] = 0
    f.write(data_movement_instruction(reg, gr, 0, 0, 31, 0, 0, 0, 2, 0, mv))                                # reg[31] = gr[2]
    f.write(data_movement_instruction(gr, 0, 0, 0, 4, 0, 0, 0, 0, 0, si))                                   # gr[4] = 0
    f.write(data_movement_instruction(reg, in_port, 0, 0, 12, 0, 0, 0, 0, 0, mv))                           # reg[12] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 12, 0, mv))                          # out = reg[12]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 1, 0, 0, 0, 0, 0, mv))                            # reg[1] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 1, 0, mv))                           # out = reg[1]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 2, 0, 0, 0, 0, 0, mv))                            # reg[2] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 5, 0, mv))                           # out = reg[5]
    f.write(data_movement_instruction(gr, SPM, 0, 0, 7, 0, 0, 1, 0, 4, mv))                                 # gr[7] = SPM[gr[4]++]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 1, 0, 8, 0, 1, 0, 6, 7, sub))                                   # gr[8] = gr[6] - gr[7]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, -1, 8, blt))                                  # blt -1 gr[8] 3
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, -1, 8, blt))                                  # blt -1 gr[8] 3
    f.write(data_movement_instruction(0, 0, 1, 0, 8, 0, 0, 0, 128, 8, addi))                                # gr[8] = gr[8] + 128
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 3
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 3
    for i in range(4):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(reg, SPM, 0, 0, 4, 0, 0, 0, 16, 8, mv))                               # reg[4] = SPM[16(gr[8])]
    f.write(data_movement_instruction(gr, reg, 0, 0, 11, 0, 0, 0, 9, 0, mv))                                # gr[11] = reg[9]
    f.write(data_movement_instruction(reg, SPM, 0, 0, 5, 0, 0, 0, 144, 8, mv))                              # reg[5] = SPM[144(gr[8])]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, SPM, 0, 0, 3, 0, 0, 0, 272, 8, mv))                              # reg[3] = SPM[272(gr[8])]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 12, 0, 1, 0, 2, 4, beq))                                  # beq gr[2] gr[4] 12
    f.write(data_movement_instruction(0, 0, 0, 0, 12, 0, 1, 0, 2, 4, beq))                                  # beq gr[2] gr[4] 12
    f.write(data_movement_instruction(gr, SPM, 0, 0, 7, 0, 0, 1, 0, 4, mv))                                 # gr[7] = SPM[gr[4]++]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 1, 0, 8, 0, 1, 0, 6, 7, sub))                                   # gr[8] = gr[6] - gr[7]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, -1, 8, blt))                                  # blt -1 gr[8] 3
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, -1, 8, blt))                                  # blt -1 gr[8] 3
    f.write(data_movement_instruction(0, 0, 1, 0, 8, 0, 0, 0, 128, 8, addi))                                # gr[8] = gr[8] + 128
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 3
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 3
    for i in range(4):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(reg, SPM, 0, 0, 4, 0, 0, 0, 16, 8, mv))                               # reg[4] = SPM[16(gr[8])]
    f.write(data_movement_instruction(0, 0, 0, 0, 10, 0, 0, 0, 0, 0, set_PC))                                  # set_PC 10
    f.write(data_movement_instruction(reg, SPM, 0, 0, 5, 0, 0, 0, 144, 8, mv))                              # reg[5] = SPM[144(gr[8])]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, SPM, 0, 0, 3, 0, 0, 0, 272, 8, mv))                              # reg[3] = SPM[272(gr[8])]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, -10, 0, 1, 0, 2, 4, bne))                                 # bne gr[2] gr[4] -10
    f.write(data_movement_instruction(0, 0, 0, 0, -10, 0, 1, 0, 2, 4, bne))                                 # bne gr[2] gr[4] -10
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 1, 0, si))                                  # gr[10] = 1
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt              104
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 8, 0, 0, 0, -1, 11, blt))                                 # blt -1 gr[11] 8
    f.write(data_movement_instruction(0, 0, 0, 0, 8, 0, 0, 0, -1, 11, blt))                                 # blt -1 gr[11] 8
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                  # gr[10] = 0
    for i in range(10):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 8, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 8
    f.write(data_movement_instruction(0, 0, 0, 0, 8, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 8
    f.write(data_movement_instruction(gr, 0, 0, 0, 5, 0, 0, 0, 0, 0, si))                                   # gr[5] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, gr, 0, 0, 4, 0, 0, 0, 2, 0, mv))                                  # gr[4] = gr[2]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, in_port, 0, 0, 2, 0, 0, 0, 0, 0, mv))                             # gr[2] = in
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 4, 0, mv))                            # out = gr[4]
    f.write(data_movement_instruction(gr, 0, 0, 0, 3, 0, 0, 0, 0, 0, mv))                                   # gr[3] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(SPM, reg, 0, 0, 272, 6, 0, 0, 3, 0, mv))                              # SPM[272(gr[6])] = reg[3]
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                  # gr[10] = 0
    f.write(data_movement_instruction(SPM, reg, 0, 0, 16, 6, 0, 0, 4, 0, mv))                               # SPM[16(gr[6])] = reg[4]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(SPM, reg, 0, 1, 144, 6, 0, 0, 5, 0, mv))                              # SPM[144(gr[6]++)] = reg[5]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 2, 0, 0, 0, 127, 6, bge))                                 # bge 127 gr[6] 2
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
    f.write(data_movement_instruction(gr, 0, 0, 0, 6, 0, 0, 0, 0, 0, si))                                   # gr[6] = 0
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
        
    f.close()

def pe_1_instruction():
    
    f = open("instructions/poa/pe_1_instruction.txt", "w")
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    for i in range(4):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(gr, in_port, 0, 0, 1, 0, 0, 0, 0, 0, mv))                             # gr[1] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                            # reg[0] = in
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 1, 0, mv))                            # out = gr[1]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 15, 0, 0, 0, 0, 0, mv))                           # reg[15] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 0, 0, mv))                           # out = reg[0]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                           # reg[13] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 15, 0, mv))                          # out = reg[15]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 13, 0, mv))                          # out = reg[13]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 16, 0, 0, 0, 0, 0, mv))                           # reg[16] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 17, 0, 0, 0, 0, 0, mv))                           # reg[17] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 16, 0, mv))                          # out = reg[16]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 18, 0, 0, 0, 0, 0, mv))                           # reg[18] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 17, 0, mv))                          # out = reg[17]
    f.write(data_movement_instruction(comp_ib, in_instr, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # ir[0] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 18, 0, mv))                          # out = reg[18]
    for i in range(POA_COMPUTE_INSTRUCTION_NUM-1):
        f.write(data_movement_instruction(comp_ib, in_instr, 0, 0, i+1, 0, 0, 0, 0, 0, mv))                 # ir[i+1] = in
        f.write(data_movement_instruction(out_instr, comp_ib, 0, 0, 0, 0, 0, 0, i, 0, mv))                  # out = ir[i]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_instr, comp_ib, 0, 0, 0, 0, 0, 0, 28, 0, mv))                     # out = ir[28]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    for i in range(8):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op             45
        
    for i in range(10):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 5, 0, 0, 0, 0, 0, si))                                   # gr[5] = 0
    f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                  # gr[10] = 0
    f.write(data_movement_instruction(reg, in_port, 0, 0, 11, 0, 0, 0, 0, 0, mv))                           # reg[11] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 10, 0, mv))                          # out = reg[10]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
    f.write(data_movement_instruction(SPM, 0, 0, 0, 272, 0, 0, 0, 0, 0, si))                                # SPM[272] = 0
    f.write(data_movement_instruction(reg, in_port, 0, 0, 11, 0, 0, 0, 0, 0, mv))                           # reg[11] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 10, 0, mv))                          # out = reg[10]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
    f.write(data_movement_instruction(SPM, 0, 0, 0, 144, 0, 0, 0, 0, 0, si))                                # SPM[144] = 0
    f.write(data_movement_instruction(reg, in_port, 0, 0, 11, 0, 0, 0, 0, 0, mv))                           # reg[11] = in
    f.write(data_movement_instruction(SPM, 0, 0, 0, 16, 0, 0, 0, 17, 0, si))                                # SPM[16] = 17
    f.write(data_movement_instruction(reg, 0, 0, 0, 9, 0, 0, 0, -2, 0, si))                                 # reg[9] = -2
    f.write(data_movement_instruction(gr, 0, 0, 0, 6, 0, 0, 0, 0, 0, si))                                   # gr[6] = 0
    for i in range(6):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(SPM, 0, 0, 0, 271, 0, 0, 0, 0, 0, si))                                # SPM[271] = 0
    f.write(data_movement_instruction(reg, reg, 0, 0, 6, 0, 0, 0, 18, 0, mv))                               # reg[6] = reg[18]
    f.write(data_movement_instruction(SPM, 0, 0, 0, 143, 0, 0, 0, 17, 0, si))                               # SPM[143] = 17
    f.write(data_movement_instruction(reg, reg, 0, 0, 7, 0, 0, 0, -1, 0, si))                               # reg[7] = -1
    f.write(data_movement_instruction(SPM, 0, 0, 0, 399, 0, 0, 0, 0, 0, si))                                # SPM[399] = 0
    f.write(data_movement_instruction(reg, reg, 0, 0, 8, 0, 0, 0, -1, 0, si))                               # reg[8] = -1
    f.write(data_movement_instruction(gr, gr, 0, 0, 4, 0, 0, 0, 2, 0, mv))                                  # gr[4] = gr[2]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 2, 0, 0, 0, 1, 0, si))                                   # gr[2] = 1
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 3, 0, 0, 0, 0, 0, si))                                   # gr[3] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op             69

    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(SPM, in_port, 0, 1, 0, 3, 0, 0, 0, 0, mv))                            # SPM[gr[3]++] = in
    f.write(data_movement_instruction(out_port, SPM, 0, 0, 0, 0, 0, 1, 0, 5, mv))                           # out = SPM[gr[5]++]
    f.write(data_movement_instruction(0, 0, 0, 0, -1, 0, 1, 0, 3, 2, blt))                                  # blt gr[3] gr[2] -1
    f.write(data_movement_instruction(0, 0, 0, 0, -1, 0, 1, 0, 5, 4, blt))                                  # blt gr[5] gr[4] -1
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 1, 0, si))                                  # gr[10] = 1
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op             74
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, set_PC))                                   # set_PC 0
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                  # gr[10] = 0
    f.write(data_movement_instruction(reg, gr, 0, 0, 31, 0, 0, 0, 2, 0, mv))                                # reg[31] = gr[2]
    f.write(data_movement_instruction(gr, 0, 0, 0, 4, 0, 0, 0, 0, 0, si))                                   # gr[4] = 0
    f.write(data_movement_instruction(reg, in_port, 0, 0, 12, 0, 0, 0, 0, 0, mv))                           # reg[12] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 12, 0, mv))                          # out = reg[12]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 1, 0, 0, 0, 0, 0, mv))                            # reg[1] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 1, 0, mv))                           # out = reg[1]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 2, 0, 0, 0, 0, 0, mv))                            # reg[2] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 5, 0, mv))                           # out = reg[5]
    f.write(data_movement_instruction(gr, SPM, 0, 0, 7, 0, 0, 1, 0, 4, mv))                                 # gr[7] = SPM[gr[4]++]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 1, 0, 8, 0, 1, 0, 6, 7, sub))                                   # gr[8] = gr[6] - gr[7]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, -1, 8, blt))                                  # blt -1 gr[8] 3
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, -1, 8, blt))                                  # blt -1 gr[8] 3
    f.write(data_movement_instruction(0, 0, 1, 0, 8, 0, 0, 0, 128, 8, addi))                                # gr[8] = gr[8] + 128
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 3
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 3
    for i in range(4):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(reg, SPM, 0, 0, 4, 0, 0, 0, 16, 8, mv))                               # reg[4] = SPM[16(gr[8])]
    f.write(data_movement_instruction(gr, reg, 0, 0, 11, 0, 0, 0, 9, 0, mv))                                # gr[11] = reg[9]
    f.write(data_movement_instruction(reg, SPM, 0, 0, 5, 0, 0, 0, 144, 8, mv))                              # reg[5] = SPM[144(gr[8])]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, SPM, 0, 0, 3, 0, 0, 0, 272, 8, mv))                              # reg[3] = SPM[272(gr[8])]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 12, 0, 1, 0, 2, 4, beq))                                  # beq gr[2] gr[4] 12
    f.write(data_movement_instruction(0, 0, 0, 0, 12, 0, 1, 0, 2, 4, beq))                                  # beq gr[2] gr[4] 12
    f.write(data_movement_instruction(gr, SPM, 0, 0, 7, 0, 0, 1, 0, 4, mv))                                 # gr[7] = SPM[gr[4]++]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 1, 0, 8, 0, 1, 0, 6, 7, sub))                                   # gr[8] = gr[6] - gr[7]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, -1, 8, blt))                                  # blt -1 gr[8] 3
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, -1, 8, blt))                                  # blt -1 gr[8] 3
    f.write(data_movement_instruction(0, 0, 1, 0, 8, 0, 0, 0, 128, 8, addi))                                # gr[8] = gr[8] + 128
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 3
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 3
    for i in range(4):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(reg, SPM, 0, 0, 4, 0, 0, 0, 16, 8, mv))                               # reg[4] = SPM[16(gr[8])]
    f.write(data_movement_instruction(0, 0, 0, 0, 10, 0, 0, 0, 0, 0, set_PC))                                  # set_PC 10
    f.write(data_movement_instruction(reg, SPM, 0, 0, 5, 0, 0, 0, 144, 8, mv))                              # reg[5] = SPM[144(gr[8])]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, SPM, 0, 0, 3, 0, 0, 0, 272, 8, mv))                              # reg[3] = SPM[272(gr[8])]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, -10, 0, 1, 0, 2, 4, bne))                                 # bne gr[2] gr[4] -10
    f.write(data_movement_instruction(0, 0, 0, 0, -10, 0, 1, 0, 2, 4, bne))                                 # bne gr[2] gr[4] -10
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 1, 0, si))                                  # gr[10] = 1
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt              104
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 8, 0, 0, 0, -1, 11, blt))                                 # blt -1 gr[11] 8
    f.write(data_movement_instruction(0, 0, 0, 0, 8, 0, 0, 0, -1, 11, blt))                                 # blt -1 gr[11] 8
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                  # gr[10] = 0
    for i in range(10):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 8, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 8
    f.write(data_movement_instruction(0, 0, 0, 0, 8, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 8
    f.write(data_movement_instruction(gr, 0, 0, 0, 5, 0, 0, 0, 0, 0, si))                                   # gr[5] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, gr, 0, 0, 4, 0, 0, 0, 2, 0, mv))                                  # gr[4] = gr[2]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, in_port, 0, 0, 2, 0, 0, 0, 0, 0, mv))                             # gr[2] = in
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 4, 0, mv))                            # out = gr[4]
    f.write(data_movement_instruction(gr, 0, 0, 0, 3, 0, 0, 0, 0, 0, mv))                                   # gr[3] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(SPM, reg, 0, 0, 272, 6, 0, 0, 3, 0, mv))                              # SPM[272(gr[6])] = reg[3]
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                  # gr[10] = 0
    f.write(data_movement_instruction(SPM, reg, 0, 0, 16, 6, 0, 0, 4, 0, mv))                               # SPM[16(gr[6])] = reg[4]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(SPM, reg, 0, 1, 144, 6, 0, 0, 5, 0, mv))                              # SPM[144(gr[6]++)] = reg[5]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(SPM, in_port, 0, 0, 400, 0, 0, 0, 0, 0, mv))                          # SPM[400] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
    f.write(data_movement_instruction(SPM, in_port, 0, 0, 401, 0, 0, 0, 0, 0, mv))                          # SPM[401] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
    f.write(data_movement_instruction(0, 0, 0, 0, 2, 0, 0, 0, 127, 6, bge))                                 # bge 127 gr[6] 2
    f.write(data_movement_instruction(out_port, SPM, 0, 0, 0, 0, 0, 0, 400, 0, mv))                         # out = SPM[400]
    f.write(data_movement_instruction(gr, 0, 0, 0, 6, 0, 0, 0, 0, 0, si))                                   # gr[6] = 0
    f.write(data_movement_instruction(out_port, SPM, 0, 0, 0, 0, 0, 0, 401, 0, mv))                         # out = SPM[401]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    f.close()
    
def pe_2_instruction():
    
    f = open("instructions/poa/pe_2_instruction.txt", "w")
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    for i in range(6):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(gr, in_port, 0, 0, 1, 0, 0, 0, 0, 0, mv))                             # gr[1] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                            # reg[0] = in
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 1, 0, mv))                            # out = gr[1]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 15, 0, 0, 0, 0, 0, mv))                           # reg[15] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 0, 0, mv))                           # out = reg[0]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                           # reg[13] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 15, 0, mv))                          # out = reg[15]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 13, 0, mv))                          # out = reg[13]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 16, 0, 0, 0, 0, 0, mv))                           # reg[16] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 17, 0, 0, 0, 0, 0, mv))                           # reg[17] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 16, 0, mv))                          # out = reg[16]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 18, 0, 0, 0, 0, 0, mv))                           # reg[18] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 17, 0, mv))                          # out = reg[17]
    f.write(data_movement_instruction(comp_ib, in_instr, 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # ir[0] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 18, 0, mv))                          # out = reg[18]
    for i in range(POA_COMPUTE_INSTRUCTION_NUM-1):
        f.write(data_movement_instruction(comp_ib, in_instr, 0, 0, i+1, 0, 0, 0, 0, 0, mv))                 # ir[i+1] = in
        f.write(data_movement_instruction(out_instr, comp_ib, 0, 0, 0, 0, 0, 0, i, 0, mv))                  # out = ir[i]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_instr, comp_ib, 0, 0, 0, 0, 0, 0, 28, 0, mv))                     # out = ir[28]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    for i in range(6):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op             45
        
    for i in range(10):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 5, 0, 0, 0, 0, 0, si))                                   # gr[5] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                  # gr[10] = 0
    f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
    f.write(data_movement_instruction(SPM, 0, 0, 0, 272, 0, 0, 0, 0, 0, si))                                # SPM[272] = 0
    f.write(data_movement_instruction(reg, in_port, 0, 0, 11, 0, 0, 0, 0, 0, mv))                           # reg[11] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 10, 0, mv))                          # out = reg[10]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
    f.write(data_movement_instruction(SPM, 0, 0, 0, 144, 0, 0, 0, 0, 0, si))                                # SPM[144] = 0
    f.write(data_movement_instruction(reg, in_port, 0, 0, 11, 0, 0, 0, 0, 0, mv))                           # reg[11] = in
    f.write(data_movement_instruction(SPM, 0, 0, 0, 16, 0, 0, 0, 17, 0, si))                                # SPM[16] = 17
    f.write(data_movement_instruction(reg, 0, 0, 0, 9, 0, 0, 0, -3, 0, si))                                 # reg[9] = -3
    f.write(data_movement_instruction(gr, 0, 0, 0, 6, 0, 0, 0, 0, 0, si))                                   # gr[6] = 0
    for i in range(10):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(SPM, 0, 0, 0, 271, 0, 0, 0, 0, 0, si))                                # SPM[271] = 0
    f.write(data_movement_instruction(reg, reg, 0, 0, 6, 0, 0, 0, 18, 0, mv))                               # reg[6] = reg[18]
    f.write(data_movement_instruction(SPM, 0, 0, 0, 143, 0, 0, 0, 17, 0, si))                               # SPM[143] = 17
    f.write(data_movement_instruction(reg, reg, 0, 0, 7, 0, 0, 0, -1, 0, si))                               # reg[7] = -1
    f.write(data_movement_instruction(SPM, 0, 0, 0, 399, 0, 0, 0, 0, 0, si))                                # SPM[399] = 0
    f.write(data_movement_instruction(reg, reg, 0, 0, 8, 0, 0, 0, -1, 0, si))                               # reg[8] = -1
    f.write(data_movement_instruction(gr, gr, 0, 0, 4, 0, 0, 0, 2, 0, mv))                                  # gr[4] = gr[2]
    f.write(data_movement_instruction(SPM, 0, 0, 0, 145, 0, 0, 0, 0, 0, si))                                # SPM[145] = 0
    f.write(data_movement_instruction(gr, 0, 0, 0, 2, 0, 0, 0, 1, 0, si))                                   # gr[2] = 1
    f.write(data_movement_instruction(SPM, 0, 0, 0, 17, 0, 0, 0, 17, 0, si))                                # SPM[17] = 17
    f.write(data_movement_instruction(gr, 0, 0, 0, 3, 0, 0, 0, 0, 0, si))                                   # gr[3] = 0
    f.write(data_movement_instruction(SPM, 0, 0, 0, 273, 0, 0, 0, 0, 0, si))                                # SPM[273] = 0      69

    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(SPM, in_port, 0, 1, 0, 3, 0, 0, 0, 0, mv))                            # SPM[gr[3]++] = in
    f.write(data_movement_instruction(out_port, SPM, 0, 0, 0, 0, 0, 1, 0, 5, mv))                           # out = SPM[gr[5]++]
    f.write(data_movement_instruction(0, 0, 0, 0, -1, 0, 1, 0, 3, 2, blt))                                  # blt gr[3] gr[2] -1
    f.write(data_movement_instruction(0, 0, 0, 0, -1, 0, 1, 0, 5, 4, blt))                                  # blt gr[5] gr[4] -1
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 1, 0, si))                                  # gr[10] = 1
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op             74
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, set_PC))                                   # set_PC 0
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                  # gr[10] = 0
    f.write(data_movement_instruction(reg, gr, 0, 0, 31, 0, 0, 0, 2, 0, mv))                                # reg[31] = gr[2]
    f.write(data_movement_instruction(gr, 0, 0, 0, 4, 0, 0, 0, 0, 0, si))                                   # gr[4] = 0
    f.write(data_movement_instruction(reg, in_port, 0, 0, 12, 0, 0, 0, 0, 0, mv))                           # reg[12] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 12, 0, mv))                          # out = reg[12]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 1, 0, 0, 0, 0, 0, mv))                            # reg[1] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 1, 0, mv))                           # out = reg[1]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 2, 0, 0, 0, 0, 0, mv))                            # reg[2] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 5, 0, mv))                           # out = reg[5]
    f.write(data_movement_instruction(gr, SPM, 0, 0, 7, 0, 0, 1, 0, 4, mv))                                 # gr[7] = SPM[gr[4]++]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 1, 0, 8, 0, 1, 0, 6, 7, sub))                                   # gr[8] = gr[6] - gr[7]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, -1, 8, blt))                                  # blt -1 gr[8] 3
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, -1, 8, blt))                                  # blt -1 gr[8] 3
    f.write(data_movement_instruction(0, 0, 1, 0, 8, 0, 0, 0, 128, 8, addi))                                # gr[8] = gr[8] + 128
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 3
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 3
    for i in range(4):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(reg, SPM, 0, 0, 4, 0, 0, 0, 16, 8, mv))                               # reg[4] = SPM[16(gr[8])]
    f.write(data_movement_instruction(gr, reg, 0, 0, 11, 0, 0, 0, 9, 0, mv))                                # gr[11] = reg[9]
    f.write(data_movement_instruction(reg, SPM, 0, 0, 5, 0, 0, 0, 144, 8, mv))                              # reg[5] = SPM[144(gr[8])]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, SPM, 0, 0, 3, 0, 0, 0, 272, 8, mv))                              # reg[3] = SPM[272(gr[8])]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 12, 0, 1, 0, 2, 4, beq))                                  # beq gr[2] gr[4] 12
    f.write(data_movement_instruction(0, 0, 0, 0, 12, 0, 1, 0, 2, 4, beq))                                  # beq gr[2] gr[4] 12
    f.write(data_movement_instruction(gr, SPM, 0, 0, 7, 0, 0, 1, 0, 4, mv))                                 # gr[7] = SPM[gr[4]++]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 1, 0, 8, 0, 1, 0, 6, 7, sub))                                   # gr[8] = gr[6] - gr[7]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, -1, 8, blt))                                  # blt -1 gr[8] 3
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, -1, 8, blt))                                  # blt -1 gr[8] 3
    f.write(data_movement_instruction(0, 0, 1, 0, 8, 0, 0, 0, 128, 8, addi))                                # gr[8] = gr[8] + 128
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 3
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 3
    for i in range(4):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(reg, SPM, 0, 0, 4, 0, 0, 0, 16, 8, mv))                               # reg[4] = SPM[16(gr[8])]
    f.write(data_movement_instruction(0, 0, 0, 0, 10, 0, 0, 0, 0, 0, set_PC))                                  # set_PC 10
    f.write(data_movement_instruction(reg, SPM, 0, 0, 5, 0, 0, 0, 144, 8, mv))                              # reg[5] = SPM[144(gr[8])]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, SPM, 0, 0, 3, 0, 0, 0, 272, 8, mv))                              # reg[3] = SPM[272(gr[8])]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, -10, 0, 1, 0, 2, 4, bne))                                 # bne gr[2] gr[4] -10
    f.write(data_movement_instruction(0, 0, 0, 0, -10, 0, 1, 0, 2, 4, bne))                                 # bne gr[2] gr[4] -10
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 1, 0, si))                                  # gr[10] = 1
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt              104
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 8, 0, 0, 0, -1, 11, blt))                                 # blt -1 gr[11] 8
    f.write(data_movement_instruction(0, 0, 0, 0, 8, 0, 0, 0, -1, 11, blt))                                 # blt -1 gr[11] 8
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                  # gr[10] = 0
    for i in range(10):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 8, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 8
    f.write(data_movement_instruction(0, 0, 0, 0, 8, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 8
    f.write(data_movement_instruction(gr, 0, 0, 0, 5, 0, 0, 0, 0, 0, si))                                   # gr[5] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, gr, 0, 0, 4, 0, 0, 0, 2, 0, mv))                                  # gr[4] = gr[2]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, in_port, 0, 0, 2, 0, 0, 0, 0, 0, mv))                             # gr[2] = in
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 4, 0, mv))                            # out = gr[4]
    f.write(data_movement_instruction(gr, 0, 0, 0, 3, 0, 0, 0, 0, 0, mv))                                   # gr[3] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(SPM, reg, 0, 0, 272, 6, 0, 0, 3, 0, mv))                              # SPM[272(gr[6])] = reg[3]
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                  # gr[10] = 0
    f.write(data_movement_instruction(SPM, reg, 0, 0, 16, 6, 0, 0, 4, 0, mv))                               # SPM[16(gr[6])] = reg[4]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(SPM, reg, 0, 1, 144, 6, 0, 0, 5, 0, mv))                              # SPM[144(gr[6]++)] = reg[5]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(SPM, in_port, 0, 0, 400, 0, 0, 0, 0, 0, mv))                          # SPM[400] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
    f.write(data_movement_instruction(SPM, in_port, 0, 0, 401, 0, 0, 0, 0, 0, mv))                          # SPM[401] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
    f.write(data_movement_instruction(SPM, in_port, 0, 0, 402, 0, 0, 0, 0, 0, mv))                          # SPM[402] = in
    f.write(data_movement_instruction(out_port, SPM, 0, 0, 0, 0, 0, 0, 400, 0, mv))                         # out = SPM[400]
    f.write(data_movement_instruction(SPM, in_port, 0, 0, 403, 0, 0, 0, 0, 0, mv))                          # SPM[403] = in
    f.write(data_movement_instruction(out_port, SPM, 0, 0, 0, 0, 0, 0, 401, 0, mv))                         # out = SPM[401]
    f.write(data_movement_instruction(0, 0, 0, 0, 2, 0, 0, 0, 127, 6, bge))                                 # bge 127 gr[6] 2
    f.write(data_movement_instruction(out_port, SPM, 0, 0, 0, 0, 0, 0, 402, 0, mv))                         # out = SPM[402]
    f.write(data_movement_instruction(gr, 0, 0, 0, 6, 0, 0, 0, 0, 0, si))                                   # gr[6] = 0
    f.write(data_movement_instruction(out_port, SPM, 0, 0, 0, 0, 0, 0, 403, 0, mv))                         # out = SPM[403]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt


    f.close()
    
def pe_3_instruction():
    
    f = open("instructions/poa/pe_3_instruction.txt", "w")
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    for i in range(8):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(gr, in_port, 0, 0, 1, 0, 0, 0, 0, 0, mv))                             # gr[1] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(reg, in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                            # reg[0] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 15, 0, 0, 0, 0, 0, mv))                           # reg[15] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                           # reg[13] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 16, 0, 0, 0, 0, 0, mv))                           # reg[16] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 17, 0, 0, 0, 0, 0, mv))                           # reg[17] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 18, 0, 0, 0, 0, 0, mv))                           # reg[18] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    for i in range(POA_COMPUTE_INSTRUCTION_NUM):
        f.write(data_movement_instruction(comp_ib, in_instr, 0, 0, i, 0, 0, 0, 0, 0, mv))                   # ir[i] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(SPM, 0, 0, 0, 420, 0, 0, 0, 0, 0, 4))                                 # SPM[420] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(SPM, 0, 0, 0, 421, 0, 0, 0, 0, 0, 4))                                 # SPM[421] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(SPM, 0, 0, 0, 422, 0, 0, 0, 0, 0, 4))                                 # SPM[422] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op             45

    for i in range(10):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 5, 0, 0, 0, 0, 0, si))                                   # gr[5] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                  # gr[10] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(SPM, 0, 0, 0, 272, 0, 0, 0, 0, 0, si))                                # SPM[272] = 0
    f.write(data_movement_instruction(reg, in_port, 0, 0, 10, 0, 0, 0, 0, 0, mv))                           # reg[10] = in
    f.write(data_movement_instruction(SPM, 0, 0, 0, 144, 0, 0, 0, 0, 0, si))                                # SPM[144] = 0
    f.write(data_movement_instruction(reg, in_port, 0, 0, 11, 0, 0, 0, 0, 0, mv))                           # reg[11] = in
    f.write(data_movement_instruction(SPM, 0, 0, 0, 16, 0, 0, 0, 17, 0, si))                                # SPM[16] = 17
    f.write(data_movement_instruction(reg, 0, 0, 0, 9, 0, 0, 0, -4, 0, si))                                 # reg[9] = -4
    f.write(data_movement_instruction(gr, 0, 0, 0, 6, 0, 0, 0, 0, 0, si))                                   # gr[6] = 0
    for i in range(8):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(SPM, 0, 0, 0, 146, 0, 0, 0, 0, 0, si))                                # SPM[146] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(SPM, 0, 0, 0, 18, 0, 0, 0, 17, 0, si))                                # SPM[18] = 17
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(SPM, 0, 0, 0, 274, 0, 0, 0, 0, 0, si))                                # SPM[274] = 0
    f.write(data_movement_instruction(SPM, 0, 0, 0, 271, 0, 0, 0, 0, 0, si))                                # SPM[271] = 0
    f.write(data_movement_instruction(reg, reg, 0, 0, 6, 0, 0, 0, 18, 0, mv))                               # reg[6] = reg[18]
    f.write(data_movement_instruction(SPM, 0, 0, 0, 143, 0, 0, 0, 17, 0, si))                               # SPM[143] = 17
    f.write(data_movement_instruction(reg, reg, 0, 0, 7, 0, 0, 0, -1, 0, si))                               # reg[7] = -1
    f.write(data_movement_instruction(SPM, 0, 0, 0, 399, 0, 0, 0, 0, 0, si))                                # SPM[399] = 0
    f.write(data_movement_instruction(reg, reg, 0, 0, 8, 0, 0, 0, -1, 0, si))                               # reg[8] = -1
    f.write(data_movement_instruction(gr, gr, 0, 0, 4, 0, 0, 0, 2, 0, mv))                                  # gr[4] = gr[2]
    f.write(data_movement_instruction(SPM, 0, 0, 0, 145, 0, 0, 0, 0, 0, si))                                # SPM[145] = 0
    f.write(data_movement_instruction(gr, 0, 0, 0, 2, 0, 0, 0, 1, 0, si))                                   # gr[2] = 1
    f.write(data_movement_instruction(SPM, 0, 0, 0, 17, 0, 0, 0, 17, 0, si))                                # SPM[17] = 17
    f.write(data_movement_instruction(gr, 0, 0, 0, 3, 0, 0, 0, 0, 0, si))                                   # gr[3] = 0
    f.write(data_movement_instruction(SPM, 0, 0, 0, 273, 0, 0, 0, 0, 0, si))                                # SPM[273] = 0      69

    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(SPM, in_port, 0, 1, 0, 3, 0, 0, 0, 0, mv))                            # SPM[gr[3]++] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, -1, 0, 1, 0, 3, 2, blt))                                  # blt gr[3] gr[2] -1
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 1, 0, si))                                  # gr[10] = 1
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op             74
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, set_PC))                                   # set_PC 0
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                  # gr[10] = 0
    f.write(data_movement_instruction(reg, gr, 0, 0, 31, 0, 0, 0, 2, 0, mv))                                # reg[31] = gr[2]
    f.write(data_movement_instruction(gr, 0, 0, 0, 4, 0, 0, 0, 0, 0, si))                                   # gr[4] = 0
    f.write(data_movement_instruction(reg, in_port, 0, 0, 12, 0, 0, 0, 0, 0, mv))                           # reg[12] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 1, 0, 0, 0, 0, 0, mv))                            # reg[1] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 2, 0, 0, 0, 0, 0, mv))                            # reg[2] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, SPM, 0, 0, 7, 0, 0, 1, 0, 4, mv))                                 # gr[7] = SPM[gr[4]++]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 1, 0, 8, 0, 1, 0, 6, 7, sub))                                   # gr[8] = gr[6] - gr[7]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, -1, 8, blt))                                  # blt -1 gr[8] 3
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, -1, 8, blt))                                  # blt -1 gr[8] 3
    f.write(data_movement_instruction(0, 0, 1, 0, 8, 0, 0, 0, 128, 8, addi))                                # gr[8] = gr[8] + 128
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 3
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 3
    for i in range(4):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(reg, SPM, 0, 0, 4, 0, 0, 0, 16, 8, mv))                               # reg[4] = SPM[16(gr[8])]
    f.write(data_movement_instruction(gr, reg, 0, 0, 11, 0, 0, 0, 9, 0, mv))                                # gr[11] = reg[9]
    f.write(data_movement_instruction(reg, SPM, 0, 0, 5, 0, 0, 0, 144, 8, mv))                              # reg[5] = SPM[144(gr[8])]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, SPM, 0, 0, 3, 0, 0, 0, 272, 8, mv))                              # reg[3] = SPM[272(gr[8])]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 12, 0, 1, 0, 2, 4, beq))                                  # beq gr[2] gr[4] 12
    f.write(data_movement_instruction(0, 0, 0, 0, 12, 0, 1, 0, 2, 4, beq))                                  # beq gr[2] gr[4] 12
    f.write(data_movement_instruction(gr, SPM, 0, 0, 7, 0, 0, 1, 0, 4, mv))                                 # gr[7] = SPM[gr[4]++]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 1, 0, 8, 0, 1, 0, 6, 7, sub))                                   # gr[8] = gr[6] - gr[7]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, -1, 8, blt))                                  # blt -1 gr[8] 3
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, -1, 8, blt))                                  # blt -1 gr[8] 3
    f.write(data_movement_instruction(0, 0, 1, 0, 8, 0, 0, 0, 128, 8, addi))                                # gr[8] = gr[8] + 128
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 3
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 3
    for i in range(4):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(reg, SPM, 0, 0, 4, 0, 0, 0, 16, 8, mv))                               # reg[4] = SPM[16(gr[8])]
    f.write(data_movement_instruction(0, 0, 0, 0, 10, 0, 0, 0, 0, 0, set_PC))                                  # set_PC 10
    f.write(data_movement_instruction(reg, SPM, 0, 0, 5, 0, 0, 0, 144, 8, mv))                              # reg[5] = SPM[144(gr[8])]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, SPM, 0, 0, 3, 0, 0, 0, 272, 8, mv))                              # reg[3] = SPM[272(gr[8])]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, -10, 0, 1, 0, 2, 4, bne))                                 # bne gr[2] gr[4] -10
    f.write(data_movement_instruction(0, 0, 0, 0, -10, 0, 1, 0, 2, 4, bne))                                 # bne gr[2] gr[4] -10
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 1, 0, si))                                  # gr[10] = 1
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt              104
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 8, 0, 0, 0, -1, 11, blt))                                 # blt -1 gr[11] 8
    f.write(data_movement_instruction(0, 0, 0, 0, 8, 0, 0, 0, -1, 11, blt))                                 # blt -1 gr[11] 8
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                  # gr[10] = 0
    for i in range(10):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 8, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 8
    f.write(data_movement_instruction(0, 0, 0, 0, 8, 0, 0, 0, 0, 0, beq))                                   # beq 0 0 8
    f.write(data_movement_instruction(gr, 0, 0, 0, 5, 0, 0, 0, 0, 0, si))                                   # gr[5] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, gr, 0, 0, 4, 0, 0, 0, 2, 0, mv))                                  # gr[4] = gr[2]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, in_port, 0, 0, 2, 0, 0, 0, 0, 0, mv))                             # gr[2] = in
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 4, 0, mv))                            # out = gr[4]
    f.write(data_movement_instruction(gr, 0, 0, 0, 3, 0, 0, 0, 0, 0, mv))                                   # gr[3] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(SPM, reg, 0, 0, 272, 6, 0, 0, 3, 0, mv))                              # SPM[272(gr[6])] = reg[3]
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                  # gr[10] = 0
    f.write(data_movement_instruction(SPM, reg, 0, 0, 16, 6, 0, 0, 4, 0, mv))                               # SPM[16(gr[6])] = reg[4]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(SPM, reg, 0, 1, 144, 6, 0, 0, 5, 0, mv))                              # SPM[144(gr[6]++)] = reg[5]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(SPM, in_port, 0, 0, 400, 0, 0, 0, 0, 0, mv))                          # SPM[400] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 1, 0, mv))                           # out = reg[1]
    f.write(data_movement_instruction(SPM, in_port, 0, 0, 401, 0, 0, 0, 0, 0, mv))                          # SPM[401] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 5, 0, mv))                           # out = reg[5]
    f.write(data_movement_instruction(SPM, in_port, 0, 0, 402, 0, 0, 0, 0, 0, mv))                          # SPM[402] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
    f.write(data_movement_instruction(SPM, in_port, 0, 0, 403, 0, 0, 0, 0, 0, mv))                          # SPM[403] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
    f.write(data_movement_instruction(SPM, in_port, 0, 0, 404, 0, 0, 0, 0, 0, mv))                          # SPM[404] = in
    f.write(data_movement_instruction(out_port, SPM, 0, 0, 0, 0, 0, 0, 400, 0, mv))                         # out = SPM[400]
    f.write(data_movement_instruction(SPM, in_port, 0, 0, 405, 0, 0, 0, 0, 0, mv))                          # SPM[405] = in
    f.write(data_movement_instruction(out_port, SPM, 0, 0, 0, 0, 0, 0, 401, 0, mv))                         # out = SPM[401]
    f.write(data_movement_instruction(0, 0, 0, 0, 2, 0, 0, 0, 127, 6, bge))                                 # bge 127 gr[6] 2
    f.write(data_movement_instruction(out_port, SPM, 0, 0, 0, 0, 0, 0, 402, 0, mv))                         # out = SPM[402]
    f.write(data_movement_instruction(gr, 0, 0, 0, 6, 0, 0, 0, 0, 0, si))                                   # gr[6] = 0
    f.write(data_movement_instruction(out_port, SPM, 0, 0, 0, 0, 0, 0, 403, 0, mv))                         # out = SPM[403]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, SPM, 0, 0, 0, 0, 0, 0, 404, 0, mv))                         # out = SPM[404]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, SPM, 0, 0, 0, 0, 0, 0, 405, 0, mv))                         # out = SPM[405]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    f.close()


if not os.path.exists("instructions/poa"):
    os.makedirs("instructions/poa")
    
poa_compute()
poa_main_instruction()
pe_0_instruction()
pe_1_instruction()
pe_2_instruction()
pe_3_instruction()
