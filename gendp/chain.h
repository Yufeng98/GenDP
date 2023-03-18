#ifndef CHAIN_H
#define CHAIN_H

#include <stdint.h>

// CHAIN
#define CHAIN_PE_GROUP_SIZE 64
#define CHAIN_COMPUTE_INSTRUCTION_NUM 12

#define CHAIN_CONSTANT_1 1
#define CHAIN_CONSTANT_16 16
#define CHAIN_CONSTANT_MIN -999999

typedef int64_t anchor_idx_t;
typedef int32_t score_t;
typedef int32_t parent_t;

#define ANCHOR_NULL (anchor_idx_t)(-1)

struct anchor_t {
    uint64_t x;
    uint64_t y;
};

struct call_t {
    anchor_idx_t n;
    float avg_qspan;
    int max_dist_x, max_dist_y, bw, n_segs;
    std::vector<anchor_t> anchors;
};

struct call_t_array {
    anchor_idx_t n;
    float avg_qspan;
    int max_dist_x, max_dist_y, bw, n_segs;
    anchor_t* anchors;
    int q_span, tmp_0, tmp_1;
};

void skip_to_EOR(FILE *fp);

call_t read_call(FILE *fp);

std::vector<call_t> chain_read_input(std::string chain_input_file, unsigned long chain_compute_instruction[][COMP_INSTR_BUFFER_GROUP_SIZE], unsigned long chain_main_instruction[], unsigned long chain_pe_instruction[][CTRL_INSTR_BUFFER_NUM][CTRL_INSTR_BUFFER_GROUP_SIZE]);
void chain_simulate(pe_array *pe_array_unit, call_t chain_input, int n, FILE* fp, int show_output, int* output);
void chain_simulation(char *inputFileName, char *outputFileName, FILE *fp, int show_output, int simulation_cases);

#endif