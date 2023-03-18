## BWA-MEM

### Compilation

make [power=1] [mac=1]
Use power=1 to profile power with sudo and run with /tmp/ksw-test.
Run on mac with mac=1.

### Run

./ksw-test -i ../../data/bwa-mem/seedext_in-1k.txt -o seedext_out-1k-new-reg.txt -t 1 -n 1000 -p
./ksw-test -i /y/yufenggu/punnet_input/approx-string-match/bwa-mem//bandedSWA_SRR7733443_1m_input.txt -o bandedSWA_SRR7733443_1m_out-new.txt -t 56 -n 10606460
./ksw-test -i /x/yufenggu/input-data/bsw_147_1m_8bit_input_character.txt -o bsw_147_1m_8bit_output_character-new.txt -t 56 -n 1932253 -g 4

Use flag -n 100000 to set number of input pairs for memory allocation.
Use flag -s to output serial result.
Use flag -p to output parallel result.
Use flag -k to print intermediate value.



## Chain

### Compilation

make [print=1] [power=1] [mac=1]
Use print=1 to output result.
Use power=1 to profile power with sudo and run with /tmp/ksw-test.
Run on mac with mac=1.

### Run

./chain -i ../../data/chain/in-3.txt -o out-3.txt > tmp
./chain -i ../../data/chain/in-100.txt -o out-100.txt
./chain -i /z/scratch7/yufenggu/input-datasets/chain/small/in-1k.txt -o out-1k.txt > tmp
./chain -i /z/scratch7/yufenggu/input-datasets/chain/large/c_elegans_40x.10k.in -o c_elegans_40x.10k.out -t 1 > tmp
./chain -i /z/scratch7/yufenggu/input-datasets/chain/small/in-1k.txt -o out-1k.txt -s 4 > tmp
./chain -i ../../data/chain/in-3.txt -o out-3-tmp.txt -s 5

Use flag -s to choose function:
0 - chain_dp
1 - chain_dp_copy
2 - chain_dp_26
3 - chain_dp_reverse_26
4 - chain_accelerator



## POA

### Compilation

make poa

### Run

./poa -read_fasta ../../data/poa/poa-180 -clustal clustal-180.out -hb blosum80_small.mat



## PairHMM

### Compilation

make

### Run

./pairhmm ../../data/pairhmm/tiny.in



## Instruction Profiling

### BSW

Function: ksw_extend2, in ksw.cpp

```bash
cd bwa-mem && make clean && -j8
./ksw-test -i /x/yufenggu/input-data/bsw_147_1m_8bit_input_character.txt -o bsw_147_1m_8bit_output_character-new.txt -t 1 -n 1932253
```

### Chain

Function: chain_dp, in src/host_kernel.cpp

```bash
cd chain && make clean && make -j8
./chain -i /z/scratch7/yufenggu/input-datasets/chain/large/c_elegans_40x.10k.in -o c_elegans_40x.10k.out -t 1
```

### PairHMM

Function: do_compute_full_prob, called in src/pairhmm_impl.h defined in src/pairhmm_scalarimpl.h

```bash
cd phmm && make clean && make -j8
./pairhmm /x/yufenggu/input-data/phmm_large.in
```

### POA

Function: align_lpo_po, called in buildip_lpo.cpp, defined in align_lpo_po2.cpp

```bash
cd poaV2 && make clean && make -j8
./poa -read_fasta ../../data/poa/poa-180 -clustal clustal-180.out -hb blosum80_small.mat
```