#ifndef __MDIO__
#define __MDIO__

//#define debug(format, args...) printf (format, ## args)
//#define debug(format, args...) ConsoleUtilsPrintf (format, ## args)
#define debug(format, args...)


#define uint unsigned int

#define PULSE 50 // clock pulse duration

/* Beagleboard

    MDIO GPIO 0.0
    MDC  GPIO 0.1
*/


void mdioInit();

void mdioSelectPins(int selection);

void mdioWriteRegister(uint phyAddr, uint regAddr, uint value );

uint mdioReadRegister(uint phyAddr, uint regAddr );

void mdioWritePreamble();

#endif // __MDIO__


