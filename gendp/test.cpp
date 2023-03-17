#include <cstdio>
#include <cstdint>

int main(){
    int a = 2, b = 4, c, a1, b1;
    long mul_tmp;
    a1 = a << 16;
    b1 = b << 16;
    mul_tmp = (long)a1 * (long)b1;
    c = (int)(mul_tmp >> 32);
    printf("%x %x %x %x %lx %x\n", a, b, a1, b1, mul_tmp, c);
    return 0;
}