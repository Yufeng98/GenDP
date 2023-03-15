### System Requirements

1. gcc >= 8.3.1
2. cmake >= 3.16.0
3. OpenMP >= 201511
4. icpc >= 2021.8.0

```bash
# Install Intel OneAPI HPC Toolkit
# See https://www.intel.com/content/www/us/en/developer/tools/oneapi/hpc-toolkit-download.html
wget https://registrationcenter-download.intel.com/akdlm/irc_nas/19084/l_HPCKit_p_2023.0.0.25400_offline.sh
sudo sh ./l_HPCKit_p_2023.0.0.25400_offline.sh
# Activate OneAPI HPC Toolkit
source /opt/intel/oneapi/setvars.sh
```

#### BSW
```bash
# Use sse4.1 as the default SIMD flag, could also choose avx2 or avx512
# Check SIMD compatibility with `lscpu | grep Flags`, e.g., sse, avx2, avx512
cd cpu-baselines/bsw/ && make -j16 CXX=icpc arch=${SIMD_FLAG}
wget https://genomicsbench.eecs.umich.edu/bsw_147_1m_8bit_input.txt
./bsw -pairs bsw_147_1m_8bit_input.txt -t ${NUM_THREADS} -b 512
```

#### PariHMM
```bash
# cd cpu-baselines/phmm/GKL && ./gradlew test
# Optionally build GKL library libgkl_pairhmm_c.so from source code
# We provide the pre-built GKL library
cd cpu-baselines/phmm && make -j16 CXX=icpc arch=${SIMD_FLAG}
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./
wget https://genomicsbench.eecs.umich.edu/large.in
./phmm -f large.in -t ${NUM_THREADS}
```

#### Chain
```bash
cd cpu-baselines/chain/Trans-Omics-Acceleration-Library && make -j16 CXX=icpc arch=${SIMD_FLAG}
wget https://genomicsbench.eecs.umich.edu/bsw_147_1m_8bit_input.txt
./bench-dp-chaining chain_in-10k.txt ${NUM_THREADS}
```

## POA
### Compilation
```bash
cd poa/racon-poa && mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && make -j16

cd ./poa/spoa && mkdir build && cd build
~/cmake-3.22.0-linux-x86_64/bin/cmake -DCMAKE_BUILD_TYPE=Release -DZLIB_INCLUDE_DIR=~/zlib-1.2.11 -DZLIB_LIBRARY=~/zlib-1.2.11/libz.so ..
scl enable devtoolset-9 bash
cd ../../ && make -j16 arch=avx512

```
### Run
```bash
./racon -t 56 guppy_hac.fastq saureus.flye.mm2.sam saureus.fasta > saureus.racon.fasta
./build/bin/racon -t 80 -g -6 ~/input_data/poa-racon/guppy_hac.fastq ~/input_data/poa-racon/saureus.flye.mm2.sam ~/input_data/poa-racon/saureus.fasta > saureus.racon.fasta
numactl -N 0 -m 0 build/bin/racon -t 28 -g -6 /x/arunsub/input-datasets/poa/guppy_hac.fastq /x/arunsub/input-datasets/poa/saureus.flye.mm2.sam /x/arunsub/input-datasets/poa/saureus.fasta > saureus.racon.fasta
# ./poa -s ~/input_data/poa_input.fasta -t 56
# ./poa -s /x/arunsub/input-datasets/poa/large/input.fasta -t 56

```
### Notes

Download inputs:
```bash
wget https://genomicsbench.eecs.umich.edu/guppy_hac.fastq
wget https://genomicsbench.eecs.umich.edu/saureus.flye.mm2.sam
wget https://genomicsbench.eecs.umich.edu/saureus.fasta
```
