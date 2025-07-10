
BUILD_DIR := build

ifndef PROJECT_NAME
# PROJECT_NAME := $(notdir $(shell pwd))
# PROJECT_NAME := $(BUILD_DIR)/program
PROJECT_NAME := $(word $(words $(subst /, ,$(dir $(abspath .)))), $(subst /, ,$(dir $(abspath .))))
else
PROJECT_NAME := $(subst /,-,$(PROJECT_NAME))
endif

ifeq ($(OS),Windows_NT)
CMDBOX := busybox
LIBS_PATH := libs
LUA := $(SDK_PATH)/tools/build-tools/bin/lua.exe
else
CMDBOX :=
LIBS_PATH := libs/linux
LUA := $(SDK_PATH)/tools/build-tools/bin/lua
endif

USER_OBJS := 
C_SRCS :=

# CC_PREFIX := riscv-none-embed-
CC_PREFIX := riscv-nuclei-elf-
CC = gcc
AS = gcc
AR = gcc-ar
LD = g++
OD = objdump
OC = objcopy
SIZE = size
OBJS := 

C_FLAGS := -DPROJECT_NAME=\"$(PROJECT_NAME)\"
S_FLAGS :=
AR_FLAGS := -D

LTO_OPTION :=
O_OPTION := -Os







