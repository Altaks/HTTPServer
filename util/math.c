#include "math.h"

__attribute__((unused)) int log10(int value){
    int result = 0;
    while(value > 0){
        value /= 10;
        result++;
    }
    return result;
}