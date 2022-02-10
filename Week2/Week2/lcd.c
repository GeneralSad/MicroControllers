/*
 * lcd.c
 *
 * Created: 9-2-2022 15:05:22
 *  Author: leonv
 */ 

#define F_CPU 8e6
#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include <string.h>

#define LCD_E 	3
#define LCD_RS	2

void lcd_strobe_lcd_e(void);
void init_4bits_mode(void);
void lcd_write_string(char *str);
void lcd_write_data(unsigned char byte);
void lcd_write_cmd(unsigned char byte);

void lcd_writeLine1 ( char text1[] );

void init() {
		// PORTC output mode and all low (also E and RS pin)
		DDRC = 0xFF;
		PORTC = 0x00;

		// Step 2 (table 12)
		PORTC = 0x20;	// function set
		lcd_strobe_lcd_e();

		// Step 3 (table 12)
		PORTC = 0x20;   // function set
		lcd_strobe_lcd_e();
		PORTC = 0x80;
		lcd_strobe_lcd_e();

		// Step 4 (table 12)
		PORTC = 0x00;   // Display on/off control
		lcd_strobe_lcd_e();
		PORTC = 0xF0;
		lcd_strobe_lcd_e();

		// Step 4 (table 12)
		PORTC = 0x00;   // Entry mode set
		lcd_strobe_lcd_e();
		PORTC = 0x60;
		lcd_strobe_lcd_e();
		
	char reset[] = "                ";
	set_cursor(0);
	lcd_write_string(reset);
	set_cursor(40);
	lcd_write_string(reset);
}

void lcd_write_string(char *str) {
	for(;*str; str++){
		lcd_write_data(*str);
	}
	
// 	int length = strlen(str);
// 	int maxLineLength = 16;
// 	
// 	if (length > maxLineLength)
// 	{	
// 		int lengthRegel2 = length - maxLineLength;
// 		char subbuff[lengthRegel2];
// 		memcpy(subbuff, &str[length], lengthRegel2);
// 		subbuff[lengthRegel2] = '\0';
// 		set_cursor(40);
// 		lcd_write_data(*subbuff);
// 	}

}void lcd_write_cmd(unsigned char byte)
{
	// First nibble.
	PORTC = byte;
	PORTC &= ~(1<<LCD_RS);
	lcd_strobe_lcd_e();

	// Second nibble
	PORTC = (byte<<4);
	PORTC &= ~(1<<LCD_RS);
	lcd_strobe_lcd_e();
}void lcd_write_data(unsigned char byte) {
	// First nibble.
	PORTC = byte;
	PORTC |= (1<<LCD_RS);
	lcd_strobe_lcd_e();

	// Second nibble
	PORTC = (byte<<4);
	PORTC |= (1<<LCD_RS);
	lcd_strobe_lcd_e();
}void lcd_strobe_lcd_e(void) {
	PORTC |= (1<<LCD_E);
	_delay_ms(1);
	PORTC &= ~(1<<LCD_E);
	_delay_ms(1);
}

void display_text(char *str) {
	//set_cursor(0);
	lcd_write_string(str);
}

void set_cursor(int position) {
	lcd_write_cmd(position | (1 << 7));
}