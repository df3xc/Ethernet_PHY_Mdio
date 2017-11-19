/*
 * main.h
 *
 *  Created on: 31.10.2017
 *      Author: df3xc
 */

#ifndef _MAIN_
#define _MAIN_

 /*! \mainpage My Personal Index Page
 *
 * \section intro_sec Introduction
 *
 * This is the introduction.
 *
 * \section install_sec Installation
 *
 * \subsection step1 Step 1: Opening the box
 *
 * etc...
 */

#include "soc_AM335x.h"


#define GPIO0_MODULE           (SOC_GPIO_0_REGS)
#define GPIO1_MODULE           (SOC_GPIO_1_REGS)
#define GPIO2_MODULE           (SOC_GPIO_2_REGS)


#define USR_LED0 21
#define USR_LED1 22
#define USR_LED2 23
#define USR_LED3 24


/*
 * Gx : GPIO module number 0,1,2
 * Px : Port Connector 8,9
 * xx : Port Pin number
 */

#define G1_P8_5  2
#define G1_P8_6  3
#define G2_P8_7  2
#define G2_P8_8  3
#define G2_P8_9  5
#define G2_P8_10 4
#define G1_P8_11 13
#define G1_P8_12 12
#define G0_P8_13 23
#define G0_P8_14 26

#define WDOG 0x44e35048

void myWait(int ticks);

#endif /* MAIN_H_ */
