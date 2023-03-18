#include <vector>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "omp.h"
#include "host_kernel.h"
#include "common.h"
// #include "minimap.h"
#include "mmpriv.h"
#include "kalloc.h"
#include "fixed.h"
// #include "compute_unit_32.h"
// #include "comp_decoder.h"

#define CLMUL 8

using namespace numeric;
typedef Fixed<16, 16> fixed;

static const char LogTable256[256] = {
#define LT(n) n, n, n, n, n, n, n, n, n, n, n, n, n, n, n, n
	-1, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
	LT(4), LT(5), LT(5), LT(6), LT(6), LT(6), LT(6),
	LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7), LT(7)
};

static inline int ilog2_32(uint32_t v)
{
	uint32_t t, tt;
	if ((tt = v>>16)) return (t = tt>>8) ? 24 + LogTable256[t] : 16 + LogTable256[tt];
	return (t = v>>8) ? 8 + LogTable256[t] : LogTable256[v];
}

static inline float mg_log2(float x) // NB: this doesn't work when x<2
{
	union { float f; uint32_t i; } z = { x };
	float log_2 = ((z.i >> 23) & 255) - 128;
	z.i &= ~(255 << 23);
	z.i += 127 << 23;
	log_2 += (-0.34484843f * z.f + 2.02466578f) * z.f - 0.67487759f;
	return log_2;
}

void execute_instrution(int id, long instruction, comp_decoder* decoder_unit, compute_unit_32* cu, int* regfile) {
  int i, op[3], in_addr[6], *out_addr, input[6];
  out_addr = (int*)malloc(sizeof(int));
//   printf("PC: %d\t", id/2);
  decoder_unit->execute(instruction, op, in_addr, out_addr, &i);
  for (i = 0; i < 6; i++) {
    input[i] = regfile[in_addr[i]];
  }
  regfile[*out_addr] = cu->execute(op, input);
}

/***********
 * if (v < 256) return LogTable256[v];
 * else if (v < 2^16) return (8 + LogTable256[t]);
 * else if (v < 2^24) return (8 + LogTable256[t]);
 * else return (8 + LogTable256[t]);
 ***********/

int32_t compute_sc(int32_t dr, int32_t dq, int32_t dd, float avg_qspan, int32_t q_span){
	int32_t min_d = dq < dr? dq : dr;	
	// if (min_d != (int32_t)(dq < dr? dq : dr)) printf("%d %d %ld\n", min_d, dq, dr);
	int32_t alpha = min_d > q_span? q_span : min_d;	// alpha
	int32_t log_dd = dd? ilog2_32(dd) : 0;
	fixed f_dd = dd;
	fixed f_tmp = 0.01 * avg_qspan;
	fixed f_result = f_dd * f_tmp;
	// if ((int)f_result.to_float() != (int)(dd * .01 * avg_qspan))
	// 	std::cout << dd << " " << f_result.to_float() << " " << (dd * .01 * avg_qspan) << "\n";
	// int32_t gap_cost = (int)(dd * .01 * avg_qspan) + (log_dd>>1);
	int32_t gap_cost = (int)f_result.to_float() + (log_dd>>1);
	return alpha - gap_cost;
}

#define MM_SEED_SEG_SHIFT  48
#define MM_SEED_SEG_MASK   (0xffULL<<(MM_SEED_SEG_SHIFT))

void chain_dp(call_t* a, return_t* ret, int64_t* num_anchor, int64_t* num_cell, int64_t* all_cell, int64_t* cycle)
{

	// TODO: make sure this works when n has more than 32 bits
	int64_t i, j, st = 0;
	int is_cdna = 0;
    const float gap_scale = 1.0f;
    const int max_iter = 5000;
    const int max_skip = 25;
    int max_dist_x = a->max_dist_x, max_dist_y = a->max_dist_y, bw = a->bw;
    float avg_qspan = a->avg_qspan;
    int n_segs = a->n_segs; 
    int64_t n = a->n;
	ret->n = n;
	*num_anchor += n;
	// printf("%lld\n", n);
	int iter = 0;
	int dd_max = 0, max_dr = 0, max_min_d = 0;

	// std::cout << n << std::endl;
	ret->scores.resize(n);
	ret->parents.resize(n);
    ret->targets.resize(n);
    ret->peak_scores.resize(n);

	// fill the score and backtrack arrays
	for (i = 0; i < n; ++i) {
		uint64_t ri = a->anchors[i].x;
		int64_t max_j = -1;
		int32_t qi = (int32_t)a->anchors[i].y, q_span = a->anchors[i].y>>32&0xff; // NB: only 8 bits of span is used!!!
		// std::cout << a->anchors[i].y << " " << (int32_t)a->anchors[i].y << " " << q_span << " " << (a->anchors[i].y>>32) << "\n";
		int32_t max_f = q_span, n_skip = 0, min_d;
		int32_t sidi = (a->anchors[i].y & MM_SEED_SEG_MASK) >> MM_SEED_SEG_SHIFT;
		while (st < i && ri > a->anchors[st].x + max_dist_x) ++st;
		if (i - st > max_iter) st = i - max_iter;
		// if ((i - 1) < st) printf("skip\n");
		for (j = i - 1; j >= st; --j) {
			*all_cell += 1;
			// if (j == i - 1) {
			// 	printf("%d\n", iter);
			// 	iter = 0;
			// }
			int32_t dr = ri - a->anchors[j].x;		// dr is int64_t data type but max_dr is only 5000
			if (dr > max_dr) max_dr = dr;
			int32_t dq = qi - (int32_t)a->anchors[j].y, dd, sc, log_dd, gap_cost;

			int32_t sidj = (a->anchors[j].y & MM_SEED_SEG_MASK) >> MM_SEED_SEG_SHIFT;
			if ((sidi == sidj && dr == 0) || dq <= 0) {continue; printf("continue 0");} // don't skip if an anchor is used by multiple segments; see below
			if ((sidi == sidj && dq > max_dist_y) || dq > max_dist_x) {continue; printf("continue 1");}
			if (int32_t(dr - dq) != (int32_t)dr - dq) printf("!");
			dd = dr > dq? dr - dq : dq - dr;
			// if ((int32_t)(dr > dq? dr - dq : dq - dr) < 0) printf("l_tmp: %ld l_tmp_minus: %ld dd: %d\n",dr - dq ,dq - dr, dd);
			// printf("%d ", dd);
			if (dd > dd_max) dd_max = dd;
			if (sidi == sidj && dd > bw) {continue; printf("continue 2");}
			if (n_segs > 1 && !is_cdna && sidi == sidj && dr > max_dist_y) {printf("continue 3");continue; }
			*num_cell += 1;
			min_d = dq < dr? dq : dr;
			if (max_min_d < min_d) max_min_d = min_d;
			sc = min_d > q_span? q_span : min_d;	// alpha
			log_dd = dd? ilog2_32(dd) : 0;
			gap_cost = 0;
			if (is_cdna || sidi != sidj) {
				int c_log, c_lin;
				c_lin = (int)(dd * .01 * avg_qspan);
				c_log = log_dd;
				if (sidi != sidj && dr == 0) ++sc; // possibly due to overlapping paired ends; give a minor bonus
				else if (dr > dq || sidi != sidj) gap_cost = c_lin < c_log? c_lin : c_log;
				else gap_cost = c_lin + (c_log>>1);
			} else gap_cost = (int)(dd * .01 * avg_qspan) + (log_dd>>1);
			// } else gap_cost = (int)f_result.to_float() + (log_dd>>1);
			sc -= (int)((double)gap_cost * gap_scale);
			sc += ret->scores[j];
			// printf("i: %d, j: %d, parent[j]: %d, target[j]: %d\n", i, j, ret->parents[j], ret->targets[j]);
			if (sc > max_f) {
				max_f = sc, max_j = j;
                if (n_skip > 0) --n_skip;
			} else if (ret->targets[j] == i) {
                if (++n_skip > max_skip) {
					break;
                }
            }
            if (ret->parents[j] >= 0) ret->targets[ret->parents[j]] = i;
			
			iter++;
			// std::cout << iter << std::endl;
		}
		ret->scores[i] = max_f, ret->parents[i] = max_j;
        ret->peak_scores[i] = max_j >= 0 && ret->peak_scores[max_j] > max_f ? ret->peak_scores[max_j] : max_f;
		// std::cout << "scores: " << ret->scores[i] << " parents: " << ret->parents[i] << std::endl;
	}
	// printf("max_dr: %d\n", max_dr);
	// printf("max_dq: %d\n", max_dq);
	// printf("dd_max: %d\n", dd_max);
	// printf("max_min_d: %d\n", max_min_d);
}



void chain_dp_copy(call_t* a, return_t* ret, int64_t* num_anchor, int64_t* num_cell, int64_t* all_cell, int64_t* cycle)
{

	// TODO: make sure this works when n has more than 32 bits
	int64_t i, j, st = 0;
    const int max_iter = 5000;
    const int max_skip = 25;
    int max_dist_x = a->max_dist_x, max_dist_y = a->max_dist_y, bw = a->bw;
    float avg_qspan = a->avg_qspan;
    int64_t n = a->n;
	ret->n = n;
	*num_anchor += n;
	int iter = 0, iter_max = 0, flag;

	ret->scores.resize(n);
	ret->parents.resize(n);
    ret->targets.resize(n);
    ret->peak_scores.resize(n);

	// fill the score and backtrack arrays
	for (i = 0; i < n; ++i) {
		uint64_t ri = a->anchors[i].x;
		int64_t max_j = -1;
		int32_t qi = (int32_t)a->anchors[i].y, q_span = a->anchors[i].y>>32&0xff; // NB: only 8 bits of span is used!!!
		int32_t max_f = q_span, n_skip = 0;
		while (st < i && ri > a->anchors[st].x + max_dist_x) ++st;
		if (i - st > max_iter) st = i - max_iter;
		// if ((i - 1) < st) printf("skip\n");
		// printf("%d\n", i - st);
		iter = i - st;
		for (j = i - 1; j >= st; --j) {
			*all_cell += 1;
				if (j == i - 1) {
					if (iter > iter_max) iter_max = iter;
					printf("iter: %d\n", iter);
					iter = 0;
				}
			int32_t dr = ri - a->anchors[j].x;		// dr is int64_t data type but max_dr is only 5000
			int32_t dq = qi - (int32_t)a->anchors[j].y;
			int32_t dd = dr > dq? dr - dq : dq - dr;

			flag = dr == 0 ? 1 : 0;
			flag = dq <= 0 ? 1 : flag;
			flag = dq > max_dist_y ? 1 : flag;
			flag = dd > bw ? 1 : flag;
			*num_cell += 1;

			int32_t sc;
			if (flag) sc = INT32_MIN;
			else sc = compute_sc(dr, dq, dd, avg_qspan, q_span);
			sc = compute_sc(dr, dq, dd, avg_qspan, q_span);
			sc += ret->scores[j];
			if (sc > max_f) {
				max_f = sc, max_j = j;
                if (n_skip > 0) --n_skip;
			} else if (ret->targets[j] == i) {
                if (++n_skip > max_skip) {
					break;
                }
            }
            if (ret->parents[j] >= 0) ret->targets[ret->parents[j]] = i;
			
			iter++;
		}
		ret->scores[i] = max_f, ret->parents[i] = max_j;
        ret->peak_scores[i] = max_j >= 0 && ret->peak_scores[max_j] > max_f ? ret->peak_scores[max_j] : max_f;
	}
	fprintf(stdout, "max_iter: %d\n", iter_max);
}

void chain_dp_25(call_t* a, return_t* ret, int64_t* num_anchor, int64_t* num_cell, int64_t* all_cell, int64_t* cycle)
{

	// TODO: make sure this works when n has more than 32 bits
	int64_t i, j;
    const int max_skip = 64;
    int max_dist_x = a->max_dist_x, max_dist_y = a->max_dist_y, bw = a->bw;
    float avg_qspan = a->avg_qspan;
    int64_t n = a->n;
	ret->n = n;
	*num_anchor += n;
	int iter = 0, flag;

	ret->scores.resize(n);
	ret->parents.resize(n);
    ret->targets.resize(n);
    ret->peak_scores.resize(n);
			
	// fill the score and backtrack arrays
	for (i = 0; i < n; ++i) {
		uint64_t ri = a->anchors[i].x;
		int64_t max_j = -1;
		int32_t qi = (int32_t)a->anchors[i].y, q_span = a->anchors[i].y>>32&0xff; // NB: only 8 bits of span is used!!!
		// if (i == 2770) printf("%d\n\n\n", q_span);
		int32_t max_f = q_span;
		int32_t beg = (i - max_skip) < 0? 0 : i - max_skip;
		for (j = i - 1; j >= beg; --j) {
			*all_cell += 1;
			if (j == i - 1) {
				// printf("%d\nd", iter);
				iter = 0;
			}
			int32_t dr = ri - a->anchors[j].x;		// dr is int64_t data type but max_dr is only 5000
			int32_t dq = qi - (int32_t)a->anchors[j].y;
			int32_t dd = dr > dq? dr - dq : dq - dr;
			// if ((dr > dq? dr - dq : dq - dr) < 0) printf("dr: %ld dq: %d dd: %d %d %d %d\n", dr, dq, dd, (dr-dq), (dq-dr), dr > dq ? 1 : 0);

			flag = dr == 0 ? 1 : 0;
			flag = dq <= 0 ? 1 : flag;
			flag = dq > max_dist_y ? 1 : flag;
			flag = dd > bw ? 1 : flag;

			int32_t sc;
			if (flag) continue;
			else sc = compute_sc(dr, dq, dd, avg_qspan, q_span);
			*num_cell += 1;

			sc += ret->scores[j];
			if (sc > max_f) {
				max_f = sc, max_j = j;
			}
            if (ret->parents[j] >= 0) ret->targets[ret->parents[j]] = i;
			
			iter++;
		}
		ret->scores[i] = max_f, ret->parents[i] = max_j;
        ret->peak_scores[i] = max_j >= 0 && ret->peak_scores[max_j] > max_f ? ret->peak_scores[max_j] : max_f;
	}
}

void chain_dp_reverse_25(call_t* a, return_t* ret, int64_t* num_anchor, int64_t* num_cell, int64_t* all_cell, int64_t* cycle)
{

	// TODO: make sure this works when n has more than 32 bits
	int64_t i, j;
    const int max_skip = 64;
    int max_dist_x = a->max_dist_x, max_dist_y = a->max_dist_y, bw = a->bw;
    float avg_qspan = a->avg_qspan;
    int64_t n = a->n;
	ret->n = n;
	*num_anchor += n;

	ret->scores.resize(n);
	ret->parents.resize(n);
    ret->targets.resize(n);
    ret->peak_scores.resize(n);

	int64_t ri, rj;
	int32_t qi, qj, dr, dq, l, end, flag;
	std::vector<int> max_index, score, q_span_array;
	for (i = 0; i < n; i++) {
		max_index.push_back(-1);
		q_span_array.push_back(a->anchors[i].y>>32 & 0xff);
		score.push_back(q_span_array[i]);
	}

	for (j = 0; j < n; ++j) {
		rj = a->anchors[j].x;
		qj = (int32_t)a->anchors[j].y;
		end = (j + max_skip) > n ? n : j + max_skip;
		for (i = j + 1; i <= end; i++) {
			*all_cell += 1;
			ri = a->anchors[i].x;
			qi = (int32_t)a->anchors[i].y;
			
			dr = (int32_t)ri - (int32_t)rj;
			dq = qi - qj;
			l = dr - dq > dq - dr ? dr - dq : dq - dr;

			flag = dr == 0 ? 1 : 0;
			flag = dq <= 0 ? 1 : flag;
			flag = dq > max_dist_y ? 1 : flag;
			flag = l > bw ? 1 : flag;

			int32_t sc;
			if (flag) continue;
			else sc = compute_sc(dr, dq, l, avg_qspan, q_span_array[i]);
			*num_cell += 1;

			sc += score[j];							// instruction 12.1:sub copy add	alpha beta score[j] null sc
			if (sc >= score[i]) {					// instruction 12.2:sub sub add		alpha beta score[j] score[i] sc_flag
				score[i] = sc, max_index[i] = j;	// instruction 13.1:mux 			sc sc_flag score[i] null score[i]
			}										// instruction 13.2:mux 			j sc_flag max_index[i] null max_index[i]
            if (max_index[j] >= 0) ret->targets[max_index[j]] = i;
													// instruction 14.1:mux				max_index[j] max_index[j] 0 null target_index
													// instruction 14.2:mux				i max_index[j] 0 null target_value
		}
	}

	for (j = 0; j < n; j++) {
		ret->scores[j] = score[j], ret->parents[j] = max_index[j];
		ret->peak_scores[j] = max_index[j] >= 0 && ret->peak_scores[max_index[j]] > score[j] ? ret->peak_scores[max_index[j]] : score[j];
		// end = (j + max_skip) > n ? n : j + max_skip;
		// for (i = j + 1; i <= end; i++) if (max_index[j] >= 0) ret->targets[max_index[j]] = i;
	}

	// free(score);
	// free(q_span_array);
	// free(max_index);
}


void chain_dp_accelerator(call_t* a, return_t* ret, int64_t* num_anchor, int64_t* num_cell, int64_t* all_cell, int64_t* cycle)
{

	// TODO: make sure this works when n has more than 32 bits
	int64_t i, j;
    const int max_skip = 64;
    int max_dist_x = a->max_dist_x, max_dist_y = a->max_dist_y, bw = a->bw, t;
    float avg_qspan = a->avg_qspan;
    int64_t n = a->n;
	ret->n = n;
	*num_anchor += n;

	ret->scores.resize(n);
	ret->parents.resize(n);
    ret->targets.resize(n);
    ret->peak_scores.resize(n);

	int64_t ri, rj;
	int32_t qi, qj, end, flag;
	int32_t dr, dq, l;
	std::vector<int> max_index, score, q_span_array;
	for (i = 0; i < n; i++) {
		max_index.push_back(-1);
		q_span_array.push_back(a->anchors[i].y>>32 & 0xff);
		score.push_back(q_span_array[i]);
	}

	int regfile[32];

	regfile[0] = 0;							// constant
	regfile[1] = 1;							// constant	
	regfile[2] = 16;						// constant
	regfile[21] = INT32_MIN;
	regfile[3] = max_dist_y;				// constant
	regfile[4] = bw;						// constant
	// regfile[5] = 0.01 * avg_qspan;		// constant
	// regfile[6] = qspan;					// constant

	// regfile[7] = dr;
	// regfile[8] = dq;
	// regfile[9] = l;
	// regfile[10] = flag;
	// regfile[11] = ri;
	// regfile[12] = qi;
	// regfile[13] = rj;	
	// regfile[14] = qj;
	// regfile[15] = score;
	// regfile[16] = parent;

	// regfile[17];	l_minus		f_dd	f_result	gap_cost	sc
	// regfile[18];	log_dd		alpha
	// regfile[19];	j
	// regfile[20];	score[j]

	for (j = 0; j < n; ++j) {
		rj = a->anchors[j].x;
		qj = (int32_t)a->anchors[j].y;
		end = (j + max_skip) > n ? n : j + max_skip;

		regfile[13] = (int32_t)rj;
		regfile[14] = qj;
		regfile[19] = j;
		regfile[20] = score[j];
		// printf("%d %d %d\n", regfile[13], regfile[14], q_span_array[j]);

		for (i = j + 1; i <= end; i++) {
			*all_cell += 1;
			ri = a->anchors[i].x;
			qi = (int32_t)a->anchors[i].y;

			regfile[6] = q_span_array[i];
			regfile[11] = (int32_t)ri;
			regfile[12] = qi;
			regfile[15] = score[i];
			regfile[16] = max_index[i];
																				// instruction 0

			regfile[7] = regfile[11] - regfile[13];								// instruction 1
			regfile[8] = regfile[12] - regfile[14];
			
			regfile[10] = regfile[7] == regfile[0] ? regfile[1] : regfile[0];	// instruction 2
			regfile[9] = (regfile[7] - regfile[8]) > (regfile[8] - regfile[7]) ? (regfile[7] - regfile[8]) : (regfile[8] - regfile[7]);	
			

			regfile[10] = regfile[8] > regfile[0] ? regfile[10] : regfile[1];	// instruction 3
			

			regfile[10] = regfile[8] > regfile[3] ? regfile[1] : regfile[10];	// instruction 4
			

			fixed f_dd = regfile[9];											// instruction 5
			regfile[10] = regfile[9] > regfile[4] ? regfile[1] : regfile[10];

			fixed f_tmp = 0.01 * avg_qspan;										// instruction 6
			fixed f_result = f_dd * f_tmp;
			regfile[18] = (regfile[9]? ilog2_32(regfile[9]) : regfile[0]) >> regfile[1];

			// printf("%x %f %f %d\n", regfile[9], 0.01 * avg_qspan * regfile[9], 0.01 * avg_qspan * regfile[9], (int)f_result.to_float());
			regfile[17] = (int)f_result.to_float() + regfile[18];				// instruction 7
			regfile[18] = (regfile[7] > regfile[8] ? regfile[8] : regfile[7]) > regfile[6] ? regfile[6] : (regfile[7] > regfile[8] ? regfile[8] : regfile[7]);

			regfile[17] = regfile[18] - regfile[17] + regfile[20];				// instruction 8

			regfile[17] = regfile[10] > 0 ? regfile[21] : regfile[17];

			regfile[15] = regfile[15] > regfile[17] ? regfile[15] : regfile[17];// instruction 9
			regfile[16] = regfile[15] > regfile[17] ? regfile[16] : regfile[19];

			
			// dr = (int32_t)ri - (int32_t)rj;
			// dq = qi - qj;
			// l = dr - dq > dq - dr ? dr - dq : dq - dr;
			// flag = dr == 0 ? 1 : 0;
			// flag = dq <= 0 ? 1 : flag;
			// flag = dq > max_dist_y ? 1 : flag;
			// flag = l > bw ? 1 : flag;

			int32_t sc;
			// if (flag) continue;
			// if (regfile[10]) continue;
			// sc = compute_sc(dr, dq, l, avg_qspan, q_span_array[i]);
			*num_cell += 1;

			// sc += score[j];		
			// sc = regfile[17];					// instruction 12.1:sub copy add	alpha beta score[j] null sc
			// if (sc >= score[i]) {					// instruction 12.2:sub sub add		alpha beta score[j] score[i] sc_flag
			// 	score[i] = sc, max_index[i] = j;	// instruction 13.1:mux 			sc sc_flag score[i] null score[i]
			// }										// instruction 13.2:mux 			j sc_flag max_index[i] null max_index[i]

			score[i] = regfile[15];
			max_index[i] = regfile[16];
			
		}
	}

	for (j = 0; j < n; j++) {
		ret->scores[j] = score[j], ret->parents[j] = max_index[j];
		ret->peak_scores[j] = max_index[j] >= 0 && ret->peak_scores[max_index[j]] > score[j] ? ret->peak_scores[max_index[j]] : score[j];
		// end = (j + max_skip) > n ? n : j + max_skip;
		// for (i = j + 1; i <= end; i++) if (max_index[j] >= 0) ret->targets[max_index[j]] = i;
	}

}


void chain_dp_instruction(call_t* a, return_t* ret, int64_t* num_anchor, int64_t* num_cell, int64_t* all_cell, int64_t* cycle, compute_unit_32* cu, comp_decoder* decoder_unit, unsigned long *instruction)
{

	// TODO: make sure this works when n has more than 32 bits
	int64_t i, j;
    const int max_skip = 64;
    int max_dist_x = a->max_dist_x, max_dist_y = a->max_dist_y, bw = a->bw, t;
    float avg_qspan = a->avg_qspan;
    int64_t n = a->n;
	ret->n = n;
	*num_anchor += n;

	ret->scores.resize(n);
	ret->parents.resize(n);
    ret->targets.resize(n);
    ret->peak_scores.resize(n);

	int64_t ri, rj;
	int32_t qi, qj, end, flag;
	int32_t dr, dq, l;
	std::vector<int> max_index, score, q_span_array;
	for (i = 0; i < n; i++) {
		max_index.push_back(-1);
		q_span_array.push_back(a->anchors[i].y>>32 & 0xff);
		score.push_back(q_span_array[i]);
	}

	int regfile[32];

	regfile[0] = 0;							// constant
	regfile[1] = 1;							// constant	
	regfile[2] = 16;						// constant
	regfile[21] = INT32_MIN;
	regfile[3] = max_dist_y;				// constant
	regfile[4] = bw;						// constant
	int avg_qspan_tmp = round(avg_qspan * 0.01 * (1<<16));		// constant
	unsigned int fractional_mask = (1 << 16) -1;	
	unsigned int integer_mask    = ~fractional_mask;
	regfile[5] = integer_mask & avg_qspan_tmp;
	regfile[22] = fractional_mask & avg_qspan_tmp;
	// regfile[6] = qspan;					// constant

	// regfile[7] = dr;
	// regfile[8] = dq;
	// regfile[9] = l;
	// regfile[10] = flag;
	// regfile[11] = ri;
	// regfile[12] = qi;
	// regfile[13] = rj;	
	// regfile[14] = qj;
	// regfile[15] = score;
	// regfile[16] = parent;

	// regfile[17];	l_minus		f_dd	f_result	gap_cost	sc
	// regfile[18];	log_dd		alpha
	// regfile[19];	j
	// regfile[20];	score[j]

	for (j = 0; j < n; ++j) {
		rj = a->anchors[j].x;
		qj = (int32_t)a->anchors[j].y;
		end = (j + max_skip) > n ? n : j + max_skip;

		regfile[13] = (int32_t)rj;
		regfile[14] = qj;
		regfile[19] = j;
		regfile[20] = score[j];
		// printf("%d %d %d\n", regfile[13], regfile[14], q_span_array[j]);

		for (i = j + 1; i <= end; i++) {
			// printf("j=%ld i=%ld\n", j, i);
			*all_cell += 1;
			ri = a->anchors[i].x;
			qi = (int32_t)a->anchors[i].y;

			regfile[6] = q_span_array[i];
			regfile[11] = (int32_t)ri;
			regfile[12] = qi;
			regfile[15] = score[i];
			regfile[16] = max_index[i];
																				// instruction 0

  			for (t = 0; t < 22; t++)
    			execute_instrution(t, instruction[t], decoder_unit, cu, regfile);

			// regfile[7] = regfile[11] - regfile[13];								// instruction 0
			// regfile[8] = regfile[12] - regfile[14];

			// regfile[10] = regfile[7] == regfile[0] ? regfile[1] : regfile[0];	// instruction 1
			// regfile[9] = (regfile[7] - regfile[8]) > (regfile[8] - regfile[7]) ? (regfile[7] - regfile[8]) : (regfile[8] - regfile[7]);	

			// regfile[17] = regfile[9] * regfile[5];
			// regfile[10] = regfile[8] > regfile[0] ? regfile[10] : regfile[1];	// instruction 2

			// regfile[23] = regfile[9] * regfile[22];								// instruction 3
			// regfile[10] = regfile[8] > regfile[3] ? regfile[1] : regfile[10];

			// regfile[17] = (regfile[17] << 16) + regfile[23];
			// regfile[10] = regfile[9] > regfile[4] ? regfile[1] : regfile[10];	// instruction 4

			// regfile[18] = (regfile[9]? ilog2_32(regfile[9]) : regfile[0]) >> regfile[1];		// instruction 5

			// // printf("%x %x %x %d\n", regfile[9], regfile[5], regfile[22], regfile[17] >> 16);
			// regfile[17] = (regfile[17] >> 16) + regfile[18];					// instruction 6
			// regfile[18] = (regfile[7] > regfile[8] ? regfile[8] : regfile[7]) > regfile[6] ? regfile[6] : (regfile[7] > regfile[8] ? regfile[8] : regfile[7]);

			// regfile[17] = regfile[18] + regfile[20] - regfile[17];				// instruction 7

			// regfile[17] = regfile[10] > 0 ? regfile[21] : regfile[17];			// instruction 8

			// regfile[15] = regfile[15] > regfile[17] ? regfile[15] : regfile[17];// instruction 9
			// regfile[16] = regfile[15] > regfile[17] ? regfile[16] : regfile[19];

  			// for (t = 6; t < 22; t++)
    		// 	execute_instrution(t, instruction[t], decoder_unit, cu, regfile);
			
			int32_t sc;

			*num_cell += 1;

			score[i] = regfile[15];
			max_index[i] = regfile[16];
			
		}
	}

	for (j = 0; j < n; j++) {
		ret->scores[j] = score[j], ret->parents[j] = max_index[j];
		ret->peak_scores[j] = max_index[j] >= 0 && ret->peak_scores[max_index[j]] > score[j] ? ret->peak_scores[max_index[j]] : score[j];
		// end = (j + max_skip) > n ? n : j + max_skip;
		// for (i = j + 1; i <= end; i++) if (max_index[j] >= 0) ret->targets[max_index[j]] = i;
	}

}

void host_chain_kernel(std::vector<call_t> &args, std::vector<return_t> &rets, int numThreads, int setting)
{
	int64_t total_num_anchor[numThreads * CLMUL], totalNumAnchor = 0;
	int64_t total_num_cell[numThreads * CLMUL], totalNumCell = 0;
	int64_t total_all_cell[numThreads * CLMUL], totalAllCell = 0;
	int64_t total_all_cycle[numThreads * CLMUL], totalAllCycle = 0;
	int i;

	compute_unit_32 cu;
  	comp_decoder decoder_unit;
	unsigned long *instruction;
  	instruction = (unsigned long*)malloc(64*sizeof(unsigned long));
	instruction[0] = 0x2f4ada000007;
	instruction[1] = 0x2f4b1c000008;
	instruction[2] = 0x2129d00020e9;
	instruction[3] = 0x1cf49c010000a;
	instruction[4] = 0x4f4a4a000011;
	instruction[5] = 0x1af4a00a0800a;
	instruction[6] = 0x4f4a6c000017;
	instruction[7] = 0x1af4a0615000a;
	instruction[8] = 0xe90440005c11;
	instruction[9] = 0x1af4a4815000a;
	instruction[10] = 0x1ef7800000000;
	instruction[11] = 0x16f4a40000012;
	instruction[12] = 0x1090440004811;
	instruction[13] = 0xc931d0001812;
	instruction[14] = 0x2904a2005011;
	instruction[15] = 0x1ef7800000000;
	instruction[16] = 0x1af4a81588011;
	instruction[17] = 0x1ef7800000000;
	instruction[18] = 0x1af4be2f8800f;
	instruction[19] = 0x1af4be3098010;
	instruction[20] = 0x1ef7800000000;
	instruction[21] = 0x1ef7800000000;
	instruction[22] = 0x20f7800000000;
	instruction[23] = 0x20f7800000000;
	for (i = 0; i < numThreads * CLMUL; i++) {
		total_num_anchor[i] = 0;
		total_num_cell[i] = 0;
		total_all_cell[i] = 0;
		total_all_cycle[i] = 0;
	}
	// printf("setting %d\n", setting);
	#pragma omp parallel num_threads(numThreads)
	{
		int tid = omp_get_thread_num();
		#pragma omp for schedule(dynamic, 1)
        for (size_t batch = 0; batch < args.size(); batch++) {
			int64_t num_anchor = 0;
			int64_t num_cell = 0;
			int64_t all_cell = 0;
			int64_t cycle = 0;
			call_t* arg = &args[batch];
			return_t* ret = &rets[batch];

			printf("setting: %d.\n", setting);
			// fprintf(stderr, "%lld\t%f\t%d\t%d\t%d\t%d\n", arg->n, arg->avg_qspan, arg->max_dist_x, arg->max_dist_y, arg->bw, arg->n_segs);
			if (setting == 0) chain_dp(arg, ret, &num_anchor, &num_cell, &all_cell, &cycle);
			if (setting == 1) chain_dp_copy(arg, ret, &num_anchor, &num_cell, &all_cell, &cycle);
			if (setting == 2) chain_dp_25(arg, ret, &num_anchor, &num_cell, &all_cell, &cycle);
			if (setting == 3) chain_dp_reverse_25(arg, ret, &num_anchor, &num_cell, &all_cell, &cycle);
			if (setting == 4) chain_dp_accelerator(arg, ret, &num_anchor, &num_cell, &all_cell, &cycle);
			if (setting == 5) chain_dp_instruction(arg, ret, &num_anchor, &num_cell, &all_cell, &cycle, &cu, &decoder_unit, instruction);
			total_num_anchor[tid * CLMUL] += num_anchor;
			total_num_cell[tid * CLMUL] += num_cell;
			total_all_cell[tid * CLMUL] += all_cell;
			total_all_cycle[tid * CLMUL] += cycle;
			// printf("Batch %d: Scan %ld Anchors\n", batch, num_anchor);
        }	
	}
	for (i = 0; i < numThreads; i++) {
		totalNumAnchor += total_num_anchor[i * CLMUL];
		totalNumCell += total_num_cell[i * CLMUL];
		totalAllCell += total_all_cell[i * CLMUL];
		totalAllCycle += total_all_cycle[i * CLMUL];
	}
    fprintf(stderr, "Total %ld Anchors\n", totalNumAnchor);
    fprintf(stderr, "Total %ld Cells\n", totalAllCell);
    fprintf(stderr, "Unskipped %ld Cells\n", totalNumCell);
    fprintf(stderr, "Total %ld Cycles\n", totalAllCycle);
	double GCPUS = totalAllCell / (totalAllCycle/50/0.75);
    fprintf(stderr, "GCUPS: %lf\n", GCPUS);
	
}
