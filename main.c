#include <__cross_studio_io.h>
#include "MSPsetup.h"

main(void)
{
  clockSetup();             // Clock Setup Function
  UARTsetup();              // UART Setup Function
  ADCsetup();               // ADC Setup Function
  TA1_Setup();              // Timer A1 Setup Function
  _EINT();                  // Enable Interrupts
  LPM3;                     // Enter LPM
}
