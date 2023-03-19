#!/bin/bash

python3 scripts/poa_instruction_generator.py
make clean && make -j
cp sim sim_poa_throughput

num_thread_group=2
num_threads=4

for i in {0..1}
do
  ./sim_poa_throughput -k 3 -i ../gendp-datasets/poa/input/input_$((i*num_threads+1)) -o poa_output/output_$((i*num_threads+1)) -s > poa_sim_results/sim_result_$((i*num_threads+1)).txt &
  P1=$!
  ./sim_poa_throughput -k 3 -i ../gendp-datasets/poa/input/input_$((i*num_threads+2)) -o poa_output/output_$((i*num_threads+2)) -s > poa_sim_results/sim_result_$((i*num_threads+2)).txt &
  P2=$!
  ./sim_poa_throughput -k 3 -i ../gendp-datasets/poa/input/input_$((i*num_threads+3)) -o poa_output/output_$((i*num_threads+3)) -s > poa_sim_results/sim_result_$((i*num_threads+3)).txt &
  P3=$!
  ./sim_poa_throughput -k 3 -i ../gendp-datasets/poa/input/input_$((i*num_threads+4)) -o poa_output/output_$((i*num_threads+4)) -s > poa_sim_results/sim_result_$((i*num_threads+4)).txt &
  P4=$!
  wait $P1 $P2 $P3 $P4
done

./sim_poa_throughput -k 3 -i ../gendp-datasets/poa/input/input_9 -o poa_output/output_9 -s > poa_sim_results/sim_result_9.txt &
  P1=$!
./sim_poa_throughput -k 3 -i ../gendp-datasets/poa/input/input_10 -o poa_output/output_10 -s > poa_sim_results/sim_result_10.txt &
  P2=$!
wait $P1 $P2

for i in {0..1}
do
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+1)) ../gendp-datasets/poa/output/output_$((i*num_threads+1)) 0 > poa_correctness/poa_$((i*num_threads+1)).txt &
  P1=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+2)) ../gendp-datasets/poa/output/output_$((i*num_threads+2)) 0 > poa_correctness/poa_$((i*num_threads+2)).txt &
  P2=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+3)) ../gendp-datasets/poa/output/output_$((i*num_threads+3)) 0 > poa_correctness/poa_$((i*num_threads+3)).txt &
  P3=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+4)) ../gendp-datasets/poa/output/output_$((i*num_threads+4)) 0 > poa_correctness/poa_$((i*num_threads+4)).txt &
  P4=$!
  wait $P1 $P2 $P3 $P4
done

python3 scripts/poa_check_correctness.py poa_output/output_9 ../gendp-datasets/poa/output/output_9 0 > poa_correctness/poa_9.txt &
  P1=$!
python3 scripts/poa_check_correctness.py poa_output/output_10 ../gendp-datasets/poa/output/output_10 0 > poa_correctness/poa_10.txt &
  P2=$!
wait $P1 $P2

python3 scripts/poa_throughput.py poa_sim_results poa_correctness 10
