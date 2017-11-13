
#include <stdio.h>
#include <string.h>
#include "gpio.h"
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
int readEthPhyRegister  (int argc, char *argv[]);
int writeEthPhyRegister  (int argc, char *argv[]);
int dumpEthPhyRegister  (int argc, char *argv[]);

/*****************************************************************************
**                INTERNAL FUNCTION DEFINITIONS
*****************************************************************************/

/*
** Print help
*/


int help (int argc, char *argv[])
{
    printf("\n\r ----------------------- HELP ------------------------");
    printf("\n\r -----------------------------------------------------");
    return(0);
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


/*
** write Ethernet PHY register
*/


int writeEthPhyRegister  (int argc, char *argv[])

{
    unsigned int val = 0;
    unsigned int mdioReg = 0;

    if(argc<2) return(1);

	mdioReg = atoi(argv[1]);
    val = atoi(argv[2]);

    printf("\n\r write PHY Register %d = 0x%X",mdioReg,val);
	mdioWritePreamble();
    mdioWriteRegister(0,mdioReg,val);

    val = mdioReadRegister(0,mdioReg);
    printf("\n\r read PHY Register %d = 0x%X",mdioReg,val);

    return(0);
}

int readEthPhyRegister  (int argc, char *argv[])

{
    unsigned int val = 0;
    unsigned int mdioReg = 0;

    if(argc<2) return(1);

    mdioReg= atoi(argv[1]);

    val = mdioReadRegister(0,mdioReg);
    printf("\n\r read PHY Register %d = 0x%.4X",mdioReg,val);

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



/*
** The main function. Application starts here.
*/
int main(int argc, char *argv[])

{

    int count = 0;

    printf("\n\r MDIO BeagleBone Black 2017");

	for (count = 0; count < argc; count++)
	{
		printf("\n\r ARG %d : %s", count, argv[count]);
	}

    setupGpioPins();
    mdioInit();

	if (argc > 0)
	{

		if (strcmp("dump", argv[1]) == 0) {
			dumpEthPhyRegister(argc, argv);
		}

		if (strcmp("rst", argv[1]) == 0) {
			mdioWriteRegister(0, 0, 0x8000);
		}

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
