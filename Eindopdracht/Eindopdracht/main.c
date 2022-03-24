/*
 * main.c
 *
 * Created: 3/23/2022 12:40:10 PM
 *  Author: Ewout
 */ 

#define F_CPU 8e6
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <xc.h>
#include "display.h"
#include "card_reader.h"


void wait(int ms)
{
	for (int i=0; i<ms; i++)
	{
		_delay_ms(1);
	}
}



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

int main()
{
	uint8_t byte;
	uint8_t str[MAX_LEN];
	_delay_ms(50);
	
	spi_masterInit();
	_delay_ms(1000);
/*	LCDClear();*/
	
	//init reader
	RC522_init();
	
	//check version of the reader
	byte = RC522_read(VersionReg);
	if(byte == 0x92)
	{
// 		LCDWriteStringXY(2,0,"MIFARE RC522v2");
// 		LCDWriteStringXY(4,1,"Detected");
	}else if(byte == 0x91 || byte==0x90)
	{
// 		LCDWriteStringXY(2,0,"MIFARE RC522v1");
// 		LCDWriteStringXY(4,1,"Detected");
	}else
	{
		/*LCDWriteStringXY(0,0,"No reader found");*/
	}
	
	byte = RC522_read(ComIEnReg);
	RC522_write(ComIEnReg,byte|0x20);
	byte = RC522_read(DivIEnReg);
	RC522_write(DivIEnReg,byte|0x80);
	
	_delay_ms(1500);
/*	LCDClear();*/
	
	while(1){
		byte = RC522_request(PICC_REQALL,str);
/*		LCDHexDumpXY(0,0,byte);*/
		_delay_ms(1000);
	}
}