#include <stdint.h>
#include <stdbool.h>
#include <ci130x_system.h>
#include <string.h>
#include <stdlib.h>
#include "ci_flash_data_info.h"
#include "flash_manage_outside_port.h"
#include "user_config.h"
#include "ci130x_spiflash.h"
#include "ci_log.h"
#include "sdk_default_config.h"
uint32_t ota_aes_info_addr_func(void)
{
#if CI_OTA_ENABLE
    return 0x7000;
#else
    return 0x3000;
#endif
}

void ota_aes_msg_check(void)
{
#if CI_OTA_ENABLE
    //写ota信息标志位
    cias_ota_flag_t cias_ota_flag_read = {0};
    post_read_flash((char *)&cias_ota_flag_read,CIAS_FLASH_IMAGE_UPGRADE_FLAG_ADDR,sizeof(cias_ota_flag_read));
    if((cias_ota_flag_read.cias_ota_chip_type != CI_CHIP_TYPE) 
        || (cias_ota_flag_read.cias_ota_uart_port != CI_OTA_UART_NUMBER)
        || (cias_ota_flag_read.cias_ota_baud != CI_OTA_UART_BAUD))
    {
        cias_ota_flag_t cias_ota_flag = {0};
        cias_ota_flag.cias_ota_chip_type = CI_CHIP_TYPE;
        cias_ota_flag.cias_ota_uart_port = CI_OTA_UART_NUMBER;
        cias_ota_flag.cias_ota_baud = CI_OTA_UART_BAUD;
        post_erase_flash(CIAS_FLASH_IMAGE_UPGRADE_FLAG_ADDR,4096);
        post_write_flash((char *)&cias_ota_flag,CIAS_FLASH_IMAGE_UPGRADE_FLAG_ADDR,sizeof(cias_ota_flag));
        post_read_flash((char *)&cias_ota_flag_read,CIAS_FLASH_IMAGE_UPGRADE_FLAG_ADDR,sizeof(cias_ota_flag_read));
        if(RETURN_ERR == flash_init(QSPI0, ENABLE))
        {
            mprintf("flash reinit error\n");
        }
    }
#endif
}