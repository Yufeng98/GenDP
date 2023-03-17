import sys

file_name = sys.argv[1]

f = open(file_name, "r")
lines = f.readlines()
cells = 0
cycle = 0
for line in lines:
    lst = line.split()
    if len(lst) < 4:
        continue
    cells += int(lst[1])
    cycle += int(lst[3])

time = cycle/(1024*1024*1024)
freq = 2

area = 5.4
area_scaling_factor_7nm = 4.5 * 1.7
print("throughput in {}GHz {:.3f} MCUPS/mm2".format(freq, 16*cells * freq / time / (1024*1024) / area * area_scaling_factor_7nm))
print("cycle", cycle)
print("cells", cells)