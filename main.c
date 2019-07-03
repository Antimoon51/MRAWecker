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
#include "alarm.h"
#include "matrix.h"

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

    alarm.hour = 0;
    alarm.min = 0;
    alarm.sec = 0;

    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    P2SEL &= ~BIT4;                              //Konfiguratio für Tonausgabe
    P2DIR &= ~BIT4;
    CCR2 = 37;
    CCTL2 = CCIE + OUTMOD_4;

    CCR0 = MATRIX_UPDATE_INTERVAL;    // Timer A mit regelmäßigen CCR0-Interrupt
    CCTL0 = CCIE;

    TACTL = TASSEL_1 + MC_2 + TACLR;

    P2IES |= BIT0 + BIT1 + BIT2 + BIT5 + BIT3; //interrupt init Button und drehencoder
    P2IFG &= ~(BIT0 + BIT1 + BIT2 + BIT5 + BIT3);
    P2IE |= BIT0 + BIT1 + BIT2 + BIT5 + BIT3;

    lcd_init();  //Initialisiert das Display
    matrix_init();

    __enable_interrupt();

    menuState = menuState_TIME;
    startupscreen();

    WDTCTL = WDTPW + WDTTMSEL + WDTCNTCL + WDTSSEL; //Intitialisierung des Watchdog Timers set for 1sec im continuos mode, reset to 0, selected ACLK as source
    IE1 |= WDTIE;           //WDT Interrupt enable



    button_flag &= ~(BIT0 + BIT1 + BIT2 + BIT3 + BIT5);

    while (1)
    {
        if (button_flag & BIT5)                 //ENTER MAINMENU
        {
            button_flag &= ~BIT5;
            lcd_clear();
            a = 0;
            while (!(button_flag & BIT5))
            {
                lcd_gotoxy(0, 0);
                lcd_write("Hauptmenu");


                    switch (a)
                    {
                    case menuState_TIME:
                        lcd_clear_line_2();
                        lcd_gotoxy(0, 1);
                        lcd_write("Zeitanzeige");
                        menuState = a;
                        break;

                    case menuState_ALARM:
                        lcd_clear_line_2();
                        lcd_write("Meine Alarme");
                        menuState = a;
                        break;

                    case menuState_SETUP:
                        lcd_clear_line_2();
                        lcd_write("Einstellungen");
                        menuState = a;
                        break;

                    }       //switchklammer

                __low_power_mode_3();
                if (a > 2)
                {
                    a = 0;
                }
                if (a < 0)
                {
                    a = 2;
                }

            }
            button_flag &= ~BIT5;
        }

        switch (menuState)
        {

        case menuState_TIME:
            if (second_flag & BIT0)
            {
                second_flag &= ~BIT0;
                lcd_clear();
                timeCorrection();
                lcd_gotoxy(4, 0);
                outputtime();
                lcd_gotoxy(5, 1);
                outputdate();
                outputday();
                array_output();
            }
            break;

        case menuState_ALARM:
            lcd_clear();
            alarmmenu();
            menuState = menuState_TIME;
            break;

        case menuState_SETUP:
            lcd_clear();
            lcd_write("Zeiteinstellung:");
            lcd_gotoxy(4, 1);
            lcd_cursor_on();
            lcd_write("00:00:00");
            setuptime();
            lcd_cursor_off();
            lcd_clear();
            lcd_write("Datum: ?");
            lcd_gotoxy(3, 1);
            lcd_cursor_on();
            lcd_write("01:01:2019");
            setupdate();
            lcd_cursor_off();
            weekdayinit();
            menuState = menuState_TIME;
            break;
        }

        if (time.hour == alarm.hour && time.min == alarm.min
                && alarm.sec == time.sec && alarm.enable == 1)
        {
            matrix_on();
            lcd_clear();
            setalarmtone();

            while (!(button_flag & BIT0))
            {

                wakeup();
                timeCorrection();
                matrix_update();

            }
            button_flag &= ~BIT0;
            matrix_clear();
            stopalarmtone();
            lcd_clear();
        }

        __low_power_mode_3();

    }

}

/*-----------------------------------------------------------------*/
//ENDE HAUPTPROGRAMM
/*-----------------------------------------------------------------*/

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
            button_flag |= BIT3;
        }
        else if (P4IN & BIT2)
        {   //upwards
            a++;
            button_flag |= BIT3;
        }

        __low_power_mode_off_on_exit();
    }

}

#pragma vector=TIMERA0_VECTOR
__interrupt void TIMERA0_ISR()
{
    CCR0 += MATRIX_UPDATE_INTERVAL;
    matrix_update();
}

#pragma vector=TIMERA1_VECTOR
__interrupt void TimerA1_ISR()
{
    switch (TAIV)
    {
    case 2:
        // CCR1 Interrupt
        break;
    case 4:
        // CCR2 Interrupt
        CCR2 += 37;
        break;
    case 10:
        // Timer Overflow
        break;
    }
}

