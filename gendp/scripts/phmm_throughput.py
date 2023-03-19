import sys

file_name = sys.argv[1]

f = open(file_name, "r")
lines = f.readlines()
cells = 0
cycle = 0
for line in lines:
    lst = line.split()
    if line[:5] != "cells":
        continue
    cells += int(lst[1])
    cycle += int(lst[3])

file_correctness = sys.argv[2]
f_correctness = open(file_correctness, "r")
lines_correctness = f_correctness.readlines()
if not len(lines_correctness) > 1:
    print("Simulation results verified.")
else:
    print("Simulation results failed.")
    print(lines_correctness)

time = cycle/(1024*1024*1024)
freq = 2

area = 5.4
area_scaling_factor_7nm = 7.8
print("Throughput in {}GHz {:.3f} MCUPS/mm2".format(freq, 16*cells * freq / time / (1024*1024) / area * area_scaling_factor_7nm))
# print("Cycle", cycle)
# print("Cells", cells)