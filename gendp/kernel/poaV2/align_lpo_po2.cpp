
#include "default.h"
#include "poa.h"
#include "seq_util.h"
#include "lpo.h"
#include <stdio.h>
#include "compute_unit_32.h"
#include "comp_decoder.h"
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>

/** set nonzero for old scoring (gap-opening penalty for X-Y transition) */
#define DOUBLE_GAP_SCORING (0)
#define MAX_LONG_DEPENDENCY 128
#define XL_INDEX_NUM 4096
#define XL_IPOS_NUM 4096


typedef struct {
  unsigned char x;
  unsigned char y;
}
DPMove_T;

typedef struct {
  LPOScore_T score;
  short gap_x, gap_y;
}
DPScore_T;

typedef struct {
  int predecessor[MAX_LONG_DEPENDENCY];
  int front, rear, count;
} queue;

void Q_init(queue *Q) {
  Q->front = 0;
  Q->rear = -1;
  Q->count = 0;
}

void enqueue(queue *Q, int data) {
  if (Q->count < MAX_LONG_DEPENDENCY) Q->count++;
  else fprintf(stderr, "Full queue.\n");
  if (Q->rear == MAX_LONG_DEPENDENCY - 1) Q->rear = -1;
  Q->predecessor[++(Q->rear)] = data;
}

int dequeue(queue *Q) {
  if (Q->count == 0) {
    fprintf(stderr, "Enpty queue.\n");
    return 0;
  } else Q->count--;
  int data = Q->predecessor[(Q->front)++];
  if (Q->front == MAX_LONG_DEPENDENCY) Q->front = 0;
  return data;
}


#define LPO_INITIAL_NODE 1
#define LPO_FINAL_NODE 2

static void get_lpo_stats (LPOSequence_T *lposeq,
			   int *n_nodes_ptr, int *n_edges_ptr, int **node_type_ptr,
			   int **refs_from_right_ptr, int *max_rows_alloced_ptr,
			   LPOLetterLink_T ***left_links_ptr)
{
  int i, j, rows_alloced = 0, max_rows_alloced = 0, n_edges = 0, len = lposeq->length;
  LPOLetter_T *seq = lposeq->letter;
  int *node_type;
  int *refs_from_right, *tmp;
  LPOLetterSource_T *src;
  LPOLetterLink_T *lnk, **left_links;
  
  CALLOC (node_type, len, int);
  CALLOC (refs_from_right, len, int);
  CALLOC (tmp, len, int);
  CALLOC (left_links, len, LPOLetterLink_T *);
    
  for (i=0; i<len; i++) {

    /* NODES CONTAINING THE FIRST RESIDUE IN ANY SEQ ARE 'INITIAL'; */
    /* DITTO, LAST RESIDUE IN ANY SEQ, 'FINAL'. */
    for (src = &(seq[i].source); src != NULL && src->iseq >= 0; src = src->more) {
      if (src->ipos == 0) {
	      node_type[i] = (node_type[i] | LPO_INITIAL_NODE);
      }
      if (src->ipos == (lposeq->source_seq[src->iseq]).length - 1) {
	      node_type[i] = (node_type[i] | LPO_FINAL_NODE);
      }
    }

    /* COUNTING THE LEFT-LINKS BACK TO EACH NODE ALLOWS FOR EFFICIENT */
    /* MEMORY MANAGEMENT OF 'SCORE' ROWS (in align_lpo_po). */
    for (lnk = &(seq[i].left); lnk != NULL && lnk->ipos >= 0; lnk = lnk->more) {
      refs_from_right[lnk->ipos]++;
      // if (refs_from_right[lnk->ipos] > 1)
      //   printf("!");
      n_edges++;
    }
  }

  /* ALL 'INITIAL' NODES (1st in some seq) MUST BE LEFT-LINKED TO -1. */
  /* THIS ALLOWS FREE ALIGNMENT TO ANY 'BRANCH' IN GLOBAL ALIGNMENT. */
  for (i=0; i<len; i++) {
    if ((node_type[i] & LPO_INITIAL_NODE) && seq[i].left.ipos != -1) {
      CALLOC (left_links[i], 1, LPOLetterLink_T);
      left_links[i]->ipos = -1;
      left_links[i]->score = 0;
      left_links[i]->more = &seq[i].left;
    }
    else {
      left_links[i] = &seq[i].left;
    }
  }
  
  for (i=0; i<len; i++) {
    tmp[i] = refs_from_right[i];
  }
  
  for (i=0; i<len; i++) {
    rows_alloced++;
    if (rows_alloced > max_rows_alloced) {
      max_rows_alloced = rows_alloced;
    }
    for (lnk = &(seq[i].left); lnk != NULL && lnk->ipos >= 0; lnk = lnk->more) {
      if ((--tmp[lnk->ipos]) == 0) {
	      rows_alloced--;
      }
    }
  }

  FREE (tmp);

  (*n_nodes_ptr) = len;
  (*n_edges_ptr) = n_edges;
  (*node_type_ptr) = node_type;
  (*refs_from_right_ptr) = refs_from_right;
  (*max_rows_alloced_ptr) = max_rows_alloced;
  (*left_links_ptr) = left_links;
}


static void trace_back_lpo_alignment (int len_x, int len_y,
				      DPMove_T **move,
				      LPOLetterLink_T **x_left,
				      LPOLetterLink_T **y_left,
				      LPOLetterRef_T best_x, LPOLetterRef_T best_y,
				      LPOLetterRef_T **x_to_y,
				      LPOLetterRef_T **y_to_x)
{
  int i, xmove, ymove;
  LPOLetterRef_T *x_al = NULL, *y_al = NULL;
  LPOLetterLink_T *left;
  
  CALLOC (x_al, len_x, LPOLetterRef_T);
  CALLOC (y_al, len_y, LPOLetterRef_T);
  LOOP (i,len_x) x_al[i] = INVALID_LETTER_POSITION;
  LOOP (i,len_y) y_al[i] = INVALID_LETTER_POSITION;
  
  while (best_x >= 0 && best_y >= 0) {

    xmove = move[best_y][best_x].x;
    ymove = move[best_y][best_x].y;
    
    if (xmove>0 && ymove>0) { /* ALIGNED! MAP best_x <--> best_y */
      x_al[best_x]=best_y;
      y_al[best_y]=best_x;
    }

    if (xmove == 0 && ymove == 0) { /* FIRST ALIGNED PAIR */
      x_al[best_x]=best_y;
      y_al[best_y]=best_x;
      break;  /* FOUND START OF ALIGNED REGION, SO WE'RE DONE */
    }
    
    if (xmove>0) { /* TRACE BACK ON X */
      left = x_left[best_x];
      while ((--xmove)>0) {
	left = left->more;
      }
      best_x = left->ipos;
    }
    
    if (ymove>0) { /* TRACE BACK ON Y */
      left = y_left[best_y];
      while ((--ymove)>0) {
	left = left->more;
      }
      best_y = left->ipos;
    }
  }
  
  if (x_to_y) /* HAND BACK ALIGNMENT RECIPROCAL MAPPINGS */
    *x_to_y = x_al;
  else
    free(x_al);
  if (y_to_x)
    *y_to_x = y_al;
  else
    free(y_al);
  
  return;
}


/** (align_lpo_po:)
    performs partial order alignment:
    lposeq_x and lposeq_y are partial orders;
    returns the alignment in x_to_y[] and y_to_x[], and also 
    returns the alignment score as the return value.
*/


int accelerator_queue(int i, int j, queue *Q_left_gap, queue *Q_left_score, queue *Q_diag_score, int *xl_index, int *xl_ipos, LPOLetter_T *seq_x, LPOLetter_T *seq_y, int len_x, int *node_type_x, int *node_type_y, int *best_x, int *best_y, LPOScore_T *min_score, LPOScore_T *best_score, LPOLetterLink_T **x_left, LPOLetterLink_T **y_left, LPOLetterLink_T *xl, LPOLetterLink_T *yl, DPMove_T **move, DPMove_T *my_move, int possible_end_square, DPScore_T **score_rows, DPScore_T *curr_score, DPScore_T *prev_score, DPScore_T *my_score, LPOScore_T *gap_penalty_x, LPOScore_T *gap_penalty_y, int *next_gap_array, int *next_perp_gap_array, int pre_gap[MAX_LONG_DEPENDENCY], int use_global_alignment, LPOScore_T (*scoring_function)(int, int, LPOLetter_T *, LPOLetter_T *, ResidueScoreMatrix_T *), ResidueScoreMatrix_T *m, int max_gap_length, int *max_gap, int *max_dependency){

  int xcount, ycount, prev_gap, gap_penalty, next_gap_array_x, next_gap_array_y, max_flag, long_dependency, insert_y_score_tmp, insert_x_score_tmp, match_score_tmp, my_score_tmp, my_score_gap_tmp, my_move_x_tmp, my_move_y_tmp, best_score_tmp, dependency_num = -1, cycle = 0, try_score_xy, try_score_match, insert_x_score, insert_y_score, match_score, insert_x_x, insert_x_gap, insert_y_y, insert_y_gap, match_x, match_y;

  int regfile[64], k, queue_front, predecessor_gap, queue_index, new_score;

  prev_score = score_rows[y_left[i]->ipos];

  // static register
  regfile[0] = 0;                                         // constant 0
  regfile[1] = prev_score[j].gap_y;                       // upper/current gap_y    (from last PE this iteration)
  regfile[2] = prev_score[j].score;                       // upper score            (from last PE this iteration)
  regfile[3];                                             // diag score             (from SPM)
  regfile[4];                                             // left/current gap_x     (from SPM)
  regfile[5];                                             // left/current score     (from SPM)
  regfile[6] = *best_score;                               // *best_score            (update in each PE)
  regfile[7] = *best_x;                                   // *best_x                (update in each PE)
  regfile[8] = *best_y;                                   // *best_y                (update in each PE)
  regfile[9] = j;                                         // j                      (initialize from static input buffer & update in each PE)
  regfile[10] = seq_y[i].letter;                          // static base - sequence (initialize from static input buffer)
  regfile[11] = i;                                        // i                      (initialize from static input buffer)
  regfile[12] = seq_x[j].letter;                          // dynamic base - graph   (from last PE this iteration)

  regfile[13] = 6;                                        // constant 6             (initialize from static input buffer)
  regfile[14] = 12;                                       // constant 12             (initialize from static input buffer)
  regfile[15] = 1;                                        // constant 1             (initialize from static input buffer)
  regfile[16] = 16;                                       // constant 16            (initialize from static input buffer)
  regfile[17] = 17;                                       // constant 17            (initialize from static input buffer)
  regfile[18] = -999999;                                  // constant min_score     (initialize from static input buffer)

  // temp register
  regfile[19];                                            // xcount                 new_score         my_score_tmp 
  regfile[20];                                            // gap_penalty            next_gap_array_y  my_score_gap_tmp
  regfile[21];                                            // try_score_xy           next_gap_array_x  my_move_y_tmp
  regfile[22];                                            // insert_y_gap                             my_move_x_tmp
  regfile[23];                                            // insert_y_score                           move[i][j].y
  regfile[24];                                            // insert_y_y                               move[i][j].x
  regfile[25];                                            // insert_x_gap
  regfile[26];                                            // insert_x_score
  regfile[27];                                            // insert_x_x
  regfile[28];                                            // match_score
  regfile[29];                                            // match_x
  regfile[30];                                            // match_y

  // Initialization
  xcount = 0;
  match_score = 0;                        // Initialization
  match_x = 0;
  match_y = 0;
  insert_y_score = *min_score;
  insert_y_y = 0;
  insert_y_gap = 0;
  insert_x_score = *min_score;
  insert_x_x = 0;
  insert_x_gap = 0;

  // Update y
  gap_penalty = (prev_score[j].gap_y == 0) ? gap_penalty_y[0]: gap_penalty_y[1];
  gap_penalty = (prev_score[j].gap_y == 17) ? 0 : gap_penalty;
  try_score_xy = prev_score[j].score - gap_penalty;
  insert_y_score = (try_score_xy > *min_score) ? try_score_xy : *min_score;
  insert_y_y = (try_score_xy > *min_score) ? 1 : 0;
  insert_y_gap = (try_score_xy > *min_score) ? prev_score[j].gap_y : 0;

  xl = x_left[j];

  /* LOOP OVER x-predecessors (OUTSIDE y-predecessor LOOP): */
  for (k = 0; k < xl_index[j+1] - xl_index[j]; k++) {
    
    xcount = xcount + 1;

    predecessor_gap = j - (xl_ipos[xl_index[j]+k]);
    // if (predecessor_gap >= MAX_LONG_DEPENDENCY) return cycle;
    queue_index = Q_left_gap->rear + 1 - predecessor_gap;
    if (queue_index < 0) queue_index += MAX_LONG_DEPENDENCY;

    // if (curr_score[xl_ipos[xl_index[j]+k]].gap_x != Q_left_gap->predecessor[queue_index])
    //   printf("left_gap: %d %d %d\n", curr_score[xl_ipos[xl_index[j]+k]].gap_x, Q_left_gap->predecessor[queue_index], predecessor_gap);
    // if (curr_score[xl_ipos[xl_index[j]+k]].score != Q_left_score->predecessor[queue_index])
    //   printf("left_score: %d %d %d\n", curr_score[xl_ipos[xl_index[j]+k]].score, Q_left_score->predecessor[queue_index], predecessor_gap);
    // if (prev_score[xl_ipos[xl_index[j]+k]].score != Q_diag_score->predecessor[queue_index])
    //   printf("diag_score: %d %d %d\n", prev_score[xl_ipos[xl_index[j]+k]].score, Q_diag_score->predecessor[queue_index], predecessor_gap);

    long_dependency = j - xl->ipos;
    if (long_dependency < MAX_LONG_DEPENDENCY) pre_gap[long_dependency]++;
    if (*max_gap < long_dependency) *max_gap = long_dependency;
    
    gap_penalty = (curr_score[xl_ipos[xl_index[j]+k]].gap_x == 0) ? gap_penalty_y[0]: gap_penalty_y[1];
    cycle++;  // instruction 7x
    gap_penalty = (curr_score[xl_ipos[xl_index[j]+k]].gap_x == 17) ? 0 : gap_penalty;

    try_score_xy = curr_score[xl_ipos[xl_index[j]+k]].score - gap_penalty;

    insert_x_x = try_score_xy > insert_x_score ? xcount : insert_x_x;
    match_x = prev_score[xl_ipos[xl_index[j]+k]].score > match_score ? xcount : match_x;

    insert_x_gap = try_score_xy > insert_x_score ? curr_score[xl_ipos[xl_index[j]+k]].gap_x : insert_x_gap;
    match_y = prev_score[xl_ipos[xl_index[j]+k]].score > match_score ? 1 : match_y;

    insert_x_score = try_score_xy > insert_x_score ? try_score_xy : insert_x_score;
    match_score = prev_score[xl_ipos[xl_index[j]+k]].score > match_score ? prev_score[xl_ipos[xl_index[j]+k]].score : match_score;
  }
  if (dependency_num > *max_dependency) *max_dependency = dependency_num;
  
  new_score = m->score[seq_x[j].letter][seq_y[i].letter];
  match_score = match_score + new_score;
  
  next_gap_array_x = insert_x_gap + 1;
  next_gap_array_y = insert_y_gap + 1;
  next_gap_array_x = (16 > insert_x_gap) ? next_gap_array_x : insert_x_gap;
  next_gap_array_y = (16 > insert_y_gap) ? next_gap_array_y : insert_y_gap;
  
  my_score_tmp = insert_x_score > insert_y_score ? insert_x_score : insert_y_score;
  my_score_gap_tmp = insert_x_score > insert_y_score ? next_gap_array_x : next_gap_array_y;
  my_move_x_tmp = insert_x_score > insert_y_score ? insert_x_x : 0;
  my_move_y_tmp = insert_x_score > insert_y_score ? 0 : insert_y_y;


  curr_score[j].score = match_score > my_score_tmp ? match_score : my_score_tmp;
  curr_score[j].gap_x = match_score > my_score_tmp ? 0 : my_score_gap_tmp;
  curr_score[j].gap_y = match_score > my_score_tmp ? 0 : my_score_gap_tmp;

  (&move[i][j])->x = match_score > my_score_tmp ? match_x : my_move_x_tmp;
  (&move[i][j])->y = match_score > my_score_tmp ? match_y : my_move_y_tmp;

  *best_x = curr_score[j].score > *best_score ? j : *best_x;
  *best_y = curr_score[j].score > *best_score ? i : *best_y;
  *best_score = curr_score[j].score > *best_score ? curr_score[j].score : *best_score;

  if (Q_left_gap->count == MAX_LONG_DEPENDENCY)
    queue_front = dequeue(Q_left_gap);
  enqueue(Q_left_gap, curr_score[j].gap_x);
  if (Q_left_score->count == MAX_LONG_DEPENDENCY)
    queue_front = dequeue(Q_left_score);
  enqueue(Q_left_score, curr_score[j].score);
  if (Q_diag_score->count == MAX_LONG_DEPENDENCY)
    queue_front = dequeue(Q_diag_score);
  enqueue(Q_diag_score, prev_score[j].score);


  return cycle;
}

int accelerator_predecessor(int i, int j, int *xl_index, int *xl_ipos, LPOLetter_T *seq_x, LPOLetter_T *seq_y, int len_x, int *node_type_x, int *node_type_y, int *best_x, int *best_y, LPOScore_T *min_score, LPOScore_T *best_score, LPOLetterLink_T **x_left, LPOLetterLink_T **y_left, LPOLetterLink_T *xl, LPOLetterLink_T *yl, DPMove_T **move, DPMove_T *my_move, int possible_end_square, DPScore_T **score_rows, DPScore_T *curr_score, DPScore_T *prev_score, DPScore_T *my_score, LPOScore_T *gap_penalty_x, LPOScore_T *gap_penalty_y, int *next_gap_array, int *next_perp_gap_array, int pre_gap[MAX_LONG_DEPENDENCY], int use_global_alignment, LPOScore_T (*scoring_function)(int, int, LPOLetter_T *, LPOLetter_T *, ResidueScoreMatrix_T *), ResidueScoreMatrix_T *m, int max_gap_length, int *max_gap, int *max_dependency){

  int xcount, ycount, prev_gap, gap_penalty, next_gap_array_x, next_gap_array_y, max_flag, long_dependency, insert_y_score_tmp, insert_x_score_tmp, match_score_tmp, my_score_tmp, my_score_gap_tmp, my_move_x_tmp, my_move_y_tmp, best_score_tmp, dependency_num = -1, cycle = 0, try_score_xy, try_score_match, insert_x_score, insert_y_score, match_score, insert_x_x, insert_x_gap, insert_y_y, insert_y_gap, match_x, match_y;

  int regfile[64], k;

  match_score = 0;                        // Initialization
  match_x = match_y = 0;
  
  insert_x_score = *min_score;
  insert_x_x = 0;
  insert_x_gap = 0;
  
  prev_score = score_rows[y_left[i]->ipos];

  // Update y
  gap_penalty = (prev_score[j].gap_y == 0) ? gap_penalty_y[0]: gap_penalty_y[1];
  // node  0:       comp ==
  gap_penalty = (prev_score[j].gap_y == 17) ? 0 : gap_penalty;
  // node  1 <- 0:  comp ==
  try_score_xy = prev_score[j].score +  y_left[i]->score - gap_penalty;
  // node  2 <- 1:  -
  // node  3 <- 2:  +
  insert_y_score = (try_score_xy > *min_score) ? try_score_xy : *min_score;
  // node  4 <- 3:  comp >
  insert_y_y = (try_score_xy > *min_score) ? 1 : 0;
  // node  5 <- 3:  comp >
  insert_y_gap = (try_score_xy > *min_score) ? prev_score[j].gap_y : 0;
  // node  6 <- 3:  comp >

  // Update x
  gap_penalty = (curr_score[x_left[j]->ipos].gap_x == 0) ? gap_penalty_y[0]: gap_penalty_y[1];
  // node  7:       comp ==
  gap_penalty = (curr_score[x_left[j]->ipos].gap_x == 17) ? 0 : gap_penalty;
  // node  8 <- 7:  comp ==
  try_score_xy = curr_score[x_left[j]->ipos].score + x_left[j]->score - gap_penalty;
  // node  9 <- 8:  -
  // node 10 <- 9:  +  
  insert_x_score = try_score_xy > *min_score ? try_score_xy : *min_score;
  // node 11 <-10:   comp >   
  insert_x_x = try_score_xy > *min_score ? 1 : 0;
  // node 12 <-10:   comp > 
  insert_x_gap = try_score_xy > *min_score ? curr_score[x_left[j]->ipos].gap_x : 0;
  // node 13 <-10:   comp >

  // Update match
  try_score_match = prev_score[x_left[j]->ipos].score + x_left[j]->score + y_left[i]->score;
  // node 14:       +
  // node 15 <-14:  +
  match_score = try_score_match > 0 ? try_score_match : 0;
  // node 16 <-15:   comp > 
  match_x = try_score_match > 0 ? 1 : 0;
  // node 17 <-15:   comp >
  match_y = try_score_match > 0 ? 1 : 0;
  // node 18 <-15:   comp >

  xl = x_left[j];
  xcount = 1;
  /* LOOP OVER x-predecessors (OUTSIDE y-predecessor LOOP): */
  // for (xcount = 1, xl = x_left[j]->more; xl != NULL; xl = xl->more) {
  for (k = 0; k < xl_index[j+1] - xl_index[j]; k++) {
    
    xcount = xcount + 1;
    gap_penalty = (curr_score[xl_ipos[xl_index[j]+k]].gap_x == 0) ? gap_penalty_y[0]: gap_penalty_y[1];
    // node  7:       comp ==     dependency_regfile[4*k+1] 0 gap_penalty_x[0] gap_penalty_x[1] gap_penalty
    cycle++;  // instruction 7x
    gap_penalty = (curr_score[xl_ipos[xl_index[j]+k]].gap_x == 17) ? 0 : gap_penalty;
    // node  8 <- 7:  comp ==     dependency_regfile[4*k+1] 17 gap_penalty_x[17] gap_penalty gap_penalty

    try_score_xy = curr_score[xl_ipos[xl_index[j]+k]].score - gap_penalty;
    // node  9 <- 8:  -
    // node 10 <- 9:  +
    try_score_match = (score_rows[y_left[i]->ipos])[xl_ipos[xl_index[j]+k]].score + y_left[i]->score;
    // node 14:       +
    // node 15 <-14:  +

    insert_x_score_tmp = insert_x_score;
    match_score_tmp = match_score;

    insert_x_score = try_score_xy > insert_x_score ? try_score_xy : insert_x_score;
    // node 11 <-10:   comp > 
    match_score = try_score_match > match_score ? try_score_match : match_score;
    // node 16 <-15:   comp > 
    insert_x_x = try_score_xy > insert_x_score_tmp ? xcount : insert_x_x;
    // node 12 <-10:   comp > 
    match_x = try_score_match > match_score_tmp ? xcount : match_x;
    // node 17 <-15:   comp >

    insert_x_gap = try_score_xy > insert_x_score_tmp ? curr_score[xl_ipos[xl_index[j]+k]].gap_x : insert_x_gap;
    // node 13 <-10:   comp >
    match_y = try_score_match > match_score_tmp ? 1 : match_y;
    // node 18 <-15:   comp >

  }
  if (dependency_num > *max_dependency) *max_dependency = dependency_num;
  
  match_score += m->score[seq_x[j].letter][seq_y[i].letter];
  
  next_gap_array_x = insert_x_gap + 1;
  // node 21 <-13:    copy copy +   insert_x_gap null 1 null next_gap_array_x
  next_gap_array_y = insert_y_gap + 1;
  // node 22 <- 6:  +   insert_y_gap null 1 null next_gap_array_y
  next_gap_array_x = (16 > insert_x_gap) ? next_gap_array_x : insert_x_gap;
  // node 23 <-21:    comp <        insert_x_gap 16 next_gap_array_x 16 next_gap_array_x
  next_gap_array_y = (16 > insert_y_gap) ? next_gap_array_y : insert_y_gap;
  // node 24 <-22:    comp <        insert_y_gap 16 next_gap_array_y 16 next_gap_array_y
  my_score_tmp = insert_x_score > insert_y_score ? insert_x_score : insert_y_score;
  // node 25 <-11,4:  comp >
  my_score_gap_tmp = insert_x_score > insert_y_score ? next_gap_array_x : next_gap_array_y;
  // node 26 <-11,4:  comp >
  my_move_x_tmp = insert_x_score > insert_y_score ? insert_x_x : 0;
  // node 27 <-11,4:  comp >
  my_move_y_tmp = insert_x_score > insert_y_score ? 0 : insert_y_y;
  // node 28 <-11,4:  comp >


  curr_score[j].score = match_score > my_score_tmp ? match_score : my_score_tmp;
  // node 29 <-25:    comp >
  curr_score[j].gap_x = match_score > my_score_tmp ? 0 : my_score_gap_tmp;
  curr_score[j].gap_y = match_score > my_score_tmp ? 0 : my_score_gap_tmp;
  // node 30 <-25,26: comp >

  (&move[i][j])->x = match_score > my_score_tmp ? match_x : my_move_x_tmp;
  // node 31 <-25,27: comp >
  (&move[i][j])->y = match_score > my_score_tmp ? match_y : my_move_y_tmp;
  // node 32 <-25,28: comp >

  best_score_tmp = *best_score;
  *best_score = curr_score[j].score > *best_score ? curr_score[j].score : *best_score;
  // node 33 <-29:    comp >
  *best_x = curr_score[j].score > best_score_tmp ? j : *best_x;
  // node 34 <-29:    comp >
  *best_y = curr_score[j].score > best_score_tmp ? i : *best_y;
  // node 35 <-29:    comp >

  return cycle;
}

int accelerator(int i, int j, int num_long_dependency, int dependency_regfile[14*4], LPOLetter_T *seq_x, LPOLetter_T *seq_y, int len_x, int *node_type_x, int *node_type_y, int *best_x, int *best_y, LPOScore_T *min_score, LPOScore_T *best_score, LPOLetterLink_T **x_left, LPOLetterLink_T **y_left, LPOLetterLink_T *xl, LPOLetterLink_T *yl, DPMove_T **move, DPMove_T *my_move, int possible_end_square, DPScore_T **score_rows, DPScore_T *curr_score, DPScore_T *prev_score, DPScore_T *my_score, LPOScore_T *gap_penalty_x, LPOScore_T *gap_penalty_y, int *next_gap_array, int *next_perp_gap_array, int pre_gap[MAX_LONG_DEPENDENCY], int use_global_alignment, LPOScore_T (*scoring_function)(int, int, LPOLetter_T *, LPOLetter_T *, ResidueScoreMatrix_T *), ResidueScoreMatrix_T *m, int max_gap_length, int *max_gap, int *max_dependency){

  int xcount, ycount, prev_gap, gap_penalty, next_gap_array_x, next_gap_array_y, max_flag, long_dependency, insert_y_score_tmp, insert_x_score_tmp, match_score_tmp, my_score_tmp, my_score_gap_tmp, my_move_x_tmp, my_move_y_tmp, best_score_tmp, dependency_num = -1, cycle = 0, try_score_xy, try_score_match, insert_x_score, insert_y_score, match_score, insert_x_x, insert_x_gap, insert_y_y, insert_y_gap, match_x, match_y;

  int regfile[64], k;

  match_score = 0;                        // Initialization
  match_x = match_y = 0;
  
  insert_x_score = *min_score;
  insert_x_x = 0;
  insert_x_gap = 0;
  
  prev_score = score_rows[y_left[i]->ipos];

  // Update y
  gap_penalty = (prev_score[j].gap_y == 0) ? gap_penalty_y[0]: gap_penalty_y[1];
  // node  0:       comp ==
  gap_penalty = (prev_score[j].gap_y == 17) ? 0 : gap_penalty;
  // node  1 <- 0:  comp ==
  try_score_xy = prev_score[j].score +  y_left[i]->score - gap_penalty;
  // node  2 <- 1:  -
  // node  3 <- 2:  +
  insert_y_score = (try_score_xy > *min_score) ? try_score_xy : *min_score;
  // node  4 <- 3:  comp >
  insert_y_y = (try_score_xy > *min_score) ? 1 : 0;
  // node  5 <- 3:  comp >
  insert_y_gap = (try_score_xy > *min_score) ? prev_score[j].gap_y : 0;
  // node  6 <- 3:  comp >

  // Update x
  gap_penalty = (curr_score[x_left[j]->ipos].gap_x == 0) ? gap_penalty_y[0]: gap_penalty_y[1];
  // node  7:       comp ==
  gap_penalty = (curr_score[x_left[j]->ipos].gap_x == 17) ? 0 : gap_penalty;
  // node  8 <- 7:  comp ==
  try_score_xy = curr_score[x_left[j]->ipos].score + x_left[j]->score - gap_penalty;
  // node  9 <- 8:  -
  // node 10 <- 9:  +  
  insert_x_score = try_score_xy > *min_score ? try_score_xy : *min_score;
  // node 11 <-10:   comp >   
  insert_x_x = try_score_xy > *min_score ? 1 : 0;
  // node 12 <-10:   comp > 
  insert_x_gap = try_score_xy > *min_score ? curr_score[x_left[j]->ipos].gap_x : 0;
  // node 13 <-10:   comp >

  // Update match
  try_score_match = prev_score[x_left[j]->ipos].score + x_left[j]->score + y_left[i]->score;
  // node 14:       +
  // node 15 <-14:  +
  match_score = try_score_match > 0 ? try_score_match : 0;
  // node 16 <-15:   comp > 
  match_x = try_score_match > 0 ? 1 : 0;
  // node 17 <-15:   comp >
  match_y = try_score_match > 0 ? 1 : 0;
  // node 18 <-15:   comp >

  xl = x_left[j];
  xcount = 1;
  /* LOOP OVER x-predecessors (OUTSIDE y-predecessor LOOP): */
  // for (xcount = 1, xl = x_left[j]->more; xl != NULL; xl = xl->more) {
  for (k = 1; k < num_long_dependency; k++) {

    long_dependency = j - xl->ipos;
    if (long_dependency < MAX_LONG_DEPENDENCY) pre_gap[long_dependency]++;
    if (*max_gap < long_dependency) *max_gap = long_dependency;

    xcount = xcount + 1;
    gap_penalty = (dependency_regfile[4*k+1] == 0) ? gap_penalty_y[0]: gap_penalty_y[1];
    // node  7:       comp ==     dependency_regfile[4*k+1] 0 gap_penalty_x[0] gap_penalty_x[1] gap_penalty
    cycle++;  // instruction 7x
    gap_penalty = (dependency_regfile[4*k+1] == 17) ? 0 : gap_penalty;
    // node  8 <- 7:  comp ==     dependency_regfile[4*k+1] 17 gap_penalty_x[17] gap_penalty gap_penalty

    try_score_xy = dependency_regfile[4*k+2] + dependency_regfile[4*k+3] - gap_penalty;
    // node  9 <- 8:  -
    // node 10 <- 9:  +
    try_score_match = dependency_regfile[4*k+0] + dependency_regfile[4*k+3] + y_left[i]->score;
    // node 14:       +
    // node 15 <-14:  +

    insert_x_score_tmp = insert_x_score;
    match_score_tmp = match_score;

    insert_x_score = try_score_xy > insert_x_score ? try_score_xy : insert_x_score;
    // node 11 <-10:   comp > 
    match_score = try_score_match > match_score ? try_score_match : match_score;
    // node 16 <-15:   comp > 
    insert_x_x = try_score_xy > insert_x_score_tmp ? xcount : insert_x_x;
    // node 12 <-10:   comp > 
    match_x = try_score_match > match_score_tmp ? xcount : match_x;
    // node 17 <-15:   comp >

    insert_x_gap = try_score_xy > insert_x_score_tmp ? dependency_regfile[4*k+1] : insert_x_gap;
    // node 13 <-10:   comp >
    match_y = try_score_match > match_score_tmp ? 1 : match_y;
    // node 18 <-15:   comp >

  }
  if (dependency_num > *max_dependency) *max_dependency = dependency_num;
  
  match_score += m->score[seq_x[j].letter][seq_y[i].letter];
  
  next_gap_array_x = insert_x_gap + 1;
  // node 21 <-13:    copy copy +   insert_x_gap null 1 null next_gap_array_x
  next_gap_array_y = insert_y_gap + 1;
  // node 22 <- 6:  +   insert_y_gap null 1 null next_gap_array_y
  next_gap_array_x = (16 > insert_x_gap) ? next_gap_array_x : insert_x_gap;
  // node 23 <-21:    comp <        insert_x_gap 16 next_gap_array_x 16 next_gap_array_x
  next_gap_array_y = (16 > insert_y_gap) ? next_gap_array_y : insert_y_gap;
  // node 24 <-22:    comp <        insert_y_gap 16 next_gap_array_y 16 next_gap_array_y
  my_score_tmp = insert_x_score > insert_y_score ? insert_x_score : insert_y_score;
  // node 25 <-11,4:  comp >
  my_score_gap_tmp = insert_x_score > insert_y_score ? next_gap_array_x : next_gap_array_y;
  // node 26 <-11,4:  comp >
  my_move_x_tmp = insert_x_score > insert_y_score ? insert_x_x : 0;
  // node 27 <-11,4:  comp >
  my_move_y_tmp = insert_x_score > insert_y_score ? 0 : insert_y_y;
  // node 28 <-11,4:  comp >


  curr_score[j].score = match_score > my_score_tmp ? match_score : my_score_tmp;
  // node 29 <-25:    comp >
  curr_score[j].gap_x = match_score > my_score_tmp ? 0 : my_score_gap_tmp;
  curr_score[j].gap_y = match_score > my_score_tmp ? 0 : my_score_gap_tmp;
  // node 30 <-25,26: comp >

  (&move[i][j])->x = match_score > my_score_tmp ? match_x : my_move_x_tmp;
  // node 31 <-25,27: comp >
  (&move[i][j])->y = match_score > my_score_tmp ? match_y : my_move_y_tmp;
  // node 32 <-25,28: comp >

  best_score_tmp = *best_score;
  *best_score = curr_score[j].score > *best_score ? curr_score[j].score : *best_score;
  // node 33 <-29:    comp >
  *best_x = curr_score[j].score > best_score_tmp ? j : *best_x;
  // node 34 <-29:    comp >
  *best_y = curr_score[j].score > best_score_tmp ? i : *best_y;
  // node 35 <-29:    comp >

  return cycle;
}

int accelerator_algorithm(int i, int j, LPOLetter_T *seq_x, LPOLetter_T *seq_y, int len_x, int *node_type_x, int *node_type_y, int *best_x, int *best_y, LPOScore_T *min_score, LPOScore_T *best_score, LPOLetterLink_T **x_left, LPOLetterLink_T **y_left, LPOLetterLink_T *xl, LPOLetterLink_T *yl, DPMove_T **move, DPMove_T *my_move, int possible_end_square, DPScore_T **score_rows, DPScore_T *curr_score, DPScore_T *prev_score, DPScore_T *my_score, LPOScore_T *gap_penalty_x, LPOScore_T *gap_penalty_y, int *next_gap_array, int *next_perp_gap_array, int pre_gap[MAX_LONG_DEPENDENCY], int use_global_alignment, LPOScore_T (*scoring_function)(int, int, LPOLetter_T *, LPOLetter_T *, ResidueScoreMatrix_T *), ResidueScoreMatrix_T *m, int max_gap_length, int *max_gap, int *max_dependency){

  int xcount, ycount, prev_gap, gap_penalty, next_gap_array_x, next_gap_array_y, max_flag, long_dependency, insert_y_score_tmp, insert_x_score_tmp, match_score_tmp, my_score_tmp, my_score_gap_tmp, my_move_x_tmp, my_move_y_tmp, best_score_tmp, dependency_num = -1, cycle = 0, try_score_xy, try_score_match, insert_x_score, insert_y_score, match_score, insert_x_x, insert_x_gap, insert_y_y, insert_y_gap, match_x, match_y;

  int regfile[64], dependency_regfile[56] = {0}, k;

  match_score = 0;                        // Initialization
  match_x = match_y = 0;
  
  insert_x_score = *min_score;
  insert_x_x = 0;
  insert_x_gap = 0;
  
  prev_score = score_rows[y_left[i]->ipos];

  gap_penalty = (prev_score[j].gap_y == 0) ? gap_penalty_y[0]: gap_penalty_y[1];
  // node  0:       comp ==     prev_score[j].gap_y 0 gap_penalty_x[0] gap_penalty_x[1] gap_penalty
  gap_penalty = (prev_score[j].gap_y == 17) ? 0 : gap_penalty;
  // node  1 <- 0:  comp ==     prev_score[j].gap_y 17 gap_penalty_x[17] gap_penalty gap_penalty

  try_score_xy = prev_score[j].score +  y_left[i]->score - gap_penalty;
  // node  2 <- 1:  -
  // node  3 <- 2:  +

  insert_y_score = (try_score_xy > insert_y_score) ? try_score_xy : *min_score;
  // node  4 <- 3:  comp >      try_score insert_y_score try_score min_score insert_y_score
  insert_y_y = (try_score_xy > *min_score) ? 1 : 0;
  // node  5 <- 3:  comp >      try_score insert_y_score ycount 0 insert_y_y
  insert_y_gap = (try_score_xy > *min_score) ? prev_score[j].gap_y : 0;
  // node  6 <- 3:  comp >      try_score insert_y_score prev_score[j].gap_y 0 insert_y_gap
  next_gap_array_y = insert_y_gap + 1;
  // node 22 <- 6:  +   insert_y_gap null 1 null next_gap_array_y
  
  /* LOOP OVER x-predecessors (OUTSIDE y-predecessor LOOP): */
  for (xcount = 0, xl = x_left[j]; xl != NULL; xl = xl->more) {

    dependency_num += 1;

    xcount = xcount + 1;      // 1 more instruction
    gap_penalty = (curr_score[xl->ipos].gap_x == 0) ? gap_penalty_y[0]: gap_penalty_y[1];
    // node  7:       comp ==     curr_score[xl->ipos].gap_x 0 gap_penalty_x[0] gap_penalty_x[1] gap_penalty
    cycle++;  // instruction 7x
    gap_penalty = (curr_score[xl->ipos].gap_x == 17) ? 0 : gap_penalty;
    // node  8 <- 7:  comp ==     curr_score[xl->ipos].gap_x 17 gap_penalty_x[17] gap_penalty gap_penalty

    long_dependency = j - xl->ipos;
    if (long_dependency < MAX_LONG_DEPENDENCY) pre_gap[long_dependency]++;
    if (*max_gap < long_dependency) *max_gap = long_dependency;

    try_score_xy = curr_score[xl->ipos].score + xl->score - gap_penalty;
    // node  9 <- 8:  -
    // node 10 <- 9:  +
    try_score_match = prev_score[xl->ipos].score + xl->score + y_left[i]->score;
    // node 14:       +
    // node 15 <-14:  +

    insert_x_score_tmp = insert_x_score;
    match_score_tmp = match_score;

    insert_x_score = try_score_xy > insert_x_score ? try_score_xy : insert_x_score;
    // node 11 <-10:   comp > 
    match_score = try_score_match > match_score ? try_score_match : match_score;
    // node 16 <-15:   comp > 
    insert_x_x = try_score_xy > insert_x_score_tmp ? xcount : insert_x_x;
    // node 12 <-10:   comp > 
    match_x = try_score_match > match_score_tmp ? xcount : match_x;
    // node 17 <-15:   comp >

    insert_x_gap = try_score_xy > insert_x_score_tmp ? curr_score[xl->ipos].gap_x : insert_x_gap;
    // node 13 <-10:   comp >
    match_y = try_score_match > match_score_tmp ? 1 : match_y;
    // node 18 <-15:   comp >

  }
  if (dependency_num > *max_dependency) *max_dependency = dependency_num;
  
  match_score += m->score[seq_x[j].letter][seq_y[i].letter];
  
  next_gap_array_x = insert_x_gap + 1;
  // node 21 <-13:    copy copy +   insert_x_gap null 1 null next_gap_array_x
  next_gap_array_y = (16 > insert_y_gap) ? next_gap_array_y : insert_y_gap;
  // node 24 <-22:    comp <        insert_y_gap 16 next_gap_array_y 16 next_gap_array_y
 
  next_gap_array_x = (16 > insert_x_gap) ? next_gap_array_x : insert_x_gap;
  // node 23 <-21:    comp <        insert_x_gap 16 next_gap_array_x 16 next_gap_array_x
  my_score_tmp = insert_x_score > insert_y_score ? insert_x_score : insert_y_score;
  // node 25 <-11,4:  comp >

  my_score_gap_tmp = insert_x_score > insert_y_score ? next_gap_array_x : next_gap_array_y;
  // node 26 <-11,4:  comp >
  my_move_x_tmp = insert_x_score > insert_y_score ? insert_x_x : 0;
  // node 27 <-11,4:  comp >

  my_move_y_tmp = insert_x_score > insert_y_score ? 0 : insert_y_y;
  // node 28 <-11,4:  comp >
  curr_score[j].score = match_score > my_score_tmp ? match_score : my_score_tmp;
  // node 29 <-25:    comp >

  curr_score[j].gap_x = match_score > my_score_tmp ? 0 : my_score_gap_tmp;
  curr_score[j].gap_y = match_score > my_score_tmp ? 0 : my_score_gap_tmp;
  // node 30 <-25,26: comp >

  (&move[i][j])->x = match_score > my_score_tmp ? match_x : my_move_x_tmp;
  // node 31 <-25,27: comp >
  (&move[i][j])->y = match_score > my_score_tmp ? match_y : my_move_y_tmp;
  // node 32 <-25,28: comp >

  best_score_tmp = *best_score;
  *best_score = curr_score[j].score > *best_score ? curr_score[j].score : *best_score;
  // node 33 <-29:    comp >

  cycle++;  // instruction 20
  *best_x = curr_score[j].score > best_score_tmp ? j : *best_x;
  // node 34 <-29:    comp >
  *best_y = curr_score[j].score > best_score_tmp ? i : *best_y;
  // node 35 <-29:    comp >

  return cycle;
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



void accelerator_outer_loop_predecessor(int len_y, int *cycle, int *cells, int *refs_from_right_y, DPScore_T *init_col_score, \

LPOLetter_T *seq_x, LPOLetter_T *seq_y, int len_x, int *node_type_x, int *node_type_y, int *best_x, int *best_y, LPOScore_T *min_score, LPOScore_T *best_score, LPOLetterLink_T **x_left, LPOLetterLink_T **y_left, LPOLetterLink_T *xl, LPOLetterLink_T *yl, DPMove_T **move, DPMove_T *my_move, int possible_end_square, DPScore_T **score_rows, DPScore_T *curr_score, DPScore_T *prev_score, DPScore_T *my_score, LPOScore_T *gap_penalty_x, LPOScore_T *gap_penalty_y, int *next_gap_array, int *next_perp_gap_array, int pre_gap[MAX_LONG_DEPENDENCY], int use_global_alignment, LPOScore_T (*scoring_function)(int, int, LPOLetter_T *, LPOLetter_T *, ResidueScoreMatrix_T *), ResidueScoreMatrix_T *m, int max_gap_length, int *max_gap, int *max_dependency) {

  int i, j, n_score_rows_alloced = 0, xcount;
  int *xl_index, *xl_ipos, index, index_accumulation;

  xl_index = (int*)malloc(XL_INDEX_NUM * sizeof(int));
  xl_ipos = (int*)malloc(XL_IPOS_NUM * sizeof(int));

  // for (i=0; i<len_y; i++) {
  //   printf("y index: %d, letter: %d\n", i, seq_y[i].letter);
  // }

  index = 0;
  index_accumulation = 0;
  xl_index[0] = 0;
  for (j=0; j<len_x; j++) {
    // printf("x index: %d, letter: %d prev: ", j, seq_x[j].letter);
    for (xcount = 0, xl = x_left[j]; xl != NULL; xl = xl->more, xcount++){
      if (xcount > 0) {
        // printf("(%d, %d) ", xl->ipos, xl->score);
        xl_ipos[index] = xl->ipos;
        index++;
        index_accumulation++;
      }
    }
    xl_index[j+1] = index_accumulation;
    // printf("\n");
  }
  
  for (i=0; i<len_y; i++) {
    
    /* ALLOCATE MEMORY FOR 'SCORE' ROW i: */
    CALLOC (score_rows[i], len_x+1, DPScore_T);
    score_rows[i] = &(score_rows[i][1]);
    n_score_rows_alloced++;
        
    curr_score = score_rows[i];
    curr_score[-1] = init_col_score[i];
    
          
    /* INNER LOOP (j-th position in LPO x): */
    for (j=0; j<len_x; j++) {

      cycle += accelerator_predecessor(i, j, xl_index, xl_ipos, seq_x, seq_y, len_x, node_type_x, node_type_y, best_x, best_y, min_score, best_score, x_left, y_left, xl, yl, move, my_move, possible_end_square, score_rows, curr_score, prev_score, my_score, gap_penalty_x, gap_penalty_y, next_gap_array, next_perp_gap_array, pre_gap, use_global_alignment, scoring_function, m, max_gap_length, max_gap, max_dependency);

      cells++;

    }

    /* UPDATE # OF REFS TO 'SCORE' ROWS; FREE MEMORY WHEN POSSIBLE: */
    for (yl = y_left[i]; yl != NULL; yl = yl->more) if ((j = yl->ipos) >= 0) {
      if ((--refs_from_right_y[j]) == 0) {
        score_rows[j] = &(score_rows[j][-1]);
        FREE (score_rows[j]);
        n_score_rows_alloced--;
      }
    }
    if (refs_from_right_y[i] == 0) {
      score_rows[i] = &(score_rows[i][-1]);
      FREE (score_rows[i]);
      n_score_rows_alloced--;
    }
  }

}

void accelerator_outer_loop(int len_y, int *cycle, int *cells, int *refs_from_right_y, DPScore_T *init_col_score, \

LPOLetter_T *seq_x, LPOLetter_T *seq_y, int len_x, int *node_type_x, int *node_type_y, int *best_x, int *best_y, LPOScore_T *min_score, LPOScore_T *best_score, LPOLetterLink_T **x_left, LPOLetterLink_T **y_left, LPOLetterLink_T *xl, LPOLetterLink_T *yl, DPMove_T **move, DPMove_T *my_move, int possible_end_square, DPScore_T **score_rows, DPScore_T *curr_score, DPScore_T *prev_score, DPScore_T *my_score, LPOScore_T *gap_penalty_x, LPOScore_T *gap_penalty_y, int *next_gap_array, int *next_perp_gap_array, int pre_gap[MAX_LONG_DEPENDENCY], int use_global_alignment, LPOScore_T (*scoring_function)(int, int, LPOLetter_T *, LPOLetter_T *, ResidueScoreMatrix_T *), ResidueScoreMatrix_T *m, int max_gap_length, int *max_gap, int *max_dependency) {

  int i, j, n_score_rows_alloced = 0, xcount;

  // for (i=0; i<len_y; i++) {
  //   printf("y index: %d, letter: %d\n", i, seq_y[i].letter);
  // }

  // for (j=0; j<len_x; j++) {
  //   printf("x index: %d, letter: %d prev: ", j, seq_x[j].letter);
  //   for (xcount = 0, xl = x_left[j]; xl != NULL; xl = xl->more, xcount++){
  //     printf("(%d, %d) ", xl->ipos, xl->score);
  //   }
  //   printf("\n");
  // }
  
  for (i=0; i<len_y; i++) {
    
    /* ALLOCATE MEMORY FOR 'SCORE' ROW i: */
    CALLOC (score_rows[i], len_x+1, DPScore_T);
    score_rows[i] = &(score_rows[i][1]);
    n_score_rows_alloced++;
        
    curr_score = score_rows[i];
    curr_score[-1] = init_col_score[i];
    
          
    /* INNER LOOP (j-th position in LPO x): */
    for (j=0; j<len_x; j++) {

      int dependency_regfile[4*14] = {0};

      for (xcount = 0, xl = x_left[j]; xl != NULL; xl = xl->more, xcount++){
        dependency_regfile[4*xcount+0] = (score_rows[y_left[i]->ipos])[xl->ipos].score;
        dependency_regfile[4*xcount+1] = curr_score[xl->ipos].gap_x;
        dependency_regfile[4*xcount+2] = curr_score[xl->ipos].score;
        dependency_regfile[4*xcount+3] = xl->score;
        if (xl->score) fprintf(stderr, "%d\n", xl->score);
      }     
      
      // cycle += accelerator(i, j, xcount, dependency_regfile, seq_x, seq_y, len_x, node_type_x, node_type_y, best_x, best_y, min_score, best_score, x_left, y_left, xl, yl, move, my_move, possible_end_square, score_rows, curr_score, prev_score, my_score, gap_penalty_x, gap_penalty_y, next_gap_array, next_perp_gap_array, pre_gap, use_global_alignment, scoring_function, m, max_gap_length, max_gap, max_dependency);
      cycle += accelerator_algorithm(i, j, seq_x, seq_y, len_x, node_type_x, node_type_y, best_x, best_y, min_score, best_score, x_left, y_left, xl, yl, move, my_move, possible_end_square, score_rows, curr_score, prev_score, my_score, gap_penalty_x, gap_penalty_y, next_gap_array, next_perp_gap_array, pre_gap, use_global_alignment, scoring_function, m, max_gap_length, max_gap, max_dependency);

      cells++;

    }

    /* UPDATE # OF REFS TO 'SCORE' ROWS; FREE MEMORY WHEN POSSIBLE: */
    for (yl = y_left[i]; yl != NULL; yl = yl->more) if ((j = yl->ipos) >= 0) {
      if ((--refs_from_right_y[j]) == 0) {
        score_rows[j] = &(score_rows[j][-1]);
        FREE (score_rows[j]);
        n_score_rows_alloced--;
      }
    }
    if (refs_from_right_y[i] == 0) {
      score_rows[i] = &(score_rows[i][-1]);
      FREE (score_rows[i]);
      n_score_rows_alloced--;
    }
  }

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

void accelerator_queue_register(int i, int j, unsigned long* instruction, queue *Q_left_gap, queue *Q_left_score, queue *Q_diag_score, int *xl_index, int *xl_ipos, LPOLetter_T *seq_x, LPOLetter_T *seq_y, int len_x, int *best_x, int *best_y, LPOScore_T *best_score, DPMove_T **move, DPScore_T **score_rows, DPScore_T *curr_score, DPScore_T *prev_score, ResidueScoreMatrix_T *m, int* regfile, compute_unit_32* cu, comp_decoder* decoder_unit){

  LPOLetterLink_T *xl, *yl;

  int t, k, queue_front, predecessor_gap, queue_index, new_score;

  // Initialization
  regfile[19] = regfile[0];  // 0x4cfb80000013
  regfile[23] = regfile[18];  // 0x4cfc80000017
  regfile[24] = regfile[0];  // 0x4cfb80000018
  regfile[22] = regfile[0];  // 0x4cfb80000016
  regfile[26] = regfile[18];  // 0x4cfc8000001a
  regfile[27] = regfile[0];  // 0x4cfb8000001b
  regfile[25] = regfile[0];  // 0x4cfb80000019
  regfile[28] = regfile[0];  // 0x4cfb8000001c
  regfile[29] = regfile[0];  // 0x4cfb8000001d
  regfile[30] = regfile[0];  // 0x4cfb8000001e

  // Update y
  regfile[9] = regfile[9] + regfile[15];
  regfile[20] = (regfile[1] == regfile[0]) ? regfile[14]: regfile[13];   // 0x77c85cc68014
  regfile[20] = (regfile[1] == regfile[17]) ? regfile[0] : regfile[20];  // 0x77c862ea0014
  regfile[21] = regfile[2] - regfile[20];                                 // 0xfc8a8000015
  regfile[23] = (regfile[21] > regfile[18]) ? regfile[21] : regfile[18];  // 0x6fcd65570017
  regfile[24] = (regfile[21] > regfile[18]) ? regfile[15] : regfile[0];  // 0x6fcd64f70018
  regfile[22] = (regfile[21] > regfile[18]) ? regfile[1] : regfile[0];   // 0x6fcd64170016

  /* LOOP OVER x-predecessors (OUTSIDE y-predecessor LOOP): */
  // for (k = 0; k < xl_index[j+1] - xl_index[j]; k++) {
  while (regfile[19] != regfile[31]) {
    
    predecessor_gap = xl_ipos[xl_index[j]+regfile[19]];
    // if (predecessor_gap >= MAX_LONG_DEPENDENCY) return cycle;
    queue_index = Q_left_gap->rear + 1 - predecessor_gap;
    if (queue_index < 0) queue_index += MAX_LONG_DEPENDENCY;

    if (curr_score[j - xl_ipos[xl_index[j]+regfile[19]]].gap_x != Q_left_gap->predecessor[queue_index])
      printf("left_gap: %d %d %d\n", curr_score[j - xl_ipos[xl_index[j]+regfile[19]]].gap_x, Q_left_gap->predecessor[queue_index], predecessor_gap);
    if (curr_score[j - xl_ipos[xl_index[j]+regfile[19]]].score != Q_left_score->predecessor[queue_index])
      printf("left_score: %d %d %d\n", curr_score[j - xl_ipos[xl_index[j]+regfile[19]]].score, Q_left_score->predecessor[queue_index], predecessor_gap);
    if (prev_score[j - xl_ipos[xl_index[j]+regfile[19]]].score != Q_diag_score->predecessor[queue_index])
      printf("diag_score: %d %d %d\n", prev_score[j - xl_ipos[xl_index[j]+regfile[19]]].score, Q_diag_score->predecessor[queue_index], predecessor_gap);

    regfile[3] = Q_diag_score->predecessor[queue_index];
    regfile[4] = Q_left_gap->predecessor[queue_index];
    regfile[5] =  Q_left_score->predecessor[queue_index];

    regfile[19] = regfile[19] + regfile[15];
    regfile[20] = (regfile[4] == regfile[0]) ? regfile[14]: regfile[13];
    regfile[20] = (regfile[4] == regfile[17]) ? regfile[0] : regfile[20];
    regfile[21] = regfile[5] - regfile[20];
    regfile[27] = regfile[21] > regfile[26] ? regfile[19] : regfile[27];
    regfile[29] = regfile[3] > regfile[28] ? regfile[19] : regfile[29];
    regfile[25] = regfile[21] > regfile[26] ? regfile[4] : regfile[25];
    regfile[30] = regfile[3] > regfile[28] ? regfile[15] : regfile[30];
    regfile[26] = regfile[21] > regfile[26] ? regfile[21] : regfile[26];
    regfile[28] = regfile[3] > regfile[28] ? regfile[3] : regfile[28];

  }

  regfile[19] = m->score[regfile[12]][regfile[10]];
  regfile[28] = regfile[28] + regfile[19];                                // 0x54881400701c

  regfile[21] = regfile[25] + regfile[15];                                // 0x7ce5e000015
  regfile[20] = regfile[22] + regfile[15];                                // 0x7cd9e000014
  regfile[21] = (regfile[16] > regfile[25]) ? regfile[21] : regfile[25];  // 0x6fcc335c8015
  regfile[20] = (regfile[16] > regfile[22]) ? regfile[20] : regfile[22];  // 0x6fcc2d4b0014
  
  regfile[19] = regfile[26] > regfile[23] ? regfile[26] : regfile[23];    // 0x6fceafab8013
  regfile[20] = regfile[26] > regfile[23] ? regfile[21] : regfile[20];    // 0x6fceaf5a0014
  regfile[22] = regfile[26] > regfile[23] ? regfile[27] : regfile[0];    // 0x6fceafb70016
  regfile[21] = regfile[26] > regfile[23] ? regfile[0] : regfile[24];    // 0x6fceaeec0015

  regfile[5] = regfile[28] > regfile[19] ? regfile[28] : regfile[19];     // 0x6fcf27c98005
  regfile[4] = regfile[28] > regfile[19] ? regfile[0] : regfile[20];     // 0x6fcf26ea0004
  regfile[1] = regfile[28] > regfile[19] ? regfile[0] : regfile[20];     // 0x6fcf26ea0001

  regfile[24] = regfile[28] > regfile[19] ? regfile[29] : regfile[22];    // 0x6fcf27db0018
  regfile[23] = regfile[28] > regfile[19] ? regfile[30] : regfile[21];    // 0x6fcf27ea8017

  regfile[7] = regfile[5] > regfile[6] ? regfile[9] : regfile[7];         // 0x6fc94c938007
  regfile[8] = regfile[5] > regfile[6] ? regfile[11] : regfile[8];        // 0x6fc94cb40008
  regfile[6] = regfile[5] > regfile[6] ? regfile[5] : regfile[6];         // 0x6fc94c530006

  regfile[3] = regfile[2];

  if (Q_left_gap->count == MAX_LONG_DEPENDENCY)
    queue_front = dequeue(Q_left_gap);
  enqueue(Q_left_gap, regfile[4]);
  if (Q_left_score->count == MAX_LONG_DEPENDENCY)
    queue_front = dequeue(Q_left_score);
  enqueue(Q_left_score, regfile[5]);
  if (Q_diag_score->count == MAX_LONG_DEPENDENCY)
    queue_front = dequeue(Q_diag_score);
  enqueue(Q_diag_score, regfile[2]);

  curr_score[j].gap_y = regfile[1];
  curr_score[j].score = regfile[5];
  curr_score[j].gap_x = regfile[4];
  *best_score = regfile[6];
  *best_x = regfile[7];
  *best_y = regfile[8];
  move[i][j].y = regfile[23];
  move[i][j].x = regfile[24];
  
  return;
}

void accelerator_queue_instruction(int i, int j, unsigned long* instruction, queue *Q_left_gap, queue *Q_left_score, queue *Q_diag_score, int *xl_index, int *xl_ipos, LPOLetter_T *seq_x, LPOLetter_T *seq_y, int len_x, int *best_x, int *best_y, LPOScore_T *best_score, DPMove_T **move, DPScore_T **score_rows, DPScore_T *curr_score, DPScore_T *prev_score, ResidueScoreMatrix_T *m, int* regfile, compute_unit_32* cu, comp_decoder* decoder_unit){

  LPOLetterLink_T *xl, *yl;

  int t, k, queue_front, predecessor_gap, queue_index, new_score;

  for (t = 0; t < 20; t++)
    execute_instrution(t, instruction[t], decoder_unit, cu, regfile);

  /* LOOP OVER x-predecessors (OUTSIDE y-predecessor LOOP): */
  // for (k = 0; k < xl_index[j+1] - xl_index[j]; k++) {
  while (regfile[19] != regfile[31]) {
    
    predecessor_gap = xl_ipos[xl_index[j]+regfile[19]];
    // if (predecessor_gap >= MAX_LONG_DEPENDENCY) return cycle;
    queue_index = Q_left_gap->rear + 1 - predecessor_gap;
    if (queue_index < 0) queue_index += MAX_LONG_DEPENDENCY;

    if (curr_score[j - xl_ipos[xl_index[j]+regfile[19]]].gap_x != Q_left_gap->predecessor[queue_index])
      printf("left_gap: %d %d %d\n", curr_score[j - xl_ipos[xl_index[j]+regfile[19]]].gap_x, Q_left_gap->predecessor[queue_index], predecessor_gap);
    if (curr_score[j - xl_ipos[xl_index[j]+regfile[19]]].score != Q_left_score->predecessor[queue_index])
      printf("left_score: %d %d %d\n", curr_score[j - xl_ipos[xl_index[j]+regfile[19]]].score, Q_left_score->predecessor[queue_index], predecessor_gap);
    if (prev_score[j - xl_ipos[xl_index[j]+regfile[19]]].score != Q_diag_score->predecessor[queue_index])
      printf("diag_score: %d %d %d\n", prev_score[j - xl_ipos[xl_index[j]+regfile[19]]].score, Q_diag_score->predecessor[queue_index], predecessor_gap);

    regfile[3] = Q_diag_score->predecessor[queue_index];
    regfile[4] = Q_left_gap->predecessor[queue_index];
    regfile[5] =  Q_left_score->predecessor[queue_index];

    for (t = 20; t < 38; t++)
      execute_instrution(t, instruction[t], decoder_unit, cu, regfile);
    
  }

  for (t = 38; t < 58; t++)
    execute_instrution(t, instruction[t], decoder_unit, cu, regfile);

  if (Q_left_gap->count == MAX_LONG_DEPENDENCY)
    queue_front = dequeue(Q_left_gap);
  enqueue(Q_left_gap, regfile[4]);
  if (Q_left_score->count == MAX_LONG_DEPENDENCY)
    queue_front = dequeue(Q_left_score);
  enqueue(Q_left_score, regfile[5]);
  if (Q_diag_score->count == MAX_LONG_DEPENDENCY)
    queue_front = dequeue(Q_diag_score);
  enqueue(Q_diag_score, regfile[2]);

  curr_score[j].gap_y = regfile[1];
  curr_score[j].score = regfile[5];
  curr_score[j].gap_x = regfile[4];
  *best_score = regfile[6];
  *best_x = regfile[7];
  *best_y = regfile[8];
  move[i][j].y = regfile[23];
  move[i][j].x = regfile[24];
  
  return;
}

void accelerator_cpu_register(int i, int j, unsigned long* instruction, queue *Q_left_gap, queue *Q_left_score, queue *Q_diag_score, int *xl_index, int *xl_ipos, LPOLetter_T *seq_x, LPOLetter_T *seq_y, int len_x, int *best_x, int *best_y, LPOScore_T *best_score, DPMove_T **move, DPScore_T **score_rows, DPScore_T *curr_score, DPScore_T *prev_score, ResidueScoreMatrix_T *m, int* regfile, compute_unit_32* cu, comp_decoder* decoder_unit){

  LPOLetterLink_T *xl, *yl;

  int t, k, queue_front, predecessor_gap, queue_index, new_score;

  // Initialization
  regfile[19] = regfile[0];  // 0x4cfb80000013
  regfile[23] = regfile[18];  // 0x4cfc80000017
  regfile[24] = regfile[0];  // 0x4cfb80000018
  regfile[22] = regfile[0];  // 0x4cfb80000016
  regfile[26] = regfile[18];  // 0x4cfc8000001a
  regfile[27] = regfile[0];  // 0x4cfb8000001b
  regfile[25] = regfile[0];  // 0x4cfb80000019
  regfile[28] = regfile[0];  // 0x4cfb8000001c
  regfile[29] = regfile[0];  // 0x4cfb8000001d
  regfile[30] = regfile[0];  // 0x4cfb8000001e

  // Update y
  regfile[9] = regfile[9] + regfile[15];
  regfile[20] = (regfile[1] == regfile[0]) ? regfile[14]: regfile[13];   // 0x77c85cc68014
  regfile[20] = (regfile[1] == regfile[17]) ? regfile[0] : regfile[20];  // 0x77c862ea0014
  regfile[21] = regfile[2] - regfile[20];                                 // 0xfc8a8000015
  regfile[23] = (regfile[21] > regfile[18]) ? regfile[21] : regfile[18];  // 0x6fcd65570017
  regfile[24] = (regfile[21] > regfile[18]) ? regfile[15] : regfile[0];  // 0x6fcd64f70018
  regfile[22] = (regfile[21] > regfile[18]) ? regfile[1] : regfile[0];   // 0x6fcd64170016

  /* LOOP OVER x-predecessors (OUTSIDE y-predecessor LOOP): */
  // for (k = 0; k < xl_index[j+1] - xl_index[j]; k++) {
  while (regfile[19] != regfile[31]) {
    
    // predecessor_gap = j - (xl_ipos[xl_index[j]+k]);
    // // if (predecessor_gap >= MAX_LONG_DEPENDENCY) return cycle;
    // queue_index = Q_left_gap->rear + 1 - predecessor_gap;
    // if (queue_index < 0) queue_index += MAX_LONG_DEPENDENCY;

    // if (curr_score[xl_ipos[xl_index[j]+k]].gap_x != Q_left_gap->predecessor[queue_index])
    //   printf("left_gap: %d %d %d\n", curr_score[xl_ipos[xl_index[j]+k]].gap_x, Q_left_gap->predecessor[queue_index], predecessor_gap);
    // if (curr_score[xl_ipos[xl_index[j]+k]].score != Q_left_score->predecessor[queue_index])
    //   printf("left_score: %d %d %d\n", curr_score[xl_ipos[xl_index[j]+k]].score, Q_left_score->predecessor[queue_index], predecessor_gap);
    // if (prev_score[xl_ipos[xl_index[j]+k]].score != Q_diag_score->predecessor[queue_index])
    //   printf("diag_score: %d %d %d\n", prev_score[xl_ipos[xl_index[j]+k]].score, Q_diag_score->predecessor[queue_index], predecessor_gap);

    // Predecessor
    regfile[3] = prev_score[j - xl_ipos[xl_index[j]+regfile[19]]].score;
    regfile[4] = curr_score[j - xl_ipos[xl_index[j]+regfile[19]]].gap_x;
    regfile[5] = curr_score[j - xl_ipos[xl_index[j]+regfile[19]]].score;

    regfile[19] = regfile[19] + regfile[15];
    regfile[20] = (regfile[4] == regfile[0]) ? regfile[14]: regfile[13];
    regfile[20] = (regfile[4] == regfile[17]) ? regfile[0] : regfile[20];
    regfile[21] = regfile[5] - regfile[20];
    regfile[27] = regfile[21] > regfile[26] ? regfile[19] : regfile[27];
    regfile[29] = regfile[3] > regfile[28] ? regfile[19] : regfile[29];
    regfile[25] = regfile[21] > regfile[26] ? regfile[4] : regfile[25];
    regfile[30] = regfile[3] > regfile[28] ? regfile[15] : regfile[30];
    regfile[26] = regfile[21] > regfile[26] ? regfile[21] : regfile[26];
    regfile[28] = regfile[3] > regfile[28] ? regfile[3] : regfile[28];
  }

  regfile[19] = m->score[regfile[12]][regfile[10]];
  regfile[28] = regfile[28] + regfile[19];                                // 0x54881400701c

  regfile[21] = regfile[25] + regfile[15];                                // 0x7ce5e000015
  regfile[20] = regfile[22] + regfile[15];                                // 0x7cd9e000014
  regfile[21] = (regfile[16] > regfile[25]) ? regfile[21] : regfile[25];  // 0x6fcc335c8015
  regfile[20] = (regfile[16] > regfile[22]) ? regfile[20] : regfile[22];  // 0x6fcc2d4b0014
  
  regfile[19] = regfile[26] > regfile[23] ? regfile[26] : regfile[23];    // 0x6fceafab8013
  regfile[20] = regfile[26] > regfile[23] ? regfile[21] : regfile[20];    // 0x6fceaf5a0014
  regfile[22] = regfile[26] > regfile[23] ? regfile[27] : regfile[0];    // 0x6fceafb70016
  regfile[21] = regfile[26] > regfile[23] ? regfile[0] : regfile[24];    // 0x6fceaeec0015

  regfile[5] = regfile[28] > regfile[19] ? regfile[28] : regfile[19];     // 0x6fcf27c98005
  regfile[4] = regfile[28] > regfile[19] ? regfile[0] : regfile[20];     // 0x6fcf26ea0004
  regfile[1] = regfile[28] > regfile[19] ? regfile[0] : regfile[20];     // 0x6fcf26ea0001

  regfile[24] = regfile[28] > regfile[19] ? regfile[29] : regfile[22];    // 0x6fcf27db0018
  regfile[23] = regfile[28] > regfile[19] ? regfile[30] : regfile[21];    // 0x6fcf27ea8017

  regfile[7] = regfile[5] > regfile[6] ? regfile[9] : regfile[7];         // 0x6fc94c938007
  regfile[8] = regfile[5] > regfile[6] ? regfile[11] : regfile[8];        // 0x6fc94cb40008
  regfile[6] = regfile[5] > regfile[6] ? regfile[5] : regfile[6];         // 0x6fc94c530006

  regfile[3] = regfile[2];
  
  // if (Q_left_gap->count == MAX_LONG_DEPENDENCY)
  //   queue_front = dequeue(Q_left_gap);
  // enqueue(Q_left_gap, curr_score[j].gap_x);
  // if (Q_left_score->count == MAX_LONG_DEPENDENCY)
  //   queue_front = dequeue(Q_left_score);
  // enqueue(Q_left_score, curr_score[j].score);
  // if (Q_diag_score->count == MAX_LONG_DEPENDENCY)
  //   queue_front = dequeue(Q_diag_score);
  // enqueue(Q_diag_score, prev_score[j].score);

  curr_score[j].gap_y = regfile[1];
  curr_score[j].score = regfile[5];
  curr_score[j].gap_x = regfile[4];
  *best_score = regfile[6];
  *best_x = regfile[7];
  *best_y = regfile[8];
  move[i][j].y = regfile[23];
  move[i][j].x = regfile[24];
  
  return;
}

int accelerator_outer_loop_queue(unsigned long* instruction, int len_y, LPOLetter_T *seq_x, LPOLetter_T *seq_y, int len_x, int *xl_index, int *xl_ipos, int *xl_pred_length, int len_ipos, int *best_x, int *best_y, LPOScore_T *best_score, DPMove_T **move, DPScore_T **score_rows, ResidueScoreMatrix_T *m) {

  // input len_x seq_x len_y seq_y len_ipos score_rows[-1][j] score_rows[i][-1]
  // in/out best_x best_y best_score

  int i, j,  xcount;
  int regfile[32];
  LPOLetterLink_T *xl, *yl;

  compute_unit_32 cu;
  comp_decoder decoder_unit;

  regfile[14] = 12;                                       // constant 12            (constant)
  regfile[13] = 6;                                        // constant 6             (constant)
  regfile[0] = 0;                                        // constant 0             (constant)
  regfile[15] = 1;                                        // constant 1             (constant)
  regfile[16] = 16;                                       // constant 16            (constant)
  regfile[17] = 17;                                       // constant 17            (constant)
  regfile[18] = -999999;                                  // constant min_score     (constant)

  queue Q_left_gap, Q_left_score, Q_diag_score;
  
  for (i=0; i<len_y; i++) {

    regfile[9] = -1;
    regfile[10] = seq_y[i].letter;
    regfile[11] = i;

    Q_init(&Q_left_gap);
    Q_init(&Q_left_score);
    Q_init(&Q_diag_score);

    enqueue(&Q_left_gap, score_rows[i][-1].gap_x);
    enqueue(&Q_left_score, score_rows[i][-1].score);
    enqueue(&Q_diag_score, score_rows[i-1][-1].score);

          
    /* INNER LOOP (j-th position in LPO x): */
    for (j=0; j<len_x; j++) {

      regfile[31] = xl_pred_length[j];
      regfile[12] = seq_x[j].letter;
      regfile[1] = score_rows[i-1][j].gap_y;
      regfile[2] = score_rows[i-1][j].score;
      regfile[6] = *best_score;
      regfile[7] = *best_x;
      regfile[8] = *best_y;

      // printf("\ni=%d j=%d\t", i, j);

      accelerator_queue_register(i, j, instruction, &Q_left_gap, &Q_left_score, &Q_diag_score, xl_index, xl_ipos, seq_x, seq_y, len_x, best_x, best_y, best_score, move, score_rows, score_rows[i], score_rows[i-1], m, regfile, &cu, &decoder_unit);
      // accelerator_queue_instruction(i, j, instruction, &Q_left_gap, &Q_left_score, &Q_diag_score, xl_index, xl_ipos, seq_x, seq_y, len_x, best_x, best_y, best_score, move, score_rows, score_rows[i], score_rows[i-1], m, regfile, &cu, &decoder_unit);

    }

    if (i > 0) {
      score_rows[i-1] = &(score_rows[i-1][-1]);
      FREE (score_rows[i-1]);
    }
  }

  return 0;
}

int accelerator_outer_loop_cpu(unsigned long* instruction, int len_y, LPOLetter_T *seq_x, LPOLetter_T *seq_y, int len_x, int *xl_index, int *xl_ipos, int *xl_pred_length, int len_ipos, int *best_x, int *best_y, LPOScore_T *best_score, DPMove_T **move, DPScore_T **score_rows, ResidueScoreMatrix_T *m) {

  // input len_x seq_x len_y seq_y len_ipos score_rows[-1][j] score_rows[i][-1]
  // in/out best_x best_y best_score

  int i, j,  xcount;
  int regfile[32];
  LPOLetterLink_T *xl, *yl;

  compute_unit_32 cu;
  comp_decoder decoder_unit;

  regfile[14] = 12;                                       // constant 12            (constant)
  regfile[13] = 6;                                        // constant 6             (constant)
  regfile[0] = 0;                                        // constant 0             (constant)
  regfile[15] = 1;                                        // constant 1             (constant)
  regfile[16] = 16;                                       // constant 16            (constant)
  regfile[17] = 17;                                       // constant 17            (constant)
  regfile[18] = -999999;                                  // constant min_score     (constant)

  queue Q_left_gap, Q_left_score, Q_diag_score;

  fprintf(stderr, "cpu.\n");
  
  for (i=0; i<len_y; i++) {

    regfile[9] = -1;
    regfile[10] = seq_y[i].letter;
    regfile[11] = i;

    Q_init(&Q_left_gap);
    Q_init(&Q_left_score);
    Q_init(&Q_diag_score);

    enqueue(&Q_left_gap, score_rows[i][-1].gap_x);
    enqueue(&Q_left_score, score_rows[i][-1].score);
    enqueue(&Q_diag_score, score_rows[i-1][-1].score);
          
    /* INNER LOOP (j-th position in LPO x): */
    for (j=0; j<len_x; j++) {

      regfile[31] = xl_pred_length[j];
      regfile[12] = seq_x[j].letter;
      regfile[1] = score_rows[i-1][j].gap_y;
      regfile[2] = score_rows[i-1][j].score;
      regfile[6] = *best_score;
      regfile[7] = *best_x;
      regfile[8] = *best_y;

      accelerator_cpu_register(i, j, instruction, &Q_left_gap, &Q_left_score, &Q_diag_score, xl_index, xl_ipos, seq_x, seq_y, len_x, best_x, best_y, best_score, move, score_rows, score_rows[i], score_rows[i-1], m, regfile, &cu, &decoder_unit);

    }

    if (i > 0) {
      score_rows[i-1] = &(score_rows[i-1][-1]);
      FREE (score_rows[i-1]);
    }
  }

  return 0;
}

LPOScore_T align_lpo_po_GenDP (LPOSequence_T *lposeq_x,
			 LPOSequence_T *lposeq_y,
			 ResidueScoreMatrix_T *m,
			 LPOLetterRef_T **x_to_y,
			 LPOLetterRef_T **y_to_x,
			 LPOScore_T (*scoring_function)
			 (int, int, LPOLetter_T *, LPOLetter_T *, ResidueScoreMatrix_T *),
			 int use_global_alignment, FILE* fp_in, FILE* fp_out)
{

  int i, j;

  LPOLetter_T *seq_x = lposeq_x->letter;
  LPOLetter_T *seq_y = lposeq_y->letter;
  
  /* GET LPO STATUS */

  int len_x, len_y;
  int n_edges_x, n_edges_y;
  int *node_type_x, *node_type_y;
  int *refs_from_right_x, *refs_from_right_y;
  int max_rows_alloced_x, max_rows_alloced_y, n_score_rows_alloced = 0;
  LPOLetterLink_T **x_left = NULL, **y_left = NULL, *xl, *yl;
  
  get_lpo_stats (lposeq_x, &len_x, &n_edges_x, &node_type_x, &refs_from_right_x, &max_rows_alloced_x, &x_left);
  get_lpo_stats (lposeq_y, &len_y, &n_edges_y, &node_type_y, &refs_from_right_y, &max_rows_alloced_y, &y_left);

  int best_x = -1, best_y = -1;
  LPOScore_T best_score = -999999;

  /* ALLOCATE MEMORY FOR 'MOVE' AND 'SCORE' MATRICES: */

  DPMove_T **move = NULL;
  CALLOC (move, len_y, DPMove_T *);
  for (i=0; i<len_y; i++) {
    CALLOC (move[i], len_x, DPMove_T);
  }
  
  /* FILL INITIAL COLUMN/ROW (-1). new sequence */

  DPScore_T *curr_score = NULL, **score_rows = NULL;
  CALLOC (score_rows, len_y+1, DPScore_T *);
  score_rows = &(score_rows[1]);
  for (i=-1; i<len_y; i++) {
    CALLOC (score_rows[i], len_x+1, DPScore_T);
    score_rows[i] = &(score_rows[i][1]);
    score_rows[i][-1].score = 0;
    score_rows[i][-1].gap_x = 17;
  }
  for (i=-1; i<len_x; i++) {
    score_rows[-1][i].score = 0;
    score_rows[-1][i].gap_y = 17;
  }

  /* LOOK FOR PREDECESSOR INFORMATION */

  int *xl_index, *xl_ipos, *xl_pred_length, index, index_accumulation, cpu = 0;
  xl_index = (int*)malloc(XL_INDEX_NUM * sizeof(int));
  xl_pred_length = (int*)malloc(XL_INDEX_NUM * sizeof(int));
  xl_ipos = (int*)malloc(XL_IPOS_NUM * sizeof(int));
  index = 0;
  index_accumulation = 0;
  xl_index[0] = 0;
  for (j=0; j<len_x; j++) {
    for (xl = x_left[j]; xl != NULL; xl = xl->more){
      if (j - xl->ipos >= MAX_LONG_DEPENDENCY) cpu = 1;
      xl_ipos[index] = j - xl->ipos;
      index++;
      index_accumulation++;
    }
    xl_index[j+1] = index_accumulation;
    xl_pred_length[j] = xl_index[j+1] - xl_index[j];
  }

  unsigned long *instruction;
  instruction = (unsigned long*)malloc(64*sizeof(unsigned long));
  instruction[0] = 0x12f4800000013;
  instruction[1] = 0x12f4c80000017;
  instruction[2] = 0x12f4800000018;
  instruction[3] = 0x12f4800000016;
  instruction[4] = 0x12f4c8000001a;
  instruction[5] = 0x12f480000001b;
  instruction[6] = 0x12f4800000019;
  instruction[7] = 0x12f480000001c;
  instruction[8] = 0x12f480000001d;
  instruction[9] = 0x12f480000001e;
  instruction[10] = 0x1cf4840e68014;
  instruction[11] = 0xf4a5e000009;
  instruction[12] = 0x1cf48620a0014;
  instruction[13] = 0x1ef7800000000;
  instruction[14] = 0x2f48a8000015;
  instruction[15] = 0x1ef7800000000;
  instruction[16] = 0x1af4d65590017;
  instruction[17] = 0x1af4d64f00018;
  instruction[18] = 0x1af4d64100016;
  instruction[19] = 0x1ef7800000000;
  instruction[20] = 0xf4cde000013;
  instruction[21] = 0x1cf4900e68014;
  instruction[22] = 0x1cf49220a0014;
  instruction[23] = 0x1ef7800000000;
  instruction[24] = 0x2f4968000015;
  instruction[25] = 0x1ef7800000000;
  instruction[26] = 0x1af4d753d801b;
  instruction[27] = 0x1af48f93e801d;
  instruction[28] = 0x1af4d744c8019;
  instruction[29] = 0x1af48f8ff001e;
  instruction[30] = 0x1af4d755d001a;
  instruction[31] = 0x1af48f83e001c;
  instruction[32] = 0x1ef7800000000;
  instruction[33] = 0x1ef7800000000;
  instruction[34] = 0x1ef7800000000;
  instruction[35] = 0x1ef7800000000;
  instruction[36] = 0x1ef7800000000;
  instruction[37] = 0x1ef7800000000;
  instruction[38] = 0xf4e5e000015;
  instruction[39] = 0xf4d9e000014;
  instruction[40] = 0x1af4c335c8015;
  instruction[41] = 0x1af4c2d4b0014;
  instruction[42] = 0x1af4eafab8013;
  instruction[43] = 0x1490b1400701c;
  instruction[44] = 0x1af4eaf5a0014;
  instruction[45] = 0x12f4880000003;
  instruction[46] = 0x1af4eafb00016;
  instruction[47] = 0x1af4eae0c0015;
  instruction[48] = 0x1af4f27c98005;
  instruction[49] = 0x1af4f260a0004;
  instruction[50] = 0x1af4f260a0001;
  instruction[51] = 0x1af4f27db0018;
  instruction[52] = 0x1af4f27ea8017;
  instruction[53] = 0x1af494c938007;
  instruction[54] = 0x1af494cb40008;
  instruction[55] = 0x1af494c530006;
  instruction[56] = 0x20f7800000000;
  instruction[57] = 0x20f7800000000;

  
  /** MAIN DYNAMIC PROGRAMMING LOOP **/

  int early_break;

  if (cpu) early_break = accelerator_outer_loop_cpu(instruction, len_y, seq_x, seq_y, len_x, xl_index, xl_ipos, xl_pred_length, index, &best_x, &best_y, &best_score, move, score_rows, m);
  else early_break = accelerator_outer_loop_queue(instruction, len_y, seq_x, seq_y, len_x, xl_index, xl_ipos, xl_pred_length, index, &best_x, &best_y, &best_score, move, score_rows, m);

  // fprintf(stdout, "Max edge for a node: %d \t Longest gap between nodes: %d\n", max_dependency, max_gap);
  
  IF_GUARD(best_x>=len_x || best_y>=len_y,1.1,(ERRTXT,"Bounds exceeded!\nbest_x,best_y:%d,%d\tlen:%d,%d\n",best_x,best_y,len_x,len_y),CRASH);
  
  fprintf (stderr, "aligned (%d nodes, %d edges) to (%d nodes, %d edges): ", len_x, n_edges_x, len_y, n_edges_y);
  fprintf (stderr, "best %s score = %d @ (%d %d)\n", (use_global_alignment ? "global" : "local"), best_score, best_x, best_y);
    
  int y_padding = len_y % 4;
  int x_padding = 4 - 1;

  if (cpu == 0) printf("acc %d\n", len_x * len_y);
  else printf("cpu %d\n", len_x * len_y);

  if (cpu == 0) {
    fprintf(fp_in, ">\n%d %d\n", len_y + y_padding, len_y);
    for (i=0; i<len_y; i++) fprintf(fp_in, "%d\n", seq_y[i].letter);
    for (i=0; i<y_padding; i++) fprintf(fp_in, "%d\n", 4);
    fprintf(fp_in, "%d\n", len_x + x_padding);
    for (i=0; i<len_x; i++) fprintf(fp_in, "%d\n", seq_x[i].letter);
    for (i=0; i<x_padding; i++) fprintf(fp_in, "%d\n", 4);
    fprintf(fp_in, "%d\n", len_x + x_padding);
    for (j=0; j<len_x; j++) fprintf(fp_in, "%d\n", xl_pred_length[j]);
    for (i=0; i<x_padding; i++) fprintf(fp_in, "%d\n", 1);
    fprintf(fp_in, "%d\n", index + x_padding);
    for (j=0; j<index; j++) fprintf(fp_in, "%d\n", xl_ipos[j]);
    for (i=0; i<x_padding; i++) fprintf(fp_in, "%d\n", 1);
    fprintf(fp_in, "%d\n", best_x);
    fprintf(fp_in, "%d\n", best_y);
    fprintf(fp_in, "%d\n", best_score);
  }


  fprintf(fp_out, "Dimension: (%d %d) %d\n", len_y, len_x, len_y * len_x);
  for (i = 0; i < len_y/4; i++) {
    for (j = 0; j < len_x + 3; j++) {
      if (j == 0) fprintf(fp_out, "x x x x x x %d %d \n", move[i*4][0].x, move[i*4][0].y);
      else if (j == 1) fprintf(fp_out, "x x x x %d %d %d %d \n", move[i*4+1][0].x, move[i*4+1][0].y, move[i*4][1].x, move[i*4][1].y);
      else if (j == 2) fprintf(fp_out, "x x %d %d %d %d %d %d \n", move[i*4+2][0].x, move[i*4+2][0].y, move[i*4+1][1].x, move[i*4+1][1].y, move[i*4][2].x, move[i*4][2].y);
      else if (j < len_x) fprintf(fp_out, "%d %d %d %d %d %d %d %d \n", move[i*4+3][j-3].x, move[i*4+3][j-3].y, move[i*4+2][j-2].x, move[i*4+2][j-2].y, move[i*4+1][j-1].x, move[i*4+1][j-1].y, move[i*4][j].x, move[i*4][j].y);
      else if (j == len_x) fprintf(fp_out, "%d %d %d %d %d %d x x \n", move[i*4+3][j-3].x, move[i*4+3][j-3].y, move[i*4+2][j-2].x, move[i*4+2][j-2].y, move[i*4+1][j-1].x, move[i*4+1][j-1].y);
      else if (j == len_x + 1) fprintf(fp_out, "%d %d %d %d x x x x \n", move[i*4+3][j-3].x, move[i*4+3][j-3].y, move[i*4+2][j-2].x, move[i*4+2][j-2].y);
      else fprintf(fp_out, "%d %d x x x x x x \n", move[i*4+3][j-3].x, move[i*4+3][j-3].y);
    }
  }
  if (y_padding == 1)
    for (j = 0; j < len_x + 3; j++) {
      if (j == 0) fprintf(fp_out, "x x x x x x %d %d \n", move[i*4][0].x, move[i*4][0].y);
      else if (j == 1) fprintf(fp_out, "x x x x %d %d %d %d \n", 0, 0, move[i*4][1].x, move[i*4][1].y);
      else if (j == 2) fprintf(fp_out, "x x %d %d %d %d %d %d \n", 0, 0, 0, 0, move[i*4][2].x, move[i*4][2].y);
      else if (j < len_x) fprintf(fp_out, "%d %d %d %d %d %d %d %d \n", 0, 0, 0, 0, 0, 0, move[i*4][j].x, move[i*4][j].y);
      else fprintf(fp_out, "x x x x x x x x \n");
    }
  else if (y_padding == 2)
    for (j = 0; j < len_x + 3; j++) {
      if (j == 0) fprintf(fp_out, "x x x x x x %d %d \n", move[i*4][0].x, move[i*4][0].y);
      else if (j == 1) fprintf(fp_out, "x x x x %d %d %d %d \n", move[i*4+1][0].x, move[i*4+1][0].y, move[i*4][1].x, move[i*4][1].y);
      else if (j == 2) fprintf(fp_out, "x x %d %d %d %d %d %d \n", 0, 0, move[i*4+1][1].x, move[i*4+1][1].y, move[i*4][2].x, move[i*4][2].y);
      else if (j < len_x) fprintf(fp_out, "%d %d %d %d %d %d %d %d \n", 0, 0, 0, 0, move[i*4+1][j-1].x, move[i*4+1][j-1].y, move[i*4][j].x, move[i*4][j].y);
      else if (j == len_x) fprintf(fp_out, "%d %d %d %d %d %d x x \n", 0, 0, 0, 0, move[i*4+1][j-1].x, move[i*4+1][j-1].y);
      else fprintf(fp_out, "x x x x x x x x \n");
    }
  else if (y_padding == 3)
    for (j = 0; j < len_x + 3; j++) {
      if (j == 0) fprintf(fp_out, "x x x x x x %d %d \n", move[i*4][0].x, move[i*4][0].y);
      else if (j == 1) fprintf(fp_out, "x x x x %d %d %d %d \n", move[i*4+1][0].x, move[i*4+1][0].y, move[i*4][1].x, move[i*4][1].y);
      else if (j == 2) fprintf(fp_out, "x x %d %d %d %d %d %d \n", move[i*4+2][0].x, move[i*4+2][0].y, move[i*4+1][1].x, move[i*4+1][1].y, move[i*4][2].x, move[i*4][2].y);
      else if (j < len_x) fprintf(fp_out, "%d %d %d %d %d %d %d %d \n", 0, 0, move[i*4+2][j-2].x, move[i*4+2][j-2].y, move[i*4+1][j-1].x, move[i*4+1][j-1].y, move[i*4][j].x, move[i*4][j].y);
      else if (j == len_x) fprintf(fp_out, "%d %d %d %d %d %d x x \n", 0, 0, move[i*4+2][j-2].x, move[i*4+2][j-2].y, move[i*4+1][j-1].x, move[i*4+1][j-1].y);
      else if (j == len_x + 1) fprintf(fp_out, "%d %d %d %d x x x x \n", 0, 0, move[i*4+2][j-2].x, move[i*4+2][j-2].y);
      else fprintf(fp_out, "%d %d x x x x x x \n", 0, 0);
    }
    

  /* DYNAMIC PROGRAMING MATRIX COMPLETE, NOW TRACE BACK FROM best_x, best_y */
  trace_back_lpo_alignment (len_x, len_y, move, x_left, y_left,
			    best_x, best_y,
			    x_to_y, y_to_x);


  /* CLEAN UP AND RETURN: */
  
  FREE (node_type_x);
  FREE (node_type_y);
  
  FREE (refs_from_right_x);
  FREE (refs_from_right_y);

  
  score_rows[-1] = &(score_rows[-1][-1]);
  FREE (score_rows[-1]);
  score_rows = &(score_rows[-1]);
  FREE (score_rows);
  
    
  for (i=0; i<len_x; i++) {
    if (x_left[i] != &seq_x[i].left) {
      FREE (x_left[i]);
    }
  }
  FREE (x_left);
  
  for (i=0; i<len_y; i++) {
    if (y_left[i] != &seq_y[i].left) {
      FREE (y_left[i]);
    }
  }
  FREE (y_left);
  
  for (i=0; i<len_y; i++) {
    FREE (move[i]);
  }
  FREE (move);
  
  // return best_score;
  return 0;
}


LPOScore_T align_lpo_po_accelerator (LPOSequence_T *lposeq_x,
			 LPOSequence_T *lposeq_y,
			 ResidueScoreMatrix_T *m,
			 LPOLetterRef_T **x_to_y,
			 LPOLetterRef_T **y_to_x,
			 LPOScore_T (*scoring_function)
			 (int, int, LPOLetter_T *, LPOLetter_T *, ResidueScoreMatrix_T *),
			 int use_global_alignment)
{
  LPOLetter_T *seq_x = lposeq_x->letter;
  LPOLetter_T *seq_y = lposeq_y->letter;
  
  int len_x, len_y;
  int n_edges_x, n_edges_y;
  int *node_type_x, *node_type_y;
  int *refs_from_right_x, *refs_from_right_y;
  int max_rows_alloced_x, max_rows_alloced_y, n_score_rows_alloced = 0;
  
  int i, j, xcount, ycount, prev_gap;
  int best_x = -1, best_y = -1;
  LPOScore_T min_score = -999999, best_score = -999999;
  int possible_end_square;
  LPOLetterLink_T **x_left = NULL, **y_left = NULL, *xl, *yl;
  DPMove_T **move = NULL, *my_move;
  
  DPScore_T *curr_score = NULL, *prev_score = NULL, *init_col_score = NULL, *my_score;
  DPScore_T **score_rows = NULL;

  int max_gap_length;
  LPOScore_T *gap_penalty_x, *gap_penalty_y;
  int *next_gap_array, *next_perp_gap_array;
  
  LPOScore_T try_score, insert_x_score, insert_y_score, match_score;
  int pre_gap[MAX_LONG_DEPENDENCY] = {0};
  int max_gap = 0, max_dependency = 0, cycle = 0, cells = 0;
  
  get_lpo_stats (lposeq_x, &len_x, &n_edges_x, &node_type_x, &refs_from_right_x, &max_rows_alloced_x, &x_left);
  get_lpo_stats (lposeq_y, &len_y, &n_edges_y, &node_type_y, &refs_from_right_y, &max_rows_alloced_y, &y_left);

  // fprintf (stdout, "sequence x:  %ld nodes, %ld edges, %ld rows at most --> %ld mem\n", len_x, n_edges_x, max_rows_alloced_x, max_rows_alloced_x * len_y);
  // fprintf (stdout, "sequence y:  %ld nodes, %ld edges, %ld rows at most --> %ld mem\n", len_y, n_edges_y, max_rows_alloced_y, max_rows_alloced_y * len_x);
  
  /* INITIALIZE GAP PENALTIES: */
  max_gap_length = m->max_gap_length;
  gap_penalty_x = m->gap_penalty_x;
  gap_penalty_y = m->gap_penalty_y;
  CALLOC (next_gap_array, max_gap_length + 2, int);
  CALLOC (next_perp_gap_array, max_gap_length + 2, int);

  for (i=0; i<max_gap_length+1; i++) {
    /* GAP LENGTH EXTENSION RULE: */
    /* 0->1, 1->2, 2->3, ..., M-1->M; but M->M. */
    next_gap_array[i] = (i<max_gap_length) ? i+1 : i;
    /* PERPENDICULAR GAP (i.e. X FOR A GROWING Y-GAP) IS KEPT AT 0 IF DOUBLE-GAP-SCORING (old scoring) IS USED. */
    next_perp_gap_array[i] = (DOUBLE_GAP_SCORING ? 0 : next_gap_array[i]);
  }
  
  /* GAP LENGTH = M+1 IS USED FOR INITIAL STATE. */
  /* THIS MUST BE TREATED DIFFERENTLY FOR GLOBAL v. LOCAL ALIGNMENT: */
  gap_penalty_x[max_gap_length+1] = gap_penalty_y[max_gap_length+1] = 0;
  next_gap_array[max_gap_length+1] = next_perp_gap_array[max_gap_length+1] = max_gap_length+1;
  
  /* ALLOCATE MEMORY FOR 'MOVE' AND 'SCORE' MATRICES: */
  
  CALLOC (move, len_y, DPMove_T *);
  for (i=0; i<len_y; i++) {
    CALLOC (move[i], len_x, DPMove_T);
  }

  CALLOC (init_col_score, len_y+1, DPScore_T);
  init_col_score = &(init_col_score[1]);
  
  CALLOC (score_rows, len_y+1, DPScore_T *);
  score_rows = &(score_rows[1]);
  CALLOC (score_rows[-1], len_x+1, DPScore_T);
  score_rows[-1] = &(score_rows[-1][1]);
  curr_score = score_rows[-1];

  /* FILL INITIAL ROW (-1). exixting graph */
  /* GAP LENGTH = M+1 IS USED FOR INITIAL STATE. */
  
  curr_score[-1].score = 0;
  curr_score[-1].gap_x = curr_score[-1].gap_y = max_gap_length+1;
  
  for (i=0; i<len_x; i++) {
    curr_score[i].score = min_score;
    for (xcount = 1, xl = x_left[i]; xl != NULL; xcount++, xl = xl->more) {
      prev_gap = curr_score[xl->ipos].gap_x;
      try_score = curr_score[xl->ipos].score + xl->score - gap_penalty_x[prev_gap];
      if (try_score > curr_score[i].score) {
        curr_score[i].score = try_score;
        curr_score[i].gap_x = next_gap_array[prev_gap];
        curr_score[i].gap_y = next_perp_gap_array[prev_gap];
      }
    }
  }
  
  /* FILL INITIAL COLUMN (-1). new sequence */
  
  init_col_score[-1] = curr_score[-1];
  for (i=0; i<len_y; i++) {
    init_col_score[i].score = min_score;
    for (ycount = 1, yl = y_left[i]; yl != NULL; ycount++, yl = yl->more) {
      prev_gap = init_col_score[yl->ipos].gap_y;
      try_score = init_col_score[yl->ipos].score + yl->score - gap_penalty_y[prev_gap];
      if (try_score > init_col_score[i].score) {
        init_col_score[i].score = try_score;
        init_col_score[i].gap_x = next_perp_gap_array[prev_gap];
        init_col_score[i].gap_y = next_gap_array[prev_gap];
      }
    }
    // printf("%d %d %d\n", init_col_score[i].score, init_col_score[i].gap_x, init_col_score[i].gap_y);
  }

  int *xl_index, *xl_ipos, index, index_accumulation;
  xl_index = (int*)malloc(XL_INDEX_NUM * sizeof(int));
  xl_ipos = (int*)malloc(XL_IPOS_NUM * sizeof(int));
  index = 0;
  index_accumulation = 0;
  xl_index[0] = 0;
  for (j=0; j<len_x; j++) {
    for (xcount = 0, xl = x_left[j]; xl != NULL; xl = xl->more, xcount++){
      // if (j - xl->ipos >= MAX_LONG_DEPENDENCY) return 1;
      xl_ipos[index] = xl->ipos;
      index++;
      index_accumulation++;
    }
    xl_index[j+1] = index_accumulation;
  }
  
  /** MAIN DYNAMIC PROGRAMMING LOOP **/

  // int early_break = accelerator_outer_loop_queue(len_y, seq_x, seq_y, len_x, xl_index, xl_ipos, index, &best_x, &best_y, &best_score, move, score_rows, m);

  // if (early_break) return 1;

  // accelerator_outer_loop_predecessor(len_y, &cycle, &cells, refs_from_right_y, init_col_score, seq_x, seq_y, len_x, node_type_x, node_type_y, &best_x, &best_y, &min_score, &best_score, x_left, y_left, xl, yl, move, my_move, possible_end_square, score_rows, curr_score, prev_score, my_score, gap_penalty_x, gap_penalty_y, next_gap_array, next_perp_gap_array, pre_gap, use_global_alignment, scoring_function, m, max_gap_length, &max_gap, &max_dependency);

  accelerator_outer_loop(len_y, &cycle, &cells, refs_from_right_y, init_col_score, seq_x, seq_y, len_x, node_type_x, node_type_y, &best_x, &best_y, &min_score, &best_score, x_left, y_left, xl, yl, move, my_move, possible_end_square, score_rows, curr_score, prev_score, my_score, gap_penalty_x, gap_penalty_y, next_gap_array, next_perp_gap_array, pre_gap, use_global_alignment, scoring_function, m, max_gap_length, &max_gap, &max_dependency);

  fprintf(stdout, "Max edge for a node: %d \t Longest gap between nodes: %d\n", max_dependency, max_gap);
  
  IF_GUARD(best_x>=len_x || best_y>=len_y,1.1,(ERRTXT,"Bounds exceeded!\nbest_x,best_y:%d,%d\tlen:%d,%d\n",best_x,best_y,len_x,len_y),CRASH);
  
  // printf ("prev_gap:");
  // for (i = 0; i < MAX_LONG_DEPENDENCY; i++) printf(" %d", pre_gap[i]);
  // printf("\n");
  fprintf (stderr, "aligned (%d nodes, %d edges) to (%d nodes, %d edges): ", len_x, n_edges_x, len_y, n_edges_y);
  fprintf (stderr, "best %s score = %d @ (%d %d)\n", (use_global_alignment ? "global" : "local"), best_score, best_x, best_y);
    
  /* DYNAMIC PROGRAMING MATRIX COMPLETE, NOW TRACE BACK FROM best_x, best_y */
  trace_back_lpo_alignment (len_x, len_y, move, x_left, y_left,
			    best_x, best_y,
			    x_to_y, y_to_x);


  /* CLEAN UP AND RETURN: */
  
  FREE (node_type_x);
  FREE (node_type_y);
  
  FREE (refs_from_right_x);
  FREE (refs_from_right_y);

  FREE (next_gap_array);
  FREE (next_perp_gap_array);
  
  score_rows[-1] = &(score_rows[-1][-1]);
  FREE (score_rows[-1]);
  score_rows = &(score_rows[-1]);
  FREE (score_rows);
  
  init_col_score = &(init_col_score[-1]);
  FREE (init_col_score);
    
  for (i=0; i<len_x; i++) {
    if (x_left[i] != &seq_x[i].left) {
      FREE (x_left[i]);
    }
  }
  FREE (x_left);
  
  for (i=0; i<len_y; i++) {
    if (y_left[i] != &seq_y[i].left) {
      FREE (y_left[i]);
    }
  }
  FREE (y_left);
  
  for (i=0; i<len_y; i++) {
    FREE (move[i]);
  }
  FREE (move);
  
  // return best_score;
  return 0;
}

LPOScore_T align_lpo_po (LPOSequence_T *lposeq_x,
			 LPOSequence_T *lposeq_y,
			 ResidueScoreMatrix_T *m,
			 LPOLetterRef_T **x_to_y,
			 LPOLetterRef_T **y_to_x,
			 LPOScore_T (*scoring_function)
			 (int, int, LPOLetter_T *, LPOLetter_T *, ResidueScoreMatrix_T *),
			 int use_global_alignment)
{
  LPOLetter_T *seq_x = lposeq_x->letter;
  LPOLetter_T *seq_y = lposeq_y->letter;
  
  int len_x, len_y;
  int n_edges_x, n_edges_y;
  int *node_type_x, *node_type_y;
  int *refs_from_right_x, *refs_from_right_y;
  int max_rows_alloced_x, max_rows_alloced_y, n_score_rows_alloced = 0;
  
  int i, j, xcount, ycount, prev_gap;
  int best_x = -1, best_y = -1;
  LPOScore_T min_score = -999999, best_score = -999999;
  int possible_end_square;
  LPOLetterLink_T **x_left = NULL, **y_left = NULL, *xl, *yl;
  DPMove_T **move = NULL, *my_move;
  
  DPScore_T *curr_score = NULL, *prev_score = NULL, *init_col_score = NULL, *my_score;
  DPScore_T **score_rows = NULL;

  int max_gap_length;
  LPOScore_T *gap_penalty_x, *gap_penalty_y;
  int *next_gap_array, *next_perp_gap_array;
  
  LPOScore_T try_score, insert_x_score, insert_y_score, match_score;
  int insert_x_x, insert_x_gap;
  int insert_y_y, insert_y_gap;
  int match_x, match_y;
  int node_with_prev_edge = 0;
  int pre_gap[MAX_LONG_DEPENDENCY] = {0};
  int gap_penalty = 0, max_gap = 0, long_dependency = 0;
  
  get_lpo_stats (lposeq_x, &len_x, &n_edges_x, &node_type_x, &refs_from_right_x, &max_rows_alloced_x, &x_left);
  get_lpo_stats (lposeq_y, &len_y, &n_edges_y, &node_type_y, &refs_from_right_y, &max_rows_alloced_y, &y_left);

  // fprintf (stdout, "sequence x:  %ld nodes, %ld edges, %ld rows at most --> %ld mem\n", len_x, n_edges_x, max_rows_alloced_x, max_rows_alloced_x * len_y);
  // fprintf (stdout, "sequence y:  %ld nodes, %ld edges, %ld rows at most --> %ld mem\n", len_y, n_edges_y, max_rows_alloced_y, max_rows_alloced_y * len_x);
  
  /* INITIALIZE GAP PENALTIES: */
  max_gap_length = m->max_gap_length;
  gap_penalty_x = m->gap_penalty_x;
  gap_penalty_y = m->gap_penalty_y;
  CALLOC (next_gap_array, max_gap_length + 2, int);
  CALLOC (next_perp_gap_array, max_gap_length + 2, int);

  for (i=0; i<max_gap_length+1; i++) {
    /* GAP LENGTH EXTENSION RULE: */
    /* 0->1, 1->2, 2->3, ..., M-1->M; but M->M. */
    next_gap_array[i] = (i<max_gap_length) ? i+1 : i;
    // printf("%d\n",next_gap_array[i]);
    /* PERPENDICULAR GAP (i.e. X FOR A GROWING Y-GAP) IS KEPT AT 0 IF DOUBLE-GAP-SCORING (old scoring) IS USED. */
    next_perp_gap_array[i] = (DOUBLE_GAP_SCORING ? 0 : next_gap_array[i]);
  }
  
  /* GAP LENGTH = M+1 IS USED FOR INITIAL STATE. */
  /* THIS MUST BE TREATED DIFFERENTLY FOR GLOBAL v. LOCAL ALIGNMENT: */
  if (0 == use_global_alignment) {   /* FREE EXTENSION OF INITIAL GAP (FOR LOCAL ALIGNMENT) */
    gap_penalty_x[max_gap_length+1] = gap_penalty_y[max_gap_length+1] = 0;
    next_gap_array[max_gap_length+1] = next_perp_gap_array[max_gap_length+1] = max_gap_length+1;
  }
  else {   /* TREAT INITIAL GAP LIKE ANY OTHER (FOR GLOBAL ALIGNMENT) */
    gap_penalty_x[max_gap_length+1] = gap_penalty_x[0];
    gap_penalty_y[max_gap_length+1] = gap_penalty_y[0];
    next_gap_array[max_gap_length+1] = next_gap_array[0];
    next_perp_gap_array[max_gap_length+1] = next_perp_gap_array[0];
  }

  // for (i = 0; i <= max_gap_length+1; i++) printf("%d, %d\n", gap_penalty_x[i], gap_penalty_y[i]);
  // printf("\n");
  
  /* ALLOCATE MEMORY FOR 'MOVE' AND 'SCORE' MATRICES: */
  
  CALLOC (move, len_y, DPMove_T *);
  for (i=0; i<len_y; i++) {
    CALLOC (move[i], len_x, DPMove_T);
  }

  CALLOC (init_col_score, len_y+1, DPScore_T);
  init_col_score = &(init_col_score[1]);
  
  CALLOC (score_rows, len_y+1, DPScore_T *);
  score_rows = &(score_rows[1]);
  CALLOC (score_rows[-1], len_x+1, DPScore_T);
  score_rows[-1] = &(score_rows[-1][1]);
  curr_score = score_rows[-1];

  /* FILL INITIAL ROW (-1). exixting graph */
  /* GAP LENGTH = M+1 IS USED FOR INITIAL STATE. */
  
  curr_score[-1].score = 0;
  curr_score[-1].gap_x = curr_score[-1].gap_y = max_gap_length+1;
  
  for (i=0; i<len_x; i++) {
    curr_score[i].score = min_score;
    for (xcount = 1, xl = x_left[i]; xl != NULL; xcount++, xl = xl->more) {
      prev_gap = curr_score[xl->ipos].gap_x;              // prev_gap [0, 17]
      if (prev_gap == 0) gap_penalty = gap_penalty_x[0];  // gap_penalty[0] = 12,  gap_penalty[1-16] = 6, gap_penalty[17] = 0
      else if (prev_gap == max_gap_length + 1) gap_penalty = gap_penalty_x[max_gap_length + 1];
      else gap_penalty = gap_penalty_x[1];
      try_score = curr_score[xl->ipos].score + xl->score - gap_penalty; // gap_penalty_x[prev_gap];
      if (try_score > curr_score[i].score) {
        curr_score[i].score = try_score;
        curr_score[i].gap_x = next_gap_array[prev_gap];   // {1,2,3,4,5,6,7,8,9,10...16,16}
        curr_score[i].gap_y = next_perp_gap_array[prev_gap];  // All initialized to 17
        if (next_gap_array[prev_gap] != next_perp_gap_array[prev_gap]) printf("!");
        // printf("%d ", curr_score[i].gap_x);
      } // else printf("!");
    }
  }
  
  /* FILL INITIAL COLUMN (-1). new sequence */
  
  init_col_score[-1] = curr_score[-1];
  for (i=0; i<len_y; i++) {
    init_col_score[i].score = min_score;
    for (ycount = 1, yl = y_left[i]; yl != NULL; ycount++, yl = yl->more) {
      prev_gap = init_col_score[yl->ipos].gap_y;
      if (prev_gap == 0) gap_penalty = gap_penalty_y[0];
      else if (prev_gap == max_gap_length + 1) gap_penalty = gap_penalty_y[max_gap_length + 1];
      else gap_penalty = gap_penalty_y[1];
      try_score = init_col_score[yl->ipos].score + yl->score - gap_penalty; // gap_penalty_y[prev_gap];
      if (try_score > init_col_score[i].score) {
        init_col_score[i].score = try_score;
        init_col_score[i].gap_x = next_perp_gap_array[prev_gap];
        init_col_score[i].gap_y = next_gap_array[prev_gap];
        if (next_gap_array[prev_gap] != next_perp_gap_array[prev_gap]) printf("!");
        // printf("%d ", curr_score[i].gap_x);
      } // else printf("!");
    }
  }

  /** MAIN DYNAMIC PROGRAMMING LOOP **/

  /* OUTER LOOP (i-th position in LPO y): */
  for (i=0; i<len_y; i++) {
    
    /* ALLOCATE MEMORY FOR 'SCORE' ROW i: */
    CALLOC (score_rows[i], len_x+1, DPScore_T);
    score_rows[i] = &(score_rows[i][1]);
    n_score_rows_alloced++;
        
    curr_score = score_rows[i];
    curr_score[-1] = init_col_score[i];
          
    /* INNER LOOP (j-th position in LPO x): */
    for (j=0; j<len_x; j++) {

      match_score = (use_global_alignment) ? min_score : 0;
      match_x = match_y = 0;
      
      insert_x_score = insert_y_score = min_score;
      insert_x_x = insert_y_y = 0;
      insert_x_gap = insert_y_gap = 0;
      
      /* THIS SQUARE CAN END THE ALIGNMENT IF WE'RE USING LOCAL ALIGNMENT, */
      /* OR IF BOTH THE X- AND Y-NODES CONTAIN THE END OF A SEQUENCE. */
      possible_end_square = ((0 == use_global_alignment) || ((node_type_x[j] & LPO_FINAL_NODE) && (node_type_y[i] & LPO_FINAL_NODE)));


      ycount = 1; yl = y_left[i];
      // if (ycount != 1) printf("y %d\n", ycount);
      prev_score = score_rows[yl->ipos];
      
      /* IMPROVE Y-INSERTION?: trace back to (i'=yl->ipos, j) */

      prev_gap = prev_score[j].gap_y;
      gap_penalty = gap_penalty_y[prev_gap];

      // if (prev_gap == 0) gap_penalty = gap_penalty_y[0];
      // else if (prev_gap == max_gap_length + 1) gap_penalty = gap_penalty_y[max_gap_length + 1];
      // else gap_penalty = gap_penalty_y[1];

      try_score = prev_score[j].score + yl->score - gap_penalty; // gap_penalty_y[prev_gap];
      if (try_score > insert_y_score) {
        insert_y_score = try_score;
        insert_y_y = ycount;
        insert_y_gap = prev_gap;
      } else printf("!");
      
      /* LOOP OVER x-predecessors (OUTSIDE y-predecessor LOOP): */
      for (xcount = 1, xl = x_left[j]; xl != NULL; xcount++, xl = xl->more) {

        /* IMPROVE X-INSERTION?: trace back to (i, j'=xl->ipos) */

        prev_gap = curr_score[xl->ipos].gap_x;
        gap_penalty = gap_penalty_x[prev_gap];

        // if (prev_gap == 0) gap_penalty = gap_penalty_x[0];
        // else if (prev_gap == max_gap_length + 1) gap_penalty = gap_penalty_x[max_gap_length + 1];
        // else gap_penalty = gap_penalty_x[1];

        long_dependency = j - xl->ipos;
        if (long_dependency < MAX_LONG_DEPENDENCY) pre_gap[long_dependency]++;
        if (long_dependency > max_gap) max_gap = long_dependency;
        try_score = curr_score[xl->ipos].score + xl->score - gap_penalty; // gap_penalty_x[prev_gap];
        if (try_score > insert_x_score) {
          insert_x_score = try_score;
          insert_x_x = xcount;
          insert_x_gap = prev_gap;
        }

        try_score = prev_score[xl->ipos].score + xl->score + yl->score;
        if (try_score > match_score) {
          match_score = try_score;
          match_x = xcount;
          match_y = ycount;
        }
      }
      
      /* USE CUSTOM OR DEFAULT SCORING FUNCTION: */
      if (scoring_function != NULL) {
	      match_score += scoring_function (j, i, seq_x, seq_y, m);
      }
      else {
	      match_score += m->score[seq_x[i].letter][seq_y[j].letter];
      }
      
      my_score = &curr_score[j];
      my_move = &move[i][j];
      
      if (match_score > insert_y_score && match_score > insert_x_score) {
        /* XY-MATCH */
        my_score->score = match_score;
        my_score->gap_x = 0;
        my_score->gap_y = 0;
        my_move->x = match_x;
        my_move->y = match_y;
      }
      else if (insert_x_score > insert_y_score) {
        /* X-INSERTION */
        my_score->score = insert_x_score;
        my_score->gap_x = next_gap_array[insert_x_gap];
        my_score->gap_y = next_perp_gap_array[insert_x_gap];
        my_move->x = insert_x_x;
        my_move->y = 0;
        if (next_gap_array[insert_x_gap] != next_perp_gap_array[insert_x_gap]) printf("!");
      }
      else {
        /* Y-INSERTION */
        my_score->score = insert_y_score;
        my_score->gap_x = next_perp_gap_array[insert_y_gap];
        my_score->gap_y = next_gap_array[insert_y_gap];
        my_move->x = 0;
        my_move->y = insert_y_y;
        if (next_gap_array[insert_y_gap] != next_perp_gap_array[insert_y_gap]) printf("!");
      }

      /* RECORD BEST ALIGNMENT END FOR TRACEBACK: */
      if (possible_end_square && my_score->score >= best_score) {
        /* BREAK TIES BY CHOOSING MINIMUM (x,y): */
        if (my_score->score > best_score || (j == best_x && i < best_y) || j < best_x) {
          best_score = my_score->score;
          best_x = j;
          best_y = i;
          // printf("%d ", best_score);
        }
      }
    }
      // printf("\n");

    /* UPDATE # OF REFS TO 'SCORE' ROWS; FREE MEMORY WHEN POSSIBLE: */
    for (yl = y_left[i]; yl != NULL; yl = yl->more) if ((j = yl->ipos) >= 0) {
      if ((--refs_from_right_y[j]) == 0) {
        score_rows[j] = &(score_rows[j][-1]);
        FREE (score_rows[j]);
        n_score_rows_alloced--;
      }
    }
    if (refs_from_right_y[i] == 0) {
      score_rows[i] = &(score_rows[i][-1]);
      FREE (score_rows[i]);
      n_score_rows_alloced--;
    }
  }
  
  IF_GUARD(best_x>=len_x || best_y>=len_y,1.1,(ERRTXT,"Bounds exceeded!\nbest_x,best_y:%d,%d\tlen:%d,%d\n",best_x,best_y,len_x,len_y),CRASH);
  
  /**/
    // printf ("prev_gap:");
    // for (i = 0; i < 127; i++) printf(" %d", pre_gap[i]);
    // printf("%d\n", max_gap);
    fprintf (stderr, "aligned (%d nodes, %ld edges) to (%d nodes, %ld edges): ", len_x, n_edges_x, len_y, n_edges_y);
    fprintf (stderr, "best %s score = %d @ (%d %d)\n", (use_global_alignment ? "global" : "local"), best_score, best_x, best_y);
    /**/
    
  /* DYNAMIC PROGRAMING MATRIX COMPLETE, NOW TRACE BACK FROM best_x, best_y */
  trace_back_lpo_alignment (len_x, len_y, move, x_left, y_left,
			    best_x, best_y,
			    x_to_y, y_to_x);


  /* CLEAN UP AND RETURN: */
  
  FREE (node_type_x);
  FREE (node_type_y);
  
  FREE (refs_from_right_x);
  FREE (refs_from_right_y);

  FREE (next_gap_array);
  FREE (next_perp_gap_array);
  
  score_rows[-1] = &(score_rows[-1][-1]);
  FREE (score_rows[-1]);
  score_rows = &(score_rows[-1]);
  FREE (score_rows);
  
  init_col_score = &(init_col_score[-1]);
  FREE (init_col_score);
    
  for (i=0; i<len_x; i++) {
    if (x_left[i] != &seq_x[i].left) {
      FREE (x_left[i]);
    }
  }
  FREE (x_left);
  
  for (i=0; i<len_y; i++) {
    if (y_left[i] != &seq_y[i].left) {
      FREE (y_left[i]);
    }
  }
  FREE (y_left);
  
  for (i=0; i<len_y; i++) {
    FREE (move[i]);
  }
  FREE (move);
  
  return best_score;
}


LPOScore_T align_lpo_po_original (LPOSequence_T *lposeq_x,
			 LPOSequence_T *lposeq_y,
			 ResidueScoreMatrix_T *m,
			 LPOLetterRef_T **x_to_y,
			 LPOLetterRef_T **y_to_x,
			 LPOScore_T (*scoring_function)
			 (int, int, LPOLetter_T *, LPOLetter_T *, ResidueScoreMatrix_T *),
			 int use_global_alignment)
{
  LPOLetter_T *seq_x = lposeq_x->letter;
  LPOLetter_T *seq_y = lposeq_y->letter;
  
  int len_x, len_y;
  int n_edges_x, n_edges_y;
  int *node_type_x, *node_type_y;
  int *refs_from_right_x, *refs_from_right_y;
  int max_rows_alloced_x, max_rows_alloced_y, n_score_rows_alloced = 0;
  
  int i, j, xcount, ycount, prev_gap;
  int best_x = -1, best_y = -1;
  LPOScore_T min_score = -999999, best_score = -999999;
  int possible_end_square;
  LPOLetterLink_T **x_left = NULL, **y_left = NULL, *xl, *yl;
  DPMove_T **move = NULL, *my_move;
  
  DPScore_T *curr_score = NULL, *prev_score = NULL, *init_col_score = NULL, *my_score;
  DPScore_T **score_rows = NULL;

  int max_gap_length;
  LPOScore_T *gap_penalty_x, *gap_penalty_y;
  int *next_gap_array, *next_perp_gap_array;
  
  LPOScore_T try_score, insert_x_score, insert_y_score, match_score;
  int insert_x_x, insert_x_gap;
  int insert_y_y, insert_y_gap;
  int match_x, match_y;
  int node_with_prev_edge = 0;
  int pre_gap[20] = {0};
  
  get_lpo_stats (lposeq_x, &len_x, &n_edges_x, &node_type_x, &refs_from_right_x, &max_rows_alloced_x, &x_left);
  get_lpo_stats (lposeq_y, &len_y, &n_edges_y, &node_type_y, &refs_from_right_y, &max_rows_alloced_y, &y_left);

  fprintf (stdout, "sequence x:  %ld nodes, %ld edges, %ld rows at most --> %ld mem\n", len_x, n_edges_x, max_rows_alloced_x, max_rows_alloced_x * len_y);
  fprintf (stdout, "sequence y:  %ld nodes, %ld edges, %ld rows at most --> %ld mem\n", len_y, n_edges_y, max_rows_alloced_y, max_rows_alloced_y * len_x);
  
  /* INITIALIZE GAP PENALTIES: */
  max_gap_length = m->max_gap_length;
  gap_penalty_x = m->gap_penalty_x;
  gap_penalty_y = m->gap_penalty_y;
  CALLOC (next_gap_array, max_gap_length + 2, int);
  CALLOC (next_perp_gap_array, max_gap_length + 2, int);

  for (i=0; i<max_gap_length+1; i++) {
    /* GAP LENGTH EXTENSION RULE: */
    /* 0->1, 1->2, 2->3, ..., M-1->M; but M->M. */
    next_gap_array[i] = (i<max_gap_length) ? i+1 : i;
    /* PERPENDICULAR GAP (i.e. X FOR A GROWING Y-GAP) IS KEPT AT 0 IF DOUBLE-GAP-SCORING (old scoring) IS USED. */
    next_perp_gap_array[i] = (DOUBLE_GAP_SCORING ? 0 : next_gap_array[i]);
  }
  
  /* GAP LENGTH = M+1 IS USED FOR INITIAL STATE. */
  /* THIS MUST BE TREATED DIFFERENTLY FOR GLOBAL v. LOCAL ALIGNMENT: */
  if (0 == use_global_alignment) {   /* FREE EXTENSION OF INITIAL GAP (FOR LOCAL ALIGNMENT) */
    gap_penalty_x[max_gap_length+1] = gap_penalty_y[max_gap_length+1] = 0;
    next_gap_array[max_gap_length+1] = next_perp_gap_array[max_gap_length+1] = max_gap_length+1;
  }
  else {   /* TREAT INITIAL GAP LIKE ANY OTHER (FOR GLOBAL ALIGNMENT) */
    gap_penalty_x[max_gap_length+1] = gap_penalty_x[0];
    gap_penalty_y[max_gap_length+1] = gap_penalty_y[0];
    next_gap_array[max_gap_length+1] = next_gap_array[0];
    next_perp_gap_array[max_gap_length+1] = next_perp_gap_array[0];
  }
  
  
  /* ALLOCATE MEMORY FOR 'MOVE' AND 'SCORE' MATRICES: */
  
  CALLOC (move, len_y, DPMove_T *);
  for (i=0; i<len_y; i++) {
    CALLOC (move[i], len_x, DPMove_T);
  }

  CALLOC (init_col_score, len_y+1, DPScore_T);
  init_col_score = &(init_col_score[1]);
  
  CALLOC (score_rows, len_y+1, DPScore_T *);
  score_rows = &(score_rows[1]);
  CALLOC (score_rows[-1], len_x+1, DPScore_T);
  score_rows[-1] = &(score_rows[-1][1]);
  curr_score = score_rows[-1];


  /* FILL INITIAL ROW (-1). exixting graph */
  /* GAP LENGTH = M+1 IS USED FOR INITIAL STATE. */
  
  curr_score[-1].score = 0;
  curr_score[-1].gap_x = curr_score[-1].gap_y = max_gap_length+1;
  
  for (i=0; i<len_x; i++) {
    curr_score[i].score = min_score;
    for (xcount = 1, xl = x_left[i]; xl != NULL; xcount++, xl = xl->more) {
      prev_gap = curr_score[xl->ipos].gap_x;
      try_score = curr_score[xl->ipos].score + xl->score - gap_penalty_x[prev_gap];
      if (try_score > curr_score[i].score) {
        curr_score[i].score = try_score;
        curr_score[i].gap_x = next_gap_array[prev_gap];
        curr_score[i].gap_y = next_perp_gap_array[prev_gap];
      }
    }
  }
  
  /* FILL INITIAL COLUMN (-1). new sequence */
  
  init_col_score[-1] = curr_score[-1];
  for (i=0; i<len_y; i++) {
    init_col_score[i].score = min_score;
    for (ycount = 1, yl = y_left[i]; yl != NULL; ycount++, yl = yl->more) {
      prev_gap = init_col_score[yl->ipos].gap_y;
      try_score = init_col_score[yl->ipos].score + yl->score - gap_penalty_y[prev_gap];
      if (try_score > init_col_score[i].score) {
        init_col_score[i].score = try_score;
        init_col_score[i].gap_x = next_perp_gap_array[prev_gap];
        init_col_score[i].gap_y = next_gap_array[prev_gap];
      }
    }
  }

  
  /** MAIN DYNAMIC PROGRAMMING LOOP **/
  printf("aligh\n");


  /* OUTER LOOP (i-th position in LPO y): */
  for (i=0; i<len_y; i++) {
    
    /* ALLOCATE MEMORY FOR 'SCORE' ROW i: */
    CALLOC (score_rows[i], len_x+1, DPScore_T);
    score_rows[i] = &(score_rows[i][1]);
    n_score_rows_alloced++;
        
    curr_score = score_rows[i];
    curr_score[-1] = init_col_score[i];
          
    /* INNER LOOP (j-th position in LPO x): */
    for (j=0; j<len_x; j++) {

      match_score = (use_global_alignment) ? min_score : 0;
      match_x = match_y = 0;
      
      insert_x_score = insert_y_score = min_score;
      insert_x_x = insert_y_y = 0;
      insert_x_gap = insert_y_gap = 0;
      
      /* THIS SQUARE CAN END THE ALIGNMENT IF WE'RE USING LOCAL ALIGNMENT, */
      /* OR IF BOTH THE X- AND Y-NODES CONTAIN THE END OF A SEQUENCE. */
      possible_end_square = ((0 == use_global_alignment) || ((node_type_x[j] & LPO_FINAL_NODE) && (node_type_y[i] & LPO_FINAL_NODE)));
      
      /* LOOP OVER y-predecessors: */

      for (ycount = 1, yl = y_left[i]; yl != NULL; ycount++, yl = yl->more) {
	
        prev_score = score_rows[yl->ipos];
        
        /* IMPROVE Y-INSERTION?: trace back to (i'=yl->ipos, j) */
        prev_gap = prev_score[j].gap_y;
        try_score = prev_score[j].score + yl->score - gap_penalty_y[prev_gap];
        if (try_score > insert_y_score) {
          insert_y_score = try_score;
          insert_y_y = ycount;
          insert_y_gap = prev_gap;
        }
        
        /* LOOP OVER x-predecessors (INSIDE y-predecessor LOOP): */
        for (xcount = 1, xl = x_left[j]; xl != NULL; xcount++, xl = xl->more) {
          
          /* IMPROVE XY-MATCH?: trace back to (i'=yl->ipos, j'=xl->ipos) */
          try_score = prev_score[xl->ipos].score + xl->score + yl->score;
          if (try_score > match_score) {
            match_score = try_score;
            match_x = xcount;
            match_y = ycount;
          }
        }
      }
      
      /* LOOP OVER x-predecessors (OUTSIDE y-predecessor LOOP): */
      for (xcount = 1, xl = x_left[j]; xl != NULL; xcount++, xl = xl->more) {

        /* IMPROVE X-INSERTION?: trace back to (i, j'=xl->ipos) */
        prev_gap = curr_score[xl->ipos].gap_x;
        pre_gap[prev_gap]++;
        try_score = curr_score[xl->ipos].score + xl->score - gap_penalty_x[prev_gap];
        if (try_score > insert_x_score) {
          insert_x_score = try_score;
          insert_x_x = xcount;
          insert_x_gap = prev_gap;
        }
      }
      // if (xcount > 2) pre_gap[0]++;
      
      /* USE CUSTOM OR DEFAULT SCORING FUNCTION: */
      if (scoring_function != NULL) {
	      match_score += scoring_function (j, i, seq_x, seq_y, m);
      }
      else {
	      match_score += m->score[seq_x[i].letter][seq_y[j].letter];
      }
      
      my_score = &curr_score[j];
      my_move = &move[i][j];
      
      if (match_score > insert_y_score && match_score > insert_x_score) {
        /* XY-MATCH */
        my_score->score = match_score;
        my_score->gap_x = 0;
        my_score->gap_y = 0;
        my_move->x = match_x;
        my_move->y = match_y;
      }
      else if (insert_x_score > insert_y_score) {
        /* X-INSERTION */
        my_score->score = insert_x_score;
        my_score->gap_x = next_gap_array[insert_x_gap];
        my_score->gap_y = next_perp_gap_array[insert_x_gap];
        my_move->x = insert_x_x;
        my_move->y = 0;
      }
      else {
        /* Y-INSERTION */
        my_score->score = insert_y_score;
        my_score->gap_x = next_perp_gap_array[insert_y_gap];
        my_score->gap_y = next_gap_array[insert_y_gap];
        my_move->x = 0;
        my_move->y = insert_y_y;
      }

      /* RECORD BEST ALIGNMENT END FOR TRACEBACK: */
      if (possible_end_square && my_score->score >= best_score) {
        /* BREAK TIES BY CHOOSING MINIMUM (x,y): */
        if (my_score->score > best_score || (j == best_x && i < best_y) || j < best_x) {
          best_score = my_score->score;
          best_x = j;
          best_y = i;
        }
      }
    }

    /* UPDATE # OF REFS TO 'SCORE' ROWS; FREE MEMORY WHEN POSSIBLE: */
    for (yl = y_left[i]; yl != NULL; yl = yl->more) if ((j = yl->ipos) >= 0) {
      if ((--refs_from_right_y[j]) == 0) {
        score_rows[j] = &(score_rows[j][-1]);
        FREE (score_rows[j]);
        n_score_rows_alloced--;
      }
    }
    if (refs_from_right_y[i] == 0) {
      score_rows[i] = &(score_rows[i][-1]);
      FREE (score_rows[i]);
      n_score_rows_alloced--;
    }
  }
  
  IF_GUARD(best_x>=len_x || best_y>=len_y,1.1,(ERRTXT,"Bounds exceeded!\nbest_x,best_y:%d,%d\tlen:%d,%d\n",best_x,best_y,len_x,len_y),CRASH);
  
  /**/
    printf ("prev_gap:");
    for (i = 0; i < 20; i++) printf(" %d", pre_gap[i]);
    fprintf (stderr, "aligned (%d nodes, %ld edges) to (%d nodes, %ld edges): ", len_x, n_edges_x, len_y, n_edges_y);
    fprintf (stderr, "best %s score = %d @ (%d %d)\n", (use_global_alignment ? "global" : "local"), best_score, best_x, best_y);
    /**/
    
  /* DYNAMIC PROGRAMING MATRIX COMPLETE, NOW TRACE BACK FROM best_x, best_y */
  trace_back_lpo_alignment (len_x, len_y, move, x_left, y_left,
			    best_x, best_y,
			    x_to_y, y_to_x);


  /* CLEAN UP AND RETURN: */
  
  FREE (node_type_x);
  FREE (node_type_y);
  
  FREE (refs_from_right_x);
  FREE (refs_from_right_y);

  FREE (next_gap_array);
  FREE (next_perp_gap_array);
  
  score_rows[-1] = &(score_rows[-1][-1]);
  FREE (score_rows[-1]);
  score_rows = &(score_rows[-1]);
  FREE (score_rows);
  
  init_col_score = &(init_col_score[-1]);
  FREE (init_col_score);
    
  for (i=0; i<len_x; i++) {
    if (x_left[i] != &seq_x[i].left) {
      FREE (x_left[i]);
    }
  }
  FREE (x_left);
  
  for (i=0; i<len_y; i++) {
    if (y_left[i] != &seq_y[i].left) {
      FREE (y_left[i]);
    }
  }
  FREE (y_left);
  
  for (i=0; i<len_y; i++) {
    FREE (move[i]);
  }
  FREE (move);
  
  return best_score;
}


