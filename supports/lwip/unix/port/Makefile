#
# This Makefile is for Unix port
#

include $(RULE_DIR)/Makefile.pre

CODE_SUBDIRS = \
	netif \


################################################
####   source file 
################################################

# ARCHFILES: Architecture specific files.
UNIX_PORT_SRC += \
	sys_arch.c \
	perf.c \
	simCmds.c \
	netif/tapif.c \

NOT_USED += \
	netif/tunif.c \
	netif/unixif.c \
	netif/list.c \
	netif/tcpdump.c \
	netif/delif.c \
	netif/sio.c \
	netif/fifo.c \


SRC += \
	$(UNIX_PORT_SRC) \
		
 
TARGET_LIB:=libLwipUnix

TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a
		 
LOCAL_ALL:=

include $(ROOT_DIR)/Makefile.post

