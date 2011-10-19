#################################################
#################### ATTENTION ##################
#
# DO NOT USE BLANKS INSIDE FOLDER NAME OR DIRECTORY.
# THERE MUST BE A TAB BEFORE EACH COMMAND, TRY NOT BE FOOLED BY BLANKS.
#
#################################################


#################################################

CC = gcc

#CXX = g++

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

BUILD_TYPE = Win32
#BUILD_TYPE = Linux

#################################################

COMMON_DIR   	= ./common
USERIAL_DIR  	= ./lib/userial/shared
WIN32IMPL_DIR   = ./lib/userial/link/Win32
LINUXIMPL_DIR   = ./lib/userial/link/Linux

ifeq ($(BUILD_TYPE), Win32)
	SRCDIR = $(COMMON_DIR) $(USERIAL_DIR) $(WIN32IMPL_DIR)
else
	SRCDIR = $(COMMON_DIR) $(USERIAL_DIR) $(LINUXIMPL_DIR)
endif

# VPATH = $(SRCDIR)
# vpath %.h $(SRCDIR)
# vpath %.c $(SRCDIR)
# GPATH = ./build/dist

#COMMON_SRC      = crcutil.c ioutil.c owerr.c ibsha33o.c
#COMMON_SRC      = crcutil.c ioutil.c owerr.c ibsha33o.c temp10.c findtype.c thermo21.c
COMMON_SRC      = crcutil.c ioutil.c owerr.c ibsha33o.c temp10.c findtype.c thermo21.c humutil.c mbee77.c mbscrx77.c pw77.c
USERIAL_SRC     = ds2480ut.c owllu.c ownetu.c owsesu.c owtrnu.c
WIN32IMPL_SRC   = win32lnk.c
LINUXIMPL_SRC   = linuxlnk.c

ifeq ($(BUILD_TYPE), Win32)
	SRCS = $(COMMON_SRC) $(USERIAL_SRC) $(WIN32IMPL_SRC)
else
	SRCS = $(COMMON_SRC) $(USERIAL_SRC) $(LINUXIMPL_SRC) 
endif

INCFLAG = $(foreach d, $(SRCDIR), $(join -I, $(d)))

HEADS = $(foreach d, $(SRCDIR), $(wildcard $(d)/*.h))

OBJDIR = ./build

DISTDIR = ./dist

COMMON_OBJDIR    =  $(OBJDIR)/common
USERIAL_OBJDIR   =  $(OBJDIR)/userial
WIN32IMPL_OBJDIR =  $(OBJDIR)/win32impl
LINUXIMPL_OBJDIR =  $(OBJDIR)/linuximpl

COMMON_OBJ       =  $(foreach f, $(COMMON_SRC),  $(patsubst %.c, $(COMMON_OBJDIR)/%.o, $(f)))
USERIAL_OBJ      =  $(foreach f, $(USERIAL_SRC), $(patsubst %.c, $(USERIAL_OBJDIR)/%.o, $(f)))
WIN32IMPL_OBJ    =  $(foreach f, $(WIN32IMPL_SRC),  $(patsubst %.c, $(WIN32IMPL_OBJDIR)/%.o, $(f)))
LINUXIMPL_OBJ    =  $(foreach f, $(LINUXIMPL_SRC),  $(patsubst %.c, $(LINUXIMPL_OBJDIR)/%.o, $(f)))

ifeq ($(BUILD_TYPE), Win32)
	OBJS = $(COMMON_OBJ) $(USERIAL_OBJ) $(WIN32IMPL_OBJ)
else
	OBJS = $(COMMON_OBJ) $(USERIAL_OBJ) $(LINUXIMPL_OBJ)
endif

LIBS =

#################################################

ifeq ($(BUILD_TYPE), Win32)
	TARGET = $(DISTDIR)/ibutton_win32.a
else
	TARGET = $(DISTDIR)/ibutton_linux.a
endif

$(TARGET): $(OBJS)
	$(AR) cqv  $@  $(OBJS)


# $(COMMON_OBJDIR)/%.o : %.c
# 	$(CC) -c   $(COMMON_DIR)/$<     -o $@ $(INCFLAG)
#
# $(USERIAL_OBJDIR)/%.o : %.c
# 	$(CC) -c   $(USERIAL_DIR)/$<    -o $@ $(INCFLAG)
#
# $(WIN32IMPL_OBJDIR)/%.o : %.c
# 	$(CC) -c   $(WIN32IMPL_DIR)/$<  -o $@ $(INCFLAG)


# $(OBJDIR)/ioutil.o:
# ifeq ($(findstring ioutil.c, $(COMMON_SRC)), ioutil.c)
# 	$(CC) -c $(COMMON_DIR)/ioutil.c -o $@ $(INCFLAG)
# endif
# ifeq ($(findstring ioutil.c, $(USERIAL_SRC)),ioutil.c)
# 	$(CC) -c $(USERIAL_DIR)/ioutil.c -o $@ $(INCFLAG)
# endif
# ifeq ($(findstring ioutil.c, $(WIN32IMPL_SRC)), ioutil.c)
# 	$(CC) -c $(WIN32IMPL_DIR)/ioutil.c -o $@ $(INCFLAG)
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
	

$(USERIAL_OBJDIR)/ds2480ut.o :
	$(CC) -c $(USERIAL_DIR)/ds2480ut.c	-o $@ $(INCFLAG)
$(USERIAL_OBJDIR)/owllu.o :
	$(CC) -c $(USERIAL_DIR)/owllu.c		-o $@ $(INCFLAG)
$(USERIAL_OBJDIR)/ownetu.o :
	$(CC) -c $(USERIAL_DIR)/ownetu.c	-o $@ $(INCFLAG)
$(USERIAL_OBJDIR)/owsesu.o :
	$(CC) -c $(USERIAL_DIR)/owsesu.c	-o $@ $(INCFLAG)
$(USERIAL_OBJDIR)/owtrnu.o :
	$(CC) -c $(USERIAL_DIR)/owtrnu.c	-o $@ $(INCFLAG)

$(WIN32IMPL_OBJDIR)/win32lnk.o :
	$(CC) -c $(WIN32IMPL_DIR)/win32lnk.c	-o $@ $(INCFLAG)

$(LINUXIMPL_OBJDIR)/linuxlnk.o :
	$(CC) -c $(LINUXIMPL_DIR)/linuxlnk.c 	-o $@ $(INCFLAG)

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
