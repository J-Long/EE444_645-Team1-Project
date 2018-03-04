#ifndef COMMANDS_H
#define COMMANDS_H

#include <msp430.h>
#include "MSPsetup.h"
#include "ISR.h"
#include <string.h>

//------------------------------------------------------------------------------
// definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// prototypes
//------------------------------------------------------------------------------
void UARTparse ( char *data, unsigned int length);              // Parse UART commands

#endif
