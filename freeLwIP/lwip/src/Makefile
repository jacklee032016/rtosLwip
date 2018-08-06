# Makefile for Atmel version
# dependent on RTOS
#


include $(RULE_DIR)/Rules.mak


###################################################################
# define directory for object files
###################################################################

#	$(LWIP_HOME)/ports/sam \


CODE_SUBDIRS = \
	netif/ppp/polarssl \
	api \
	core/ipv4 \
	lwip/protocols \
	mux/cmn	\
	mux/lwip	\
	mux/http	\
	mux/mdns	\
	mux/nmos	\
	mux/ipCmd	\


# 
CFLAGS += \
	$(LWIP_FLAGS) \
	$(LWIP_HEADER) \
	$(CPACK_FLAGS) \
	$(RTOS_FLAGS) \
	$(RTOS_HEADER) \
	$(CPACK_FLAGS) \
	$(APP_HEADER) \
	


###################################################################
# define source files
###################################################################

LWIP_CORE_SRC += \
	core/def.c \
	core/dns.c \
	core/inet_chksum.c \
	core/mem.c \
	core/memp.c \
	core/netif.c \
	core/ip.c \
	core/pbuf.c \
	core/stats.c \
	core/sys.c \
	core/tcp.c \
	core/tcp_in.c \
	core/raw.c \
	core/tcp_out.c \
	core/udp.c \
	core/init.c \
	core/timeouts.c \

# timeouts.c replace timer.c
	#	src/core/timers.c \
	
LWIP_CORE_V4_SRC += \
	core/ipv4/autoip.c \
	core/ipv4/dhcp.c \
	core/ipv4/etharp.c \
	core/ipv4/icmp.c \
	core/ipv4/igmp.c \
	core/ipv4/ip4.c \
	core/ipv4/ip4_addr.c \
	core/ipv4/ip4_frag.c \

LWIP_API_SRC += \
	api/api_lib.c \
	api/api_msg.c \
	api/err.c \
	api/netbuf.c \
	api/netdb.c \
	api/tcpip.c \
	api/sockets.c \

LWIP_IF_SRC += \
	netif/ethernet.c \
	netif/ppp/polarssl/sha1.c \


LWIP_MUX_SRC += \
	lwip/protocols/tftpServer.c \
	lwip/protocols/lwiperf.c \

#	src/lwip/jsonUtils.c \

MUX_LWIP_SRC += \
	mux/lwip/muxLwipStart.c \
	mux/lwip/muxLwipUtils.c \
	mux/lwip/muxLwipTelnetd.c \


MDNS_SRC += \
	mux/mdns/muxMdns.c \
	mux/mdns/muxMdnsClient.c \


WEB_SRC += \
	mux/http/muxHttpPost.c \
	mux/http/muxHttpFs.c \
	mux/http/muxHttpConns.c \
	mux/http/muxHttpRequest.c \
	mux/http/muxHttpResponseFile.c \
	mux/http/muxHttpSend.c \
	mux/http/muxHttpWebSocket.c \
	mux/http/muxHttpWebPages.c \
	mux/http/muxHttpMain.c \


CMN_SRC += \
	mux/cmn/muxCmnUtils.c	\
	mux/cmn/muxCmnJsonUtils.c \
	mux/cmn/muxCmnSysConfig.c \
	mux/cmn/muxUploadFile.c \
	mux/cmn/jsmn.c \



NMOS_SRC += \
	mux/nmos/muxNmosHttp.c \
	mux/nmos/muxNmosHttpErrors.c \
	mux/nmos/muxNmosPost.c \
	mux/nmos/muxNmosNode.c \
	mux/nmos/muxNmosConn.c \
	mux/nmos/muxNmosUtils.c \
	mux/nmos/muxNmosInit.c \
	mux/nmos/muxNmosSdp.c \


IPCMD_SRC += \
	mux/ipCmd/muxIpCmdRequest.c \
	mux/ipCmd/muxIpCmdResponses.c \
	mux/ipCmd/muxIpCmdExecs.c \
	mux/ipCmd/muxIpCmdSetup.c \
	mux/ipCmd/muxIpCmdMain.c \
	

MUX_SRC += \
	$(MUX_LWIP_SRC) \
	$(MDNS_SRC) \
	$(WEB_SRC) \
	$(NMOS_SRC) \
	$(CMN_SRC) \
	$(IPCMD_SRC) \
	mux/base64.c \

#	lwip/frozen.c \

SRC += \
	$(LWIP_CORE_SRC) \
	$(LWIP_CORE_V4_SRC) \
	$(LWIP_API_SRC) \
	$(LWIP_IF_SRC) \
	$(LWIP_MUX_SRC) \
	$(MUX_SRC) \


TARGET_LIB:=libLwip

TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a
		 
LOCAL_ALL:=

include $(ROOT_DIR)/Makefile.post
