#include "api_inferior_int.h"

#pragma hls_top
int inferior_int(int arg1, int arg2) {
        return arg1 < arg2;
}