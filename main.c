/**
 * @authors         Moritz Noerenberg; Jacob Ueltzen
 * @date            June 2019
 * @brief           Project des Faches Mikrorechnerarchitekturen. Entwurf eines C-Programmes zur Implementierung eines Weckers auf dem MPS430 Education-Board.
 *
 */

#include <msp430.h>
#include <stdint.h>
#include <stdio.h>

#include "lcd.h"

//Funktionsankündigungen
void outputtime();
void outputdate();
void timeCorrection();
void startupscreen();
void setuptime();

char schaltjahr();

uint16_t a = 48;                     //Zählervariable für Drehencoder
static volatile uint8_t buttonpressed = 0; //Variable für die 4 Taster, (Taster 1 Bit 0, Taster 2 Bit 1 usw.)
static volatile uint8_t button_flag = 0;
//Typendefinitionen
typedef struct
{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t mon;
    uint16_t year;
    char weekday;
} time_t;

time_t time;

//Initialisierungen:

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





    P2IES |= BIT0 + BIT1 + BIT2 + BIT5 + BIT3;              //interrupt init Button und dreencoder
    P2IFG &= ~(BIT0 + BIT1 + BIT2 + BIT5 + BIT3);
    P2IE |= BIT0 + BIT1 + BIT2 + BIT5 + BIT3;

    __enable_interrupt();

    lcd_init();  //Initialisiert das Display

    startupscreen();

    WDTCTL = WDTPW + WDTTMSEL + WDTCNTCL + WDTSSEL; //Intitialisierung des Watchdog Timers set for 1sec im continous mode, reset to 0, selected ACLK as source
       IE1 |= WDTIE;           //WDT Interrupt enable


    while (1) //Schleife zur sekündlichen Aktualisierung des Display mit Uhrzeit und Datum
    {
        lcd_gotoxy(4, 0);
        outputtime();
        outputdate();
        __low_power_mode_3();
        timeCorrection();
    }

}

/*-----------------------------------------------------------------*/
//ENDE HAUPTPROGRAMM
/*-----------------------------------------------------------------*/

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
    lcd_gotoxy(3, 1);
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
    __delay_cycles(1000000);
    lcd_clear();
    lcd_write("Uhrzeit?:");
    lcd_gotoxy(4, 1);
    outputtime();
    setuptime();
    lcd_clear();

}

void setuptime()
{


    a = 48;


    while (!(button_flag & BIT0))           // studen Zehner
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
        __low_power_mode_3();
    }
    button_flag &= ~BIT0;             //Buttonflag zurücksetzen
    time.hour = (a - 48) * 10;


    a = 48;
    while (!(button_flag & BIT0))           //Stunden einer
    {
        if (a > 57)
        {
            a = 48;
        }
        if (a < 48)
        {
            a = 57;
        }

        lcd_gotoxy(5, 1);
        lcd_put_char(a);
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
        __low_power_mode_3();
    }
    button_flag &= ~BIT0;
    time.min = time.min + (a - 48);



    a = 48;
    while (!(button_flag & BIT0))               //Einstellung Zehner der Sekunden
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
        __low_power_mode_3();
    }
    button_flag &= ~BIT0;
    time.sec = time.sec + (a - 48);

}
/*---------------------------------------------------------------------------------------------
 * INTERRUPTSECTION
 ----------------------------------------------------------------------------------------------*/

#pragma vector=WDT_VECTOR       //Interrupt Vektor, der die Struktur time.sec sekündlich um 1 erhöht.
__interrupt void WDT_ISR()
{
    time.sec++;
    __low_power_mode_off_on_exit();
}

#pragma vector=PORT2_VECTOR     // Interrupt Vektor für Drehencoder und Taster
__interrupt void PORT2_ISR()
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
        button_flag |= BIT3;
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

