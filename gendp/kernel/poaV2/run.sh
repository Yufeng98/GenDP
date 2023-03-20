#!/bin/bash

num_thread_group=10
num_threads=1

for i in {0..9}
do
  ./poa -datasets_path ../../../gendp-datasets -read_fasta ../../../gendp-datasets/poa/poa_$((i*num_threads+1)) -clustal tmp -hb blosum80_small.mat &
  P1=$!
  wait $P1
done

