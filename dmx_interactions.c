/*
 * dmx_interactions.c
 *
 * Created: 18/09/2016 18:39:56
 *  Author: josefe
 */ 

#include "dmx_interactions.h"
#include "dmx_basic.h"

volatile unsigned char* data_output;
volatile unsigned char* valid_frame_flag_pointer;
volatile unsigned long* timeout_pointer;

void set_output_data_pointer(volatile unsigned char* data_pointer)
{
	data_output = data_pointer;}


volatile unsigned char* get_output_data_pointer(void)
{
	return data_output;
}

void set_timeout_pointer(volatile unsigned long* timeout_pointer)
{
	timeout_pointer=timeout_pointer;
}

volatile unsigned long* get_timeout_pointer(void)
{
	return timeout_pointer;
}

void set_valid_frame_flag_pointer(volatile unsigned char* flag_pointer)
{
	valid_frame_flag_pointer = flag_pointer;
}

volatile unsigned char* get_valid_frame_flag_pointer(void)
{
	return valid_frame_flag_pointer;
}


void process_data(unsigned char data_RX, int RX_index, int start_address, int channels)
{
	
	if (RX_index>=start_address && RX_index<DMX_MAX_CHANNELS)
	{
		if (RX_index-start_address<channels)
		{
			data_output[RX_index-start_address]=data_RX;
		}
	}
}

void validate_frame(unsigned char data_RX, int RX_index, int start_address, int channels)
{
	if (RX_index >= (DMX_MAX_CHANNELS-1) || RX_index-start_address >= (channels-1)){
		*valid_frame_flag_pointer=FRAME_VALID;		
		*timeout_pointer=0;			
		
	}
}

unsigned char end_rx(unsigned char data_RX, int RX_index, int start_address, int channels)
{
	return (RX_index >= DMX_MAX_CHANNELS);// || RX_index-start_address >= (channels));
}

inline void registers_init(void)
{
		DMX_BRL = BRL_LOAD;
		DMX_BRH = BRH_LOAD;
		DMX_UCSRA = UCSRA_LOAD;
		DMX_UCSRB = UCSRB_RESET_LOAD;
		DMX_UCSRC = UCSRC_LOAD;
		DMX_UCSRB = UCSRB_LOAD;
}

inline void reset_oerr(void)
{
	DMX_UCSRB = UCSRB_RESET_LOAD;
	DMX_UCSRB = UCSRB_LOAD;
}

inline void reset_ferr(void)
{
	DMX_UCSRA &= ~(1<<DMX_FE);
}