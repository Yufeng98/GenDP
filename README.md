## GenDP: A Dynamic Programming Acceleration Framework for Genome Sequencing Analysis

### CPU Baselines

The table below shows the CPU system configuration and the runtime(second) for each kernel.

| CPU                                          | SIMD Flag | Operatin System       | Threads | BSW    | Chain | PairHMM | POA   |
| -------------------------------------------- | --------- | --------------------- | ------- | -----  | ----- | ------- | ----- |
| Intel(R) Xeon(R) Platinum 8380 CPU @ 2.30GHz | AVX512    | CentOS Linux 7 (CORE) | 80      | 0.0504 | 0.306 | 0.587   | 16.6  |
| Intel(R) Xeon(R) Gold 6326 CPU @ 2.90GHz     | AVX512    | Ubuntu 20.04.5 LTS    | 32      | 0.0984 | 0.473 | 0.678   | 34.3  |
| Intel(R) Xeon(R) CPU E5-2697 v3 @ 2.60GHz    | AVX2      | CentOS Linux 7 (CORE) | 28      | 0.196  | 2.35  | 2.13    | 41.7  |
| Intel(R) Core(TM) i7-7700 CPU @ 3.60GHz      | AVX2      | Ubuntu 20.04.5 LTS    | 8       | 0.278  | 4.98  | 2.13    | 90.1  |


### GPU Baselines

The table below shows the GPU system configuration and the runtime(second) for each kernel.

| GPU                | Arch Code | CUDA Version | BSW   | Chain | PairHMM | POA  |
| ------------------ | --------- | ---- | ----- | ----- | ------  | ---- |
| NVIDIA RTX A100    | sm_80     | 11.2 | 0.012 | 0.155 | 0.597   | 2.53 |
| NVIDIA RTX A6000   | sm_86     | 12.0 | 0.012 | 0.339 | 0.572   | 3.70 |
| NVIDIA TITAN Xp    | sm_61     | 10.2 | 0.020 | 0.747 | 0.915   | 11.2 |

### GenDP Speedup Over CPU/GPU

The CPU baselines are obtained from Intel(R) Xeon(R) Platinum 8380 CPU @ 2.30GHz with 80 threads in 1 socket and AVX512. THe CPU die area is 600mm2. The GPU baselines are obtained from NVIDIA RTX A100 and its die area is 826mm2. In `Chain` benchmark, GPU and GenDP throughputs are penalized by 3.72x because they use re-ordered chain algorithm and compute 3.72x more cells than CPU. The CPU baselines and GenDP throughputs are normalized to 7nm to make a fair comparison with GPU baselines. GenDP achieves 157.8x throughput/mm2 speedup over GPU.

|                             | BSW         | Chain       | PairHMM       | POA           |
| --------------------------- | ----------- | ----------- | ------------- | ------------- |
| Total Cell Updates          | 2431855834  | 20736142007 | 258363282803  | 6448581509    |
| CPU Runtime (second)        | 0.0504      | 0.306       | 0.587         | 16.6          |
| CPU GCUPS                   | 44.91       | 19.61       | 32.88         | 14.51         |
| CPU Normalized MCUPS/mm2    | 130.29      | 56.89       | 95.41         | 42.11         |
| GPU Runtime (second)        | 0.012       | 0.155       | 0.597         | 2.53          |
| GPU GCUPS                   | 192.92      | 12.89       | 32.35         | 95.13         | 
| GPU MCUPS/mm2               | 239.16      | 12.89       | 40.11         | 117.94        |
| GenDP Normalized MCUPS/mm2  | 47574       | 3626        | 17681         | 2965          |
| GenDP Speepup over CPU      | 365.1x      | 63.7x       | 185.3x        | 70.4x         |
| GenDP Speepup over GPU      | 198.9x      | 281.4x      | 440.8x        | 25.1x         |


### Instructions and Scripts

#### Step 1: Check System Requirements

1. Linux OS
2. gcc >= 8.3.1
3. cmake >= 3.16.0
4. OpenMP >= 201511
5. [Intel DPC++/C++ Compiler](https://www.intel.com/content/www/us/en/developer/articles/tool/oneapi-standalone-components.html#dpcpp-cpp) >= 2021.8.0
6. ZLIB >= 1.2.8
7. NVIDIA GPU and CUDA >= 10.0
8. Python >= 3.7.9
9. numactl >= 2.0.0
10. Check [GPU compute capability and architecture code](https://developer.nvidia.com/cuda-gpus) 

```bash
# Install Intel(R) oneAPI DPC++/C++ Compiler (ICX)
wget https://registrationcenter-download.intel.com/akdlm/irc_nas/19123/l_dpcpp-cpp-compiler_p_2023.0.0.25393_offline.sh
sudo sh ./l_dpcpp-cpp-compiler_p_2023.0.0.25393_offline.sh
# Activate OneAPI Toolkit
source /opt/intel/oneapi/setvars.sh

git clone --recursive https://github.com/Yufeng98/GenDP.git
cd GenDP
```

#### Step 2: Download Datasets

```bash
wget https://genomicsbench.eecs.umich.edu/gendp-datasets.tar.gz
tar -zxvf gendp-datasets.tar.gz
```

#### Step 3: Run CPU Baselines

If there are errors during running, please see scripts in README under `cpu-baselines` folder for debugging.

```bash
# Download datasets
export GenDP_WORK_DIR=`pwd`
# Specify the SIMD flag and number of threads to use.
# Check SIMD compatibility with `lscpu | grep Flags`, e.g., sse, avx2, avx512
# Use sse4.1 as the default SIMD flag, could also choose avx2 or avx512
bash run-cpu-baselines.sh <SIMD_FLAG> <NUM_THREADS> > cpu-baselines-log.txt 2>&1
python3 $GenDP_WORK_DIR/profile-cpu-baselines-log.py cpu-baselines-log.txt
```

#### Step 4: Run GPU Baselines

If there are errors during running, please see scripts in README under `gpu-baselines` folder for debugging.

```bash
export GenDP_WORK_DIR=`pwd`
# The path of CUDA library is usually /usr/local/cuda-xx
# See Step 1.7 for how to look for ARCH_CODE
bash run-gpu-baselines.sh <CUDA_PATH> <CUDA_BINARY_PATH> <ARCH_CODE> > gpu-baselines-log.txt 2>&1
python3 $GenDP_WORK_DIR/profile-gpu-baselines-log.py gpu-baselines-log.txt
```

#### Step 5: Run GenDP Simulator

If there are errors during running, please see scripts in README under `gendp` folder for debugging. The simulation results could be different from the reported above (< 5% error) because the script does not run the entire datasets. The script could also be configured to run the extire datasets by changing parameters but it may takes ~100 hours for simulation. 

```bash
export GenDP_WORK_DIR=`pwd`
bash run-gendp-simulation.sh > gendp-simulation-log.txt 2>&1
python3 $GenDP_WORK_DIR/profile-gendp-simulation-log.py gendp-simulation-log.txt
```
