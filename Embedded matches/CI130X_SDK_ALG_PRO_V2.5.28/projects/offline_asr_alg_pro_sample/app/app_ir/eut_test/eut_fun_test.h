
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "voice_module_uart_protocol.h"
#include "ci130x_uart.h"
#include "sdk_default_config.h"

#ifndef _EUT_FUN_TEST_
#define _EUT_FUN_TEST_


#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
    EUT_STATE_IDLE = 1,
	EUT_STATE_IR_RECV_FAIL,
	EUT_STATE_IR_RECV_FAIL_HANDSHK,
	EUT_STATE_IR_RECV_OK,
	
	EUT_STATE_IR_SEND_START=5,
	EUT_STATE_IR_SEND_FAIL_HANDSHK,

	EUT_STATE_MIC_START=7,
	EUT_STATE_MIC_FAIL_HANDSHK,
	
	RECV_STATE_SPK_START=9,
	RECV_STATE_SPK_FAIL_HANDSHK,

	RECV_STATE_TEST_END_HANDSHK,

	
}test_send_messg_state_t;

typedef enum
{
    BOARD_STATE_IDLE = 1,
	BOARD_STATE_IR_RECV_START,	
	BOARD_STATE_IR_RECV_FAIL_HANDSHK,
	BOARD_STATE_IR_SEND_START,
	BOARD_STATE_IR_SEND_FAIL_HANDSHK=5,
	BOARD_STATE_MIC_START,
	BOARD_STATE_MIC_FAIL_HANDSHK=7,
	BOARD_STATE_SPK_START,
	BOARD_STATE_SPK_FAIL_HANDSHK,

	BOARD_STATE_TEST_END=10,
	BOARD_STATE_TEST_END_HANDSHK=11,
	
}test_recv_board_state_t;


void userapp_test_state_recv_cmd(uint8_t cmd);

#if(IR_TEST==1)

int eut_test_init(void);

#endif




#ifdef __cplusplus
}
#endif

#endif

