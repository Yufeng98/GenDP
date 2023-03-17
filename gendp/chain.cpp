#include "sys_def.h"
#include "pe_array.h"
#include "chain.h"

void skip_to_EOR(FILE *fp) {
    const char *loc = "EOR";
    while (*loc != '\0') {
        if (fgetc(fp) == *loc) {
            loc++;
        }
    }
}

call_t read_call(FILE *fp, long long* dram_load_size, long long* dram_store_size) {
    call_t call;

    long long n;
    float avg_qspan;
    int max_dist_x, max_dist_y, bw, n_segs;

    int t = fscanf(fp, "%lld%f%d%d%d%d",
            &n, &avg_qspan, &max_dist_x, &max_dist_y, &bw, &n_segs);
    // fprintf(stderr, "read %d arguments\n", t);
    *dram_load_size += 8 + 5*4;  // 1x8 Byte and 4x4 Byte inputs
    if (t != 6) {
        call.n = ANCHOR_NULL;
        call.avg_qspan = .0;
        return call;
    }

    call.n = n;
    call.avg_qspan = avg_qspan;
    call.max_dist_x = max_dist_x;
    call.max_dist_y = max_dist_y;
    call.bw = bw;
    call.n_segs = n_segs;
    // fprintf(stderr, "%lld\t%f\t%d\t%d\t%d\t%d\n", n, avg_qspan, max_dist_x, max_dist_y, bw, n_segs);

    call.anchors.resize(call.n);

    for (anchor_idx_t i = 0; i < call.n; i++) {
        uint64_t x, y;
        fscanf(fp, "%lu%lu", &x, &y);
        *dram_load_size += 4 + 8;   // 1x8 Byte and 1x4 Byte inputs
        *dram_store_size += 4;      // 1x4 Byte outputs

        anchor_t t;
        t.x = x; t.y = y;

        call.anchors[i] = t;
    }

    skip_to_EOR(fp);
    return call;
}


std::vector<call_t> chain_read_input(std::string chain_input_file, unsigned long chain_compute_instruction[][COMP_INSTR_BUFFER_GROUP_SIZE], unsigned long chain_main_instruction[][CTRL_INSTR_BUFFER_GROUP_SIZE], unsigned long chain_pe_instruction[][CTRL_INSTR_BUFFER_NUM][CTRL_INSTR_BUFFER_GROUP_SIZE]) {

    int i;
    std::string chain_compute_instruction_file = "../data/chain/compute_instruction.txt";
    std::string chain_main_instruction_file = "../data/chain/main_instruction.txt";
    std::string chain_pe_instruction_file[CHAIN_PE_GROUP_SIZE];
    for (i=0; i<CHAIN_PE_GROUP_SIZE; i++)
        chain_pe_instruction_file[i] = "../data/chain/pe_" + std::to_string(i) + "_instruction.txt";
    int read_index = 0;
    std::string line;
    std::fstream fp_chain_input, fp_chain_compute_instruction, fp_chain_main_instruction, fp_chain_pe_instruction[CHAIN_PE_GROUP_SIZE];
    // int index, move_begin = 0;
    FILE* in = fopen(chain_input_file.c_str(), "r");
    std::vector<call_t> calls;
    long long *dram_load_size, *dram_store_size;
    dram_load_size = (long long*)malloc(sizeof(long long));
    dram_store_size = (long long*)malloc(sizeof(long long));
    *dram_load_size = 0;
    *dram_store_size = 0;
    for (call_t call = read_call(in, dram_load_size, dram_store_size);
            call.n != ANCHOR_NULL;
            call = read_call(in, dram_load_size, dram_store_size)) {
        calls.push_back(call);
    }
    printf("DRAM load %llu\n", *dram_load_size);
    printf("DRAM store %llu\n", *dram_store_size);
    free(dram_load_size);
    free(dram_store_size);

    fclose(in);

    fp_chain_compute_instruction.open(chain_compute_instruction_file, std::ios::in);
    if (fp_chain_compute_instruction.is_open()) {
        read_index = 0;
        while(getline(fp_chain_compute_instruction, line)) {
            chain_compute_instruction[read_index/2][read_index%2] = std::stol(line, 0, 16);
            read_index++;
        }
    } else {
        fprintf(stderr, "Cannot open file %s.\n", chain_compute_instruction_file.c_str());
        exit(-1);
    }
    fp_chain_compute_instruction.close();

    fp_chain_main_instruction.open(chain_main_instruction_file, std::ios::in);
    if (fp_chain_main_instruction.is_open()) {
        read_index = 0;
        while(getline(fp_chain_main_instruction, line)) {
            chain_main_instruction[read_index/2][read_index%2] = std::stol(line, 0, 16);
            read_index++;
        }
    } else {
        fprintf(stderr, "Cannot open file %s.\n", chain_main_instruction_file.c_str());
        exit(-1);
    }
    fp_chain_main_instruction.close();

    for (i = 0; i < CHAIN_PE_GROUP_SIZE; i++) {
        fp_chain_pe_instruction[i].open(chain_pe_instruction_file[i], std::ios::in);
        if (fp_chain_pe_instruction[i].is_open()) {
            read_index = 0;
            while(getline(fp_chain_pe_instruction[i], line)) {
                chain_pe_instruction[i][read_index/2][read_index%2] = std::stol(line, 0, 16);
                read_index++;
            }
        } else {
            fprintf(stderr, "Cannot open file %s.\n", chain_pe_instruction_file[i].c_str());
            exit(-1);
        }
        fp_chain_pe_instruction[i].close();
    }

    return calls;
}

void chain_simulate(pe_array *pe_array_unit, call_t_array* chain_input, int n, FILE* fp, int show_output, int* output) {

    int i;
    int simd = 0;
    pe_array_unit->buffer_reset(pe_array_unit->input_buffer, 20480 * 100);
    pe_array_unit->buffer_reset(pe_array_unit->output_buffer, 20480 * 100);
    pe_array_unit->main_PC = 0;
    for (i = 0; i < CHAIN_PE_GROUP_SIZE; i++) pe_array_unit->pe_unit[i]->reset();
    for (i = 0; i < FIFO_GROUP_SIZE; i++) pe_array_unit->fifo_unit[0][i].clear();

    // Load data from input file to input buffer
    int chain_n = chain_input->n;
    int const_1 = CHAIN_CONSTANT_1;
    int const_16 = CHAIN_CONSTANT_16;
    int const_min = CHAIN_CONSTANT_MIN;
    pe_array_unit->input_buffer_write_from_ddr(0, &chain_n);
    pe_array_unit->input_buffer_write_from_ddr(1, &const_1);
    pe_array_unit->input_buffer_write_from_ddr(2, &const_16);
    pe_array_unit->input_buffer_write_from_ddr(3, &const_min);
    pe_array_unit->input_buffer_write_from_ddr(4, &chain_input->max_dist_y);
    pe_array_unit->input_buffer_write_from_ddr(5, &chain_input->bw);
    int avg_qspan_tmp = round(chain_input->avg_qspan * 0.01 * (1<<16));		// constant
	unsigned int fractional_mask = (1 << 16) -1;	
	unsigned int integer_mask    = ~fractional_mask;
    unsigned int tmp_0 = integer_mask & avg_qspan_tmp;
    unsigned int tmp_1 = fractional_mask & avg_qspan_tmp;
    pe_array_unit->input_buffer_write_from_ddr_unsigned(6, &tmp_0);
    pe_array_unit->input_buffer_write_from_ddr_unsigned(7, &tmp_1);
    for (i = 0; i < chain_input->n; i++) {
        int32_t tmp_2 = (int32_t)chain_input->anchors[i].x;
        int32_t tmp_3 = (int32_t)chain_input->anchors[i].y;
        pe_array_unit->input_buffer_write_from_ddr(i*3+8, &tmp_2);
        pe_array_unit->input_buffer_write_from_ddr(i*3+1+8, &tmp_3);
        int32_t q_span = (int32_t)(chain_input->anchors[i].y>>32 & 0xff);
        pe_array_unit->input_buffer_write_from_ddr(i*3+2+8, &q_span);
    }

    // printf("%d ", chain_input->n);
    pe_array_unit->run(n, simd, PE_64_SETTING, MAIN_INSTRUCTION_2);

    if (show_output) pe_array_unit->chain_show_output_buffer(chain_input->n, fp);

}

void chain_simulation(char *inputFileName, char *outputFileName, FILE *fp, int show_output) {

    int i, j;
    pe_array *pe_array_unit = new pe_array(20480 * 100, 20480 * 100);

    unsigned long chain_compute_instruction[CHAIN_COMPUTE_INSTRUCTION_NUM][COMP_INSTR_BUFFER_GROUP_SIZE];
    unsigned long chain_main_instruction[CTRL_INSTR_BUFFER_NUM][CTRL_INSTR_BUFFER_GROUP_SIZE];
    unsigned long chain_pe_instruction[CHAIN_PE_GROUP_SIZE][CTRL_INSTR_BUFFER_NUM][CTRL_INSTR_BUFFER_GROUP_SIZE];
    for (i = 0; i < CHAIN_COMPUTE_INSTRUCTION_NUM; i++) {
        chain_compute_instruction[i][0] = 0x20f7800000000;
        chain_compute_instruction[i][1] = 0x20f7800000000;
    }
    for (i = 0; i < CHAIN_PE_GROUP_SIZE; i++) {
        for (j = 0; j < CTRL_INSTR_BUFFER_NUM; j++) {
            chain_pe_instruction[i][j][0] = 0xf;
            chain_pe_instruction[i][j][1] = 0xf;
        }
    }
    for (i = 0; i < CTRL_INSTR_BUFFER_NUM; i++) {
        chain_main_instruction[i][0] = 0xf;
        chain_main_instruction[i][1] = 0xf;
    }


    fprintf(stderr, "read input %s\n", inputFileName);
    std::vector<call_t> calls = chain_read_input(inputFileName, chain_compute_instruction, chain_main_instruction, chain_pe_instruction);
    call_t_array *calls_array;
    calls_array = (call_t_array*)malloc((int)calls.size()*sizeof(call_t_array));
    for (i = 0; i < (int)calls.size(); i++) {
        calls_array[i].n = calls[i].n;
        calls_array[i].avg_qspan = calls[i].avg_qspan;
        calls_array[i].max_dist_x = calls[i].max_dist_x;
        calls_array[i].bw = calls[i].bw;
        calls_array[i].n_segs = calls[i].n_segs;
        calls_array[i].anchors = (anchor_t*)malloc(calls_array[i].n * sizeof(anchor_t));
        for (j = 0; j < (int)calls[i].anchors.size(); j++){
            calls_array[i].anchors[j].x = calls[i].anchors[j].x;
            calls_array[i].anchors[j].y = calls[i].anchors[j].y;
            calls_array[i].q_span = calls[i].anchors[j].y>>32 & 0xff;
        }
    }


    // Load data from input file to instruction buffer
    for (i = 0; i < CHAIN_COMPUTE_INSTRUCTION_NUM; i++) {
        pe_array_unit->compute_instruction_buffer_write_from_ddr(i, chain_compute_instruction[i]);
    }

    // Load main & pe instructions into pe_array instruction buffer
    for (i = 0; i < CTRL_INSTR_BUFFER_NUM; i++) {
        pe_array_unit->main_instruction_buffer_write_from_ddr(i, chain_main_instruction[i]);
        for (j = 0; j < CHAIN_PE_GROUP_SIZE; j++)
            pe_array_unit->pe_instruction_buffer_write_from_ddr(i, chain_pe_instruction[j][i], j);
    }

	if (show_output) fp = fopen(outputFileName, "w");

    int* chain_output;
    int index = 0;
    int total_len = 0;
    for (i = 0; i < (int)calls.size(); i++)
        total_len += calls[i].n;
    chain_output = (int*)malloc(total_len * sizeof(int));

    // chain_simulate(pe_array_unit, calls[0], 1000000, fp, show_output);
    for (i = 0; i < (int)calls.size(); i++) {
    // for (i = 0; i < 3; i++)
        chain_simulate(pe_array_unit, calls_array+i, 10000000, fp, show_output, chain_output + index);
        index += calls[i].n;
    }
    
    if (show_output) fclose(fp);
    free(chain_output);


    for (i = 0; i < (int)calls.size(); i++)
        free(calls_array[i].anchors);
    free(calls_array);
    delete pe_array_unit;
}