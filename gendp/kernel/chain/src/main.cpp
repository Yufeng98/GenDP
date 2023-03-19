#include <cstdio>
#include <vector>
#include <time.h>
#include <sys/time.h>
#include <getopt.h>
#include <string>
#include <string.h>
#include <iostream>
#include "omp.h"
#include "host_data_io.h"
#include "host_data.h"
#include "host_kernel.h"

#if RAPL
#include "Rapl.h"
#endif

#ifdef VTUNE_ANALYSIS
#include <ittnotify.h>
#endif

void help() {
    std::cout <<
        "\n"
        "usage: ./chain [options ...]\n"
        "\n"
        "    options:\n"
        "        -i <input file>\n"
        "            default: NULL\n"
        "            the input anchor set\n"
        "        -o <output file>\n"
        "            default: NULL\n"
        "            the output scores, best predecessor set\n"
        "        -t <int>\n"
        "            default: 1\n"
        "            number of CPU threads\n"
        "        -h \n"
        "            prints the usage\n";
}

const char *__parsec_roi_begin(const char *s, int *beg, int *end)
{
    const char *colon = strrchr(s, ':');
    if (colon == NULL) {
        *beg = 0; *end = 0x7fffffff;
        return s + strlen(s);
    }
    return s + strlen(s);
}

const char *__parsec_roi_end(const char *s, int *beg, int *end)
{
    const char *colon = strrchr(s, ':');
    if (colon == NULL) {
        *beg = 0; *end = 0x7fffffff;
        return s + strlen(s);
    }
    return NULL;
}


int main(int argc, char **argv) {
#ifdef VTUNE_ANALYSIS
    __itt_pause();
#endif
    FILE *in, *out;
    std::string inputFileName, outputFileName;

    char opt;
    int numThreads = 1, setting = 0, input_size = -1;
    while ((opt = getopt(argc, argv, "i:o:t:s:n:h")) != -1) {
        switch (opt) {
            case 'i': inputFileName = optarg; break;
            case 'o': outputFileName = optarg; break;
            case 't': numThreads = atoi(optarg); break;
            case 's': setting = atoi(optarg); break;
            case 'n': input_size = atoi(optarg); break;
            case 'h': help(); return 0;
            default: help(); return 1;
        }
    }

    if (argc == 1 || argc != optind) {
        help();
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Input file: %s\n", inputFileName.c_str());
    fprintf(stderr, "Output file: %s\n", outputFileName.c_str());

    in = fopen(inputFileName.c_str(), "r");
    out = fopen(outputFileName.c_str(), "w");

    std::vector<call_t> calls;
    std::vector<return_t> rets;

    for (call_t call = read_call(in);
            call.n != ANCHOR_NULL;
            call = read_call(in)) {
        calls.push_back(call);
    }

    rets.resize(calls.size());


#pragma omp parallel num_threads(numThreads)
{
    int tid = omp_get_thread_num();
    if (tid == 0) {
        fprintf(stderr, "Running with threads: %d\n", numThreads);
    }
}

    struct timeval start_time, end_time;
    double runtime = 0;
    
#ifdef VTUNE_ANALYSIS
    const char *roi_q;
    int roi_i, roi_j;
    char roi_s[20] = "chr22:0-5";
    roi_q = __parsec_roi_begin(roi_s, &roi_i, &roi_j);
    __itt_resume();
#endif

#if RAPL
	// int outer_cnt = 1;
	float pkg_idle_power = 14.408;
  	float dram_idle_power = 8.174;
	Rapl * rapl = new Rapl();
	// float pkg_energy_sum = 0.0;
  	// float dram_energy_sum = 0.0;
	// reset rapl
	rapl->reset();
#endif

    gettimeofday(&start_time, NULL);

    host_chain_kernel(calls, rets, numThreads, setting, input_size);

    gettimeofday(&end_time, NULL);

#if RAPL
	// sample
	rapl->sample();
	// report total energy
	float total_time = rapl->total_time();
	float pkg_energy = rapl->pkg_total_energy();
	//float pp0_energy = rapl->pp0_total_energy();
	//float pp1_energy = rapl->pp1_total_energy();
	float dram_energy = rapl->dram_total_energy();
	float pkg_power = rapl->pkg_average_power();
	float dram_power = rapl->dram_average_power();
	printf("Running time is %f sec.\n", total_time);
	printf("Energy: pkg %f J; DRAM %f J\n", pkg_energy, dram_energy);
	printf("Power: pkg %f W; DRAM %f W\n", pkg_power, dram_power);
	// float avg_pkg_energy = pkg_energy_sum / (numThreads * outer_cnt);
	// printf("Avg pkg energy is %.9f J.\n", avg_pkg_energy);
	// printf("Total energy: pp0 %f J; pp1 %f J\n", pp0_energy, pp1_energy);
	// float offset_total_pkg_energy = (pkg_energy - total_time * pkg_idle_power);
	// float offset_total_dram_energy = (dram_energy - total_time * dram_idle_power);
	// printf("Corrected total energy (remove idle): pkg %f J; DRAM %f J\n", offset_total_pkg_energy, offset_total_dram_energy);
	// float offset_total_pkg_power = (pkg_power - pkg_idle_power);
	// float offset_total_dram_power = (dram_power - dram_idle_power);
	// printf("Corrected total power (remove idle): pkg %f W; DRAM %f W\n", offset_total_pkg_power, offset_total_dram_power);
	// float offset_avg_pkg_energy = (pkg_energy_sum-total_time*pkg_idle_power) / (numThreads * outer_cnt);
	// float offset_avg_dram_energy = (dram_energy_sum-total_time*dram_idle_power) / (numThreads * outer_cnt);
	// printf("Corrected average pkg energy is %.9f J.\n", offset_avg_pkg_energy);
	// printf("Corrected average DRAM energy is %.9f J.\n", offset_avg_dram_energy);
#endif

#ifdef VTUNE_ANALYSIS
    roi_q = __parsec_roi_end(roi_s, &roi_i, &roi_j);
    __itt_pause();
#endif

    runtime += (end_time.tv_sec - start_time.tv_sec) * 1e6 + (end_time.tv_usec - start_time.tv_usec);
    
#ifdef PRINT_OUTPUT
    for (auto it = rets.begin(); it != rets.end(); it++) {
        print_return(out, *it);
    }
#endif

    fprintf(stderr, "Time in kernel: %.2f sec\n", runtime * 1e-6);

    fclose(in);
    fclose(out);

    return 0;
}
