/*************************************************************************************
MIT License

Copyright (c) 2020 Intel Labs

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Authors: Saurabh Kalikar <saurabh.kalikar@intel.com>; Vasimuddin Md <vasimuddin.md@intel.com>; Sanchit Misra <sanchit.misra@intel.com>; Chirag Jain
*****************************************************************************************/

#include<iostream>
#include<fstream>
#include <unistd.h>
#include <map>
#include <x86intrin.h>
#ifdef VTUNE_ANALYSIS
#include <ittnotify.h>
#endif
#include<vector>
#include "parallel_chaining_32_bit.h"
//#include "parallel_chaining_32_bit_v2_22.h"

// #define RAPL 1
#if RAPL
    #include "Rapl.h"
#endif

using namespace std;

#ifdef INT64
typedef int64_t num_bits_t;
#else
typedef uint32_t num_bits_t;
#endif

int main(int argc, char* argv[]){
#ifdef VTUNE_ANALYSIS
    __itt_pause();
#endif
#if RAPL
    Rapl* rapl = new Rapl();
#endif
#ifdef INT64
	fprintf(stderr, "Running with 64 bit number representation\n");
#else
	fprintf(stderr, "Running with 32 bit number representation\n");
#endif
#ifdef __AVX512BW__
        fprintf(stderr, "Running AVX512 code\n");
#endif
	int numThreads = 1;
	if(argc != 2 && argc != 3){
		fprintf(stderr, "Input parameters: 1. Anchors_file_name. 2. Number of threads (default: 1)\n");
		exit(0);
	}

	if (argc > 2) numThreads = atoi(argv[2]);
	vector<vector<anchor_t>> anc_v;
	vector<num_bits_t*>ar_v;
	vector<num_bits_t*>aq_v;
	vector<num_bits_t*>al_v;
	vector<num_bits_t*>f_v;
	vector<int32_t*>p_v;
	vector<int32_t*>v_v;


	dp_chain obj(5000,5000,500,25,5000,1.000000,0,1);
	uint64_t total_ticks = 0;

	uint64_t r;
	int32_t q, l, id;

	ifstream fin(argv[1]);
	while(fin>>id){
		vector<anchor_t> anc;
		//fprintf(stderr, "%d\n", id);
		while(id--){
			fin>>r>>q>>l;
			//fprintf(stderr, "%lu\t%d\t%d\n", r, q, l);
			anchor_t a(r,q,l);
			anc.push_back(a);
		}
		int32_t n = anc.size();
		num_bits_t *f = (num_bits_t*)malloc(sizeof(num_bits_t)*(n+16)); f = f + 16;
		int32_t *p = (int32_t*)malloc(sizeof(num_bits_t)*(n+16)); p = p + 16;
		int32_t *v = (int32_t*)malloc(sizeof(num_bits_t)*(n+16)); v = v + 16;

		num_bits_t *anchor_r, *anchor_q, *anchor_l;
		create_SoA_Anchors_32_bit(&anc[0], anc.size(), anchor_r, anchor_q, anchor_l);

		anc_v.push_back(anc);
		ar_v.push_back(anchor_r);
		aq_v.push_back(anchor_q);
		al_v.push_back(anchor_l);
		f_v.push_back(f);
		p_v.push_back(p);
		v_v.push_back(v);
	}
		uint64_t start =  __rdtsc();
#ifdef VTUNE_ANALYSIS
        __itt_resume();
#endif
#if RAPL
    rapl->reset();
#endif

#pragma omp parallel num_threads(numThreads)
{
#pragma omp for schedule(dynamic)
	for (int it = 0; it < anc_v.size(); it++)
	{
		obj.mm_dp_vectorized(anc_v[it].size(), &anc_v[it][0], ar_v[it], aq_v[it], al_v[it], f_v[it], p_v[it], v_v[it], 5000, 5000, NULL, NULL);	
#ifdef DEBUG
		for(int j = 0; j < anc_v[it].size(); j++){
			cout<<f_v[it][j]<<" "<<p_v[it][j]<<"\n";
		}
#endif
	}
}
#ifdef VTUNE_ANALYSIS
        __itt_pause();
#endif
		uint64_t end =  __rdtsc();
		total_ticks += end-start;

#if RAPL
    rapl->sample();
    float total_time = rapl->total_time();
    float pkg_power = rapl->pkg_average_power();
    float dram_power = rapl->dram_average_power();
    printf("Running time is %f sec.\n", total_time);
    printf("Power: pkg %f W; DRAM %f W\n", pkg_power, dram_power);
#endif
		uint64_t tim = __rdtsc();
		sleep(1);
		double freq = __rdtsc() - tim;
	
		fprintf(stderr, "Processor freq: %0.2lf MHz\n", freq/1e6);
		fprintf(stderr, "Total ticks = %lld, %0.2lf s\n", total_ticks, total_ticks * 1.0 / freq);
}
