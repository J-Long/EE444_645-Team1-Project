#include "commands.h"

//------------------------------------------------------------------------------
// UART Commands
//------------------------------------------------------------------------------
void UARTparse ( char *data, unsigned int length)
{
  // parse a string for commands
  char check;
  if (strcmp ("START", data) == 0)
  {
    tempflag = 1;
  }
  else if (strcmp ("STOP", data) == 0)
  {
    tempflag = 0;
  }
  else{
    snprintf(TXbuf+TXbufLen,50,"Not a Command. Valid Commands are: \r\n");  // Tell the user the input was not a command. Correct.
    TXbufLen = (unsigned)strlen(TXbuf);
    snprintf(TXbuf+TXbufLen,50,"START \r\n");
    TXbufLen = (unsigned)strlen(TXbuf);
    snprintf(TXbuf+TXbufLen,50,"STOP \r\n");
    TXbufLen = (unsigned)strlen(TXbuf);
    UCA1TXBUF = TXbuf[TXbufPos];
  }
}
