#################################################
#################### ATTENTION ##################
#
# DO NOT USE BLANKS INSIDE FOLDER NAME OR DIRECTORY.
# THERE MUST BE A TAB BEFORE EACH COMMAND, TRY NOT BE FOOLED BY BLANKS.
#
#################################################

#APPNAME = shaapp
#APPNAME = temp
#APPNAME = thermo
#APPNAME = atodtst
APPNAME = humalog

#################################################

#BUILD_TYPE = Win32
BUILD_TYPE = Linux

#################################################

ifeq ($(BUILD_TYPE), Win32)
	CC = gcc
	CXX = g++
endif

ifeq ($(BUILD_TYPE), S3C6410) 

    ARCH			:= arm
    CROSS_COMPILE	:= /usr/local/arm/arm-none-linux-gnueabi/bin/arm-none-linux-gnueabi-
    
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
    AWK			= awk
    GENKSYMS	= scripts/genksyms/genksyms
    DEPMOD		= /sbin/depmod
    KALLSYMS	= scripts/kallsyms
    PERL		= perl
    CHECK		= sparse
    
endif




#################################################

CFLAGS := -O2 -g -Wall -fmessage-length=0
ifeq ($(BUILD_TYPE), Win32)
	CFLAGS += -v -mwindows
endif

# CFLAGS += -mwindows
# CFLAGS += -Iinclude

CXXFLAGS := -O2 -g -Wall -fmessage-length=0
# CXXFLAGS +=

EMPTY =
BLANK = $(EMPTY) $(EMPTY)

#################################################

APP_DIR 		= $(APPNAME)
COMMON_DIR   	= ../common
USERIAL_DIR  	= ../lib/userial/shared
WIN32IMPL_DIR   = ../lib/userial/link/Win32
LINUXIMPL_DIR   = ../lib/userial/link/Linux

ifeq ($(BUILD_TYPE), Win32)
	SRCDIR  = $(APP_DIR) $(COMMON_DIR) $(USERIAL_DIR) $(WIN32IMPL_DIR)
else
	SRCDIR  = $(APP_DIR) $(COMMON_DIR) $(USERIAL_DIR) $(LINUXIMPL_DIR)
endif

# VPATH = $(SRCDIR)
# vpath %.h $(SRCDIR)
# vpath %.c $(SRCDIR)
# GPATH = build/dist

############################################

ifeq ($(APPNAME), shaapp)
	APP_SRC = shaapp.c
endif

ifeq ($(APPNAME), temp)
	APP_SRC = temp.c
endif

ifeq ($(APPNAME), thermo)
	#APP_SRC = thermodl.c 
	APP_SRC = thermoms.c
endif

ifeq ($(APPNAME), atodtst)
	APP_SRC = atodtst.c
endif

ifeq ($(APPNAME), humalog)
	APP_SRC = humalog.c
endif

############################################

SRCS    = $(APP_SRC)

INCFLAG = $(foreach d, $(SRCDIR), $(join -I, $(d)))

HEADS   = $(foreach d, $(SRCDIR), $(wildcard $(d)/*.h))

OBJDIR = ../build/$(APPNAME)

OBJS = $(foreach f, $(SRCS), $(patsubst %.c, $(OBJDIR)/%.o, $(f)))

ifeq ($(BUILD_TYPE), Win32)
	LIBS = ../dist/ibutton_win32.a
else
	LIBS = ../dist/ibutton_linux.a
endif

DISTDIR = ../dist

#################################################

TARGET = $(DISTDIR)/$(APPNAME).exe

$(TARGET):	$(OBJS)
	$(CC) -o $@ $^ $(LIBS)
#	$(CC) -o $(TARGET) $(OBJS) $(LIBS)

$(OBJDIR)/%.o:  $(APP_DIR)/%.c
	$(CC) -c $< -o $@ $(INCFLAG)

# $(OBJDIR)/shaapp.o :
# 	$(CC) -c $(APP_DIR)/shaapp.c -o $@ $(INCFLAG)


#################################################

.PHONY: all clean test

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

test:   $(OBJS)
	@echo TARGET:    	$(TARGET)
	@echo SRCS:    		$(SRCS)
	@echo INCFLAG: 		$(INCFLAG)
	@echo HEADS:  		$(HEADS)
	@echo OBJS:   		$(OBJS)
	@echo end
