#!/bin/bash
set -x

# preparation
export SIMD_FLAG=$1
export NUM_THREADS=$2

# BSW
# Use sse4.1 as the default SIMD flag, could also choose avx2 or avx512
cd $GenDP_WORK_DIR/cpu-baselines/bsw/
make clean
make -j CXX=icpc arch=$SIMD_FLAG
numactl -N 0 -N 0 ./bsw -pairs $GenDP_WORK_DIR/gendp-datasets/bsw_147_1m_8bit_input.txt -t $NUM_THREADS -b 512

# Chain
cd $GenDP_WORK_DIR/cpu-baselines/chain/tal
make clean
make -j CXX=icpc arch=$SIMD_FLAG
numactl -N 0 -N 0 ./bench-dp-chaining $GenDP_WORK_DIR/gendp-datasets/chain_in-10k.txt $NUM_THREADS

# PairHMM
cd $GenDP_WORK_DIR/cpu-baselines/phmm
make clean
make -j CXX=icpc arch=$SIMD_FLAG
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./
numactl -N 0 -N 0 ./phmm -f $GenDP_WORK_DIR/gendp-datasets/large.in -t $NUM_THREADS

# POA
cd $GenDP_WORK_DIR/cpu-baselines/poa/racon
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make clean
make -j
numactl -N 0 -N 0 ./bin/racon -t $NUM_THREADS -g -6 $GenDP_WORK_DIR/gendp-datasets/guppy_hac.fastq $GenDP_WORK_DIR/gendp-datasets/saureus.flye.mm2.sam $GenDP_WORK_DIR/gendp-datasets/saureus.fasta > saureus.racon.fasta

cd $GenDP_WORK_DIR