#ifndef _HC_SR04_H_
#define _HC_SR04_H_

#define TRIG GPIO_Pin_0//�����ź�
#define ECHO GPIO_Pin_1//�����ź�
#define HC_SR04_PROT GPIOA//������

uint16_t HC_SR04_avange(void);
void HC_SR04_Distance(uint16_t *dataA,uint16_t *dataB);
void TIM2_IRQHandler(void);//�ж�
void HC_SR04_Init(void);

#endif
