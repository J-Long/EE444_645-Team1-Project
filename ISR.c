#include "ISR.h"

unsigned int count = 0, inputPnt = 0, TXbufPos = 0, TXbufLen = 0;
float time = 0;
char input[20], TXbuf[50], A_flag = 0;

// Variables for Timer B to service ADC12. The unsigned int ones may change later depending on how we
// setup UART to access ADC12 stored memory.
unsigned int accel_x = 0, accel_y = 0, accel_z = 0, ppg_var = 0, eda_var = 0;
enum Sensor_Channels {Ax = 0, Ay = 1, Az = 2, PPG = 3, EDA = 4};
enum Sensor_Channels current_channel = Ax;                           

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
  
  TA1CCR0 += 4680;                             // Add Offset to CCR0
  P1OUT ^= BIT1;

  A_flag = 1;
}

//------------------------------------------------------------------------------
// Timer B0 ISR
//------------------------------------------------------------------------------
void TIMER0B0_ISR(void) __interrupt [TIMER0_B0_VECTOR] {

  ADC12CTL0 &= ~ADC12ENC; // Disable conversion
  ADC12CTL1 &= 0x0FFF; // Clear ADC12A conversion start address
  ADC12CTL1 |= 0x1000 * (int)current_channel; // lets us determine the appropriate channel without
                                              // putting the CSTARTADD assignment statement in every 
                                              // case of the switch function
  ADC12CTL0 |= ADC12ENC; // Enable conversion
  ADC12CTL0 |= ADC12SC; // Start conversion
  __delay_cycles(25); // Delay for a bit to let the ADC12 warm up or whatever
  while (ADC12CTL1 & BUSY); // Poll the busy bit

  // Switches between each of the five input channels for our three sensors.
  // (Or 4, if the homebrew EDA is not being used.
  // In that event, [case EDA] needs to be disabled.
  if (A_flag == 1){
    switch(current_channel) {
      // 3/5/18 - Again, right now, the most important thing is that raw ADC values could be taken. There will
      // need to be computations here to turn the raw ADC into physical measurements.
      case Ax :
        accel_x = ADC12MEM0;
        snprintf(TXbuf+TXbufLen,50,"ACCX\t%d\r\n",accel_x);
        TXbufLen = (unsigned)strlen(TXbuf);
        UCA1TXBUF = TXbuf[TXbufPos];
        current_channel = Ay;
        break;

      case Ay :
        accel_y = ADC12MEM1;
        snprintf(TXbuf+TXbufLen,50,"ACCY\t%d\r\n",accel_y);
        TXbufLen = (unsigned)strlen(TXbuf);
        UCA1TXBUF = TXbuf[TXbufPos];
        current_channel = Az;
        break;

      case Az :
        accel_z = ADC12MEM2;
        snprintf(TXbuf+TXbufLen,50,"ACCZ\t%d\r\n",accel_z);
        TXbufLen = (unsigned)strlen(TXbuf);
        UCA1TXBUF = TXbuf[TXbufPos];
        current_channel = PPG;
        break;

      // 3/5/18 - PPG and EDA sensors will need to be added here. These cases are just here to show that
      // all five channels are working.
      case PPG :
        ppg_var = ADC12MEM3;
        snprintf(TXbuf+TXbufLen,50,"PPG\t%d\r\n",ppg_var);
        TXbufLen = (unsigned)strlen(TXbuf);
        UCA1TXBUF = TXbuf[TXbufPos];
        current_channel = EDA;
        break;

      case EDA :
        eda_var = ADC12MEM4;
        snprintf(TXbuf+TXbufLen,50,"EDA\t%d\r\n",eda_var);
        TXbufLen = (unsigned)strlen(TXbuf);
        UCA1TXBUF = TXbuf[TXbufPos];
        current_channel = Ax;
        A_flag = 0;
        break;

      default: current_channel = Ax; break;
    }
  }
}

//------------------------------------------------------------------------------
// ADC12 ISR
//------------------------------------------------------------------------------
void ADC12_ISR(void) __interrupt [ADC12_VECTOR] {
  switch (ADC12IV) {
  
  case ADC12IV_ADC12OVIFG :
    _NOP(); // We'll put something in here later
    break;
  
  case ADC12IV_ADC12TOVIFG :
    _NOP(); // We'll put something in here later
    break;

  default :
    _NOP(); // as above
    break;
  }
}
