/**
 * @file    main.c
 * @author  Ali Choudhry, Carter Bordeleau, Mahmud Jumaev, Mustafa Siddiqui.
 * @brief   Main logic for the solar tracking system.
 * @date    04/06/2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "../inc/init.h"
#include "../inc/spi.h"
#include "../inc/accel.h"
#include "../inc/uart.h"
//-//
#include <xc.h>
#include <stdio.h>  // sprintf()
    
#define _XTAL_FREQ 8000000  // 8 MHz

int main(void) {
    // set clock freq to 8 MHz
    OSCCON = 0x72;
    
    // set all pins as digital output
    initPins();
    
    // turn on LEDs to indicate start of init process
    LATDbits.LATD2 = 1;
    LATDbits.LATD3 = 1;
    
    // initialize PIC18 as master for SPI
    initSPI();
    _delay(10000);
    
    // initialize UART module
    UART_RX_Init();
    UART_send_str(" UART initialized...");
    _delay(10000);
    
    // initialize accelerometer for communication
    int status = initAccel();
    
    // turn off LEDs to indicate end of init process
    LATDbits.LATD2 = 0;
    LATDbits.LATD3 = 0;
    _delay(10000);
    
    while (1) {
        if (status) {
            unsigned char deviceID = _ACCEL_getDeviceID();
            char str[20];
            sprintf(str, " Device ID: %x ", deviceID);
            UART_send_str(str);
            _delay(10000);
        }
        
        // test spi communication
        unsigned char dataFormatReg = _ACCEL_readFromRegister(_ADDR_DATA_FORMAT);
        char strToSend[20];
        sprintf(strToSend, " Data Format: %x ", dataFormatReg);
        UART_send_str(strToSend);
        _delay(10000);
    }

    return 0;
}
