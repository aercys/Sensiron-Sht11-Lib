/*
 *
 * [Atmega328p - SHT11]
 * Temperature & Humidity Measurement Application
 * Author: aercys
 * E-Mail: aercys@gmail.com
 * Web: i2xr.com
 * Date: 10/03/2015
 *
 */



#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>
#include "sht/sht.h"
#include "sht/sht.c"


int main(void) {
    _delay_us(11);                  // Wait for sensor initialization
    serial_init();
    stdout = &uart_output; // Assign output
    while (1) {
        SHTData data;
        data = shtSense();
        printf("Humidity: %.2f, Temp: %.2f \n",
               (double)data.Humidity, (double)data.Temperature);
    }
    
    return 0; // never reached
}





