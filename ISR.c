#include "ISR.h"

unsigned int count = 0, inputPnt = 0, TXbufPos = 0, TXbufLen = 0;
float time = 0;
char input[20], TXbuf[50];                           

//------------------------------------------------------------------------------
// UART ISR
//------------------------------------------------------------------------------
void UART_ISR(void) __interrupt [USCI_A1_VECTOR]
{
  char temp, buf[30];
  unsigned int i;

  switch(UCA1IV)
  {
  case 0:break;                             // no interrupt
  case 2:                                   // RXIFG
    temp = UCA1RXBUF;
    
    if (temp == '\r')                       // If the character is return
    {
      UARTparse(input, inputPnt) ;          // go parse the UART commands
      for (i=0;i<inputPnt+1;i++)
      {
        input[i] = 0;
      }
      inputPnt = 0;
      count = 0;
      UCA1TXBUF = TXbuf[TXbufPos];
    }
    else if(temp == '\b')                   // If the character is backspace
    {
      snprintf(TXbuf+TXbufLen,50,"\b \b");  // Erase the character in the terminal  
      TXbufLen = (unsigned)strlen(TXbuf);             
      if(inputPnt > 0)                      // move the input pointer back one element
      {
        inputPnt=inputPnt-1;
      }
      UCA1TXBUF = TXbuf[TXbufPos];
    }
    else 
    {
    UCA1TXBUF = temp;                       // RXed character
    input[inputPnt] = temp;
    inputPnt = inputPnt+1;
    count = count+1;
    }
    break;
  case 4:                                   // TXIFG
    if(TXbufPos!=TXbufLen)
    {
      TXbufPos = TXbufPos+1;
      UCA1TXBUF = TXbuf[TXbufPos];  
    }
    else
    {
      TXbufPos = 0;
      TXbufLen = 0;
    }
    break;

  default: break;
  }
}


//------------------------------------------------------------------------------
// Timer A0 ISR
//------------------------------------------------------------------------------
void TIMER1A0_ISR(void) __interrupt [TIMER1_A0_VECTOR]
{
  //  with TACLK = 32768Hz
  //  CCR0 = (1638)/32768 = 50ms
  float temp;
  char tempstring[20], mins[20], secs[20];
  
  TA1CCR0 += 468;                             // Add Offset to CCR0
  P1OUT ^= BIT1;

  if (tempflag == 1){
    time += 1;
    ADC12CTL0 |= ADC12SC;                       // Start Conversion
    temp = ADC12MEM0;                           // Read the ADC value
    // Calibrate and convert the temp to C
    ADC12CTL0 &= ~ADC12SC;                      // Stop Conversion

    // Print the Temp and Time
    snprintf(TXbuf+TXbufLen,50,"%.2f\r\n",temp);
    TXbufLen = (unsigned)strlen(TXbuf);
    UCA1TXBUF = TXbuf[TXbufPos];
  } else {
    time = 0;
  }
}
