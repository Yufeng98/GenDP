#ifndef PAIRHMM_SCALARIMPL_H
#define PAIRHMM_SCALARIMPL_H

#include <vector>
#include <new>
#include <cmath>
#include <limits>
#include <iostream>
#include <cxxabi.h>
#include <xmmintrin.h>
#include "pairhmm_impl.h"
#include "constants.h"
#include "diagonals.h"
#include "simulation.h"

#define NUM_FRACTION_BITS 16
#define MAX_RANGE NUM_FRACTION_BITS
#define NUM_INTEGER_BITS 5
#define TRISTATE_CORRECTION 3
#define doNotUseTristateCorrection 0
#define NOCOMPUTETH -10

template <class PRECISION>
struct constants_app{
  std::vector<PRECISION> mm;
  std::vector<PRECISION> gm;
  std::vector<PRECISION> mx;
  std::vector<PRECISION> xx;
  std::vector<PRECISION> my;
  std::vector<PRECISION> yy;
};

template <class PRECISION>
class PairhmmScalarImpl: public PairhmmImpl<PRECISION, Diagonals3<PRECISION>, Constants<PRECISION>, 1>  {
  using Base =  PairhmmImpl<PRECISION,Diagonals3<PRECISION>, Constants<PRECISION>, 1>;

public:
  PairhmmScalarImpl(const size_t initial_size = Base::INITIAL_SIZE): Base {initial_size} { }
  virtual ~PairhmmScalarImpl() { }

  int* LOGSUM_TABLE_ENTRY = createLogSumTableEntry();
  int* LOGSUM_VALUE_ENTRY = createLogSumValueEntry();
  int* QUAL2PROB_TABLE = createQual2ProbTable();
  int* QUAL2ERROR_DIV3_TABLE = createQual2ErrorDiv3Table();
  int LOG2CURRENT_ERROR = Float2Fix((float)(log(0.2) / log(2)));
  int EARLYSTOP_THRESHOLD = Float2Fix((float)(log(pow(10, NOCOMPUTETH))/log(2))) + Upper_LOG2_accurate(INITIAL_CONDITION_UP);
 
  float INITIAL_CONDITION = (float)pow(2, 127);
  float INITIAL_CONDITION_LOG10 = (float)log10(INITIAL_CONDITION);
  float INITIAL_CONDITION_UP = INITIAL_CONDITION;
  // float INITIAL_CONDITION_UP = (float)pow(2, pow(2, NUM_INTEGER_BITS)-1);
  int MIN_INTEGER = -pow(2, NUM_FRACTION_BITS+NUM_INTEGER_BITS);

  int Float2Fix(float exact_value) {
    if (exact_value == - std::numeric_limits<float>::infinity())
      return MIN_INTEGER;
    int result = (int)ceil(exact_value * pow(2, NUM_FRACTION_BITS));
    return result;
  }

  float Fix2Float(int integer) {
    float result = (float)(integer / pow(2, NUM_FRACTION_BITS));
    return result;
  }

  int* createLogSumTableEntry(){
    int *result;
    result = (int*)malloc((MAX_RANGE + 1) * sizeof(int));
    for(int i = 0; i < MAX_RANGE + 1; i++){
        result[i] = -1 * i;
    }
    return result;
  }

  int* createLogSumValueEntry(){
    int *result;
    result = (int*)malloc((MAX_RANGE + 1) * sizeof(int));
    for(int i = 0; i < MAX_RANGE + 1; i++){
        result[i] = Float2Fix((log(1 + pow(2, LOGSUM_TABLE_ENTRY[i])) / log(2)));
    }
    return result;
  }

  int* createQual2ProbTable(){
    int *result = new int[256];   //256 because quality score is a byte
    for(int i = 0; i < 64; i++){
        result[i] = Upper_LOG2_accurate((float)(1 - this->m_ph2pr[i]));
    }
    for(int i = 64; i < 256; i++){
        result[i] = result[63];
    }
    return result; 
  }
  
  int* createQual2ErrorDiv3Table(){
    int *result = new int[256];
    for(int i = 0; i < 64; i++){
        result[i] = Upper_LOG2_accurate((float)this->m_ph2pr[i] / TRISTATE_CORRECTION);
    }
    for(int i = 64; i < 256; i++){
        result[i] = result[63];       
    }
    return result; 
  }

  int Upper_LOG2_accurate(float num){
    float numLog2 = log(num) / log(2);
    int result = Float2Fix(numLog2);
    return result;
  }

// int x0 = m_app[r-1][h] + consts_app->mx[r];      1
//     int x1 = x_app[r-1][h] + consts_app->xx[r];  1

// int Upper_LOGSUM(int num1, int num2){            2
// sub((x_app[r-1][h] + consts_app->xx[r])) - x_app[r-1][h] + consts_app->xx[r]) 
// std::max(num1, num2)
//     int diff = std::min(MAX_RANGE, >> NUM_FRACTION_BITS); //14
//     int result =  + LOGSUM_VALUE_ENTRY[diff];
//     return result;
//   }
  int Upper_LOGSUM(int num1, int num2){
    int diff_num1_num2 = num1 - num2;
    int diff = std::min(MAX_RANGE, abs(diff_num1_num2) >> NUM_FRACTION_BITS);
    int result = std::max(num1, num2) + LOGSUM_VALUE_ENTRY[diff];
    return result;
  }

  int LUT(int diff) {
    int _diff = std::min(MAX_RANGE, abs(diff) >> NUM_FRACTION_BITS);
    return LOGSUM_VALUE_ENTRY[_diff];
  }

/*
0~15
00001
16~31
11110
10000
- -
max
min
shift >> 16
max
+

*/

  float Upper_LOG2_float(int integer){
    float numLog2 = Fix2Float(integer);
    float result = exp(numLog2 * log(2));
    return result;
  }

  void execute_instrution(int id, long instruction, comp_decoder* decoder_unit, compute_unit_32* cu, int* regfile) {
    int i, op[3], in_addr[6], *out_addr, input[6];
    out_addr = (int*)malloc(sizeof(int));
    // printf("PC: %d\t", id/2);
    decoder_unit->execute(instruction, op, in_addr, out_addr, &i);
    for (i = 0; i < 6; i++) {
      input[i] = regfile[in_addr[i]];
    }
    regfile[*out_addr] = cu->execute(op, input);
  }

protected:

  float accelerator(int *result, int r, int h, int rl, int hl, const int read_base, const int hap_base, const int qual, std::vector<std::vector<int> > *m, std::vector<std::vector<int> > *x, std::vector<std::vector<int> > *y, struct constants_app<int> *consts_app){
    std::vector<std::vector<int> > &m_app = *m;
    std::vector<std::vector<int> > &x_app = *x;
    std::vector<std::vector<int> > &y_app = *y;
    const int prior = ((read_base == hap_base) || (read_base == 'N') || (hap_base == 'N')) ?  QUAL2PROB_TABLE[qual] : QUAL2ERROR_DIV3_TABLE[qual];
    int tm = prior + m_app[r-1][h-1] + consts_app->mm[r];
    int tx = prior + x_app[r-1][h-1] + consts_app->gm[r];
    int ty = prior + y_app[r-1][h-1] + consts_app->gm[r]; // 4
    m_app[r][h] = Upper_LOGSUM(tm, Upper_LOGSUM(tx, ty)); // 2*2
    int x0 = m_app[r-1][h] + consts_app->mx[r];
    int x1 = x_app[r-1][h] + consts_app->xx[r];
    int y0 = m_app[r][h-1] + consts_app->my[r];
    int y1 = y_app[r][h-1] + consts_app->yy[r];           // 4
    x_app[r][h] = Upper_LOGSUM(x0, x1);
    y_app[r][h] = Upper_LOGSUM(y0, y1);                   // 2*2
    // 3 pre_value = value
    // 12(7) 19+1(10) 16
    if (r == rl && h == hl) (*result) = Upper_LOGSUM(m_app[rl][h], x_app[rl][h]);
    return 0;
  }

  float accelerator_instruction(unsigned long *instruction, int* regfile, int *result, int r, int h, int rl, int hl, const int read_base, const int hap_base, const int qual, std::vector<std::vector<int> > *m, std::vector<std::vector<int> > *x, std::vector<std::vector<int> > *y, struct constants_app<int> *consts_app, compute_unit_32* cu, comp_decoder* decoder_unit){

    int t;

    std::vector<std::vector<int> > &m_app = *m;
    std::vector<std::vector<int> > &x_app = *x;
    std::vector<std::vector<int> > &y_app = *y;

    regfile[6] = m_app[r-1][h-1];
    regfile[7] = x_app[r-1][h-1];
    regfile[8] = y_app[r-1][h-1];
    regfile[9] = m_app[r-1][h];
    regfile[10] = x_app[r-1][h];
    regfile[23] = m_app[r][h-1];
    regfile[25] = y_app[r][h-1];
    regfile[13] = read_base;
    regfile[14] = hap_base;
    regfile[15] = qual;

    regfile[16];  // prior diff_xy diff_tmxy
    regfile[17];  // tx txy
    regfile[18];  // ty diff_my max_mx
    regfile[19];  // tm
    regfile[20];  // max_my
    regfile[21];  // diff_mx
    regfile[22];  // result
    regfile[23];  // new_m
    regfile[24];  // new_x
    regfile[25];  // new_y
    regfile[26];  // left_x
    regfile[27];  // padding_flag

    if (regfile[13] == 65) regfile[13] = 0;
    else if (regfile[13] == 67) regfile[13] = 1;
    else if (regfile[13] == 71) regfile[13] = 2;
    else if (regfile[13] == 84) regfile[13] = 3;
    else if (regfile[13] == 78) regfile[13] = 4;

    if (regfile[14] == 65) regfile[14] = 0;
    else if (regfile[14] == 67) regfile[14] = 1;
    else if (regfile[14] == 71) regfile[14] = 2;
    else if (regfile[14] == 84) regfile[14] = 3;
    else if (regfile[14] == 78) regfile[14] = 4;

    for (t = 0; t < 18; t++)
    			execute_instrution(t, instruction[t], decoder_unit, cu, regfile);
        
    // regfile[16] = ((regfile[13] == regfile[14]) || (regfile[13] == 'N') || (regfile[14] == 'N')) ?  QUAL2PROB_TABLE[regfile[15]] : QUAL2ERROR_DIV3_TABLE[regfile[15]];
    //                                                                         // instruction 0  left_x  regfile[7]

    // regfile[17] = regfile[16] + regfile[7] + regfile[2];                    // instruction 1  left_m  regfile[6]
    // regfile[18] = regfile[16] + regfile[8] + regfile[2];
    
    // regfile[19] = regfile[16] + regfile[6] + regfile[3];                    // instruction 2  up_m    regfile[9]
    // regfile[16] = regfile[17] - regfile[18];
    
    // int x0 = regfile[9] + regfile[4];
    // int x1 = regfile[10] + regfile[1];
    // int y0 = regfile[11] + regfile[5];
    // int y1 = regfile[12] + regfile[1];
    
    // regfile[17] = std::max(regfile[17], regfile[18]) + LUT(regfile[16]);    // instruction 3  
    // regfile[18] = (regfile[11] + regfile[5])-(regfile[12] + regfile[1]);
    
    // regfile[16] = regfile[19]-regfile[17];                                  // instruction 4  
    // regfile[20] = std::max((regfile[11] + regfile[5]), (regfile[12] + regfile[1]));

    // regfile[21] = (regfile[9] + regfile[4])-(regfile[10] + regfile[1]);     // instruction 5  
    // regfile[12] = regfile[27] > 0 ? regfile[25] : 0;                       

    // regfile[25] = regfile[20] + LUT(regfile[18]);                           // instruction 6
    // regfile[18] = std::max((regfile[9] + regfile[4]), (regfile[10] + regfile[1]));   

    // regfile[26] = regfile[27] > 0 ? regfile[24] : 0;                        // instruction 7
    // regfile[11] = regfile[27] > 0 ? regfile[23] : 0;    
    
    // regfile[24] = regfile[18] + LUT(regfile[21]);                           // instruction 8
    // regfile[23] = std::max(regfile[17], regfile[19]) + LUT(regfile[16]);


    m_app[r][h] = regfile[23];
    x_app[r][h] = regfile[24];
    y_app[r][h] = regfile[25];

    // 3 pre_value = value
    // 12(7) 19+1(10) 16
    if (r == rl && h == hl) (*result) = Upper_LOGSUM(m_app[rl][h], x_app[rl][h]);
    // if (r == rl) {
    //   regfile[22] = (*result);
    //   regfile[16] = regfile[23]-regfile[24];
    //   regfile[16] = std::max(regfile[23], regfile[24]) + LUT(regfile[16]);
    //   regfile[17] = regfile[16]-regfile[22];
    //   regfile[22] = std::max(regfile[16], regfile[22]) + LUT(regfile[17]);
    //   (*result) = regfile[22];
    // }
    
    return 0;
  }

// Prune
  double do_compute_full_prob(const Read<PRECISION,PRECISION>& read, const Haplotype<PRECISION>& haplotype) override {

    int r, h;
    const int hl = haplotype.original_length;  // haplotype original length (unpadded)
    const int rl = read.original_length;       // read original length (unpadded)
    const int mrl = this->max_original_read_length();  // alias for max original read length for readability in the code below (max read length in the testcase)
    int result_app = 0;                          // result accumulator
    // printf("%d\n", MIN_INTEGER);
    double result;
    auto &consts = this->m_constants;
    int rows = rl + 1;

    std::vector<std::vector<int> > m_app;
    std::vector<std::vector<int> > x_app;
    std::vector<std::vector<int> > y_app;
    m_app.resize(rl+1, std::vector<int>(hl+1, 0));
    x_app.resize(rl+1, std::vector<int>(hl+1, 0));
    y_app.resize(rl+1, std::vector<int>(hl+1, 0));

    for (h = 0; h < hl+1; h++) 
      y_app[0][h] = Upper_LOG2_accurate(INITIAL_CONDITION_UP / hl);
    
    struct constants_app<int> consts_app;
    consts_app.mm.resize(rows, 0);
    consts_app.gm.resize(rows, 0);
    consts_app.mx.resize(rows, 0);
    consts_app.xx.resize(rows, 0);
    consts_app.my.resize(rows, 0);
    consts_app.yy.resize(rows, 0);

    for (r = 1; r < rows; r++) {
      consts_app.mm[r] = Upper_LOG2_accurate(consts.mm[r]);
      consts_app.gm[r] = Upper_LOG2_accurate(consts.gm[r]);
      consts_app.mx[r] = Upper_LOG2_accurate(consts.mx[r]);
      consts_app.xx[r] = Upper_LOG2_accurate(consts.xx[r]);
      consts_app.my[r] = Upper_LOG2_accurate(consts.my[r]);
      consts_app.yy[r] = Upper_LOG2_accurate(consts.yy[r]);
    }

    int regfile[32];
    for (r = 0; r < 32; r++) regfile[r] = 0;
    regfile[0] = 0;
    regfile[1] = -217705;
    regfile[2] = -9961;


    compute_unit_32 cu;
  	comp_decoder decoder_unit;

    unsigned long *instruction;
  	instruction = (unsigned long*)malloc(64*sizeof(unsigned long));
    instruction[0] = 0x1ef7800000000;
    instruction[1] = 0x14f135cf00010;
    instruction[2] = 0x9040e000811;
    instruction[3] = 0x90410000812;
    instruction[4] = 0x9040c000c13;
    instruction[5] = 0x2f4c64000010;
    instruction[6] = 0xac0464004011;
    instruction[7] = 0xdca006432;
    instruction[8] = 0x2f4ce2000010;
    instruction[9] = 0x2dca006434;
    instruction[10] = 0xa48002835;
    instruction[11] = 0x12f4e4000000c;
    instruction[12] = 0x12c0500004819;
    instruction[13] = 0x2a48002832;
    instruction[14] = 0x12f4e0000001a;
    instruction[15] = 0x12f4dc000000b;
    instruction[16] = 0x12c0480005418;
    instruction[17] = 0xac0466004017;
    instruction[18] = 0x20f7800000000;
    instruction[19] = 0x20f7800000000;
    instruction[20] = 0x2f4a54000010;
    instruction[21] = 0x1ef7800000000;
    instruction[22] = 0xac0254004010;
    instruction[23] = 0x1ef7800000000;
    instruction[24] = 0x2f4c2c000011;
    instruction[25] = 0x1ef7800000000;
    instruction[26] = 0xac042c004416;
    instruction[27] = 0x1ef7800000000;
    instruction[28] = 0x20f7800000000;
    instruction[29] = 0x20f7800000000;

    // fprintf(stderr, "%d\n", hl);

    int rl_padding, last_iter_index;
    if (rl%4 == 0) {
      rl_padding = rl; last_iter_index = 3;
    } else {
      rl_padding = (rl/4) * 4 + 4; last_iter_index = rl%4-1;
    }
    // generate inputs
    fprintf(stderr, "> %d %d\n%d %d %d\n", rl, hl, rl_padding, last_iter_index, hl);
    for (r = 1; r < rl+1; r++) {
      fprintf(stderr, "%d %d %d ", consts_app.mm[r], consts_app.mx[r], consts_app.my[r]);
      if ((int)read.bases[r] == 65) fprintf(stderr, "%d ", 0);
      else if ((int)read.bases[r] == 67) fprintf(stderr, "%d ", 1);
      else if ((int)read.bases[r] == 71) fprintf(stderr, "%d ", 2);
      else if ((int)read.bases[r] == 84) fprintf(stderr, "%d ", 3);
      else if ((int)read.bases[r] == 78) fprintf(stderr, "%d ", 4);
      fprintf(stderr, "%d\n", (int)read.base_qual_char[r]);
    }
    for (r = rl+1; r < rl_padding+1; r++) {
      fprintf(stderr, "%d %d %d %d %d\n", 0, 0, 0, 0, (int)read.base_qual_char[0]);
    }
    for (h = 1; h < hl+1; h++) {
      if ((int)haplotype.bases[mrl+hl+1-h] == 65) fprintf(stderr, "%d\n", 0);
      else if ((int)haplotype.bases[mrl+hl+1-h] == 67) fprintf(stderr, "%d\n", 1);
      else if ((int)haplotype.bases[mrl+hl+1-h] == 71) fprintf(stderr, "%d\n", 2);
      else if ((int)haplotype.bases[mrl+hl+1-h] == 84) fprintf(stderr, "%d\n", 3);
      else if ((int)haplotype.bases[mrl+hl+1-h] == 78) fprintf(stderr, "%d\n", 4);
    }

    // for (r=0; r<256; r++) {
    //   // fprintf(stderr, "%d\n", QUAL2ERROR_DIV3_TABLE[r]);
    //   fprintf(stderr, "%d\n", QUAL2PROB_TABLE[r]);
    // }

    for (r = 1; r < rl+1; r++) {
      // if (consts_app.xx[r] != -217705 || consts_app.yy[r] != -217705 || consts_app.gm[r] != -9961) fprintf(stderr, "%d %d %d\n", consts_app.xx[r], consts_app.yy[r], consts_app.gm[r]);
      regfile[3] = consts_app.mm[r];
      regfile[4] = consts_app.mx[r];
      regfile[5] = consts_app.my[r];
      for (h = 1; h < hl+1; h++) {
        const int read_base = read.bases[r];
        const int hap_base = haplotype.bases[mrl+hl+1-h];
        const int qual = read.base_qual_char[r];
        // printf("r=%d h=%d\n", r-1, h-1);
        accelerator(&result_app, r, h, rl, hl, read_base, hap_base, qual, &m_app, &x_app, &y_app, &consts_app);
        // accelerator_instruction(instruction, regfile, &result_app, r, h, rl, hl, read_base, hap_base, qual, &m_app, &x_app, &y_app, &consts_app, &cu, &decoder_unit);
      }
    }

    // result_app = Upper_LOGSUM(m_app[rl][hl], x_app[rl][hl]);
    result = log10(pow(2, Fix2Float(result_app - Upper_LOG2_accurate(INITIAL_CONDITION_UP))));

    m_app.clear();
    x_app.clear();
    y_app.clear();

    // printf("%d %lf\n", result_app, result);
    printf("%d\n", result_app);
    // printf("%lf\n", result);
    return result;
  }







  // // Prune
  // double do_compute_full_prob(const Read<PRECISION,PRECISION>& read, const Haplotype<PRECISION>& haplotype) override {

  //   int r, h;
  //   const int hl = haplotype.original_length;  // haplotype original length (unpadded)
  //   const int rl = read.original_length;       // read original length (unpadded)
  //   const int mrl = this->max_original_read_length();  // alias for max original read length for readability in the code below (max read length in the testcase)
  //   int result_app = MIN_INTEGER;                          // result accumulator
  //   double result;
  //   auto &consts = this->m_constants;
  //   int rows = rl + 1;
  //   // int maxNum = -1;
  //   // int earlyStopRowIndex = rl;

  //   // std::vector<std::vector<int> > prune_app;
  //   std::vector<std::vector<int> > m_app;
  //   std::vector<std::vector<int> > x_app;
  //   std::vector<std::vector<int> > y_app;
  //   // prune_app.resize(rl+1, std::vector<int>(hl+1, -1));
  //   m_app.resize(rl+1, std::vector<int>(hl+1, 0));
  //   x_app.resize(rl+1, std::vector<int>(hl+1, 0));
  //   y_app.resize(rl+1, std::vector<int>(hl+1, 0));

  //   for (h = 0; h < hl+1; h++) {
  //     // y_app[0][h] = Upper_LOG2_accurate(constants_with_precision::INITIAL_CONSTANT_WITH_PRECISION<PRECISION>() / hl);
  //     y_app[0][h] = Upper_LOG2_accurate(INITIAL_CONDITION_UP / hl);
  //   }
    
  //   struct constants_app<int> consts_app;
  //   consts_app.mm.resize(rows, 0);
  //   consts_app.gm.resize(rows, 0);
  //   consts_app.mx.resize(rows, 0);
  //   consts_app.xx.resize(rows, 0);
  //   consts_app.my.resize(rows, 0);
  //   consts_app.yy.resize(rows, 0);

  //   for (r = 1; r < rows; r++) {
  //     consts_app.mm[r] = Upper_LOG2_accurate(consts.mm[r]);
  //     consts_app.gm[r] = Upper_LOG2_accurate(consts.gm[r]);
  //     consts_app.mx[r] = Upper_LOG2_accurate(consts.mx[r]);
  //     consts_app.xx[r] = Upper_LOG2_accurate(consts.xx[r]);
  //     consts_app.my[r] = Upper_LOG2_accurate(consts.my[r]);
  //     consts_app.yy[r] = Upper_LOG2_accurate(consts.yy[r]);
  //   }

  //   for (r = 1; r < rl+1; r++) {
  //     for (h = 1; h < hl+1; h++) {
  //       const int read_base = read.bases[r];
  //       const int hap_base = haplotype.bases[mrl+hl+1-h];
  //       const int qual = read.base_qual_char[r];
  //       const float base_qual = read.base_quals[r];
  //       const int prior_comp = Upper_LOG2_accurate(float(((read_base == hap_base) || (read_base == 'N') || (hap_base == 'N')) ?  static_cast<PRECISION>(1) - base_qual : base_qual / (doNotUseTristateCorrection ? 1.0 : TRISTATE_CORRECTION)));
  //       const int prior = ((read_base == hap_base) || (read_base == 'N') || (hap_base == 'N')) ?  QUAL2PROB_TABLE[qual] : QUAL2ERROR_DIV3_TABLE[qual];
  //       int tm = prior + m_app[r-1][h-1] + consts_app.mm[r];
  //       int tx = prior + x_app[r-1][h-1] + consts_app.gm[r];
  //       int ty = prior + y_app[r-1][h-1] + consts_app.gm[r];
  //       tm = tm < MIN_INTEGER ? MIN_INTEGER : tm;
  //       tx = tx < MIN_INTEGER ? MIN_INTEGER : tx;
  //       ty = ty < MIN_INTEGER ? MIN_INTEGER : ty;
  //       // printf("i=%d j=%d %d %d %d %d %d %d %d %d %d %d ", r, h, prior, m_app[r-1][h-1], x_app[r-1][h-1], y_app[r-1][h-1], consts_app.mm[r], consts_app.gm[r], consts_app.mx[r], consts_app.xx[r],consts_app.my[r], consts_app.yy[r]);
  //       // if ((tx + ty - tm) < LOG2CURRENT_ERROR) {
  //       //   prune_app[r][h] = 1;
  //       // }
  //       // else {
  //       //   prune_app[r][h] = 0;
  //       // }
  //       m_app[r][h] = Upper_LOGSUM(tm, Upper_LOGSUM(tx, ty));
  //       int x0 = m_app[r-1][h] + consts_app.mx[r];
  //       int x1 = x_app[r-1][h] + consts_app.xx[r];
  //       x0 = x0 < MIN_INTEGER ? MIN_INTEGER : x0;
  //       x1 = x1 < MIN_INTEGER ? MIN_INTEGER : x1;
  //       int y0 = m_app[r][h-1] + consts_app.my[r];
  //       int y1 = y_app[r][h-1] + consts_app.yy[r];
  //       y0 = y0 < MIN_INTEGER ? MIN_INTEGER : y0;
  //       y1 = y1 < MIN_INTEGER ? MIN_INTEGER : y1;
  //       x_app[r][h] = Upper_LOGSUM(x0, x1);
  //       y_app[r][h] = Upper_LOGSUM(y0, y1);
  //       // x_app[r][h] = Upper_LOGSUM(m_app[r-1][h] + consts_app.mx[r], x_app[r-1][h] + consts_app.xx[r]);
  //       // y_app[r][h] = Upper_LOGSUM(m_app[r][h-1] + consts_app.my[r], y_app[r][h-1] + consts_app.yy[r]);
  //       // printf("%d %d %d %d %d\n", m_app[r][h], x_app[r][h], y_app[r][h], y0, y1);
  //     }

  //     // // 18% reduction in fixed point computation
  //     // if(r%16 == 0){
  //     //   for(h = 1; h <= hl; h++){
  //     //     maxNum = std::max(m_app[r][h], maxNum);	
  //     //   }
  //     //   if(maxNum<EARLYSTOP_THRESHOLD){
  //     //       earlyStopRowIndex = r;
  //     //       break;
  //     //   }
  //     // }
  //   }

  //   for (h = 1; h < hl + 1; h++)
  //     result_app = Upper_LOGSUM(Upper_LOGSUM(m_app[rl][h], x_app[rl][h]), result_app);

  //   // int skip_index_lastrow = hl;
  //   // int max_m = m_app[rl][skip_index_lastrow];
  //   // for (h = hl; h > 0; h--) {
  //   //   if (m_app[rl][h] > max_m) {
  //   //     max_m = m_app[rl][h];
  //   //     skip_index_lastrow = h;
  //   //   }
  //   //   // printf("max=%f match[%d]=%f\n", Fix2Float(max_m), h, Fix2Float(m_app[rl][h]));
  //   //   result += Upper_LOG2_float(Upper_LOGSUM(m_app[rl][h], x_app[rl][h]));
  //   // }
  //   // printf("earlyStopRowIndex: %d\tskip_index_lastrow: %d\n", earlyStopRowIndex, skip_index_lastrow);

  //   // printf("prune\n");
  //   // for (r = 1; r < rl+1; r++) {
  //   //   for (h = 1; h < hl+1; h++) {
  //   //     printf("%d ", prune_app[r][h]);
  //   //   }
  //   //   printf("\n");
  //   // }
  //   // printf("Istop\n");
  //   // for (r = 1; r < rl+1; r++) {
  //   //   for (h = 1; h < hl+1; h++) {
  //   //     printf("%d ", Istop[r+1][h+1]);
  //   //   }
  //   //   printf("\n");
  //   // }
  //   // printf("Jstop\n");
  //   // for (r = 1; r < rl+1; r++) {
  //   //   for (h = 1; h < hl+1; h++) {
  //   //     printf("%d ", Jstop[r+1][h+1]);
  //   //   }
  //   //   printf("\n");
  //   // }
  //   // printf("skip_r: %d\t skip_h: %d\n", earlyStopRowIndex, skip_index_lastrow);

  //   m_app.clear();
  //   x_app.clear();
  //   y_app.clear();

  //   // result = log10(static_cast<double>(result)) - log10(static_cast<double>(this->INITIAL_CONSTANT));
  //   // result = log10(static_cast<double>(result)) - log10(static_cast<double>(INITIAL_CONDITION_UP));
  //   result = log10(pow(2, Fix2Float(result_app - Upper_LOG2_accurate(INITIAL_CONDITION_UP))));

  //   printf("%lf\n", result);
  //   return result;
  // }






  // // Original
  // double do_compute_full_prob(const Read<PRECISION,PRECISION>& read, const Haplotype<PRECISION>& haplotype) override {

  //   const auto hl = haplotype.original_length;  // haplotype original length (unpadded)
  //   const auto rl = read.original_length;       // read original length (unpadded)
  //   const auto rows = rl + read.left_padding;  // number of rows in the diagonals (padded read length)
  //   const auto mrl = this->max_original_read_length();  // alias for max original read length for readability in the code below (max read length in the testcase)
  //   const auto fd = mrl - rl;                   // first diagonal to compute (saves compute of all-0 diagonals when read is shorter than the padding - which will be the maximum read length in the testcase)
  //   auto result = 0.l;                          // result accumulator
  //   auto &diags = this->m_diagonals;
  //   auto &consts = this->m_constants;

  //   // for (auto d = fd; d != mrl + hl - 1; ++d) { // d for diagonal
  //   for (int d = 0; d != mrl + rl - 1; ++d) {
  //     const auto hap_offset = mrl+hl-1;
  //     for (auto r = 1u; r != rows; ++r) {       // r for row
  //       const auto read_base = read.bases[r];
  //       const auto hap_base = haplotype.bases[hap_offset+r-d];
  //       const auto base_qual = read.base_quals[r];
  //       const auto prior = ((read_base == hap_base) || (read_base == 'N') || (hap_base == 'N')) ?  static_cast<PRECISION>(1) - base_qual : base_qual;
  //       diags.m[r] = prior * ((diags.mpp[r-1] * consts.mm[r]) + (consts.gm[r] * (diags.xpp[r-1] + diags.ypp[r-1])));
  //       diags.x[r] = diags.mp[r-1] * consts.mx[r] + diags.xp[r-1] * consts.xx[r];
  //       diags.y[r] = diags.mp[r] * consts.my[r] + diags.yp[r] * consts.yy[r];
  //     }
  //     result += diags.m[rows-1] + diags.x[rows-1];
  //     diags.rotate();
  //   }
  //   result = result < this->MIN_ACCEPTED ?
  //     this->FAILED_RUN_RESULT : // if we underflowed return failed constant to rerun with higher precision if desired
  //     log10(static_cast<double>(result)) - log10(static_cast<double>(this->INITIAL_CONSTANT));
  //   printf("%lf\n", result);
  //   return result;
  // }






  // // Reorder
  // double do_compute_full_prob(const Read<PRECISION,PRECISION>& read, const Haplotype<PRECISION>& haplotype) override {

  //   int r, h;
  //   const int hl = haplotype.original_length;  // haplotype original length (unpadded)
  //   const int rl = read.original_length;       // read original length (unpadded)
  //   const int mrl = this->max_original_read_length();  // alias for max original read length for readability in the code below (max read length in the testcase)
  //   float result = 0.l;                          // result accumulator
  //   auto &consts = this->m_constants;

  //   // printf("hl: %d rl: %d mrl: %d\n", hl, rl, mrl);
  //   // printf("haplotype\t");

  //   // for (h = hl + mrl; h > mrl; h--) {
  //   //   if (haplotype.bases[h] == 65) printf("A");
  //   //   else if (haplotype.bases[h] == 67) printf("C");
  //   //   else if (haplotype.bases[h] == 71) printf("G");
  //   //   else if (haplotype.bases[h] == 84) printf("T");
  //   // }
  
  //   // printf("\nread\t\t");

  //   // for (r = 1; r < rl + 1; r++) {
  //   //   if (read.bases[r] == 65) printf("A");
  //   //   else if (read.bases[r] == 67) printf("C");
  //   //   else if (read.bases[r] == 71) printf("G");
  //   //   else if (read.bases[r] == 84) printf("T");
  //   // }
  //   // printf("\n");

  //   std::vector<std::vector<PRECISION> > m;
  //   std::vector<std::vector<PRECISION> > x;
  //   std::vector<std::vector<PRECISION> > y;

  //   m.resize(rl+1, std::vector<PRECISION>(hl+1, 0));
  //   x.resize(rl+1, std::vector<PRECISION>(hl+1, 0));
  //   y.resize(rl+1, std::vector<PRECISION>(hl+1, 0));

  //   for (h = 0; h < hl+1; h++) {
  //     y[0][h] = constants_with_precision::INITIAL_CONSTANT_WITH_PRECISION<PRECISION>() / hl;
  //   }

  //   for (r = 1; r < rl+1; r++) {
  //     for (h = 1; h < hl+1; h++) {
  //       const int read_base = read.bases[r];
  //       const int hap_base = haplotype.bases[mrl+hl+1-h];
  //       const float base_qual = read.base_quals[r];
  //       const float prior = ((read_base == hap_base) || (read_base == 'N') || (hap_base == 'N')) ?  static_cast<PRECISION>(1) - base_qual : base_qual / (doNotUseTristateCorrection ? 1.0 : TRISTATE_CORRECTION);
  //       m[r][h] = prior * (m[r-1][h-1] * consts.mm[r] + (x[r-1][h-1] + y[r-1][h-1]) * consts.gm[r]);
        
  //       x[r][h] = m[r-1][h] * consts.mx[r] + x[r-1][h] * consts.xx[r];
  //       y[r][h] = m[r][h-1] * consts.my[r] + y[r][h-1] * consts.yy[r];

  //       // printf("MPP: %.0lf\t XPP: %.0lf\t YPP: %.0lf\n", m[r-1][h-1], x[r-1][h-1], y[r-1][h-1]);
  //       // printf("MPX: %.0lf\t XP: %.0lf\t MPY: %.0lf\t YP: %.0lf\n", m[r-1][h], x[r-1][h], m[r][h-1], y[r][h-1]);
  //       // printf("%lf\t %lf\t %lf\t %lf\t %lf\t %lf\t %lf\n", prior, consts.mm[r], consts.gm[r], consts.mx[r], consts.xx[r], consts.my[r], consts.yy[r]);
  //       // printf("r=%d h=%d\t M:%.0lf\t X:%.0lf\t Y:%.0lf\n", r, h, m[r][h], x[r][h], y[r][h]);
  //     }
  //   }

  //   for (h = 1; h < hl+1; h++) {
  //     result += m[rl][h] + x[rl][h];
  //     // printf("h=%d r=%d %lf %lf %lf\n", mrl+hl+1-h, rl, result, m[rl][h], x[rl][h]);      
  //   }
  //   // printf("\n");

  //   // for (r = 1; r < rl+1; r++) {
  //   //   for (h = 1; h < hl+1; h++) {
  //   //     printf("h=%d r=%d\t M:%lf\t X:%lf\t Y:%lf\n", r, h, m[r][h], x[r][h], y[r][h]);
  //   //   }
  //   // }

  //   m.clear();
  //   x.clear();
  //   y.clear();

  //   // printf("%lf %lf ", log10(static_cast<double>(result)), log10(static_cast<double>(this->INITIAL_CONSTANT)));
  //   result = log10(static_cast<double>(result)) - log10(static_cast<double>(this->INITIAL_CONSTANT));
  //   // result = result < this->MIN_ACCEPTED ? this->FAILED_RUN_RESULT : result;
  //   // if we underflowed return failed constant to rerun with higher precision if desired
      
  //   printf("%lf\n", result);
  //   return result;
  // }


};

#endif
