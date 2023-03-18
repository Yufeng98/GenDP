#!/bin/bash

python3 scripts/chain_instruction_generator.py
make clean && make -j
cp sim sim_chain

./sim_chain -k 4 -i datasets/large/c_elegans_40x.10k.in -n $1 > chain_sim_results/chain_sim_result-10k.txt
# ./sim_chain -k 4 -i datasets/large/in-1k.txt > chain_sim_results/chain_sim_result-1k.txt

# ./sim_chain -k 4 -i /z/scratch7/yufenggu/input-datasets/chain/small/in-1k.txt -o chain_tmp.txt -s
# R 229459104 W 76477016

# ./sim_chain -k 4 -i /z/scratch7/yufenggu/input-datasets/chain/large/c_elegans_40x.10k.in -o chain_tmp.txt -s
# # R 4507486680 W 1502402208
# python3 scripts/expand_memory_trace.py /x/yufenggu/input-data/chain_sim_result_dram/chain_sim_result_10k.trace 4507486680 1502402208
# python3 scripts/calculate_memory_bandwidth.py /x/yufenggu/input-data/chain_sim_result/chain_sim_result-10k.txt ../../ramulator/chain/chain_channel_8_rank_1.txt 32

# ./sim_chain -k 4 -i ../data/chain/in-3.txt -o output.txt -s > sim_result.txt
