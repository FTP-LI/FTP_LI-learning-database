################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
c:/Users/ATLAS/Desktop/CH32-Part/CH32_demo/user/src/isr.c \
c:/Users/ATLAS/Desktop/CH32-Part/CH32_demo/user/src/main.c 

C_DEPS += \
./user_c/isr.d \
./user_c/main.d 

OBJS += \
./user_c/isr.o \
./user_c/main.o 



# Each subdirectory must supply rules for building sources it contributes
user_c/isr.o: c:/Users/ATLAS/Desktop/CH32-Part/CH32_demo/user/src/isr.c
	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall -g -I"c:/Users/ATLAS/Desktop/CH32-Part/Libraries/doc" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/zf_components" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/sdk/Core" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/sdk/Ld" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/sdk/Peripheral" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/sdk/Startup" -I"c:/Users/ATLAS/Desktop/CH32-Part/CH32_demo/user/inc" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/zf_common" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/zf_device" -I"c:/Users/ATLAS/Desktop/CH32-Part/CH32_demo/code" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
user_c/main.o: c:/Users/ATLAS/Desktop/CH32-Part/CH32_demo/user/src/main.c
	@	riscv-none-embed-gcc -march=rv32imafc -mabi=ilp32f -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -pedantic -Wunused -Wuninitialized -Wall -g -I"c:/Users/ATLAS/Desktop/CH32-Part/Libraries/doc" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/zf_components" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/sdk/Core" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/sdk/Ld" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/sdk/Peripheral" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/sdk/Startup" -I"c:/Users/ATLAS/Desktop/CH32-Part/CH32_demo/user/inc" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/zf_common" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/zf_device" -I"c:/Users/ATLAS/Desktop/CH32-Part/CH32_demo/code" -I"c:/Users/ATLAS/Desktop/CH32-Part/libraries/zf_driver" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

