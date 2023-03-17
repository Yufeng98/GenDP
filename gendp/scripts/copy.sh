#!/bin/bash

scp yufenggu@mbit7.eecs.umich.edu:/x/yufenggu/input-data/bsw_147_1m_8bit_input.txt .
scp -r yufenggu@mbit7.eecs.umich.edu:/x/yufenggu/input-data/bsw_sim_result .
scp yufenggu@mbit7.eecs.umich.edu:/z/scratch7/yufenggu/input-datasets/chain/large/c_elegans_40x.10k.in .
scp yufenggu@mbit7.eecs.umich.edu:/z/scratch7/yufenggu/input-datasets/chain/small/in-1k.txt .
scp yufenggu@mbit7.eecs.umich.edu:/x/yufenggu/input-data/chain_output.txt .
scp -r yufenggu@mbit7.eecs.umich.edu:/x/yufenggu/input-data/chain_sim_result .
# scp -r yufenggu@mbit7.eecs.umich.edu:/x/yufenggu/input-data/poa .
scp -r yufenggu@mbit7.eecs.umich.edu:/x/yufenggu/input-data/poa_input .
scp -r yufenggu@mbit7.eecs.umich.edu:/x/yufenggu/input-data/poa_output .
scp -r yufenggu@mbit7.eecs.umich.edu:/x/yufenggu/input-data/poa_sim_result .
scp yufenggu@mbit7.eecs.umich.edu:/x/yufenggu/input-data/phmm_large_app.in .
scp -r yufenggu@mbit7.eecs.umich.edu:/x/yufenggu/input-data/phmm_inputs .
scp -r yufenggu@mbit7.eecs.umich.edu:/x/yufenggu/input-data/phmm_output .
scp -r yufenggu@mbit7.eecs.umich.edu:/x/yufenggu/input-data/phmm_sim_result .