import sys

file_name = sys.argv[1]

f = open(file_name, "r")
lines = f.readlines()

cells = 0
cycle = 0
reads = 0

for line in lines:
    if line[:5] == "cycle":
        cycle += int(line.split()[-1])
    elif line[:5] == "Cells":
        cells += int(line.split()[-1])
        reads += 1

area_scaling_factor_7nm = 7.8
time = cycle/(1024*1024*1024)
freq = 2
area = 5.4
cells_overhead = 3.72

file_correctness = sys.argv[2]
f_correctness = open(file_correctness, "r")
lines_correctness = f_correctness.readlines()
if not len(lines_correctness) > 1:
    print("Simulation results verified.")
else:
    print("Simulation results failed.")
    print(lines_correctness)

print("Throughput in {}GHz {:.3f} MCUPS/mm2".format(freq, cells*freq/time/(1024*1024)/area*area_scaling_factor_7nm/cells_overhead))
# print("Throughput in {}GHz {:.3f} Reads/mm2".format(freq, reads*freq/time/area*area_scaling_factor_7nm))
# print("Cycle", cycle)
# print("Cells", cells)
# print("Reads", reads)