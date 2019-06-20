/*
 * alarm.h
 *
 *  Created on: 20.06.2019
 *      Author: jueltzen
 */

#ifndef ALARM_H_
#define ALARM_H_

//extern volatile uint8_t button_flag; //Variable für die 4 Taster, (Taster 1 Bit 0, Taster 2 Bit 1 usw.)
//extern volatile uint8_t second_flag;

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

typedef struct
{
    uint8_t min;
    uint8_t hour;
} alarm_t;

time_t time;
alarm_t alarm;

uint16_t a;                   //Zählervariable für Drehencoder
uint8_t weekdayi;


void setupalarm();
void outputalarm();
void alarmmenu();



#endif /* ALARM_H_ */
