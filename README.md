## Simple SHT1x-7x Measurement Library For AVR CPUs
### Usage

As it can be noticed below, sht_data struct is utilized to hold calculated temperature and humidity data.

`sht_sense()` function initializes the sensor and makes the calculation. Initialization and calculation process is not separeted. 

```
sht_data data;
data = sht_sense();
```
