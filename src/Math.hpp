#include <cmath>
double LogarithmOfAddition(double a, double b){
    if (a > 0) return b;
    if (b > 0) return a;
    if (a > b) LogarithmOfAddition(b, a);
    return a + log(1 + exp(b - a));
}