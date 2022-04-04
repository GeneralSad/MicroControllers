#define F_CPU 8e6
#include <avr/io.h>
#include "wait.h"
#include "lcd.h"
#include <string.h>
#include <stdio.h>

#define LCD_E 	3
#define LCD_RS	2

void lcd_strobe_lcd_e(void);
void init_4bits_mode(void);
void lcd_write_string(char *str);
void lcd_write_data(unsigned char byte);
void lcd_write_cmd(unsigned char byte);

void lcd_init() {
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
	
	char reset[] = "                "; //Reset string

	lcd_set_cursor(0);
	lcd_write_string(reset);
	lcd_set_cursor(40);
	lcd_write_string(reset);
}

void lcd_write_string(char *str) {
	
	for(;*str; str++){
		lcd_write_data(*str);
	}

}

void lcd_write_cmd(unsigned char byte)
{
	// First nibble.
	PORTC = byte;
	PORTC &= ~(1<<LCD_RS);
	lcd_strobe_lcd_e();

	// Second nibble
	PORTC = (byte<<4);
	PORTC &= ~(1<<LCD_RS);
	lcd_strobe_lcd_e();
}

void lcd_write_data(unsigned char byte) {
	// First nibble.
	PORTC = byte;
	PORTC |= (1<<LCD_RS);
	lcd_strobe_lcd_e();

	// Second nibble
	PORTC = (byte<<4);
	PORTC |= (1<<LCD_RS);
	lcd_strobe_lcd_e();
}

void lcd_strobe_lcd_e(void) {
	PORTC |= (1<<LCD_E);
	wait(1);
	PORTC &= ~(1<<LCD_E);
	wait(1);
}


void lcd_display_text(char * str) {
	lcd_set_cursor(0);
	lcd_write_string(str);
}

void lcd_set_cursor(int position) {
	lcd_write_cmd(position | (1 << 7));
}