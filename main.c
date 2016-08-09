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
    
    float humidity_data, humidity_data_com, temperature_data;
    uint16_t humidityLin;
    
    _delay_us(11);                  // Wait for sensor initialization
    
    serial_init();
    stdout = &uart_output;
    
    while (1) {
        initialize_transmisson();   // Initilize the sensor to send command
        send_command(temperature);
        _delay_us(80);              // Wait for measurement to complete
        temperature_data = calculateMeasuredData(read_data(), temperature);
        reset();
        send_command(humidity);
        _delay_us(80);
        humidityLin = read_data();
        humidity_data = calculateMeasuredData(humidityLin, humidity);
        humidity_data_com = (temperature_data - 25) * (0.01 + 0.00008 * humidityLin) + humidity_data;
        printf("Humidity: %.2f (Compansated: %.2f) Temp: %.2f \n",
               (double)humidity_data, (double)humidity_data_com, (double)temperature_data);
        reset();
        _delay_ms(1000);
    }
    
    return 0; // never reached
}





