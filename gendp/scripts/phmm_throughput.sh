#!/bin/bash

python3 scripts/phmm_instruction_generator.py
make clean && make -j
cp sim sim_phmm_throughput

./sim_phmm_throughput -k 2 -i datasets/large/phmm_large_app.in -n $1 > phmm_sim_results/phmm_large_app_sim_result.txt

# ./sim_phmm_throughput -k 2 -i /x/yufenggu/input-data/phmm_large_app.in -o phmm_output/phmm_output.txt -s > /x/yufenggu/input-data/phmm_sim_result_dram/phmm_sim_result.txt
# 808965199 5681064
# python3 scripts/expand_memory_trace.py /x/yufenggu/input-data/phmm_sim_result_dram/phmm_sim_result.trace 808965199 5681064