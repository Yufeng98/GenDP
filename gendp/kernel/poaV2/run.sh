#!/bin/bash

num_thread_group=2
num_threads=4

for i in {0..1}
do
  ./poa -datasets_path ../../../gendp-datasets -read_fasta ../../../gendp-datasets/poa/poa_$((i*num_threads+1)) -clustal clustal-180.out -hb blosum80_small.mat &
  P1=$!
  ./poa -datasets_path ../../../gendp-datasets -read_fasta ../../../gendp-datasets/poa/poa_$((i*num_threads+2)) -clustal clustal-180.out -hb blosum80_small.mat &
  P2=$!
  ./poa -datasets_path ../../../gendp-datasets -read_fasta ../../../gendp-datasets/poa/poa_$((i*num_threads+3)) -clustal clustal-180.out -hb blosum80_small.mat &
  P3=$!
  ./poa -datasets_path ../../../gendp-datasets -read_fasta ../../../gendp-datasets/poa/poa_$((i*num_threads+4)) -clustal clustal-180.out -hb blosum80_small.mat &
  P4=$!
  wait $P1 $P2 $P3 $P4
done

./poa -datasets_path ../../../gendp-datasets -read_fasta ../../../gendp-datasets/poa/poa_9 -clustal clustal-180.out -hb blosum80_small.mat &
  P1=$!
./poa -datasets_path ../../../gendp-datasets -read_fasta ../../../gendp-datasets/poa/poa_10 -clustal clustal-180.out -hb blosum80_small.mat &
  P2=$!
wait $P1 $P2

