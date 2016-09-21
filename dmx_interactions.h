/*
 * dmx_interactions.h
 *
 * Created: 18/09/2016 18:40:50
 *  Author: josefe
 */ 


#include "dmx_hardware.h"

#ifndef DMX_INTERACTIONS_H_
#define DMX_INTERACTIONS_H_

#ifndef inline
#define inline
#endif

#define FRAME_VALID 1
#define FRAME_DEPRECATED 0

/*Functions to write data on UART registers*/
inline void reset_oerr(void);
inline void reset_ferr(void);
inline void registers_init(void);

/*Functions to process data reception*/
void process_data(unsigned char, int, int, int);
void validate_frame(unsigned char, int, int, int);
unsigned char end_rx(unsigned char, int, int, int);

/*Functions to set and get data output pointers*/
void set_output_data_pointer(volatile unsigned char*);
volatile unsigned char* get_output_data_pointer(void);

/*Functions to set and get frame timeout pointer*/
void set_timeout_pointer(volatile unsigned long*);
volatile unsigned long* get_timeout_pointer(void);

/*Functions to set and get valid frame flag pointer*/
void set_valid_frame_flag_pointer(volatile unsigned char*);
volatile unsigned char* get_valid_frame_flag_pointer(void);


#endif /* DMX_INTERACTIONS_H_ */