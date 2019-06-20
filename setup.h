/*
 * setup.h
 *
 *  Created on: 20.06.2019
 *      Author: jueltzen
 */

#ifndef SETUP_H_
#define SETUP_H_

extern volatile uint8_t button_flag; //Variable f�r die 4 Taster, (Taster 1 Bit 0, Taster 2 Bit 1 usw.)
extern volatile uint8_t second_flag;    //sek�ndlich ausl�sendes Flag f�r das LCD, damit nur alle Sekunde aktualisiert wird


//Typendefinition
typedef struct              //Variablen f�r die Weckerzeiten
{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t mon;
    uint16_t year;
} time_t;



time_t time;


extern volatile uint16_t a;                   //Z�hlervariable f�r Drehencoder
extern volatile uint8_t weekdayi;               //Laufvariable f�r die Wochentage 0=Son ; 6=Sam

//Funktionsprototypen
void outputtime();
void outputdate();
void timeCorrection();
void startupscreen();
void setuptime();
void setupdate();
void weekdayinit();
void outputday();


#endif /* SETUP_H_ */
