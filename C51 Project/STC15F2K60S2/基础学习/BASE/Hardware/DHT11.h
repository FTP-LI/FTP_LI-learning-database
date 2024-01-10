#ifndef __DHT11_H__
#define __DHT11_H__
 
#include "Sys.h" 

void DHT11_Start(void);
uint8_t DHT11_Byte(void);
void DHT11_Receive(void);
void Tran_Dec(void);
    
#endif