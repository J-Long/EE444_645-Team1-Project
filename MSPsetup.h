#ifndef MSPSETUP_H
#define MSPSETUP_H

#include <msp430.h>

//------------------------------------------------------------------------------
// definitions
//------------------------------------------------------------------------------

extern unsigned char tempflag;                        // a flag to determine if the ADC should
                                                      // temp data

void SetVcoreUp (unsigned int level);                 // Set core voltage to level(e.g. PMMCOREV_1)
void clockSetup (void);                               // Setup the MCLK, SMCLK 
void UARTsetup (void);                                // Setup the UART interface
void ADC12_Setup (void);                              // Setup the ADC12 for five sensors.
void TA1_Setup(void);                                 // Setup the Timer A1 for UART
void TB0_Setup(void);                                 // Setup the Timer B0 for ADC12
#endif

