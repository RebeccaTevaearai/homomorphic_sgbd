#include "api_and_short.h"

#pragma hls_top
short tfhe_and_short(short arg1, short arg2) {
        return arg1 && arg2;
}