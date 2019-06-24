/*
 * alarm.h
 *
 *  Created on: 20.06.2019
 *      Author: jueltzen
 */

#ifndef ALARM_H_
#define ALARM_H_


typedef struct
{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
} alarm_t;

alarm_t alarm;
alarm_t alarmold;

extern volatile uint8_t d;


void setupalarm();
void outputalarm();
void alarmmenu();
void wakeup();
void setalarmtone();



#endif /* ALARM_H_ */
