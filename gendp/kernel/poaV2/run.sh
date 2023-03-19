#!/bin/bash

num_thread_group=31
num_threads=32

for i in {0..30}
do
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+1)) -clustal clustal-180.out -hb blosum80_small.mat &
  P1=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+2)) -clustal clustal-180.out -hb blosum80_small.mat &
  P2=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+3)) -clustal clustal-180.out -hb blosum80_small.mat &
  P3=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+4)) -clustal clustal-180.out -hb blosum80_small.mat &
  P4=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+5)) -clustal clustal-180.out -hb blosum80_small.mat &
  P5=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+6)) -clustal clustal-180.out -hb blosum80_small.mat &
  P6=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+7)) -clustal clustal-180.out -hb blosum80_small.mat &
  P7=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+8)) -clustal clustal-180.out -hb blosum80_small.mat &
  P8=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+9)) -clustal clustal-180.out -hb blosum80_small.mat &
  P9=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+10)) -clustal clustal-180.out -hb blosum80_small.mat &
  P10=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+11)) -clustal clustal-180.out -hb blosum80_small.mat &
  P11=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+12)) -clustal clustal-180.out -hb blosum80_small.mat &
  P12=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+13)) -clustal clustal-180.out -hb blosum80_small.mat &
  P13=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+14)) -clustal clustal-180.out -hb blosum80_small.mat &
  P14=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+15)) -clustal clustal-180.out -hb blosum80_small.mat &
  P15=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+16)) -clustal clustal-180.out -hb blosum80_small.mat &
  P16=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+17)) -clustal clustal-180.out -hb blosum80_small.mat &
  P17=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+18)) -clustal clustal-180.out -hb blosum80_small.mat &
  P18=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+19)) -clustal clustal-180.out -hb blosum80_small.mat &
  P19=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+20)) -clustal clustal-180.out -hb blosum80_small.mat &
  P20=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+21)) -clustal clustal-180.out -hb blosum80_small.mat &
  P21=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+22)) -clustal clustal-180.out -hb blosum80_small.mat &
  P22=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+23)) -clustal clustal-180.out -hb blosum80_small.mat &
  P23=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+24)) -clustal clustal-180.out -hb blosum80_small.mat &
  P24=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+25)) -clustal clustal-180.out -hb blosum80_small.mat &
  P25=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+26)) -clustal clustal-180.out -hb blosum80_small.mat &
  P26=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+27)) -clustal clustal-180.out -hb blosum80_small.mat &
  P27=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+28)) -clustal clustal-180.out -hb blosum80_small.mat &
  P28=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+29)) -clustal clustal-180.out -hb blosum80_small.mat &
  P29=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+30)) -clustal clustal-180.out -hb blosum80_small.mat &
  P30=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+31)) -clustal clustal-180.out -hb blosum80_small.mat &
  P31=$!
  ./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+32)) -clustal clustal-180.out -hb blosum80_small.mat &
  P32=$!
  wait $P1 $P2 $P3 $P4 $P5 $P6 $P7 $P8 $P9 $P10 $P11 $P12 $P13 $P14 $P15 $P16 $P17 $P18 $P19 $P20 $P21 $P22 $P23 $P24 $P25 $P26 $P27 $P28 $P29 $P30 $P31 $P32
done

./poa -read_fasta ../../datasets/poa/poa_993 -clustal clustal-180.out -hb blosum80_small.mat &
  P1=$!
./poa -read_fasta ../../datasets/poa/poa_994 -clustal clustal-180.out -hb blosum80_small.mat &
  P2=$!
./poa -read_fasta ../../datasets/poa/poa_995 -clustal clustal-180.out -hb blosum80_small.mat &
  P3=$!
./poa -read_fasta ../../datasets/poa/poa_996 -clustal clustal-180.out -hb blosum80_small.mat &
  P4=$!
./poa -read_fasta ../../datasets/poa/poa_997 -clustal clustal-180.out -hb blosum80_small.mat &
  P5=$!
./poa -read_fasta ../../datasets/poa/poa_998 -clustal clustal-180.out -hb blosum80_small.mat &
  P6=$!
./poa -read_fasta ../../datasets/poa/poa_999 -clustal clustal-180.out -hb blosum80_small.mat &
  P7=$!
./poa -read_fasta ../../datasets/poa/poa_1000 -clustal clustal-180.out -hb blosum80_small.mat &
  P8=$!
wait $P1 $P2 $P3 $P4 $P5 $P6 $P7 $P8

