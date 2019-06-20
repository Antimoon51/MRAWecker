/*
 * alarm.c
 *
 *  Created on: 20.06.2019
 *      Author: jueltzen
 */

#include <msp430.h>
#include <stdint.h>
#include <stdio.h>

#include "lcd.h"
#include "setup.h"
#include "alarm.h"

void setupalarm()
{
    alarmold.hour = alarm.hour;
    alarmold.min = alarm.min;

    a = alarmold.hour%10 + 48;

    while (!(button_flag & BIT0))           // stunden Zehner
    {
        if (a > 50)
        {
            a = 48;
        }
        if (a < 48)
        {
            a = 50;
        }

        lcd_gotoxy(0, 1);
        lcd_put_char(a);
        lcd_gotoxy(0, 1);
        __low_power_mode_3();
    }
    __delay_cycles(5000L);                  //entprellen des Tasters
    button_flag &= ~BIT0;             //Buttonflag zurücksetzen
    alarm.hour = (a - 48) * 10;

    a = alarmold.hour - (alarmold.hour%10)*10 + 48;
    while (!(button_flag & BIT0))           //Stunden einer
    {
        if (alarm.hour == 20)
        {
            if (a > 51)
            {
                a = 48;
            }
            if (a < 48)
            {
                a = 51;
            }
        }
        else
        {
            if (a > 57)
            {
                a = 48;
            }
            if (a < 48)
            {
                a = 57;
            }
        }

        lcd_gotoxy(1, 1);
        lcd_put_char(a);
        lcd_gotoxy(1, 1);
        __low_power_mode_3();
    }
    __delay_cycles(5000L);                  //entprellen des Tasters
    button_flag &= ~BIT0;
    alarm.hour = alarm.hour + (a - 48);

    a = alarmold.min%10 + 48;;
    while (!(button_flag & BIT0))           //minuten Zehner
    {
        if (a > 53)
        {
            a = 48;
        }
        if (a < 48)
        {
            a = 53;
        }

        lcd_gotoxy(3, 1);
        lcd_put_char(a);
        lcd_gotoxy(3, 1);
        __low_power_mode_3();
    }
    __delay_cycles(5000L);                  //entprellen des Tasters
    button_flag &= ~BIT0;
    alarm.min = 10 * (a - 48);

    a = alarmold.min - (alarmold.min%10)*10 + 48;
    while (!(button_flag & BIT0))           //Einstellung Einer der Minuten
    {
        if (a > 57)
        {
            a = 48;
        }
        if (a < 48)
        {
            a = 57;
        }
        lcd_gotoxy(4, 1);
        lcd_put_char(a);
        lcd_gotoxy(4, 1);
        __low_power_mode_3();
    }
    __delay_cycles(5000L);                  //entprellen des Tasters
    button_flag &= ~BIT0;
    alarm.min = alarm.min + (a - 48);

}

void outputalarm()
{
    lcd_put_char((alarm.hour - (alarm.hour % 10)) / 10 + 48);
    lcd_put_char((alarm.hour % 10) + 48);
    lcd_write(":");
    lcd_put_char((alarm.min - (alarm.min % 10)) / 10 + 48);
    lcd_put_char((alarm.min % 10) + 48);
}

void alarmmenu()
{
    lcd_clear();
    while (!(button_flag & BIT5))
    {
        lcd_write("Alarme:");
        lcd_gotoxy(0, 1);
        lcd_write("set alarm 1");
        lcd_gotoxy(0, 1);
        __low_power_mode_3();

        if (button_flag & BIT2)
        {
            __delay_cycles(5000L);          //entprellen des Tasters
            button_flag &= ~BIT2;
            lcd_gotoxy(0, 1);
            lcd_write("                 ");
            lcd_gotoxy(0, 1);
            outputalarm();
            lcd_cursor_on();
            setupalarm();
            lcd_cursor_off();
            button_flag = 0;
        }
    }
    __delay_cycles(5000L);                  //entprellen des Tasters
    button_flag &= ~BIT5;
    lcd_clear();
}

