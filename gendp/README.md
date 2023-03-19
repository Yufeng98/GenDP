## GenDP Simulator

### System Requirements

1. gcc >= 8.3.1
2. Python >= 3.7.9

### Datasets Preparation

```bash
mkdir -p datasets
cd datasets
wget https://genomicsbench.eecs.umich.edu/bsw_147_1m_8bit_input.txt     # BSW dataset
wget https://genomicsbench.eecs.umich.edu/c_elegans_40x.10k.in          # Chain dataset
wget https://genomicsbench.eecs.umich.edu/large.in                      # PairHMM dataset
wget https://genomicsbench.eecs.umich.edu/poa_input.fasta               # POA dataset
cd ..

```

### BSW
```bash
python3 scripts/preprocess_bsw_datasets.py datasets/bsw_147_1m_8bit_input.txt datasets/bsw_147_1m_8bit_input_character.txt
cd kernel/bwa-mem
make -j
./ksw-test -i ../../datasets/bsw_147_1m_8bit_input_character.txt -o ../../datasets/bsw_147_1m_8bit_output.txt -x -n 2000000
cd ../../
# bash scripts/bsw_throughput.sh <dataset input size>
bash scripts/bsw_throughput.sh -1
```

### Chain
```bash
cd kernel/chain
make -j print=1
./chain -i ../../datasets/c_elegans_40x.10k.in -o ../../datasets/chain_output.txt -s 4 -n 100
# bash scripts/chaim_throughput.sh <dataset input size>
mkdir -p chain_sim_results
bash scripts/chain_throughput.sh 100
```

### PairHMM
```bash
cd kernel/Pairhmm
make -j
./pairhmm ../../datasets/large.in 1000 > ../../datasets/phmm_large_output.txt 2> ../../datasets/phmm_large_app.txt
mkdir -p phmm_sim_results
# bash scripts/phmm_throughput.sh <dataset input size>
bash scripts/phmm_throughput.sh 10000
```

### POA
```bash
# python3 scripts/poa_generate_script.py <simulation script name> <kernel script name> <number of inputs> <number of threads>
python3 scripts/poa_generate_script.py scripts/poa_throughput.sh kernel/poaV2/run.sh 100 8
python3 scripts/preprocess_poa_datasets.py datasets/poa_input.fasta datasets/poa
cd kernel/poaV2
make -j
./run.sh > log.txt 2>&1
cd ../../
bash scripts/poa_throughput.sh
```