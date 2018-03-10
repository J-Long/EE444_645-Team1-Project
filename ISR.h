#ifndef ISR_H
#define ISR_H

#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "MSPsetup.h"
#include "commands.h"

//------------------------------------------------------------------------------
// definitions
//------------------------------------------------------------------------------
extern unsigned int TXbufLen, TXbufPos;
extern char TXbuf[50];         

//------------------------------------------------------------------------------
// prototypes
//------------------------------------------------------------------------------
void UART_ISR(void);
void TIMER1A0_ISR(void);
void TIMER0B0_ISR(void);
void ADC12_ISR(void);

#endif
