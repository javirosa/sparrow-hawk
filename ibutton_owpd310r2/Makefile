#################################################
#################### ATTENTION ##################
#
# DO NOT USE BLANKS INSIDE FOLDER NAME OR DIRECTORY.
# THERE MUST BE A TAB BEFORE EACH COMMAND, TRY NOT BE FOOLED BY BLANKS.
#
#################################################


#################################################

#BUILD_TYPE = Win32
#BUILD_TYPE = Linux
BUILD_TYPE = S3C6410

#################################################

ifeq ($(BUILD_TYPE), Win32)
	CC = gcc
	CXX = g++
endif

#################################################

CFLAGS := -O2 -g -Wall -fmessage-length=0
# CFLAGS += -v -mwindows
# CFLAGS += -mwindows
# CFLAGS += -Iinclude

CXXFLAGS := -O2 -g -Wall -fmessage-length=0
# CXXFLAGS +=

EMPTY =
BLANK = $(EMPTY) $(EMPTY)

#################################################

COMMON_DIR   	= ./common

USERIAL_SHARE_DIR  	= ./lib/userial/shared
USERIAL_WIN32_DIR   = ./lib/userial/link/Win32
USERIAL_LINUX_DIR   = ./lib/userial/link/Linux

GENERAL_SHARE_DIR   = ./lib/general/shared
GENERAL_S3C6410_DIR = ./lib/general/link/S3C6410

ifeq ($(BUILD_TYPE), Win32)
	SRCDIR = $(COMMON_DIR) $(USERIAL_SHARE_DIR) $(USERIAL_WIN32_DIR)
endif

ifeq ($(BUILD_TYPE), Linux)
	SRCDIR = $(COMMON_DIR) $(USERIAL_SHARE_DIR) $(USERIAL_LINUX_DIR)
endif

ifeq ($(BUILD_TYPE), S3C6410)
	SRCDIR = $(COMMON_DIR) $(GENERAL_SHARE_DIR) $(GENERAL_S3C6410_DIR)
endif

# VPATH = $(SRCDIR)
# vpath %.h $(SRCDIR)
# vpath %.c $(SRCDIR)
# GPATH = ./build/dist

#COMMON_SRC      = crcutil.c ioutil.c owerr.c ibsha33o.c
#COMMON_SRC      = crcutil.c ioutil.c owerr.c ibsha33o.c temp10.c findtype.c thermo21.c
COMMON_SRC      = crcutil.c ioutil.c owerr.c ibsha33o.c temp10.c findtype.c thermo21.c humutil.c mbee77.c mbscrx77.c pw77.c
USERIAL_SHARE_SRC   = ds2480ut.c owllu.c ownetu.c owsesu.c owtrnu.c
USERIAL_WIN32_SRC   = win32lnk.c
USERIAL_LINUX_SRC   = linuxlnk.c
GENERAL_SHARE_SRC   = ownet.c owtran.c
GENERAL_S3C6410_SRC   = s3c6410lnk.c s3c6410ses.c

ifeq ($(BUILD_TYPE), Win32)
	SRCS = $(COMMON_SRC) $(USERIAL_SHARE_SRC) $(USERIAL_WIN32_SRC)
endif

ifeq ($(BUILD_TYPE), Linux)
	SRCS = $(COMMON_SRC) $(USERIAL_SHARE_SRC) $(USERIAL_LINUX_SRC) 
endif

ifeq ($(BUILD_TYPE), S3C6410)
	SRCS = $(COMMON_SRC) $(GENERAL_SHARE_SRC) $(GENERAL_S3C6410_SRC) 
endif

INCFLAG = $(foreach d, $(SRCDIR), $(join -I, $(d)))

HEADS = $(foreach d, $(SRCDIR), $(wildcard $(d)/*.h))

OBJDIR = ./build

DISTDIR = ./dist

COMMON_OBJDIR    =  $(OBJDIR)/common
USERIAL_SHARE_OBJDIR   =  $(OBJDIR)/userial_shared
USERIAL_WIN32_OBJDIR =  $(OBJDIR)/userial_win32impl
USERIAL_LINUX_OBJDIR =  $(OBJDIR)/userial_linuximpl
GENERAL_SHARE_OBJDIR =  $(OBJDIR)/general_shared
GENERAL_S3C6410_ORJDIR =  $(OBJDIR)/general_s3c6410

COMMON_OBJ       =  $(foreach f, $(COMMON_SRC),  $(patsubst %.c, $(COMMON_OBJDIR)/%.o, $(f)))
USERIAL_SHARE_OBJ    =  $(foreach f, $(USERIAL_SHARE_SRC), $(patsubst %.c, $(USERIAL_SHARE_OBJDIR)/%.o, $(f)))
USERIAL_WIN32_OBJ    =  $(foreach f, $(USERIAL_WIN32_SRC),  $(patsubst %.c, $(USERIAL_WIN32_OBJDIR)/%.o, $(f)))
USERIAL_LINUX_OBJ    =  $(foreach f, $(USERIAL_LINUX_SRC),  $(patsubst %.c, $(USERIAL_LINUX_OBJDIR)/%.o, $(f)))
GENERAL_SHARE_OBJ    =  $(foreach f, $(GENERAL_SHARE_SRC),  $(patsubst %.c, $(GENERAL_SHARE_OBJDIR)/%.o, $(f)))
GENERAL_S3C6410_OBJ    =  $(foreach f, $(GENERAL_S3C6410_SRC),  $(patsubst %.c, $(GENERAL_S3C6410_OBJDIR)/%.o, $(f)))

ifeq ($(BUILD_TYPE), Win32)
	OBJS = $(COMMON_OBJ) $(USERIAL_SHARE_OBJ) $(USERIAL_WIN32_OBJ)
endif

ifeq ($(BUILD_TYPE), Linux)
	OBJS = $(COMMON_OBJ) $(USERIAL_SHARE_OBJ) $(USERIAL_LINUX_OBJ)
endif

ifeq ($(BUILD_TYPE), S3C6410)
	OBJS = $(COMMON_OBJ) $(GENERAL_SHARE_OBJ) $(GENERAL_S3C641_OBJ)
endif

LIBS =

#################################################

ifeq ($(BUILD_TYPE), Win32)
	TARGET = $(DISTDIR)/ibutton_userial_win32.a
endif

ifeq ($(BUILD_TYPE), Linux)
	TARGET = $(DISTDIR)/ibutton_userial_linux.a
endif

ifeq ($(BUILD_TYPE), S3C6410)
	TARGET = $(DISTDIR)/ibutton_general_s3c6410.a
endif

$(TARGET): $(OBJS)
	$(AR) cqv  $@  $(OBJS)


# $(COMMON_OBJDIR)/%.o : %.c
# 	$(CC) -c   $(COMMON_DIR)/$<     -o $@ $(INCFLAG)
#
# $(USERIAL_SHARE_OBJDIR)/%.o : %.c
# 	$(CC) -c   $(USERIAL_SHARE_DIR)/$<    -o $@ $(INCFLAG)
#
# $(USERIAL_WIN32_OBJDIR)/%.o : %.c
# 	$(CC) -c   $(USERIAL_WIN32_DIR)/$<  -o $@ $(INCFLAG)


# $(OBJDIR)/ioutil.o:
# ifeq ($(findstring ioutil.c, $(COMMON_SRC)), ioutil.c)
# 	$(CC) -c $(COMMON_DIR)/ioutil.c -o $@ $(INCFLAG)
# endif
# ifeq ($(findstring ioutil.c, $(USERIAL_SHARE_SRC)),ioutil.c)
# 	$(CC) -c $(USERIAL_SHARE_DIR)/ioutil.c -o $@ $(INCFLAG)
# endif
# ifeq ($(findstring ioutil.c, $(USERIAL_WIN32_SRC)), ioutil.c)
# 	$(CC) -c $(USERIAL_WIN32_DIR)/ioutil.c -o $@ $(INCFLAG)
# endif
# ifeq ($(findstring ioutil.c, $(APP_SRC)), ioutil.c)
# 	$(CC) -c $(APP_DIR)/ioutil.c -o $@ $(INCFLAG)
# endif


# $(OBJDIR)/%.o:  %.c  $(HEADS)
#	$(CC) -c $< -o $@ $(INCFLAG)


$(COMMON_OBJDIR)/crcutil.o :
	$(CC) -c $(COMMON_DIR)/crcutil.c	-o $@ $(INCFLAG)
$(COMMON_OBJDIR)/ioutil.o :
	$(CC) -c $(COMMON_DIR)/ioutil.c		-o $@ $(INCFLAG)
$(COMMON_OBJDIR)/owerr.o :
	$(CC) -c $(COMMON_DIR)/owerr.c		-o $@ $(INCFLAG)
$(COMMON_OBJDIR)/ibsha33o.o :
	$(CC) -c $(COMMON_DIR)/ibsha33o.c	-o $@ $(INCFLAG)
	
	
$(COMMON_OBJDIR)/temp10.o :
	$(CC) -c $(COMMON_DIR)/temp10.c		-o $@ $(INCFLAG)
$(COMMON_OBJDIR)/findtype.o :
	$(CC) -c $(COMMON_DIR)/findtype.c	-o $@ $(INCFLAG)
$(COMMON_OBJDIR)/thermo21.o :
	$(CC) -c $(COMMON_DIR)/thermo21.c	-o $@ $(INCFLAG)
	
	
$(COMMON_OBJDIR)/humutil.o :
	$(CC) -c $(COMMON_DIR)/humutil.c		-o $@ $(INCFLAG)
$(COMMON_OBJDIR)/mbee77.o :
	$(CC) -c $(COMMON_DIR)/mbee77.c	-o $@ $(INCFLAG)
$(COMMON_OBJDIR)/mbscrx77.o :
	$(CC) -c $(COMMON_DIR)/mbscrx77.c	-o $@ $(INCFLAG)
$(COMMON_OBJDIR)/pw77.o :
	$(CC) -c $(COMMON_DIR)/pw77.c	-o $@ $(INCFLAG)
	
	
$(USERIAL_SHARE_OBJDIR)/ds2480ut.o :
	$(CC) -c $(USERIAL_SHARE_DIR)/ds2480ut.c	-o $@ $(INCFLAG)
$(USERIAL_SHARE_OBJDIR)/owllu.o :
	$(CC) -c $(USERIAL_SHARE_DIR)/owllu.c		-o $@ $(INCFLAG)
$(USERIAL_SHARE_OBJDIR)/ownetu.o :
	$(CC) -c $(USERIAL_SHARE_DIR)/ownetu.c	-o $@ $(INCFLAG)
$(USERIAL_SHARE_OBJDIR)/owsesu.o :
	$(CC) -c $(USERIAL_SHARE_DIR)/owsesu.c	-o $@ $(INCFLAG)
$(USERIAL_SHARE_OBJDIR)/owtrnu.o :
	$(CC) -c $(USERIAL_SHARE_DIR)/owtrnu.c	-o $@ $(INCFLAG)


$(USERIAL_WIN32_OBJDIR)/win32lnk.o :
	$(CC) -c $(USERIAL_WIN32_DIR)/win32lnk.c	-o $@ $(INCFLAG)
$(USERIAL_LINUX_OBJDIR)/linuxlnk.o :
	$(CC) -c $(USERIAL_LINUX_DIR)/linuxlnk.c 	-o $@ $(INCFLAG)
	

$(GENERAL_SHARE_OBJDIR)/ownet.o :
	$(CC) -c $(GENERAL_SHARE_DIR)/ownet.c	-o $@ $(INCFLAG)
$(GENERAL_SHARE_OBJDIR)/owtran.o :
	$(CC) -c $(GENERAL_SHARE_DIR)/owtran.c	-o $@ $(INCFLAG)
	
	
$(GENERAL_S3C6410_OBJDIR)/s3c6410lnk.o :
	$(CC) -c $(GENERAL_S3C6410_DIR)/s3c6410lnk.c	-o $@ $(INCFLAG)
$(GENERAL_S3C6410_OBJDIR)/s3c6410ses.o :
	$(CC) -c $(GENERAL_S3C6410_DIR)/s3c6410ses.c	-o $@ $(INCFLAG)


#################################################

.PHONY: all clean test

all:  $(TARGET)

clean:
	rm -f  $(OBJS) $(TARGET)

test:
	@echo TARGET:    	$(TARGET)
	@echo SRCS:    		$(SRCS)
	@echo INCFLAG: 		$(INCFLAG)
	@echo HEADS:  		$(HEADS)
	@echo OBJS:   		$(OBJS)
	@echo end
