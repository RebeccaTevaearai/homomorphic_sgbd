#include "api_inferior_short.h"

#pragma hls_top
short inferior_short(short arg1, short arg2) {
        return arg1 < arg2;
}