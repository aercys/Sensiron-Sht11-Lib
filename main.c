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



// Commands for Measurement ------------------------------------------------------------------------------

#define temperature 0x03
#define humidity 0x05
#define soft_reset 0x1E

// Data and Clock Pin | Ports ----------------------------------------------------------------------------

#define SCK 1
#define DATA 0

// Data and Clock Modify Macros --------------------------------------------------------------------------

#define sck_high PORTB |= (1 << SCK)
#define sck_low PORTB &= ~(1 << SCK)

#define data_high PORTB |= (1 << DATA)
#define data_low PORTB &= ~(1 << DATA)

#define data_out DDRB = (1 << SCK) | (1 << DATA)
#define data_in DDRB = (1 << SCK) | ~(1 << DATA)



// Function prototypes -----------------------------------------------------------------------------------

void initialize_transmisson(void);
void send_command(uint8_t command);
void reset(void);
void serial_init(void);
uint16_t read_data(void);
void send_uart_char(unsigned char serial_data, FILE *stream);
float calculateMeasuredData(uint16_t measuredData, uint8_t value);


/*
 *
 * Output stream for usage of printf function
 * with send_uart_char
 *
 */

FILE uart_output = FDEV_SETUP_STREAM(send_uart_char, NULL, _FDEV_SETUP_WRITE);


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
    
    uint8_t dataL, dataH;
    uint16_t measured;
    
    dataL = 0x00;
    dataH = 0x00;

    
    
    data_in;
    data_high;
    
     while (PINB & (1<<DATA));
    
    /*
     *
     * Start receiving first byte
     *
     */
    
    
    for (int j = 0; j < 8; j++) {
        
        sck_high;
        _delay_us(4);
        
       
        if ((PINB & (1 << PINB0))) {
            dataH |= (1 << (7 - j));
        } else {
            dataH &= ~(1 << (7 - j));
            
        }
        
        sck_low;
        _delay_us(4);
    }
    
 
    /*
     *
     * First Byte Received and send acknowledge
     * to sensor in order to start receiving 
     * second byte
     *
     */
     
     
    data_out;
    
    data_low;
    _delay_us(4);
    sck_high;
    _delay_us(4);
    sck_low;
    _delay_us(4);
    
    data_in;
    data_high;
    

    for (int i = 0; i < 8; i++) {
        
        sck_high;
        _delay_us(4);
        
        if ((PINB & (1 << PINB0))) {
            dataL |= (1 << (7 - i));
            
        } else {
            dataL &= ~(1 << (7 - i));
        }
        
        sck_low;
        _delay_us(4);
        
    }
   
  
   
    measured = (dataH << 8) | dataL;
    
    return measured;
    
    
}

float calculateMeasuredData(uint16_t measuredData, uint8_t value) {
    
    float humiLin, tempLin;
    
    switch (value) {
        
        case humidity:
            humiLin = measuredData * 0.0405 - 2.8 * pow(10, -6) * pow(measuredData, 2) - 4;
            return humiLin;
            break;
            
        case temperature:
            tempLin = -40.00 + 0.01 * measuredData;
            return tempLin;
            break;
            
    }
    
    return 1;
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
    
    /*
     *
     * Send each byte until reached \n character
     *
     */
    
    if (serial_data == '\n') {
        send_uart_char('\r', stream);
    }
    
    while (!(UCSR0A & (1 << UDRE0)));
    
    UDR0 = serial_data;
    
}




