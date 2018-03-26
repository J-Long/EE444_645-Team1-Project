/******************************************************
University of Alaska Fairbanks
EE 444/645 Final Project
Spring 2018

Device: MSP430F5438 

Authors: 
Katherine Dolma
Paul Huske
Justin Long
Christopher O'Shea

Input Pins:
P7.4 - PPG Sensor
P7.5 - EDA Sensor

Power Pins:
P5.0, P5.1

Files:
MSPsetup.c    clock and peripheral setup, pin setup
MSPsetup.h
ISR.c         Interrupt services Routine
ISR.h
commands.c    Contains all intermediate functions
commands.h


********************************************************/

#include <__cross_studio_io.h>
#include "MSPsetup.h"

main(void)
{
  pinSetup();               // Pin Setup Function
  clockSetup();             // Clock Setup Function
  UARTsetup();              // UART Setup Function
  ADC12_Setup();            // ADC Setup Function
  TA1_Setup();              // Timer A1 Setup Function
  TB0_Setup();              // Timer B0 Setup Function
  _EINT();                  // Enable Interrupts
  LPM3;                     // Enter LPM
}
