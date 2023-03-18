import sys

filename = sys.argv[1]
total_consensus = int(sys.argv[2])
num_threads = int(sys.argv[3])

f = open(filename, "w")
f.write("#!/bin/bash\n\n")
f.write("python3 scripts/poa_instruction_generator.py\n")
f.write("make clean && make -j\n")
f.write("cp sim sim_poa_throughput\n\n")
f.write("num_thread_group={}\n".format(total_consensus//num_threads))
f.write("num_threads={}\n\n".format(num_threads))

f.write("for i in {0.." + str(total_consensus//num_threads-1) + "}\ndo\n")
for i in range(num_threads):
    f.write("  ./sim_poa_throughput -k 3 -i datasets/large/poa_input/input_$((i*num_threads+" + str(i+1) + ")) -o poa_output/output_$((i*num_threads+" + str(i+1) + ")) -s > poa_sim_results/sim_result_$((i*num_threads+" + str(i+1) + ")).txt &\n")
    f.write("  P{}=$!\n".format(i+1))
f.write("  wait")
for i in range(num_threads):
    f.write(" $P{}".format(i+1))
f.write("\ndone\n\n")

for i in range(total_consensus%num_threads):
    f.write("./sim_poa_throughput -k 3 -i datasets/large/poa_input/input_" + str(total_consensus//num_threads * num_threads + i + 1) + " > poa_sim_results/sim_result_" + str(total_consensus//num_threads * num_threads + i + 1) + ".txt &\n")
    f.write("  P{}=$!\n".format(i+1))
    f.write("wait")
    for i in range(total_consensus%num_threads):
        f.write(" $P{}".format(i+1))
    f.write("\n\n")
    
f.write("for i in {0.." + str(total_consensus//num_threads-1) + "}\ndo\n")
for i in range(num_threads):
    f.write("  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+" + str(i+1) + ")) /x/yufenggu/input-data/poa_output/output_$((i*num_threads+" + str(i+1) + ")) 0 > poa_correctness/poa_$((i*num_threads+" + str(i+1) + ")).txt &\n")
    f.write("  P{}=$!\n".format(i+1))
f.write("  wait")
for i in range(num_threads):
    f.write(" $P{}".format(i+1))
f.write("\ndone\n\n")

for i in range(total_consensus%num_threads):
    f.write("python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+" + str(i+1) + ")) /x/yufenggu/input-data/poa_output/output_$((i*num_threads+" + str(i+1) + ")) 0 > poa_correctness/poa_$((i*num_threads+" + str(i+1) + ")).txt &\n")
    f.write("  P{}=$!\n".format(i+1))
    f.write("wait")
    for i in range(total_consensus%num_threads):
        f.write(" $P{}".format(i+1))
    f.write("\n\n")
    
f.write("python3 scripts/poa_throughput.py poa_sim_results poa_correctness {}\n".format(total_consensus))
    
f.close()