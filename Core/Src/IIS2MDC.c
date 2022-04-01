/*
 * IIS2MDC.c
 *
 *  Created on: Mar 27, 2022
 *      Author: evanl
 */
/**************************************//**************************************//**************************************
 * Includes
 **************************************//**************************************//**************************************/
#include "IIS2MDC.h"
#include "log.h"
/**************************************//**************************************//**************************************
 * Typedefs / Enumerations
 **************************************//**************************************//**************************************/

/**************************************//**************************************//**************************************
 * Private Function Prototypes
 **************************************//**************************************//**************************************/
static void ConvertMagnetic(IIS2MDC_Handle_t *Dev,uint8_t *pdata);
/**************************************//**************************************//**************************************
 * Defines / Constants
 **************************************//**************************************//**************************************/
static const uint8_t IIS2MDC_DEVICE_ID = 0x40;
/**************************************//**************************************//**************************************
 * Public Function Definitions
 **************************************//**************************************//**************************************/

/**************************************//**************************************
 *@Brief: Initializes a IIS2MDC Device Handle
 *@Params: IIS2MDC Init Settings, Dev Handle pointer, Low level driver structure
 *@Return: None
 *@Precondition: LowLevelDrivers and Settings params should already be initialized.
 *@Postcondition: Dev Handle members and IIS2MDC Hardware registers will be initialized.
 **************************************//**************************************/
void IIS2MDC_Init(IIS2MDC_InitStruct_t Settings, IIS2MDC_Handle_t *Dev, IIS2MDC_IO_Drv_t LowLevelDrivers){
	Dev->IIS2MDC_IO.Init = LowLevelDrivers.Init;
	Dev->IIS2MDC_IO.DeInit = LowLevelDrivers.DeInit;
	Dev->IIS2MDC_IO.WriteReg = LowLevelDrivers.WriteReg;
	Dev->IIS2MDC_IO.ReadReg = LowLevelDrivers.ReadReg;
	Dev->IIS2MDC_IO.ioctl = LowLevelDrivers.ioctl;
	Dev->IIS2MDC_IO.Init();

	Dev->IIS2MDC_IO.ioctl(IIS2MDC_IRQDisable);

	uint8_t buffer8;
	uint16_t buffer16;

	/*WHO AM I*/
	if(Dev->IIS2MDC_IO.ReadReg(IIS2MDC_REG_WHO_AM_I, &buffer8,1) != IIS2MDC_Ok){
		_log(log_iis2mdc, "Initialization: Read Device ID Reg Failed.");
	} else if(buffer8 != IIS2MDC_DEVICE_ID){
		_log(log_iis2mdc, "Initialization: Device ID Mismatch");
	}

	/*Offset X Regs*/
	buffer16 = Settings.Offset_X;
	if(Dev->IIS2MDC_IO.WriteReg(IIS2MDC_REG_OFFSET_X_REG_L, (uint8_t*)&buffer16,2) != IIS2MDC_Ok){
		_log(log_iis2mdc, "Initialization: Offset X Write Failed");
	}

	/*Offset Y Regs*/
	buffer16 = Settings.Offset_Y;
	if(Dev->IIS2MDC_IO.WriteReg(IIS2MDC_REG_OFFSET_Y_REG_L, (uint8_t*)&buffer16,2) != IIS2MDC_Ok){
		_log(log_iis2mdc, "Initialization: Offset Y Write Failed");
	}

	/*Offset Z Regs*/
	buffer16 = Settings.Offset_Z;
	if(Dev->IIS2MDC_IO.WriteReg(IIS2MDC_REG_OFFSET_Z_REG_L, (uint8_t*)&buffer16,2) != IIS2MDC_Ok){
		_log(log_iis2mdc, "Initialization: Offset Z Write Failed");
	}

	/*IRQ Threshold*/
	buffer16 = Settings.IntThreshold;
	if(Dev->IIS2MDC_IO.WriteReg(IIS2MDC_REG_INT_THS_L_REG, (uint8_t*)&buffer16,2) != IIS2MDC_Ok){
		_log(log_iis2mdc, "Initialization: Int Threshold Write Failed");
	}

	/*CFG A*/
	buffer8 = (Settings.TempComp << 7) | (Settings.PowerMode << 4) | (Settings.DataRate << 2) | (Settings.OperatingMode << 0);
	if(Dev->IIS2MDC_IO.WriteReg(IIS2MDC_REG_CFG_REG_A,&buffer8,1) != IIS2MDC_Ok){
		_log(log_iis2mdc, "Initialization: Write CFG Reg A Failed.");
	}

	/*CFG B*/
	buffer8 = (Settings.IRQOffsetMode << 3) | (Settings.OffsetCancellationPulse << 2) | (Settings.LPF << 0);
	if(Settings.OperatingMode == IIS2MDC_OneShotMode){
		buffer8 |= Settings.OffsetCancellation << 4;
	} else {
		buffer8 |= Settings.OffsetCancellation << 1;
	}

	if(Dev->IIS2MDC_IO.WriteReg(IIS2MDC_REG_CFG_REG_B,&buffer8,1) != IIS2MDC_Ok){
		_log(log_iis2mdc, "Initialization: Write CFG Reg B Failed.");
	}

	/*CFG C*/
	buffer8 = (Settings.IntPinMode << 6) | (1 << 4) | (Settings.DrdyPinMode);
	if(Dev->IIS2MDC_IO.WriteReg(IIS2MDC_REG_CFG_REG_C,&buffer8,1) != IIS2MDC_Ok){
		_log(log_iis2mdc, "Initialization: Write CFG Reg C Failed.");
	}

	/*Int Source Reg*/
	buffer8 = Settings.IRQConfig;
	if(Dev->IIS2MDC_IO.WriteReg(IIS2MDC_REG_INT_CTRL_REG,&buffer8,1) != IIS2MDC_Ok){
		_log(log_iis2mdc, "Initialization: Write Int Ctrl Reg Failed.");
	}

	/*Clear Data acquired while initializing*/
	uint8_t buffer6bytes[6];
	if(Dev->IIS2MDC_IO.ReadReg(IIS2MDC_REG_OUTX_L_REG,buffer6bytes,6) != IIS2MDC_Ok){
		_log(log_iis2mdc, "Initialization: Reading Data Reg Failed.");
	}

	Dev->IIS2MDC_IO.ioctl(IIS2MDC_IRQEnable);


}


/**************************************//**************************************
 *@Brief: Resets and Deinitializes Low level hardware interface for given device handle
 *@Params: Device handle to be de-intialized.
 *@Return: None
 *@Precondition: Device handle is initialized.
 *@Postcondition: Sensor chip will be reset and MCU gpio pins deinitialized. Does not de-init communication bus in case anyone other device is using it.
 **************************************//**************************************/
void IIS2MDC_DeInit(IIS2MDC_Handle_t *Dev){
	IIS2MDC_Reset(Dev);
	Dev->IIS2MDC_IO.DeInit();
}


/**************************************//**************************************
 *@Brief: Resets given II2SMDC Chip Handle to default settings.
 *@Params: II2SMDC Device handle
 *@Return: None
 *@Precondition: Device handle is initalized.
 *@Postcondition: IIS2MDC chip registers reset to default settings.
 **************************************//**************************************/
void IIS2MDC_Reset(IIS2MDC_Handle_t *Dev){
	uint8_t reset_signal = 1 << 5;
	if(Dev->IIS2MDC_IO.WriteReg(IIS2MDC_REG_CFG_REG_A, &reset_signal,1) != IIS2MDC_Ok){
		_log(log_iis2mdc, "Reset Failed.");
	}
}


/**************************************//**************************************
 *@Brief: Starts a conversion for a Device in one-shot mode.
 *@Params: IIS2MDC Device Handle
 *@Return: None
 *@Precondition: Device handle should be initialized in ONE SHOT Mode
 *@Postcondition: Device will begin an A-to-D conversion
 **************************************//**************************************/
void IIS2MDC_StartConversion(IIS2MDC_Handle_t *Dev){
	uint8_t reg;
	if(Dev->IIS2MDC_IO.ReadReg(IIS2MDC_REG_CFG_REG_A, &reg, 1) != IIS2MDC_Ok){ //read reg
		_log(log_iis2mdc, "Read CFG A Reg Failed.");
	}

	reg &= ~(1 << 1); //Modify reg
	reg |=  (1 << 0);

	if(Dev->IIS2MDC_IO.WriteReg(IIS2MDC_REG_CFG_REG_A, &reg, 1) != IIS2MDC_Ok){ //store reg
		_log(log_iis2mdc, "Writing CFG A Reg Failed.");
	}
}


/**************************************//**************************************
 *@Brief: Reads Data from given IIS2MDC Sensor and converts it to millgause
 *@Params: IIS2MDC Device Handle
 *@Return: Status of read attempt: IIS2MDC_DataNotReady if new data is not available, IIS2MDC_DataReady if data was read successfully
 *@Precondition: Device handle is initialized, StartConversion should be called prior to this in OneShot mode (otherwise the read wont be successful)
 *@Postcondition: Dev Handle will contain new data in Milligause. If data was read successfully, DataReadyFlag will be set to IIS2MDC_DataNotReady.
 **************************************//**************************************/
IIS2MDC_DataReadyStatus_t IIS2MDC_ReadMagnetic(IIS2MDC_Handle_t *Dev){
	uint8_t buffer[6];
	if(Dev->IIS2MDC_IO.ReadReg(IIS2MDC_REG_STATUS_REG, buffer,1) != IIS2MDC_Ok){
		_log(log_iis2mdc, "Reading Status Reg Failed.");
		return IIS2MDC_DataNotReady;
	} else if ((buffer[0] & 0x07) != 0x07){
		Dev->DataReadyFlag = IIS2MDC_DataNotReady;
		return IIS2MDC_DataNotReady;
	}

	if(Dev->IIS2MDC_IO.ReadReg(IIS2MDC_REG_OUTX_L_REG, buffer,6) != IIS2MDC_Ok){
		_log(log_iis2mdc, "Reading Data Regs Failed.");
	}
	Dev->DataReadyFlag = IIS2MDC_DataNotReady; //Data has been read, so reset data ready flag
	ConvertMagnetic(Dev, buffer);

	return IIS2MDC_DataReady;
}


/**************************************//**************************************
 *@Brief: Reads a given register from an IIS2MDC Device
 *@Params: Device Handle, Reg to read, data buffer, number of bytes to read
 *@Return: None
 *@Precondition: Device handle is initialized
 *@Postcondition: pdata will contain the read register data.
 **************************************//**************************************/
void IIS2MDC_ReadReg(IIS2MDC_Handle_t *Dev, uint8_t reg, uint8_t *pdata, uint8_t length){
	if(Dev->IIS2MDC_IO.ReadReg(reg, pdata, length) != IIS2MDC_Ok){
		_log(log_iis2mdc, "Read Reg Failed. Address: %x", reg);
	}
}


/**************************************//**************************************
 *@Brief: Write a given register from an IIS2MDC Device
 *@Params: Device Handle, Reg to write, data buffer, number of bytes to write
 *@Return: None
 *@Precondition: Device handle is initialized
 *@Postcondition: Given register(s) will be written to in the IIS2MDC Device
 **************************************//**************************************/
void IIS2MDC_WriteReg(IIS2MDC_Handle_t *Dev, uint8_t reg, uint8_t *pdata, uint8_t length){
	if(Dev->IIS2MDC_IO.WriteReg(reg, pdata, length) != IIS2MDC_Ok){
		_log(log_iis2mdc, "Write Reg Failed. Address: %x ", reg);
	}
}

/**************************************//**************************************//**************************************
 * Private Function Definitions
 **************************************//**************************************//**************************************/

/**************************************//**************************************
 *@Brief: Converts given raw sensor data to Milligause
 *@Params: Device handle to store data in, buffer of raw sensor output data
 *@Return: None
 *@Precondition: Device handle is initialzed, pdata contains valid data
 *@Postcondition: Device Handle will contain new magnetism measurements in Milligause.
 **************************************//**************************************/
static void ConvertMagnetic(IIS2MDC_Handle_t *Dev, uint8_t *pdata){
	int16_t MagX = ((int8_t)pdata[1] << 8) |  ((int8_t)pdata[0] << 0);
	int16_t MagY = ((int8_t)pdata[3] << 8) |  ((int8_t)pdata[2] << 0);
	int16_t MagZ = ((int8_t)pdata[5] << 8) |  ((int8_t)pdata[4] << 0);
	Dev->MagX = MagX * 1.5;
	Dev->MagY = MagY * 1.5;
	Dev->MagZ = MagZ * 1.5;
}

