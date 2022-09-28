#include "api_product_short.h"

#pragma hls_top
short tfhe_product_short(short arg1, short arg2) {
    return arg1 * arg2;
}