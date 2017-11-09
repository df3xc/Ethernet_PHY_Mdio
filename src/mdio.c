
#include <stdio.h>
#include <unistd.h>
#include "main.h"
#include "gpio.h"
#include "mdio.h"




PIN MDC_Pin = MDC;
PIN MDIO_Pin = MDIO;



void mdioSelectPins(int selection)

{
if (selection==0)
    {
        printf("\n\r access on-board PHY via MDC and MDIO pins");

        MDC_Pin = MDC;
        MDIO_Pin = MDIO;

    }
    else
    {
        printf("\n\r access external PHY via Port 8 pins");

        MDC_Pin = P8_14;  
        MDIO_Pin = P8_13;

        printf("\n\r MDC  : %s",MDC_Pin.name);
        printf("\n\r MDIO : %s",MDIO_Pin.name);
    }
}


/***************************************************************//**
 \brief init MDIO and MDC as output
*******************************************************************/

void mdioInit()
{
  debug ("\n\r MDIO INIT start ");

  debug("\n\r MDC  : %s",MDC_Pin.name);
  debug("\n\r MDIO : %s",MDIO_Pin.name);

  digitalOutput(MDC_Pin );
  digitalOutput(MDIO_Pin);
//	digitalWrite(MDC, 0);
//	digitalWrite((MDIO),1);
  debug ("\n\r MDIO INIT done ");
}

/***************************************************************//**
 \brief set MDIO as input
*******************************************************************/

void mdioInput()
{
	digitalInput(MDIO_Pin);
    //debug(" MDIO as input ");
}

/***************************************************************//**
 \brief set MDIO data pin 1 or 0
*******************************************************************/

void setMdioPin(uint value)
{
if (value==0)
    {
    debug(" D0 ");
	digitalWrite(MDIO_Pin,0);
    }
else
    {
    debug(" D1 ");
	digitalWrite(MDIO_Pin, 1);
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
	  digitalWrite(MDC_Pin, 0);
    }
else
    {
    debug(" C1 ");
	  digitalWrite(MDC_Pin, 1);
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

void mdioWrite(uint data)
{
uint i;
uint gpio;

mdioInit();

mdioWritePreamble();

for (i=0;i<32;i++)
    {
     debug("\n\r %d ",i);
     gpio=data & 0x80000000;
     setMdioPin(gpio);
     mdcClock();
     data = data << 1;
    }
  mdcClock();
}

/***************************************************************//**
 \brief write data using GPIO pins MDIO and MDC
*******************************************************************/

uint mdioRead(uint data)
{
uint i;
uint gpio = 0;
uint phyRegister = 0;

mdioInit();

mdioWritePreamble();


for (i=0;i<14;i++)
    {
     debug("\n\r %.2d ",i);
     gpio=data & 0x80000000;
     setMdioPin(gpio);
     mdcClock();
     data = data << 1;
    }

// handover to PHY

 //debug("\n\r");
 mdioInput();
 //debug("\n\r");
 mdcClock();
 //debug("\n\r");

 // read MDIO pin while clock is low

 for (i=0;i<16;i++)
    {
     mdcClock();
      debug("\n\r %.2d ",i);

     if (digitalRead(MDIO))
         {
            debug(" MDI 1  ");
            phyRegister = phyRegister | 1;
         }
     else
         {
            debug(" MDI 0 ");
         }
     phyRegister = phyRegister << 1;
    }


 mdioInit();
 mdcClock();

phyRegister = phyRegister >> 1;

setMdioPin(1);

usleep(500);

//ConsoleUtilsPrintf(" result: 0x%04X",phyRegister );

return(phyRegister);

}


/***************************************************************//**
 \brief write MDIO preamble (32 time MDIO = 1)
*******************************************************************/


void mdioWritePreamble()

{
    uint i=0;

    debug ("\n\r PREEMBLE start");

    for (i=0;i<32;i++)
        {
         debug("\n\r %.2d ",i);
         setMdioPin(1);
         mdcClock();
        }

   debug ("\n\r PREEMBLE done ");
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

    mdio = 0x50000000; // start, write

    d =  (phyAddr<<23);
    mdio = mdio | d;

    d = (regAddr<<18);
    mdio = mdio | d;

    mdio = mdio | (value);

    mdioWrite(mdio);

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

    mdio = 0x60000000; // start, write

    d =  (phyAddr<<23);
    mdio = mdio | d;

    d = (regAddr<<18);
    mdio = mdio | d;

    value = mdioRead(mdio);

    return(value);
}



