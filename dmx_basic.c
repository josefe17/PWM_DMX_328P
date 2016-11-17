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
	/*Data and status backups to prevent being modified while processing data*/
	unsigned char data_RX;			
	unsigned char status_register;		
	
	while (DMX_RXIF) //While there were data pending to be processed
		{
		
		/*Backup registers*/			
		status_register=DMX_RX_STATUS; 
		data_RX = DMX_RX_DATA;
		
		/*If overrun error flag is set, it discards data and resets FSM*/
		if (status_register & (1<<DMX_DOR))
		{
			reset_oerr(); //Clear overrun flag
			*FSM_status = WAIT_FOR_BYTES;
			return;
		}
					
		switch (*FSM_status)
		{
			case WAIT_FOR_BYTES:   
				if (status_register &(1<<DMX_FE)) //If line is idle and framing error is detected means that a break was sent
				{
					reset_ferr(); //Clear framing flag
					*FSM_status = WAIT_FOR_BREAK; //We're on break state									
				}
				
				break;
			
			case WAIT_FOR_BREAK:   			
				
				if (status_register &(1<<DMX_FE)) //If break continues
				{
					reset_ferr(); //Clear framing flag
					if (!data_RX) //If it is received a 0
					{
						*FSM_status = WAIT_FOR_START; //Goes to start state
																	
					}	

				}
				break;
				
			case WAIT_FOR_START: 
				
				if (status_register &(1<<DMX_FE)) // If framing error is set while it's waiting for start
				{
					reset_ferr();			
					*FSM_status = WAIT_FOR_BYTES; // Reception is aborted and goes back to idle state
					break;
				}			
			
				if (!data_RX)						//If it receives a valid start byte
				{
 					(*RX_index) = 0;
					*FSM_status = DATA_RECEPTION;	//We are going to start receiving data
				} 
				else
				{
					*FSM_status = WAIT_FOR_BREAK;	//if not, goes back to break status
				}			
				break;
				
			case DATA_RECEPTION:
				
				if (status_register &(1<<DMX_FE))	//If framing overrun is set while data is being received
				{
					reset_ferr();					
					if (!data_RX)
					{
						*FSM_status = WAIT_FOR_START; // If line is low goes back to start state
					}
					else
					{
						*FSM_status = WAIT_FOR_BYTES; // If not goes to idle state
					}
					break;
				}				
				/*if data received is valid*/
				process_data(data_RX, *RX_index, start_address, channels); //Processes it
				validate_frame(data_RX, *RX_index, start_address, channels); // Sets valid flag	
			
				(*RX_index)++;
				if (end_rx(data_RX, *RX_index, start_address, channels))	// Ends reception
				{	
						
					*FSM_status=WAIT_FOR_BYTES;
				}
				break;
			}
			
		}
	}


/*Initializes DMX FSM variables and hardware*/
void dmx_init(unsigned char* FSM_status, int* RX_index)
{
	*FSM_status=WAIT_FOR_BYTES;
	*RX_index=0;	
	registers_init();
	
}