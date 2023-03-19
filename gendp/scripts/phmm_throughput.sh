#!/bin/bash

python3 scripts/phmm_instruction_generator.py
make clean && make -j
cp sim sim_phmm_throughput

./sim_phmm_throughput -k 2 -i datasets/phmm_large_app.txt -n $1 -o phmm_large_app_output.txt -s > phmm_sim_results/phmm_large_app_sim_result.txt
python3 scripts/phmm_check_correctness.py datasets/phmm_large_output.txt phmm_large_app_output.txt > phmm_correctness.txt
python3 scripts/phmm_throughput.py phmm_sim_results/phmm_large_app_sim_result.txt phmm_correctness.txt