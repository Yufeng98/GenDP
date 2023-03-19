#include "sys_def.h"
#include "pe_array.h"
#include "phmm.h"


int phmm_read_input(phmm *phmm_input, std::string phmm_input_file, unsigned long phmm_compute_instruction[][COMP_INSTR_BUFFER_GROUP_SIZE], unsigned long phmm_main_instruction[][CTRL_INSTR_BUFFER_GROUP_SIZE], unsigned long phmm_pe_instruction[][CTRL_INSTR_BUFFER_NUM][CTRL_INSTR_BUFFER_GROUP_SIZE]) {

    int i;
    std::string phmm_compute_instruction_file = "instructions/phmm/compute_instruction.txt";
    std::string phmm_main_instruction_file = "instructions/phmm/main_instruction.txt";
    std::string phmm_pe_instruction_file[PHMM_PE_GROUP_SIZE];
    for (i=0; i<PHMM_PE_GROUP_SIZE; i++)
        phmm_pe_instruction_file[i] = "instructions/phmm/pe_" + std::to_string(i) + "_instruction.txt";
    int phmm_input_index = -1, read_index = 0, read_begin = 0;
    std::string line;
    std::fstream fp_phmm_input, fp_phmm_compute_instruction, fp_phmm_main_instruction, fp_phmm_pe_instruction[PHMM_PE_GROUP_SIZE];
    int item;

    long long *dram_load_size, *dram_store_size;
    dram_load_size = (long long*)malloc(sizeof(long long));
    dram_store_size = (long long*)malloc(sizeof(long long));
    *dram_load_size = 0;
    *dram_store_size = 0;

    fp_phmm_input.open(phmm_input_file, std::ios::in);
    if (fp_phmm_input.is_open()) {
        while(getline(fp_phmm_input, line)) {
            if (line[0] == ">"[0]) {
                phmm_input_index++; read_index = 0; continue; 
            } else if (read_index == 0) {
                std::istringstream ss(line);
                ss >> item; phmm_input[phmm_input_index].len_read = item;
                ss >> item; phmm_input[phmm_input_index].last_raw_index = item;
                ss >> item; phmm_input[phmm_input_index].len_hap = item;
                *dram_load_size += 3 * 4;   // input is 3 integers above
                *dram_store_size += 1 * 4;  // output is a single 4 Byte value
                phmm_input[phmm_input_index].mm = (int*)malloc(phmm_input[phmm_input_index].len_read * sizeof(int));
                phmm_input[phmm_input_index].mx = (int*)malloc(phmm_input[phmm_input_index].len_read * sizeof(int));
                phmm_input[phmm_input_index].my = (int*)malloc(phmm_input[phmm_input_index].len_read * sizeof(int));
                phmm_input[phmm_input_index].read_base = (int*)malloc(phmm_input[phmm_input_index].len_read * sizeof(int));
                phmm_input[phmm_input_index].read_base_qual = (int*)malloc(phmm_input[phmm_input_index].len_read * sizeof(int));
                phmm_input[phmm_input_index].hap_base = (int*)malloc((phmm_input[phmm_input_index].len_hap + 4) * sizeof(int));
                read_index++; read_begin = read_index; continue; 
            } else if (read_index < read_begin + phmm_input[phmm_input_index].len_read) {
                std::istringstream ss(line);
                *dram_load_size += 5;   // input is 5 values below with each 1 Byte size
                ss >> item; phmm_input[phmm_input_index].mm[read_index - read_begin] = item;
                ss >> item; phmm_input[phmm_input_index].mx[read_index - read_begin] = item;
                ss >> item; phmm_input[phmm_input_index].my[read_index - read_begin] = item;
                ss >> item; phmm_input[phmm_input_index].read_base[read_index - read_begin] = item;
                ss >> item; phmm_input[phmm_input_index].read_base_qual[read_index - read_begin] = item;
                read_index++; continue; 
            } else if (read_index < read_begin + phmm_input[phmm_input_index].len_read + phmm_input[phmm_input_index].len_hap) {
                std::istringstream ss(line);
                ss >> item; phmm_input[phmm_input_index].hap_base[read_index - read_begin - phmm_input[phmm_input_index].len_read] = item;
                *dram_load_size += 1;
                read_index++; continue; 
            }
        }
    } else {
        fprintf(stderr, "Cannot open file %s.\n", phmm_input_file.c_str());
        exit(-1);
    }
    fp_phmm_input.close();

    printf("DRAM load %llu\n", *dram_load_size);
    printf("DRAM store %llu\n", *dram_store_size);
    free(dram_load_size);
    free(dram_store_size);

    fp_phmm_compute_instruction.open(phmm_compute_instruction_file, std::ios::in);
    if (fp_phmm_compute_instruction.is_open()) {
        read_index = 0;
        while(getline(fp_phmm_compute_instruction, line)) {
            phmm_compute_instruction[read_index/2][read_index%2] = std::stol(line, 0, 16);
            read_index++;
        }
    } else {
        fprintf(stderr, "Cannot open file %s.\n", phmm_compute_instruction_file.c_str());
        exit(-1);
    }
    fp_phmm_compute_instruction.close();

    fp_phmm_main_instruction.open(phmm_main_instruction_file, std::ios::in);
    if (fp_phmm_main_instruction.is_open()) {
        read_index = 0;
        while(getline(fp_phmm_main_instruction, line)) {
            phmm_main_instruction[read_index/2][read_index%2] = std::stol(line, 0, 16);
            read_index++;
        }
    } else {
        fprintf(stderr, "Cannot open file %s.\n", phmm_main_instruction_file.c_str());
        exit(-1);
    }
    fp_phmm_main_instruction.close();

    for (i = 0; i < PHMM_PE_GROUP_SIZE; i++) {
        fp_phmm_pe_instruction[i].open(phmm_pe_instruction_file[i], std::ios::in);
        if (fp_phmm_pe_instruction[i].is_open()) {
            read_index = 0;
            while(getline(fp_phmm_pe_instruction[i], line)) {
                phmm_pe_instruction[i][read_index/2][read_index%2] = std::stol(line, 0, 16);
                read_index++;
            }
        } else {
            fprintf(stderr, "Cannot open file %s.\n", phmm_pe_instruction_file[i].c_str());
            exit(-1);
        }
        fp_phmm_pe_instruction[i].close();
    }

    return phmm_input_index;
}

int Float2Fix_phmm(float exact_value) {
    if (exact_value == - std::numeric_limits<float>::infinity())
        return -pow(2, NUM_FRACTION_BITS+NUM_INTEGER_BITS);
    int result = (int)ceil(exact_value * pow(2, NUM_FRACTION_BITS));
        return result;
}

int Upper_LOG2_accurate_phmm(float num){
    float numLog2 = log(num) / log(2);
    int result = Float2Fix_phmm(numLog2);
    return result;
  }

void phmm_simulate(pe_array *pe_array_unit, phmm* phmm_input, int n, FILE* fp, int show_output, int* output) {

    int i, zero = 0, XX = PHMM_CONSTANT_XX, GM = PHMM_CONSTANT_GM, init;
    int simd = 0;
    float INITIAL_CONDITION = (float)pow(2, 127);
    init = Upper_LOG2_accurate_phmm(INITIAL_CONDITION / phmm_input->len_hap);
    pe_array_unit->buffer_reset(pe_array_unit->input_buffer, 1024);
    pe_array_unit->buffer_reset(pe_array_unit->output_buffer, 1024);
    pe_array_unit->main_PC = 0;
    for (i = 0; i < PHMM_PE_GROUP_SIZE; i++) pe_array_unit->pe_unit[i]->reset();
    for (i = 0; i < FIFO_GROUP_SIZE; i++) pe_array_unit->fifo_unit[0][i].clear();

    // Load data from input file to input buffer
    pe_array_unit->input_buffer_write_from_ddr(0, &phmm_input->len_read);
    pe_array_unit->input_buffer_write_from_ddr(1, &phmm_input->len_hap);
    pe_array_unit->input_buffer_write_from_ddr(2, &phmm_input->last_raw_index);
    pe_array_unit->input_buffer_write_from_ddr(3, &init);
    pe_array_unit->input_buffer_write_from_ddr(4, &zero);
    // pe_array_unit->input_buffer_write_from_ddr(4, -2097152);
    pe_array_unit->input_buffer_write_from_ddr(5, &XX);
    pe_array_unit->input_buffer_write_from_ddr(6, &GM);
    for (i = 0; i < phmm_input->len_read; i++) {
        pe_array_unit->input_buffer_write_from_ddr(7+i*5+0, &phmm_input->mm[i]);
        pe_array_unit->input_buffer_write_from_ddr(7+i*5+1, &phmm_input->mx[i]);
        pe_array_unit->input_buffer_write_from_ddr(7+i*5+2, &phmm_input->my[i]);
        pe_array_unit->input_buffer_write_from_ddr(7+i*5+4, &phmm_input->read_base_qual[i]);
        pe_array_unit->input_buffer_write_from_ddr(7+i*5+3, &phmm_input->read_base[i]);
    }
    for (i = 0; i < phmm_input->len_hap; i++) {
        pe_array_unit->input_buffer_write_from_ddr(7+i+phmm_input->len_read*5, &phmm_input->hap_base[i]);
    }
    for (i = 0; i < 4; i++)
        pe_array_unit->input_buffer_write_from_ddr(7+i+phmm_input->len_hap+phmm_input->len_read*5, &zero);


    printf("cells %d ", phmm_input->len_read * phmm_input->len_hap);
    pe_array_unit->run(n, simd, PE_4_SETTING, MAIN_INSTRUCTION_2);

    if (show_output) pe_array_unit->phmm_show_output_buffer(fp);

    

}

void phmm_simulation(char *inputFileName, char *outputFileName, FILE *fp, int show_output, int simulation_cases) {

    int i, j;
    pe_array *pe_array_unit = new pe_array(1024, 1024);

    unsigned long phmm_compute_instruction[PHMM_COMPUTE_INSTRUCTION_NUM][COMP_INSTR_BUFFER_GROUP_SIZE];
    unsigned long phmm_main_instruction[CTRL_INSTR_BUFFER_NUM][CTRL_INSTR_BUFFER_GROUP_SIZE];
    unsigned long phmm_pe_instruction[PHMM_PE_GROUP_SIZE][CTRL_INSTR_BUFFER_NUM][CTRL_INSTR_BUFFER_GROUP_SIZE];
    for (i = 0; i < PHMM_COMPUTE_INSTRUCTION_NUM; i++) {
        phmm_compute_instruction[i][0] = 0x20f7800000000;
        phmm_compute_instruction[i][1] = 0x20f7800000000;
    }
    for (i = 0; i < PHMM_PE_GROUP_SIZE; i++) {
        for (j = 0; j < CTRL_INSTR_BUFFER_NUM; j++) {
            phmm_pe_instruction[i][j][0] = 0xf;
            phmm_pe_instruction[i][j][1] = 0xf;
        }
    }
    for (i = 0; i < CTRL_INSTR_BUFFER_NUM; i++) {
        phmm_main_instruction[i][0] = 0xf;
        phmm_main_instruction[i][1] = 0xf;
    }

    // phmm phmm_input[PHMM_MAX_INPUT];
    phmm* phmm_input;
    phmm_input = (phmm*)malloc(PHMM_MAX_INPUT * sizeof(phmm));
    fprintf(stderr, "read input %s\n", inputFileName);
    int phmm_input_num = phmm_read_input(phmm_input, inputFileName, phmm_compute_instruction, phmm_main_instruction, phmm_pe_instruction);

    // Load data from input file to instruction buffer
    for (i = 0; i < PHMM_COMPUTE_INSTRUCTION_NUM; i++) {
        pe_array_unit->compute_instruction_buffer_write_from_ddr(i, phmm_compute_instruction[i]);
    }

    // Load main & pe instructions into pe_array instruction buffer
    for (i = 0; i < CTRL_INSTR_BUFFER_NUM; i++) {
        pe_array_unit->main_instruction_buffer_write_from_ddr(i, phmm_main_instruction[i]);
        for (j = 0; j < PHMM_PE_GROUP_SIZE; j++)
            pe_array_unit->pe_instruction_buffer_write_from_ddr(i, phmm_pe_instruction[j][i], j);
    }

	if (show_output) fp = fopen(outputFileName, "w");

    int* phmm_output;
    int index = 0;
    phmm_output = (int*)malloc(phmm_input_num * sizeof(int));

    printf("Start simulation.\n");
    if (simulation_cases < 0 || simulation_cases >= phmm_input_num) {
        for (i = 0; i <= phmm_input_num; i++) {
            phmm_simulate(pe_array_unit, phmm_input+i, 10000000, fp, show_output, phmm_output+index);
            index++;
        }
    } else {
        for (i = 0; i <= simulation_cases; i++) {
            phmm_simulate(pe_array_unit, phmm_input+i, 10000000, fp, show_output, phmm_output+index);
            index++;
        }
    }
        
    
    if (show_output) fclose(fp);
    free(phmm_output);

    for (i = 0; i <= phmm_input_num; i++) {
        free(phmm_input[i].mm);
        free(phmm_input[i].mx);
        free(phmm_input[i].my);
        free(phmm_input[i].read_base);
        free(phmm_input[i].read_base_qual);
        free(phmm_input[i].hap_base);
    }
    free(phmm_input);

    delete pe_array_unit;
}