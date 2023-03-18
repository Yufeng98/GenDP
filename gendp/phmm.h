#ifndef PHMM_H
#define PHMM_H

// PHMM
#define PHMM_PE_GROUP_SIZE 4
#define PHMM_COMPUTE_INSTRUCTION_NUM 15
// #define PHMM_MAX_INPUT 10000
#define PHMM_MAX_INPUT 1420266
#define NUM_INTEGER_BITS 5
#define NUM_FRACTION_BITS 16

#define PHMM_CONSTANT_XX -217705
#define PHMM_CONSTANT_GM -9961

typedef struct phmm_input {
    int len_read, len_hap, last_raw_index, *mm, *mx, *my, *read_base, *read_base_qual, *hap_base;
} phmm;

int phmm_read_input(phmm *phmm_input, std::string phmm_input_file, unsigned long phmm_compute_instruction[][COMP_INSTR_BUFFER_GROUP_SIZE], unsigned long phmm_main_instruction[], unsigned long phmm_pe_instruction[][CTRL_INSTR_BUFFER_NUM][CTRL_INSTR_BUFFER_GROUP_SIZE]);
void phmm_simulate(pe_array *pe_array_unit, phmm phmm_input, int n, FILE* fp, int show_output, int* output);
void phmm_simulation(char *inputFileName, char *outputFileName, FILE *fp, int show_output, int simulation_cases);

#endif