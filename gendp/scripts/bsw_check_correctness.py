import sys

file_sim = sys.argv[1]
file_kernel = sys.argv[2]

f_sim = open(file_sim, "r")
f_kernel = open(file_kernel, "r")
lines_sim = f_sim.readlines()
lines_kernel = f_kernel.readlines()

sim = []
kernel = []
index = [0, 4]

for line in lines_sim:
    sim.append(line.split())

for line in lines_kernel:
    kernel.append(line.split())

for i in range(len(sim)):
    if sim[i] != kernel[i]:
        flag = 0
        for j in index:
            if len(sim[i])!=len(kernel[i]):
                print(sim[i], kernel[i])
            elif sim[i][j] != kernel[i][j]:
                flag = 1
        if flag == 1:
            print(i, sim[i], kernel[i])