#ifndef _E2ROM_H_
#define _E2ROM_H_

#include "Sys.h"

void IapIdle(void);
uint8_t IapReadByte(uint16_t addr);
void IapProgramByte(uint16_t addr,uint8_t dat);
void IapEraseSector(uint16_t addr);

#endif
