/**
 * @file    spi.h
 * @author  Mustafa Siddiqui
 * @brief   Header file for SPI communication on PIC18 MCU.
 * @date    04/14/2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _SPI_H_
#define _SPI_H_

/* Serial Port Pins */
#define _SPI_SDI TRISCbits.TRISC4  /* serial data in */
#define _SPI_SDO TRISCbits.TRISC5  /* serial data out */
#define _SPI_SCK TRISCbits.TRISC3  /* serial clock */

#define _SPI_CS1 LATEbits.LATE2  /* chip select 1 */
#define _SPI_CS2 LATDbits.LATD6  /* chip select 2 */

/* macros to choose slave */
#define ACCELEROMETER   1
#define MAGNETOMETER    2

/**
 * @brief   Enable SPI IO i.e. basically enable the serial port on the PIC18.
 * @param   NULL
 * @return  NULL
 */
void _SPI_enableIO(void);

/**
 * @brief   Disable serial port function.
 * @param   NULL
 * @return  NULL
 */
void _SPI_disableIO(void);

/**
 * @brief   Initialize the SPI module on the PIC18. 
 *          Master mode, clock polarity, data input sample phase, clock edge,
 *          and clock rate are set.
 * @param   NULL
 * @return  NULL
 */
void initSPI(void);

/**
 * @brief   Selects slave given that the PIC18 is in master mode.
 *          => DOES NOT configure slave device <=
 * @param   slave: accelerometer or magnetometer
 * @return  NULL
 */
void _SPI_selectSlave(int slave);

/**
 * @brief   Transmit 8 bits of data to selected slave device.
 * @param   data: 1 byte
 * @param   slave: accelerometer or  magnetometer
 * @return  NULL
 */
void _SPI_write(unsigned char data, int slave);

/**
 * @brief   Receive n bits of data where n = length * 8.
 *          i.e. length = number of bytes to receive
 * @param   data: passed by reference, data received is stored here 
 * @param   length: size of data
 * @return  NULL
 */
void _SPI_read(unsigned char* data, int length);

#endif /* _SPI_H_*/
