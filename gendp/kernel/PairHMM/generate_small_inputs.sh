#!/bin/bash

for i in {35..150}
do
    ./pairhmm /x/yufenggu/input-data/phmm_inputs/phmm_large_${i}.in > prune_result_16_app_${i}.txt 2> /x/yufenggu/input-data/phmm_inputs/phmm_large_app_${i}.in
done
