#include "pe_array.h"
#include "sys_def.h"
#include "bsw.h"
#include "phmm.h"
#include "poa.h"
#include "chain.h"
#include <getopt.h>

void help() {
    std::cout <<
        "\n"
        "usage: ./sim [options ...]\n"
        "\n"
        "    options:\n"
        "        -k kernel\n"
        "            default: 0\n"
        "            1 - bsw"
        "            2 - phmm"
        "            3 - poa"
        "            4 - chain"
        "        -i <input file>\n"
        "            default: NULL\n"
        "        -o <output file>\n"
        "            default: NULL\n"
        "        -h \n"
        "            prints the usage\n";
}


int main(int argc, char *argv[]) {

    FILE *fp = NULL;
    char *inputFileName = NULL, *outputFileName = NULL;
    char opt;
    int show_output = 0, kernel = 0;
    while ((opt = getopt(argc, argv, "k:i:o:sh")) != -1) {
        switch (opt) {
            case 'k': kernel = atoi(optarg); break;
            case 'i': inputFileName = optarg; break;
            case 'o': outputFileName = optarg; break;
            case 's': show_output = 1; break;
            case 'h': help(); return 0;
            default: help(); return 1;
        }
    }

    if (!kernel) fprintf(stderr, "Please specify kernel.\n");
    else if (kernel == 1) bsw_simulation(inputFileName, outputFileName, fp, show_output);
    else if (kernel == 2) phmm_simulation(inputFileName, outputFileName, fp, show_output);
    else if (kernel == 3) poa_simulation(inputFileName, outputFileName, fp, show_output);
    else if (kernel == 4) chain_simulation(inputFileName, outputFileName, fp, show_output);

    return 0;
}