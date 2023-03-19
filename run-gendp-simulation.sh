#!/bin/bash
set -x

INPUT_SIZE_CHAIN=$1
INPUT_SIZE_PHMM=$2
INPUT_SIZE_POA=$3

### BSW   1932254  1 hour
cd $GenDP_WORK_DIR/gendp
python3 scripts/preprocess_bsw_datasets.py $GenDP_WORK_DIR/gendp-datasets/bsw_147_1m_8bit_input.txt $GenDP_WORK_DIR/gendp-datasets/bsw_147_1m_8bit_input_character.txt
cd kernel/bwa-mem
make clean
make -j
./ksw-test -i $GenDP_WORK_DIR/gendp-datasets/bsw_147_1m_8bit_input_character.txt -o $GenDP_WORK_DIR/gendp-datasets/bsw_147_1m_8bit_output.txt -x -n 2000000
cd ../../
bash scripts/bsw_throughput.sh -1

### Chain   500 2h  1000 5h   10000 50h
cd $GenDP_WORK_DIR/gendp
cd kernel/chain
make clean
make -j print=1
./chain -i $GenDP_WORK_DIR/gendp-datasets/c_elegans_40x.10k.in -o $GenDP_WORK_DIR/gendp-datasets/chain_output.txt -s 4 -n $INPUT_SIZE_CHAIN
cd ../../
mkdir -p chain_sim_results
bash scripts/chain_throughput.sh $INPUT_SIZE_CHAIN

### PairHMM     100000 1h  500000 5h  1420266 14h
cd $GenDP_WORK_DIR/gendp
cd kernel/PairHMM
make clean
make -j
./pairhmm $GenDP_WORK_DIR/gendp-datasets/large.in $INPUT_SIZE_PHMM > $GenDP_WORK_DIR/gendp-datasets/phmm_large_output.txt 2> $GenDP_WORK_DIR/gendp-datasets/phmm_large_app.txt
cd ../../
mkdir -p phmm_sim_results
bash scripts/phmm_throughput.sh $INPUT_SIZE_PHMM

### POA     20 15 min   100 2h  200 6h  6216 >100h
cd $GenDP_WORK_DIR/gendp
python3 scripts/poa_generate_script.py scripts/poa_throughput.sh kernel/poaV2/run.sh $INPUT_SIZE_POA 1
python3 scripts/preprocess_poa_datasets.py $GenDP_WORK_DIR/gendp-datasets/poa_input.fasta $GenDP_WORK_DIR/gendp-datasets/poa
cd kernel/poaV2
make clean
make -j
./run.sh > log.txt 2>&1
cd ../../
bash scripts/poa_throughput.sh
