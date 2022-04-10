/*
 * IIS2MDC.h
 *
 *  Created on: Mar 27, 2022
 *      Author: evanl
 */

#ifndef INC_IIS2MDC_H_
#define INC_IIS2MDC_H_
/**************************************//**************************************//**************************************
 * Includes
 **************************************//**************************************//**************************************/
#include "IIS2MDC_Hardware.h"
#include <stdint.h>
/**************************************//**************************************//**************************************
 * Typedefs / Enumerations
 **************************************//**************************************//**************************************/
typedef enum{
	IIS2MDC_DataReady,
	IIS2MDC_DataNotReady
}IIS2MDC_DataReadyStatus_t;

typedef enum{
	IIS2MDC_TemperatureCompDisabled,
	IIS2MDC_TemperatureCompEnabled
}IIS2MDC_TemperatureComp_t;

typedef enum{
	IIS2MDC_HighResolutionMode,
	IIS2MDC_LowPowerMode
}IIS2MDC_ResolutionPowerMode_t;

typedef enum{
	IIS2MDC_10Hz,
	IIS2MDC_20Hz,
	IIS2MDC_50Hz,
	IIS2MDC_100Hz
}IIS2MDC_OutputDataRate_t;

typedef enum{
	IIS2MDC_ContinuousMode,
	IIS2MDC_OneShotMode,
	IIS2MDC_IdleMode
}IIS2MDC_OperatingMode_t;

typedef enum{
	IIS2MDC_OffsetCancellationDisabled,
	IIS2MDC_OffsetCancellationEnabled,
}IIS2MDC_OffsetCancelation_t;

typedef enum{
	IIS2MDC_OffsetCancellationPulseDI,
	IIS2MDC_OffsetCancellationPulseEN,
}IIS2MDC_OffsetCancelationPulseMode_t;

typedef enum{
	IIS2MDC_TriggerIRQwithoutOffset,
	IIS2MDC_TriggerIRQWithOffset
}IIS2MDC_IRQwithOffset_t;

typedef enum{
	IIS2MDC_LowPassFilterDisabled,
	IIS2MDC_LowPassFilterEnabled
}IIS2MDC_LowPassFilterMode_t;

typedef enum{
	IIS2MDC_DrdySignalDisabled,
	IIS2MDC_DrdyOnPin,
}IIS2MDC_DrdyPinMode_t;

typedef enum{
	IIS2MDC_IntSignalDisabled,
	IIS2MDC_IntOnPin
}IIS2MDC_IntPinMode_t;


/*Bitwise OR these together*/
typedef enum{
	IIS2MDC_XThresholdEnabled = (1 << 0),
	IIS2MDC_YThresholdEnabled = (1 << 1),
	IIS2MDC_ZThresholdEnabled = (1 << 2),
	IIS2MDC_IRQActiveHigh = (1 << 5),
	IIS2MDC_IRQBitLatched = (1 << 6),
	IIS2MDC_IRQEnabled = (1 << 7)
}IIS2MDC_IRQConfig_t;

/**************************************//**************************************//**************************************
 * Driver Structs
 **************************************//**************************************//**************************************/
typedef struct{
	IIS2MDC_IO_Drv_t IIS2MDC_IO;
	IIS2MDC_DataReadyStatus_t DataReadyFlag;
	int16_t MagX;
	int16_t MagY;
	int16_t MagZ;
}IIS2MDC_Handle_t;

typedef struct{
	int16_t Offset_X;
	int16_t Offset_Y;
	int16_t Offset_Z;
	int16_t IntThreshold;
	IIS2MDC_TemperatureComp_t TempComp;
	IIS2MDC_ResolutionPowerMode_t PowerMode;
	IIS2MDC_OutputDataRate_t DataRate;
	IIS2MDC_OperatingMode_t OperatingMode;
	IIS2MDC_OffsetCancelation_t OffsetCancellation;
	IIS2MDC_OffsetCancelationPulseMode_t OffsetCancellationPulse;
	IIS2MDC_IRQwithOffset_t IRQOffsetMode;
	IIS2MDC_LowPassFilterMode_t LPF;
	IIS2MDC_DrdyPinMode_t DrdyPinMode;
	IIS2MDC_IntPinMode_t IntPinMode;
	IIS2MDC_IRQConfig_t IRQConfig;
}IIS2MDC_InitStruct_t;

/**************************************//**************************************//**************************************
 * Defines
 **************************************//**************************************//**************************************/
#define IIS2MDC_REG_OFFSET_X_REG_L (0x45U)
#define IIS2MDC_REG_OFFSET_X_REG_H (0x46U)
#define IIS2MDC_REG_OFFSET_Y_REG_L (0x47U)
#define IIS2MDC_REG_OFFSET_Y_REG_H (0x48U)
#define IIS2MDC_REG_OFFSET_Z_REG_L (0x49U)
#define IIS2MDC_REG_OFFSET_Z_REG_H (0x4AU)

#define IIS2MDC_REG_WHO_AM_I (0x4FU)

#define IIS2MDC_REG_CFG_REG_A (0x60U)
#define IIS2MDC_REG_CFG_REG_B (0x61U)
#define IIS2MDC_REG_CFG_REG_C (0x62U)

#define IIS2MDC_REG_INT_CTRL_REG (0x63U)
#define IIS2MDC_REG_INT_SOURCE_REG (0x64U)
#define IIS2MDC_REG_INT_THS_L_REG (0x65U)
#define IIS2MDC_REG_INT_THS_H_REG (0x66U)

#define IIS2MDC_REG_STATUS_REG (0x67U)

#define IIS2MDC_REG_OUTX_L_REG (0x68U)
#define IIS2MDC_REG_OUTX_H_REG (0x69U)
#define IIS2MDC_REG_OUTY_L_REG (0x6AU)
#define IIS2MDC_REG_OUTY_H_REG (0x6BU)
#define IIS2MDC_REG_OUTZ_L_REG (0x6CU)
#define IIS2MDC_REG_OUTZ_H_REG (0x6DU)

#define IIS2MDC_REG_TEMP_OUT_L_REG (0x6EU)
#define IIS2MDC_REG_TEMP_OUT_H_REG (0x6FU)


/**************************************//**************************************//**************************************
 * Public Function Prototypes
 **************************************//**************************************//**************************************/
void IIS2MDC_Init(IIS2MDC_InitStruct_t Settings, IIS2MDC_Handle_t *Dev, IIS2MDC_IO_Drv_t LowLevelDrivers);
void IIS2MDC_DeInit(IIS2MDC_Handle_t *Dev);
void IIS2MDC_Reset(IIS2MDC_Handle_t *Dev);
void IIS2MDC_StartConversion(IIS2MDC_Handle_t *Dev);
IIS2MDC_DataReadyStatus_t IIS2MDC_ReadMagnetic(IIS2MDC_Handle_t *Dev);
void IIS2MDC_ReadReg(IIS2MDC_Handle_t *Dev, uint8_t reg, uint8_t *pdata, uint8_t length);
void IIS2MDC_WriteReg(IIS2MDC_Handle_t *Dev, uint8_t reg, uint8_t *pdata, uint8_t length);

#endif /* INC_IIS2MDC_H_ */
