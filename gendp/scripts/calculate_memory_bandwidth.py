import sys

simulation_filename = sys.argv[1]
dram_filename = sys.argv[2]
num_tile = int(sys.argv[3])
PE_array_per_tile = 16
if "chain" in simulation_filename:
    num_PE_array = num_tile
else:
    num_PE_array = num_tile * PE_array_per_tile

f_s = open(simulation_filename, "r")
f_lines = f_s.readlines()
f_s.close()
cycle = 0
for line in f_lines:
    lst = line.split()
    if "cycle" in line:
        cycle += int(lst[-1])
gendp_cycle = cycle // num_PE_array

f_d = open(dram_filename, "r")
f_lines = f_d.readlines()
f_d.close()
dram_cycle = 0
for line in f_lines:
    lst = line.split()
    if lst[0] == "ramulator.dram_cycles":
        dram_cycle += int(lst[1])

print(gendp_cycle, dram_cycle)
