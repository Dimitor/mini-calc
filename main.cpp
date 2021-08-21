#include <cstdio>
#include "calculator.h"

int main()
{
    Calculator cl;
    double res1 = cl.calc("(((1+4)))");
    double res2 = cl.calc("(1+4)*5/(5)");
    double res3 = cl.calc("15 + 1 - (-12*3)");
    double res4 = cl.calc("2^(5-1)*4/2/2/2");
    double res5 = cl.calc("-(-(-(-(12)*2)*2)*2)/9");
    printf("%g\n%g\n%g\n%g\n%g", res1, res2, res3, res4, res5);
    return 0;
}