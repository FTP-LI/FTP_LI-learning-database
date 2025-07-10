/*============================================================================*/
/* @file exe_int_bsp.h
 * @brief EXE link layer Bit Stream Processing internal header.
 * @spec Vol 6, Part B, 3  BIT STREAM PROCESSING
 * @author onmicro
 * @date 2020/02
 */

#ifndef __EXE_INT_BSP_H__
#define __EXE_INT_BSP_H__

#include <stdint.h>
#include <stdbool.h>

extern uint32_t exe_crc_init;

extern uint8_t *current_whiten_table;

/*
 * BLE packet CRC generator.
 */
uint32_t exe_reverse_crc24(uint32_t crc24);
uint32_t exe_packet_crc24(uint8_t *p_data, int len, uint32_t crc_init);
uint32_t dmitry_packet_crc24(uint8_t *p_data, int len, uint32_t crc_init);
uint32_t dmitry_packet_crc24_fast(uint8_t *p_data, int len, uint32_t crc_init);

/*
 * BLE data stream whiten/dewhiten processing.
 */
void exe_whiten_init(uint8_t chn_idx, uint8_t *whiten_table);
void exe_whiten_packet(uint8_t *p_xpkt_dst, uint8_t *p_pkt_src);
void dmitry_whiten_packet(uint8_t *p_xpkt_dst, uint8_t *p_pkt_src);
uint8_t exe_dewhiten_hdr_len(uint8_t *p_pkt);
int exe_dewhiten_data(uint8_t *p_data, int off_in_pkt, int data_len);

#endif /* #ifndef __EXE_INT_BSP_H__ */

