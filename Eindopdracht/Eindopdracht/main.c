/*
 * main.c
 *
 * Created: 3/23/2022 12:40:10 PM
 *  Author: Ewout
 */

#define F_CPU 8e6

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <xc.h>
#include "display.h"
#include "mfrc522.h"
#include "spi.h"
#include "lcd.h"
#include <string.h>
#include "wait.h"
#include <stdio.h>

int readSerial();
/******************************************************************/
// int main( void )
// /*
// short:			main() loop, entry point of executable
// inputs:
// outputs:
// notes:			Looping forever, trashing the HT16K33
// Version :    	DMK, Initial code
// *******************************************************************/
// {
// 	//displayInit();
// 	//wait(500);
//
// 	//displayChar('1', 0, 0);
//
// 	//int a = 0;
//
// 	//while(1==1) {
// 		//displayChar((char)(a % 127),0,0);
// 		//wait(500);
// 		//display();
// 		//displayClr();
// 		//wait(500);
// 		//display();
// 		//a++;
//
// 	//}
//
// 	return 1;
// }

uint8_t SelfTestBuffer[64];

#define holderLength 16
char stringHolder[holderLength];

void resetHolder()
{
	for (size_t i = 0; i < holderLength; i++)
	{
		stringHolder[i] = ' ';
	}
}

void test_write(const char *str)
{
	resetHolder();
	strcpy(stringHolder, str);
	lcd_display_text(stringHolder);
}

void writeHex(int hex)
{
	resetHolder();
	lcd_display_text(stringHolder);
	sprintf(stringHolder, "%d", hex);
	lcd_display_text(stringHolder);
}

uint8_t byte = 255;
uint8_t str[MAX_LEN];

int main()
{
	lcd_init();
	wait(100);

	test_write("lcd init");

	DDRE = 0xff;

	spi_masterInit();
	wait(100);

	// init reader
	mfrc522_init();

	// check version of the reader
	byte = mfrc522_read(VersionReg);
	wait(10);

	if (byte == 0x92)
	{
		test_write("rc522 v2");
	}
	else if (byte == 0x91 || byte == 0x90)
	{
		test_write("rc522 v1");
	}
	else
	{
		test_write("no rc522");
	}

	PORTE = byte;

	byte = mfrc522_read(ComIEnReg);
	mfrc522_write(ComIEnReg, byte | 0x20);
	byte = mfrc522_read(DivIEnReg);
	mfrc522_write(DivIEnReg, byte | 0x80);

	wait(1500);

	while (1)
	{
		byte = mfrc522_request(PICC_REQALL, str);
		writeHex(byte);
		PORTE = byte;
		if (byte == CARD_FOUND)
		{
			if (!readSerial())
				continue;
			byte = mfrc522_get_card_auth(str);
			PORTE = byte;

			lcd_set_cursor(0);
			for (byte = 0; byte < 8; byte++)
			{
				writeHex(str[byte]);
				wait(1000);
			}
			wait(250);
		}

		wait(1000);
	}
}

int readSerial()
{
	test_write("card found");
	byte = mfrc522_get_card_serial(str);
	PORTE = byte;
	if (byte == CARD_FOUND)
	{
		lcd_set_cursor(0);
		for (byte = 0; byte < 8; byte++)
		{
			// writeHex(str[byte]);
			// wait(500);
		}
		wait(250);
	}
	else
	{
		return 0;
		// PORTE = byte;
	}
	return 1;
}