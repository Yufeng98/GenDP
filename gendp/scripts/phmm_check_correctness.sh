#!/bin/bash

for i in {0..34}
do
    diff phmm_output/phmm_output_${i}.txt ../kernel/PairHMM/prune_result_16_app_${i}.txt
done