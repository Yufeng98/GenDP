#!/bin/bash

python3 scripts/chain_instruction_generator.py
make clean && make -j
cp sim sim_chain

./sim_chain -k 4 -i datasets/in-1k.txt -n $1 -o chain_output-1k.txt -s > chain_sim_results/chain_sim_result-1k.txt
# ./sim_chain -k 4 -i datasets/c_elegans_40x.10k.in -n $1 -o chain_output.txt -s > chain_sim_results/chain_sim_result.txt
python3 scripts/chain_check_correctness.py datasets/chain_output.txt chain_output.txt > chain_correctness.txt
python3 scripts/chain_throughput.py chain_sim_results/chain_sim_result.txt chain_correctness.txt
