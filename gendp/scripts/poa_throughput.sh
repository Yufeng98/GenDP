#!/bin/bash

python3 scripts/poa_instruction_generator.py
make clean && make -j
cp sim sim_poa_throughput

num_thread_group=31
num_threads=32

for i in {0..30}
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
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+9)) -o poa_output/output_$((i*num_threads+9)) -s > poa_sim_results/sim_result_$((i*num_threads+9)).txt &
  P9=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+10)) -o poa_output/output_$((i*num_threads+10)) -s > poa_sim_results/sim_result_$((i*num_threads+10)).txt &
  P10=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+11)) -o poa_output/output_$((i*num_threads+11)) -s > poa_sim_results/sim_result_$((i*num_threads+11)).txt &
  P11=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+12)) -o poa_output/output_$((i*num_threads+12)) -s > poa_sim_results/sim_result_$((i*num_threads+12)).txt &
  P12=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+13)) -o poa_output/output_$((i*num_threads+13)) -s > poa_sim_results/sim_result_$((i*num_threads+13)).txt &
  P13=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+14)) -o poa_output/output_$((i*num_threads+14)) -s > poa_sim_results/sim_result_$((i*num_threads+14)).txt &
  P14=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+15)) -o poa_output/output_$((i*num_threads+15)) -s > poa_sim_results/sim_result_$((i*num_threads+15)).txt &
  P15=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+16)) -o poa_output/output_$((i*num_threads+16)) -s > poa_sim_results/sim_result_$((i*num_threads+16)).txt &
  P16=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+17)) -o poa_output/output_$((i*num_threads+17)) -s > poa_sim_results/sim_result_$((i*num_threads+17)).txt &
  P17=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+18)) -o poa_output/output_$((i*num_threads+18)) -s > poa_sim_results/sim_result_$((i*num_threads+18)).txt &
  P18=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+19)) -o poa_output/output_$((i*num_threads+19)) -s > poa_sim_results/sim_result_$((i*num_threads+19)).txt &
  P19=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+20)) -o poa_output/output_$((i*num_threads+20)) -s > poa_sim_results/sim_result_$((i*num_threads+20)).txt &
  P20=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+21)) -o poa_output/output_$((i*num_threads+21)) -s > poa_sim_results/sim_result_$((i*num_threads+21)).txt &
  P21=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+22)) -o poa_output/output_$((i*num_threads+22)) -s > poa_sim_results/sim_result_$((i*num_threads+22)).txt &
  P22=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+23)) -o poa_output/output_$((i*num_threads+23)) -s > poa_sim_results/sim_result_$((i*num_threads+23)).txt &
  P23=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+24)) -o poa_output/output_$((i*num_threads+24)) -s > poa_sim_results/sim_result_$((i*num_threads+24)).txt &
  P24=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+25)) -o poa_output/output_$((i*num_threads+25)) -s > poa_sim_results/sim_result_$((i*num_threads+25)).txt &
  P25=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+26)) -o poa_output/output_$((i*num_threads+26)) -s > poa_sim_results/sim_result_$((i*num_threads+26)).txt &
  P26=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+27)) -o poa_output/output_$((i*num_threads+27)) -s > poa_sim_results/sim_result_$((i*num_threads+27)).txt &
  P27=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+28)) -o poa_output/output_$((i*num_threads+28)) -s > poa_sim_results/sim_result_$((i*num_threads+28)).txt &
  P28=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+29)) -o poa_output/output_$((i*num_threads+29)) -s > poa_sim_results/sim_result_$((i*num_threads+29)).txt &
  P29=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+30)) -o poa_output/output_$((i*num_threads+30)) -s > poa_sim_results/sim_result_$((i*num_threads+30)).txt &
  P30=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+31)) -o poa_output/output_$((i*num_threads+31)) -s > poa_sim_results/sim_result_$((i*num_threads+31)).txt &
  P31=$!
  ./sim_poa_throughput -k 3 -i datasets/poa/input/input_$((i*num_threads+32)) -o poa_output/output_$((i*num_threads+32)) -s > poa_sim_results/sim_result_$((i*num_threads+32)).txt &
  P32=$!
  wait $P1 $P2 $P3 $P4 $P5 $P6 $P7 $P8 $P9 $P10 $P11 $P12 $P13 $P14 $P15 $P16 $P17 $P18 $P19 $P20 $P21 $P22 $P23 $P24 $P25 $P26 $P27 $P28 $P29 $P30 $P31 $P32
done

./sim_poa_throughput -k 3 -i datasets/poa/input/input_993 > poa_sim_results/sim_result_993.txt &
  P1=$!
./sim_poa_throughput -k 3 -i datasets/poa/input/input_994 > poa_sim_results/sim_result_994.txt &
  P2=$!
./sim_poa_throughput -k 3 -i datasets/poa/input/input_995 > poa_sim_results/sim_result_995.txt &
  P3=$!
./sim_poa_throughput -k 3 -i datasets/poa/input/input_996 > poa_sim_results/sim_result_996.txt &
  P4=$!
./sim_poa_throughput -k 3 -i datasets/poa/input/input_997 > poa_sim_results/sim_result_997.txt &
  P5=$!
./sim_poa_throughput -k 3 -i datasets/poa/input/input_998 > poa_sim_results/sim_result_998.txt &
  P6=$!
./sim_poa_throughput -k 3 -i datasets/poa/input/input_999 > poa_sim_results/sim_result_999.txt &
  P7=$!
./sim_poa_throughput -k 3 -i datasets/poa/input/input_1000 > poa_sim_results/sim_result_1000.txt &
  P8=$!
wait $P1 $P2 $P3 $P4 $P5 $P6 $P7 $P8

for i in {0..30}
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
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+9)) datasets/poa/output/output_$((i*num_threads+9)) 0 > poa_correctness/poa_$((i*num_threads+9)).txt &
  P9=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+10)) datasets/poa/output/output_$((i*num_threads+10)) 0 > poa_correctness/poa_$((i*num_threads+10)).txt &
  P10=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+11)) datasets/poa/output/output_$((i*num_threads+11)) 0 > poa_correctness/poa_$((i*num_threads+11)).txt &
  P11=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+12)) datasets/poa/output/output_$((i*num_threads+12)) 0 > poa_correctness/poa_$((i*num_threads+12)).txt &
  P12=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+13)) datasets/poa/output/output_$((i*num_threads+13)) 0 > poa_correctness/poa_$((i*num_threads+13)).txt &
  P13=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+14)) datasets/poa/output/output_$((i*num_threads+14)) 0 > poa_correctness/poa_$((i*num_threads+14)).txt &
  P14=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+15)) datasets/poa/output/output_$((i*num_threads+15)) 0 > poa_correctness/poa_$((i*num_threads+15)).txt &
  P15=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+16)) datasets/poa/output/output_$((i*num_threads+16)) 0 > poa_correctness/poa_$((i*num_threads+16)).txt &
  P16=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+17)) datasets/poa/output/output_$((i*num_threads+17)) 0 > poa_correctness/poa_$((i*num_threads+17)).txt &
  P17=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+18)) datasets/poa/output/output_$((i*num_threads+18)) 0 > poa_correctness/poa_$((i*num_threads+18)).txt &
  P18=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+19)) datasets/poa/output/output_$((i*num_threads+19)) 0 > poa_correctness/poa_$((i*num_threads+19)).txt &
  P19=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+20)) datasets/poa/output/output_$((i*num_threads+20)) 0 > poa_correctness/poa_$((i*num_threads+20)).txt &
  P20=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+21)) datasets/poa/output/output_$((i*num_threads+21)) 0 > poa_correctness/poa_$((i*num_threads+21)).txt &
  P21=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+22)) datasets/poa/output/output_$((i*num_threads+22)) 0 > poa_correctness/poa_$((i*num_threads+22)).txt &
  P22=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+23)) datasets/poa/output/output_$((i*num_threads+23)) 0 > poa_correctness/poa_$((i*num_threads+23)).txt &
  P23=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+24)) datasets/poa/output/output_$((i*num_threads+24)) 0 > poa_correctness/poa_$((i*num_threads+24)).txt &
  P24=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+25)) datasets/poa/output/output_$((i*num_threads+25)) 0 > poa_correctness/poa_$((i*num_threads+25)).txt &
  P25=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+26)) datasets/poa/output/output_$((i*num_threads+26)) 0 > poa_correctness/poa_$((i*num_threads+26)).txt &
  P26=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+27)) datasets/poa/output/output_$((i*num_threads+27)) 0 > poa_correctness/poa_$((i*num_threads+27)).txt &
  P27=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+28)) datasets/poa/output/output_$((i*num_threads+28)) 0 > poa_correctness/poa_$((i*num_threads+28)).txt &
  P28=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+29)) datasets/poa/output/output_$((i*num_threads+29)) 0 > poa_correctness/poa_$((i*num_threads+29)).txt &
  P29=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+30)) datasets/poa/output/output_$((i*num_threads+30)) 0 > poa_correctness/poa_$((i*num_threads+30)).txt &
  P30=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+31)) datasets/poa/output/output_$((i*num_threads+31)) 0 > poa_correctness/poa_$((i*num_threads+31)).txt &
  P31=$!
  python3 scripts/poa_check_correctness.py poa_output/output_$((i*num_threads+32)) datasets/poa/output/output_$((i*num_threads+32)) 0 > poa_correctness/poa_$((i*num_threads+32)).txt &
  P32=$!
  wait $P1 $P2 $P3 $P4 $P5 $P6 $P7 $P8 $P9 $P10 $P11 $P12 $P13 $P14 $P15 $P16 $P17 $P18 $P19 $P20 $P21 $P22 $P23 $P24 $P25 $P26 $P27 $P28 $P29 $P30 $P31 $P32
done

python3 scripts/poa_check_correctness.py poa_output/output_993 datasets/poa/output/output_993 0 > poa_correctness/poa_993.txt &
  P1=$!
python3 scripts/poa_check_correctness.py poa_output/output_994 datasets/poa/output/output_994 0 > poa_correctness/poa_994.txt &
  P2=$!
python3 scripts/poa_check_correctness.py poa_output/output_995 datasets/poa/output/output_995 0 > poa_correctness/poa_995.txt &
  P3=$!
python3 scripts/poa_check_correctness.py poa_output/output_996 datasets/poa/output/output_996 0 > poa_correctness/poa_996.txt &
  P4=$!
python3 scripts/poa_check_correctness.py poa_output/output_997 datasets/poa/output/output_997 0 > poa_correctness/poa_997.txt &
  P5=$!
python3 scripts/poa_check_correctness.py poa_output/output_998 datasets/poa/output/output_998 0 > poa_correctness/poa_998.txt &
  P6=$!
python3 scripts/poa_check_correctness.py poa_output/output_999 datasets/poa/output/output_999 0 > poa_correctness/poa_999.txt &
  P7=$!
python3 scripts/poa_check_correctness.py poa_output/output_1000 datasets/poa/output/output_1000 0 > poa_correctness/poa_1000.txt &
  P8=$!
wait $P1 $P2 $P3 $P4 $P5 $P6 $P7 $P8

python3 scripts/poa_throughput.py poa_sim_results poa_correctness 1000
