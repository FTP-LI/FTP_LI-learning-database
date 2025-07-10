#ifndef __OTA_PATTITION__VERIFY_H__
#define __OTA_PATTITION__VERIFY_H__

#include "stdint.h"
#include "ci130x_spiflash.h"
#include "flash_update.h"
#include "flash_manage_outside_port.h"
#include "ci_flash_data_info.h"

int check_ota_finish(void);
#endif  //__OTA_PATTITION__VERIFY_H__