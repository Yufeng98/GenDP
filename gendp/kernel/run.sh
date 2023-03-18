# bwa-mem
./ksw-test -i ../../data/kernel/bwa-mem/seedext_in-1k.txt -o seedext_out-1k-parallel.txt -t 56 -n 1000
./ksw-test -i ../../data/kernel/bwa-mem/seedext_in-1m.txt -o seedext_out-1m-parallel.txt -t 56 -n 1000000
./ksw-test -i /y/arunsub/punnet_data/inputs/approx-string-match/bwa-mem/bandedSWA_SRR7733443_1m_input.txt -o bandedSWA_SRR7733443_1m_out.txt -t 56 -n 10606460

# bwa-mem2
make            # without power profiling
make power=1    # power profiling
make power
./bsw -pairs ../../data/kernel/bwa-mem2/in-16.txt -t 56 -b 512
./bsw -pairs /z/scratch7/yufenggu/input-datasets/bsw/small/bandedSWA_SRR7733443_100k_input.txt -t 56 -b 512
./bsw -pairs /z/scratch7/yufenggu/input-datasets/bsw/large/bandedSWA_SRR7733443_1m_input.txt -t 56 -b 512

# chain
make print=1    # without power profiling but print result
make power=1    # power profiling
make power
./chain -i ../../data/kernel/chain/in-3.txt -o out-3.txt -t 56
./chain -i /z/scratch7/yufenggu/input-datasets/chain/small/in-1k.txt -o out-1k.txt -t 56
./chain -i /z/scratch7/yufenggu/input-datasets/chain/large/c_elegans_40x.10k.in -o c_elegans_40x.10k.out -t 56
sudo /tmp/chain -i /z/scratch7/yufenggu/input-datasets/chain/large/c_elegans_40x.10k.in -o c_elegans_40x.10k.out -t 56

# poaV2
./poa -read_fasta ../../data/kernel/poa/poa-180 -clustal clustal-180.out -hb blosum80.mat

# spoa