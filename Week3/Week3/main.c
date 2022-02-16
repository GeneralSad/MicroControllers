/*
 * main.c
 *
 * Created: 2/16/2022 10:19:11 AM
 *  Author: Ewout
 */ 


#define F_CPU 8e6
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include <xc.h>

void wait( int ms ) {
	for (int i=0; i<ms; i++) {
		_delay_ms( 1 );		// library function (max 30 ms at 8MHz)
	}
}


int main(void)
{
    while(1)
    {
        //TODO:: Please write your application code 
    }
}