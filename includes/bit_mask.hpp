#ifndef BIT_MASK_HPP
	#define BIT_MASK_HPP

#define SET_BIT(param) 1 << param
#define CLEAR_BIT(param) ~(SET_BIT(param))

int     set_bit(int el, int param);
int     clear_bit(int el, int param);
int     toggle_bit(int el, int param);
bool     get_bit(int el, int bit_pos);


#endif