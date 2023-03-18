#!/bin/bash

num_thread_group=1
num_threads=8

for i in {0..0}
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
  wait $P1 $P2 $P3 $P4 $P5 $P6 $P7 $P8
done

./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+1)) -clustal clustal-180.out -hb blosum80_small.mat &
  P1=$!
./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+2)) -clustal clustal-180.out -hb blosum80_small.mat &
  P2=$!
./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+3)) -clustal clustal-180.out -hb blosum80_small.mat &
  P3=$!
./poa -read_fasta ../../datasets/poa/poa_$((i*num_threads+4)) -clustal clustal-180.out -hb blosum80_small.mat &
  P4=$!
wait $P1 $P2 $P3 $P4

