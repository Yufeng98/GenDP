## GenDP Simulator

### System Requirements

1. gcc >= 8.3.1

### Datasets Preparation

```bash
mkdir datasets/large
cd datasets/large
wget https://genomicsbench.eecs.umich.edu/bsw_147_1m_8bit_input.txt     # BSW dataset
wget https://genomicsbench.eecs.umich.edu/c_elegans_40x.10k.in          # Chain dataset
wget https://genomicsbench.eecs.umich.edu/in-1k.txt                     # Chain small dataset
wget https://genomicsbench.eecs.umich.edu/phmm_large_app.in             # PairHMM dataset
wget https://genomicsbench.eecs.umich.edu/poa_input.tar.gz              # POA dataset
tar -zxvf poa_input.tar.gz

# initialization
mkdir -p bsw_sim_results
mkdir -p chain_sim_results
mkdir -p phmm_sim_results
mkdir -p poa_sim_results
mkdir -p poa_output
mkdir -p poa_correctness
mkdir -p instructions/bsw
mkdir -p instructions/chain
mkdir -p instructions/phmm
mkdir -p instructions/poa
```

### BSW
```bash
# bash scripts/bsw_throughput.sh <dataset input size>
bash scripts/bsw_throughput.sh 1000
```

### Chain
```bash
# bash scripts/bsw_throughput.sh <dataset input size>
bash scripts/bsw_throughput.sh 1000
```

### PairHMM
```bash
# bash scripts/bsw_throughput.sh <dataset input size>
bash scripts/bsw_throughput.sh 1000
```

### POA
```bash
# python3 scripts/poa_generate_script.py <script name> <number of inputs> <number of threads>
python3 scripts/poa_generate_script.py scripts/poa_throughput.sh 16 5
bash scripts/poa_throughput.sh
```