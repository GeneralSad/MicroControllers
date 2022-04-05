/*
 * main.c
 *
 * Created: 3/23/2022 12:40:10 PM
 *  Author: Ewout,Leon
 */

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
uint32_t convertSerialN(uint8_t *send_data);
uint32_t scanCard();
void cardScanInit();
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
	lcd_display_text(stringHolder);
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

uint32_t key = 0;
int userPrompted = 0;

int main()
{
	lcd_init();
	wait(10);
	cardScanInit();
	wait(10);
	test_write("lcd init");
	
	displayInit();
	wait(500);
	displayChar('1', 0, 0);

	while (1)
	{
		wait(10);
		if (key == 0 && userPrompted == 0)
		{
			userPrompted = 1;
			test_write("Scan master key");
			key = scanCard();
			test_write("key was set");
		}
		
		test_write("present key");
		displayChar('?',0,0);
		uint32_t scanned = scanCard();

		if (scanned != key)
		{
			test_write("key invalid");
			displayChar('X',0,0);
			continue;
		}
		test_write("key valid");
		displayChar('V',0,0);
		
		wait(1000);
	}
}

void cardScanInit()
{
	spi_masterInit();
	wait(10);

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
	byte = mfrc522_read(ComIEnReg);
	mfrc522_write(ComIEnReg, byte | 0x20);
	byte = mfrc522_read(DivIEnReg);
	mfrc522_write(DivIEnReg, byte | 0x80);
}

uint32_t scanCard()
{
	while (1)
	{
		byte = mfrc522_request(PICC_REQALL, str);
		
		if (byte != CARD_FOUND)
		{
			wait(10);
			continue;
		}

		if (readSerial())
		{
			return convertSerialN(str);
		}
		wait(10);
	}
	return 0;
}

uint32_t convertSerialN(uint8_t *send_data)
{
	uint32_t holdefffff = 0;
	for (int i = 0; i < 4; i++)
	{
		holdefffff = holdefffff << 8;
		holdefffff |= send_data[i];
		send_data[i] = 0;
	}

	return holdefffff;
}

int readSerial()
{
	byte = mfrc522_get_card_serial(str);

	if (byte != CARD_FOUND)
		return 0;

	return 1;
}