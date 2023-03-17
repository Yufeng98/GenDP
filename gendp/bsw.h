#ifndef BSW_H
#define BSW_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define BSW_COMPUTE_INSTRUCTION_NUM 32
#define BSW_PE_GROUP_SIZE 4

#define MAX_SEQ_LEN_REF 128
#define MAX_SEQ_LEN_QER 128
#define DEFAULT_MATCH 1
#define DEFAULT_MISMATCH 4
#define DEFAULT_OPEN 6
#define DEFAULT_EXTEND 1
#define DEFAULT_AMBIG -1

#define BSW_MAX_INPUT 483065

typedef struct dnaSeqPair
{
    int64_t idr, idq, id;
    int32_t len1, len2;
    int32_t h0;
    int seqid, regid;
    int32_t score, tle, gtle, qle;
    int32_t gscore, max_off;

} SeqPair;

typedef struct bsw_input {
    unsigned int seqBufQer32[MAX_SEQ_LEN_QER], seqBufRef32[MAX_SEQ_LEN_REF], H_row32[MAX_SEQ_LEN_QER], H_col32[MAX_SEQ_LEN_REF];
    unsigned int qlen32, tlen32, mlen32, max_qlen, min_qlen, max_tlen, one32;
} bsw;

void loadPairs(char *pairFileName, FILE *pairFile, SeqPair *seqPairArray, int8_t *seqBufRef, int8_t* seqBufQer, int offset, int numPairs, long long *dram_load_size, long long *dram_store_size);

int bsw_read_pairs(char *inputFileName, FILE *pairFile);

void bsw_read_input(bsw* bsw_input, SeqPair* seqPairArray, int8_t* seqBufQer, int8_t* seqBufQerReorder, int8_t* seqBufRef, int8_t* seqBufRefReorder, int8_t* qlen, int8_t* tlen, int8_t* mlen, int8_t* H_row, int8_t* H_col, unsigned int* seqBufQer32, unsigned int* seqBufRef32, unsigned int* H_row32, unsigned int* H_col32, unsigned int* qlen32, unsigned int* tlen32, unsigned int* mlen32, unsigned int* max_qlen, unsigned int* min_qlen, unsigned int* max_tlen, char *inputFileName, FILE *pairFile, int roundNumPairs, int numPairs);

void bsw_simulate(pe_array *pe_array_unit, bsw bsw_input, int n, FILE* fp, int show_output, int* output);

void bsw_simulation(char *inputFileName, char *outputFileName, FILE *fp, int show_output);

unsigned int load_constant(int8_t constant);

#endif
