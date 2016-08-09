//
//  sht.c
//  avrSHT
//
//  Created by aercys on 10/08/16.
//  Copyright © 2016 aercys. All rights reserved.
//

#include <stdio.h>

void initialize_transmisson() {
    /*
     * SHT11
     * Start Sequence
     *        __    __
     * SCK  _|  |__|  |__
     *      __       ____
     * DATA   |_____|
     *
     */
    data_out;
    data_high;
    _delay_us(2);
    sck_high;
    _delay_us(2);
    data_low;
    _delay_us(2);
    sck_low;
    _delay_us(8);
    sck_high;
    _delay_us(2);
    data_high;
    _delay_us(2);
    sck_low;
}

void send_command(uint8_t command) {
    _delay_us(4);
    data_out;                               // Enable data as output port
    
    for (int i = 0; i < 8; i++) {
        if (0x01 & (command >> (7-i))) {
            data_high;
        } else {
            data_low;
        }
        
        _delay_us(4);
        sck_high;
        _delay_us(4);
        sck_low;
        
        if (i == 7) {
            data_in;
            data_high;
            while ((PINB & (1 << DATA)));   // Wait for data line low
        }
    }
    
    _delay_us(4);
    sck_high;
    _delay_us(4);
    sck_low;
}

void reset() {
    data_out;
    data_high;
    
    for (int i = 0; i < 9; i++) {
        _delay_us(4);
        sck_high;
        _delay_us(4);
        sck_low;
    }
    initialize_transmisson();
}



uint16_t read_data() {
    uint8_t data_l, data_h;
    uint16_t measured;
    data_l = 0x00;
    data_h = 0x00;
    data_in;
    data_high;
    
    while (PINB & (1<<DATA));
    //Start receiving MSB
    receive_bytes(&data_h);
    ack(); // ack() to start getting LSB
    //Start receiving LSB
    receive_bytes(&data_l);
    
    measured = (data_h << 8) | data_l;
    return measured;
}

float calculate_measured_data(uint16_t measured_data, uint8_t value) {
    float _humidity, _temperature;
    
    switch (value) {
        case humidity:
            _humidity = measured_data * 0.0405 - 2.8 * pow(10, -6) * pow(measured_data, 2) - 4;
            return _humidity;
            break;
        case temperature:
            _temperature = -40.00 + 0.01 * measured_data;
            return _temperature;
            break;
    }
    
    return 1;
}

sht_data sht_sense() {
    uint8_t _temperature, _humidity;
    uint16_t __humidity;
    sht_data data;
    initialize_transmisson();   // Initilize the sensor to send command
    send_command(temperature);
    _delay_us(80);              // Wait for measurement to complete
    _temperature = calculate_measured_data(read_data(), temperature);
    reset();
    send_command(humidity);
    _delay_us(80);
    __humidity = read_data();
    _humidity = calculate_measured_data(__humidity, humidity);
    _humidity = (_temperature - 25) * (0.01 + 0.00008 * __humidity) + _humidity;
    reset();
    data._temperature = _temperature;
    data._humidity = _humidity;
    _delay_ms(100);
    return data;
}


void serial_init() {
    /*
     *
     * Initialize UART,
     * 8 bit frame, 1 Stop Bit
     * No Parity
     *
     */
    UCSR0B = (1 << TXEN0);
    UBRR0L = 51;
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void send_uart_char(unsigned char serial_data, FILE *stream) {
    if (serial_data == '\n') { // Send each byte until reached \n character
        send_uart_char('\r', stream);
    }
    
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = serial_data;
}

void receive_bytes(uint8_t *target) {
    for (int i = 0; i < 8; i++) {
        sck_high;
        _delay_us(4);
        if ((PINB & (1 << PINB0))) {
            *target |= (1 << (7 - i));
        } else {
            *target &= ~(1 << (7 - i));
        }
        sck_low;
        _delay_us(4);
    }
}

void ack(void) {
    data_out;
    data_low;
    _delay_us(4);
    sck_high;
    _delay_us(4);
    sck_low;
    _delay_us(4);
    data_in;
    data_high;
}
