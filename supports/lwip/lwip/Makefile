#
# This Makefile is for Unix port
#

include $(RULE_DIR)/Makefile.pre

EXT_HOME:=src/exts

CODE_SUBDIRS = \
	src/api \
	src/core/ipv4 \
	src/netif/ppp/polarssl \
	$(EXT_HOME)/cmn	\
	$(EXT_HOME)/http	\
	$(EXT_HOME)/iperf	\
	$(EXT_HOME)/mdns	\
	$(EXT_HOME)/mqtt	\
	$(EXT_HOME)/nmos	\
	$(EXT_HOME)/misc	\
	$(EXT_HOME)/cmd	\
	$(EXT_HOME)/ptp/dep	\


#	src/core/ipv6 \
#	src/core/snmp \



################################################
####   source file 
################################################
LWIP_CORE_SRC += \
	src/core/init.c \
	src/core/def.c \
	src/core/dns.c \
	src/core/inet_chksum.c \
	src/core/mem.c \
	src/core/memp.c \
	src/core/netif.c \
	src/core/ip.c \
	src/core/pbuf.c \
	src/core/stats.c \
	src/core/sys.c \
	src/core/tcp.c \
	src/core/tcp_in.c \
	src/core/raw.c \
	src/core/tcp_out.c \
	src/core/udp.c \
	src/core/timeouts.c \

#	src/core/timers.c \

	
LWIP_CORE_V4_SRC += \
	src/core/ipv4/autoip.c \
	src/core/ipv4/dhcp.c \
	src/core/ipv4/etharp.c \
	src/core/ipv4/icmp.c \
	src/core/ipv4/igmp.c \
	src/core/ipv4/ip4.c \
	src/core/ipv4/ip4_addr.c \
	src/core/ipv4/ip4_frag.c \


# APIFILES: The files which implement the sequential and socket APIs.
LWIP_API_SRC += \
	src/api/api_lib.c \
	src/api/api_msg.c \
	src/api/tcpip.c \
	src/api/err.c \
	src/api/sockets.c \
	src/api/netbuf.c \
	src/api/netdb.c \
	

# NETIFFILES: Files implementing various generic network interface functions.'
LWIP_IF_SRC += \
	src/netif/ethernet.c \
	src/netif/ppp/polarssl/sha1.c \

#	src/netif/slipif.c \


EXTS_MISC_SRC += \
	$(EXT_HOME)/misc/tftpServer.c \
	$(EXT_HOME)/misc/extTelnetd.c \
	$(EXT_HOME)/misc/extPing.c \
	$(EXT_HOME)/mqtt/mqtt.c \
	$(EXT_HOME)/mqtt/mqttClient.c \

#	src/lwip/jsonUtils.c \

EXTS_MDNS_SRC += \
	$(EXT_HOME)/mdns/mdns.c \
	$(EXT_HOME)/mdns/mdnsClient.c \


EXTS_WEB_SRC += \
	$(EXT_HOME)/http/httpPost.c \
	$(EXT_HOME)/http/httpFs.c \
	$(EXT_HOME)/http/httpConns.c \
	$(EXT_HOME)/http/httpRequest.c \
	$(EXT_HOME)/http/httpResponseFile.c \
	$(EXT_HOME)/http/httpSend.c \
	$(EXT_HOME)/http/httpWebSocket.c \
	$(EXT_HOME)/http/httpWebPages.c \
	$(EXT_HOME)/http/httpMain.c \


EXTS_CMN_SRC += \
	$(EXT_HOME)/cmn/cmnUtils.c	\
	$(EXT_HOME)/cmn/cmnJsonUtils.c \
	$(EXT_HOME)/cmn/cmnSysConfig.c \
	$(EXT_HOME)/cmn/cmnUploadFile.c \
	$(EXT_HOME)/cmn/cmnLwipUtils.c \
	$(EXT_HOME)/cmn/cmnCmdLine.c \
	$(EXT_HOME)/cmn/jsmn.c \
	$(EXT_HOME)/cmn/base64.c \



EXTS_NMOS_SRC += \
	$(EXT_HOME)/nmos/nnmosHttp.c \
	$(EXT_HOME)/nmos/nnmosHttpErrors.c \
	$(EXT_HOME)/nmos/nnmosPost.c \
	$(EXT_HOME)/nmos/nnmosNode.c \
	$(EXT_HOME)/nmos/nnmosConn.c \
	$(EXT_HOME)/nmos/nnmosUtils.c \
	$(EXT_HOME)/nmos/nnmosInit.c \
	$(EXT_HOME)/nmos/nnmosSdp.c \


EXTS_CMD_SRC += \
	$(EXT_HOME)/cmd/udpCmdRequest.c \
	$(EXT_HOME)/cmd/udpCmdResponses.c \
	$(EXT_HOME)/cmd/udpCmdExecs.c \
	$(EXT_HOME)/cmd/udpCmdSetup.c \
	$(EXT_HOME)/cmd/udpCmdMain.c \
	
EXTS_IPERF_SRC += \
	$(EXT_HOME)/iperf/iperfUdp.c \
	$(EXT_HOME)/iperf/iperfLwip.c \

EXTS_LWIP_SRC += \
	$(EXTS_CMN_SRC) \
	$(EXTS_MISC_SRC) \
	$(EXTS_MDNS_SRC) \
	$(EXTS_WEB_SRC) \
	$(EXTS_NMOS_SRC) \
	$(EXTS_CMD_SRC) \
	$(EXTS_IPERF_SRC) \
	$(EXT_HOME)/extLwipStart.c \


#	src/lwip/frozen.c \

SRC += \
	$(LWIP_CORE_SRC) \
	$(LWIP_CORE_V4_SRC) \
	$(LWIP_API_SRC) \
	$(LWIP_IF_SRC) \
	$(EXTS_LWIP_SRC) \
		
 
TARGET_LIB:=libLwip

TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a
		 
LOCAL_ALL:=

include $(ROOT_DIR)/Makefile.post

