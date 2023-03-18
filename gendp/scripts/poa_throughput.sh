#!/bin/bash

python3 scripts/poa_instruction_generator.py
make clean && make -j
cp sim sim_poa_throughput

num_thread_group=1
num_threads=8

for i in {0..0}
do
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+1)) -o poa_output/output_$((i*num_threads+1)) -s > poa_sim_results/sim_result_$((i*num_threads+1)).txt &
  P1=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+2)) -o poa_output/output_$((i*num_threads+2)) -s > poa_sim_results/sim_result_$((i*num_threads+2)).txt &
  P2=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+3)) -o poa_output/output_$((i*num_threads+3)) -s > poa_sim_results/sim_result_$((i*num_threads+3)).txt &
  P3=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+4)) -o poa_output/output_$((i*num_threads+4)) -s > poa_sim_results/sim_result_$((i*num_threads+4)).txt &
  P4=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+5)) -o poa_output/output_$((i*num_threads+5)) -s > poa_sim_results/sim_result_$((i*num_threads+5)).txt &
  P5=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+6)) -o poa_output/output_$((i*num_threads+6)) -s > poa_sim_results/sim_result_$((i*num_threads+6)).txt &
  P6=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+7)) -o poa_output/output_$((i*num_threads+7)) -s > poa_sim_results/sim_result_$((i*num_threads+7)).txt &
  P7=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+8)) -o poa_output/output_$((i*num_threads+8)) -s > poa_sim_results/sim_result_$((i*num_threads+8)).txt &
  P8=$!
  wait $P1 $P2 $P3 $P4 $P5 $P6 $P7 $P8
done

./sim_poa_throughput -k 3 -i datasets/poa/input/input_9 > poa_sim_results/sim_result_9.txt &
  P1=$!
./sim_poa_throughput -k 3 -i datasets/poa/input/input_10 > poa_sim_results/sim_result_10.txt &
  P2=$!
./sim_poa_throughput -k 3 -i datasets/poa/input/input_11 > poa_sim_results/sim_result_11.txt &
  P3=$!
./sim_poa_throughput -k 3 -i datasets/poa/input/input_12 > poa_sim_results/sim_result_12.txt &
  P4=$!
wait $P1 $P2 $P3 $P4

for i in {0..0}
do
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+1)) datasets/poa/output/output_$((i*num_threads+1)) 0 > poa_correctness/poa_$((i*num_threads+1)).txt &
  P1=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+2)) datasets/poa/output/output_$((i*num_threads+2)) 0 > poa_correctness/poa_$((i*num_threads+2)).txt &
  P2=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+3)) datasets/poa/output/output_$((i*num_threads+3)) 0 > poa_correctness/poa_$((i*num_threads+3)).txt &
  P3=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+4)) datasets/poa/output/output_$((i*num_threads+4)) 0 > poa_correctness/poa_$((i*num_threads+4)).txt &
  P4=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+5)) datasets/poa/output/output_$((i*num_threads+5)) 0 > poa_correctness/poa_$((i*num_threads+5)).txt &
  P5=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+6)) datasets/poa/output/output_$((i*num_threads+6)) 0 > poa_correctness/poa_$((i*num_threads+6)).txt &
  P6=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+7)) datasets/poa/output/output_$((i*num_threads+7)) 0 > poa_correctness/poa_$((i*num_threads+7)).txt &
  P7=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+8)) datasets/poa/output/output_$((i*num_threads+8)) 0 > poa_correctness/poa_$((i*num_threads+8)).txt &
  P8=$!
  wait $P1 $P2 $P3 $P4 $P5 $P6 $P7 $P8
done

python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+1)) datasets/poa/output/output_$((i*num_threads+1)) 0 > poa_correctness/poa_$((i*num_threads+1)).txt &
  P1=$!
python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+2)) datasets/poa/output/output_$((i*num_threads+2)) 0 > poa_correctness/poa_$((i*num_threads+2)).txt &
  P2=$!
python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+3)) datasets/poa/output/output_$((i*num_threads+3)) 0 > poa_correctness/poa_$((i*num_threads+3)).txt &
  P3=$!
python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+4)) datasets/poa/output/output_$((i*num_threads+4)) 0 > poa_correctness/poa_$((i*num_threads+4)).txt &
  P4=$!
wait $P1 $P2 $P3 $P4

python3 scripts/poa_throughput.py poa_sim_results poa_correctness 12
