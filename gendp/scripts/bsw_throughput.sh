#!/bin/bash

python3 scripts/bsw_instruction_generator.py
make clean && make -j16
cp sim sim_bsw

./sim_bsw -k 1 -i /x/yufenggu/input-data/bsw_147_1m_8bit_input.txt > bsw_sim_result/bsw_sim_result.txt

# ./sim_bsw -k 1 -i /x/yufenggu/input-data/bsw_147_1m_8bit_input.txt -o bsw_tmp.txt -s #
# 57033737 46374096
# python3 scripts/expand_memory_trace.py /x/yufenggu/input-data/bsw_sim_result_dram/bsw_sim_result.trace 57033737 46374096


# python3 scripts/calculate_memory_bandwidth.py /x/yufenggu/input-data/bsw_sim_result_dram/bsw_sim_result.txt ../../ramulator/bsw/bsw_channel_8_rank_1.txt 32

