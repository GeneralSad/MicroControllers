/* spi Serial Peripheral Interface
 *	 7 segments display
 *  Master
 *	WvdE
 * 24 febr 2011
 */
/*
 * Project name		: Demo5_5 : spi - 7 segments display
 * Author			: Avans-TI, WvdE, JW
 * Revision History	: 20110228: - initial release;
 * Description		: This program sends data to 1 4-digit display with spi
 * Test configuration:
     MCU:             ATmega128
     Dev.Board:       BIGAVR6
     Oscillator:      External Clock 08.0000 MHz
     Ext. Modules:    Serial 7-seg display
     SW:              AVR-GCC
 * NOTES			: Turn ON switch 15, PB1/PB2/PB3 to MISO/MOSI/SCK
*/

#define F_CPU 8e6
#include <avr/io.h>
#include <util/delay.h>

#define BIT(x)		( 1<<x )
#define DDR_SPI		DDRB					// spi Data direction register
#define PORT_SPI	PORTB					// spi Output register
#define SPI_SCK		1						// PB1: spi Pin System Clock
#define SPI_MOSI	2						// PB2: spi Pin MOSI
#define SPI_MISO	3						// PB3: spi Pin MISO
#define SPI_SS		0						// PB0: spi Pin Slave Select

// wait(): busy waiting for 'ms' millisecond
// used library: util/delay.h
void wait(int ms)
{
	for (int i=0; i<ms; i++)
		{
			_delay_ms(1);
		}
}

void spi_masterInit(void)
{
	DDR_SPI = 0xff;							// All pins output: MOSI, SCK, SS, SS_display as output
	DDR_SPI &= ~BIT(SPI_MISO);				// 	except: MISO input
	PORT_SPI |= BIT(SPI_SS);				// SS_ADC == 1: deselect slave
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1);	// or: SPCR = 0b11010010;
											// Enable spi, MasterMode, Clock rate fck/64, bitrate=125kHz
											// Mode = 0: CPOL=0, CPPH=0;
}


// Write a byte from master to slave
void spi_write( unsigned char data )				
{
	SPDR = data;							// Load byte to Data register --> starts transmission
	while( !(SPSR & BIT(SPIF)) ); 			// Wait for transmission complete 
}

// Write a byte from master to slave and read a byte from slave
// nice to have; not used here
char spi_writeRead( unsigned char data )
{
	SPDR = data;							// Load byte to Data register --> starts transmission
	while( !(SPSR & BIT(SPIF)) ); 			// Wait for transmission complete 
	data = SPDR;							// New received data (eventually, MISO) in SPDR
	return data;							// Return received byte
}

// Select device on pinnumer PORTB
void spi_slaveSelect(unsigned char chipNumber)
{
	PORTB &= ~BIT(chipNumber);
}

// Deselect device on pinnumer PORTB
void spi_slaveDeSelect(unsigned char chipNumber)
{

	PORTB |= BIT(chipNumber);
}

// Write a word = address byte + data byte from master to slave
void spi_writeWord ( unsigned char adress, unsigned char data ) {
		spi_slaveSelect(0);
		spi_write(adress);
		spi_write(data);
		spi_slaveDeSelect(0);
}

// Initialize the driver chip (type MAX 7219)
void displayDriverInit() 
{
	
	spi_writeWord(0x09, 0xFF);
	spi_writeWord(0x0A, 0x04);
	spi_writeWord(0x0B, 0x03);
	spi_writeWord(0x0C, 0x01);
	
}

// Set display on ('normal operation')
void displayOn() 
{
	
	spi_writeWord(0x0C, 0x01);
	
}

// Set display off ('shut down')
void displayOff() 
{
	
	spi_writeWord(0x0C, 0x00);
	
}



void writeLedDisplay( int value ) // toont de waarde van value op het 4-digit display
{
	if (value < 0 || value > 9999)
	{
		return;
	}
	
	spi_writeWord('1', (value % 10));
	spi_writeWord('2', (value % 100)/10);
	spi_writeWord('3', (value % 1000)/100);
	spi_writeWord('4', (value % 10000)/1000);	
	
}


int main()
{
	// inilialize
	DDRB=0x01;					  	// Set PB0 pin as output for display select
	spi_masterInit();              	// Initialize spi module
	displayDriverInit();            // Initialize display chip

 	// clear display (all zero's)
	//for (char i =1; i<=2; i++) was 2, maar moest 4 zijn
	for (char i =1; i<=4; i++)
	{
		spi_writeWord(i, 0);
	}    
	wait(1000);
	
	for (int i = 0; i < 10000; i++)
	{
		writeLedDisplay(i);
		wait(10);	
	}
	
	wait(1000);



  	return (1);
}