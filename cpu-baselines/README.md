### System Requirements

1. gcc >= 8.3.1
2. cmake >= 3.16.0
3. OpenMP >= 201511s
4. icpc >= 2021.8.0
5. ZLIB >= 1.28

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
cd cpu-baselines/bsw/
make -j CXX=icpc arch=${SIMD_FLAG}
wget https://genomicsbench.eecs.umich.edu/bsw_147_1m_8bit_input.txt
./bsw -pairs bsw_147_1m_8bit_input.txt -t ${NUM_THREADS} -b 512
```

#### Chain
```bash
cd cpu-baselines/chain/tal
make -j CXX=icpc arch=${SIMD_FLAG}
wget https://genomicsbench.eecs.umich.edu/chain_in-10k.txt
./bench-dp-chaining chain_in-10k.txt ${NUM_THREADS}
```

#### PariHMM
```bash
# cd cpu-baselines/phmm/GKL\n./gradlew test
# Optionally build GKL library libgkl_pairhmm_c.so from source code
# We provide the pre-built GKL library
cd cpu-baselines/phmm
make -j CXX=icpc arch=${SIMD_FLAG}
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./
wget https://genomicsbench.eecs.umich.edu/large.in
./phmm -f large.in -t ${NUM_THREADS}
```

## POA
### Compilation
```bash
cd cpu-baselines/poa/racon
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j
wget https://genomicsbench.eecs.umich.edu/guppy_hac.fastq
wget https://genomicsbench.eecs.umich.edu/saureus.flye.mm2.sam
wget https://genomicsbench.eecs.umich.edu/saureus.fasta
./bin/racon -t ${NUM_THREADS} -g -6 guppy_hac.fastq saureus.flye.mm2.sam saureus.fasta > saureus.racon.fasta
```
