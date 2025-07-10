/*============================================================================*/
/* @file exe_hal_tc.h
 * @brief EXE HAL toolchain header.
 * @author onmicro
 * @date 2020/02
 */

#ifndef __EXE_HAL_TC_H__
#define __EXE_HAL_TC_H__

#if defined(__CORTEX_M) || defined(__ANDES_N)

  /* CMSIS headers are included already. */

#else

#if   defined ( __CC_ARM )
  #if !defined(__ASM)
  #define __ASM            __asm__                                      /*!< asm keyword for ARM Compiler          */
  #endif
  #if !defined(__INLINE)
  #define __INLINE         __inline                                   /*!< inline keyword for ARM Compiler       */
  #endif
  #if !defined(__STATIC_INLINE)
  #define __STATIC_INLINE         static __inline                                  
  #endif
  #if !defined(__PACKED)
  #define __PACKED         __packed                                  
  #endif
  #if !defined(__PACKED_GCC)
  #define __PACKED_GCC                                 
  #endif
  #if !defined(__ALIGNED)
  #define __ALIGNED(n)     __align(n)
  #endif
#elif defined ( __ICCARM__ )
  #define __ASM            __asm                                      /*!< asm keyword for IAR Compiler          */
  #define __INLINE         inline                                     /*!< inline keyword for IAR Compiler. Only available in High optimization mode! */
  #define __STATIC_INLINE  static inline
  #define __PACKED         __packed
  #define __PACKED_GCC
  #define __ALIGNED(n)     _Pragma(IAR_STR(data_alignment= ## n ##))

#elif defined ( __TMS470__ )
  #define __ASM            __asm__                                      /*!< asm keyword for TI CCS Compiler       */
  #define __STATIC_INLINE  static inline

#elif defined ( __GNUC__ ) || defined ( __ICCRISCV__ )
  #define __ASM            __asm__                                      /*!< asm keyword for GNU Compiler          */
  #define __INLINE         inline                                     /*!< inline keyword for GNU Compiler       */
  #define __STATIC_INLINE  static inline
  #define __PACKED
  #define __PACKED_GCC     __attribute__ ((packed))
  #define __ALIGNED(n)     __attribute__ ((aligned (n)))
  #define __ROOT           __attribute__ ((section (".text")))

#elif defined ( __TASKING__ )
  #define __ASM            __asm__                                      /*!< asm keyword for TASKING Compiler      */
  #define __INLINE         inline                                     /*!< inline keyword for TASKING Compiler   */
  #define __STATIC_INLINE  static inline
  #define __ALIGNED(n)

#elif defined ( __C51__ )
  #define __ASM
  #define __INLINE
  #define __STATIC_INLINE  static
  #define __PACKED
  #define __PACKED_GCC
  #define __ALIGNED(n)
  #define __ROOT

#elif defined ( __LCC__ )
  #define __ASM
  #define __INLINE
  #define __STATIC_INLINE  static
  #define __PACKED
  #define __PACKED_GCC
  #define __ALIGNED(n)
  #define __ROOT

#endif /* different toolchain */

#endif /* if defined(__ASM) */

#endif /* #ifndef __EXE_HAL_TC_H__ */

