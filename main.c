/*
 * PWM_DMX.c
 *
 * Created: 18/09/2016 16:59:18
 * Author : josefe
 */ 

#ifndef DMX_MAX_CHANNELS
#define DMX_MAX_CHANNELS 512
#endif

#define F_CPU 20000000

#define ANALOG_CHANNELS 6
#define TIMEOUT_TRESHOLD 78125

#define OFF 0
#define ON 1

#define DDRC_MASK 0b00111111
#define DDRD_MASK 0b00010110

#define PINC_MASK	0b00111111
#define PIND_MASK_L 0b00000110
#define PIND_MASK_H 0b00010000

#define PWM0_VALUE OCR0A
#define PWM1_VALUE OCR0B
#define PWM2_VALUE OCR1A
#define PWM3_VALUE OCR1B
#define PWM4_VALUE OCR2A
#define PWM5_VALUE OCR2B

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "dmx_basic.h"

/*Prototypes*/

unsigned char attach_bit(volatile unsigned char*, unsigned char, volatile unsigned char* , unsigned char);
void port_init(void);
void timer0_interrupts_init();
void timer0_as_pwm_init(void);
void timer1_as_pwm_init(void);
void timer2_as_pwm_init(void);

int read_address (void);
unsigned char check_shapping(void);
void fire_pwm(void);
void clear_buffer(volatile unsigned char*, int);

/*Data values for output shapping*/
/*AC linear dimming curve*/
const unsigned char shape[] = 
{ 
	0, 0, 27, 31, 34, 37, 39, 42, 44, 45, 47, 49, 50, 52, 53, 54, 56, 57, 58, 59, 60, 61, 62, 63, 
	64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 73, 74, 75, 76, 76, 77, 78, 79, 79, 80, 81, 82, 82, 
	83, 84, 84, 85, 86, 86, 87, 88, 88, 89, 90, 90, 91, 91, 92, 93, 93, 94, 94, 95, 96, 96, 97, 
	97, 98, 98, 99, 100, 100, 101, 101, 102, 102, 103, 103, 104, 105, 105, 106, 106, 107, 107, 
	108, 108, 109, 109, 110, 110, 111, 111, 112, 112, 113, 114, 114, 115, 115, 116, 116, 117, 117, 
	118, 118, 119, 119, 120, 120, 121, 121, 122, 122, 123, 123, 124, 124, 125, 125, 126, 126, 127, 
	127, 128, 128, 129, 129, 130, 130, 131, 131, 132, 132, 133, 133, 134, 134, 135, 135, 136, 136, 
	137, 137, 138, 138, 139, 139, 140, 140, 141, 141, 142, 142, 143, 143, 144, 144, 145, 145, 146, 
	147, 147, 148, 148, 149, 149, 150, 150, 151, 151, 152, 152, 153, 154, 154, 155, 155, 156, 156, 
	157, 157, 158, 159, 159, 160, 160, 161, 162, 162, 163, 163, 164, 165, 165, 166, 167, 167, 168, 
	168, 169, 170, 170, 171, 172, 173, 173, 174, 175, 175, 176, 177, 178, 178, 179, 180, 181, 181, 
	182, 183, 184, 185, 186, 187, 187, 188, 189, 190, 191, 192, 193, 194, 195, 197, 198, 199, 200, 
	202, 203, 204, 206, 207, 209, 211, 213, 215, 217, 220, 223, 227, 255, 255 
};

/*LED dimming curve*/
/*
const unsigned char shape[] = 
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
	0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05,
	0x05, 0x06, 0x06, 0x06, 0x07, 0x07, 0x07, 0x08, 0x08, 0x08, 0x09, 0x09, 0x0A, 0x0A, 0x0B, 0x0B,
	0x0C, 0x0C, 0x0D, 0x0D, 0x0E, 0x0F, 0x0F, 0x10, 0x11, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1F, 0x20, 0x21, 0x23, 0x24, 0x26, 0x27, 0x29, 0x2B, 0x2C,
	0x2E, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3A, 0x3C, 0x3E, 0x40, 0x43, 0x45, 0x47, 0x4A, 0x4C, 0x4F,
	0x51, 0x54, 0x57, 0x59, 0x5C, 0x5F, 0x62, 0x64, 0x67, 0x6A, 0x6D, 0x70, 0x73, 0x76, 0x79, 0x7C,
	0x7F, 0x82, 0x85, 0x88, 0x8B, 0x8E, 0x91, 0x94, 0x97, 0x9A, 0x9C, 0x9F, 0xA2, 0xA5, 0xA7, 0xAA,
	0xAD, 0xAF, 0xB2, 0xB4, 0xB7, 0xB9, 0xBB, 0xBE, 0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCA, 0xCC, 0xCE,
	0xD0, 0xD2, 0xD3, 0xD5, 0xD7, 0xD8, 0xDA, 0xDB, 0xDD, 0xDE, 0xDF, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5,
	0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xED, 0xEE, 0xEF, 0xEF, 0xF0, 0xF1, 0xF1, 0xF2,
	0xF2, 0xF3, 0xF3, 0xF4, 0xF4, 0xF5, 0xF5, 0xF6, 0xF6, 0xF6, 0xF7, 0xF7, 0xF7, 0xF8, 0xF8, 0xF8,
	0xF9, 0xF9, 0xF9, 0xF9, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFB, 0xFB, 0xFB, 0xFB, 0xFB, 0xFB, 0xFC,
	0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD,
	0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF
};
*/

volatile unsigned char* pwm_addresses[]={&OCR0A, &OCR0B, &OCR1AL, &OCR1BL, &OCR2A, &OCR2B};
unsigned char FSM_status;
volatile unsigned char data_valid;
volatile unsigned long timeout;
int address;
unsigned char shapping;
int RX_index;
volatile unsigned char data_buffer [ANALOG_CHANNELS]={0};
	
	
ISR(USART_RX_vect)
{	
	dmx_interrupt(&FSM_status, address, ANALOG_CHANNELS, &RX_index);
	timeout=0;	
}

ISR(TIMER0_OVF_vect)
{	
	if  (data_valid==FRAME_VALID)
	{		
		if (timeout<TIMEOUT_TRESHOLD && timeout>=0)
		{
			timeout++;
			data_valid=FRAME_VALID;
		}
		else
		{
			timeout=0;
			data_valid=FRAME_DEPRECATED;
		}				
	}		
}

int main (void)
{
	
	/*Variables initializations*/	
	data_valid = FRAME_DEPRECATED;
	timeout=0;
	shapping=check_shapping();
	address=read_address();
	clear_buffer(data_buffer, ANALOG_CHANNELS);
	
	/*Attach callback function's pointers*/
	set_output_data_pointer(data_buffer);
	set_timeout_pointer(&timeout);
	set_valid_frame_flag_pointer(&data_valid);
	
	port_init();
	timer0_interrupts_init();
	timer0_as_pwm_init();
	timer1_as_pwm_init();
	timer2_as_pwm_init();
			
	dmx_init(&FSM_status, &RX_index);	
	
	/*Interrupts enable*/
	sei();
	
	while (1)
	{		
		address=read_address();
		shapping=check_shapping();		
		fire_pwm();
		
	}
	return 1;
			
}

void port_init()
{
	/*Set input addresses*/
	DDRC&= ~DDRC_MASK;
	DDRD&= ~DDRD_MASK;
	DDRB&= ~(1<<PORTB0);
	/*Enable pull-ups*/
	PORTC|=	DDRC_MASK;	
	PORTD|= DDRD_MASK;
	PORTB|= (1<<PORTB0);
	
}
	
void timer0_interrupts_init()
{
	TIMSK0|=(1<<TOIE0);
}

void timer0_as_pwm_init()
{
	DDRD|=0b01100000;		//Set pins as outputs 
	TCNT0=0;				//Clears counter
	OCR0A=0;
	OCR0B=0;
	TCCR0A = 0b10100011;	//Fast PWM mode
	TCCR0B|=(1 << CS00);	//Enables timer
}

void timer1_as_pwm_init()
{
	DDRB|=0b00000110;		//Set pins as outputs
	TCNT1= 0;				//Clears counter
	OCR1A=0;
	OCR1B=0;
	TCCR1A = 0b10100001;	//Fast PWM mode
	TCCR1B|=(1 << WGM12);
	TCCR1B&= ~(1<<WGM13);
	TCCR1B|=(1 << CS10);	//Enables timer
}

void timer2_as_pwm_init(void)
{
	DDRB|= 0b00000001;		//Set pins as outputs
	DDRD|= 0b00001000;
	TCNT2=0;				//Clears counter
	OCR2A=0;
	OCR2B=0;
	TCCR2A= 0b10100011;		//Fast PWM mode
	TCCR2B|= (1<<CS20);
}

unsigned char attach_bit (volatile unsigned char* input_value, unsigned char input_bit, volatile unsigned char* output_value, unsigned char output_bit)
{
	(*output_value) ^=((((*output_value)>>output_bit)^((*input_value)>>input_bit))<<output_bit)&(1<<output_bit);
	return (*output_value);
}

int read_address (void)
{	
	return  ((((~PINC) & PINC_MASK) | (((~PIND) & PIND_MASK_L)<<5)) + (((~PIND) & PIND_MASK_H)>>PIND4)*256);
}

unsigned char check_shapping(void)
{
	return (~PINB & (1<<PINB0));	
}

void fire_pwm(void)
{
	unsigned char i;
	
	for (i=0; i<ANALOG_CHANNELS; i++)
	{
		if (data_valid==FRAME_DEPRECATED || address>=(DMX_MAX_CHANNELS-i))
		{
			*(pwm_addresses[i])=0;
		}
		else if (shapping)
		{
			*(pwm_addresses[i])=shape[data_buffer[i]];
		}
		else
		{
			*(pwm_addresses[i])=data_buffer[i];
		}
	}
}

void clear_buffer(volatile unsigned char* buffer, int size)
{
	int i;
	for (i=0; i<size; i++)
	{
		buffer[i]=0;
	}
}

