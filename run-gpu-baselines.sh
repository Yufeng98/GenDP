#!/bin/bash}
set -x

# preparation
CUDA_PATH=$1
CUDA_BINARY_PATH=$2
ARCH_CODE=$3

### BSW
cd $GenDP_WORK_DIR/gpu-baselines/bsw/GASAL2
# Preprocessing input datasets
python3 process_bsw_input.py $GenDP_WORK_DIR/gendp-datasets/bsw_147_1m_8bit_input.txt
./configure.sh $CUDA_PATH    # E.g., /usr/local/cuda
make clean
make GPU_SM_ARCH=$ARCH_CODE MAX_QUERY_LEN=132 N_CODE=0x4E    # E.g. GPU_SM_ARCH=sm_80
cd test_prog
make clean
make -j
./test_prog.out -k 100 -y ksw ../bsw_seqs.fasta ../bsw_refs.fasta

### Chain
export PATH=$CUDA_BINARY_PATH:$PATH   # E.g., /usr/local/cuda/bin
cd $GenDP_WORK_DIR/gpu-baselines/chain/minimap2-acceleration/kernel/cuda
make clean
make GPU_SM_ARCH=$ARCH_CODE -j
./kernel $GenDP_WORK_DIR/gendp-datasets/in-10k.txt out-10k-gpu.txt
# To count number of cell updates (run scalar version of kernel on host)

### PairHMM
cd $GenDP_WORK_DIR/gpu-baselines/phmm/PairHMM
make clean
make GPU=1 GPU_SM_ARCH=$ARCH_CODE CUDA_PATH=$CUDA_PATH -j
# GPU time provided as total sow time in the output
./pairhmm $GenDP_WORK_DIR/gendp-datasets/large.in > large.gpu.out

## POA
cd $GenDP_WORK_DIR/gpu-baselines/poa/GenomeWorks
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install -Dgw_cuda_gen_all_arch=OFF -DCUDA_TOOLKIT_ROOT_DIR=$CUDA_PATH
make clean
make -j cudapoa-bin
cd cudapoa
./cudapoa -i $GenDP_WORK_DIR/gendp-datasets/input-cudapoa.txt -b 0 -m 3 -n -5 -g -4 > output-cudapoa.txt 2> cudapoa-log.txt
python3 $GenDP_WORK_DIR/gpu-baselines/poa/GenomeWorks/calculate_poa_runtime.py cudapoa-log.txt

cd $GenDP_WORK_DIR