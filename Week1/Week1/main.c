/*
 * main.c
 *
 * Created: 2/2/2022 10:45:27 AM
 *  Author: leonv
 */ 

#include <xc.h>

int main(void)
{
    while(1)
    {
        PORTD = 0xAA;
		wait(250);
		PORTD = 0x55;
		wait(250);
    }
}