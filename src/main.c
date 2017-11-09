
#include <stdio.h>
#include <string.h>
#include "gpio.h"
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
int help (int argc, char *argv[]);
int testCmdLine (int argc, char *argv[]);

int selectMdioPins  (int argc, char *argv[]);

int readEthPhyRegister  (int argc, char *argv[]);
int writeEthPhyRegister  (int argc, char *argv[]);
int dumpEthPhyRegister  (int argc, char *argv[]);
int displayEthPhyRegister(int argc, char *argv[]);
int dispatchEthPhyRegister  (int argc, char *argv[]);
int probePhyAddress (int argc, char *argv[]);
int setTestMode  (int argc, char *argv[]);
int mdioTest (int argc, char *argv[]);
int testInput(int argc, char *argv[]);


/*****************************************************************************
**                VARIABLES
*****************************************************************************/

phy_reg_struct phy_regs[20] =

{
{0, "Basic control                  "},
{1, "Basic status                   "},
{2, "PHY IDENTIFIER 1               "},
{3, "PHY IDENTIFIER 1               "},
{4, "AUTO NEG ADVERTISEMENT         "},
{5, "AUTO NEG LINK PARTNER ABILITY  "},
{6, "AUTO NEG EXPANSION             "},
{17,"MODE CONTROL/STATUS            "},
{18,"Basic status register          "},
{999,""},
};

tCmdLineEntry globalCmdTable[] =
{
  {"help",        help,                   "provide help"},
  {"testCmdLine", testCmdLine,            "test command line processing"},
  {"dp",          dumpEthPhyRegister,     "dump Ethernet PHY register 0..31"},
  {"wp",          writeEthPhyRegister,    "write Ethernet PHY register , value"},
  {"rp",          readEthPhyRegister,     "read Ethernet PHY register"},
  {"sta",         dispatchEthPhyRegister, "display Ethernet PHY status"},
  {"pins",        selectMdioPins,         "1: local PHY, 0: external PHY"},
  {"tmode",       setTestMode,            "enable PHY test mode 0..6"},
  {"rep",         mdioTest ,              "repeat read register 0"},
  {"ti",          testInput ,             "P8_14 as input, USR as output"},
  {0,0,0}


};

unsigned int PHY_Address = 0;

/*****************************************************************************
**                INTERNAL FUNCTION DEFINITIONS
*****************************************************************************/

int testInput(int argc, char *argv[])

{
while(1)
  {
    digitalInput(P8_13);
    digitalWrite(USR1,digitalRead(P8_13));
    
    digitalWrite(USR3,1);
    usleep(50000);
    
    digitalWrite(USR3,0);
    usleep(50000);    
    
  }

}

/*
** Print help
*/


int help (int argc, char *argv[])
{

    tCmdLineEntry *pCmdEntry = &globalCmdTable[0];

    printf("\n\r ----------------------- HELP ------------------------");

    while(pCmdEntry->pcCmd)
    {
        printf("\n\r %s \t\t : \t\t %s",pCmdEntry->pcCmd,pCmdEntry->pcHelp);
        pCmdEntry++;
    }
    printf("\n\r -----------------------------------------------------");
    return(0);

}


int selectMdioPins  (int argc, char *argv[])

{
    int selection = 0;

    printf("\n\r select pins for MDIO/MDC interface ");

    selection = atoi(argv[1]);

    mdioSelectPins(selection);

}

int setPhyAddress (int argc, char *argv[])
{
    unsigned int p;

    p = atoi(argv[1]);

    PHY_Address = p;

    printf ("\n\r set PHY address %d ",PHY_Address);

    return(0);

}

/*
** List PHY Register Names
*/

void listRegisterNames()

{
int k = 0;

while(phy_regs[k].id!=999)
    {
        printf ("\n\r id %d  : name %s",phy_regs[k].id,phy_regs[k].name);
        k++;
    }
printf ("\n\r");
}

/*
** Just test command line processing
*/

int testCmdLine (int argc, char *argv[])
{
    int k = 0;
    int a = 0;

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

int mdioTest (int argc, char *argv[])
{
    int val;
    int mdioReg = 0;

    while(1)
    {
    val = mdioReadRegister(PHY_Address,mdioReg);
    printf("\n\r read PHY Register %d = 0x%.4X \n\r",mdioReg,val);
    usleep(15000);
    }

}


/*
** write Ethernet PHY register
*/

int writeEthPhyRegister  (int argc, char *argv[])

{
    unsigned int val = 0;
    unsigned int mdioReg = 0;

    if(argc<2) return(1);

	mdioReg = atoi(argv[1]);

    sscanf(argv[2],"%X",&val);


    printf("\n\r write PHY Register %d = 0x%X",mdioReg,val);
	mdioWritePreamble();
    mdioWriteRegister(PHY_Address,mdioReg,val);

    val = mdioReadRegister(PHY_Address,mdioReg);
    printf("\n\r read PHY Register %d = 0x%X \n\r",mdioReg,val);

    return(0);
}

/*
** read Ethernet PHY register
*/

int readEthPhyRegister  (int argc, char *argv[])

{
    unsigned int val = 0;
    unsigned int mdioReg = 0;

    if(argc<2) return(1);

    mdioReg= atoi(argv[1]);

    val = mdioReadRegister(PHY_Address,mdioReg);
    printf("\n\r read PHY Register %d = 0x%.4X \n\r",mdioReg,val);

    return(0);
}


/*
** read Ethernet PHY register
*/

int dumpEthPhyRegister  (int argc, char *argv[])

{
    unsigned int  k = 0;
    unsigned int mdioReg;

    digitalWrite(P8_19,1);


    for (k=0;k<32;k++)
        {
        mdioReg = mdioReadRegister(0,k);
        printf("\n\r PHY Reg %d = 0x%.4X",k,mdioReg);
        }

    digitalWrite(P8_19,0);

    return(0);

}

int displayEthPhyRegister(int argc, char *argv[])
{

int k = 0;
int val = 0;
unsigned int mdioReg;


        val = mdioReadRegister(0,PHY_ID2);
        printf("\n\r 0x%.4X val \n\r",val);

        if ((val==0x0000) || (val == 0xFFFF))
        {
         printf("\n\r ------------- ERROR ------------ ");
         printf("\n\r Cannot read PHY ID register      ");
         printf("\n\r Check cabling and power supply   ");
         printf("\n\r -------------------------------- ");
         printf("\n\r");

         return(-1);
        }


while(phy_regs[k].id!=999)
    {
        mdioReg = mdioReadRegister(0,k);
        printf("\n\r PHY Reg %d  \t%s = 0x%.4X",phy_regs[k].id,phy_regs[k].name,mdioReg);
        k++;
    }
printf ("\n\r");

}

/******************************************************************************
 * Ethernet Status Report
******************************************************************************/

int dispatchEthPhyRegister  (int argc, char *argv[])

{
    unsigned int mdioReg = 0;
    unsigned int val = 0;

    mdioReg = 1;
    val = mdioReadRegister(0,mdioReg);

    printf("\n\r read PHY Basic Status Register %d = 0x%X",mdioReg,val);

    if (val%0x0004) {
        printf("\n\r Link Up ");
    }
    else {
        printf("\n\r Link Down ");
    }


    mdioReg = 17;
    val = mdioReadRegister(0,mdioReg);

    printf("\n\r read PHY Extended Control Register %d = 0x%X",mdioReg,val);


    mdioReg = 18;
    val = mdioReadRegister(0,mdioReg);

    printf("\n\r read PHY Configuration Register #1  %d = 0x%X",mdioReg,val);

    if (val%0x8000) {
        printf("\n\r PHY is Master ");
    }
    else {
        printf("\n\r PHY is Slave ");
    }

     return(0);

}

int probePhyAddress (int argc, char *argv[])
{
    unsigned int  k = 0;
    unsigned int mdioReg;

    digitalWrite(P8_19,1);


    for (k=0;k<32;k++)
        {
        mdioReg = mdioReadRegister(k,0);
        printf("\n\r PHY Address %d Reg 0x%.4X",k,mdioReg);
        }

    digitalWrite(P8_19,0);

    return(0);


}

/******************************************************************************
 * Set PHY test mode
******************************************************************************/


int setTestMode  (int argc, char *argv[])
{
    unsigned int val = 0;
    unsigned int mode;

    mode = atoi(argv[2]);


    printf("\n\r Disable Auto OP 15");

    val = mdioReadRegister(0,18);
    val = val & ~(0x4000);
    mdioWriteRegister(0,18,val);

    printf("\n\r Disable Link Control Bit 15");

    val = mdioReadRegister(0,17);
    val = val & ~(0x8000);
    mdioWriteRegister(0,17,val);

    printf("\n\r enable test mode %d (Bit 6..8) ", mode);

    val = mdioReadRegister(0,17);
    val = val & ~(3<<6);
    val = val | (mode<<6);
    mdioWriteRegister(0,17,val);

    return(0);
}


/*
** The main function. Application starts here.
*/
int main(int argc, char *argv[])

{

    int count = 0;
    char cmdline[100];
    int k;
    char ch;

    printf("\n\r MDIO BeagleBone Black 2017");

	for (count = 0; count < argc; count++)
	{
		printf("\n\r ARG %d : %s", count, argv[count]);
	}

	mdioSelectPins(1);
    setupGpioPins();
    mdioInit();


    while(1)
    {
        k=0;
        printf("\n\r->");
        
        while((ch = getchar()) != '\n')
        {
            cmdline[k] = ch;
            k++;
        }
       cmdline[k]='\0';
        
        //fgets(cmdline,20,stdin);
        printf("CMD Line: <%s>",cmdline);
        CmdLineProcess(cmdline);


    }

}



void setupGpioPins()

{
    printf("\n\r setup GPIO pins ... ");

    /* define pins as GPIO */

 	digitalOutput(USR0);
	digitalOutput(USR1);
    digitalOutput(USR2);
	digitalOutput(USR3);
	digitalOutput(P8_19);

    printf(" DONE ");

}






/******************************* End of file *********************************/
