### GenDP Simulator

#### BSW
```bash
cd $GenDP_WORK_DIR/gendp
python3 scripts/preprocess_bsw_datasets.py $GenDP_WORK_DIR/gendp-datasets/bsw_147_1m_8bit_input.txt $GenDP_WORK_DIR/gendp-datasets/bsw_147_1m_8bit_input_character.txt
cd kernel/bwa-mem
make -j
./ksw-test -i $GenDP_WORK_DIR/gendp-datasets/bsw_147_1m_8bit_input_character.txt -o $GenDP_WORK_DIR/gendp-datasets/bsw_147_1m_8bit_output.txt -x -n 2000000
cd ../../
# bash scripts/bsw_throughput.sh <dataset input size>
bash scripts/bsw_throughput.sh -1
```

#### Chain
```bash
cd $GenDP_WORK_DIR/gendp
cd kernel/chain
make -j print=1
./chain -i $GenDP_WORK_DIR/gendp-datasets/c_elegans_40x.10k.in -o $GenDP_WORK_DIR/gendp-datasets/chain_output.txt -s 4 -n 100
# bash scripts/chaim_throughput.sh <dataset input size>
cd ../../
mkdir -p chain_sim_results
bash scripts/chain_throughput.sh 100
```

#### PairHMM
```bash
cd $GenDP_WORK_DIR/gendp
cd kernel/Pairhmm
make -j
./pairhmm $GenDP_WORK_DIR/gendp-datasets/large.in 1000 > $GenDP_WORK_DIR/gendp-datasets/phmm_large_output.txt 2> $GenDP_WORK_DIR/gendp-datasets/phmm_large_app.txt
cd ../../
mkdir -p phmm_sim_results
# bash scripts/phmm_throughput.sh <dataset input size>
bash scripts/phmm_throughput.sh 100000
```

#### POA
```bash
cd $GenDP_WORK_DIR/gendp
# python3 scripts/poa_generate_script.py <simulation script name> <kernel script name> <number of inputs> <number of threads>
python3 scripts/poa_generate_script.py scripts/poa_throughput.sh kernel/poaV2/run.sh 100 8
python3 scripts/preprocess_poa_datasets.py $GenDP_WORK_DIR/gendp-datasets/poa_input.fasta $GenDP_WORK_DIR/gendp-datasets/poa
cd kernel/poaV2
make -j
./run.sh > log.txt 2>&1
cd ../../
bash scripts/poa_throughput.sh
```