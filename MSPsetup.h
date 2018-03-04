#ifndef MSPSETUP_H
#define MSPSETUP_H

#include <msp430.h>

//------------------------------------------------------------------------------
// definitions
//------------------------------------------------------------------------------
// Calibrate the Temp Sensor
#define CALADC12_15V_30C  *((unsigned int *)0x1A1A)   // Temperature Sensor Calibration-30 C
#define CALADC12_15V_85C  *((unsigned int *)0x1A1C)   // Temperature Sensor Calibration-85 C

extern unsigned char tempflag;                        // a flag to determine if the ADC should
                                                      // temp data

void SetVcoreUp (unsigned int level);                 // Set core voltage to level(e.g. PMMCOREV_1)
void clockSetup (void);                               // Setup the MCLK, SMCLK 
void UARTsetup (void);                                // Setup the UART interface
void ADCsetup (void);                                 // setup the ADC
void TA1_Setup(void);                                 // Setup the Timer
#endif

