/*
 * IIS2MDC_Hardware.c
 *
 *  Created on: Mar 27, 2022
 *      Author: evanl
 */
/**************************************//**************************************//**************************************
 * Includes
 **************************************//**************************************//**************************************/
#include "IIS2MDC_Hardware.h"
#include <stdio.h>
#include "stm32u5xx_hal.h"
#include "gpio.h"
#include "i2c.h"
#include "log.h"

/**************************************//**************************************//**************************************
 * Defines / Constants
 **************************************//**************************************//**************************************/
static const uint8_t IIS2MDC_DEVICE_ADDRESS = 0x3CU;
static const uint16_t IIS2MDC_TIMEOUT_MS = 500;

/**************************************//**************************************//**************************************
 * Private Function Prototypes
 **************************************//**************************************//**************************************/
static void IIS2MDC_Init();
static void IIS2MDC_DeInit();
static IIS2MDC_Status_t IIS2MDC_WriteReg(uint8_t reg, uint8_t *pdata, uint8_t length);
static IIS2MDC_Status_t IIS2MDC_ReadReg(uint8_t reg, uint8_t *pdata, uint8_t length);
static uint8_t IIS2MDC_ioctl(IIS2MDC_Cmd_t command);

/**************************************//**************************************//**************************************
 * Private Function Definitions
 **************************************//**************************************//**************************************/

/*Initializes low level IO*/
static void IIS2MDC_Init(){
	HAL_Delay(20); //Device takes 20 ms to boot.
	IIS2MDC_GPIO_Init();
	MX_I2C2_Init();
}

/*DeInitializes low level IO.*/
static void IIS2MDC_DeInit(){
	//Do Not De-Init I2C Peripheral as other devices may be using it.
	HAL_GPIO_DeInit(IIS2MDC_IRQ_GPIO_Port, IIS2MDC_IRQ_Pin);
}

/*Sends data to register over I2C2 Bus*/
static IIS2MDC_Status_t IIS2MDC_WriteReg(uint8_t reg, uint8_t *pdata, uint8_t length){
	if(HAL_I2C_Mem_Write(&hi2c2, IIS2MDC_DEVICE_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, pdata , length, IIS2MDC_TIMEOUT_MS) != HAL_OK){
		_log(log_i2c,"Write to IIS2MDC Reg address %x failed.",reg);
		return IIS2MDC_Error;
	}
	return IIS2MDC_Ok;
}

/*Reads data from register over I2C2 Bus*/
static IIS2MDC_Status_t IIS2MDC_ReadReg(uint8_t reg, uint8_t *pdata, uint8_t length){
	if(HAL_I2C_Mem_Read(&hi2c2, IIS2MDC_DEVICE_ADDRESS | 0x01, reg, I2C_MEMADD_SIZE_8BIT, pdata , length, IIS2MDC_TIMEOUT_MS) != HAL_OK){
		_log(log_i2c,"Read from IIS2MDC Reg address %x failed.",reg);
		return IIS2MDC_Error;
	}
	return IIS2MDC_Ok;
}

/*Performs any other needed functions for the driver.*/
static uint8_t IIS2MDC_ioctl(IIS2MDC_Cmd_t command){
	uint8_t PinStatus;
	switch(command){

	case IIS2MDC_IRQEnable:
		NVIC_EnableIRQ(IIS2MDC_IRQ_EXTI_IRQn);
		return IIS2MDC_Ok;
		break;

	case IIS2MDC_IRQDisable:
		NVIC_DisableIRQ(IIS2MDC_IRQ_EXTI_IRQn);
		return IIS2MDC_Ok;
		break;

	case IIS2MDC_ReadIntPin:
		PinStatus = HAL_GPIO_ReadPin(IIS2MDC_IRQ_GPIO_Port, IIS2MDC_IRQ_Pin);
		if(PinStatus == GPIO_PIN_SET){
			return 1;
		} else {
			return 0;
		}
	default:
		break;

	}
	return 0;
}


/**************************************//**************************************//**************************************
 * Public Variable Defitinion
 **************************************//**************************************//**************************************/
IIS2MDC_IO_Drv_t IIS2MDC_Hardware_Drv = {
		.Init = IIS2MDC_Init,
		.DeInit = IIS2MDC_DeInit,
		.WriteReg = IIS2MDC_WriteReg,
		.ReadReg = IIS2MDC_ReadReg,
		.ioctl = IIS2MDC_ioctl
};


