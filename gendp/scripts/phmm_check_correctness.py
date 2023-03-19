import sys

filename_kernel = sys.argv[1]
filename_sim = sys.argv[2]

f_kernel = open(filename_kernel, "r")
f_sim = open(filename_sim, "r")

lines_kernel = f_kernel.readlines()
lines_sim = f_sim.readlines()

for i in range(min(len(lines_kernel), len(lines_sim))):
    if lines_kernel[i] != lines_sim[i]:
        print(lines_kernel[i][:-1], lines_sim[i][:-1])

f_kernel.close()
f_sim.close()