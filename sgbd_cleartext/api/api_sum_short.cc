#include "api_sum_short.h"

#pragma hls_top
short tfhe_sum_short(short arg1, short arg2) {
        return arg1 + arg2;
}