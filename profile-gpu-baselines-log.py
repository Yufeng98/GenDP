import sys

gpu_baselines_log_filename = sys.argv[1]
f = open(gpu_baselines_log_filename, "r")
lines = f.readlines()
for line in lines:
    lst = line.split()
    if "Total execution time (in milliseconds)" in line:
        bsw_runtime = float(lst[5])
        print("BSW GPU Baseline Runtime: {:.4f} s".format(bsw_runtime/1000))
    elif "seconds to transfer in and execute" in line:
        chain_runtime = float(lst[3])
        print("Chain GPU Baseline Runtime: {:.4f} s".format(chain_runtime))
    elif "total sow time" in line:
        phmm_runtime = float(lst[3][:-2])
        print("PairHMM GPU Baseline Runtime: {:.4f} s".format(phmm_runtime/1000))
    elif "cudapoa kernel runtime" in line:
        poa_runtime = float(lst[3])
        print("POA GPU Baseline Runtime: {:.4f} s".format(poa_runtime))
