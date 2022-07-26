#include "bit_mask.hpp"


#define SET_BIT(param) 1 << param
#define CLEAR_BIT(param) ~(SET_BIT(param))


int     set_bit(int el, int param) {
    return (el |= SET_BIT(param));
}

int     clear_bit(int el, int param) {
    return (el &= CLEAR_BIT(param));
}

int     toggle_bit(int el, int param) {
    return (el ^= (1 << param));
}

bool     get_bit(int el, int bit_pos) {
    return (el & SET_BIT(bit_pos)) ? true : false;
}
