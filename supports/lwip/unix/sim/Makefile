#
# $Author$
#


include $(RULE_DIR)/Makefile.pre

CODE_SUBDIRS = \
	apps/httpserver	\
	apps/chargen \
	apps/udpecho	\
	apps/tcpecho	\
	apps/shell	\
	apps/netio \
	apps/ping \
	apps/socket_examples \
	apps1/httpd \
	apps/rtp \
	apps1/sntp \
	apps1/tftp \
	apps1/netbiosns \


CFLAGS += -Iapps/httpserver_raw
CFLAGS += -Iapps/udpecho
CFLAGS += -Iapps/tcpecho
CFLAGS += -Iapps/shell
CFLAGS += -Iapps/netio
CFLAGS += -Iapps/ping
CFLAGS += -Iapps/socket_examples
CFLAGS += -Iapps/rtp

SRC:= \
	apps/chargen/chargen.c \
	apps/udpecho/udpecho.c \
	apps/netio/netio.c \
	apps/rtp/rtp.c \
	apps1/sntp/sntp.c \
	apps1/sntp/sntp.c \
	apps1/netbiosns/netbiosns.c \

# move to lwIP/src/lwip/protocols
#	apps1/tftp/tftp_server.c \

	
NOT_USED += \
	apps/udpecho_raw/udpecho_raw.c \
	apps/tcpecho_raw/tcpecho_raw.c \
	apps/smtp/smtp.c \
	apps/snmp_private_mib/lwip_prvmib.c \
	addons/tcp_isn/tcp_isn.c	\
	apps/ping/ping.c \
	apps/shell/shell.c \
	apps/httpserver/httpserver-netconn.c \
	apps/socket_examples/socket_examples.c \
	
	
		
OTHERS += \
		simhost.c \
		simnode.c \
		simrouter.c \


LOCAL_ALL := \
		simhost \
		simnode \
		simrouter \
		

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
	$(MOVE) $@ $(BIN_DIR)/usr/bin  

simnode:$(OBJ_DIR)/simnode.o $(OBJS)
	$(CC) -s -Wl,--gc-sections -o $@ $^ $(LDFLAGS)
	$(MOVE) $@ $(BIN_DIR)/usr/bin  

simrouter:$(OBJ_DIR)/simrouter.o $(OBJS)
	$(CC) -s -Wl,--gc-sections -o $@ $^ $(LDFLAGS)
	$(MOVE) $@ $(BIN_DIR)/usr/bin  

