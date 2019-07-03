/*
 * alarm.h
 *
 *  Created on: 20.06.2019
 *      Author: jueltzen
 */

#ifndef ALARM_H_
#define ALARM_H_

uint8_t menuState;

#define menuState_TIME  0
#define menuState_ALARM 1
#define menuState_SETUP 2

typedef struct
{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t enable;
} alarm_t;

alarm_t alarm;
alarm_t alarmold;

extern volatile uint8_t d;


void setupalarm();
void outputalarm();
void alarmmenu();
void wakeup();
void setalarmtone();
void stopalarmtone();


#endif /* ALARM_H_ */
