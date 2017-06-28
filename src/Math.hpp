#ifndef MATH_HPP
#define MATH_HPP

#include <iostream>
#include <cmath>
#define LOG_ZERO (-(1.0/0))
typedef long double ld;
using namespace std;

class LogDouble{
private:
    ld value;
public:
    LogDouble(){
        this->value = LOG_ZERO;
    }
    LogDouble(ld value){
        this->value = value;
    }
    void init(){
        this->value = LOG_ZERO;
    }
    ld get(){
        return this->value;
    }
    LogDouble operator+(LogDouble x){
        if (x.get() == LOG_ZERO) return LOG_ZERO;
        if (this->value == LOG_ZERO) return LOG_ZERO;
        return LogDouble(this->value + x.get());
    }
    LogDouble operator-(LogDouble x){
        if (x.get() == LOG_ZERO) return LOG_ZERO;
        if (this->value == LOG_ZERO) return LOG_ZERO;
        return LogDouble(this->value - x.get());
    }
    void operator=(LogDouble x){
        this->value = x.get();
    }
    void operator=(ld x){
        this->value = x;
    }
    bool operator>(LogDouble x){
        if (this->value == LOG_ZERO) return false;
        else if (x.get() == LOG_ZERO) return true;
        else return this->value > x.get();
    }
    // ostream& operator<<(ostream& out, LogDouble x){
    //     out << c.get();
    //     return out;
    // }
};
LogDouble LogarithmOfAddition(LogDouble a, LogDouble b){
    if (a.get() == LOG_ZERO) return b;
    if (b.get() == LOG_ZERO) return a;
    // if (a.get() < b.get()) return LogarithmOfAddition(b, a);
    // cout<<"!"<<(b - a).get()<<endl;
    return LogDouble(a.get() + log(1 + exp((b - a).get())));
}

// double LogarithmOfAddition2(double a, double b){
//     if (a > 1000) return b;
//     if (b > 1000) return a;
//     if (a > b) LogarithmOfAddition(b, a);
//     return a + log(1 + exp(b - a));
// }

#endif