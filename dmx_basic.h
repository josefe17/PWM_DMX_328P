/*
 * dmx_basic.h
 *
 * Created: 18/09/2016 17:44:08
 *  Author: josefe
 */ 

#include "dmx_interactions.h"


#ifndef DMX_BASIC_H_
#define DMX_BASIC_H_

#define DMX_MAX_CHANNELS 512

/*IS routine*/
void dmx_interrupt(unsigned char*, int, int, int*);
/*USART init*/
void dmx_init(unsigned char*, int*);


#endif /* DMX_BASIC_H_ */