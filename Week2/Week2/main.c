/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** ioisr.c
**
** Beschrijving:	ISR on PORTD demonstrattion  
** Target:			AVR mcu
** Build:			avr-gcc -std=c99 -Wall -O3 -mmcu=atmega128 -D F_CPU=8000000UL -c ioisr.c
**					avr-gcc -g -mmcu=atmega128 -o ioisr.elf ioisr.o
**					avr-objcopy -O ihex ioisr.elf ioisr.hex 
**					or type 'make'
** Author: 			dkroeske@gmail.com
** -------------------------------------------------------------------------*/

#define F_CPU 8e6
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


/******************************************************************
short:			Busy wait number of millisecs
inputs:			int ms (Number of millisecs to busy wait)
outputs:	
notes:			Busy wait, not very accurate. Make sure (external)
				clock value is set. This is used by _delay_ms inside
				util/delay.h
Version :    	DMK, Initial code
*******************************************************************/
void wait( int ms ) {
	for (int i=0; i<ms; i++) {
		_delay_ms( 1 );		// library function (max 30 ms at 8MHz)
	}
}


typedef struct {
	unsigned char data;
} PATTERN_STRUCT;

PATTERN_STRUCT pattern[] = {
	{0b00111111}, {0b00000110}, {0b01011011}, {0b01001111}, {0b01100110}, {0b01101101}, {0b01111101}, {0b00000111}, {0b01111111}, {0b01100111},
	{0b01110111}, {0b01111100}, {0b00111001}, {0b01011110}, {0b01111001}, {0b01110001}
};

typedef struct {
	unsigned char data;
	unsigned int delay ;
} PATTERN_STRUCT2;

PATTERN_STRUCT2 pattern2[] = {
	{0x00, 100}, {0x01, 100}, {0x02, 100}, {0x04, 100}, {0x10, 100}, {0x20, 100}, {0x40, 100}, {0x80, 100}, //een kant op
	{0x80, 100}, {0x40, 100}, {0x20, 100}, {0x10, 100}, {0x4, 100}, {0x02, 100}, {0x01, 100}, {0x00, 100}, //andere kant op
	{0xAA,  50}, {0x55,  50},{0xAA,  50}, {0x55,  50},{0xAA,  50}, {0x55,  50},{0x00, 100}, //knipperen
	{0x81, 100}, {0x42, 100}, {0x24, 100}, {0x18, 100}, // naar midden vanuit buiten
	{0x18, 100}, {0x24, 100}, {0x42, 100}, {0x81, 100}, //naar buiten vanuit midden
	{0x0F, 200}, {0xF0, 200}, {0x0F, 200}, {0xF0, 200}, // half om half
	{0x00, 0x00}
};

void display(int digit) {
	if ((digit >= 0) && (digit <= 15)) {
		PORTE = pattern[digit].data;
		} else {
		PORTE = pattern[14].data;
	}
	
}







unsigned int number = 0;
int button1 = 0;
int button2 = 0;

ISR( INT2_vect ) {
	button2 = 1;
}

ISR( INT1_vect ) {
	button1 = 1;
}



int main(void)
{
DDRE = 0xFF;


while(1) {
	display(number);
	
	int index = 0;
	while( pattern2[index].delay != 0 ) {
		// Write data to PORTD
		PORTE = pattern2[index].data;
		// wait
		wait(pattern2[index].delay);
		// increment for next round
		index++;
	}
	wait(1);
}

return 1;
}

int opdrachtB3( void ) {
	DDRD = 0xF0;
	DDRE = 0xFF;
	
	EICRA |= 0b00111100;			// INT1 falling edge, INT0 rising edge
	EIMSK |= 0x06;			// Enable INT2 & INT1
	display(number);
	sei();
	
	while(1) {
		if ((button1 == 1) && (button2 == 1)) 
		{
			number = 0;
		} else if (button1 == 1)
		{
			number >= 16 ? number = 16 : number++;
		} else if (button2 == 1)
		{
			number <= 0 ? number = 0 : number--;
		}
		
		button1 = 0;
		button2 = 0;
		wait(10);
		
		display(number);
	}
	
	return 1;
}

void opdrachtB2() {
	
	//ISR( INT2_vect ) {
	//	if (PINE != 0b00000001)
	//	{
	//		PORTE = PINE>>1;
	//	}
	//}
	

	//ISR( INT1_vect ) {
	//	if (PINE != 0b10000000)
	//	{
	//		PORTE = PINE<<1;
	//	}
	//}
	
		// Init I/O
		DDRD = 0xF0;			// PORTD(7:4) output, PORTD(3:0) input
		DDRE = 0xFF;
		PORTE = 0b00000001;

		// Init Interrupt hardware
		EICRA |= 0b00111100;			// INT1 falling edge, INT0 rising edge
		EIMSK |= 0x06;			// Enable INT1 & INT0
		
		// Enable global interrupt system
		//SREG = 0x80;			// Of direct via SREG of via wrapper
		sei();

		while (1) {
			PORTD ^= (1<<7);	// Toggle PORTD.7
			wait( 500 );
		}
}