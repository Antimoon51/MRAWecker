/*
 * lcd.h
 *
 *  Created on: 17.06.2019
 *      Author: jueltzen
 */

#ifndef LCD_H_
#define LCD_H_



void lcd_init();
void lcd_gotoxy(uint8_t x, uint8_t y);
void lcd_put_char(char c);
void lcd_write(const char *str);


#endif /* LCD_H_ */
