/*
 * main.c
 *
 * Created: 3/23/2022 12:40:10 PM
 *  Author: Ewout
 */ 

#define F_CPU 8e6
#include <avr/io.h>
#include <util/delay.h>
#include <xc.h>

void wait(int ms)
{
	for (int i=0; i<ms; i++)
	{
		_delay_ms(1);
	}
}


int main(void)
{
    while(1)
    {
        //TODO:: Please write your application code 
    }
}