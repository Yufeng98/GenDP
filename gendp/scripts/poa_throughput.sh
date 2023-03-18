#!/bin/bash

python3 scripts/poa_instruction_generator.py
make clean && make -j
cp sim sim_poa_throughput

num_thread_group=3
num_threads=5

for i in {0..2}
do
  ./sim_poa_throughput -k 3 -i datasets/large/poa_input/input_$((i*num_threads+1)) -o poa_output/output_$((i*num_threads+1)) -s > poa_sim_results/sim_result_$((i*num_threads+1)).txt &
  P1=$!
  ./sim_poa_throughput -k 3 -i datasets/large/poa_input/input_$((i*num_threads+2)) -o poa_output/output_$((i*num_threads+2)) -s > poa_sim_results/sim_result_$((i*num_threads+2)).txt &
  P2=$!
  ./sim_poa_throughput -k 3 -i datasets/large/poa_input/input_$((i*num_threads+3)) -o poa_output/output_$((i*num_threads+3)) -s > poa_sim_results/sim_result_$((i*num_threads+3)).txt &
  P3=$!
  ./sim_poa_throughput -k 3 -i datasets/large/poa_input/input_$((i*num_threads+4)) -o poa_output/output_$((i*num_threads+4)) -s > poa_sim_results/sim_result_$((i*num_threads+4)).txt &
  P4=$!
  ./sim_poa_throughput -k 3 -i datasets/large/poa_input/input_$((i*num_threads+5)) -o poa_output/output_$((i*num_threads+5)) -s > poa_sim_results/sim_result_$((i*num_threads+5)).txt &
  P5=$!
  wait $P1 $P2 $P3 $P4 $P5
done

./sim_poa_throughput -k 3 -i datasets/large/poa_input/input_16 > poa_sim_results/sim_result_16.txt &
  P1=$!
wait $P1

for i in {0..2}
do
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+1)) /x/yufenggu/input-data/poa_output/output_$((i*num_threads+1)) 0 > poa_correctness/poa_$((i*num_threads+1)).txt &
  P1=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+2)) /x/yufenggu/input-data/poa_output/output_$((i*num_threads+2)) 0 > poa_correctness/poa_$((i*num_threads+2)).txt &
  P2=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+3)) /x/yufenggu/input-data/poa_output/output_$((i*num_threads+3)) 0 > poa_correctness/poa_$((i*num_threads+3)).txt &
  P3=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+4)) /x/yufenggu/input-data/poa_output/output_$((i*num_threads+4)) 0 > poa_correctness/poa_$((i*num_threads+4)).txt &
  P4=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+5)) /x/yufenggu/input-data/poa_output/output_$((i*num_threads+5)) 0 > poa_correctness/poa_$((i*num_threads+5)).txt &
  P5=$!
  wait $P1 $P2 $P3 $P4 $P5
done

python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+1)) /x/yufenggu/input-data/poa_output/output_$((i*num_threads+1)) 0 > poa_correctness/poa_$((i*num_threads+1)).txt &
  P1=$!
wait $P1

python3 scripts/poa_throughput.py poa_sim_results poa_correctness 16
