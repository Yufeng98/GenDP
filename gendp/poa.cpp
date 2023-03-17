#include "sys_def.h"
#include "pe_array.h"
#include "poa.h"

int poa_read_input(poa *poa_input, std::string poa_input_file, unsigned long poa_compute_instruction[][COMP_INSTR_BUFFER_GROUP_SIZE], unsigned long poa_main_instruction[], unsigned long poa_pe_instruction[][CTRL_INSTR_BUFFER_NUM][CTRL_INSTR_BUFFER_GROUP_SIZE]) {

    int i;
    std::string poa_compute_instruction_file = "../data/poa/compute_instruction.txt";
    std::string poa_main_instruction_file = "../data/poa/main_instruction.txt";
    std::string poa_pe_instruction_file[POA_PE_GROUP_SIZE];
    poa_pe_instruction_file[0] = "../data/poa/pe_0_instruction.txt";
    poa_pe_instruction_file[1] = "../data/poa/pe_1_instruction.txt";
    poa_pe_instruction_file[2] = "../data/poa/pe_2_instruction.txt";
    poa_pe_instruction_file[3] = "../data/poa/pe_3_instruction.txt";
    int poa_input_index = -1, read_index = 0, pred_num_begin = 0, pred_pos_begin = 0, seq_y_begin = 0, seq_x_begin = 0;
    std::string line;
    std::fstream fp_poa_input, fp_poa_compute_instruction, fp_poa_main_instruction, fp_poa_pe_instruction[POA_PE_GROUP_SIZE];
    int item;
    // int index, move_begin = 0;

    long long *dram_load_size, *dram_store_size;
    dram_load_size = (long long*)malloc(sizeof(long long));
    dram_store_size = (long long*)malloc(sizeof(long long));
    *dram_load_size = 0;
    *dram_store_size = 0;

    fp_poa_input.open(poa_input_file, std::ios::in);
    if (fp_poa_input.is_open()) {
        while(getline(fp_poa_input, line)) {
            if (line[0] == ">"[0]) { poa_input_index++; read_index = 0; continue; }
            else if (read_index == 0) {
                // read len_y and allocate for seq_y
                std::istringstream ss(line);
                ss >> item; poa_input[poa_input_index].len_y = item;
                ss >> item; poa_input[poa_input_index].len_y_unpadding = item;
                // size of y sequence is a 4-byte integer and each charachter requires 2-bit (0.25 Byte)
                *dram_load_size += 4 + round(poa_input[poa_input_index].len_y * 0.25);  
                poa_input[poa_input_index].seq_y = (int*)malloc(poa_input[poa_input_index].len_y * sizeof(int));
                read_index++; seq_y_begin = read_index; continue; 
            } else if (read_index < seq_y_begin + poa_input[poa_input_index].len_y) {
                // read seq_y
                std::istringstream ss(line);
                ss >> item; poa_input[poa_input_index].seq_y[read_index - seq_y_begin] = item;
                read_index++; continue;
            } else if (read_index == seq_y_begin + poa_input[poa_input_index].len_y) {
                // read len_x and allocate for seq_x
                poa_input[poa_input_index].len_x = std::stoi(line); 
                // size of x sequence is a 4-byte integer and each charachter requires 2-bit (0.25 Byte)
                *dram_load_size += 4 + round(poa_input[poa_input_index].len_x * 0.25);
                // x axis output requires 1 Byte (0~127), y axis output requires 1 bit (0/1)
                *dram_store_size += poa_input[poa_input_index].len_x * poa_input[poa_input_index].len_y * (1+0.125);
                poa_input[poa_input_index].seq_x = (int*)malloc(poa_input[poa_input_index].len_x * sizeof(int));
                read_index++; seq_x_begin = read_index; continue;
            } else if (read_index < seq_x_begin + poa_input[poa_input_index].len_x) {
                // read seq_x
                std::istringstream ss(line);
                ss >> item; poa_input[poa_input_index].seq_x[read_index - seq_x_begin] = item;
                read_index++; continue;
            } else if (read_index == seq_x_begin + poa_input[poa_input_index].len_x) {
                // allocte for pred_num
                poa_input[poa_input_index].pred_num = (int*)malloc(poa_input[poa_input_index].len_x * sizeof(int));
                read_index++; pred_num_begin = read_index; continue; 
                // each node in x axis requires 1 Byte to store how many in edges it has
                *dram_load_size += round(poa_input[poa_input_index].len_x);
            } else if (read_index < pred_num_begin + poa_input[poa_input_index].len_x) {
                // read pred_num
                poa_input[poa_input_index].pred_num[read_index-pred_num_begin] = std::stoi(line);
                read_index++; continue;
            } else if (read_index == pred_num_begin + poa_input[poa_input_index].len_x) {
                poa_input[poa_input_index].len_pos = std::stoi(line); 
                poa_input[poa_input_index].pred_pos = (int*)malloc(poa_input[poa_input_index].len_pos * sizeof(int));
                // each in coming edge requires 1 Byte
                *dram_load_size += poa_input[poa_input_index].len_pos;
                read_index++; pred_pos_begin = read_index; continue; 
            } else if (read_index < pred_pos_begin + poa_input[poa_input_index].len_pos) {
                poa_input[poa_input_index].pred_pos[read_index-pred_pos_begin] = std::stoi(line);
                read_index++; continue;
            } else if (read_index == pred_pos_begin + poa_input[poa_input_index].len_pos) {
                poa_input[poa_input_index].best_x = std::stoi(line);
                read_index++; continue;
            } else if (read_index == pred_pos_begin + poa_input[poa_input_index].len_pos + 1) {
                poa_input[poa_input_index].best_y = std::stoi(line);
                read_index++; continue;
            } else if (read_index == pred_pos_begin + poa_input[poa_input_index].len_pos + 2) {
                poa_input[poa_input_index].best_score = std::stoi(line);
                read_index++; continue;
            } 
        }
    } else {
        fprintf(stderr, "Cannot open file %s.\n", poa_input_file.c_str());
        exit(-1);
    }
    fp_poa_input.close();
    printf("DRAM load %llu\n", *dram_load_size);
    printf("DRAM store %llu\n", *dram_store_size);
    free(dram_load_size);
    free(dram_store_size);

    fp_poa_compute_instruction.open(poa_compute_instruction_file, std::ios::in);
    if (fp_poa_compute_instruction.is_open()) {
        read_index = 0;
        while(getline(fp_poa_compute_instruction, line)) {
            poa_compute_instruction[read_index/2][read_index%2] = std::stol(line, 0, 16);
            read_index++;
        }
    } else {
        fprintf(stderr, "Cannot open file %s.\n", poa_compute_instruction_file.c_str());
        exit(-1);
    }
    fp_poa_compute_instruction.close();

    fp_poa_main_instruction.open(poa_main_instruction_file, std::ios::in);
    if (fp_poa_main_instruction.is_open()) {
        read_index = 0;
        while(getline(fp_poa_main_instruction, line)) {
            poa_main_instruction[read_index] = std::stol(line, 0, 16);
            read_index++;
        }
    } else {
        fprintf(stderr, "Cannot open file %s.\n", poa_main_instruction_file.c_str());
        exit(-1);
    }
    fp_poa_main_instruction.close();

    // for (i = 0; i < read_index; i++ ) {
    //     printf("%lx\n", poa_main_instruction[i]);
    // }

    for (i = 0; i < POA_PE_GROUP_SIZE; i++) {
        fp_poa_pe_instruction[i].open(poa_pe_instruction_file[i], std::ios::in);
        if (fp_poa_pe_instruction[i].is_open()) {
            read_index = 0;
            while(getline(fp_poa_pe_instruction[i], line)) {
                poa_pe_instruction[i][read_index/2][read_index%2] = std::stol(line, 0, 16);
                read_index++;
            }
        } else {
            fprintf(stderr, "Cannot open file %s.\n", poa_pe_instruction_file[i].c_str());
            exit(-1);
        }
        fp_poa_pe_instruction[i].close();
        // for (j = 0; j < read_index/2; j++ ) {
        //     printf("%lx %lx\n", poa_pe_instruction[i][j][0], poa_pe_instruction[i][j][1]);
        // }
    }

    // int i;
    // for (i = 0; i <= poa_input_index; i++ ) {
    //     poa_print_input(poa_input[i]);
    // }
    // for (i = 0; i <= POA_COMPUTE_INSTRUCTION_NUM; i++ ) {
    //     printf("%lx %lx\n", poa_compute_instruction[i][0], poa_compute_instruction[i][1]);
    // }
    

    return poa_input_index;
}

void poa_print_input(poa poa_input) {
    int j;
    printf(">\n");
    printf("%d\n", poa_input.len_y); 
    for (j = 0; j < poa_input.len_y; j++) printf("%d\n", poa_input.seq_y[j]);
    printf("%d\n", poa_input.len_x); 
    for (j = 0; j < poa_input.len_x; j++) printf("%d\n",poa_input.seq_x[j]);
    printf("%d\n", poa_input.len_x); 
    for (j = 0; j < poa_input.len_x; j++) printf("%d\n",poa_input.pred_num[j]);
    printf("%d\n", poa_input.len_pos);
    for (j = 0; j < poa_input.len_pos; j++) printf("%d\n", poa_input.pred_pos[j]);
    printf("%d\n", poa_input.best_x);
    printf("%d\n", poa_input.best_y);
    printf("%d\n", poa_input.best_score);
}

void poa_simulate(pe_array *pe_array_unit, poa poa_input, int n, FILE* fp, int show_output, int* output) {

    int i, j;
    int simd = 0;
    pe_array_unit->buffer_reset(pe_array_unit->input_buffer, 16384);
    pe_array_unit->buffer_reset(pe_array_unit->output_buffer, 400 * 1024 * 1024);
    pe_array_unit->main_PC = 0;
    for (i = 0; i < FIFO_GROUP_NUM; i++)
        for (j = 0; j < FIFO_GROUP_SIZE; j++)
            pe_array_unit->fifo_unit[i][j].clear();
    for (i = 0; i < POA_PE_GROUP_SIZE; i++) {
        pe_array_unit->pe_unit[i]->reset();
    }

    int init_score = POA_INIT_SCORE;
    int const_1 = POA_CONSTANT_1;
    int const_6 = POA_CONSTANT_6;
    int const_12 = POA_CONSTANT_12;
    int const_16 = POA_CONSTANT_16;
    int init_gap = POA_INIT_GAP;
    int min_score = POA_MIN_SCORE;

    // Load data from input file to input buffer
    pe_array_unit->input_buffer_write_from_ddr(0, &poa_input.len_y);
    pe_array_unit->input_buffer_write_from_ddr(1, &poa_input.len_x);
    pe_array_unit->input_buffer_write_from_ddr(2, &init_score);
    pe_array_unit->input_buffer_write_from_ddr(3, &const_1);
    pe_array_unit->input_buffer_write_from_ddr(4, &const_6);
    pe_array_unit->input_buffer_write_from_ddr(5, &const_12);
    pe_array_unit->input_buffer_write_from_ddr(6, &const_16);
    pe_array_unit->input_buffer_write_from_ddr(7, &init_gap);
    pe_array_unit->input_buffer_write_from_ddr(8, &min_score);
    for (i = 0; i < poa_input.len_y; i++) {
        pe_array_unit->input_buffer_write_from_ddr(i+9, &poa_input.seq_y[i]);
    }
    for (i = 0; i < poa_input.len_x; i++) {
        pe_array_unit->input_buffer_write_from_ddr(i+9+poa_input.len_y, &poa_input.seq_x[i]);
    }
    for (i = 0; i < poa_input.len_x; i++) {
        pe_array_unit->input_buffer_write_from_ddr(i+9+poa_input.len_y+poa_input.len_x, &poa_input.pred_num[i]);
    }
    for (i = 0; i < poa_input.len_pos; i++) {
        pe_array_unit->input_buffer_write_from_ddr(i+9+poa_input.len_y+2*poa_input.len_x, &poa_input.pred_pos[i]);
    }


    pe_array_unit->run(n, simd, PE_4_SETTING, MAIN_INSTRUCTION_1);
    // printf("len_y %d len_x %d\n", poa_input.len_y_unpadding, poa_input.len_x - 3);

    if (show_output) pe_array_unit->poa_show_output_buffer(poa_input.len_y, poa_input.len_x, fp);
}

void poa_simulation(char *inputFileName, char *outputFileName, FILE *fp, int show_output) {

    int i, j;
    pe_array *pe_array_unit = new pe_array(16384, 400 * 1024 * 1024);

    unsigned long poa_compute_instruction[POA_COMPUTE_INSTRUCTION_NUM][COMP_INSTR_BUFFER_GROUP_SIZE];
    unsigned long poa_main_instruction[CTRL_INSTR_BUFFER_NUM];
    unsigned long poa_pe_instruction[POA_PE_GROUP_SIZE][CTRL_INSTR_BUFFER_NUM][CTRL_INSTR_BUFFER_GROUP_SIZE];
    // poa_pe_instruction = (unsigned long **)malloc(POA_PE_GROUP_SIZE * sizeof(unsigned long *));
    for (i = 0; i < POA_PE_GROUP_SIZE; i++) {
        // poa_pe_instruction[i] = (unsigned long *)malloc(CTRL_INSTR_BUFFER_NUM * sizeof(unsigned long));
        for (j = 0; j < CTRL_INSTR_BUFFER_NUM; j++) {
            poa_pe_instruction[i][j][0] = 0x20f7800000000;
            poa_pe_instruction[i][j][1] = 0x20f7800000000;
        }
    }
    for (i = 0; i < CTRL_INSTR_BUFFER_NUM; i++) {
        poa_main_instruction[i] = -1;
    }
    poa poa_input[POA_MAX_INPUT];
    fprintf(stderr, "read input %s\n", inputFileName);
    int poa_input_index = poa_read_input(poa_input, inputFileName, poa_compute_instruction, poa_main_instruction, poa_pe_instruction);

    // Load data from input file to instruction buffer
    for (i = 0; i < POA_COMPUTE_INSTRUCTION_NUM; i++) {
        pe_array_unit->compute_instruction_buffer_write_from_ddr(i, poa_compute_instruction[i]);
    }

    // Load main & pe instructions into pe_array instruction buffer
    for (i = 0; i < CTRL_INSTR_BUFFER_NUM; i++) {
        unsigned long tmp[CTRL_INSTR_BUFFER_GROUP_SIZE];
        tmp[0] = 0xe;
        tmp[1] = poa_main_instruction[i];
        pe_array_unit->main_instruction_buffer_write_from_ddr(i, tmp);
        for (j = 0; j < POA_PE_GROUP_SIZE; j++)
            pe_array_unit->pe_instruction_buffer_write_from_ddr(i, poa_pe_instruction[j][i], j);
    }

	if (show_output) fp = fopen(outputFileName, "w");

    int max_node = 0, max_edge = 0;
    for (i = 0; i <= poa_input_index; i++ ) {
        if (poa_input[i].len_x > max_node) max_node = poa_input[i].len_x;
        if (poa_input[i].len_pos > max_edge) max_edge = poa_input[i].len_pos;
    }
    // printf("max_node %d max_edge %d\n", max_node, max_edge);

    int* poa_output;
    int index = 0;
    int total_len = 0;
    for (i = 0; i < poa_input_index; i++)
        total_len += poa_input[i].len_x * poa_input[i].len_y *2;
    poa_output = (int*)malloc(total_len * sizeof(int));


    // poa_simulate(pe_array_unit, poa_input[8], 60000, fp, show_output);
    for (i = 0; i < poa_input_index; i++) {
        poa_simulate(pe_array_unit, poa_input[i], 100000000, fp, show_output, poa_output+index);
        index += poa_input[i].len_x * poa_input[i].len_y *2;
    }
    
    if (show_output) fclose(fp);
    free(poa_output);

    for (i = 0; i <= poa_input_index; i++ ) {
        free(poa_input[i].seq_y);
        free(poa_input[i].seq_x);
        free(poa_input[i].pred_num);
        free(poa_input[i].pred_pos);
    }

    delete pe_array_unit;
}