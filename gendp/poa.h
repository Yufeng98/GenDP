#ifndef POA_H
#define POA_H

// POA
#define POA_PE_GROUP_SIZE 4
#define POA_COMPUTE_INSTRUCTION_NUM 29
#define POA_MAX_INPUT 200
#define POA_MAX_SEQ_LEN 2048
#define POA_MAX_EDGE_LEN 4096
#define POA_INIT_SCORE 0
#define POA_INIT_GAP 17
#define POA_CONSTANT_1 1
#define POA_CONSTANT_6 6
#define POA_CONSTANT_12 12
#define POA_CONSTANT_16 16
#define POA_MIN_SCORE -999999
#define POA_CONSTANT_MINUS_1 -1

typedef struct poa_input {
    int len_y, len_y_unpadding, *seq_y, *index_y, len_x, *seq_x, *index_x;
    int len_pos, *pred_pos, *pred_num;
    int best_x, best_y, best_score;
} poa;

int poa_read_input(poa *poa_input, std::string poa_input_file, unsigned long poa_compute_instruction[][COMP_INSTR_BUFFER_GROUP_SIZE], unsigned long poa_main_instruction[], unsigned long poa_pe_instruction[][CTRL_INSTR_BUFFER_NUM][CTRL_INSTR_BUFFER_GROUP_SIZE]);
void poa_simulate(pe_array *pe_array_unit, poa poa_input, int n, FILE* fp, int show_output, int* output);
void poa_print_input(poa poa_input);
void poa_simulation(char *inputFileName, char *outputFileName, FILE *fp, int show_output, int simulation_cases);

#endif