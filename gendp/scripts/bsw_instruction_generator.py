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

BSW_COMPUTE_INSTRUCTION_NUM = 32

PE_INIT_CONSTANT_AND_INSTRUCTION = 1
PE_GROUP = 47+4
PE_GROUP_1 = 76+4
PE_INIT = 109+4
PE_RUN = 113+4
PE_GSCORE = 121+4
PE_EARLY_BERAK = 124+4
PE_END = 130+4



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
    
    
def bsw_compute():
    
    f = open("instructions/bsw/compute_instruction.txt", "w")
    
    f.write(compute_instruction(1, 9, 5, 7, 21, 0, 0, 0, 0, 20))        # head = max(0, i-qlen)                 0
    f.write(compute_instruction(0, 9, 6, 21, 21, 0, 0, 22, 0, 23))      # mlen = min(qlen+qlen, tlen)
    f.write(compute_instruction(13, 15, 9, 20, 21, 0, 5, 0, 0, 25))     # exit0 = head > tail ? 0 : 1
    f.write(compute_instruction(13, 15, 9, 2, 21, 23, 22, 0, 0, 23))    # mlen = 64 > qlen ? mlen : tlen
    f.write(compute_instruction(13, 15, 9, 7, 23, 0, 25, 0, 0, 25))     # exit0 = i_1 > mlen ? 0 : exit0
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))
    f.write(compute_instruction(14, 15, 9, 20, 21, 0, 25, 0, 0, 25))    # exit0 = head == tail ? 0 : exit0
    f.write(compute_instruction(9, 15, 9, 0, 0, 0, 0, 0, 0, 13))        # E_up = 0
    f.write(compute_instruction(16, 15, 15, 0, 0, 0, 0, 0, 0, 0))       # halt                                  4
    f.write(compute_instruction(16, 15, 15, 0, 0, 0, 0, 0, 0, 0))       # halt
    
    
    
    f.write(compute_instruction(10, 9, 0, 6, 11, 0, 0, 12, 0, 17))      # S = match_score(query, ref) + H_diag  5
    f.write(compute_instruction(13, 15, 9, 22, 7, 25, 0, 0, 0, 25))     # exit0 = tlen > i_1 ? exit0 : 0            send E to next PE
    f.write(compute_instruction(13, 0, 0, 12, 0, 17, 0, 3, 4, 18))      # H_diag = H_diag > 0 ? H_diag_S : 0    6
                                                                        # tmp = H_diag - (gap_o + gap_e)
    f.write(compute_instruction(13, 5, 5, 12, 0, 17, 0, 15, 13, 16))    # H_diag = H_diag > 0 ? H_diag_S : 0        send H_left to next PE
                                                                        # H = F_left > E_up ? F_left : E_up
                                                                        # H = H > H_diag ? H : H_diag;
    f.write(compute_instruction(13, 15, 9, 0, 8, 14, 16, 0, 0, 14))     # H_left = 0 > j ? H_left : H           7   send query to next PE
    f.write(compute_instruction(5, 0, 5, 18, 0, 0, 0, 13, 4, 13))       # E_up -= gap_e
                                                                        # tmp = tmp > 0? tmp : 0
                                                                        # E = E_up > tmp? E_up : tmp
    f.write(compute_instruction(5, 0, 5, 18, 0, 0, 0, 15, 4, 15))       # F_left -= gap_e                       8
                                                                        # tmp = tmp > 0? tmp : 0
                                                                        # F = F_left > tmp? F_left : tmp
    f.write(compute_instruction(13, 15, 9, 9, 16, 10, 8, 0, 0, 10))     # m_j = max_H > H? m_j : j
    f.write(compute_instruction(13, 15, 9, 9, 16, 9, 16, 0, 0, 9))      # max_H = max_H > H? max_H : H          9
    f.write(compute_instruction(0, 15, 9, 8, 5, 0, 0, 0, 0, 8))         # j++;
    f.write(compute_instruction(16, 15, 15, 0, 0, 0, 0, 0, 0, 0))       # halt                                  10
    f.write(compute_instruction(13, 15, 9, 5, 8, 0, 15, 0, 0, 15))      # F = 1 > j ? 0 : F
    
    
    
    f.write(compute_instruction(14, 15, 9, 8, 21, 5, 0, 0, 0, 23))      # cmp1 = j == qlen ? 1 : 0                  11
    f.write(compute_instruction(13, 15, 9, 26, 16, 27, 7, 0, 0, 19))    # max_ie_new = gscore > H_left ? max_ie : i_1
    f.write(compute_instruction(13, 15, 9, 25, 0, 23, 0, 0, 0, 23))     # cmp1 = exit0 > 0 ? cmp1 : 0
    f.write(compute_instruction(13, 15, 9, 26, 16, 26, 16, 0, 0, 20))   # gscore = gscore > H_left ? gscore : H_left
    f.write(compute_instruction(13, 15, 9, 23, 0, 19, 27, 0, 0, 27))    # max_ie = cmp1 > 0 ? max_ie_new : max_ie
    f.write(compute_instruction(13, 15, 9, 23, 0, 20, 26, 0, 0, 26))    # gscore = cmp1 > 0 ? gscore_new : gscore
    f.write(compute_instruction(16, 15, 15, 0, 0, 0, 0, 0, 0, 0))       # halt                                      14
    f.write(compute_instruction(16, 15, 15, 0, 0, 0, 0, 0, 0, 0))       # halt
    
    
    
    f.write(compute_instruction(14, 15, 9, 9, 0, 5, 0, 0, 0, 23))       # tmp = m == 0 ? 1 : 0                          15
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))
    f.write(compute_instruction(14, 15, 9, 23, 5, 0, 5, 0, 0, 20))      # break0 = tmp == 1 ? 0 : 1                     16
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))
    f.write(compute_instruction(13, 15, 9, 23, 0, 0, 25, 0, 0, 25))     # exit0 = tmp > 0 ? 0 : exit0                   17
    f.write(compute_instruction(9, 15, 9, 24, 0, 0, 0, 0, 0, 20))       # old_max_score = max_score
    f.write(compute_instruction(5, 15, 9, 9, 24, 0, 0, 0, 0, 23))       # tmp_max = max(m, max_score)                   18
    f.write(compute_instruction(1, 15, 9, 7, 10, 0, 0, 0, 0, 19))       # diff = m_j - i_1
    f.write(compute_instruction(13, 15, 9, 25, 0, 23, 24, 0, 0, 24))    # max_score = exit0 > 0 ? tmp_max : max_score   19
    f.write(compute_instruction(1, 15, 9, 10, 7, 0, 0, 0, 0, 18))       # diff_minus = i-1 - m_j
    f.write(compute_instruction(13, 15, 9, 24, 20, 5, 0, 0, 0, 23))     # cmp = max_score > old_max_score ? 1 : 0       20
    f.write(compute_instruction(13, 15, 9, 19, 0, 19, 18, 0, 0, 17))    # max_off_new = diff > 0 ? diff : diff_minus
    f.write(compute_instruction(13, 15, 9, 23, 0, 10, 28, 0, 0, 28))    # qle = cmp > 0 ? m_j : qle                     21
    f.write(compute_instruction(5, 15, 9, 17, 30, 0, 0, 0, 0, 17))      # max_off_new = max(max_off_new, max_off)       
    f.write(compute_instruction(13, 15, 9, 23, 0, 7, 29, 0, 0, 29))     # tle = cmp > 0 ? i_1 : tle                     22
    f.write(compute_instruction(13, 15, 9, 23, 0, 17, 30, 0, 0, 30))    # max_off = cmp > 0 ? max_off_new : max_off
    f.write(compute_instruction(16, 15, 15, 0, 0, 0, 0, 0, 0, 0))       # halt                                          23
    f.write(compute_instruction(16, 15, 15, 0, 0, 0, 0, 0, 0, 0))       # halt
    
    
    
    f.write(compute_instruction(13, 15, 9, 18, 24, 5, 0, 0, 0, 25))     # cmp = max_score_pre > max_score ? 1 : 0       24
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))
    f.write(compute_instruction(13, 15, 9, 25, 0, 18, 24, 0, 0, 24))    # max_score = cmp > 0 ? max_score_pre : max_score
    f.write(compute_instruction(13, 15, 9, 20, 26, 19, 27, 0, 0, 27))    # max_ie = gscore_pre > gscore ? max_ie_pre : max_ie
    f.write(compute_instruction(13, 15, 9, 20, 26, 20, 26, 0, 0, 26))    # gscore = gscore_pre > gscore ? gscore_pre : gscore
    f.write(compute_instruction(13, 15, 9, 25, 0, 21, 28, 0, 0, 28))    # qle = cmp > 0 ? qle_pre : qle
    f.write(compute_instruction(13, 15, 9, 25, 0, 22, 29, 0, 0, 29))    # tle = cmp > 0 ? tle_pre : tle
    f.write(compute_instruction(13, 15, 9, 25, 0, 23, 30, 0, 0, 30))    # max_off = cmp > 0 ? max_off_pre : max_off
    f.write(compute_instruction(16, 15, 15, 0, 0, 0, 0, 0, 0, 0))       # halt                                          28
    f.write(compute_instruction(16, 15, 15, 0, 0, 0, 0, 0, 0, 0))       # halt                                          
    
    f.write(compute_instruction(0, 15, 9, 27, 5, 0, 0, 0, 0, 27))       #                                               29
    f.write(compute_instruction(0, 15, 9, 28, 5, 0, 0, 0, 0, 28))
    f.write(compute_instruction(0, 15, 9, 29, 5, 0, 0, 0, 0, 29))
    f.write(compute_instruction(15, 15, 15, 0, 0, 0, 0, 0, 0, 0))
    f.write(compute_instruction(16, 15, 15, 0, 0, 0, 0, 0, 0, 0))       # halt                                          
    f.write(compute_instruction(16, 15, 15, 0, 0, 0, 0, 0, 0, 0))       # halt                                          
    
    f.close()


# dest, src, flag_0, flag_1, imm/reg_0, reg_0(++), flag_2, flag_3, imm/reg_1, reg_1(++), opcode
def bsw_main_instruction():
    
    f = open("instructions/bsw/main_instruction.txt", "w")
    
    f.write(data_movement_instruction(gr, 0, 0, 0, 1, 0, 0, 0, 4, 0, si))                                   # gr[1] = pe_group_size
    f.write(data_movement_instruction(gr, 0, 0, 0, 2, 0, 0, 0, 0, 0, si))                                   # gr[2] = 0
    f.write(data_movement_instruction(gr, in_buf, 0, 0, 3, 0, 0, 1, 0, 2, mv))                              # gr[3] = input[gr[2]++]
    f.write(data_movement_instruction(gr, in_buf, 0, 0, 4, 0, 0, 1, 0, 2, mv))                              # gr[4] = input[gr[2]++]
    f.write(data_movement_instruction(gr, in_buf, 0, 0, 5, 0, 0, 1, 0, 2, mv))                              # gr[5] = input[gr[2]++]
    f.write(data_movement_instruction(0, 0, 0, 0, PE_INIT_CONSTANT_AND_INSTRUCTION, 0, 0, 0, 0, 0, set_PC)) # PE_PC = consts&instr
    for i in range (8):
        f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 0, 1, 0, 2, mv));                   # out = input[gr[2]++]
    for i in range(BSW_COMPUTE_INSTRUCTION_NUM):
        f.write(data_movement_instruction(out_port, comp_ib, 0, 0, 0, 0, 0, 0, i, 0, mv));                  # out = instr[i]
        
    f.write(data_movement_instruction(0, 0, 0, 0, 8, 0, 1, 0, 2, 3, add))                                   # gr[8] = gr[2] + gr[3]              
    f.write(data_movement_instruction(0, 0, 0, 0, 9, 0, 1, 0, 8, 3, add))                                   # gr[9] = gr[8] + gr[3]              
    f.write(data_movement_instruction(0, 0, 0, 0, 10, 0, 1, 0, 9, 4, add))                                  # gr[10] = gr[9] + gr[4]              
    # f.write(data_movement_instruction(0, 0, 0, 0, 10, 0, 0, 0, 3, 10, addi))                                # gr[10] = gr[10] + 3
    f.write(data_movement_instruction(gr, 0, 0, 0, 11, 0, 0, 0, 0, 0, si))                                  # gr[11] = 0       
    f.write(data_movement_instruction(fifo[0], in_buf, 0, 0, 0, 0, 0, 1, 0, 10, mv))                        # FIFO_H = input[gr[10]++]
    f.write(data_movement_instruction(fifo[1], gr, 0, 0, 0, 0, 0, 0, 0, 0, mv))                             # FIFO_E = gr[0]
    f.write(data_movement_instruction(0, 0, 0, 0, 11, 0, 0, 0, 1, 11, addi))                                # gr[11]++
    f.write(data_movement_instruction(0, 0, 0, 0, -3, 0, 1, 0, 11, 4, bne))                                 # bne gr[11] gr[4] -3
    
    f.write(data_movement_instruction(0, 0, 0, 0, 11, 0, 1, 0, 0, 1, add))                                  # gr[11] = gr[1]              
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 1, 0, 3, 1, add))                                   # gr[3] = gr[3] + gr[1]
    f.write(data_movement_instruction(0, 0, 0, 0, 6, 0, 0, 0, 3, 4, addi))                                  # gr[6] = gr[4] + 3
    f.write(data_movement_instruction(gr, 0, 0, 0, 15, 0, 0, 0, 0, 0, si))                                  # gr[15] = 0       
    f.write(data_movement_instruction(0, 0, 0, 0, 96, 0, 1, 0, 11, 3, bge))                                 # bge gr[11] gr[3] 96
    f.write(data_movement_instruction(gr, 0, 0, 0, 12, 0, 0, 0, -3, 0, si))                                 # gr[12] = 0 - 3
    f.write(data_movement_instruction(0, 0, 0, 0, 37, 0, 1, 0, 6, 11, bge))                                 # bge gr[6] gr[11] 37
    f.write(data_movement_instruction(0, 0, 0, 0, 12, 0, 1, 0, 11, 6, sub))                                 # gr[12] = gr[11] - gr[6]
    f.write(data_movement_instruction(0, 0, 0, 0, 5, 0, 1, 0, 12, 4, bge))                                  # bge gr[12] gr[4] 5
    f.write(data_movement_instruction(0, 0, 0, 0, 15, 0, 0, 0, 1, 15, addi))                                # gr[15]++
    f.write(data_movement_instruction(out_port, fifo[0], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_H
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_E
    f.write(data_movement_instruction(0, 0, 0, 0, -3, 0, 1, 0, 12, 15, bne))                                # bne gr[12] gr[15] -3
    f.write(data_movement_instruction(0, 0, 0, 0, 12, 0, 0, 0, -3, 12, addi))                               # gr[12] = gr[12] - 3
    
    f.write(data_movement_instruction(0, 0, 0, 0, PE_GROUP, 0, 0, 0, 0, 0, set_PC))                         # PE_PC = pe_group
    f.write(data_movement_instruction(0, 0, 0, 0, 14, 0, 0, 0, 0, 12, set_8))                               # gr[14] = set_8(gr[12])
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 14, 0, mv))                           # out = gr[14]
    for i in range(3):
        f.write(data_movement_instruction(0, 0, 0, 0, 11, 0, 0, 0, -1, 11, addi))                           # gr[11]--
        f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 1, 0, 2, 11, mv))                   # out = input[gr[2](gr[11])]
        f.write(data_movement_instruction(0, 0, 0, 0, 7, 0, 0, 0, 0, 11, set_8))                            # gr[7] = set_8(gr[11])
        f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 7, 0, mv))                        # out = gr[7]
        f.write(data_movement_instruction(0, 0, 0, 0, 12, 0, 0, 0, 1, 12, addi))                            # gr[12]++
        f.write(data_movement_instruction(0, 0, 0, 0, 14, 0, 0, 0, 0, 12, set_8))                           # gr[14] = set_8(gr[12])
        f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 14, 0, mv))                       # out = gr[14]
    f.write(data_movement_instruction(0, 0, 0, 0, 11, 0, 0, 0, -1, 11, addi))                               # gr[11]--
    f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 1, 0, 2, 11, mv))                       # out = input[gr[2](gr[11])]
    f.write(data_movement_instruction(0, 0, 0, 0, 7, 0, 0, 0, 0, 11, set_8))                                # gr[7] = set_8(gr[11])
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 7, 0, mv))                            # out = gr[7]
    f.write(data_movement_instruction(0, 0, 0, 0, 33, 0, 0, 0, 0, 0, beq))                                  # beq 0 0 33
    
    f.write(data_movement_instruction(0, 0, 0, 0, PE_GROUP_1, 0, 0, 0, 0, 0, set_PC))                       # PE_PC = pe_group_1
    f.write(data_movement_instruction(0, 0, 0, 0, 14, 0, 0, 0, 0, 12, set_8))                               # gr[14] = set_8(gr[12])
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 14, 0, mv))                           # out = gr[14]
    for i in range(3):
        f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 1, 0, 8, 11, mv))                   # out = input[gr[8](gr[11])]
        f.write(data_movement_instruction(0, 0, 0, 0, 11, 0, 0, 0, -1, 11, addi))                           # gr[11]--
        f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 1, 0, 2, 11, mv))                   # out = input[gr[2](gr[11])]
        f.write(data_movement_instruction(0, 0, 0, 0, 7, 0, 0, 0, 0, 11, set_8))                            # gr[7] = set_8(gr[11])
        f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 7, 0, mv))                        # out = gr[7]
        f.write(data_movement_instruction(0, 0, 0, 0, 12, 0, 0, 0, 1, 12, addi))                            # gr[12]++
        f.write(data_movement_instruction(0, 0, 0, 0, 14, 0, 0, 0, 0, 12, set_8))                           # gr[14] = set_8(gr[12])
        f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 14, 0, mv))                       # out = gr[14]
    f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 1, 0, 8, 11, mv))                       # out = input[gr[8](gr[11])]
    f.write(data_movement_instruction(0, 0, 0, 0, 11, 0, 0, 0, -1, 11, addi))                               # gr[11]--
    f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 1, 0, 2, 11, mv))                       # out = input[gr[2](gr[11])]
    f.write(data_movement_instruction(0, 0, 0, 0, 7, 0, 0, 0, 0, 11, set_8))                                # gr[7] = set_8(gr[11])
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 7, 0, mv))                            # out = gr[7]
    
    f.write(data_movement_instruction(0, 0, 0, 0, 11, 0, 1, 0, 11, 1, add))                                 # gr[11] = gr[11] + gr[1]
    f.write(data_movement_instruction(0, 0, 0, 0, PE_INIT, 0, 0, 0, 0, 0, set_PC))                          # PE_PC = pe_init
    f.write(data_movement_instruction(0, 0, 0, 0, 11, 0, 1, 0, 11, 1, add))                                 # gr[11] = gr[11] + gr[1]
    f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 1, 1, 9, 12, mv))                       # out = input[gr[9](gr[12]++)]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    
    f.write(data_movement_instruction(0, 0, 0, 0, PE_RUN, 0, 0, 0, 0, 0, set_PC))                           # PE_PC = pe_run
    f.write(data_movement_instruction(out_port, fifo[0], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_H
    f.write(data_movement_instruction(out_port, fifo[1], 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # out = FIFO_E
    f.write(data_movement_instruction(fifo[0], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_H = in
    f.write(data_movement_instruction(out_port, in_buf, 0, 0, 0, 0, 1, 1, 9, 12, mv))                       # out = input[gr[9](gr[12]++)]
    f.write(data_movement_instruction(fifo[1], in_port, 0, 0, 0, 0, 0, 0, 0, 0, mv))                        # FIFO_E = in
    f.write(data_movement_instruction(0, 0, 0, 0, -6, 0, 1, 0, 5, 12, bge))                                 # bge gr[5] gr[12] -6
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    
    f.write(data_movement_instruction(0, 0, 0, 0, PE_GSCORE, 0, 0, 0, 0, 0, set_PC))                        # PE_PC = pe_gscore
    f.write(data_movement_instruction(0, 0, 0, 0, 2, 0, 1, 0, 6, 12, blt))                                  # blt gr[6] gr[12] 2
    f.write(data_movement_instruction(0, 0, 0, 0, -10, 0, 0, 0, 0, 0, beq))                                 # beq 0 0 -10
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    
    f.write(data_movement_instruction(0, 0, 0, 0, PE_EARLY_BERAK, 0, 0, 0, 0, 0, set_PC))                   # PE_PC = pe_early_break
    for i in range(4):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 3, 0, 0, 0, 0, 13, beq))                                  # beq 0 gr[13] 3
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, -95, 0, 0, 0, 0, 0, beq))                                 # beq 0 0 -95
    
    f.write(data_movement_instruction(0, 0, 0, 0, PE_END, 0, 0, 0, 0, 0, set_PC))                           # PE_PC = pe_end
    for i in range(11):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 15, 0, 0, 0, 0, 0, si))                                  # gr[15] = 0       
    for i in range(6):
        f.write(data_movement_instruction(out_buf, in_port, 0, 1, 0, 15, 0, 0, 0, 0, mv))                   # output[gr[15]++] = in
    
    # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    f.close()
    
def pe_0_instruction():
    
    f = open("instructions/bsw/pe_0_instruction.txt", "w")

    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt              0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 21, 0, 0, 0, 0, 0, mv))                           # reg[21] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 22, 0, 0, 0, 0, 0, mv))                           # reg[22] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 21, 0, mv))                          # out = reg[21]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 24, 0, 0, 0, 0, 0, mv))                           # reg[24] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 22, 0, mv))                          # out = reg[22]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 2, 0, 0, 0, 0, 0, mv))                            # reg[2] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 3, 0, 0, 0, 0, 0, mv))                            # reg[3] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 2, 0, mv))                           # out = reg[2]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 4, 0, 0, 0, 0, 0, mv))                            # reg[4] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 3, 0, mv))                           # out = reg[3]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 5, 0, 0, 0, 0, 0, mv))                            # reg[5] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 4, 0, mv))                           # out = reg[4]
    f.write(data_movement_instruction(gr, in_port, 0, 0, 1, 0, 0, 0, 0, 0, mv))                             # gr[1] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 5, 0, mv))                           # out = reg[5]
    f.write(data_movement_instruction(comp_ib, in_instr, 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # ir[0] = in
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 5, 0, mv))                            # out = gr[1]
    for i in range(BSW_COMPUTE_INSTRUCTION_NUM-1):
        f.write(data_movement_instruction(comp_ib, in_instr, 0, 0, i+1, 0, 0, 0, 0, 0, mv))                 # ir[i+1] = in
        f.write(data_movement_instruction(out_instr, comp_ib, 0, 0, 0, 0, 0, 0, i, 0, mv))                  # out = ir[i]
    f.write(data_movement_instruction(reg, reg, 0, 0, 26, 0, 0, 0, 4, 0, mv))                               # reg[26] = reg[4]
    f.write(data_movement_instruction(out_instr, comp_ib, 0, 0, 0, 0, 0, 0, 28, 0, mv))                     # out = ir[28]
    f.write(data_movement_instruction(reg, reg, 0, 0, 27, 0, 0, 0, 4, 0, mv))                               # reg[27] = reg[4]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, reg, 0, 0, 28, 0, 0, 0, 4, 0, mv))                               # reg[28] = reg[4]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, reg, 0, 0, 29, 0, 0, 0, 4, 0, mv))                               # reg[29] = reg[4]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, reg, 0, 0, 30, 0, 0, 0, 0, 0, mv))                               # reg[30] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    for i in range(6):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt              46
    
    for i in range(4):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op             47
    f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                            # reg[8] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 14, 0, 0, 0, 0, 0, si))                                 # reg[14] = 0
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 8, 0, mv))                           # out = reg[8]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                 # reg[10] = 0
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 6, 0, mv))                           # out = reg[6]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 15, 0, 0, 0, 0, 0, si))                                 # reg[15] = 0
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 7, 0, mv))                           # out = reg[7]
    f.write(data_movement_instruction(reg, 0, 0, 0, 9, 0, 0, 0, 0, 0, si))                                  # reg[9] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op

    for i in range(2):
        f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                            # reg[8] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 8, 0, mv))                           # out = reg[8]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in        
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 6, 0, mv))                           # out = reg[6]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in        
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 7, 0, mv))                           # out = reg[7]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op


    f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                            # reg[8] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt              75
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt  
    
    for i in range(4):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op             76
    f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                            # reg[8] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 8, 0, mv))                           # out = reg[8]
    f.write(data_movement_instruction(reg, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                 # reg[10] = 0
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 15, 0, 0, 0, 0, 0, si))                                 # reg[15] = 0
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 6, 0, mv))                           # out = reg[6]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 9, 0, 0, 0, 0, 0, si))                                  # reg[9] = 0
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 7, 0, mv))                           # out = reg[7]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op

    for i in range(2):
        f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                            # reg[8] = in
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 8, 0, mv))                           # out = reg[8]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in        
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 6, 0, mv))                           # out = reg[6]
        f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in        
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 7, 0, mv))                           # out = reg[7]
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op

    f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                            # reg[8] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt              108
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt

    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op             109
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, set_PC))                                # set 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 11, 0, 0, 0, 0, 0, mv))                           # reg[11] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 12, 0, 0, 0, 0, 0, mv))                           # reg[12] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 1, 0, si))                                  # gr[10] = 1        113
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    
    f.write(data_movement_instruction(reg, in_port, 0, 0, 12, 0, 0, 0, 0, 0, mv))                           # reg[12] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 5, 0, 0, 0, 0, 0, set_PC))                                # set 5             114
    f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                           # reg[13] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 11, 0, 0, 0, 0, 0, mv))                           # reg[11] = in
    for i in range(2):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 13, 0, mv))                          # out = reg[13]
    for i in range(2):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt              120
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op             121
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 11, 0, 0, 0, 0, 0, set_PC))                               # set 11
    for i in range(5):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op             124
        
    f.write(data_movement_instruction(0, 0, 0, 0, 15, 0, 0, 0, 0, 0, set_PC))                               # set 15
    for i in range(5):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(gr, reg, 0, 0, 10, 0, 0, 0, 20, 0, mv))                               # gr[10] = reg[20]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op             130
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                  # gr[10] = 0
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 27, 0, mv))                          # out = reg[27]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 28, 0, mv))                          # out = reg[28]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 29, 0, mv))                          # out = reg[29]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 30, 0, mv))                          # out = reg[30]
    for i in range(24):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    f.close()

def pe_1_instruction():
    
    f = open("instructions/bsw/pe_1_instruction.txt", "w")
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt              0
    for i in range(4):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 21, 0, 0, 0, 0, 0, mv))                           # reg[21] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 22, 0, 0, 0, 0, 0, mv))                           # reg[22] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 21, 0, mv))                          # out = reg[21]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 24, 0, 0, 0, 0, 0, mv))                           # reg[24] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 22, 0, mv))                          # out = reg[22]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 2, 0, 0, 0, 0, 0, mv))                            # reg[2] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 3, 0, 0, 0, 0, 0, mv))                            # reg[3] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 2, 0, mv))                           # out = reg[2]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 4, 0, 0, 0, 0, 0, mv))                            # reg[4] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 3, 0, mv))                           # out = reg[3]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 5, 0, 0, 0, 0, 0, mv))                            # reg[5] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 4, 0, mv))                           # out = reg[4]
    f.write(data_movement_instruction(gr, in_port, 0, 0, 1, 0, 0, 0, 0, 0, mv))                             # gr[1] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 5, 0, mv))                           # out = reg[5]
    f.write(data_movement_instruction(comp_ib, in_instr, 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # ir[0] = in
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 5, 0, mv))                            # out = gr[1]
    for i in range(BSW_COMPUTE_INSTRUCTION_NUM-1):
        f.write(data_movement_instruction(comp_ib, in_instr, 0, 0, i+1, 0, 0, 0, 0, 0, mv))                 # ir[i+1] = in
        f.write(data_movement_instruction(out_instr, comp_ib, 0, 0, 0, 0, 0, 0, i, 0, mv))                  # out = ir[i]
    f.write(data_movement_instruction(reg, reg, 0, 0, 26, 0, 0, 0, 4, 0, mv))                               # reg[26] = reg[4]
    f.write(data_movement_instruction(out_instr, comp_ib, 0, 0, 0, 0, 0, 0, 28, 0, mv))                     # out = ir[28]
    f.write(data_movement_instruction(reg, reg, 0, 0, 27, 0, 0, 0, 4, 0, mv))                               # reg[27] = reg[4]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, reg, 0, 0, 28, 0, 0, 0, 4, 0, mv))                               # reg[28] = reg[4]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, reg, 0, 0, 29, 0, 0, 0, 4, 0, mv))                               # reg[29] = reg[4]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, reg, 0, 0, 30, 0, 0, 0, 0, 0, mv))                               # reg[30] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    for i in range(4):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    for i in range(6):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op             47
    f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                            # reg[8] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 14, 0, 0, 0, 0, 0, si))                                 # reg[14] = 0
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 8, 0, mv))                           # out = reg[8]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                 # reg[10] = 0
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 6, 0, mv))                           # out = reg[6]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 15, 0, 0, 0, 0, 0, si))                                 # reg[15] = 0
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 7, 0, mv))                           # out = reg[7]
    f.write(data_movement_instruction(reg, 0, 0, 0, 9, 0, 0, 0, 0, 0, si))                                  # reg[9] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op

    f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                            # reg[8] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 8, 0, mv))                           # out = reg[8]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 6, 0, mv))                           # out = reg[6]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 7, 0, mv))                           # out = reg[7]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                            # reg[8] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in        
    for i in range(13):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt              75
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    for i in range(6):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op             76
    f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                            # reg[8] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 8, 0, mv))                           # out = reg[8]
    f.write(data_movement_instruction(reg, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                 # reg[10] = 0
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 15, 0, 0, 0, 0, 0, si))                                 # reg[15] = 0
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 6, 0, mv))                           # out = reg[6]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 9, 0, 0, 0, 0, 0, si))                                  # reg[9] = 0
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 7, 0, mv))                           # out = reg[7]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op

    f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                            # reg[8] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 8, 0, mv))                           # out = reg[8]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 6, 0, mv))                           # out = reg[6]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 7, 0, mv))                           # out = reg[7]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op

    f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                            # reg[8] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in        
    for i in range(15):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt              108
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt

    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op             109
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, set_PC))                                # set 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 1, 0, si))                                  # gr[10] = 1
    for i in range(5):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op             113
    
    f.write(data_movement_instruction(0, 0, 0, 0, 5, 0, 0, 0, 0, 0, set_PC))                                # set 5
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 11, 0, 0, 0, 0, 0, mv))                           # reg[11] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 12, 0, 0, 0, 0, 0, mv))                           # reg[12] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
    for i in range(2):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                           # reg[13] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 13, 0, mv))                          # out = reg[13]
    for i in range(2):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op             121
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 11, 0, 0, 0, 0, 0, set_PC))                               # set 11
    for i in range(5):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op             124
        
    f.write(data_movement_instruction(0, 0, 0, 0, 15, 0, 0, 0, 0, 0, set_PC))                               # set 15
    for i in range(5):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(gr, reg, 0, 0, 10, 0, 0, 0, 20, 0, mv))                               # gr[10] = reg[20]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op             130
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 18, 0, 0, 0, 0, 0, mv))                           # reg[18] = in
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                  # gr[10] = 0
    f.write(data_movement_instruction(reg, in_port, 0, 0, 19, 0, 0, 0, 0, 0, mv))                           # reg[19] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 20, 0, 0, 0, 0, 0, mv))                           # reg[20] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 24, 0, 0, 0, 0, 0, set_PC))                               # set 24
    f.write(data_movement_instruction(reg, in_port, 0, 0, 21, 0, 0, 0, 0, 0, mv))                           # reg[21] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 22, 0, 0, 0, 0, 0, mv))                           # reg[22] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 23, 0, 0, 0, 0, 0, mv))                           # reg[23] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 27, 0, mv))                          # out = reg[27]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 28, 0, mv))                          # out = reg[28]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 29, 0, mv))                          # out = reg[29]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 30, 0, mv))                          # out = reg[30]
    for i in range(14):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    f.close()
    
def pe_2_instruction():
    
    f = open("instructions/bsw/pe_2_instruction.txt", "w")

    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt              0
    for i in range(6):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 21, 0, 0, 0, 0, 0, mv))                           # reg[21] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 22, 0, 0, 0, 0, 0, mv))                           # reg[22] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 21, 0, mv))                          # out = reg[21]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 24, 0, 0, 0, 0, 0, mv))                           # reg[24] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 22, 0, mv))                          # out = reg[22]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 2, 0, 0, 0, 0, 0, mv))                            # reg[2] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 3, 0, 0, 0, 0, 0, mv))                            # reg[3] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 2, 0, mv))                           # out = reg[2]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 4, 0, 0, 0, 0, 0, mv))                            # reg[4] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 3, 0, mv))                           # out = reg[3]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 5, 0, 0, 0, 0, 0, mv))                            # reg[5] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 4, 0, mv))                           # out = reg[4]
    f.write(data_movement_instruction(gr, in_port, 0, 0, 1, 0, 0, 0, 0, 0, mv))                             # gr[1] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 5, 0, mv))                           # out = reg[5]
    f.write(data_movement_instruction(comp_ib, in_instr, 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # ir[0] = in
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 5, 0, mv))                            # out = gr[1]
    for i in range(BSW_COMPUTE_INSTRUCTION_NUM-1):
        f.write(data_movement_instruction(comp_ib, in_instr, 0, 0, i+1, 0, 0, 0, 0, 0, mv))                 # ir[i+1] = in
        f.write(data_movement_instruction(out_instr, comp_ib, 0, 0, 0, 0, 0, 0, i, 0, mv))                  # out = ir[i]
    f.write(data_movement_instruction(reg, reg, 0, 0, 26, 0, 0, 0, 4, 0, mv))                               # reg[26] = reg[4]
    f.write(data_movement_instruction(out_instr, comp_ib, 0, 0, 0, 0, 0, 0, 28, 0, mv))                     # out = ir[28]
    f.write(data_movement_instruction(reg, reg, 0, 0, 27, 0, 0, 0, 4, 0, mv))                               # reg[27] = reg[4]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, reg, 0, 0, 28, 0, 0, 0, 4, 0, mv))                               # reg[28] = reg[4]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, reg, 0, 0, 29, 0, 0, 0, 4, 0, mv))                               # reg[29] = reg[4]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, reg, 0, 0, 30, 0, 0, 0, 0, 0, mv))                               # reg[30] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    for i in range(2):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    for i in range(8):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op             47
    f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                            # reg[8] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 14, 0, 0, 0, 0, 0, si))                                 # reg[14] = 0
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 8, 0, mv))                           # out = reg[8]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                 # reg[10] = 0
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 6, 0, mv))                           # out = reg[6]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 15, 0, 0, 0, 0, 0, si))                                 # reg[15] = 0
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 7, 0, mv))                           # out = reg[7]
    f.write(data_movement_instruction(reg, 0, 0, 0, 9, 0, 0, 0, 0, 0, si))                                  # reg[9] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op

    f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                            # reg[8] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in        
    for i in range(25):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt              75
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    for i in range(8):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op             76
    f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                            # reg[8] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 8, 0, mv))                           # out = reg[8]
    f.write(data_movement_instruction(reg, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                 # reg[10] = 0
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 15, 0, 0, 0, 0, 0, si))                                 # reg[15] = 0
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 6, 0, mv))                           # out = reg[6]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 9, 0, 0, 0, 0, 0, si))                                  # reg[9] = 0
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 7, 0, mv))                           # out = reg[7]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op

    f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                            # reg[8] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in        
    for i in range(29):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt              108
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt

    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op             109
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, set_PC))                                # set 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 1, 0, si))                                  # gr[10] = 1
    for i in range(5):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op             113
    
    f.write(data_movement_instruction(0, 0, 0, 0, 5, 0, 0, 0, 0, 0, set_PC))                                # set 5
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 11, 0, 0, 0, 0, 0, mv))                           # reg[11] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 11, 0, mv))                          # out = reg[11]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 12, 0, 0, 0, 0, 0, mv))                           # reg[12] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
    for i in range(2):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                           # reg[13] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 13, 0, mv))                          # out = reg[13]
    for i in range(2):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op             121
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 11, 0, 0, 0, 0, 0, set_PC))                               # set 11            
    for i in range(5):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op             124
        
    f.write(data_movement_instruction(0, 0, 0, 0, 15, 0, 0, 0, 0, 0, set_PC))                               # set 15            
    for i in range(5):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(gr, reg, 0, 0, 10, 0, 0, 0, 20, 0, mv))                               # gr[10] = reg[20]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op             130
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                  # gr[10] = 0        
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 27, 0, mv))                          # out = reg[27]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 28, 0, mv))                          # out = reg[28]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 29, 0, mv))                          # out = reg[29]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 18, 0, 0, 0, 0, 0, mv))                           # reg[18] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 30, 0, mv))                          # out = reg[30]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 19, 0, 0, 0, 0, 0, mv))                           # reg[19] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 18, 0, mv))                          # out = reg[18]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 20, 0, 0, 0, 0, 0, mv))                           # reg[20] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 19, 0, mv))                          # out = reg[19]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 21, 0, 0, 0, 0, 0, mv))                           # reg[21] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 20, 0, mv))                          # out = reg[20]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 22, 0, 0, 0, 0, 0, mv))                           # reg[22] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 21, 0, mv))                          # out = reg[21]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 23, 0, 0, 0, 0, 0, mv))                           # reg[23] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 22, 0, mv))                          # out = reg[22]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 23, 0, mv))                          # out = reg[23]
    for i in range(12):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt

    f.close()
    
def pe_3_instruction():
    
    f = open("instructions/bsw/pe_3_instruction.txt", "w")

    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt              0
    for i in range(8):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 21, 0, 0, 0, 0, 0, mv))                           # reg[21] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 22, 0, 0, 0, 0, 0, mv))                           # reg[22] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 21, 0, mv))                          # out = reg[21]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 24, 0, 0, 0, 0, 0, mv))                           # reg[24] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 22, 0, mv))                          # out = reg[22]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 2, 0, 0, 0, 0, 0, mv))                            # reg[2] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 3, 0, 0, 0, 0, 0, mv))                            # reg[3] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 2, 0, mv))                           # out = reg[2]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 4, 0, 0, 0, 0, 0, mv))                            # reg[4] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 3, 0, mv))                           # out = reg[3]
    f.write(data_movement_instruction(reg, in_port, 0, 0, 5, 0, 0, 0, 0, 0, mv))                            # reg[5] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 4, 0, mv))                           # out = reg[4]
    f.write(data_movement_instruction(gr, in_port, 0, 0, 1, 0, 0, 0, 0, 0, mv))                             # gr[1] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 5, 0, mv))                           # out = reg[5]
    f.write(data_movement_instruction(comp_ib, in_instr, 0, 0, 0, 0, 0, 0, 0, 0, mv))                       # ir[0] = in
    f.write(data_movement_instruction(out_port, gr, 0, 0, 0, 0, 0, 0, 5, 0, mv))                            # out = gr[1]
    for i in range(BSW_COMPUTE_INSTRUCTION_NUM-1):
        f.write(data_movement_instruction(comp_ib, in_instr, 0, 0, i+1, 0, 0, 0, 0, 0, mv))                 # ir[i+1] = in
        f.write(data_movement_instruction(out_instr, comp_ib, 0, 0, 0, 0, 0, 0, i, 0, mv))                  # out = ir[i]
    f.write(data_movement_instruction(reg, reg, 0, 0, 26, 0, 0, 0, 4, 0, mv))                               # reg[26] = reg[4]
    f.write(data_movement_instruction(out_instr, comp_ib, 0, 0, 0, 0, 0, 0, 28, 0, mv))                     # out = ir[28]
    f.write(data_movement_instruction(reg, reg, 0, 0, 27, 0, 0, 0, 4, 0, mv))                               # reg[27] = reg[4]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, reg, 0, 0, 28, 0, 0, 0, 4, 0, mv))                               # reg[28] = reg[4]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, reg, 0, 0, 29, 0, 0, 0, 4, 0, mv))                               # reg[29] = reg[4]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, reg, 0, 0, 30, 0, 0, 0, 0, 0, mv))                               # reg[30] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    for i in range(10):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op             47
    f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                            # reg[8] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 14, 0, 0, 0, 0, 0, si))                                 # reg[14] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                 # reg[10] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 15, 0, 0, 0, 0, 0, si))                                 # reg[15] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 9, 0, 0, 0, 0, 0, si))                                  # reg[9] = 0
    for i in range(33):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt              75
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    for i in range(10):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op             76
    f.write(data_movement_instruction(reg, in_port, 0, 0, 8, 0, 0, 0, 0, 0, mv))                            # reg[8] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 14, 0, 0, 0, 0, 0, mv))                           # reg[14] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                 # reg[10] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 6, 0, 0, 0, 0, 0, mv))                            # reg[6] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 15, 0, 0, 0, 0, 0, si))                                 # reg[15] = 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 7, 0, 0, 0, 0, 0, mv))                            # reg[7] = in        
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, 0, 0, 0, 9, 0, 0, 0, 0, 0, si))                                  # reg[9] = 0
    for i in range(41):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt              108

    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op             109
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, set_PC))                                # set 0
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 1, 0, si))                                  # gr[10] = 1
    for i in range(5):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op             113
    
    f.write(data_movement_instruction(0, 0, 0, 0, 5, 0, 0, 0, 0, 0, set_PC))                                # set 5
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 11, 0, 0, 0, 0, 0, mv))                           # reg[11] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 12, 0, 0, 0, 0, 0, mv))                           # reg[12] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 14, 0, mv))                          # out = reg[14]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 13, 0, 0, 0, 0, 0, mv))                           # reg[13] = in
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 13, 0, mv))                          # out = reg[13]
    for i in range(2):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt              120
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op             121
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 11, 0, 0, 0, 0, 0, set_PC))                               # set 11            
    for i in range(5):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op             124
        
    f.write(data_movement_instruction(0, 0, 0, 0, 15, 0, 0, 0, 0, 0, set_PC))                               # set 15            
    for i in range(5):
        f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                              # No-op
    f.write(data_movement_instruction(gr, reg, 0, 0, 10, 0, 0, 0, 20, 0, mv))                               # gr[10] = reg[20]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op             130
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 18, 0, 0, 0, 0, 0, mv))                           # reg[18] = in
    f.write(data_movement_instruction(gr, 0, 0, 0, 10, 0, 0, 0, 0, 0, si))                                  # gr[10] = 0
    f.write(data_movement_instruction(reg, in_port, 0, 0, 19, 0, 0, 0, 0, 0, mv))                           # reg[19] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 20, 0, 0, 0, 0, 0, mv))                           # reg[20] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 24, 0, 0, 0, 0, 0, set_PC))                               # set 24
    f.write(data_movement_instruction(reg, in_port, 0, 0, 21, 0, 0, 0, 0, 0, mv))                           # reg[21] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 22, 0, 0, 0, 0, 0, mv))                           # reg[22] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 23, 0, 0, 0, 0, 0, mv))                           # reg[23] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 18, 0, 0, 0, 0, 0, mv))                           # reg[18] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 19, 0, 0, 0, 0, 0, mv))                           # reg[19] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 20, 0, 0, 0, 0, 0, mv))                           # reg[20] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 24, 0, 0, 0, 0, 0, set_PC))                               # set 24
    f.write(data_movement_instruction(reg, in_port, 0, 0, 21, 0, 0, 0, 0, 0, mv))                           # reg[21] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 22, 0, 0, 0, 0, 0, mv))                           # reg[22] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(reg, in_port, 0, 0, 23, 0, 0, 0, 0, 0, mv))                           # reg[23] = in
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(0, 0, 0, 0, 29, 0, 0, 0, 0, 0, set_PC))                               # set 29
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 24, 0, mv))                          # out = reg[24]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 27, 0, mv))                          # out = reg[27]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 26, 0, mv))                          # out = reg[26]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 28, 0, mv))                          # out = reg[28]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 29, 0, mv))                          # out = reg[29]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, none))                                  # No-op
    f.write(data_movement_instruction(out_port, reg, 0, 0, 0, 0, 0, 0, 30, 0, mv))                          # out = reg[30]
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt
    f.write(data_movement_instruction(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, halt))                                  # halt

    f.close()
 
bsw_compute()
bsw_main_instruction()
pe_0_instruction()
pe_1_instruction()
pe_2_instruction()
pe_3_instruction()
