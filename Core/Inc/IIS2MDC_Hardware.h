/*
 * IIS2MDC_Hardware.h
 *
 *  Created on: Mar 27, 2022
 *      Author: evanl
 */

#ifndef INC_IIS2MDC_HARDWARE_H_
#define INC_IIS2MDC_HARDWARE_H_
/**************************************//**************************************//**************************************
 * Includes
 **************************************//**************************************//**************************************/
#include <stdint.h>

/**************************************//**************************************//**************************************
 * Typedefs / Enumerations
 **************************************//**************************************//**************************************/
typedef enum{
	IIS2MDC_IRQEnable,
	IIS2MDC_IRQDisable,
	IIS2MDC_ReadIntPin
}IIS2MDC_Cmd_t;

typedef enum{
	IIS2MDC_Ok,
	IIS2MDC_Error
}IIS2MDC_Status_t;

/**************************************//**************************************//**************************************
 * Driver Structs
 **************************************//**************************************//**************************************/
typedef struct{
	void (*Init)(void);
	void (*DeInit)(void);
	IIS2MDC_Status_t (*ReadReg)(uint8_t, uint8_t*, uint8_t);
	IIS2MDC_Status_t (*WriteReg)(uint8_t, uint8_t*, uint8_t);
	uint8_t (*ioctl)(IIS2MDC_Cmd_t);
}IIS2MDC_IO_Drv_t;


/**************************************//**************************************//**************************************
 * Public/Exported Variables
 **************************************//**************************************//**************************************/
extern IIS2MDC_IO_Drv_t IIS2MDC_Hardware_Drv;


#endif /* INC_IIS2MDC_HARDWARE_H_ */
