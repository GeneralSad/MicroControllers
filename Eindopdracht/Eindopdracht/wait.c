/*
 * wait.c
 *
 * Created: 4-4-2022 12:27:19
 *  Author: Ewout
 */ 
#define F_CPU 8e6
#include <util/delay.h>
#include "wait.h"

void wait(int ms)
{
	for (int i = 0; i < ms; i++)
	{
		_delay_ms(1);
	}
}