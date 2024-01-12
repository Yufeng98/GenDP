## GenDP: A Dynamic Programming Acceleration Framework for Genome Sequencing Analysis

### CPU Baselines

The table below shows the CPU system configuration and runtime (in seconds) for each kernel.

| CPU                                          | SIMD Flag | Operating System       | Threads | BSW    | Chain | PairHMM | POA   |
| -------------------------------------------- | --------- | --------------------- | ------- | -----  | ----- | ------- | ----- |
| Intel(R) Xeon(R) Platinum 8380 CPU @ 2.30GHz | AVX512    | CentOS Linux 7 (CORE) | 80      | 0.0504 | 0.306 | 0.587   | 16.6  |
| Intel(R) Xeon(R) Gold 6326 CPU @ 2.90GHz     | AVX512    | Ubuntu 20.04.5 LTS    | 32      | 0.0984 | 0.473 | 0.792   | 34.3  |
| Intel(R) Xeon(R) CPU E5-2697 v3 @ 2.60GHz    | AVX2      | CentOS Linux 7 (CORE) | 28      | 0.196  | 2.35  | 2.13    | 41.7  |
| 12th Gen Intel(R) Core(TM) i5-12600          | AVX2      | Ubuntu 22.04.2 LTS    | 12      | 0.140  | 2.21  | 1.71    | 36.6  |
| Intel(R) Core(TM) i7-7700 CPU @ 3.60GHz      | AVX2      | Ubuntu 20.04.5 LTS    | 8       | 0.29   | 4.79  | 4.51    | 98.5  |

### GPU Baselines

The table below shows the GPU system configuration and runtime (in seconds) for each kernel. 

| GPU                | Arch Code | CUDA Version | BSW   | Chain | PairHMM | POA  |
| ------------------ | --------- | ---- | ----- | ----- | ------  | ---- |
| NVIDIA A100        | sm_80     | 11.2 | 0.012 | 0.155 | 0.597   | 2.53 |
| NVIDIA RTX A6000   | sm_86     | 12.0 | 0.012 | 0.339 | 0.572   | 3.70 |
| NVIDIA TITAN Xp    | sm_61     | 10.2 | 0.020 | 0.747 | 0.915   | 11.2 |

### GenDP Speedup Over CPU/GPU

The CPU baselines are obtained from the Intel Xeon Platinum 8380 CPU @ 2.30GHz with 80 threads in 1 socket and AVX512. The CPU die area is 600mm<sup>2</sup>. The GPU baselines are obtained from the NVIDIA RTX A100 and its die area is 826mm<sup>2</sup>. In the `Chain` benchmark, GPU and GenDP throughputs are penalized by 3.72x because they use a re-ordered chaining algorithm and compute 3.72x more cells than the CPU implementation. The CPU baselines and GenDP throughputs are normalized to 7nm technology for a fair comparison with GPU baselines. GenDP achieves an average 157.8x throughput/mm<sup>2</sup> speedup over GPU. The Metric used in the table is Giga Cell Updates per Second (GCUPS) and Mega Cell Updates per Second/mm<sup>2</sup> (MCUPS/mm<sup>2</sup>).

|                                       | BSW         | Chain       | PairHMM       | POA           |
| ------------------------------------- | ----------- | ----------- | ------------- | ------------- |
| Total Cell Updates                    | 2431855834  | 20736142007 | 258363282803  | 6448581509    |
| CPU Runtime (seconds)                 | 0.0504      | 0.306       | 0.587         | 16.6          |
| CPU GCUPS                             | 44.91       | 19.61       | 32.88         | 14.51         |
| CPU Normalized MCUPS/mm<sup>2</sup>   | 130.29      | 56.89       | 95.41         | 42.11         |
| GPU Runtime (seconds)                 | 0.012       | 0.155       | 0.597         | 2.53          |
| GPU GCUPS                             | 192.92      | 10.40       | 32.35         | 95.13         | 
| GPU MCUPS/mm<sup>2</sup>              | 239.16      | 12.89       | 40.11         | 117.94        |
| ASIC Normalized MCUPS/mm<sup>2</sup>  | 118,950      | -           | 51,867         | -             |
| GenDP Normalized MCUPS/mm<sup>2</sup> | 47,574       | 3,626        | 17,681         | 2,965          |
| GenDP Speedup over CPU                | 365.1x      | 63.7x       | 185.3x        | 70.4x         |
| GenDP Speedup over GPU                | 198.9x      | 281.4x      | 440.8x        | 25.1x         |


### Instructions and Scripts

#### Step 1: Check System Requirements

1. Intel CPU with 16G memory and 40G storage 
2. Linux OS
3. NVIDIA GPU and CUDA >= 10.0
4. gcc >= 8.3.1
5. cmake >= 3.16.0
6. OpenMP >= 201511
7. [Intel DPC++/C++ Compiler](https://www.intel.com/content/www/us/en/developer/articles/tool/oneapi-standalone-components.html#dpcpp-cpp) >= 2021.8.0
8. ZLIB >= 1.2.8 
9. Python >= 3.7.9
10. numactl >= 2.0.0

```bash
# Install Intel(R) oneAPI DPC++/C++ Compiler (ICX)
wget https://registrationcenter-download.intel.com/akdlm/irc_nas/19123/l_dpcpp-cpp-compiler_p_2023.0.0.25393_offline.sh
sudo sh ./l_dpcpp-cpp-compiler_p_2023.0.0.25393_offline.sh
# Activate OneAPI Toolkit
source /opt/intel/oneapi/setvars.sh
```

#### Step 2: Download Repository and Datasets

```bash
# Clone GenDP
git clone --recursive https://github.com/Yufeng98/GenDP.git
cd GenDP
# Download Datasets
wget https://genomicsbench.eecs.umich.edu/gendp-datasets.tar.gz
tar -zxvf gendp-datasets.tar.gz
```

#### Step 3: Run CPU Baselines

If you encounter errors while running, please see the scripts in <a href="https://github.com/Yufeng98/GenDP/blob/main/cpu-baselines/README.md">`cpu-baselines/README.md`</a> for debugging.

```bash
# Specify workspace directory
export GenDP_WORK_DIR=`pwd`
# Specify the SIMD flag and number of threads to use.
# Check SIMD compatibility with `lscpu | grep Flags`, e.g., sse, avx2, avx512
# Use sse4.1 as the default SIMD flag, could also choose avx2 or avx512
bash run-cpu-baselines.sh <SIMD_FLAG> <NUM_THREADS> 2>&1 | tee cpu-baselines-log.txt
python3 $GenDP_WORK_DIR/profile-cpu-baselines-log.py cpu-baselines-log.txt
```

#### Step 4: Run GPU Baselines

If you encounter errors while running, please see the scripts in <a href="https://github.com/Yufeng98/GenDP/blob/main/gpu-baselines/README.md">`gpu-baselines/README.md`</a> for debugging.

```bash
export GenDP_WORK_DIR=`pwd`
# The path of CUDA library <CUDA_PATH> is usually /usr/local/cuda-xx
# The path of CUDA binary library <CUDA_BINARY_PATH> is usually /usr/local/cuda-xx/bin
# The <ARCH_CODE> could be found by checking the compute capability of the GPU from https://developer.nvidia.com/cuda-gpus
# E.g. if the Compute Capability of NVIDIA A100 is 8.0, its ARCH_CODE is sm_80
bash run-gpu-baselines.sh <CUDA_PATH> <CUDA_BINARY_PATH> <ARCH_CODE> 2>&1 | tee gpu-baselines-log.txt
python3 $GenDP_WORK_DIR/profile-gpu-baselines-log.py gpu-baselines-log.txt
```

#### Step 5: Run GenDP Simulator

If you encounter errors while running, please see the scripts in <a href="https://github.com/Yufeng98/GenDP/blob/main/gendp/README.md">`gendp/README.md`</a> for debugging. The simulation results could be different but comparable to the [reported table](https://github.com/Yufeng98/GenDP#gendp-speedup-over-cpugpu). because the script does not run the entire datasets. The script could also be configured to run the entire datasets by changing the input size to -1 and will generate the same throughputs as above, but it may take ~250 hours for simulation and ~2T storage. 

```bash
export GenDP_WORK_DIR=`pwd`
# bash run-gendp-simulation.sh <Chain input size> <PairHMM input size> <POA input size>
# See approximate runtime on different input sizes for each kernel in script run-gendp-simulation.sh
# BSW simulation is fast and entire dataset is default.
bash run-gendp-simulation.sh 500 100000 100 2>&1 | tee gendp-simulation-log.txt     # ~ 6 hours
bash run-gendp-simulation.sh 2000 500000 200 2>&1 | tee gendp-simulation-log.txt    # ~ 24 hours
bash run-gendp-simulation.sh -1 -1 -1 2>&1 | tee gendp-simulation-log.txt           # ~ 250 hours for entire dataset
python3 $GenDP_WORK_DIR/profile-gendp-simulation-log.py gendp-simulation-log.txt
```

### Issues and Acknowledgment

We appreciate any feedback and suggestions from the community. Feel free to raise an issue or submit a pull request on Github. For assistance in using GenDP, please contact: Yufeng Gu (yufenggu AT umich DOT edu).

If you decide to use GenDP in your research, please cite the following references:

> Gu, Y., Subramaniyan, A., Dunn, T., Khadem, A., Chen, K.Y., Paul, S., Vasimuddin, M., Misra, S., Blaauw, D., Narayanasamy, S. and Das, R., 2023, June. GenDP: A Framework of Dynamic Programming Acceleration for Genome Sequencing Analysis. In Proceedings of the 50th Annual International Symposium on Computer Architecture (pp. 1-15).

```
@inproceedings{gu2023gendp,
  title={GenDP: A Framework of Dynamic Programming Acceleration for Genome Sequencing Analysis},
  author={Gu, Yufeng and Subramaniyan, Arun and Dunn, Tim and Khadem, Alireza and Chen, Kuan-Yu and Paul, Somnath and Vasimuddin, Md and Misra, Sanchit and Blaauw, David and Narayanasamy, Satish and others},
  booktitle={Proceedings of the 50th Annual International Symposium on Computer Architecture},
  pages={1--15},
  year={2023}
}
``` 
