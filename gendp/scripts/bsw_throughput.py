import sys

file_name_sim_result = sys.argv[1]

f = open(file_name_sim_result, "r")
lines = f.readlines()
simd = {}
simd["<10%"] = 0
simd["<20%"] = 0
simd["<30%"] = 0
simd["<40%"] = 0
simd["<50%"] = 0
simd["<60%"] = 0
simd["<70%"] = 0
simd["<80%"] = 0
simd["<90%"] = 0
simd["<100%"] = 0
max_cycle = 4000
cycle = 0
for line in lines:
    if line[:5] == "cycle":
        c = int(line.split()[-1])
        c -= 5  # constant
        c -= 32 # instructions
        cycle += c
        simd_utilization = c/max_cycle
        if simd_utilization < 0.1:
            simd["<10%"] += 1
        elif simd_utilization < 0.2:
            simd["<20%"] += 1
        elif simd_utilization < 0.3:
            simd["<30%"] += 1
        elif simd_utilization < 0.4:
            simd["<40%"] += 1
        elif simd_utilization < 0.5:
            simd["<50%"] += 1
        elif simd_utilization < 0.6:
            simd["<60%"] += 1
        elif simd_utilization < 0.7:
            simd["<70%"] += 1
        elif simd_utilization < 0.8:
            simd["<80%"] += 1
        elif simd_utilization < 0.9:
            simd["<90%"] += 1
        else:
            simd["<100%"] += 1

cells = 2760466818
reads = 4*len(lines)
time = cycle/(1024*1024*1024)
freq = 2

area = 5.4
area_scaling_factor_7nm = 7.8

file_correctness = sys.argv[2]
f_correctness = open(file_correctness, "r")
lines_correctness = f_correctness.readlines()
if not len(lines_correctness) > 1:
    print("BSW Simulation results verified.")
else:
    print("BSW Simulation results failed.")
    print(lines_correctness)

print("BSW Throughput: {:.3f} MCUPS/mm2".format(16*cells * freq / time / (1024*1024) / area * area_scaling_factor_7nm))
# print("Throughput in {}GHz {:.3f} KReads/mm2".format(freq, 16*reads * freq / 1024 / time / area * area_scaling_factor_7nm))
# print("Cycle", cycle)
# print("Cells", cells)
# print("Reads", reads)