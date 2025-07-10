################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
c:/Users/ATLAS/Desktop/CH32-Part/libraries/sdk/Core/core_riscv.c 

C_DEPS += \
./sdk/Core/core_riscv.d 

OBJS += \
./sdk/Core/core_riscv.o 



# Each subdirectory must supply rules for building sources it contributes
sdk/Core/core_riscv.o: c:/Users/ATLAS/Desktop/CH32-Part/libraries/sdk/Core/core_riscv.c
	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall -g -I"c:/Users/ATLAS/Desktop/CH32-Part/Libraries/doc" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/zf_components" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/sdk/Core" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/sdk/Ld" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/sdk/Peripheral" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/sdk/Startup" -I"c:/Users/ATLAS/Desktop/CH32-Part/CH32_demo/user/inc" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/zf_common" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/zf_device" -I"c:/Users/ATLAS/Desktop/CH32-Part/CH32_demo/code" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

