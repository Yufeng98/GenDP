### GPU Baselines

#### BSW
```bash
cd $GenDP_WORK_DIR/gpu-baselines/bsw/GASAL2
# Preprocessing input datasets
python3 process_bsw_input.py $GenDP_WORK_DIR/gendp-datasets/bsw_147_1m_8bit_input.txt
./configure.sh $CUDA_PATH    # E.g., /usr/local/cuda
make GPU_SM_ARCH=$ARCH_CODE MAX_QUERY_LEN=132 N_CODE=0x4E    # E.g. GPU_SM_ARCH=sm_80
cd test_prog
make -j
./test_prog.out -k 100 -y ksw ../bsw_seqs.fasta ../bsw_refs.fasta
```
See BSW runtime(ms) after `Total execution time (in milliseconds)`.

#### Chain
```bash
export PATH=$CUDA_BINARY_PATH:$PATH   # E.g., /usr/local/cuda/bin
cd $GenDP_WORK_DIR/gpu-baselines/chain/minimap2-acceleration/kernel/cuda
make GPU_SM_ARCH=$ARCH_CODE -j
./kernel $GenDP_WORK_DIR/gendp-datasets/in-10k.txt out-10k-gpu.txt
# To count number of cell updates (run scalar version of kernel on host)
USE_HOST_KERNEL=1 in-10k.txt out-10k-gpu.txt    
```
See Chain runtime(s) before `seconds to transfer in and execute`.

#### PairHMM
```bash
cd $GenDP_WORK_DIR/gpu-baselines/phmm/PairHMM
make GPU=1 GPU_SM_ARCH=$ARCH_CODE CUDA_PATH=$CUDA_PATH -j
# GPU time provided as total sow time in the output
./pairhmm $GenDP_WORK_DIR/gendp-datasets/large.in > large.gpu.out
```
See PairHMM runtime(ms) after `total sow time`.

#### POA

```bash
cd $GenDP_WORK_DIR/gpu-baselines/poa/GenomeWorks
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install -Dgw_cuda_gen_all_arch=OFF -DCUDA_TOOLKIT_ROOT_DIR=$CUDA_PATH
make -j cudapoa-bin
cd cudapoa
./cudapoa -i $GenDP_WORK_DIR/gendp-datasets/input-cudapoa.txt -b 0 -m 3 -n -5 -g -4 > output-cudapoa.txt 2> cudapoa-log.txt
python3 $GenDP_WORK_DIR/gpu-baselines/poa/GenomeWorks/calculate_poa_runtime.py cudapoa-log.txt
```

