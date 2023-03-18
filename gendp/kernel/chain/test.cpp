#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <typeinfo>
#include "src/fixed.h"

using namespace numeric;
typedef Fixed<13, 3> fixed;

static inline fixed log_exp_32(int32_t v, int fraction_size, float* LUT)
{
	int32_t t;
    t = v >> (16 - fraction_size);
    // printf("%d\n", t);
    int half_table = pow(2, (4+fraction_size));
    if (t < -half_table) {
        fixed result(-16);
        return result;
    } else if (t < half_table) {
        fixed result(LUT[half_table + t]);
        return result;
    } else {
        fixed result(16);
        return result;
    }
}


int main(){

    // float x = 61.4220;
    // float y = 0.3012;
    // fixed f_1(x);
    // fixed f_2(y);
    // float b = f_1.to_float();
    // fixed F_1(log(x));
    // fixed F_2(log(y));
    // float c = F_1.to_float();
    // float d = (f_1*f_2).to_float();
    // float e = exp((F_1+F_2).to_float());
    // float f = (f_1+f_2).to_float();
    // printf("%f\n", (F_1-F_2).to_float());
    // printf("%x\n", (F_1-F_2).to_raw());
    // printf("%f\n", log_exp_32((F_1-F_2).to_raw(), size, LUT).to_float());
    // float g = exp((F_2+log_exp_32((F_1-F_2).to_raw(), size, LUT)).to_float());
    
    // printf("32-bits x %18f\n", (b-x));
    // printf("32-bits log(x) %13f\n", abs((c-log(x))/log(x)));
    // printf("32-bits x*y %16f\n", abs((d-(x*y))/(x*y)));
    // printf("32-bits log(x*y) %11f\n", abs((e-(x*y))/(x*y)));
    // printf("32-bits x+y %16f\n", abs((f-(x+y))/(x+y)));
    // printf("32-bits log(x+y) %11f\n\n", abs((g-(x+y))/(x+y)));

    // printf("%x, %x\n", f_1.to_raw(), (F_2-F_1).to_raw()>>12);
    // std::cout << "\n" << typeid(f_1.to_raw()).name();
    // printf("\n%x", log_exp((F_1-F_2).to_raw(), 4));

    // fixed f_x = x;
    // fixed f_y = y;
    // fixed f_result = x * y;
    // // printf("%f, %f", x*y, f_result.to_float());

    // int64_t dx = 2147483648 * 2 + 100 + 4026531840;
    // int32_t dy = 101;
    // int l = dx - dy;
    // printf("%llx %x %llx %llx %x %d", dx, dy, dx - dy, dy - dx, l, (int32_t)dx);
    // if (dx > dy) printf("!");

    // int64_t l_comp_64 = 9402;
    // printf("%d %x %x\n", l_comp_64, l_comp_64, (l_comp_64 >> 32));
    // l_comp_64 = 222222222992;
    // int32_t l_comp_32 = (int32_t)l_comp_64;
    // printf("%ld %lx %lx\n", l_comp_64, l_comp_64, l_comp_32);
    // l_comp_64 = -222222222992;
    // l_comp_32 = (int32_t)l_comp_64;
    // printf("%ld %lx %lx\n", l_comp_64, l_comp_64, l_comp_32);

    int64_t dr = 8495445298828;
    int32_t dq = -10780;
    int32_t dd = dr > dq? dr - dq : dq - dr;
    printf("%d\n", dd); // -1880
    return 0;
}




// #include "flexfloat/include/flexfloat.hpp"
// using namespace flx;
// typedef flexfloat<6, 12> floatc;
// // typedef floatx<6, 12> floatc;

// int main(){
    
//     floatc f_1 = 500.1;
//     floatc f_2 = -2000;
//     floatc f_3 = 0.001;
//     float b = float(f_1 * f_2);
//     float c = float(f_3 / f_1);
//     float d = float(f_1 - f_1);
//     printf("%f, %f, %f", b, c, d);
//     return 0;
// }


// int main(){
    
//     // FP_LONG f_1 = FromFloat(500.1);
//     // FP_LONG f_2 = FromFloat(-2000);
//     // FP_LONG f_3 = FromFloat(0.001);
//     // FP_LONG f_4 = FromFloat(2147480648);
//     // FP_LONG f_5 = FromFloat(-2147480648);
//     // float b = ToFloat(Mul(f_1, f_2));           // 500.1 * (-2000) = -1000200.00        (-1000200.00)       correct
//     // float c = ToFloat(DivPrecise(f_3, f_1));    // 0.001 / 500.1 = 0.000002             (0.000002)          correct
//     // float d = ToFloat(Sub(f_1,f_1));            // 500.1 - 500.1 = 0.000000             (0.000000)          correct

//     // float e = ToFloat(Add(f_4,f_1));            // 2147480648 + 500.1 = 2147481148.1    (2147481216.000000) 
//     // float f = ToFloat(DivPrecise(f_4,f_1));     // 2147480648 / 500.1 = 4294102.5       (4294102.500000)    correct
//     // float g = ToFloat(Mul(f_4,f_1));            // 2147480648 * 500.1 = overflow        (213289184.000000)

//     // float h = ToFloat(Add(f_5,f_1));            // -2147480648 + 500.1 = -2147480149.9  (-2147480192.000000)
//     // float i = ToFloat(DivPrecise(f_5,f_1));     // -2147480648 / 500.1 = -4294102.5     (-4294102.500000)   correct
//     // float j = ToFloat(Mul(f_5,f_1));            // -2147480648 * 500.1 = overflow       (-213289184.000000)
//     // printf("%f, %f, %f\n%f, %f, %f\n%f, %f, %f\n", b, c, d, e, f, g, h, i, j);
    
//     FP_LONG f_1 = FromDouble(0.00000000116);
//     double f = ToDouble(f_1);
//     printf("%.16lf", f);
    
//     return 0;


// }
