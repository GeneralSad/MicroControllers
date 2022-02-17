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


void timer2Init( void ) {
	//OCR2 = 250;				// Compare value of counter 2
	TIMSK |= BIT(7);		// T2 compare match interrupt enable
	sei();					// turn_on intr all
	TCCR2 = 0b00001101;		// Initialize T2: timer, prescaler=1024, compare output disconnected,CTC,RUN
}

volatile int TimerPreset1 = -195;
volatile int TimerPreset2 = -117;
volatile int state = 0;
ISR( TIMER2_COMP_vect ) {
	if (state) {
		TCNT2 = TimerPreset1;
		PORTD ^= BIT(7);
		state = !state;
		
	} else {
		TCNT2 = TimerPreset2;
		PORTD ^= BIT(7);
		state = !state;
	}
}

int main( void ) {
	DDRD = 0xFF;
	DDRC = 0xFF;					// set PORTC for output (toggle PC0)
	timer2Init();

	while (1) {
		// do something else
		PORTC = TCNT2;
		wait(10);			// every 10 ms (busy waiting
	}
}

//------------------------------------------- Opdracht B1

// wait(): busy waiting for 'ms' millisecond
// Used library: util/delay.h
//void wait( int ms ) {
//	for (int tms=0; tms<ms; tms++) {
//		_delay_ms( 1 );			// library function (max 30 ms at 8MHz)
//	}
//}

//volatile int TimerPreset = 0;  // 0xF6, 10 till overflow
// Interrupt routine timer2 overflow
//ISR( TIMER2_OVF_vect ) {
//	TCNT2 = TimerPreset;	// Preset value
//}

// Initialize timer2
//void timer2Init( void ) {
// 	TIMSK |= BIT(6);		// T2 overflow interrupt enable
//	sei();				// turn_on intr all
//	TCCR2 = 0x07;		// Initialize T2: ext.counting, rising edge, run
//}

int opdrachtb1(void) {
	
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