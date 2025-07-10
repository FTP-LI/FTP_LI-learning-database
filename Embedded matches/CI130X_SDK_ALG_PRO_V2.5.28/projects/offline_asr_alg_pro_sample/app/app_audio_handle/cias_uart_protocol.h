/*
 * @FileName:: cias_uart_protocol.h
 * @Author: 
 * @Date: 2022-03-04 11:02:42
 * @LastEditTime: 2022-05-16 15:42:07
 * @Description: uart communication function
 */

#ifndef _CIAS_UART_PROTOCOL_H_
#define _CIAS_UART_PROTOCOL_H_

int8_t network_uart_port_init(void);
int32_t network_send(int8_t *str,uint32_t length);

#endif //_CIAS_UART_PROTOCOL_H_