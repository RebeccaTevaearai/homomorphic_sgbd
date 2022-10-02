#include "api_inferior_or_equal_short.h"

#pragma hls_top
short inferior_or_equal_short(short arg1, short arg2) {
        return arg1 <= arg2;
}