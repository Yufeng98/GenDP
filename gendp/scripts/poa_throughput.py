import sys
import os


def parse_file(file_sim, dic, num_poa_input):
    index = int(file_sim.split(".")[0].split("_")[-1])
    if index <= num_poa_input:
        f_sim = open(file_sim, "r")
        lines = f_sim.readlines()

        for line in lines:
            lst = line.split()
            if line[0] == 'm':
                if int(lst[1]) > dic["max_node"]:
                    dic["max_node"] = int(lst[1])
                if int(lst[3]) > dic["max_edge"]:
                    dic["max_edge"] = int(lst[3])
            elif line[0] == 'c':
                dic["cycle"] += int(lst[1])
                dic["reads"] += 1
            elif line[0] == 'l':
                dic["cells"] += int(lst[1]) * int(lst[3])

sim_result_dir = sys.argv[1]
correctness_check_dir = sys.argv[2]
num_poa_input = int(sys.argv[3])
sim_result_files = os.listdir(sim_result_dir)
correctness_check_files = os.listdir(correctness_check_dir)
correctness_check_flag = True

dic = {}

dic["cycle"] = 0
dic["cells"] = 0
dic["reads"] = 0
dic["max_node"] = 0
dic["max_edge"] = 0

area_scaling_factor_7nm = 7.8

for file in sim_result_files:
    parse_file(sim_result_dir+'/'+file, dic, num_poa_input)
for file in correctness_check_files:
    index = int(file.split(".")[0].split("_")[-1])
    if index <= num_poa_input:
        f_correctness = open(correctness_check_dir+"/"+file, "r")
        lines = f_correctness.readlines()
        if len(lines) > 1:
            print(lines)
            correctness_check_flag = False
    

time = dic["cycle"]/(1024*1024*1024)
freq = 2
area = 5.4

if correctness_check_flag:
    print("POA Simulation results verified.")
else:
    print("POA Simulation results failed.")

print("POA Throughput: {:.3f} MCUPS/mm2".format(16*dic["cells"]*freq/time/(1024*1024)/area*area_scaling_factor_7nm))
# print("Throughput in {}GHz {:.3f} KReads/mm2".format(freq, 16*dic["reads"]*freq/time/1024/area*area_scaling_factor_7nm))
# print("Cycle", dic["cycle"])
# print("Cells", dic["cells"])
# print("Reads", dic["reads"])
# print("Max_node", dic["max_node"], "Max_edge", dic["max_edge"])
