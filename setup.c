/*
 * setup.c
 *
 *  Created on: 20.06.2019
 *      Author: jueltzen
 */

#include <msp430.h>
#include <stdint.h>
#include <stdio.h>

#include "lcd.h"
#include "setup.h"

extern volatile uint8_t button_flag = 0; //Variable für die 4 Taster, (Taster 1 Bit 0, Taster 2 Bit 1 usw.)
extern volatile uint8_t second_flag = 0;

void outputtime() //Funktion zur Ausgabe der Zeit auf dem Display mittels Zerlegung in Einzelkomponenten und zeichenweiser Ausgabe
{
    lcd_put_char((time.hour - (time.hour % 10)) / 10 + 48);
    lcd_put_char((time.hour % 10) + 48);
    lcd_write(":");
    lcd_put_char((time.min - (time.min % 10)) / 10 + 48);
    lcd_put_char((time.min % 10) + 48);
    lcd_write(":");
    lcd_put_char((time.sec - (time.sec % 10)) / 10 + 48);
    lcd_put_char((time.sec % 10) + 48);
}

void outputdate()           //Funktion zur Ausgabe des Datums
{
    lcd_put_char((time.day - (time.day % 10)) / 10 + 48);
    lcd_put_char((time.day % 10) + 48);
    lcd_write(".");
    lcd_put_char((time.mon - (time.mon % 10)) / 10 + 48);
    lcd_put_char((time.mon % 10) + 48);
    lcd_write(".");

    char Jahr[5]; //Zerlegung der Strucktur time.year in ein Array zur einfacheren Ausgabe der vollständigen Jahreszahl
    snprintf(Jahr, sizeof(Jahr), "%d", time.year);
    lcd_write(Jahr);
}

void timeCorrection()    //Funktion zum Überlauf handling für alle Zeiteinheiten
{
    if (time.sec > 59)
    {
        time.sec = 0;
        time.min++;

        if (time.min > 59)
        {
            time.min = 0;
            time.hour++;

            if (time.hour > 23)
            {
                time.hour = 0;
                time.day++;
                weekdayi++;
                if (weekdayi > 6)
                {
                    weekdayi = 0;
                }

                if (time.day > 31
                        && (time.mon == 1 || time.mon == 3 || time.mon == 5
                                || time.mon == 7 || time.mon == 8
                                || time.mon == 10 || time.mon == 12))
                {
                    time.day = 1;
                    time.mon++;
                }
                else if (time.day > 30
                        && (time.mon == 4 || time.mon == 6 || time.mon == 9
                                || time.mon == 11))
                {
                    time.day = 1;
                    time.mon++;
                }
                if (time.mon > 12)
                {
                    time.mon = 1;
                    time.year++;
                }
                else if (time.mon == 2)
                {
                    if (time.year % 4 == 0 && time.year % 100 != 0
                            || time.year % 400 == 0)
                    {
                        if (time.day > 29)
                        {
                            time.mon++;
                            time.day = 1;
                        }
                    }
                    else
                    {
                        if (time.day > 28)
                        {
                            time.mon++;
                            time.day = 1;
                        }
                    }

                }
            }
        }
    }

}

void startupscreen()
{

    lcd_clear();
    lcd_gotoxy(2, 0);
    lcd_write("Willkommen!!");
    __delay_cycles(1000000);
    lcd_gotoxy(0, 1);
    lcd_write("Initialisierung:");
    __delay_cycles(2000000);
    lcd_clear();
    lcd_write("Uhrzeit: ?");
    lcd_gotoxy(4, 1);
    lcd_cursor_on();           //Cursor ein
    outputtime();
    setuptime();
    lcd_clear();
    lcd_write("Datum: ?");
    lcd_gotoxy(3, 1);
    outputdate();
    setupdate();
    weekdayinit();
    lcd_cursor_off();           //Cursor aus
    lcd_clear();
    lcd_write("Initialisierung");
    lcd_gotoxy(1, 1);
    lcd_write("abgeschlossen!");
    __delay_cycles(2000000);
    lcd_clear();
}

void setuptime()
{

    a = 48;

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

        lcd_gotoxy(4, 1);
        lcd_put_char(a);
        lcd_gotoxy(4, 1);
        __low_power_mode_3();
    }
    button_flag &= ~BIT0;             //Buttonflag zurücksetzen
    time.hour = (a - 48) * 10;

    a = 48;
    while (!(button_flag & BIT0))           //Stunden einer
    {
        if (time.hour == 20)
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

        lcd_gotoxy(5, 1);
        lcd_put_char(a);
        lcd_gotoxy(5, 1);
        __low_power_mode_3();
    }
    button_flag &= ~BIT0;
    time.hour = time.hour + (a - 48);

    a = 48;
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

        lcd_gotoxy(7, 1);
        lcd_put_char(a);
        lcd_gotoxy(7, 1);
        __low_power_mode_3();
    }
    button_flag &= ~BIT0;
    time.min = 10 * (a - 48);

    a = 48;
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
        lcd_gotoxy(8, 1);
        lcd_put_char(a);
        lcd_gotoxy(8, 1);
        __low_power_mode_3();
    }
    button_flag &= ~BIT0;
    time.min = time.min + (a - 48);

    a = 48;
    while (!(button_flag & BIT0))              //Einstellung Zehner der Sekunden
    {
        if (a > 53)
        {
            a = 48;
        }
        if (a < 48)
        {
            a = 53;
        }
        lcd_gotoxy(10, 1);
        lcd_put_char(a);
        lcd_gotoxy(10, 1);
        __low_power_mode_3();
    }
    button_flag &= ~BIT0;
    time.sec = (a - 48) * 10;

    a = 48;
    while (!(button_flag & BIT0))           //Einstellen einer sekunde
    {
        if (a > 57)
        {
            a = 48;
        }
        if (a < 48)
        {
            a = 57;
        }
        lcd_gotoxy(11, 1);
        lcd_put_char(a);
        lcd_gotoxy(11, 1);
        __low_power_mode_3();
    }
    button_flag &= ~BIT0;
    time.sec = time.sec + (a - 48);

}

void setupdate()
{
    a = 48;
    while (!(button_flag & BIT0))           // Tage Zehner
    {
        if (a > 51)
        {
            a = 48;
        }
        if (a < 48)
        {
            a = 51;
        }

        lcd_gotoxy(3, 1);
        lcd_put_char(a);
        lcd_gotoxy(3, 1);
        __low_power_mode_3();
    }
    button_flag &= ~BIT0;             //Buttonflag zurücksetzen
    time.day = (a - 48) * 10;

    a = 49;
    while (!(button_flag & BIT0))           //Tage Einer
    {
        if (time.day == 30)
        {
            if (a > 49)
            {
                a = 48;
            }
            if (a < 48)
            {
                a = 49;
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
        lcd_gotoxy(4, 1);
        lcd_put_char(a);
        lcd_gotoxy(4, 1);
        __low_power_mode_3();
    }
    button_flag &= ~BIT0;
    time.day = time.day + (a - 48);

    if (time.day == 0)
    {
        time.day = 1;
        lcd_gotoxy(4, 1);
        lcd_put_char(time.day + 48);
        lcd_gotoxy(4, 1);
    }

    a = 48;
    while (!(button_flag & BIT0))           // Monate Zehner
    {
        if (a > 49)
        {
            a = 48;
        }
        if (a < 48)
        {
            a = 49;
        }

        lcd_gotoxy(6, 1);
        lcd_put_char(a);
        lcd_gotoxy(6, 1);
        __low_power_mode_3();
    }
    button_flag &= ~BIT0;             //Buttonflag zurücksetzen
    time.mon = (a - 48) * 10;

    a = 49;
    while (!(button_flag & BIT0))           //Monate Einer
    {
        if (time.mon == 10)
        {
            if (a > 50)
            {
                a = 48;
            }
            if (a < 48)
            {
                a = 50;
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
        lcd_gotoxy(7, 1);
        lcd_put_char(a);
        lcd_gotoxy(7, 1);
        __low_power_mode_3();
    }
    button_flag &= ~BIT0;
    time.mon = time.mon + (a - 48);

    if (time.mon == 0)
    {
        time.mon = 1;
        lcd_gotoxy(7, 1);
        lcd_put_char(time.mon + 48);
        lcd_gotoxy(7, 1);
    }

    if (time.mon == 2 && time.day > 28)
    {
        time.day = 28;
        lcd_gotoxy(3, 1);
        lcd_put_char(2 + 48);
        lcd_gotoxy(4, 1);
        lcd_put_char(8 + 48);
    }

    a = 50;
    while (!(button_flag & BIT0))          //Einstellung Tausender der Jahre
    {
        if (a > 57)
        {
            a = 48;
        }
        if (a < 48)
        {
            a = 57;
        }
        lcd_gotoxy(9, 1);
        lcd_put_char(a);
        lcd_gotoxy(9, 1);
        __low_power_mode_3();
    }
    button_flag &= ~BIT0;
    time.year = (a - 48) * 1000;

    a = 48;
    while (!(button_flag & BIT0))          //Einstellung Hunderter der Jahre
    {
        if (a > 57)
        {
            a = 48;
        }
        if (a < 48)
        {
            a = 57;
        }
        lcd_gotoxy(10, 1);
        lcd_put_char(a);
        lcd_gotoxy(10, 1);
        __low_power_mode_3();
    }
    button_flag &= ~BIT0;
    time.year = time.year + (a - 48) * 100;

    a = 49;
    while (!(button_flag & BIT0))             //Einstellung Zehner der Jahre
    {
        if (a > 57)
        {
            a = 48;
        }
        if (a < 48)
        {
            a = 57;
        }
        lcd_gotoxy(11, 1);
        lcd_put_char(a);
        lcd_gotoxy(11, 1);
        __low_power_mode_3();
    }
    button_flag &= ~BIT0;
    time.year = time.year + (a - 48) * 10;

    a = 57;
    while (!(button_flag & BIT0))              //Einstellung Einer der Jahre
    {
        if (a > 57)
        {
            a = 48;
        }
        if (a < 48)
        {
            a = 57;
        }
        lcd_gotoxy(12, 1);
        lcd_put_char(a);
        lcd_gotoxy(12, 1);
        __low_power_mode_3();
    }
    button_flag &= ~BIT0;
    time.year = time.year + (a - 48);
}



void weekdayinit()
{
    uint8_t h;
    uint16_t k;

    if (time.mon <= 2)
    {
        h = time.mon + 12;
        k = time.year - 1;
    }
    else
    {
        h = time.mon;
        k = time.year;
    }

    weekdayi = (time.day + 2 * h + (3 * h + 3) / 5 + k + k / 4 - k / 100
            + k / 400 + 1) % 7;             //Wochentagsberechnung

}


void outputday()
{
    lcd_gotoxy(0, 1);
    switch (weekdayi)
    {
    case 0:
        lcd_write("SUN,");
        break;
    case 1:
        lcd_write("MON,");
        break;
    case 2:
        lcd_write("TUE,");
        break;
    case 3:
        lcd_write("WED,");
        break;
    case 4:
        lcd_write("THU,");
        break;
    case 5:
        lcd_write("FRI,");
        break;
    case 6:
        lcd_write("SAT,");
        break;
    }

}
