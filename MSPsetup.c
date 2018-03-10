#include "MSPsetup.h"

unsigned char tempflag = 0;

//------------------------------------------------------------------------------
// clock Setup
//------------------------------------------------------------------------------
void clockSetup (void)
{
  SetVcoreUp(PMMCOREV_1);                     
  SetVcoreUp(PMMCOREV_2);                   // Set VCore up

  P11DIR = BIT0+BIT1+BIT2;                  // P11.1-2 to output direction
  P11SEL |= BIT0+BIT1+BIT2;                 // P11.1-2 to output ACLK,SMCLK,MCLK

  // Initialize LFXT1
  P7SEL |= 0x03;                            // Select XT1
  UCSCTL6 &= ~(XT1OFF);                     // XT1 On
  UCSCTL6 |= XCAP_3;                        // Internal load cap

  // Loop until XT1,XT2 & DCO fault flag is cleared
  do
  {
    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);
                                            // Clear XT2,XT1,DCO fault flags
    SFRIFG1 &= ~OFIFG;                      // Clear fault flags
    __delay_cycles(100);
  }while (SFRIFG1&OFIFG);                   // Test oscillator fault flag



  __bis_SR_register(SCG0);                  // Disable the FLL control loop while setting DCO
  UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
  UCSCTL1 = DCORSEL_6;                      // Select DCO range 25MHz operation
  UCSCTL2 = FLLD_1 + 518;                   // Set DCO Multiplier for 17MHz
                                            // (N + 1) * FLLRef = Fdco
                                            // (518 + 1) * 32768 = 17MHz
                                            // Set FLL Div = fDCOCLK/2
  __bic_SR_register(SCG0);                  // Enable the FLL control loop
  
  // 32 x 32 x 25 MHz / 32,768 Hz = 781250 = MCLK cycles for DCO to settle
  __delay_cycles(781250);
}

//------------------------------------------------------------------------------
// UART Setup
//------------------------------------------------------------------------------

void UARTsetup (void)
{
  P5SEL |= BIT6 + BIT7;                     // Set the TX/RX for USCI_A1
  UCA1CTL1 |= UCSWRST;                      // disable UART during setup
  UCA1CTL1 |= UCSSEL__SMCLK;                // Select SMCLK

  // Oversampling method
  // N = 17MHz / 115200 = 147.57
  // N/16 = 9.223
  // round(0.223 * 16) = 4
  UCA1BR0 = 9;                              // INT(N/16)
  UCA1MCTL |= UCBRS_0 + UCBRF_4 + UCOS16;   // UCBRF_x = round((N/16-INT(N/16))*16)
  UCA1CTL1 &= ~UCSWRST;                     // Re-enable the UART
  UCA1IE |= UCRXIE + UCTXIE;                         // Enable interrupts
}

//------------------------------------------------------------------------------
// ADC Setup
//------------------------------------------------------------------------------
void ADC12_Setup(void) {
  
  /* Initialize reference module, VREF = 1.5 V */
  REFCTL0 |= REFMSTR + REFVSEL_0 + REFON + REFOUT;

  /* Initialize ADC12 */
  // Set ADC12 to pulse mode. In addition, ADC12SC bit-driven, ~5 MHz MODCLK, SCSC sequence.
  ADC12CTL1 = ADC12SHP;
 
  // (1/5 MHz)*64 = 12.8 us samp time, accelerometer requires > 10 us
  ADC12CTL0 = ADC12SHT0_4;

  // Set up conversion storage registers to store the sensor samples
  // the ADC12 generates. The reference voltage is AVCC, ~3.0 V, same as the power for the
  // accelerometer.
  ADC12MCTL0 = ADC12SREF_0 + ADC12INCH_1; // MEM0 = A1 = Accel_X
  ADC12MCTL1 = ADC12SREF_0 + ADC12INCH_2; // MEM1 = A2 = Accel_Y
  ADC12MCTL2 = ADC12SREF_0 + ADC12INCH_3; // MEM2 = A3 = Accel_Z
  ADC12MCTL3 = ADC12SREF_0 + ADC12INCH_12; // MEM3 = A12 = PPG
  ADC12MCTL4 = ADC12SREF_0 + ADC12INCH_13; // MEM4 = A13 = EDA (if used)

  // Let REF Module Stabilize after being turned on. 1000 is test value we can change later.
  __delay_cycles(1000); 

  // Turn on ADC12, enable conversions, and enable mem overflow 
  // and conversion overflow interrupts
  ADC12CTL0 |= ADC12ON + ADC12OVIE + ADC12TOVIE + ADC12ENC;

  // Enable input from Ports 6.1 (A1, Accel_X), 6.2 (A2, Accel_Y), and 6.3 (A3, Accel_Z)
  // The MSP430F5438A datasheet says that these values don't matter if the pin is selected
  // as an ADC12 input channel but I don't believe the datasheet
  P6SEL |= BIT1+BIT2+BIT3;
  P6DIR &= ~(BIT1+BIT2+BIT3);

  /* This section below is used to turn on board power to external sensors. */

  // Turn on board power to accelerometer
  P6DIR = BIT0;
  P6OUT = BIT0;

  /* The section above is used to turn on board power to external sensors. */
}

//------------------------------------------------------------------------------
// Timer A1 Setup
//------------------------------------------------------------------------------
void TA1_Setup(void)
{
  TA1CCTL0 = CCIE;                          // CCR0 toggle, interrupt enabled
  TA1CTL = TASSEL_1 + MC_2 + TACLR;         // ACLK, contmode, clear TAR
  TA1CCR0 = 468;                          // 16384/32768 = 0.5 seconds
  P1DIR |= BIT1;                            // Setup an LED to flash with the timer
}

//------------------------------------------------------------------------------
// Timer B Setup
//------------------------------------------------------------------------------
void TB0_Setup(void) {
  TBCCTL0 = CCIE; // Enable interrupt from CCR0
  TBCTL = TBSSEL_1 + MC_1 + TBCLR; // ACLK, upmode, clear TBR

  TBCCR0 = 33; // Counts up 32 cycles + 1, 32.768 kHz / 33 ~ 1 kHz
               // or, interrupt every 1 ms
  
}

//------------------------------------------------------------------------------
// Set the core voltage
//------------------------------------------------------------------------------
void SetVcoreUp (unsigned int level)
{
  // Open PMM registers for write
  PMMCTL0_H = PMMPW_H;              
  // Set SVS/SVM high side new level
  SVSMHCTL = SVSHE + SVSHRVL0 * level + SVMHE + SVSMHRRL0 * level;
  // Set SVM low side to new level
  SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * level;
  // Wait till SVM is settled
  while ((PMMIFG & SVSMLDLYIFG) == 0);
  // Clear already set flags
  PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);
  // Set VCore to new level
  PMMCTL0_L = PMMCOREV0 * level;
  // Wait till new level reached
  if ((PMMIFG & SVMLIFG))
    while ((PMMIFG & SVMLVLRIFG) == 0);
  // Set SVS/SVM low side to new level
  SVSMLCTL = SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level;
  // Lock PMM registers for write access
  PMMCTL0_H = 0x00;
}

