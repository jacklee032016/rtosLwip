#
# $Author$
#


include $(RULE_DIR)/Makefile.pre

CODE_SUBDIRS = \
	apps/chargen \
	apps/netio \
	apps/rtp \
	apps1/sntp \
	apps1/tftp \
	apps1/netbiosns \


CFLAGS += -Iapps/netio
CFLAGS += -Iapps/rtp

SRC:= \
	apps/chargen/chargen.c \
	apps/netio/netio.c \
	apps/rtp/rtp.c \
	apps1/sntp/sntp.c \
	apps1/sntp/sntp.c \
	apps1/netbiosns/netbiosns.c \

# move to lwIP/src/lwip/protocols
#	apps1/tftp/tftp_server.c \

	
NOT_USED += \
	apps/smtp/smtp.c \
	apps/snmp_private_mib/lwip_prvmib.c \
	addons/tcp_isn/tcp_isn.c	\
	
		
OTHERS += \
		simhost.c \

NOT_USED += \
		simnode.c \
		simrouter.c \


LOCAL_ALL := \
		simhost \

#		simnode \
#		simrouter \
		

include $(ROOT_DIR)/Makefile.post

LDFLAGS := -L$(BIN_DIR)/lib 

ifeq ($(ARCH),arm)
LDFLAGS += $(HI_LIB_PATHS) 
else
LDFLAGS += -lpthread -ldl
endif

LDFLAGS += -lLwip -lLwipUnix -lz -lpthread -lm


simhost:$(OBJ_DIR)/simhost.o $(OBJS)
	$(CC) -s -Wl,--gc-sections -o $@ $^ $(LDFLAGS) 
	$(MOVE) $@ $(BIN_DIR)/bin  

#simnode:$(OBJ_DIR)/simnode.o $(OBJS)
#	$(CC) -s -Wl,--gc-sections -o $@ $^ $(LDFLAGS)
#	$(MOVE) $@ $(BIN_DIR)/bin  

#simrouter:$(OBJ_DIR)/simrouter.o $(OBJS)
#	$(CC) -s -Wl,--gc-sections -o $@ $^ $(LDFLAGS)
#	$(MOVE) $@ $(BIN_DIR)/bin  

