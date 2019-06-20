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
    uint8_t min;
    uint8_t hour;
} alarm_t;

alarm_t alarm;
alarm_t alarmold;


void setupalarm();
void outputalarm();
void alarmmenu();




#endif /* ALARM_H_ */
