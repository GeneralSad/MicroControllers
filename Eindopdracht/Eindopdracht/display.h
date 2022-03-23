/*
 * display.h
 *
 * Created: 1/16/2018 8:17:18 AM
 *  Author: Gebruiker
 */ 


#ifndef DISPLAY_H_
#define DISPLAY_H_

void displayInit(void);
void displaySetPixel(uint8_t x, uint8_t y);
void displayClrPixel(uint8_t x, uint8_t y);
void displayChar(char ch, uint8_t x, uint8_t y);
void displayString(char *str, uint8_t x, uint8_t y);
void displayRotl(void);
void displayRotr(void);
void displayClr(void);

void display(void);





#endif /* DISPLAY_H_ */