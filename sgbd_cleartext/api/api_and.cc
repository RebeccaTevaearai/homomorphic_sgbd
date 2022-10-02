#include "api_and.h"

#pragma hls_top
int tfhe_and(int arg1, int arg2) {
        return arg1 && arg2;
}