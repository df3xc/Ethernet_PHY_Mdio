#ifndef __MDIO__
#define __MDIO__

//#include "consoleUtils.h"

//#define debug(format, args...) printf (stderr, format, ## args)
//#define debug(format, args...) ConsoleUtilsPrintf (format, ## args)
#define debug(format, args...)

#define printf(format, args...) ConsoleUtilsPrintf (format, ## args)

#define uint unsigned int

#define PULSE 50 // clock pulse duration

#define PinMDC 0
#define PinMDIO 1


void mdioInit();

void setMdioPin(int value);

void setMdcPin(int value);

uint getMdioPin();

void mdcClock();

void mdioSelectPins(int selection);

void mdioWriteRegister(uint phyAddr, uint regAddr, uint value );

uint mdioReadRegister(uint phyAddr, uint regAddr );

void mdioWritePreamble();

int mdioTestInput();

#endif // __MDIO__


