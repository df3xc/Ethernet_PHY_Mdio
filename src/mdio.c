#include "config.h"

#ifdef BAREMETAL

#include "soc_AM335x.h"
#include "beaglebone.h"
#include "gpio_v2.h"
#include "pin_mux.h"
#include "hw_types.h"
#include "delay.h"
#include "consoleUtils.h"

#else

#include <stdio.h>
#include <unistd.h>
#include "main.h"
#include "libgpio.h"
#include "mdio.h"

#endif

#include "main.h"
#include "mdio.h"

#ifdef BAREMETAL

#else
PIN MDC_Pin;
PIN MDIO_Pin;
#endif


/***************************************************************//**
 *  \brief select GPIOs used as MDIO and MDC
 *
 * \param
 * 0 : access on-board PHY via MDC and MDIO pins
 * 1 : access external PHY via Port 8 pins
*******************************************************************/

void mdioSelectPins(int selection)

{
if (selection==0)
    {
        printf("\n\r access on-board PHY via MDC and MDIO pins");

#ifdef BAREMETAL

#else

        MDC_Pin = MDC;
        MDIO_Pin = MDIO;
#endif

    }
    else
    {
        printf("\n\r access external PHY via Port 8 pins");
#ifdef BAREMETAL

#else
        MDC_Pin = P8_14;
        MDIO_Pin = P8_13;

        printf("\n\r MDC  : %s",MDC_Pin.name);
        printf("\n\r MDIO : %s",MDIO_Pin.name);
#endif
    }
}

/***************************************************************//**
 \brief init MDIO and MDC as output
*******************************************************************/

void mdioInit()
{
  debug ("\n\r MDIO INIT start ");

#ifdef BAREMETAL
  /* MDIO interface to on-board Ethernet PHY */

    GPIO0PinMuxSetup(PinMDIO); // MDIO
    GPIO0PinMuxSetup(PinMDC);  // MDC

    GPIODirModeSet(GPIO0_MODULE,PinMDIO,GPIO_DIR_OUTPUT);
    GPIODirModeSet(GPIO0_MODULE,PinMDC,GPIO_DIR_OUTPUT);
#else
    digitalOutput(MDIO_Pin);
    digitalOutput(MDC_Pin);

#endif

}

/***************************************************************//**
 \brief set MDIO as input
*******************************************************************/

void mdioInput()

{
#ifdef BAREMETAL
    GPIODirModeSet(GPIO0_MODULE,PinMDIO,GPIO_DIR_INPUT);
#else
    digitalInput(MDIO_Pin);
#endif
    //debug(" MDIO as input ");

}

/***************************************************************//**
 \brief set MDIO data pin 1 or 0
*******************************************************************/

void setMdioPin(int value)
{

#ifdef BAREMETAL

if (value==0)
    {
    //debug("\n\r MDIO 0 ");
    GPIOPinWrite(GPIO0_MODULE,PinMDIO,GPIO_PIN_LOW);
    }
else
    {
    //debug("\n\r MDIO 1 ");
    GPIOPinWrite(GPIO0_MODULE,PinMDIO,GPIO_PIN_HIGH);
    }
#else

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

#endif
}

/***************************************************************//**
 \brief read MDIO data pin
*******************************************************************/

uint getMdioPin()
{
#ifdef BAREMETAL
     return (GPIOPinRead(GPIO0_MODULE,PinMDIO));
#else
     return (digitalRead(MDIO_Pin));
#endif
}

/***************************************************************//**
 \brief set MDC Clock pin 1 or 0
*******************************************************************/

void setMdcPin(int value)
{
#ifdef BAREMETAL
if (value==0)
    {
    //debug(" MDC 0 ");
    GPIOPinWrite(GPIO0_MODULE,PinMDC,GPIO_PIN_LOW);
    }
else
    {
    //debug(" MDC 1 ");
    GPIOPinWrite(GPIO0_MODULE,PinMDC,GPIO_PIN_HIGH);
    }
#else
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

#endif
}

/***************************************************************//**
 \brief MDC clock cycle

 clock set to 0,1,1,0 is a sequence

*******************************************************************/

void mdcClock()

{
 setMdcPin(0);
 myWait(PULSE);
 setMdcPin(1);
 myWait(2*PULSE);
 setMdcPin(0);
 myWait(PULSE);
}

/***************************************************************//**
 \brief write data using GPIO pins MDIO and MDC
*******************************************************************/

void mdioWrite(int data)
{
int i;
int gpio;

mdioInit();

mdioWritePreamble();

for (i=0;i<32;i++)
    {
     gpio=data & 0x80000000;
     setMdioPin(gpio);
     mdcClock();
     data = data << 1;
    }

}

/***************************************************************//**
 \brief write data using GPIO pins MDIO and MDC
*******************************************************************/

int mdioRead(int data)
{
int i;
int gpio = 0;
int phyRegister = 0;

mdioInit();

mdioWritePreamble();

//ConsoleUtilsPrintf("\n\r Mdio read ... ");

for (i=0;i<14;i++)
    {
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

     if (getMdioPin())
         {
            //debug("\n\r MDIO is high");
            phyRegister = phyRegister | 1;
         }
     else
         {
            //debug("\n\r MDIO is low");
         }
     phyRegister = phyRegister << 1;
    }

 phyRegister = phyRegister >> 1;

 mdioInit();
 setMdioPin(1);
 mdcClock();
 mdcClock();
 mdcClock();




//ConsoleUtilsPrintf(" result: 0x%04X",phyRegister );

return(phyRegister);

}


/***************************************************************//**
 \brief write MDIO preamble (32 time MDIO = 1)
*******************************************************************/


void mdioWritePreamble()

{
    int i=0;

    mdioInit();

    for (i=0;i<32;i++)
        {
         setMdioPin(1);
         mdcClock();
        }
}


/***************************************************************//**
 \brief write PHY register

 - phyAddr : address of the PHY
 - regAddr : address of the register
 - value   : value written to register

 TJA1100 needs MDIO=1 in the first TJA clock cycle

*******************************************************************/

void mdioWriteRegister(uint phyAddr, uint regAddr, uint value )

{
    unsigned int mdio;
    unsigned int d;

    printf("\n\r mdioWriteRegister %d %d 0x%.4x", phyAddr, regAddr, value);

    mdio = 0x50020000; // start, write , first TA=1

    d =  (phyAddr<<23);
    mdio = mdio | d;

    d = (regAddr<<18);
    mdio = mdio | d;

    mdio = mdio | (value);

    mdioWrite(mdio);

}

/***************************************************************//**
  \brief read PHY register


 \param
 - phyAddr : address of the PHY
 - regAddr : address of the register


 \return  :  register value

*******************************************************************/

uint mdioReadRegister(uint phyAddr, uint regAddr )

{
    int mdio;
    int d;
    int value;

    mdio = 0x60020000; // start, read

    d =  (phyAddr<<23);
    mdio = mdio | d;

    d = (regAddr<<18);
    mdio = mdio | d;

    value = mdioRead(mdio);

    // Caution : setting width for a number does not work
    //           using starterware library
    //           0x%.4x must be set to 0x%x

    printf ("\n\r mdioReadRegister %d %d 0x%x", phyAddr, regAddr, value);

    return(value);
}

int mdioTestInput()

{
unsigned int gpio=0;
int k=0;


printf("\n\r mdioTestInput");

mdioInput();


#ifdef BAREMETAL

#else

    gpio = getMdioPin();

    printf("\n\r MDIO reading 0x%.4X",gpio);

    digitalWrite(USR1,gpio);

    for (k=0; k<5; k++)
    {
        digitalWrite(USR3,1);
        usleep(90000);

        digitalWrite(USR3,0);
        usleep(90000);
    }
#endif

    return(0);

}


