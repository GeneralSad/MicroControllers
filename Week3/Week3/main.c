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
#include <stdio.h>

#define BIT(x)			(1 << (x))

// wait(): busy waiting for 'ms' millisecond
// Used library: util/delay.h
void wait( int ms ) {
	for (int tms=0; tms<ms; tms++) {
		_delay_ms( 1 );			// library function (max 30 ms at 8MHz)
	}
}

volatile int TimerPreset = 0;  // 0xF6, 10 till overflow
// Interrupt routine timer2 overflow
ISR( TIMER2_OVF_vect ) {
	TCNT2 = TimerPreset;	// Preset value
}

// Initialize timer2
void timer2Init( void ) {
	TIMSK |= BIT(6);		// T2 overflow interrupt enable
	sei();				// turn_on intr all
	TCCR2 = 0x07;		// Initialize T2: ext.counting, rising edge, run
}

int main(void) {
	
	DDRD &= ~BIT(7);		// PD7 op input: DDRD=xxxx xxx0
	DDRA = 0xFF;			// set PORTA for output (shows countregister)
	
	timer2Init();
	lcd_init();
	lcd_set_cursor(0);
	char str[4];

	
	int lastValue = 0;
	int currentValue = 0;

	while (1) {
		PORTA = TCNT2;		// show value counter 2
		currentValue = TCNT2;
		if (lastValue != currentValue)
		{
			sprintf(str, "%d  ",	currentValue);
			lcd_set_cursor(0);
			lcd_display_text(str);
			
			lastValue = currentValue;
		}
		
		
		wait(100);
	}
}