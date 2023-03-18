from os import listdir
from os.path import isfile, join
import sys

# mypath = sys.argv[1]
# onlyfiles = [f for f in listdir(mypath) if isfile(join(mypath, f))]
# # print(onlyfiles)
# load_cycle = 240567660
# total_cycle = load_cycle
# total_cells = 0
# max_dependency_length = 0
# gap_exception = 0
# for file in onlyfiles:
#     max_longest_gap = 0
#     f = open(mypath+file, 'r')
#     lines = f.readlines()
#     cycle = 0
#     cells = 0
#     for line in lines:
#         if "Cells" not in line:
#             continue
#         else:
#             cycle += int(line.split("Cycle:")[-1].split("Cells:")[0])
#             cells += int(line.split("Cells:")[-1])
#             # print(cycle, cells)
#             dependency_length = int(line.split("Longest")[0].split("node:")[-1])
#             longest_gap = int(line.split("Cycle:")[0].split("nodes:")[-1])
#             if (dependency_length > max_dependency_length):
#                 max_dependency_length = dependency_length
#             if (longest_gap > max_longest_gap):
#                 max_longest_gap = longest_gap
#     # print(max_longest_gap)
#     if (max_longest_gap > 256):
#         gap_exception += 1
#     total_cycle += cycle
#     total_cells += cells

# time = total_cycle/64/750000000
# print("Gap exception > 256:", gap_exception, "out of 6216 input groups")
# print("Max dependency", max_dependency_length)
# print("Total cycles:", total_cycle)
# print("Total cells:", total_cells)
# print("Time:", time)
# print("GCUPS:", total_cells/time/1000000000)

file_name = sys.argv[1]
f = open(file_name, "r")
lines = f.readlines()
acc_cells = 0
cpu_cells = 0
for line in lines:
    if line[0] == 'a':
        acc_cells += int(line.split()[1])
    elif line[0] == 'c':
        cpu_cells += int(line.split()[1])
print("cpu", cpu_cells, "acc", acc_cells)