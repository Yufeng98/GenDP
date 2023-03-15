### System Requirements

1. gcc >= 8.3.1
2. cmake >= 3.16.0
3. CUDA 10.0+

### GPU Baselines

#### BSW
```bash
cd gpu-baselines/bsw/GASAL2
# Preprocessing input datasets
wget https://genomicsbench.eecs.umich.edu/bsw_147_1m_8bit_input.txt
python3 process_bsw_input.py bsw_147_1m_8bit_input.txt
./configure.sh ${CUDA_PATH}    # E.g., /usr/local/cuda
make GPU_SM_ARCH=${ARCH_CODE} MAX_QUERY_LEN=132 N_CODE=0x4E    # E.g. GPU_SM_ARCH=sm_80
cd test_prog
make -j
./test_prog.out -k 100 -y ksw ../bsw_seqs.fasta ../bsw_refs.fasta
```

#### Chain
```bash
export PATH=${CUDA_BINARY_PATH}:$PATH   # E.g., /usr/local/cuda/bin
cd gpu-baselines/chain/minimap2-acceleration/kernel/cuda
make GPU_SM_ARCH=${ARCH_CODE} -j
wget https://genomicsbench.eecs.umich.edu/in-10k.txt
./kernel in-10k.txt out-10k-gpu.txt
# To count number of cell updates (run scalar version of kernel on host)
USE_HOST_KERNEL=1 in-10k.txt out-10k-gpu.txt    
```

#### PairHMM
```bash
cd gpu-baselines/phmm/PairHMM
make GPU=1 GPU_SM_ARCH=${ARCH_CODE} CUDA_PATH=${CUDA_PATH} -j
wget https://genomicsbench.eecs.umich.edu/large.in
# GPU time provided as total sow time in the output
./pairhmm large.in > large.gpu.out
```

#### POA

```bash
cd poa/GenomeWorks
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install -Dgw_cuda_gen_all_arch=OFF -DCUDA_TOOLKIT_ROOT_DIR=${CUDA_PATH}
make -j cudapoa-bin
cd cudapoa
wget https://genomicsbench.eecs.umich.edu/input-cudapoa.txt
./cudapoa -i input-cudapoa.txt -b 0 -m 3 -n -5 -g -4 > output-cudapoa.txt
```

