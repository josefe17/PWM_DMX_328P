/*
 * dmx_basic.c
 *
 * Created: 18/09/2016 17:40:48
 *  Author: josefe
 */ 

#include "dmx_basic.h"


/*DMX RX STATUS*/
enum{
	WAIT_FOR_BYTES,
	WAIT_FOR_BREAK,
	WAIT_FOR_START,
	DATA_RECEPTION
	};

void dmx_interrupt(unsigned char* FSM_status, int start_address, int channels, int* RX_index)
{
	unsigned char data_RX;	
	unsigned char status_register;	
	
	
	while (DMX_RXIF) 
		{
					
		status_register=DMX_RX_STATUS;
		data_RX = DMX_RX_DATA;
		
		if (status_register & (1<<DMX_DOR))
		{
			reset_oerr();
			*FSM_status = WAIT_FOR_BYTES;
			return;
		}
					
		switch (*FSM_status)
		{
			case WAIT_FOR_BYTES:   
				if (status_register &(1<<DMX_FE))
				{
					reset_ferr();
					*FSM_status = WAIT_FOR_BREAK;										
				}
				
				break;
			
			case WAIT_FOR_BREAK:   				
				
				if (status_register &(1<<DMX_FE))
				{
					reset_ferr();
					if (!data_RX)
					{
						*FSM_status = WAIT_FOR_START;	
																	
					}	

				}
				break;
				
			case WAIT_FOR_START: 
				
				if (status_register &(1<<DMX_FE))
				{
					reset_ferr();
					*FSM_status = WAIT_FOR_BYTES;
					break;
				}			
			
				if (!data_RX)
				{
 					(*RX_index) = 0;
					*FSM_status = DATA_RECEPTION;
				} 
				else
				{
					*FSM_status = WAIT_FOR_BREAK;
				}			
				break;
				
			case DATA_RECEPTION:
				
				if (status_register &(1<<DMX_FE))
				{
					reset_ferr();					
					if (!data_RX)
					{
						*FSM_status = WAIT_FOR_START;
					}
					else
					{
						*FSM_status = WAIT_FOR_BYTES;
					}
					break;
				}				

				process_data(data_RX, *RX_index, start_address, channels);
				validate_frame(data_RX, *RX_index, start_address, channels);	
			
				(*RX_index)++;
				if (end_rx(data_RX, *RX_index, start_address, channels))
				{	
						
					*FSM_status=WAIT_FOR_BYTES;
				}
				break;
			}
			
		}
	}



void dmx_init(unsigned char* FSM_status, int* RX_index)
{
	*FSM_status=WAIT_FOR_BYTES;
	*RX_index=0;	
	registers_init();
	
}