#include "sys_def.h"
#include "pe_array.h"
#include "bsw.h"

void loadPairs(char *pairFileName, FILE *pairFile, SeqPair *seqPairArray, int8_t *seqBufRef, int8_t* seqBufQer, int seq_offset, int numPairs, long long *dram_load_size, long long *dram_store_size)
{
	int numPairsRead = 0;
	while (numPairsRead < numPairs) {
		int32_t h0 = 0;
		char temp[10];
		fgets(temp, 10, pairFile);
		sscanf(temp, "%d", &h0);
        *dram_load_size += 1*4;
        *dram_store_size += 6*4;    // 6 4-Byte outputs
		if (!fgets((char *)(seqBufRef + seq_offset + numPairsRead * (int64_t)(MAX_SEQ_LEN_REF)), MAX_SEQ_LEN_REF + 1, pairFile)) {
			printf("WARNING! fgets returned NULL in %s. Num Pairs : %d\n", pairFileName, numPairsRead);
			break;
        }
		if (!fgets((char *)(seqBufQer + seq_offset + numPairsRead * (int64_t)(MAX_SEQ_LEN_QER)), MAX_SEQ_LEN_QER + 1, pairFile)) {
			printf("WARNING! Odd number of sequences in %s\n", pairFileName);
			break;
        }
		SeqPair sp;
		sp.id = numPairsRead;
		sp.len1 = strnlen((char *)(seqBufRef + seq_offset + numPairsRead * MAX_SEQ_LEN_REF), MAX_SEQ_LEN_REF) - 1;
		sp.len2 = strnlen((char *)(seqBufQer + seq_offset + numPairsRead * MAX_SEQ_LEN_QER), MAX_SEQ_LEN_QER) - 1;
        *dram_load_size += round(sp.len1 * 0.25);   // Each character requires 2-bit
        *dram_load_size += round(sp.len2 * 0.25);
        if (sp.len1 <= 0 || sp.len2 <= 0) {
            fprintf(stderr, "ref %s", seqBufRef + seq_offset + numPairsRead * MAX_SEQ_LEN_REF);
            fprintf(stderr, "qeury %s", seqBufQer + seq_offset + numPairsRead * MAX_SEQ_LEN_QER);
            fprintf(stderr, "%d %d %d %d\n", numPairsRead, sp.len1, sp.len2, 3*(seq_offset/128+numPairsRead));
            exit(-1);
        }
        assert(sp.len1 > 0);
        assert(sp.len2 > 0);
		sp.h0 = h0;
		int8_t *seq1 = seqBufRef + seq_offset + numPairsRead * MAX_SEQ_LEN_REF;
		int8_t *seq2 = seqBufQer + seq_offset + numPairsRead * MAX_SEQ_LEN_QER;
		sp.idr =  numPairsRead * MAX_SEQ_LEN_REF;
		sp.idq =  numPairsRead * MAX_SEQ_LEN_QER;
		for (int l = 0; l < sp.len1; l++) {
			seq1[l] -= 48;
        }
		for (int l = 0; l < sp.len2; l++) {
			seq2[l] -= 48;
        }
		sp.seqid = sp.regid = sp.score = sp.tle = sp.gtle = sp.qle = -1;
		sp.gscore = sp.max_off = -1;
		seqPairArray[numPairsRead] = sp;
		numPairsRead++;
		// SW_cells += (sp.len1 * sp.len2);
	}
}

int bsw_read_pairs(char *inputFileName, FILE *pairFile) {

    if (pairFile == NULL) {
        fprintf(stderr, "Could not open file: %s\n", inputFileName);
	    exit(-1);
    }

    const int bufSize = 1024 * 1024;
    char* buffer = (char*)malloc(bufSize * sizeof(char));
    int numLines = 0;
    int n;
    n = fread(buffer, sizeof(char), bufSize, pairFile);
    while (n) {
        for (int i = 0; i < n; i++) {
            if (buffer[i] == '\n') {
                numLines++;
            }
        }
        n = fread(buffer, sizeof(char), bufSize, pairFile);
    }
    free(buffer);

    fseek(pairFile, 0L, SEEK_SET);

    int numPairs = numLines / 3;
    return numPairs;
}

void bsw_read_input(bsw* bsw_input, SeqPair* seqPairArray, int8_t* seqBufQer, int8_t* seqBufQerReorder, int8_t* seqBufRef, int8_t* seqBufRefReorder, int8_t* qlen, int8_t* tlen, int8_t* mlen, int8_t* H_row, int8_t* H_col, unsigned int* seqBufQer32, unsigned int* seqBufRef32, unsigned int* H_row32, unsigned int* H_col32, unsigned int* qlen32, unsigned int* tlen32, unsigned int* mlen32, unsigned int* max_qlen, unsigned int* min_qlen, unsigned int* max_tlen, char *inputFileName, FILE *pairFile, int roundNumPairs, int numPairs) {

    int i, j, k, batchSize = 512, maxq, minq, maxt;
    int oe_ins = DEFAULT_OPEN + DEFAULT_EXTEND, e_ins = DEFAULT_EXTEND;
    int numPairsIndex = 0;

    long long *dram_load_size, *dram_store_size;
    dram_load_size = (long long*)malloc(sizeof(long long));
    dram_store_size = (long long*)malloc(sizeof(long long));
    *dram_load_size = 0;
    *dram_store_size = 0;

    for (i = 0; i < roundNumPairs; i += batchSize) {
        int nPairsBatch = (numPairs - i) >= batchSize ? batchSize : numPairs - i;
        loadPairs(inputFileName, pairFile, seqPairArray + numPairsIndex, seqBufRef, seqBufQer, numPairsIndex * MAX_SEQ_LEN_QER, nPairsBatch, dram_load_size, dram_store_size);
        numPairsIndex += nPairsBatch;
    }

    printf("DRAM load %llu\n", *dram_load_size);
    printf("DRAM store %llu\n", *dram_store_size);
    free(dram_load_size);
    free(dram_store_size);

    for (i = 0; i < roundNumPairs/SIMD_WIDTH8; i++) {
        maxq = 0; minq = 127; maxt = 0;
        for (j = 0; j < SIMD_WIDTH8; j++) {
            tlen[i * SIMD_WIDTH8 + j] = seqPairArray[i * SIMD_WIDTH8 + j].len1;
            qlen[i * SIMD_WIDTH8 + j] = seqPairArray[i * SIMD_WIDTH8 + j].len2;
            if (2*qlen[i * SIMD_WIDTH8 + j] > tlen[i * SIMD_WIDTH8 + j]) mlen[i * SIMD_WIDTH8 + j] = tlen[i * SIMD_WIDTH8 + j];
            else mlen[i * SIMD_WIDTH8 + j] = 2*qlen[i * SIMD_WIDTH8 + j];    
            if (seqPairArray[i * SIMD_WIDTH8 + j].len1 > maxt) maxt = seqPairArray[i * SIMD_WIDTH8 + j].len1;
            if (seqPairArray[i * SIMD_WIDTH8 + j].len2 > maxq) maxq = seqPairArray[i * SIMD_WIDTH8 + j].len2;
            if (seqPairArray[i * SIMD_WIDTH8 + j].len2 < minq) minq = seqPairArray[i * SIMD_WIDTH8 + j].len2;
        }
        max_qlen[i] = maxq; max_tlen[i] = maxt; min_qlen[i] = minq;
    }

    for (i = 0; i < roundNumPairs/SIMD_WIDTH8; i++) {

        for (j = 0; j < SIMD_WIDTH8; j++) {

            for (k = 0; k < (int)max_tlen[i]; k++) {
                if (k < tlen[i * SIMD_WIDTH8 + j])
                    seqBufRefReorder[i * MAX_SEQ_LEN_REF*SIMD_WIDTH8 + k*SIMD_WIDTH8 + j] = seqBufRef[i * MAX_SEQ_LEN_REF*SIMD_WIDTH8 + j*MAX_SEQ_LEN_REF + k];
                else seqBufRefReorder[i * MAX_SEQ_LEN_REF*SIMD_WIDTH8 + k*SIMD_WIDTH8 + j] = 4;
                if (k == 0) H_col[i * MAX_SEQ_LEN_REF*SIMD_WIDTH8 + j] = seqPairArray[i * SIMD_WIDTH8 + j].h0;
                else if (k == 1) H_col[i * MAX_SEQ_LEN_REF*SIMD_WIDTH8 + k*SIMD_WIDTH8 + j] = seqPairArray[i * SIMD_WIDTH8 + j].h0 > oe_ins? seqPairArray[i * SIMD_WIDTH8 + j].h0-oe_ins : 0;
                else H_col[i * MAX_SEQ_LEN_REF*SIMD_WIDTH8 + k*SIMD_WIDTH8 + j] = seqPairArray[i * SIMD_WIDTH8 + j].h0 > oe_ins+e_ins*(k-1)? seqPairArray[i * SIMD_WIDTH8 + j].h0-oe_ins-e_ins*(k-1) : 0;
            }

            for (k = 0; k < (int)max_qlen[i]; k++) {
                if (k < qlen[i * SIMD_WIDTH8 + j])
                    seqBufQerReorder[i * MAX_SEQ_LEN_QER*SIMD_WIDTH8 + k*SIMD_WIDTH8 + j] = seqBufQer[i * MAX_SEQ_LEN_QER*SIMD_WIDTH8 + j*MAX_SEQ_LEN_QER + k];
                else seqBufQerReorder[i * MAX_SEQ_LEN_QER*SIMD_WIDTH8 + k*SIMD_WIDTH8 + j] = 4;
                if (k == 0) H_row[i * MAX_SEQ_LEN_QER*SIMD_WIDTH8 + j] = seqPairArray[i * SIMD_WIDTH8 + j].h0;
                else if (k == 1) H_row[i * MAX_SEQ_LEN_QER*SIMD_WIDTH8 + k*SIMD_WIDTH8 + j] = seqPairArray[i * SIMD_WIDTH8 + j].h0 > oe_ins? seqPairArray[i * SIMD_WIDTH8 + j].h0-oe_ins : 0;
                else H_row[i * MAX_SEQ_LEN_QER*SIMD_WIDTH8 + k*SIMD_WIDTH8 + j] = seqPairArray[i * SIMD_WIDTH8 + j].h0 > oe_ins+e_ins*(k-1)? seqPairArray[i * SIMD_WIDTH8 + j].h0-oe_ins-e_ins*(k-1) : 0;
            }

        }

    }
    if (MAX_SEQ_LEN_QER * roundNumPairs * sizeof(int8_t) != MAX_SEQ_LEN_QER * roundNumPairs/SIMD_WIDTH8 * sizeof(unsigned int))
        printf("seqBufQer scalar and simd size %ld %ld\n", MAX_SEQ_LEN_QER * roundNumPairs * sizeof(int8_t), MAX_SEQ_LEN_QER * roundNumPairs/SIMD_WIDTH8 * sizeof(unsigned int));
    if (roundNumPairs * sizeof(int8_t) != roundNumPairs/SIMD_WIDTH8 * sizeof(unsigned int))
        printf("qlen32 scalar and simd size %ld %ld\n", roundNumPairs * sizeof(int8_t), roundNumPairs/SIMD_WIDTH8 * sizeof(unsigned int));
    memcpy(seqBufQer32, seqBufQerReorder, MAX_SEQ_LEN_QER * roundNumPairs * sizeof(int8_t));
    memcpy(seqBufRef32, seqBufRefReorder, MAX_SEQ_LEN_QER * roundNumPairs * sizeof(int8_t));
    memcpy(H_row32, H_row, MAX_SEQ_LEN_QER * roundNumPairs * sizeof(int8_t));
    memcpy(H_col32, H_col, MAX_SEQ_LEN_QER * roundNumPairs * sizeof(int8_t));
    memcpy(qlen32, qlen, roundNumPairs * sizeof(int8_t));
    memcpy(tlen32, tlen, roundNumPairs * sizeof(int8_t));
    memcpy(mlen32, mlen, roundNumPairs * sizeof(int8_t));

    // printf("%lx %d %d %d %d\n", H_row32[0], H_row[0], H_row[1], H_row[2], H_row[3]);

    for (i = 0; i < roundNumPairs/SIMD_WIDTH8; i++) {
        bsw_input[i].max_qlen = max_qlen[i];
        bsw_input[i].max_tlen = max_tlen[i];
        bsw_input[i].min_qlen = min_qlen[i];
        bsw_input[i].qlen32 = qlen32[i];
        bsw_input[i].tlen32 = tlen32[i];
        bsw_input[i].mlen32 = mlen32[i];
        for (j = 0; j < MAX_SEQ_LEN_QER; j++) {
            bsw_input[i].seqBufQer32[j] = seqBufQer32[i*MAX_SEQ_LEN_QER + j];
            bsw_input[i].seqBufRef32[j] = seqBufRef32[i*MAX_SEQ_LEN_QER + j];
            bsw_input[i].H_row32[j] = H_row32[i*MAX_SEQ_LEN_QER + j];
            bsw_input[i].H_col32[j] = H_col32[i*MAX_SEQ_LEN_QER + j];
        }
    }
}

void bsw_simulate(pe_array *pe_array_unit, bsw* bsw_input, int n, FILE* fp, int show_output, int* output) {

    int i, j;
    int simd = 1;
    pe_array_unit->buffer_reset(pe_array_unit->main_addressing_register, MAIN_ADDR_REGISTER_NUM);
    pe_array_unit->buffer_reset(pe_array_unit->input_buffer, pe_array_unit->input_buffer_size);
    pe_array_unit->buffer_reset(pe_array_unit->output_buffer, pe_array_unit->output_buffer_size);

    pe_array_unit->main_PC = 0;
    for (i = 0; i < FIFO_GROUP_NUM; i++)
        for (j = 0; j < FIFO_GROUP_SIZE; j++)
            pe_array_unit->fifo_unit[i][j].clear();
    for (i = 0; i < BSW_PE_GROUP_SIZE; i++) {
        pe_array_unit->pe_unit[i]->reset();
    }

    // Load data from input file to input buffer
    unsigned int const_64 = load_constant(64);
    unsigned int const_minus_6 = load_constant(-6);
    unsigned int const_minus_1 = load_constant(-1);
    unsigned int const_1 = load_constant(1);
    unsigned int const_4 = load_constant(4);
    pe_array_unit->input_buffer_write_from_ddr_unsigned(0, &bsw_input->max_tlen);
    pe_array_unit->input_buffer_write_from_ddr_unsigned(1, &bsw_input->max_qlen);
    pe_array_unit->input_buffer_write_from_ddr_unsigned(2, &bsw_input->min_qlen);
    pe_array_unit->input_buffer_write_from_ddr_unsigned(3, &bsw_input->qlen32);
    pe_array_unit->input_buffer_write_from_ddr_unsigned(4, &bsw_input->tlen32);
    pe_array_unit->input_buffer_write_from_ddr_unsigned(5, &bsw_input->H_row32[0]);
    pe_array_unit->input_buffer_write_from_ddr_unsigned(6, &const_64);
    pe_array_unit->input_buffer_write_from_ddr_unsigned(7, &const_minus_6);
    pe_array_unit->input_buffer_write_from_ddr_unsigned(8, &const_minus_1);
    pe_array_unit->input_buffer_write_from_ddr_unsigned(9, &const_1);
    pe_array_unit->input_buffer_write_from_ddr_unsigned(10, &const_4);
    for (i = 0; i < (int)bsw_input->max_tlen; i++) {
        pe_array_unit->input_buffer_write_from_ddr_unsigned(i+11, &bsw_input->seqBufRef32[i]);
    }
    for (i = 0; i < (int)bsw_input->max_tlen; i++) 
        pe_array_unit->input_buffer_write_from_ddr_unsigned(i+11+bsw_input->max_tlen, &bsw_input->H_col32[i]);
    for (i = 0; i < (int)bsw_input->max_qlen; i++) {
        pe_array_unit->input_buffer_write_from_ddr_unsigned(i+11+bsw_input->max_tlen*2, &bsw_input->seqBufQer32[i]);
    }
    for (i = 0; i < (int)bsw_input->max_qlen; i++)
        pe_array_unit->input_buffer_write_from_ddr_unsigned(i+11+bsw_input->max_tlen*2+bsw_input->max_qlen, &bsw_input->H_row32[i]);

    pe_array_unit->run(n, simd, PE_4_SETTING, MAIN_INSTRUCTION_1);

    if (show_output) pe_array_unit->bsw_show_output_buffer(fp);

}

void bsw_simulation(char *inputFileName, char *outputFileName, FILE *fp, int show_output) {

    pe_array *pe_array_unit = new pe_array(1024, 1024);

    fprintf(stderr, "read input %s\n", inputFileName);
    FILE *pairFile = fopen(inputFileName, "r");
    int i, j, numPairs = bsw_read_pairs(inputFileName, pairFile);
    int roundNumPairs = ((numPairs + SIMD_WIDTH8 - 1) / SIMD_WIDTH8 ) * SIMD_WIDTH8;

    SeqPair* seqPairArray = (SeqPair *) malloc (roundNumPairs * sizeof(SeqPair));
    int8_t* seqBufQer = (int8_t*) malloc (MAX_SEQ_LEN_QER * roundNumPairs * sizeof(int8_t));
    int8_t* seqBufQerReorder = (int8_t*) malloc (MAX_SEQ_LEN_QER * roundNumPairs * sizeof(int8_t));
    int8_t* seqBufRef = (int8_t*) malloc (MAX_SEQ_LEN_REF * roundNumPairs * sizeof(int8_t));
    int8_t* seqBufRefReorder = (int8_t*) malloc (MAX_SEQ_LEN_REF * roundNumPairs * sizeof(int8_t));
    int8_t* qlen = (int8_t*) malloc (roundNumPairs * sizeof(int8_t));
    int8_t* tlen = (int8_t*) malloc (roundNumPairs * sizeof(int8_t));
    int8_t* mlen = (int8_t*) malloc (roundNumPairs * sizeof(int8_t));
    int8_t* H_row = (int8_t*) malloc (MAX_SEQ_LEN_QER * roundNumPairs * sizeof(int8_t));
    int8_t* H_col = (int8_t*) malloc (MAX_SEQ_LEN_REF * roundNumPairs * sizeof(int8_t));

    for (i = 0; i < MAX_SEQ_LEN_QER * roundNumPairs; i++) {
        seqBufQerReorder[i] = 4;
        seqBufRefReorder[i] = 4;
        H_row[i] = 0;
        H_col[i] = 0;
    }
    
    unsigned int* seqBufQer32 = (unsigned int*) malloc (MAX_SEQ_LEN_QER * roundNumPairs/SIMD_WIDTH8 * sizeof(unsigned int));
    unsigned int* seqBufRef32 = (unsigned int*) malloc (MAX_SEQ_LEN_REF * roundNumPairs/SIMD_WIDTH8 * sizeof(unsigned int));
    unsigned int* H_row32 = (unsigned int*) malloc (MAX_SEQ_LEN_REF * roundNumPairs/SIMD_WIDTH8 * sizeof(unsigned int));
    unsigned int* H_col32 = (unsigned int*) malloc (MAX_SEQ_LEN_REF * roundNumPairs/SIMD_WIDTH8 * sizeof(unsigned int));
    unsigned int* qlen32 = (unsigned int*) malloc (roundNumPairs/SIMD_WIDTH8 * sizeof(unsigned int));
    unsigned int* tlen32 = (unsigned int*) malloc (roundNumPairs/SIMD_WIDTH8 * sizeof(unsigned int));
    unsigned int* mlen32 = (unsigned int*) malloc (roundNumPairs/SIMD_WIDTH8 * sizeof(unsigned int));    
    unsigned int* max_qlen = (unsigned int*) malloc (roundNumPairs/SIMD_WIDTH8 * sizeof(unsigned int));
    unsigned int* min_qlen = (unsigned int*) malloc (roundNumPairs/SIMD_WIDTH8 * sizeof(unsigned int));
    unsigned int* max_tlen = (unsigned int*) malloc (roundNumPairs/SIMD_WIDTH8 * sizeof(unsigned int));
    
    bsw* bsw_input = (bsw*) malloc (roundNumPairs/SIMD_WIDTH8 * sizeof(bsw));

    bsw_read_input(bsw_input, seqPairArray, seqBufQer, seqBufQerReorder, seqBufRef, seqBufRefReorder, qlen, tlen, mlen, H_row, H_col, seqBufQer32, seqBufRef32, H_row32, H_col32, qlen32, tlen32, mlen32, max_qlen, min_qlen, max_tlen, inputFileName, pairFile, roundNumPairs, numPairs);

    fclose(pairFile);

    unsigned long bsw_compute_instruction[BSW_COMPUTE_INSTRUCTION_NUM][COMP_INSTR_BUFFER_GROUP_SIZE];
    unsigned long bsw_main_instruction[CTRL_INSTR_BUFFER_NUM];
    unsigned long bsw_pe_instruction[BSW_PE_GROUP_SIZE][CTRL_INSTR_BUFFER_NUM][CTRL_INSTR_BUFFER_GROUP_SIZE];
    for (i = 0; i < BSW_PE_GROUP_SIZE; i++) {
        for (j = 0; j < CTRL_INSTR_BUFFER_NUM; j++) {
            bsw_pe_instruction[i][j][0] = 0x20f7800000000;
            bsw_pe_instruction[i][j][1] = 0x20f7800000000;
        }
    }
    for (i = 0; i < CTRL_INSTR_BUFFER_NUM; i++) {
        bsw_main_instruction[i] = -1;
    }

    std::string bsw_compute_instruction_file = "../data/bsw/compute_instruction.txt";
    std::string bsw_main_instruction_file = "../data/bsw/main_instruction.txt";
    std::string bsw_pe_instruction_file[BSW_PE_GROUP_SIZE];
    bsw_pe_instruction_file[0] = "../data/bsw/pe_0_instruction.txt";
    bsw_pe_instruction_file[1] = "../data/bsw/pe_1_instruction.txt";
    bsw_pe_instruction_file[2] = "../data/bsw/pe_2_instruction.txt";
    bsw_pe_instruction_file[3] = "../data/bsw/pe_3_instruction.txt";
    std::fstream fp_bsw_input, fp_bsw_compute_instruction, fp_bsw_main_instruction, fp_bsw_pe_instruction[BSW_PE_GROUP_SIZE];
    std::string line;
    int read_index = 0;

    fp_bsw_compute_instruction.open(bsw_compute_instruction_file, std::ios::in);
    if (fp_bsw_compute_instruction.is_open()) {
        read_index = 0;
        while(getline(fp_bsw_compute_instruction, line)) {
            bsw_compute_instruction[read_index/2][read_index%2] = std::stol(line, 0, 16);
            read_index++;
        }
    } else {
        fprintf(stderr, "Cannot open file %s.\n", bsw_compute_instruction_file.c_str());
        exit(-1);
    }
    fp_bsw_compute_instruction.close();

    fp_bsw_main_instruction.open(bsw_main_instruction_file, std::ios::in);
    if (fp_bsw_main_instruction.is_open()) {
        read_index = 0;
        while(getline(fp_bsw_main_instruction, line)) {
            bsw_main_instruction[read_index] = std::stol(line, 0, 16);
            read_index++;
        }
    } else {
        fprintf(stderr, "Cannot open file %s.\n", bsw_main_instruction_file.c_str());
        exit(-1);
    }
    fp_bsw_main_instruction.close();

    for (i = 0; i < BSW_PE_GROUP_SIZE; i++) {
        fp_bsw_pe_instruction[i].open(bsw_pe_instruction_file[i], std::ios::in);
        if (fp_bsw_pe_instruction[i].is_open()) {
            read_index = 0;
            while(getline(fp_bsw_pe_instruction[i], line)) {
                bsw_pe_instruction[i][read_index/2][read_index%2] = std::stol(line, 0, 16);
                read_index++;
            }
        } else {
            fprintf(stderr, "Cannot open file %s.\n", bsw_pe_instruction_file[i].c_str());
            exit(-1);
        }
        fp_bsw_pe_instruction[i].close();
    }

    // Load data from input file to instruction buffer
    for (i = 0; i < BSW_COMPUTE_INSTRUCTION_NUM; i++) {
        pe_array_unit->compute_instruction_buffer_write_from_ddr(i, bsw_compute_instruction[i]);
    }

    // Load main & pe instructions into pe_array instruction buffer
    for (i = 0; i < CTRL_INSTR_BUFFER_NUM; i++) {
        unsigned long tmp[CTRL_INSTR_BUFFER_GROUP_SIZE];
        tmp[0] = 0xe;
        tmp[1] = bsw_main_instruction[i];
        pe_array_unit->main_instruction_buffer_write_from_ddr(i, tmp);
        for (j = 0; j < BSW_PE_GROUP_SIZE; j++)
            pe_array_unit->pe_instruction_buffer_write_from_ddr(i, bsw_pe_instruction[j][i], j);
    }

	if (show_output) fp = fopen(outputFileName, "w");

    int* bsw_output;
    int index = 0;
    bsw_output = (int*)malloc(6*roundNumPairs/SIMD_WIDTH8*sizeof(int));

    for (i = 0; i < roundNumPairs/SIMD_WIDTH8; i++) {
    // for (i = 0; i < 10; i++)
        bsw_simulate(pe_array_unit, bsw_input+i, 100000, fp, show_output, bsw_output + index);
        index += 6;
    }

    if (show_output) fclose(fp);
    free(bsw_output);

    free(bsw_input);
    free(seqPairArray); free(seqBufQer); free(seqBufQerReorder); free(seqBufRef); free(seqBufRefReorder); 
    free(qlen); free(tlen); free(mlen); free(H_row); free(H_col);
    free(seqBufQer32); free(seqBufRef32); free(H_row32); free(H_col32); free(qlen32); free(tlen32); free(mlen32);
    free(max_qlen); free(min_qlen); free(max_tlen);

    delete pe_array_unit;
}


unsigned int load_constant(int8_t constant) {
    int8_t* constant_array = (int8_t*) malloc (4 * sizeof(int8_t));
    int i;
    for (i = 0; i < 4; i++)
        constant_array[i] = constant;
    unsigned int constant32;
    memcpy(&constant32, constant_array, 4 * sizeof(int8_t));
    free(constant_array);
    return constant32;
}
