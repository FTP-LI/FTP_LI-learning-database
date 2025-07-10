/*============================================================================*/
/* @file exe_hal_cpu.h
 * @brief EXE HAL CPU header.
 * @author onmicro
 * @date 2020/02
 */

#ifndef __EXE_HAL_CPU_H__
#define __EXE_HAL_CPU_H__

#include <stdint.h>

#if defined(__EC616)
#define RAM_CODE_SECTION                __attribute__((__section__(".ramCode2")))
#elif defined(HM1001_M0)
#define RAM_CODE_SECTION                __attribute__((__section__(".ramCode")))
#else
#define RAM_CODE_SECTION
#endif

/**
 * @breif cpu endianess.
 */
#define U16_HI(a)                       (((a) >> 8) & 0xFF)
#define U16_LO(a)                       ((a) & 0xFF)

#if defined(__LCC__)
extern uint8_t little_endian_read_8(const uint8_t * buffer, int pos);
extern uint16_t little_endian_read_16(const uint8_t * buffer, int pos);
extern uint32_t little_endian_read_32(const uint8_t * buffer, int pos);
extern void     little_endian_store_8(uint8_t *buffer, uint16_t pos, uint16_t value);
extern void     little_endian_store_16(uint8_t *buffer, uint16_t pos, uint16_t value);
extern void     little_endian_store_32(uint8_t *buffer, uint16_t pos, uint32_t value);
#else
__STATIC_INLINE uint8_t little_endian_read_8(const uint8_t * buffer, int pos)
{
  return ((uint16_t) buffer[pos]);
}
__STATIC_INLINE uint16_t little_endian_read_16(const uint8_t * buffer, int pos)
{
  return (uint16_t)(((uint16_t) buffer[pos]) | (((uint16_t)buffer[(pos)+1]) << 8));
}
__STATIC_INLINE uint32_t little_endian_read_32(const uint8_t * buffer, int pos)
{
  return (uint32_t)(((uint32_t) buffer[pos]) | (((uint32_t)buffer[(pos)+1]) << 8) | (((uint32_t)buffer[(pos)+2]) << 16) | (((uint32_t)buffer[(pos)+3]) << 24));
}
__STATIC_INLINE void     little_endian_store_8(uint8_t *buffer, uint16_t pos, uint16_t value)
{
    buffer[pos++] = (uint8_t)value;
}
__STATIC_INLINE void     little_endian_store_16(uint8_t *buffer, uint16_t pos, uint16_t value)
{
    buffer[pos++] = (uint8_t)value;
    buffer[pos++] = (uint8_t)(value >> 8);
}
__STATIC_INLINE void     little_endian_store_32(uint8_t *buffer, uint16_t pos, uint32_t value)
{
    buffer[pos++] = (uint8_t)value;
    buffer[pos++] = (uint8_t)(value >> 8);
    buffer[pos++] = (uint8_t)(value >> 16);
    buffer[pos++] = (uint8_t)(value >> 24);
}
#endif /* defined(__LCC__) */

#endif /* #ifndef __EXE_HAL_CPU_H__ */

