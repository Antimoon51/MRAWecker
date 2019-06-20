/**
 * @authors         Moritz Noerenberg; Jacob Ueltzen
 * @date            June 2019
 * @breef           Project des Faches Mikrorechnerarchitekturen. Entwurf eines C-Programmes zur Implementierung eines Weckers auf dem MPS430 Education-Board.
 *
 */

#include <msp430.h>
#include <stdint.h>
#include <stdio.h>

#include "lcd.h"
#include "setup.h"

//Funktionsankündigungen
//void outputtime();
//void outputdate();
//void timeCorrection();
//void startupscreen();
//void setuptime();
//void setupdate();
//void weekdayinit();
//void outputday();
//void alarmmenu();
//void setupalarm();
//void outputalarm();

char schaltjahr();

//uint16_t a;                   //Zählervariable für Drehencoder
//uint8_t weekdayi;

//static volatile uint8_t button_flag = 0; //Variable für die 4 Taster, (Taster 1 Bit 0, Taster 2 Bit 1 usw.)
//static volatile uint8_t second_flag = 0; //Flagge ist gesetzt, wenn der Sekundeninterrupt ausgelöst wird;

//Typendefinitionen
//typedef struct
//{
//    uint8_t sec;
//    uint8_t min;
//    uint8_t hour;
//    uint8_t day;
//    uint8_t mon;
//    uint16_t year;
//    char weekday;
//} time_t;

//time_t time;

//typedef struct
//{
//    uint8_t min;
//    uint8_t hour;
//} alarm_t;
//
//time_t time;
//alarm_t alarm;

/*-----------------------------------------------------------------*/
//ANFANG HAUPTPROGRAMM
/*-----------------------------------------------------------------*/

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    time.sec = 0;          //init times
    time.min = 0;
    time.hour = 0;
    time.day = 1;
    time.mon = 1;
    time.year = 2019;

    P2IES |= BIT0 + BIT1 + BIT2 + BIT5 + BIT3; //interrupt init Button und drehencoder
    P2IFG &= ~(BIT0 + BIT1 + BIT2 + BIT5 + BIT3);
    P2IE |= BIT0 + BIT1 + BIT2 + BIT5 + BIT3;

    __enable_interrupt();

    lcd_init();  //Initialisiert das Display

    startupscreen();

    WDTCTL = WDTPW + WDTTMSEL + WDTCNTCL + WDTSSEL; //Intitialisierung des Watchdog Timers set for 1sec im continuos mode, reset to 0, selected ACLK as source
    IE1 |= WDTIE;           //WDT Interrupt enable

    button_flag &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5);

    while (1)
    {
        if (button_flag & BIT5)
        {      //switch 1 enter Alarmmenü
            button_flag &= ~BIT5;
            alarmmenu();
        }

        if (second_flag & BIT0)
        {
            second_flag &= ~BIT0;
            timeCorrection();
            lcd_gotoxy(4, 0);
            outputtime();
            lcd_gotoxy(5, 1);
            outputdate();
            outputday();
        }

        __low_power_mode_3();

    }

}

/*-----------------------------------------------------------------*/
//ENDE HAUPTPROGRAMM
/*-----------------------------------------------------------------*/

//void outputtime() //Funktion zur Ausgabe der Zeit auf dem Display mittels Zerlegung in Einzelkomponenten und zeichenweiser Ausgabe
//{
//    lcd_put_char((time.hour - (time.hour % 10)) / 10 + 48);
//    lcd_put_char((time.hour % 10) + 48);
//    lcd_write(":");
//    lcd_put_char((time.min - (time.min % 10)) / 10 + 48);
//    lcd_put_char((time.min % 10) + 48);
//    lcd_write(":");
//    lcd_put_char((time.sec - (time.sec % 10)) / 10 + 48);
//    lcd_put_char((time.sec % 10) + 48);
//}
//void outputdate()           //Funktion zur Ausgabe des Datums
//{
//    lcd_put_char((time.day - (time.day % 10)) / 10 + 48);
//    lcd_put_char((time.day % 10) + 48);
//    lcd_write(".");
//    lcd_put_char((time.mon - (time.mon % 10)) / 10 + 48);
//    lcd_put_char((time.mon % 10) + 48);
//    lcd_write(".");
//
//    char Jahr[5]; //Zerlegung der Strucktur time.year in ein Array zur einfacheren Ausgabe der vollständigen Jahreszahl
//    snprintf(Jahr, sizeof(Jahr), "%d", time.year);
//    lcd_write(Jahr);
//}
//void timeCorrection()    //Funktion zum Überlauf handling für alle Zeiteinheiten
//{
//    if (time.sec > 59)
//    {
//        time.sec = 0;
//        time.min++;
//
//        if (time.min > 59)
//        {
//            time.min = 0;
//            time.hour++;
//
//            if (time.hour > 23)
//            {
//                time.hour = 0;
//                time.day++;
//                weekdayi++;
//                if (weekdayi > 6)
//                {
//                    weekdayi = 0;
//                }
//
//                if (time.day > 31
//                        && (time.mon == 1 || time.mon == 3 || time.mon == 5
//                                || time.mon == 7 || time.mon == 8
//                                || time.mon == 10 || time.mon == 12))
//                {
//                    time.day = 1;
//                    time.mon++;
//                }
//                else if (time.day > 30
//                        && (time.mon == 4 || time.mon == 6 || time.mon == 9
//                                || time.mon == 11))
//                {
//                    time.day = 1;
//                    time.mon++;
//                }
//                if (time.mon > 12)
//                {
//                    time.mon = 1;
//                    time.year++;
//                }
//                else if (time.mon == 2)
//                {
//                    if (time.year % 4 == 0 && time.year % 100 != 0
//                            || time.year % 400 == 0)
//                    {
//                        if (time.day > 29)
//                        {
//                            time.mon++;
//                            time.day = 1;
//                        }
//                    }
//                    else
//                    {
//                        if (time.day > 28)
//                        {
//                            time.mon++;
//                            time.day = 1;
//                        }
//                    }
//
//                }
//            }
//        }
//    }
//
//}
//void startupscreen()
//{
//
//    lcd_clear();
//    lcd_gotoxy(2, 0);
//    lcd_write("Willkommen!!");
//    __delay_cycles(1000000);
//    lcd_gotoxy(0, 1);
//    lcd_write("Initialisierung:");
//    __delay_cycles(2000000);
//    lcd_clear();
//    lcd_write("Uhrzeit: ?");
//    lcd_gotoxy(4, 1);
//    lcd_cursor_on();           //Cursor ein
//    outputtime();
//    setuptime();
//    lcd_clear();
//    lcd_write("Datum: ?");
//    lcd_gotoxy(3, 1);
//    outputdate();
//    setupdate();
//    weekdayinit();
//    lcd_cursor_off();           //Cursor aus
//    lcd_clear();
//    lcd_write("Initialisierung");
//    lcd_gotoxy(1, 1);
//    lcd_write("abgeschlossen!");
//    __delay_cycles(2000000);
//    lcd_clear();
//}
//void setuptime()
//{
//
//    a = 48;
//
//    while (!(button_flag & BIT0))           // stunden Zehner
//    {
//        if (a > 50)
//        {
//            a = 48;
//        }
//        if (a < 48)
//        {
//            a = 50;
//        }
//
//        lcd_gotoxy(4, 1);
//        lcd_put_char(a);
//        lcd_gotoxy(4, 1);
//        __low_power_mode_3();
//    }
//    button_flag &= ~BIT0;             //Buttonflag zurücksetzen
//    time.hour = (a - 48) * 10;
//
//    a = 48;
//    while (!(button_flag & BIT0))           //Stunden einer
//    {
//        if (time.hour == 20)
//        {
//            if (a > 51)
//            {
//                a = 48;
//            }
//            if (a < 48)
//            {
//                a = 51;
//            }
//        }
//        else
//        {
//            if (a > 57)
//            {
//                a = 48;
//            }
//            if (a < 48)
//            {
//                a = 57;
//            }
//        }
//
//        lcd_gotoxy(5, 1);
//        lcd_put_char(a);
//        lcd_gotoxy(5, 1);
//        __low_power_mode_3();
//    }
//    button_flag &= ~BIT0;
//    time.hour = time.hour + (a - 48);
//
//    a = 48;
//    while (!(button_flag & BIT0))           //minuten Zehner
//    {
//        if (a > 53)
//        {
//            a = 48;
//        }
//        if (a < 48)
//        {
//            a = 53;
//        }
//
//        lcd_gotoxy(7, 1);
//        lcd_put_char(a);
//        lcd_gotoxy(7, 1);
//        __low_power_mode_3();
//    }
//    button_flag &= ~BIT0;
//    time.min = 10 * (a - 48);
//
//    a = 48;
//    while (!(button_flag & BIT0))           //Einstellung Einer der Minuten
//    {
//        if (a > 57)
//        {
//            a = 48;
//        }
//        if (a < 48)
//        {
//            a = 57;
//        }
//        lcd_gotoxy(8, 1);
//        lcd_put_char(a);
//        lcd_gotoxy(8, 1);
//        __low_power_mode_3();
//    }
//    button_flag &= ~BIT0;
//    time.min = time.min + (a - 48);
//
//    a = 48;
//    while (!(button_flag & BIT0))              //Einstellung Zehner der Sekunden
//    {
//        if (a > 53)
//        {
//            a = 48;
//        }
//        if (a < 48)
//        {
//            a = 53;
//        }
//        lcd_gotoxy(10, 1);
//        lcd_put_char(a);
//        lcd_gotoxy(10, 1);
//        __low_power_mode_3();
//    }
//    button_flag &= ~BIT0;
//    time.sec = (a - 48) * 10;
//
//    a = 48;
//    while (!(button_flag & BIT0))           //Einstellen einer sekunde
//    {
//        if (a > 57)
//        {
//            a = 48;
//        }
//        if (a < 48)
//        {
//            a = 57;
//        }
//        lcd_gotoxy(11, 1);
//        lcd_put_char(a);
//        lcd_gotoxy(11, 1);
//        __low_power_mode_3();
//    }
//    button_flag &= ~BIT0;
//    time.sec = time.sec + (a - 48);
//
//}
//void setupdate()
//{
//    a = 48;
//    while (!(button_flag & BIT0))           // Tage Zehner
//    {
//        if (a > 51)
//        {
//            a = 48;
//        }
//        if (a < 48)
//        {
//            a = 51;
//        }
//
//        lcd_gotoxy(3, 1);
//        lcd_put_char(a);
//        lcd_gotoxy(3, 1);
//        __low_power_mode_3();
//    }
//    button_flag &= ~BIT0;             //Buttonflag zurücksetzen
//    time.day = (a - 48) * 10;
//
//    a = 49;
//    while (!(button_flag & BIT0))           //Tage Einer
//    {
//        if (time.day == 30)
//        {
//            if (a > 49)
//            {
//                a = 48;
//            }
//            if (a < 48)
//            {
//                a = 49;
//            }
//        }
//        else
//        {
//            if (a > 57)
//            {
//                a = 48;
//            }
//            if (a < 48)
//            {
//                a = 57;
//            }
//        }
//            lcd_gotoxy(4, 1);
//            lcd_put_char(a);
//            lcd_gotoxy(4, 1);
//            __low_power_mode_3();
//    }
//        button_flag &= ~BIT0;
//        time.day = time.day + (a - 48);
//
//        if (time.day == 0)
//        {
//            time.day = 1;
//            lcd_gotoxy(4, 1);
//            lcd_put_char(time.day + 48);
//            lcd_gotoxy(4, 1);
//        }
//
//        a = 48;
//        while (!(button_flag & BIT0))           // Monate Zehner
//        {
//            if (a > 49)
//            {
//                a = 48;
//            }
//            if (a < 48)
//            {
//                a = 49;
//            }
//
//            lcd_gotoxy(6, 1);
//            lcd_put_char(a);
//            lcd_gotoxy(6, 1);
//            __low_power_mode_3();
//        }
//        button_flag &= ~BIT0;             //Buttonflag zurücksetzen
//        time.mon = (a - 48) * 10;
//
//        a = 49;
//        while (!(button_flag & BIT0))           //Monate Einer
//        {
//            if (time.mon == 10)
//            {
//                if (a > 50)
//                {
//                    a = 48;
//                }
//                if (a < 48)
//                {
//                    a = 50;
//                }
//            }
//            else
//            {
//                if (a > 57)
//                {
//                    a = 48;
//                }
//                if (a < 48)
//                {
//                    a = 57;
//                }
//            }
//            lcd_gotoxy(7, 1);
//            lcd_put_char(a);
//            lcd_gotoxy(7, 1);
//            __low_power_mode_3();
//        }
//        button_flag &= ~BIT0;
//        time.mon = time.mon + (a - 48);
//
//        if (time.mon == 0)
//        {
//            time.mon = 1;
//            lcd_gotoxy(7, 1);
//            lcd_put_char(time.mon + 48);
//            lcd_gotoxy(7, 1);
//        }
//
//        if(time.mon == 2 && time.day > 28){
//            time.day = 28;
//            lcd_gotoxy(3, 1);
//            lcd_put_char(2 + 48);
//            lcd_gotoxy(4, 1);
//            lcd_put_char(8 + 48);
//        }
//
//        a = 50;
//        while (!(button_flag & BIT0))          //Einstellung Tausender der Jahre
//        {
//            if (a > 57)
//            {
//                a = 48;
//            }
//            if (a < 48)
//            {
//                a = 57;
//            }
//            lcd_gotoxy(9, 1);
//            lcd_put_char(a);
//            lcd_gotoxy(9, 1);
//            __low_power_mode_3();
//        }
//        button_flag &= ~BIT0;
//        time.year = (a - 48) * 1000;
//
//        a = 48;
//        while (!(button_flag & BIT0))          //Einstellung Hunderter der Jahre
//        {
//            if (a > 57)
//            {
//                a = 48;
//            }
//            if (a < 48)
//            {
//                a = 57;
//            }
//            lcd_gotoxy(10, 1);
//            lcd_put_char(a);
//            lcd_gotoxy(10, 1);
//            __low_power_mode_3();
//        }
//        button_flag &= ~BIT0;
//        time.year = time.year + (a - 48) * 100;
//
//        a = 49;
//        while (!(button_flag & BIT0))             //Einstellung Zehner der Jahre
//        {
//            if (a > 57)
//            {
//                a = 48;
//            }
//            if (a < 48)
//            {
//                a = 57;
//            }
//            lcd_gotoxy(11, 1);
//            lcd_put_char(a);
//            lcd_gotoxy(11, 1);
//            __low_power_mode_3();
//        }
//        button_flag &= ~BIT0;
//        time.year = time.year + (a - 48) * 10;
//
//        a = 57;
//        while (!(button_flag & BIT0))              //Einstellung Einer der Jahre
//        {
//            if (a > 57)
//            {
//                a = 48;
//            }
//            if (a < 48)
//            {
//                a = 57;
//            }
//            lcd_gotoxy(12, 1);
//            lcd_put_char(a);
//            lcd_gotoxy(12, 1);
//            __low_power_mode_3();
//        }
//        button_flag &= ~BIT0;
//        time.year = time.year + (a - 48);
//    }
//void weekdayinit()
//{
//    uint8_t h;
//    uint16_t k;
//
//    if (time.mon <= 2)
//    {
//        h = time.mon + 12;
//        k = time.year - 1;
//    }
//    else
//    {
//        h = time.mon;
//        k = time.year;
//    }
//
//    weekdayi = (time.day + 2 * h + (3 * h + 3) / 5 + k + k / 4 - k / 100
//            + k / 400 + 1) % 7;             //Wochentagsberechnung
//
//}

//void outputday()
//{
//    lcd_gotoxy(0, 1);
//    switch (weekdayi)
//    {
//    case 0:
//        lcd_write("SUN,");
//        break;
//    case 1:
//        lcd_write("MON,");
//        break;
//    case 2:
//        lcd_write("TUE,");
//        break;
//    case 3:
//        lcd_write("WED,");
//        break;
//    case 4:
//        lcd_write("THU,");
//        break;
//    case 5:
//        lcd_write("FRI,");
//        break;
//    case 6:
//        lcd_write("SAT,");
//        break;
//    }
//
//}

//void alarmmenu()
//{
////    __delay_cycles(200000);
//    lcd_clear();
//    while (!(button_flag & BIT5))
//    {
//        lcd_write("Alarme:");
//        lcd_gotoxy(0, 1);
//        lcd_write("set alarm 1");
//        lcd_gotoxy(0, 1);
//        __delay_cycles(2000000);
//        lcd_write("                 ");
//        lcd_gotoxy(0, 1);
//        outputalarm();
//        lcd_cursor_on();
//        setupalarm();
//        lcd_cursor_off();
//    }
//    button_flag &= ~BIT5;
//    lcd_clear();
//}
//
//void setupalarm()
//{
//    a = 48;
//
//    while (!(button_flag & BIT0))           // stunden Zehner
//    {
//        if (a > 50)
//        {
//            a = 48;
//        }
//        if (a < 48)
//        {
//            a = 50;
//        }
//
//        lcd_gotoxy(0, 1);
//        lcd_put_char(a);
//        lcd_gotoxy(0, 1);
//        __low_power_mode_3();
//    }
//    button_flag &= ~BIT0;             //Buttonflag zurücksetzen
//    alarm.hour = (a - 48) * 10;
//
//    a = 48;
//    while (!(button_flag & BIT0))           //Stunden einer
//    {
//        if (alarm.hour == 20)
//        {
//            if (a > 51)
//            {
//                a = 48;
//            }
//            if (a < 48)
//            {
//                a = 51;
//            }
//        }
//        else
//        {
//            if (a > 57)
//            {
//                a = 48;
//            }
//            if (a < 48)
//            {
//                a = 57;
//            }
//        }
//
//        lcd_gotoxy(1, 1);
//        lcd_put_char(a);
//        lcd_gotoxy(1, 1);
//        __low_power_mode_3();
//    }
//    button_flag &= ~BIT0;
//    alarm.hour = alarm.hour + (a - 48);
//
//    a = 48;
//    while (!(button_flag & BIT0))           //minuten Zehner
//    {
//        if (a > 53)
//        {
//            a = 48;
//        }
//        if (a < 48)
//        {
//            a = 53;
//        }
//
//        lcd_gotoxy(3, 1);
//        lcd_put_char(a);
//        lcd_gotoxy(3, 1);
//        __low_power_mode_3();
//    }
//    button_flag &= ~BIT0;
//    alarm.min = 10 * (a - 48);
//
//    a = 48;
//    while (!(button_flag & BIT0))           //Einstellung Einer der Minuten
//    {
//        if (a > 57)
//        {
//            a = 48;
//        }
//        if (a < 48)
//        {
//            a = 57;
//        }
//        lcd_gotoxy(4, 1);
//        lcd_put_char(a);
//        lcd_gotoxy(4, 1);
//        __low_power_mode_3();
//    }
//    button_flag &= ~BIT0;
//    alarm.min = alarm.min + (a - 48);
//
//}
//
//void outputalarm()
//{
//    alarm.hour = time.hour;
//    alarm.min = time.min;
//
//    lcd_put_char((alarm.hour - (alarm.hour % 10)) / 10 + 48);
//    lcd_put_char((alarm.hour % 10) + 48);
//    lcd_write(":");
//    lcd_put_char((alarm.min - (alarm.min % 10)) / 10 + 48);
//    lcd_put_char((alarm.min % 10) + 48);
//}

/*---------------------------------------------------------------------------------------------
 * INTERRUPTSECTION
 ----------------------------------------------------------------------------------------------*/

#pragma vector=WDT_VECTOR       //Interrupt Vektor, der die Struktur time.sec sekündlich um 1 erhöht.
__interrupt
void WDT_ISR()
{
    time.sec++;
    second_flag = BIT0;
    __low_power_mode_off_on_exit();
}

#pragma vector=PORT2_VECTOR     // Interrupt Vektor für Drehencoder und Taster
__interrupt
void PORT2_ISR()
{
    if (P2IFG & BIT0)
    {
        P2IFG &= ~BIT0;
        button_flag |= BIT0;
        __low_power_mode_off_on_exit();
    }
    if (P2IFG & BIT1)
    {
        P2IFG &= ~BIT1;
        button_flag |= BIT1;
        __low_power_mode_off_on_exit();
    }
    if (P2IFG & BIT2)
    {
        P2IFG &= ~BIT2;
        button_flag |= BIT2;
        __low_power_mode_off_on_exit();
    }
    if (P2IFG & BIT5)
    {
        P2IFG &= ~BIT5;
        button_flag |= BIT5;
        __low_power_mode_off_on_exit();
    }

    if (P2IFG & BIT3)
    {
        P2IFG &= ~BIT3;

        if (P4IN & BIT1)
        {          // downwards
            a--;
        }
        else if (P4IN & BIT2)
        {   //upwards
            a++;
        }

        __low_power_mode_off_on_exit();
    }

}

