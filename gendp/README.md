## GenDP Simulator

### System Requirements

1. gcc >= 8.3.1

### Datasets Preparation

```bash
mkdir datasets/large
cd datasets/large
wget https://genomicsbench.eecs.umich.edu/bsw_147_1m_8bit_input.txt     # BSW dataset
wget https://genomicsbench.eecs.umich.edu/c_elegans_40x.10k.in          # Chain dataset
wget https://genomicsbench.eecs.umich.edu/phmm_large_app.in             # PairHMM dataset
wget https://genomicsbench.eecs.umich.edu/poa_input.tar.gz              # POA dataset

mkdir bsw_sim_results
mkdir chain_sim_results
mkdir phmm_sim_results
mkdir poa_sim_results
```