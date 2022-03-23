/*
 * main.c
 *
 * Created: 3/23/2022 12:40:10 PM
 *  Author: Ewout
 */ 

#define F_CPU 8e6
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <xc.h>
#include "display.h"


void wait(int ms)
{
	for (int i=0; i<ms; i++)
	{
		_delay_ms(1);
	}
}



/******************************************************************/
int main( void )
/* 
short:			main() loop, entry point of executable
inputs:			
outputs:	
notes:			Looping forever, trashing the HT16K33
Version :    	DMK, Initial code
*******************************************************************/
{
	displayInit();
	wait(500);

	//displayChar('1', 0, 0);
	
	int a = 0;	

	while(1==1) {
		displayChar((char)(a % 127),0,0);
		wait(500);
		display();
		displayClr();
		wait(500);
		display();
		a++;

	}

	return 1;
}