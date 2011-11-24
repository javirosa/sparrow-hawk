
#x86 or ARM
BUILD_TYPE = x86

ifeq ($(BUILD_TYPE), ARM) 
	CROSS_COMPILE	:= /usr/local/arm/arm-none-linux-gnueabi/bin/arm-none-linux-gnueabi-
endif

# Make variables (CC, etc...)

AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump