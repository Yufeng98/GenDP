import sys

cpu_baselines_log_filename = sys.argv[1]
f = open(cpu_baselines_log_filename, "r")
lines = f.readlines()
for line in lines:
    lst = line.split()
    if "Overall SW cycles" in line:
        bsw_runtime = float(lst[5])
        print("BSW CPU Baseline Runtime: {:.4f} s".format(bsw_runtime))
    elif "Total ticks" in line:
        chain_runtime = float(lst[4])
        print("Chain CPU Baseline Runtime: {:.4f} s".format(chain_runtime))
    elif "PairHMM completed. Kernel runtime" in line:
        phmm_runtime = float(lst[4])
        print("PairHMM CPU Baseline Runtime: {:.4f} s".format(phmm_runtime))
    elif "[racon::Polisher::polish] generating consensus [====================]" in line:
        poa_runtime = float(lst[4])
        print("POA CPU Baseline Runtime: {:.4f} s".format(poa_runtime))
