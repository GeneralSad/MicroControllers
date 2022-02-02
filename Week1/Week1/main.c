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

typedef struct {
	unsigned char data;
	unsigned int delay ;
} PATTERN_STRUCT;

PATTERN_STRUCT pattern[] = {
	{0x00, 100}, {0x01, 100}, {0x02, 100}, {0x04, 100}, {0x10, 100}, {0x20, 100}, {0x40, 100}, {0x80, 100}, //een kant op
	{0x80, 100}, {0x40, 100}, {0x20, 100}, {0x10, 100}, {0x4, 100}, {0x02, 100}, {0x01, 100}, {0x00, 100}, //andere kant op
	{0xAA,  50}, {0x55,  50},{0xAA,  50}, {0x55,  50},{0xAA,  50}, {0x55,  50},{0x00, 100}, //knipperen
	{0x81, 100}, {0x42, 100}, {0x24, 100}, {0x18, 100}, // naar midden vanuit buiten
	{0x18, 100}, {0x24, 100}, {0x42, 100}, {0x81, 100}, //naar buiten vanuit midden
	{0x0F, 200}, {0xF0, 200}, {0x0F, 200}, {0xF0, 200}, // half om half
	{0x00, 0x00}
};

int opdrachtb5(void);

int main( void )
{
	opdrachtb5();
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

int opdrachtb3( void )
{
	DDRD = 0b11111111;					// PORTD.7 input all other bits output
	DDRC = 0b11111110;
	
	while (1) {
		if (PINC & 0x1) {
			PORTD = 0x80;
			wait(250);
			PORTD = 0x00;
			wait(250);
			} else {
			PORTD = 0x00;				// write 0 to all the bits of PortD
		}
	}

	return 1;
}

int opdrachtb4(void)
{
	DDRD = 0b11111111;
	int increment = 1;
	int value = 0x01;
	
	while(1)
	{
		
		if (value == 0x80)
		{
			increment = -1;
		} else if (value == 0x01)
		{
			increment = 1;
		}
		
		if (increment == 1)
		{
			value = value << 1;
		} else
		{
			value = value >> 1;
		}
		
		PORTD = value;
		wait(50);
	}
	
	return 1;
}

int opdrachtb5(void)
{
	DDRD = 0b11111111;					// PORTD all output

	while (1==1) {
	// Set index to begin of pattern array
		int index = 0;
	// as long as delay has meaningful content
		while( pattern[index].delay != 0 ) {
			// Write data to PORTD
			PORTD = pattern[index].data;
			// wait
			wait(pattern[index].delay);
			// increment for next round
			index++;
			}
	}
	return 1;
}
