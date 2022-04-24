/**
 * @file    accel.c
 * @author  Mustafa Siddiqui
 * @brief   Function definitions for accelerometer module.
 * @date    04/12/2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "../inc/accel.h"
#include "../inc/spi.h"     // _SPI_*() functions
#include "../inc/uart.h"    // added for debugging
//-//
#include <xc.h>
#include <stdio.h>  // sprintf()
#include <string.h> // memset()
#include <math.h>   // sqrt()

/* create first data byte for SDI line for accelerometer */
unsigned char _ACCEL_createDataByte1(int RW, int MB, unsigned char addr) {
    unsigned char dataByte1 = 0x0;
    dataByte1 |= addr;

    // set read/write bit
    if (RW == 1)
        SET(dataByte1, 7);
    else
        CLEAR(dataByte1, 7);

    // set multiple byte transmission bit
    if (MB == 1)
        SET(dataByte1, 6);
    else
        CLEAR(dataByte1, 6);

    return dataByte1;
}

/* write to register on accelerometer */
signed char _ACCEL_writeToRegister(unsigned char addr, unsigned char data) {
    // first byte consists of R/W, MB, and address bits (of the register to write to)
    // latter byte is data to be written
    unsigned char dataByte_1 = _ACCEL_createDataByte1(0, 0, addr);
    unsigned char dataByte_2 = data;

    // start transmission by pulling ~CS low
    _SPI_selectSlave(ACCELEROMETER);
    
    signed char status1 = _SPI_write(dataByte_1);
    signed char status2 = _SPI_write(dataByte_2);

    // end transmisson by setting ~CS high
    _SPI_unselectSlave(ACCELEROMETER);
    
    // write not successful; collision occured somewhere
    if ((status1 != 0) || (status2 != 0)) {
        return -1;
    }

    return 0;
}

/* read from register on accelerometer */
unsigned char _ACCEL_readFromRegister(unsigned char addr) {
    // to store received data
    unsigned char data = 0x00;

    // first byte consists of R/W, MB, and address of register to read
    unsigned char dataByte_1 = _ACCEL_createDataByte1(1, 0, addr);

    // start transmission by pulling ~CS low
    _SPI_selectSlave(ACCELEROMETER);
    
    // write on MOSI line & receive on MISO line
    _SPI_write(dataByte_1);
    data = _SPI_read();
    
    // end transmission by setting ~CS high
    _SPI_unselectSlave(ACCELEROMETER); 
    
    return data;
}

/* get device ID */
unsigned char _ACCEL_getDeviceID(void) {
    return _ACCEL_readFromRegister(_ADDR_DEVID);
}

/* get current x, y, z axis readings */
void _ACCEL_getCurrentReading(float *sensorData) {
    unsigned char xVal_0 = _ACCEL_readFromRegister(_ADDR_DATA_X0);
    unsigned char xVal_1 = _ACCEL_readFromRegister(_ADDR_DATA_X1);
    unsigned char yVal_0 = _ACCEL_readFromRegister(_ADDR_DATA_Y0);
    unsigned char yVal_1 = _ACCEL_readFromRegister(_ADDR_DATA_Y1);
    unsigned char zVal_0 = _ACCEL_readFromRegister(_ADDR_DATA_Z0);
    unsigned char zVal_1 = _ACCEL_readFromRegister(_ADDR_DATA_Z1);

    // combine high and low values into one number
    // bits represent signed 2's complement format
    float x = (xVal_0 << 8) | xVal_1;
    float y = (yVal_0 << 8) | yVal_1;
    float z = (zVal_0 << 8) | zVal_1;

    // populate var
    memset(sensorData, 0, 3);
    sensorData[0] = x;
    sensorData[1] = y;
    sensorData[2] = z;
}

/* initialize accelerometer module */
int initAccel(void) {
    // => [self_test, spi, int_invert, 0, full_res, justify, range<1:0>]
    //      spi mode: 3-wire (1), 4-wire (0)
    //      resolution: full_res (1), 10-bit mode (0)
    //      justify: left-justified (MSB) mode (1), right-justified (0)
    //      range: 16g
    _ACCEL_writeToRegister(_ADDR_DATA_FORMAT, 0x07);
    
    // enable measurement mode (set bit D3)
    _ACCEL_writeToRegister(_ADDR_POWER_CTL, 0x08);

    // select rate_bits in bw_rate to set output data rate
    // => default value is 0x0A which translates to 100 Hz, let's keep it at that atm
    
    // return 0 if incorrect device id
    if (_ACCEL_getDeviceID() != DEVID) 
        return 0;
    
    return 1;
}

/* get zenith angle value */
int getCurrentZenith(void) {
    // we don't to interpret sensor readings in terms of g (9.8 m/s^2) because
    // the calculations cancel out the effect i.e. we obtain a ratio -- which is
    // going to be the same regardless
    // format: [x, y, z]
    float sensorData[3] = {0.0};
    _ACCEL_getCurrentReading(sensorData);
    
    // see axis data used for calculations
    char dataStr[20];
    sprintf(dataStr, "[%f, %f, %f]", sensorData[0], sensorData[1], sensorData[2]);
    UART_send_str(dataStr);
    __delay_ms(1000);
    
    // V = sqrt(Vx^2 + Vy^2 + Vz^2)
    float vector = sqrt(SQUARE(sensorData[0] + SQUARE(sensorData[1]) + SQUARE(sensorData[2])));

    // calculate the zenith angle (angle between vector and the vertical axis)
    float angle = acos((float)sensorData[2] / vector);

    // return integer value of angle in degrees
    return (int)(angle * (180/M_PI));
}
