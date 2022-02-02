/*
 * main.c
 *
 * Created: 2/2/2022 10:45:27 AM
 *  Author: leonv
 */ 

#define F_CPU 8e6

// #include <xc.h>
#include <avr/io.h>
#include <util/delay.h>

void wait (int ms) {
	for (int i = 0; i < ms; i++)
	{
		_delay_ms(1);
	}
}

int main(void)
{
	DDRD = 0b11111111;
	
	while(1)
	{
		PORTD = 0x80;
		wait(250);
		PORTD = 0x40;
		wait(250);
	}
	
	return 1;
	
}

int opdrachtb2(void)
{
	DDRD = 0b11111111;
	
	while(1)
	{
		PORTD = 0x80;
		wait(250);
		PORTD = 0x40;
		wait(250);
	}
	
	return 1;
	
}