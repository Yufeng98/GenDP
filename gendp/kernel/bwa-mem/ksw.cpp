/* The MIT License

   Copyright (c) 2011 by Attractive Chaos <attractor@live.co.uk>

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#include <ctype.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <emmintrin.h>
#include <sys/time.h>
#include <getopt.h>
#include "omp.h"
#include "ksw.h"
#include "compute_unit_32.h"
#include "comp_decoder.h"

#if RAPL
#include "Rapl.h"
#endif

#define CLMUL 8

#undef MAX_SEQ_LEN_REF
#define MAX_SEQ_LEN_REF 2048
#undef MAX_SEQ_LEN_QER
#define MAX_SEQ_LEN_QER 256

unsigned char seq_nt4_table[256] = {
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4, 
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4, 
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4, 
	4, 0, 4, 1,  4, 4, 4, 2,  4, 4, 4, 4,  4, 4, 4, 4, 
	4, 4, 4, 4,  3, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4, 
	4, 0, 4, 1,  4, 4, 4, 2,  4, 4, 4, 4,  4, 4, 4, 4, 
	4, 4, 4, 4,  3, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4, 
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4, 
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4, 
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4, 
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4, 
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4, 
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4, 
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4, 
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4
};

// #define MAX_SEQ 1000
#define MAX_SEQ 20000000

#ifdef USE_MALLOC_WRAPPERS
#  include "malloc_wrap.h"
#endif

/********************
 *** SW extension ***
 ********************/

typedef struct {
	int32_t h, e;
} eh_t;

int comp_mux(uint8_t read_base, uint8_t ref_base, int match, int mismatch) {
	return (read_base == 4 || ref_base == 4) ? -1 : read_base == ref_base ? match : -mismatch;
}

void execute_instrution(int id, long instruction, comp_decoder* decoder_unit, compute_unit_32* cu, int* regfile) {
  int i, op[3], in_addr[6], *out_addr, input[6];
  out_addr = (int*)malloc(sizeof(int));
  // printf("\nPC: %d\t", id/2);
  decoder_unit->execute(instruction, op, in_addr, out_addr, &i);
  for (i = 0; i < 6; i++) {
    input[i] = regfile[in_addr[i]];
  }
  regfile[*out_addr] = cu->execute(op, input);
}

void accelerator_register(const uint8_t *target, const uint8_t *query, int match, int mismatch, int tlen, int qlen, int *begin_origin, int *ending_origin, int *begin_align, int *ending_align, int *H_init, eh_t *eh, int gap_o, int gap_e, int *max, int *max_i, int *max_j, int *max_ie, int *gscore, int *max_off, int64_t* numCellsComputed, int print_score) {
	// printf("accelerator_register\n");
	int tmp, max_H = 0, m_j = 0;
	int H_diag = 0, H_left = 0, E_up = 0, F_left = 0, E, F, H = 0;
	int i = 0, j = 0, beg, end;

	int regfile[32];

	regfile[0] = 0;							// constant
	regfile[1] = match;						// constant	
	regfile[2] = mismatch;					// constant	64
	regfile[3] = -gap_o;					// constant
	regfile[4] = -gap_e;					// constant
	regfile[5] = 1;							// constant

	regfile[6] = target[i];					// row initializaiton
	regfile[7] = i;							// row initialization
	regfile[8] = j;							// row initialization and update in the cell
	regfile[9] = max_H;						// row initialization and update in the cell
	regfile[10] = m_j;						// row initialization and update in the cell	

	regfile[11] = query[j];					// from prev pe
	regfile[12] = H_diag;					// from prev pe / fifo
	regfile[13] = E_up;						// from prev pe / fifo
	regfile[14] = H_left;					// update in the cell
	regfile[15] = F_left;					// update in the cell
	regfile[16] = H;						// update in the cell
	// regfile[17];				tmp
	// regfile[18];				tmp
	// regfile[19];				tmp
	// regfile[20];				tmp

	// regfile[21];	qlen
	// regfile[22]; tlen
	// regfile[23]; mlen		tmp
	// regfile[24]; max			output
	// regfile[25]; exit0
	// regfile[26]; gscore		output
	// regfile[27]; max_ie		output
	// regfile[28]; qle			output
	// regfile[29]; tle			output
	// regfile[30]; max_off		output

	for (i = 0; i < tlen; ++i) {
		F_left = 0; max_H = 0; m_j = 0;
		beg = begin_align[i]; end = ending_align[i]; 

		regfile[7] = i;
		regfile[8] = beg-1;
		regfile[9] = regfile[0];												// max_H = 0
		regfile[10] = regfile[0];												// m_j = 0
		regfile[15] = regfile[0];												// F_left = 0
		regfile[14] = H_init[i];												// H_left = H_init[i]

		// beg = begin_origin[i]; end = ending_origin[i]; 
		H_left = H_init[i];
		int qlen_origin = ending_origin[i] - begin_origin[i];
		if (qlen_origin >= 0) (*numCellsComputed) += qlen_origin;

		// printf("%d %d\n", beg, end);
		for (j = beg; j < end; j++) {
		// for (j = begin_origin[i]; j < ending_origin[i]; j++) {
		// 	(*numCellsComputed)++;
			eh_t *p = &eh[j];

			// comp_mux

			H_diag = p->h;					// H(i-1,j-1)
			E_up = p->e;					// E(i-1,j)

			regfile[12] = p->h;														// H_diag
			regfile[13] = p->e;														// E_up

			regfile[17] = comp_mux(target[i], query[j], match, mismatch);			// S = match_score(query, ref)
			regfile[17] = regfile[12] + regfile[17];								// H_diag_S = H_diag + S
			regfile[8] = regfile[8] + regfile[5];									// j++;

			regfile[12] = regfile[12] > regfile[0] ? regfile[17] : regfile[0];		// H_diag = H_diag > 0 ? H_diag_S : 0
			regfile[18] = regfile[12] + (regfile[3] + regfile[4]);					// tmp = H_diag - (gap_o + gap_e)

			regfile[12] = regfile[12] > regfile[0] ? regfile[17] : regfile[0];		// H_diag = H_diag > 0 ? H_diag_S : 0
			regfile[16] = regfile[15] > regfile[13] ? regfile[15] : regfile[13];	// H = F_left > E_up ? F_left : E_up
			regfile[16] = regfile[16] > regfile[12] ? regfile[16] : regfile[12];	// H = H > H_diag ? H : H_diag;

			regfile[10] = regfile[9] > regfile[16] ? regfile[10] : regfile[8];		// m_j = max_H > H? m_j : j
			regfile[9] = regfile[9] > regfile[16] ? regfile[9] : regfile[16];		// max_H = max_H > H? max_H : H

			regfile[20] = regfile[13] + regfile[4];									// E_up -= gap_e
			regfile[13] = regfile[18] > regfile[0]? regfile[18] : regfile[0];		// tmp = tmp > 0? tmp : 0
			regfile[13] = regfile[13] > regfile[20] ? regfile[13] : regfile[20];	// E = E_up > tmp? E_up : tmp
			
			regfile[19] = regfile[15] + regfile[4];									// F_left -= gap_e
			regfile[15] = regfile[18] > regfile[0]? regfile[18] : regfile[0];		// tmp = tmp > 0? tmp : 0
			regfile[15] = regfile[15] > regfile[19] ? regfile[15] : regfile[19];	// F = F_left > tmp? F_left : tmp

			p->h = regfile[14];
			regfile[14] = regfile[16];
			p->e = regfile[13];

			// p->h = H_left;          		// save H(i,j-1) for the next row
			// H_left = H;						// save H(i,j) to H_left for the next column (no need for HW)
			// p->e = E;						// save E(i+1,j) for the next row (no need for HW)
			// F_left = F;

			max_H = regfile[9];
			m_j = regfile[10];
			H_left = regfile[14];

		}
		eh[end].h = H_left; eh[end].e = 0;

		if (end == qlen) {					// software
			*max_ie = *gscore > H_left? *max_ie : i;
			*gscore = *gscore > H_left? *gscore : H_left;
		}
		if (max_H == 0) {
			// printf("break\n");
			break;
		}
		if (max_H > *max) {
			*max = max_H, *max_i = i, *max_j = m_j;
			*max_off = *max_off > abs(m_j - i)? *max_off : abs(m_j - i);
		}

		// update beg and end for the next round
		for (j = beg; j < end && eh[j].h == 0 && eh[j].e == 0; ++j);
		beg = j;
		for (j = end; j >= beg && eh[j].h == 0 && eh[j].e == 0; --j);
		end = j + 2 < qlen? j + 2 : qlen;
		beg = 0; end = qlen; // uncomment this line for debugging
	}
	// printf("%ld\n", (*numCellsComputed));
}

void accelerator(const uint8_t *target, const uint8_t *query, int match, int mismatch, int tlen, int qlen, int *begin_origin, int *ending_origin, int *begin_align, int *ending_align, int *H_init, eh_t *eh, int gap_o, int gap_e, int *max, int *max_i, int *max_j, int *max_ie, int *gscore, int *max_off, int64_t* numCellsComputed, int64_t* cycle, int print_score) {
	// printf("accelerator\n");

	int tmp, max_H = 0, m_j = 0;
	int H_diag, H_left, E_up, F_left, E, F, H, S;
	int i, j, beg, end;
	for (i = 0; i < tlen; ++i) {
		F_left = 0; max_H = 0; m_j = 0;
		beg = begin_align[i]; end = ending_align[i]; 
		// beg = begin_origin[i]; end = ending_origin[i]; 
		H_left = H_init[i];
		int qlen_origin = ending_origin[i] - begin_origin[i];
		if (qlen_origin >= 0) (*numCellsComputed) += qlen_origin;

		// printf("%d %d %d %d\n", begin_align[i], begin_origin[i], ending_align[i], ending_origin[i]);
		// At the beginning of inner loop:
		// eh[j] = {H(i-1,j-1), E(i-1,j)}
		// F_left = F(i,j-1)
		// H_left = H(i,j-1)

		// H(i,j)   = max{H(i-1,j-1) + S(i,j), E(i,j), F(i,j), 0}
		// E(i+1,j) = max{H(i,j) - gapoe, E(i,j) - gape, 0}
		// F(i,j+1) = max{H(i,j) - gapoe, F(i,j) - gape, 0}

		// H(i,j)   = max{H(i-1,j-1) + S(i,j), E(i-1,j), F(i,j-1), 0}
		// E(i,j) 	= max{H_diag - gapoe, E(i-1,j) - gape, 0}
		// F(i,j) 	= max{H_diag - gapoe, F(i,j-1) - gape, 0}

		// two inconsistency between code and algorithm above
		// 1. if H(i-1,j-1) < 0, then no need to add S(i,j), just ignore diagonal
		// 2. Use H_diag rather than new H(i,j) incase new H is E(i-1,j) or F(i,j-1)
		//    then meaningless to compare E(i-1,j) - gapoe and E(i-1,j) - gape

		// printf("%d %d\n", beg, end);
		for (j = beg; j < end; j++) {
		// for (j = begin_origin[i]; j < ending_origin[i]; j++) {
		// 	(*numCellsComputed)++;
			(*cycle)+=5;
			eh_t *p = &eh[j];

			// comp_mux
			S = comp_mux(target[i], query[j], match, mismatch);

			H_diag = p->h;					// H(i-1,j-1)
			E_up = p->e;					// E(i-1,j)

			tmp = H_diag + S;						// instruction 1: copy copy + 	H(i-1,j-1) null S(i,j) null tmp
			H_diag = H_diag > 0 ? tmp : 0;			// instruction 2: mux			tmp H_diag 0 null H_diag
			// H = H_diag > E_up ? H_diag : E_up;
			// H = H > F_left ? H : F_left;			// instruction 3: copy max max	H_diag null E(i,j) F(i,j) H(i,j)
			H = E_up > F_left ? E_up : F_left;
			H = H_diag > H ? H_diag : H;
			// if (print_score) printf("%d %d %d %d %d %d\n", max_H, H, m_j, j, beg, end);
			m_j = max_H > H? m_j : j; 				// instruction 5.1: mux			m_j tmp j null m_j
			max_H = max_H > H? max_H : H; 			// instruction 5.2: mux			max_H tmp H(i,j) null max_H
			tmp = H_diag - (gap_o + gap_e);			// instruction 6: copy + -		H_diag null gap_o gap_e tmp
			tmp = tmp > 0? tmp : 0;
			E_up -= gap_e;							// instruction 7.1: - max max	E_up gap_e tmp 0 E(i,j)
			E = E_up > tmp? E_up : tmp;
			F_left -= gap_e;						// instruction 7.2: - max max	F_left gap_e tmp 0 F(i,j)
			F = F_left > tmp? F_left : tmp;
			// printf("%d %d %d\n", H, E, F);

			p->h = H_left;          		// save H(i,j-1) for the next row
			H_left = H;						// save H(i,j) to H_left for the next column (no need for HW)
			p->e = E;						// save E(i+1,j) for the next row (no need for HW)
			F_left = F;
			// if (print_score) printf("%ld %d %d %d\n", *numCellsComputed, H, E, F);
			// if (print_score) printf("%d %d %d %d %d\n", H, E, F, max_H, m_j);
		}
		// if (print_score) printf("\n");
		eh[end].h = H_left; eh[end].e = 0;

		if (end == qlen) {					// software
			*max_ie = *gscore > H_left? *max_ie : i;
			*gscore = *gscore > H_left? *gscore : H_left;
		}
		if (max_H == 0) {
			// printf("break\n");
			break;
		}
		if (max_H > *max) {
			*max = max_H, *max_i = i, *max_j = m_j;
			*max_off = *max_off > abs(m_j - i)? *max_off : abs(m_j - i);
		}

		// update beg and end for the next round
		for (j = beg; j < end && eh[j].h == 0 && eh[j].e == 0; ++j);
		beg = j;
		for (j = end; j >= beg && eh[j].h == 0 && eh[j].e == 0; --j);
		end = j + 2 < qlen? j + 2 : qlen;
		beg = 0; end = qlen; // uncomment this line for debugging
	}
	// printf("%ld\n", (*numCellsComputed));
}
int largest(int arr[], int n)
{
    int i;
    // Initialize maximum element
    int max = arr[0];
    // Traverse array elements from second and
    // compare every element with current max 
    for (i = 1; i < n; i++)
        if (arr[i] > max)
            max = arr[i];
    return max;
}
int smallest(int arr[], int n)
{
    int i;
    // Initialize maximum element
    int min = arr[0];
    // Traverse array elements from second and
    // compare every element with current max 
    for (i = 1; i < n; i++)
        if (arr[i] < min)
            min = arr[i];
    return min;
}
// ksw_extend2_new(match, mismatch, qlen, read_ar, tlen, ref_ar, 5, mat, gapo, gape, w, 5, 100, h0, &qle, &tle, &gtle, &gscore, &max_off)
int ksw_extend2_new(int match, int mismatch, int qlen, const uint8_t *query, int tlen, const uint8_t *target, int m, const int8_t *mat, int gap_o, int gap_e, int w, int end_bonus, int h0, int *_qle, int *_tle, int *_gtle, int *_gscore, int *_max_off, int64_t* numCellsComputed, int64_t* cycle, int print_score, int pe_group)
{
	int i, j, k, gap_oe = gap_o + gap_e, beg, end, max_ins, max;

	eh_t *eh; // H, E score array
	// allocate memory
	eh = (eh_t*)calloc(qlen + 1, 8);	// store the value in the last row
	// fill the last row
	eh[0].h = h0; eh[1].h = h0 > gap_oe? h0 - gap_oe : 0;
	for (j = 2; j <= qlen && eh[j-1].h > gap_e; ++j)
		eh[j].h = eh[j-1].h - gap_e;

	// adjust $w if it is too large
	k = m * m;
	for (i = 0, max = 0; i < k; ++i) // get the max score: 1
		max = max > mat[i]? max : mat[i];
	max_ins = (int)((double)(qlen * max + end_bonus - gap_o) / gap_e + 1.);
	if (max_ins != qlen) printf("!");
	max_ins = max_ins > 1? max_ins : 1;
	w = w < max_ins? w : max_ins;
	if (qlen) w = qlen;
	// if (w != max_ins) printf("%d %d\n", w, max_ins);

	// DP loop
	int max_i = -1, max_j = -1, max_ie = -1, gscore = -1, max_off = 0;
	max = h0;
	
	int *H_init, *begin, *ending, *begin_align, *ending_align;
	H_init = (int*)malloc(tlen * sizeof(int));
	begin = (int*)malloc(tlen * sizeof(int));
	ending = (int*)malloc(tlen * sizeof(int));
	begin_align = (int*)malloc(tlen * sizeof(int));
	ending_align = (int*)malloc(tlen * sizeof(int));
	for (i = 0; i < tlen; ++i) {
		beg = 0, end = qlen;
		// apply the band and the constraint (if provided)
		if (beg < i - w) begin[i] = i - w;
		else begin[i] = beg;
		if (end > i + w + 1) ending[i] = i + w + 1;
		else ending[i] = end;
		// printf("%d %d %d %d\n", i, qlen, begin[i], ending[i]);
		// compute the first column
		if (begin[i] == 0) {
			H_init[i] = h0 - (gap_o + gap_e * (i + 1));
			if (H_init[i] < 0) H_init[i] = 0;
		} else H_init[i] = 0;
	}

	// for (i = 0; i < tlen; ++i)
	// 	for (j = begin[i]; j < ending[i]; j++){
	// 		(*numCellsComputed)++;
	// 		// (*cycle)+=5;
	// 	}
	
	// find the smallest beginning position and the largest ending position
	int pe_group_size = pe_group, begin_tmp[64], end_tmp[64], begin_min, end_max;
	// printf("%d", pe_group_size);
	for (i = 0; i < 64; i++){
		begin_tmp[i] = 0; end_tmp[i] = 0;
	}
	for (i = 0; i < tlen; i+=pe_group_size){
		(*cycle)++;
		for (j = 0; j < pe_group_size; j++) if((j+i)<tlen) begin_tmp[j] = begin[i+j];
		for (j = 0; j < pe_group_size; j++) if((j+i)<tlen) end_tmp[j] = ending[i+j];
		if((pe_group_size+i)<tlen) begin_min = smallest(begin_tmp, pe_group_size);
		else begin_min = smallest(begin_tmp, tlen-i);
		if((pe_group_size+i)<tlen) end_max = largest(end_tmp, pe_group_size);
		else end_max = largest(end_tmp, tlen-i);
		for (j = 0; j < pe_group_size; j++) {
			if((j+i)<tlen) {
				begin_align[j+i] = begin_min;
				ending_align[j+i] = end_max;
			}
			else break;
			// printf("%d %d %d %d\n", i, begin_min, end_max, end);
			// if((j+i)<tlen) for (j = begin_min; j < end_max; j++) {
			// 	(*cycle)+=5;
			// } else for (j = 0; j < pe_group_size; j++) (*cycle)+=5;
		}
	}
		
	// resort to accelerator
	// accelerator_register(target, query, match, mismatch, tlen, qlen, begin, ending, begin_align, ending_align, H_init, eh, gap_o, gap_e, &max, &max_i, &max_j, &max_ie, &gscore, &max_off, numCellsComputed, print_score);
	accelerator(target, query, match, mismatch, tlen, qlen, begin, ending, begin_align, ending_align, H_init, eh, gap_o, gap_e, &max, &max_i, &max_j, &max_ie, &gscore, &max_off, numCellsComputed, cycle, print_score);

	free(eh);


	if (_qle) *_qle = max_j + 1;
	if (_tle) *_tle = max_i + 1;
	if (_gtle) *_gtle = max_ie + 1;
	if (_gscore) *_gscore = gscore;
	if (_max_off) *_max_off = max_off;
	// printf("%d %d %d %d %d\n", max_j, max_i, max_ie, gscore, max_off);
	// printf("%d %d %d %d %d\n", *_qle, *_tle, *_gtle, *_gscore, *_max_off);	
	
	return max;
}

int ksw_extend2(int qlen, const uint8_t *query, int tlen, const uint8_t *target, int m, const int8_t *mat, int o_del, int e_del, int o_ins, int e_ins, int w, int end_bonus, int zdrop, int h0, int *_qle, int *_tle, int *_gtle, int *_gscore, int *_max_off, int64_t* numCellsComputed, int print_score)
{
	eh_t *eh; // score array
	int8_t *qp; // query profile
	int i, j, k, oe_del = o_del + e_del, oe_ins = o_ins + e_ins, beg, end, max, max_i, max_j, max_ins, max_del, max_ie, gscore, max_off;
	// if(h0 <= 0) printf("%d", h0);
	// else assert(h0 > 0);
	assert(h0 > 0);
	// allocate memory
	qp = (int8_t*)malloc(qlen * m);
	eh = (eh_t*)calloc(qlen + 1, 8);	// store the value in the last row
	// generate the query profile
	for (k = i = 0; k < m; ++k) {
		const int8_t *p = &mat[k * m];
		for (j = 0; j < qlen; ++j) {
			qp[i++] = p[query[j]];
			// printf("%d ", p[query[j]]);
		}
		// printf("\n");
	}

	// fill the last row
	eh[0].h = h0; eh[1].h = h0 > oe_ins? h0 - oe_ins : 0;
	for (j = 2; j <= qlen && eh[j-1].h > e_ins; ++j)
		eh[j].h = eh[j-1].h - e_ins;
	// adjust $w if it is too large
	k = m * m;
	for (i = 0, max = 0; i < k; ++i) // get the max score: 1
		max = max > mat[i]? max : mat[i];
	max_ins = (int)((double)(qlen * max + end_bonus - o_ins) / e_ins + 1.);
	max_ins = max_ins > 1? max_ins : 1;
	w = w < max_ins? w : max_ins;
	max_del = (int)((double)(qlen * max + end_bonus - o_del) / e_del + 1.);
	max_del = max_del > 1? max_del : 1;
	w = w < max_del? w : max_del; // TODO: is this necessary?
	// if (w > 100) printf("%d ", qlen);
	// DP loop
	max = h0, max_i = max_j = -1; max_ie = -1, gscore = -1;
	max_off = 0;
	beg = 0, end = qlen;

	// struct timeval start_time, end_time;
    // gettimeofday(&start_time, NULL);
	for (i = 0; i < tlen; ++i) {
		int t, f = 0, h1, m = 0, mj = -1;
		int8_t *q = &qp[target[i] * qlen];
		// apply the band and the constraint (if provided)
		if (beg < i - w) beg = i - w;
		if (end > i + w + 1) end = i + w + 1;
		if (end > qlen) end = qlen;
		// compute the first column
		if (beg == 0) {
			h1 = h0 - (o_del + e_del * (i + 1));
			if (h1 < 0) h1 = 0;
		} else h1 = 0;
		// printf("%d %d %d\n", w, beg, end);
		for (j = beg; j < end; ++j) {
			// At the beginning of the loop: eh[j] = { H(i-1,j-1), E(i-1,j) }, f = F(i,j-1) and h1 = H(i,j-1)
			// Similar to SSE2-SW, cells are computed in the following order:
			//   H(i,j)   = max{H(i-1,j-1)+S(i,j), E(i,j), F(i,j)}
			//   E(i+1,j) = max{H(i,j)-gapoe, E(i,j)} - gape
			//   F(i,j+1) = max{H(i,j)-gapoe, F(i,j)} - gape
			(*numCellsComputed)++;
			eh_t *p = &eh[j];
			int h, M = p->h, e = p->e; // get H(i-1,j-1) and E(i-1,j)
			if (print_score) printf("%d ", M);
			p->h = h1;          // save H(i,j-1) for the next row
			M = M? M + q[j] : 0;// H(i-1,j-1)+S(i,j)
			if (print_score) printf("%d ", M);
			h = M > e? M : e;   // e and f are guaranteed to be non-negative, so h>=0 even if M<0
			h = h > f? h : f;	// h = max(M, e, f)
			mj = m > h? mj : j; // record the position where max score is achieved
			m = m > h? m : h;   // m is stored at eh[mj+1]
			t = M - oe_del;
			t = t > 0? t : 0;
			e -= e_del;
			e = e > t? e : t;   // computed E(i+1,j)
			// t = M - oe_ins;
			// t = t > 0? t : 0;
			f -= e_ins;
			f = f > t? f : t;   // computed F(i,j+1)
			h1 = h;             // save H(i,j) to h1 for the next column
			p->e = e;           // save E(i+1,j) for the next row
			// if (print_score) printf("%ld %d %d %d\n", *numCellsComputed, h, e, f);
			if (print_score) printf("%d %d %d\n", h, e, f);
		}

		if (print_score) printf("\n");
		eh[end].h = h1; eh[end].e = 0;
		if (j == qlen) {
			max_ie = gscore > h1? max_ie : i;
			gscore = gscore > h1? gscore : h1;
			// printf("%d %d\n", i, gscore);
		}
		if (m == 0) {
			// printf("break\n");
			break;
		}
		if (m > max) {
			max = m, max_i = i, max_j = mj;
			max_off = max_off > abs(mj - i)? max_off : abs(mj - i);
		} 
		else if (zdrop > 0) {
			if (i - max_i > mj - max_j) {
				if (max - m - ((i - max_i) - (mj - max_j)) * e_del > zdrop) {printf("ZDROP\n"); break;}
			} else {
				if (max - m - ((mj - max_j) - (i - max_i)) * e_ins > zdrop) {printf("ZDROP\n"); break;}
			}
		}
		// update beg and end for the next round
		for (j = beg; j < end && eh[j].h == 0 && eh[j].e == 0; ++j);
		beg = j;
		for (j = end; j >= beg && eh[j].h == 0 && eh[j].e == 0; --j);
		end = j + 2 < qlen? j + 2 : qlen;
		// beg = 0; end = qlen; // uncomment this line for debugging
	}
	// printf("\n");
	// printf("%ld\n", (*numCellsComputed));

	// gettimeofday(&end_time, NULL);
    // *runtime += (end_time.tv_sec - start_time.tv_sec) * 1e6 + (end_time.tv_usec - start_time.tv_usec);
	free(eh); free(qp);
	// printf("%ld\n", *numCellsComputed);
	// printf("Qlen:%d,Rlen:%d,ScalarNumCellsComputed:%lld\n", qlen, tlen, numCellsComputed);

	if (_qle) *_qle = max_j + 1;
	if (_tle) *_tle = max_i + 1;
	if (_gtle) *_gtle = max_ie + 1;
	if (_gscore) *_gscore = gscore;
	if (_max_off) *_max_off = max_off;

	// printf("%d %d %d %d %d\n", max_j, max_i, max_ie, gscore, max_off);
	// printf("%d %d %d %d %d\n", *_qle, *_tle, *_gtle, *_gscore, *_max_off);
	return max;
}

void help() {
    printf("\n"
        "usage: ./chain [options ...]\n"
        "\n"
        "    options:\n"
        "        -i <input file>\n"
        "            default: NULL\n"
        "            the input dataset\n"
        "        -o <output file>\n"
        "            default: NULL\n"
        "            the output file\n"
        "        -t <int>\n"
        "            default: 1\n"
        "            number of CPU threads\n"
        "        -h \n"
        "            prints the usage\n");
}

#define MAX_LINE_LEN 2048

typedef struct dnaSeqPair {
    uint8_t *read_ar = 0, *ref_ar = 0;
	int h0, qlen, tlen, w;
    int score = 0, tle = -1, gtle = -1, qle = -1, gscore = -1, max_off = 0;
}SeqPair;

int main(int argc, char *argv[])
{
	int sa = 1, sb = 4, i, j, k;	// sa: match reward, sb: mismatch penalty
	int8_t mat[25];
	int gapo = 6, gape = 1;
	uint8_t *read_ar = 0, *ref_ar = 0;

	// read_ar = (uint8_t*) malloc(MAX_SEQ * sizeof(uint8_t));
	// ref_ar = (uint8_t*) malloc(MAX_SEQ * sizeof(uint8_t));

    FILE* fp_in, *fp_out;

	char *inputFileName = 0, *outputFileName = 0;
	int serial = 0, parallel = 0, print_score = 0, num_lines = 1000, pe_group = 4, accelerator_kernel = 0;

    char opt;
	int numThreads = 1;
    while ((opt = getopt(argc, argv, "i:o:t:n:g:hspka")) != -1) {
        switch (opt) {
            case 'i': inputFileName = optarg; break;
            case 'o': outputFileName = optarg; break;
            case 't': numThreads = atoi(optarg); break;
			case 'n': num_lines = atoi(optarg); break;
			case 'g': pe_group = atoi(optarg); break;
            case 'h': help(); return 0;
			case 's': serial = 1; break;
			case 'p': parallel = 1; break;
			case 'k': print_score = 1; break;
			case 'a': accelerator_kernel = 1; break;
            default: help(); return 1;
        }
    }

	// if (argc < 3) {
	// 	fprintf(stderr, "Usage: ksw <input.txt> <output.txt>\n");
	// 	return 1;
	// }
	// inputFileName = argv[1];
	// outputFileName = argv[2];

	/* initialize scoring matrix
		 1  -4  -4  -4  -1
		-4   1  -4  -4  -1
		-4  -4   1  -4  -1
		-4  -4  -4   1  -1
		-1  -1  -1  -1  -1
	*/
	for (i = k = 0; i < 4; ++i) {
		for (j = 0; j < 4; ++j)
			mat[k++] = i == j? sa : -sb;
		mat[k++] = -1; // ambiguous base
	}
	for (j = 0; j < 5; ++j) mat[k++] = -1;

	// open file
	// fp_in = fopen(argv[1], "r");
	// fp_out = fopen(argv[2], "w");

	printf("%s %s\n", inputFileName, outputFileName);
	fp_in = fopen(inputFileName, "r");
	fp_out = fopen(outputFileName, "w");

	if (fp_in == NULL) {
		fprintf(stderr, "Unable to open %s for reading\n", argv[1]);
		return 1;
	}

	if (fp_out == NULL) {
		fprintf(stderr, "Unable to open %s for writing\n", argv[2]);
		return 1;
	}

	char line[MAX_LINE_LEN], *linePtr;
	std::string delim_string = ",";
	char delim[2];
	strcpy(delim, delim_string.c_str());
	int numLinesRead = 0;

	char* query = 0, *target = 0;
	// int h0, qlen, tlen, w;
	// int score = 0, qle = 0, tle = 0, gtle = 0, gscore = 0, max_off = 0;
	double runtime = 0;
	int64_t numCellsComputed = 0;
	SeqPair* SeqPairArr;
	SeqPairArr = (SeqPair*) malloc(num_lines * sizeof(SeqPair));
	struct timeval start_time, end_time;
	//
	// Read input data
	//
	while (fgets(line, sizeof(line), fp_in) != NULL && numLinesRead < MAX_SEQ) {
		linePtr = line;
		if (!isalpha(*linePtr)) continue;
		query = strsep(&linePtr, delim);
		target = strsep(&linePtr, delim);
		if (query == NULL || target == NULL) continue;
		SeqPairArr[numLinesRead].h0 = atoi(strsep(&linePtr, delim));
		SeqPairArr[numLinesRead].w = atoi(strsep(&linePtr, delim));
		SeqPairArr[numLinesRead].qlen = strlen(query);
		SeqPairArr[numLinesRead].tlen = strlen(target);
		SeqPairArr[numLinesRead].read_ar = (uint8_t*) malloc(SeqPairArr[numLinesRead].qlen * sizeof(uint8_t));
		SeqPairArr[numLinesRead].ref_ar = (uint8_t*) malloc(SeqPairArr[numLinesRead].tlen * sizeof(uint8_t));
		for (i = 0; i < SeqPairArr[numLinesRead].qlen; ++i) {
			SeqPairArr[numLinesRead].read_ar[i] = seq_nt4_table[(int)query[i]];
		}
		for (i = 0; i < SeqPairArr[numLinesRead].tlen; ++i) {
			SeqPairArr[numLinesRead].ref_ar[i] = seq_nt4_table[(int)target[i]];
		}

    	gettimeofday(&start_time, NULL);

		// SeqPairArr[numLinesRead].score = ksw_extend2(SeqPairArr[numLinesRead].qlen, SeqPairArr[numLinesRead].read_ar, SeqPairArr[numLinesRead].tlen, SeqPairArr[numLinesRead].ref_ar, 5, mat, gapo, gape, gapo, gape, SeqPairArr[numLinesRead].w, 5, 100, SeqPairArr[numLinesRead].h0, &SeqPairArr[numLinesRead].qle, &SeqPairArr[numLinesRead].tle, &SeqPairArr[numLinesRead].gtle, &SeqPairArr[numLinesRead].gscore, &SeqPairArr[numLinesRead].max_off, &numCellsComputed, print_score); 
		// SeqPairArr[numLinesRead].score = ksw_extend2_new(sa, sb, SeqPairArr[numLinesRead].qlen, SeqPairArr[numLinesRead].read_ar, SeqPairArr[numLinesRead].tlen, SeqPairArr[numLinesRead].ref_ar, 5, mat, gapo, gape, SeqPairArr[numLinesRead].w, 5, SeqPairArr[numLinesRead].h0, &SeqPairArr[numLinesRead].qle, &SeqPairArr[numLinesRead].tle, &SeqPairArr[numLinesRead].gtle, &SeqPairArr[numLinesRead].gscore, &SeqPairArr[numLinesRead].max_off, &numCellsComputed, print_score); 

		// score = ksw_extend2(qlen, read_ar, tlen, ref_ar, 5, mat, gapo, gape, gapo, gape, w, 5, 100, h0, &qle, &tle, &gtle, &gscore, &max_off, &numCellsComputed, &numQuery, print_score); 
		
		gettimeofday(&end_time, NULL);
   	 	runtime += (end_time.tv_sec - start_time.tv_sec) * 1e6 + (end_time.tv_usec - start_time.tv_usec);
		
		if(serial) fprintf(fp_out, "%d,%d,%d,%d,%d,%d\n", SeqPairArr[numLinesRead].score, SeqPairArr[numLinesRead].qle, SeqPairArr[numLinesRead].tle, SeqPairArr[numLinesRead].gtle, SeqPairArr[numLinesRead].gscore, SeqPairArr[numLinesRead].max_off);
		
		numLinesRead += 1;
		free(read_ar);
		free(ref_ar);
	}
	printf("Update %ld cells\n", numCellsComputed);
    fprintf(stderr, "Time in kernel: %.2f sec\n", runtime * 1e-6);

	fclose(fp_in);

#pragma omp parallel num_threads(numThreads)
{
    int tid = omp_get_thread_num();
    if (tid == 0) {
        fprintf(stderr, "Running with threads: %d\n", numThreads);
    }
}

	int64_t numCellsUpdate[numThreads * CLMUL];
	int64_t numCycles[numThreads * CLMUL];
	int64_t TotalCellsUpdate = 0;
	int64_t TotalCycles = 0;
	double TotalRuntime = 0;
	for (i = 0; i < numThreads * CLMUL; i++) {
		numCellsUpdate[i] = 0;
		numCycles[i] = 0;
	}

    gettimeofday(&start_time, NULL);

#if RAPL
	Rapl * rapl = new Rapl();
	rapl->reset();
#endif

#pragma omp parallel num_threads(numThreads)
{
    int tid = omp_get_thread_num();
	#pragma omp for schedule(dynamic, 1)
	for(i = 0; i < numLinesRead; i++) {
		int64_t numCellsComputed = 0;
		int64_t cycle = 0;

		if (accelerator_kernel) SeqPairArr[i].score = ksw_extend2_new(sa, sb, SeqPairArr[i].qlen, SeqPairArr[i].read_ar, SeqPairArr[i].tlen, SeqPairArr[i].ref_ar, 5, mat, gapo, gape, SeqPairArr[i].w, 5, SeqPairArr[i].h0, &(SeqPairArr[i].qle), &(SeqPairArr[i].tle), &(SeqPairArr[i].gtle), &(SeqPairArr[i].gscore), &(SeqPairArr[i].max_off), &numCellsComputed, &cycle, print_score, pe_group);
		else SeqPairArr[i].score = ksw_extend2(SeqPairArr[i].qlen, SeqPairArr[i].read_ar, SeqPairArr[i].tlen, SeqPairArr[i].ref_ar, 5, mat, gapo, gape, gapo, gape, SeqPairArr[i].w, 5, 100, SeqPairArr[i].h0, &(SeqPairArr[i].qle), &(SeqPairArr[i].tle), &(SeqPairArr[i].gtle), &(SeqPairArr[i].gscore), &(SeqPairArr[i].max_off), &numCellsComputed, print_score); 
		
		if (parallel) fprintf(fp_out, "%d %d %d %d %d %d\n", SeqPairArr[i].score, SeqPairArr[i].qle, SeqPairArr[i].tle, SeqPairArr[i].gtle, SeqPairArr[i].gscore, SeqPairArr[i].max_off);

		numCellsUpdate[tid * CLMUL] += numCellsComputed;
		numCycles[tid * CLMUL] += cycle;
	}
}

#if RAPL
	rapl->sample();
	float total_time = rapl->total_time();
	float pkg_power = rapl->pkg_average_power();
	float dram_power = rapl->dram_average_power();
	printf("Running time is %f sec.\n", total_time);
	printf("Power: pkg %f W; DRAM %f W\n", pkg_power, dram_power);
#endif


	gettimeofday(&end_time, NULL);
    TotalRuntime += (end_time.tv_sec - start_time.tv_sec) * 1e6 + (end_time.tv_usec - start_time.tv_usec);

	for (i = 0; i < numThreads; i++) {
		TotalCellsUpdate += numCellsUpdate[i * CLMUL];
		TotalCycles += numCycles[i * CLMUL];
	}

	double GCUPS = TotalCellsUpdate / (TotalCycles / 64 / 4 / 1.5);

	printf("Update %ld cells\n", TotalCellsUpdate);
	printf("cycle: %ld\n", TotalCycles);
	printf("GCUPS: %lf\n", GCUPS);
    fprintf(stderr, "Parallel Time: %.2f sec\n", TotalRuntime * 1e-6);
	
	fclose(fp_out);
	return 0;
}
