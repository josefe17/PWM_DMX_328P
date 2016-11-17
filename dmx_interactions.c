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

/*Stores DMX data output pointer*/
void set_output_data_pointer(volatile unsigned char* data_pointer)
{
	data_output = data_pointer;
}

/*Gets DMX data output pointer*/
volatile unsigned char* get_output_data_pointer(void)
{
	return data_output;
}

/*Stores timeout count pointer*/
void set_timeout_pointer(volatile unsigned long* timeout_pointer)
{
	timeout_pointer=timeout_pointer;
}

/*Gets timeout count pointer*/
volatile unsigned long* get_timeout_pointer(void)
{
	return timeout_pointer;
}

/*Stores valid frame (ACK) pointer*/
void set_valid_frame_flag_pointer(volatile unsigned char* flag_pointer)
{
	valid_frame_flag_pointer = flag_pointer;
}

/*Gets valid frame (ACK) pointer*/
volatile unsigned char* get_valid_frame_flag_pointer(void)
{
	return valid_frame_flag_pointer;
}

/*Custom function that processes or stores data received by USART*/
void process_data(unsigned char data_RX, int RX_index, int start_address, int channels)
{
	
	if (RX_index>=start_address && RX_index<DMX_MAX_CHANNELS)	//If data counter is higher than start address and lower than last address
	{
		if (RX_index-start_address<channels) // If data counter is lower than highest channel address
		{
			data_output[RX_index-start_address]=data_RX; //Stores data
		}
	}
}

/*Custom function that acknowledges frame received*/
void validate_frame(unsigned char data_RX, int RX_index, int start_address, int channels)
{
	if (RX_index >= (DMX_MAX_CHANNELS-1) || RX_index-start_address >= (channels-1)){ //If data counter is bigger than highest channel address
		*valid_frame_flag_pointer=FRAME_VALID;		//Validates frame
		*timeout_pointer=0;							//Resets timer
		
	}
}

/*Custom function that detects end of data reception*/
unsigned char end_rx(unsigned char data_RX, int RX_index, int start_address, int channels) 
{
	return (RX_index >= DMX_MAX_CHANNELS);// || RX_index-start_address >= (channels)); // If counter is higer than last address, return 1
}

/*Inits UART HW registers*/
/*Depends on HW*/
inline void registers_init(void)
{
		DMX_BRL = BRL_LOAD;
		DMX_BRH = BRH_LOAD;
		DMX_UCSRA = UCSRA_LOAD;
		DMX_UCSRB = UCSRB_RESET_LOAD;
		DMX_UCSRC = UCSRC_LOAD;
		DMX_UCSRB = UCSRB_LOAD;
}

/*Clears overrun error*/
/*Depends on HW*/
inline void reset_oerr(void)
{
	DMX_UCSRB = UCSRB_RESET_LOAD;
	DMX_UCSRB = UCSRB_LOAD;
}

/*Clears framing error*/
/*Depends on HW*/
inline void reset_ferr(void)
{
	DMX_UCSRA &= ~(1<<DMX_FE);
}