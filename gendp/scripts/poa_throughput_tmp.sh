#!/bin/bash

python3 scripts/poa_instruction_generator.py
make clean && make $1 -j16
cp sim sim_poa_throughput

# ./sim_poa_throughput -k 3 -i /x/yufenggu/input-data/poa_input/input_1 -o poa_tmp.txt -s > /x/yufenggu/input-data/poa_sim_result_dram/poa_sim_result_1.txt


num_thread_group=1553
num_threads=4
for i in {0..$((num_thread_group-1))}
do
    ./sim_poa_throughput -k 3 -i /x/yufenggu/input-data/poa_input/input_$((i*num_threads+1)) > poa_sim_result/sim_result_$((i*num_threads+1)).txt &
    P1=$!
    ./sim_poa_throughput -k 3 -i /x/yufenggu/input-data/poa_input/input_$((i*num_threads+2)) > poa_sim_result/sim_result_$((i*num_threads+2)).txt &
    P2=$!
    ./sim_poa_throughput -k 3 -i /x/yufenggu/input-data/poa_input/input_$((i*num_threads+3)) > poa_sim_result/sim_result_$((i*num_threads+3)).txt &
    P3=$!
    ./sim_poa_throughput -k 3 -i /x/yufenggu/input-data/poa_input/input_$((i*num_threads+4)) > poa_sim_result/sim_result_$((i*num_threads+4)).txt &
    P4=$!
    # ./sim_poa_throughput -k 3 -i /x/yufenggu/input-data/poa_input/input_${i} -o poa_tmp.txt -s >> tmp
    # python3 scripts/expand_memory_trace.py /x/yufenggu/input-data/poa_sim_result_dram/poa_sim_result_${i}.txt 32 > /x/yufenggu/input-data/poa_sim_result_dram/poa_sim_result_${i}.trace
    # python3 scripts/calculate_memory_bandwidth.py /x/yufenggu/input-data/poa_sim_result_dram/poa_sim_result_${i}.txt ../../ramulator/poa/poa_${i}_channel_8_rank_1.txt 32
    wait $P1 $P2 $P3 $P4
done

# for i in {6201..6216}
# do
#     ./sim_poa_throughput -k 3 -i /x/yufenggu/input-data/poa_input/input_${i} > poa_sim_result/sim_result_${i}.txt &
# done

# 1097656402 315037380879
# 1097656 315037380 reduce 1000x to save time and space

# cp tmp poa_dram_read_write_cycle
# python3 ../generator.py poa_dram_read_write_cycle
# python3 scripts/expand_memory_trace.py /x/yufenggu/input-data/poa_sim_result_dram/poa_sim_result.trace 1097656 315037380




wait
