/*
 * wait.c
 *
 * Created: 4-4-2022 12:27:19
 *  Author: Ewout
 */ 
#include <util/delay.h>

void wait(int ms)
{
	for (int i = 0; i < ms; i++)
	{
		_delay_ms(10);
	}
}