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

#define setupTimeStateTenHour 0
#define setupTimeStateOneHour 1
#define setupTimeStateTenMin  2
#define setupTimeStateOneMin  3
#define setupTimeStateTenSec  4
#define setupTimeStateOneSec  5

#define setupDateStateTenDay 0
#define setupDateStateOneDay 1
#define setupDateStateTenMon 2
#define setupDateStateOneMon 3
#define setupDateStateThoYea 4
#define setupDateStateHunYea 5
#define setupDateStateTenYea 6
#define setupDateStateOneYea 7

uint8_t setupTimeState = 0;
uint8_t setupDateState = 0;

extern volatile uint8_t button_flag = 0; //Variable für die 4 Taster, (Taster 1 Bit 0, Taster 2 Bit 1 usw.)
extern volatile uint8_t second_flag = 0;

extern volatile int16_t a = 0;                 //Zählervariable für Drehencoder
extern volatile uint8_t weekdayi = 0; //Laufvariable für die Wochentage 0=Son ; 6=Sam

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

void startupscreen() //Funktion, die beim einschalten aufgerufen wird und die Initialisierung managed
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

void setuptime()            //Funktion zur Zeiteinstellung
{
    __delay_cycles(5000);
    button_flag = 0;
    setupTimeState = setupTimeStateTenHour;

    a = 48;

    while (!(button_flag & BIT5))
    {

        switch (setupTimeState)
        {

        case setupTimeStateTenHour:
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

            if (button_flag & BIT0)                 // Weiter-Button
            {
                button_flag &= ~BIT0;
                time.hour = (a - 48) * 10;
                a = 48;
                setupTimeState = setupTimeStateOneHour;
            }
            break;

        case setupTimeStateOneHour:
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

            if (button_flag & BIT0)                 // Weiter-Button
            {
                button_flag &= ~BIT0;
                time.hour = time.hour + (a - 48);
                a = 48;
                setupTimeState = setupTimeStateTenMin;
            }

            if (button_flag & BIT1)                 // Zurück-Button
            {
                button_flag &= ~BIT1;
                time.hour = time.hour + (a - 48);
                a = 48;
                setupTimeState = setupTimeStateTenHour;
            }

            break;

        case setupTimeStateTenMin:
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

            if (button_flag & BIT0)                 // Weiter-Button
            {
                button_flag &= ~BIT0;
                time.min = (a - 48) * 10;
                a = 48;
                setupTimeState = setupTimeStateOneMin;
            }

            if (button_flag & BIT1)                 // Zurück-Button
            {
                button_flag &= ~BIT1;
                time.min = (a - 48) * 10;
                a = 48;
                setupTimeState = setupTimeStateOneHour;
            }
            break;

        case setupTimeStateOneMin:
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

            if (button_flag & BIT0)                 // Weiter-Button
            {
                button_flag &= ~BIT0;
                time.min = time.min + (a - 48);
                a = 48;
                setupTimeState = setupTimeStateTenSec;
            }

            if (button_flag & BIT1)                 // Zurück-Button
            {
                button_flag &= ~BIT1;
                time.min = time.min + (a - 48);
                a = 48;
                setupTimeState = setupTimeStateTenMin;
            }
            break;

        case setupTimeStateTenSec:
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

            if (button_flag & BIT0)                 // Weiter-Button
            {
                button_flag &= ~BIT0;
                time.sec = (a - 48) * 10;
                a = 48;
                setupTimeState = setupTimeStateOneSec;
            }

            if (button_flag & BIT1)                 // Zurück-Button
            {
                button_flag &= ~BIT1;
                time.sec = (a - 48) * 10;
                a = 48;
                setupTimeState = setupTimeStateOneMin;
            }
            break;

        case setupTimeStateOneSec:
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

            if (button_flag & BIT0)                 // Weiter-Button
            {
                button_flag &= ~BIT0;
                time.sec = time.sec + (a - 48);
                a = 48;
                setupTimeState = setupTimeStateOneSec;
            }

            if (button_flag & BIT1)                 // Zurück-Button
            {
                button_flag &= ~BIT1;
                time.sec = time.sec + (a - 48);
                a = 48;
                setupTimeState = setupTimeStateTenSec;
            }
            break;

        }                 //switchklammer

    }
}

void setupdate()            //Funktion zur Einstellung des Datums
{
    __delay_cycles(5000);
    button_flag = 0;
    setupDateState = setupDateStateTenDay;
    a = 48;


    while (!(button_flag & BIT5)){
        switch (setupDateState)
        {

        case setupDateStateTenDay:
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

            if (button_flag & BIT0)             //Weiter Button
            {
                button_flag &= ~BIT0;
                time.day = (a - 48) * 10;
                a = 49;
                setupDateState = setupDateStateOneDay;
            }
            break;

        case setupDateStateOneDay:
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

            if (button_flag & BIT0)             //Weiter Button
            {
                button_flag &= ~BIT0;
                time.day = time.day + (a - 48);
                a = 48;
                setupDateState = setupDateStateTenMon;
            }

            if (button_flag & BIT1)             //Zurück Button
            {
                button_flag &= ~BIT1;
                time.day = time.day + (a - 48);
                a = 48;
                setupDateState = setupDateStateTenDay;
            }
            break;

        case setupDateStateTenMon:
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

            if (button_flag & BIT0)             //Weiter Button
            {
                button_flag &= ~BIT0;
                time.mon = (a - 48) * 10;
                a = 49;
                setupDateState = setupDateStateOneMon;
            }

            if (button_flag & BIT1)             //Zurück Button
            {
                button_flag &= ~BIT1;
                time.mon = (a - 48) * 10;
                a = 49;
                setupDateState = setupDateStateOneDay;
            }
            break;

        case setupDateStateOneMon:
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

            if (button_flag & BIT0)             //Weiter Button
            {
                button_flag &= ~BIT0;
                time.mon = time.mon + (a - 48);
                a = 50;
                setupDateState = setupDateStateThoYea;
            }

            if (button_flag & BIT1)             //Zurück Button
            {
                button_flag &= ~BIT1;
                time.mon = time.mon + (a - 48);
                a = 48;
                setupDateState = setupDateStateTenMon;
            }
            break;

        case setupDateStateThoYea:
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

            if (button_flag & BIT0)             //Weiter Button
            {
                button_flag &= ~BIT0;
                time.year = (a - 48) * 1000;
                a = 48;
                setupDateState = setupDateStateHunYea;
            }

            if (button_flag & BIT1)             //Zurück Button
            {
                button_flag &= ~BIT1;
                time.year = (a - 48) * 1000;
                a = 49;
                setupDateState = setupDateStateOneMon;
            }
            break;

        case setupDateStateHunYea:
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

            if (button_flag & BIT0)             //Weiter Button
            {
                button_flag &= ~BIT0;
                time.year = time.year + (a - 48) * 100;
                a = 49;
                setupDateState = setupDateStateTenYea;
            }

            if (button_flag & BIT1)             //Zurück Button
            {
                button_flag &= ~BIT1;
                time.year = time.year + (a - 48) * 100;
                a = 50;
                setupDateState = setupDateStateThoYea;
            }
            break;

        case setupDateStateTenYea:
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

            if (button_flag & BIT0)             //Weiter Button
            {
                button_flag &= ~BIT0;
                time.year = time.year + (a - 48) * 10;
                a = 57;
                setupDateState = setupDateStateOneYea;
            }

            if (button_flag & BIT1)             //Zurück Button
            {
                button_flag &= ~BIT1;
                time.year = time.year + (a - 48) * 10;
                a = 48;
                setupDateState = setupDateStateHunYea;
            }
            break;

        case setupDateStateOneYea:
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

            if (button_flag & BIT0)             //Ende Button
            {
                button_flag &= ~BIT0;
                time.year = time.year + (a - 48);
                a = 57;
                setupDateState = setupDateStateOneYea;
            }

            if (button_flag & BIT1)             //Zurück Button
            {
                button_flag &= ~BIT1;
                time.year = time.year + (a - 48);
                a = 49;
                setupDateState = setupDateStateTenYea;
            }
            break;

        }         //switchklammer

    }
    button_flag &= ~BIT5;
}


void weekdayinit() //Funktion zur Berechnung der Laufvariablen weekdayi mithilfe der Gauß'schen Wochentagsformel
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

weekdayi = (time.day + 2 * h + (3 * h + 3) / 5 + k + k / 4 - k / 100 + k / 400
        + 1) % 7;             //Wochentagsberechnung

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
