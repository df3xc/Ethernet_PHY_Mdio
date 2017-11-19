
/**
 * \file  main.c
 *
 * \brief the main is the mains is the main ....
 */


#include "config.h"

#ifdef BAREMETAL
#include <stdio.h>
#include <string.h>
#include "soc_AM335x.h"
#include "beaglebone.h"
#include "gpio_v2.h"
#include "pin_mux.h"
#include "hw_types.h"
#include "delay.h"
#include "consoleUtils.h"
#include "convert.h"


#else

#include <stdio.h>
#include <string.h>
#include "am335x.h"
#include "libgpio.h"

#endif

#include "cmdline.h"
#include "tja1100.h"
#include "mdio.h"
#include "main.h"




/*****************************************************************************
**                INTERNAL MACRO DEFINITIONS
*****************************************************************************/



/*****************************************************************************
**                INTERNAL FUNCTION PROTOTYPES
*****************************************************************************/

void setupGpioPins();
void myWait(int ticks);
int help (int argc, char *argv[]);
int testCmdLine (int argc, char *argv[]);

int selectMdioPins       (int argc, char *argv[]);
int enableWriteSmi       (int argc, char *argv[]);
int readEthPhyRegister   (int argc, char *argv[]);
int writeEthPhyRegister  (int argc, char *argv[]);
int dumpEthPhyRegister   (int argc, char *argv[]);
int displayEthPhyRegister(int argc, char *argv[]);
int dispatchEthPhyRegister  (int argc, char *argv[]);
int setPhyAddress        (int argc, char *argv[]);
int probePhyAddress      (int argc, char *argv[]);
int setTestMode          (int argc, char *argv[]);
int mdioReadTest         (int argc, char *argv[]);
int mdioWriteTest        (int argc, char *argv[]);
int testInput            (int argc, char *argv[]);


/*****************************************************************************
**                VARIABLES
*****************************************************************************/

phy_reg_struct phy_regs[20] =

{
{0, "Basic control                  "},
{1, "Basic status                   "},
{2, "PHY IDENTIFIER 1               "},
{3, "PHY IDENTIFIER 2               "},
{4, "AUTO NEG ADVERTISEMENT         "},
{5, "AUTO NEG LINK PARTNER ABILITY  "},
{6, "AUTO NEG EXPANSION             "},
{17,"MODE CONTROL/STATUS            "},
{18,"Basic status                   "},
{23,"Communication status           "},
{999,""},
};

/*****************************************************************************
**                COMMAND LINE TABLE of COMMANDS
*****************************************************************************/


tCmdLineEntry g_sCmdTable[20] =
{
  {"help",        help,                   "provide help"},
  {"cmdline",     testCmdLine,            "test command line processing"},
  {"padr",        setPhyAddress,          "set PHY <address>"},
  {"swe",         enableWriteSmi,         "enable SWI write "},
  {"dump",        dumpEthPhyRegister,     "dump Ethernet PHY register 0..31"},
  {"disp",        displayEthPhyRegister,  "display Ethernet PHY register"},
  {"wp",          writeEthPhyRegister,    "write Ethernet PHY <register> <value>"},
  {"rp",          readEthPhyRegister,     "read Ethernet PHY <register>"},
  {"sta",         dispatchEthPhyRegister, "display Ethernet PHY status"},
  {"probe",       probePhyAddress ,       "search for PHYs at adress 0..32"},
  {"pins",        selectMdioPins,         "<0> local PHY, <1> external PHY"},
  {"tmode",       setTestMode,            "enable PHY test mode <0..6>"},
  {"rep",         mdioReadTest ,          "repeat read <register>"},
  {"wep",         mdioWriteTest ,         "repeat write <register> <value> "},
  {"tin",         testInput ,             "GPIO as input, USR1 Led as output"},
  {0,0,0}
};

unsigned int PHY_Address = 0;

/*****************************************************************************
**                INTERNAL FUNCTION DEFINITIONS
*****************************************************************************/


/***************************************************************//**
** \brief Low level test : set USR1 Led according to input pin state.
*******************************************************************/

int testInput(int argc, char *argv[])

{
mdioTestInput();

return(0);
}

/***************************************************************//**
* \brief Print help
*******************************************************************/


int help (int argc, char *argv[])
{

    tCmdLineEntry *pCmdEntry = &g_sCmdTable[0];

    printf("\n\r ----------------------- HELP ------------------------");
    printf("\n\r <...> are the command line parameters");
    printf("\n\r");

    while(pCmdEntry->pcCmd)
    {
        printf("\n\r %s \t\t : \t\t %s",pCmdEntry->pcCmd,pCmdEntry->pcHelp);
        pCmdEntry++;
    }
    printf("\n\r -----------------------------------------------------");
    return(0);

}

/***************************************************************//**
 \brief select MDIO pins
*******************************************************************/
int selectMdioPins  (int argc, char *argv[])

{
    int selection = 0;

    printf("\n\r select pins for MDIO/MDC interface ");

    selection = atoi(argv[1]);

    mdioSelectPins(selection);

    mdioInit();

    return (0);

}


/***************************************************************//**
 *  \brief set Address of the PHY
 *
 * \param
 *
 * \return Returns
 * \b
 *******************************************************************/

int setPhyAddress (int argc, char *argv[])
{
    unsigned int p;

    p = atoi(argv[1]);

    PHY_Address = p;

    printf ("\n\r set PHY address %d ",PHY_Address);

    return(0);

}

/****************************************************************//**
 * \brief List PHY Register Names
 * \param
 *
 * \return Returns
 * \b
 * \b comment
********************************************************************/

void listRegisterNames()
{
int k = 0;

while(phy_regs[k].addr!=999)
    {
        printf ("\n\r addr %d  : name %s",phy_regs[k].addr,phy_regs[k].name);
        k++;
    }
printf ("\n\r");
}

/****************************************************************//**
* Just test command line processing
********************************************************************/

int testCmdLine (int argc, char *argv[])
{
    int k = 0;
    unsigned int a = 0;

    printf("\n\r ----------- Test Command Line ------------");

    printf("\n\r Number of arguments %d",argc);

    for(k=0;k<argc;k++)
    {
        printf("\n\r argument %d : %s ",k,argv[k]);

        if (argv[k][1]=='x')
        {
        a = atoi(argv[k]);
        printf(" atoi result : %x ",a);
        }

    }

    printf("\n\r ------------------------------------------");
    return(0);
}


/***************************************************************//**
*  \brief mdio test : repeat reading PHY register
*******************************************************************/

int mdioReadTest (int argc, char *argv[])
{
    int val;
    int mdioReg = 0;

    printf("\n\r MDIO test");

    if(argc<2) {
        printf("\n\r ERROR : missing arguments ");
    return(-1);
    }

    mdioReg = atoi(argv[1]);

    while(1)
    {
        val = mdioReadRegister(PHY_Address,mdioReg);
        printf("\n\r read PHY Register %d = 0x%.4X \n\r",mdioReg,val);
        myWait(500);
    }

    return(0);
}


/***************************************************************//**
*  \brief mdio test : repeat write PHY register
*******************************************************************/

int mdioWriteTest (int argc, char *argv[])
{
    int val;
    int mdioReg = 0;

    printf("\n\r MDIO test");

    if(argc<3) {
        printf("\n\r ERROR : missing arguments ");
    return(-1);
    }


    mdioReg = atoi(argv[1]);

    sscanf(argv[2],"%X",&val);

    while(1)
    {
        mdioWriteRegister(PHY_Address,mdioReg,val);
        printf("\n\r write PHY Register %d = 0x%.4X \n\r",mdioReg,val);
        myWait(500);
    }

    return(0);
}

/***************************************************************//**
*  \brief enable SMI write access
*******************************************************************/

int enableWriteSmi (int argc, char *argv[])

{
    unsigned int val = 0;
    unsigned int mdioReg = 0;

    printf("\n\r enable SMI write register");

    val = mdioReadRegister(PHY_Address,MCS);
    val = val | 0x4;
    mdioWriteRegister(PHY_Address,MCS,val);

    return(0);

}


/***************************************************************//**
*  \brief write Ethernet PHY register
*******************************************************************/


int writeEthPhyRegister  (int argc, char *argv[])

{
    unsigned int val = 0;
    unsigned int mdioReg = 0;

    if(argc<3) {
        printf("\n\r ERROR : missing arguments ");
    return(-1);
    }

	mdioReg = atoi(argv[1]);

#ifdef BAREMETAL

    val=atoi(argv[2]);

#else

    sscanf(argv[2],"%X",&val);

#endif // BAREMETAL


    printf("\n\r write PHY Register %d = 0x%X",mdioReg,val);
    mdioWriteRegister(PHY_Address,mdioReg,val);

    val = mdioReadRegister(PHY_Address,mdioReg);
    printf("\n\r read PHY Register %d = 0x%X \n\r",mdioReg,val);

    return(0);
}

/***************************************************************//**
*  \brief read Ethernet PHY register
*******************************************************************/

int readEthPhyRegister  (int argc, char *argv[])

{
    unsigned int val = 0;
    unsigned int mdioReg = 0;

    if(argc<2) {
        printf("\n\r ERROR : missing arguments ");
    return(-1);
    }

    mdioReg= atoi(argv[1]);

    val = mdioReadRegister(PHY_Address,mdioReg);
    printf("\n\r read PHY Register %d = 0x%X",mdioReg,val);

    return(0);
}


/***************************************************************//**
*  \brief read Ethernet PHY register
*******************************************************************/


int dumpEthPhyRegister  (int argc, char *argv[])

{
    unsigned int  k = 0;
    unsigned int mdioReg;



    for (k=0;k<32;k++)
        {
        mdioReg = mdioReadRegister(PHY_Address,k);
        printf("\n\r PHY Reg %d = 0x%x",k,mdioReg);
        }

    return(0);
}

/***************************************************************//**
*
*  \brief Display PHY register
*
*******************************************************************/
int displayEthPhyRegister(int argc, char *argv[])
{

int k = 0;
int val = 0;
volatile unsigned int mdioReg;


val = mdioReadRegister(PHY_Address,PHY_ID2);
printf("\n\r 0x%x val \n\r",val);

if ((val==0x0000) || (val == 0xFFFF))
{
 printf("\n\r ------------- ERROR ------------ ");
 printf("\n\r Cannot read PHY ID register      ");
 printf("\n\r Check cabling and power supply   ");
 printf("\n\r -------------------------------- ");
 printf("\n\r");
 return(-1);
}


while(phy_regs[k].addr!=999)
    {
        mdioReg = mdioReadRegister(PHY_Address,phy_regs[k].addr);
        printf("\n\r PHY Reg %d  \t%s = 0x%x",phy_regs[k].addr,phy_regs[k].name,mdioReg);
        k++;
    }
printf ("\n\r");

return(0);

}

/***************************************************************//**
 *  \brief Ethernet Status Report
********************************************************************/

int dispatchEthPhyRegister  (int argc, char *argv[])

{
    unsigned int mdioReg = 0;
    unsigned int val = 0;

    mdioReg = 0;
    val = mdioReadRegister(PHY_Address,mdioReg);
    printf("\n\r read PHY Basic Control Register %d = 0x%X",mdioReg,val);


    mdioReg = 1;
    val = mdioReadRegister(PHY_Address,mdioReg);
    printf("\n\r read PHY Basic Status Register %d = 0x%X",mdioReg,val);

    if ((val&0x0004)==0x0004) {
        printf("\n\r ## Link is Up ");
    }
    else {
        printf("\n\r ## Link is Down ");
    }


    mdioReg = 17;
    val = mdioReadRegister(PHY_Address,mdioReg);
    printf("\n\r read PHY Extended Control Register %d = 0x%X",mdioReg,val);


    if ((val&0x8000)==0x8000) {
        printf("\n\r ## LINK Control enabled ");
    }
    else {
        printf("\n\r ## LINK Control disabled ");
    }

    mdioReg = 18;
    val = mdioReadRegister(PHY_Address,mdioReg);
    printf("\n\r read PHY Configuration Register #1  %d = 0x%X",mdioReg,val);

    if ((val&0x8000)==0x8000) {
        printf("\n\r ## PHY is Master ");
    }
    else {
        printf("\n\r ## PHY is Slave ");
    }

    mdioReg = 23;
    val = mdioReadRegister(PHY_Address,mdioReg);
    printf("\n\r read PHY Communication Status Register #1  %d = 0x%X",mdioReg,val);

    if ((val&0x8000)==0x8000) {
        printf("\n\r ## Link is OK ");
    }
    else {
        printf("\n\r ## Link failure ");
    }

     return(0);

}


/***************************************************************//**
*  \brief probe for PHY
*******************************************************************/

int probePhyAddress (int argc, char *argv[])
{
    unsigned int  k = 0;
    unsigned int mdioReg;



    for (k=0;k<32;k++)
        {
            mdioReg = mdioReadRegister(k,0);

            if ((mdioReg==0x0000) || (mdioReg == 0xFFFF))
            {

            }
            else
            {
            printf("\n\r PHY FOUND at PHY Address %.2d Reg 0x%.4X",k,mdioReg );
            }
        }

    return(0);

}

/***************************************************************//**
 *  \brief Set PHY test mode
********************************************************************/


int setTestMode  (int argc, char *argv[])
{
    unsigned int val = 0;
    unsigned int mode;

    if(argc<2) {
        printf("\n\r ERROR : missing arguments ");
    return(-1);
    }

    mode = atoi(argv[1]);


    printf("\n\r Disable Auto OP 15");

    val = mdioReadRegister(PHY_Address,18);
    val = val & ~(0x4000);
    mdioWriteRegister(PHY_Address,18,val);

    printf("\n\r Disable Link Control Bit 15");

    val = mdioReadRegister(PHY_Address,17);
    val = val & ~(0x8000);
    mdioWriteRegister(PHY_Address,17,val);

    printf("\n\r enable test mode %d (Bit 6..8) ", mode);

    val = mdioReadRegister(PHY_Address,17);
    val = val & ~(3<<6);
    val = val | (mode<<6);
    mdioWriteRegister(PHY_Address,17,val);

    return(0);
}

/***************************************************************//**
*  \brief The main function. Application starts here.
*******************************************************************/
int main(int argc, char *argv[])


{

    int count = 0;
    char cmdline[100];
    int k;
    char ch;

#ifdef BAREMETAL

    HWREG(WDOG) = 0xAAAA;
    myWait(1000);
    HWREG(WDOG) = 0x5555;

    ConsoleUtilsInit();
    setupGpioPins();
    printf("\n\r MDIO bare metal on BeagleBone Black 2017");

#else

    printf("\n\r MDIO Linux App on BeagleBone Black 2017");
    mdioSelectPins(1);
    setupGpioPins();
    mdioInit();

#endif




    while(1)
    {

#ifdef BAREMETAL

        /* Driving a logic HIGH on the GPIO pin. */
        GPIOPinWrite(GPIO1_MODULE, USR_LED0, GPIO_PIN_HIGH);
        GPIOPinWrite(GPIO1_MODULE, USR_LED2, GPIO_PIN_HIGH);

        GPIOPinWrite(GPIO2_MODULE, G2_P8_8,  GPIO_PIN_HIGH);
        GPIOPinWrite(GPIO0_MODULE, G0_P8_13, GPIO_PIN_HIGH);
        GPIOPinWrite(GPIO0_MODULE, G0_P8_14, GPIO_PIN_HIGH);

        printf("\n\r -> ");

        count++;

        ConsoleUtilsGets(cmdline,100);

        /* Driving a logic LOW on the GPIO pin. */
        GPIOPinWrite(GPIO1_MODULE, USR_LED0, GPIO_PIN_LOW);
        GPIOPinWrite(GPIO1_MODULE, USR_LED2, GPIO_PIN_LOW);

        GPIOPinWrite(GPIO2_MODULE, G2_P8_8,  GPIO_PIN_LOW);
        GPIOPinWrite(GPIO0_MODULE, G0_P8_13, GPIO_PIN_LOW);
        GPIOPinWrite(GPIO0_MODULE, G0_P8_14, GPIO_PIN_LOW);

#else

        k=0;
        printf("\n\r->");

        while((ch = getchar()) != '\n')
        {
            cmdline[k] = ch;
            k++;
        }
       cmdline[k]='\0';

       printf("CMD Line: <%s>",cmdline);

#endif

        CmdLineProcess(cmdline);

    }

}

/***************************************************************//**
*******************************************************************/
void setupGpioPins()

{
    printf("\n\r setup GPIO pins ... ");

#ifdef BAREMETAL

    /* Enabling functional clocks for GPIO1 instance. */
    GPIO1ModuleClkConfig();

    /* define pins as GPIO */

    GpioPinMuxSetup(GPIO_1_21, 7); // use pin as GPIO1_21 : Led USR0
    GpioPinMuxSetup(GPIO_1_22, 7); // use pin as GPIO1_22 : Led USR1
    GpioPinMuxSetup(GPIO_1_23, 7); // use pin as GPIO1_23 : Led USR2
    GpioPinMuxSetup(GPIO_1_24, 7); // use pin as GPIO1_24 : Led USR3

    /* Expansion header P8 */

    GPIO1PinMuxSetup(G1_P8_5);  // P8 Header Pin 5 as GPIO
    GPIO1PinMuxSetup(G1_P8_6);  // P8 Header Pin 6 as GPIO
    GPIO2PinMuxSetup(G2_P8_7);  // P8 Header Pin 7 as GPIO
    GPIO2PinMuxSetup(G2_P8_8);  // P8 Header Pin 8 as GPIO
    GPIO2PinMuxSetup(G2_P8_9);  // P8 Header Pin 9 as GPIO
    GPIO2PinMuxSetup(G2_P8_10); // P8 Header Pin 10 as GPIO
    GPIO1PinMuxSetup(G1_P8_11); // P8 Header Pin 11 as GPIO
    GPIO1PinMuxSetup(G1_P8_12); // P8 Header Pin 12 as GPIO
    GPIO0PinMuxSetup(G0_P8_13); // P8 Header Pin 13 as GPIO
    GPIO0PinMuxSetup(G0_P8_14); // P8 Header Pin 14 as GPIO

    /* MDIO interface to on-board Ethernet PHY */

    GPIO0PinMuxSetup(0); // MDIO
    GPIO0PinMuxSetup(1); // MDC

//    /* Enabling the GPIO module. */
//    GPIOModuleEnable(GPIO1_MODULE);
//    GPIOModuleEnable(GPIO0_MODULE);
//
//    /* Resetting the GPIO module. */
//    GPIOModuleReset(GPIO1_MODULE);
//    GPIOModuleReset(GPIO0_MODULE);

    /* Setting the USR LED as an output pins */
    GPIODirModeSet(GPIO1_MODULE,USR_LED0,GPIO_DIR_OUTPUT);
    GPIODirModeSet(GPIO1_MODULE,USR_LED1,GPIO_DIR_OUTPUT);
    GPIODirModeSet(GPIO1_MODULE,USR_LED2,GPIO_DIR_OUTPUT);
    GPIODirModeSet(GPIO1_MODULE,USR_LED3,GPIO_DIR_OUTPUT);

    printf("\n\r Set GPIO pins as OUTPUT/INPUT ... ");

    GPIODirModeSet(GPIO2_MODULE, G2_P8_8, GPIO_DIR_OUTPUT);
    GPIODirModeSet(GPIO2_MODULE, G2_P8_7, GPIO_DIR_INPUT);

    GPIODirModeSet(GPIO0_MODULE, G0_P8_13, GPIO_DIR_OUTPUT);
    GPIODirModeSet(GPIO0_MODULE, G0_P8_14, GPIO_DIR_OUTPUT);

#else

    /* define pins as GPIO */

    digitalOutput(USR0);
    digitalOutput(USR1);
    digitalOutput(USR2);
    digitalOutput(USR3);
    digitalOutput(P8_19);
#endif

    printf(" DONE ");

}

/*
 * wait about 0.5 milliseconds (not yet calibrated)
 */

void myWait(int ticks)
{
  volatile int a;

  ticks=ticks*100;
  while(ticks>0)
      {
       ticks--;
       a = a + 1;
      }
  }






/******************************* End of file *********************************/
