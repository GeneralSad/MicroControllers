#define F_CPU 8e6
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"

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
	{0x00, 100}, {0x01, 100}, {0x02, 100}, {0x04, 100}, {0x10,100}, {0x20, 100}, {0x40, 100}, {0x80, 100},
	{0x80, 100}, {0x40, 100}, {0x20, 100}, {0x10, 100}, {0x4, 100}, {0x02, 100}, {0x01, 100}, {0x00, 100},
	{0xAA,  50}, {0x55,  50}, {0xAA,  50}, {0x55,  50}, {0xAA, 50}, {0x55,  50}, {0x00, 100},
	{0x18, 100}, {0x24, 100}, {0x42, 100}, {0x81, 100}, 
	{0x0F, 200}, {0xF0, 200}, {0x0F, 200}, {0xF0, 200}, 
	{0x00,0x00}
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

int main(void) {
	
	lcd_init();
	char text[] = "Hello there";
	char text2[] = "Bottom text";
	lcd_set_cursor(0); //Cursor naar eerste regel
	lcd_display_text(text); //Schrijf text naar display
	lcd_set_cursor(40); //Cursor naar tweede regel
	lcd_display_text(text2); //Schrijf text naar display
	return 1;
	
}

int OpdrachtB4(void)
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
		if ((button1 == 1) && (button2 == 1)) //Check of beide knoppen worden ingedrukt
		{
			number = 0;
		} else if (button1 == 1) //Check of knop 1 wordt ingedrukt
		{
			number >= 16 ? number = 16 : number++; //Increment het nummer op het display als het getal niet hoger wordt dan 16, laat het anders op 16 staan
		} else if (button2 == 1) //Check of knop 2 wordt ingedrukt
		{
			number <= 0 ? number = 0 : number--; //Decrement het nummer op het display als het getal niet lager wordt dan 0, laat het anders op 0 staan
		}
		
		button1 = 0;
		button2 = 0;
		wait(10);
		
		display(number); //Zet het nieuwe nummer op het display
	}
	
	return 1;
}

void opdrachtB2() {
	
	//ISR( INT2_vect ) { //Interrupt voor lopen
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