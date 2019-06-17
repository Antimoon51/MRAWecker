#include <msp430.h>
#include <stdint.h>
#include <stdio.h>

#include "lcd.h"

void outputtime();
void outputdate();
void timeCorrection();
char schaltjahr();
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

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    WDTCTL = WDTPW + WDTTMSEL + WDTCNTCL + WDTSSEL;
    IE1 |= WDTIE;

    time.sec = 55;
    time.min = 59;
    time.hour = 23;
    time.day = 28;
    time.mon = 2;
    time.year = 2019;

    __enable_interrupt();

    lcd_init();
    outputtime(time);

    while (1)
    {
        outputtime();
        outputdate();
        __low_power_mode_3();
        timeCorrection();


    }

}

void outputtime()
{
    lcd_gotoxy(4, 0);
    lcd_put_char((time.hour - (time.hour % 10)) / 10 + 48);
    lcd_put_char((time.hour % 10) + 48);
    lcd_write(":");
    lcd_put_char((time.min - (time.min % 10)) / 10 + 48);
    lcd_put_char((time.min % 10) + 48);
    lcd_write(":");
    lcd_put_char((time.sec - (time.sec % 10)) / 10 + 48);
    lcd_put_char((time.sec % 10) + 48);
}

void timeCorrection()
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
                  if (time.year % 4 == 0 && time.year % 100 != 0 || time.year % 400 == 0)
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


void outputdate()
{
    lcd_gotoxy(3, 1);
    lcd_put_char((time.day - (time.day % 10)) / 10 + 48);
    lcd_put_char((time.day % 10) + 48);
    lcd_write(".");
    lcd_put_char((time.mon - (time.mon % 10)) / 10 + 48);
    lcd_put_char((time.mon % 10) + 48);
    lcd_write(".");

    char Jahr[5];
    snprintf(Jahr, sizeof(Jahr), "%d", time.year);
    lcd_write(Jahr);
}

#pragma vector=WDT_VECTOR
__interrupt void WDT_ISR()
{
    time.sec++;
    __low_power_mode_off_on_exit();
}
