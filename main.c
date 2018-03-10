#include <__cross_studio_io.h>
#include "MSPsetup.h"

main(void)
{
  clockSetup();             // Clock Setup Function
  UARTsetup();              // UART Setup Function
  ADC12_Setup();            // ADC Setup Function
  TA1_Setup();              // Timer A1 Setup Function
  TB0_Setup();              // Timer B0 Setup Function
  _EINT();                  // Enable Interrupts
  LPM3;                     // Enter LPM
}

/*
                _____
             ,-"     "-.
            / o       o \
           /   \     /   \
          /     )-"-(     \
         /     ( 6 6 )     \
        /       \ " /       \
       /         )=(         \
      /   o   .--"-"--.   o   \
     /    I  /  -   -  \  I    \
 .--(    (_}y/\       /\y{_)    )--.
(    ".___l\/__\_____/__\/l___,"    )
 \                                 /
  "-._      o O o O o O o      _,-"
      `--Y--.___________.--Y--'
         |==.___________.==|
         `==.___________.=='

*/