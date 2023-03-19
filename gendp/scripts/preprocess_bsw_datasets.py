import sys
import os

if not os.path.exists("bsw_sim_results"):
    os.makedirs("bsw_sim_results")
    
# convert bsw input from 0123 to ACGT
filename_input = sys.argv[1]
filename_output = sys.argv[2]

# sort bsw_147 in each batch
f = open(filename_input, 'r')
batch = 512
f_result = open(filename_input.split(".")[0]+"_512."+filename_input.split(".")[1], 'w')
lines = f.readlines()
sum = len(lines)//3
input = {}
index = 0
sum_index = 0
for i in range(len(lines)//3):
    reg_length = len(lines[i*3+1][:-1])
    if reg_length not in input.keys():
        input[reg_length] = [(lines[i*3], lines[i*3+1], lines[i*3+2])]
    else:
        input[reg_length].append((lines[i*3], lines[i*3+1], lines[i*3+2]))
    index += 1
    sum_index += 1
    if index == batch or sum_index == sum:
        for key in sorted(input.keys()):
            for case in input[key]:
                # if (len(case[2][:-1]) < 10 and len(case[1][:-1]) < 10):
                    # continue
                f_result.write(case[0])
                f_result.write(case[1])
                f_result.write(case[2])
        input = {}
        index = 0
f_result.close()
f.close()

f = open(filename_input.split(".")[0]+"_512."+filename_input.split(".")[1], 'r')
f_result = open(filename_output.split(".")[0]+"."+filename_output.split(".")[1], 'w')
lines = f.readlines()
for i in range(len(lines)//3):
    seq = ''
    for j in lines[i*3+2]:
        if j == '0':
            seq+='A'
        if j == '1':
            seq+='C'
        if j == '2':
            seq+='G'
        if j == '3':
            seq+='T'
        if j == '4':
            seq+='N'
    seq+=','
    for j in lines[i*3+1]:
        if j == '0':
            seq+='A'
        if j == '1':
            seq+='C'
        if j == '2':
            seq+='G'
        if j == '3':
            seq+='T'
        if j == '4':
            seq+='N'
    seq+=','
    seq+=lines[i*3][:-1]
    seq+=',100\n'
    f_result.write(seq)
f_result.close()
f.close()