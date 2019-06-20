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

