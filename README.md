# IIS2MDC_Driver

Simple IIS2MDC Magnetometer Driver

Files of Interest:

IIS2MDC.h: Device specific header file - Shouldn't need modification
IIS2MDC.c: Device specific source file - Shouldn't need modification
IIS2MDC_Hardware.h: Hardware specific header file - Should not need modification beyond the exported low level driver
IIS2MDC_Hardware.c: Hardware specific source file - User must implement this file for their board/project needs

To Use:
0. Include IIS2MDC.h
1. Create an IIS2MDC_Init_Struct_t with desired user settings.
2. Create an IIS2MDC_IO_Drv_t with necessary low level IO functions (I2C/SPI, GPIO Communication functions).
3. Create a IIS2MDC_Handle_t
4. Pass the init struct, IO Driver, and device handle to IIS2MDC_Init()
5. Functions listed in IIS2MDC.h can now be used by passing the initialized device handle as a function arguement
Above example was implemented on an STM32U5 processor (b-u585i-iot02a discovery board)
