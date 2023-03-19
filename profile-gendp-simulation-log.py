import sys

gendp_simulation_log_filename = sys.argv[1]
f = open(gendp_simulation_log_filename, "r")
lines = f.readlines()
for line in lines:
    lst = line.split()
    if " Simulation results verified." in line:
        print(line[:-1])
    elif " Throughput:" in line:
        print(line[:-1])
