### CPU Baselines

#### BSW
```bash
cd $GenDP_WORK_DIR/cpu-baselines/bsw/
make -j CXX=icpc arch=$SIMD_FLAG
./bsw -pairs $GenDP_WORK_DIR/gendp-datasets/bsw_147_1m_8bit_input.txt -t $NUM_THREADS -b 512
```
See BSW cycles and runtime(s) after `Overall SW cycles =`


#### Chain
```bash
cd $GenDP_WORK_DIR/cpu-baselines/chain/tal
make -j CXX=icpc arch=$SIMD_FLAG
./bench-dp-chaining $GenDP_WORK_DIR/gendp-datasets/chain_in-10k.txt $NUM_THREADS
```
See Chain cycles and runtime(s) after `Total ticks =`

#### PariHMM
```bash
# cd cpu-baselines/phmm/GKL\n./gradlew test
# Optionally build GKL library libgkl_pairhmm_c.so from source code
# We provide the pre-built GKL library
cd $GenDP_WORK_DIR/cpu-baselines/phmm
make -j CXX=icpc arch=$SIMD_FLAG
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./
./phmm -f $GenDP_WORK_DIR/gendp-datasets/large.in -t $NUM_THREADS
```
See PairHMM runtime(s) after `PairHMM completed. Kernel runtime:`

#### POA
```bash
cd $GenDP_WORK_DIR/cpu-baselines/poa/racon
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j
./bin/racon -t $NUM_THREADS -g -6 $GenDP_WORK_DIR/gendp-datasets/guppy_hac.fastq $GenDP_WORK_DIR/gendp-datasets/saureus.flye.mm2.sam $GenDP_WORK_DIR/gendp-datasets/saureus.fasta > saureus.racon.fasta
```
See POA runtime(s) after `[racon::Polisher::polish] generating consensus` 
