#ifndef _DS18B20_H_
#define _DS18B20_H_

#include "Sys.h"

void Delay_OneWire(uint16_t t);
void Write_DS18B20(uint8_t dat);
uint8_t Read_DS18B20(void);    
bit init_ds18b20(void);
void DS18B20_Get_Tempreature(void);


#endif
