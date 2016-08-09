//
//  sht.h
//  avrSHT
//
//  Created by aercys on 10/08/16.
//  Copyright © 2016 aercys. All rights reserved.
//

#ifndef sht_h
#define sht_h

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

// SHT Data type
typedef struct {
    uint8_t Temperature;
    uint8_t Humidity;
} SHTData;



// Function prototypes -----------------------------------------------------------------------------------

void initialize_transmisson(void);
void send_command(uint8_t command);
void reset(void);
void serial_init(void);
uint16_t read_data(void);
void send_uart_char(unsigned char serial_data, FILE *stream);
float calculateMeasuredData(uint16_t measuredData, uint8_t value);
void receiveBytes(uint8_t *target);
void ack(void);
SHTData shtSense(void);

FILE uart_output = FDEV_SETUP_STREAM(send_uart_char, NULL, _FDEV_SETUP_WRITE);

#endif /* sht_h */
