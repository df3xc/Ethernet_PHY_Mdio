/*
 * system_util.c
 *
 *  Created on: 19.11.2017
 *      Author: df3xc
 */

#include "soc_AM335x.h"
#include "hw_types.h"


/**
 * \brief  This API performs the reset of the CPU. (CLU)
 */

void swReset()
{
    HWREG(SOC_PRM_DEVICE_REGS) |= 0x02;
}
