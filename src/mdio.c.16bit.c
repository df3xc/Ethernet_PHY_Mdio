
#include "main.h"
#include "libgpio.h"
#include "mdio.h"


/***************************************************************//**
 \brief init MDIO and MDC as output
*******************************************************************/

void mdioInit()
{
	digitalOutput(MDC);
	digitalOutput(MDIO);
//	digitalWrite(MDC, 0);
	digitalWrite(MDIO,1);
}

/***************************************************************//**
 \brief set MDIO as input
*******************************************************************/

void mdioInput()

{
	digitalInput(MDIO);
    //debug(" MDIO as input ");
}

/***************************************************************//**
 \brief set MDIO data pin 1 or 0
*******************************************************************/

void setMdioPin(uint value)
{
if (value==0)
    {
    debug("\n\r D0 ");
	digitalWrite(MDIO,0);
    }
else
    {
    debug("\n\r D1 ");
	digitalWrite(MDIO, 1);
    }
}

/***************************************************************//**
 \brief set MDC Clock pin 1 or 0
*******************************************************************/

void setMdcPin(uint value)
{
if (value==0)
    {
    debug(" C0 ");
	  digitalWrite(MDC, 0);
    }
else
    {
    debug(" C1 ");
	  digitalWrite(MDC, 1);
    }
}

/***************************************************************//**
 \brief MDC clock cycle
*******************************************************************/

void mdcClock()

{
 setMdcPin(0);
 usleep(PULSE);
 setMdcPin(1);
 usleep(2*PULSE);
 setMdcPin(0);
 usleep(PULSE);
}

/***************************************************************//**
 \brief write data using GPIO pins MDIO and MDC
*******************************************************************/

void mdioWrite(uint data, uint value)
{
uint i;
uint gpio;

for (i = 0; i<14; i++)
{
	gpio = data & 0x8000;
	setMdioPin(gpio);
	mdcClock();
	data = data << 1;
}


for (i=0;i<16;i++)
    {
     gpio=value & 0x8000;
     setMdioPin(gpio);
     mdcClock();
     value = value << 1;
    }

}

/***************************************************************//**
 \brief write data using GPIO pins MDIO and MDC
*******************************************************************/

uint mdioRead(uint data)
{
uint i;
uint gpio = 0;
uint phyRegister = 0;


for (i=0;i<14;i++)
    {
     gpio=data & 0x8000;
     setMdioPin(gpio);
     mdcClock();
     data = data << 1;
    }

// handover to PHY
 mdioInput();
 mdcClock();
 //mdcClock();

 // read MDIO pin while clock is low

 for (i=0;i<16;i++)
    {
     mdcClock();

     if (digitalRead(MDIO)!=0)
         {
            debug("\n\r MD1");
            phyRegister = phyRegister | 1;
         }
     else
         {
            debug("\n\r MD0");
         }
      phyRegister = phyRegister << 1;


    }

 mdioInit();
 setMdioPin(1);
  mdcClock();
 mdcClock();



phyRegister = phyRegister >> 1;

//ConsoleUtilsPruintf(" result: 0x%04X",phyRegister );

return(phyRegister);

}


/***************************************************************//**
 \brief write MDIO preamble (32 time MDIO = 1)
*******************************************************************/


void mdioWritePreamble()

{
    uint i=0;

	  setMdioPin(1);

    for (i=0;i<32;i++)
        {
         mdcClock();
        }
}


/***************************************************************//**
 \brief write PHY register

 - phyAddr
 - regAddr
 - value   :  value written to register

*******************************************************************/
//

void mdioWriteRegister(uint phyAddr, uint regAddr, uint value )

{
    uint mdio;
    uint d;

    mdio = 0x5000; // start, write

    d =  (phyAddr<<7);
    mdio = mdio | d;

    d = (regAddr<<2);
    mdio = mdio | d;

    mdioWrite(mdio,value);

}

/***************************************************************//**
 \brief read PHY register

 - phyAddr
 - regAddr
 - return   :  value from register

*******************************************************************/
//

uint mdioReadRegister(uint phyAddr, uint regAddr )

{
    uint mdio;
    uint d;
    uint value;

    mdio = 0x6000; // start, write

    d =  (phyAddr<<7);
    mdio = mdio | d;

    d = (regAddr<<2);
    mdio = mdio | d;

	  //printf("\n\r data 0x%.4X", mdio);

    value = mdioRead(mdio);

    return(value);
}



