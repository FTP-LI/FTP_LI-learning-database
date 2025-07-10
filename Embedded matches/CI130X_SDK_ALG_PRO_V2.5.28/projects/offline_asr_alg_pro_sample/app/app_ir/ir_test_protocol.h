#ifndef _IR_TEST_PROTOCOL_H__
#define _IR_TEST_PROTOCOL_H__

#include "common_api.h"
#include "voice_module_uart_protocol.h"

#define NVDATA_ID_WAKE_NUMBER               0x70000009 /*'LIGHTINFO'*/
#if (IR_TEST == 1)
void test_board_send_protocol(unsigned char cmd);
void userapp_test_cmd(sys_msg_com_data_t *msg);
#endif
void get_flash_eut_state(void);
void set_flash_eut_state(int gt_wtn);
#endif   //_IR_TEST_PROTOCOL_H__