#!/bin/bash

python3 scripts/bsw_instruction_generator.py
make clean && make -j
cp sim sim_bsw

# ./sim_bsw -k 1 -i datasets/bsw_147_1m_8bit_input.txt -o bsw_147_1m_8bit_sim_output.txt -s -n $1 > bsw_sim_results/bsw_147_1m_8bit_sim_result.txt
# python3 scripts/bsw_check_correctness.py bsw_147_1m_8bit_sim_output.txt datasets/bsw_147_1m_8bit_output.txt > bsw_correctness.txt
# python3 scripts/bsw_throughput.py bsw_sim_results/bsw_147_1m_8bit_sim_result.txt bsw_correctness.txt

./sim_bsw -k 1 -i datasets/bsw_147_1m_8bit_input_512.txt -o bsw_147_1m_8bit_sim_output.txt -s -n $1 > bsw_sim_results/bsw_147_1m_8bit_sim_result.txt
python3 scripts/bsw_check_correctness.py bsw_147_1m_8bit_sim_output.txt datasets/bsw_147_1m_8bit_output.txt > bsw_correctness.txt
python3 scripts/bsw_throughput.py bsw_sim_results/bsw_147_1m_8bit_sim_result.txt bsw_correctness.txt
