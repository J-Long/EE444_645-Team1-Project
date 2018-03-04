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
void ADCsetup (void)
{
  // Initialize the shared reference module
  REFCTL0 |= REFMSTR + REFVSEL_0 + REFON + REFOUT;    // Enable internal 1.5V reference

  
  // Initialize ADC12_A
  ADC12CTL0 = ADC12SHT0_8 + ADC12ON;        // Set sample time 
  ADC12CTL1 = ADC12SHP;                     // Enable sample timer
  ADC12MCTL0 = ADC12SREF_1 + ADC12INCH_7;  // ADC input ch A10 => temp sense 
  
  __delay_cycles(100);                      // delay to allow Ref to settle
                                            // based on default DCO frequency.
                                            // See Datasheet for typical settle
                                            // time.
  ADC12CTL0 |= ADC12ENC;                    // Enable conversions

  P8DIR |= BIT5;                            // Set a pin to output for power
  P8OUT |= BIT5;
}

//------------------------------------------------------------------------------
// Timer Setup
//------------------------------------------------------------------------------
void TA1_Setup(void)
{
  TA1CCTL0 = CCIE;                          // CCR0 toggle, interrupt enabled
  TA1CTL = TASSEL_1 + MC_2 + TACLR;         // ACLK, contmode, clear TAR
  TA1CCR0 = 468;                          // 16384/32768 = 0.5 seconds
  P1DIR |= BIT1;                            // Setup an LED to flash with the timer
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

