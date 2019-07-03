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
void lcd_clear();
void lcd_cursor_on();
void lcd_cursor_off();
void lcd_clear_line_1();
void lcd_clear_line_2();


#endif /* LCD_H_ */
