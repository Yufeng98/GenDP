import sys
import os


def parse_file(file_sim, dic):
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
sim_result_files = os.listdir(sim_result_dir)

dic = {}

dic["cycle"] = 0
dic["cells"] = 0
dic["reads"] = 0
dic["max_node"] = 0
dic["max_edge"] = 0

area_scaling_factor_7nm = 4.5 * 1.7

for file in sim_result_files:
    # n = int(file.split(".")[0].split("_")[-1])
    # if n > 200:
    #     continue
    parse_file(sim_result_dir+'/'+file, dic)

time = dic["cycle"]/(1024*1024*1024)
freq = 2
area = 5.4

print("throughput in {}GHz {:.3f} MCUPS/mm2".format(freq, 16*dic["cells"]*freq/time/(1024*1024)/area*area_scaling_factor_7nm))
print("throughput in {}GHz {:.3f} KReads/mm2".format(freq, 16*dic["reads"]*freq/time/1024/area*area_scaling_factor_7nm))
print("cycle", dic["cycle"])
print("cells", dic["cells"])
print("reads", dic["reads"])
print("max_node", dic["max_node"], "max_edge", dic["max_edge"])



# file_sim = sys.argv[1]

# cycle = 0
# cells = 0
# max_node = 0
# max_edge = 0

# f_sim = open(file_sim, "r")
# lines = f_sim.readlines()

# for line in lines:
#     lst = line.split()
#     if line[0] == 'm':
#         max_node = int(lst[1])
#         max_edge = int(lst[3])
#     elif line[0] == 'c':
#         cycle += int(lst[1])
#     elif line[0] == 'l':
#         cells += int(lst[1]) * int(lst[3])

# print("throughput in 1GHz {:.3f} MCUPS/mm2".format(16*cells/cycle*1000/7))
# print("cycle", cycle)
# print("cells", cells)
# print("max_node", max_node, "max_edge", max_edge)