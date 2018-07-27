#
# This Makefile is for Unix port
#

include $(RULE_DIR)/Makefile.pre

CODE_SUBDIRS = \
	src/api \
	src/core/ipv4 \
	src/netif/ppp/polarssl \
	src/lwip/protocols	\
	src/mux/cmn	\
	src/mux/lwip	\
	src/mux/http	\
	src/mux/mdns	\
	src/mux/nmos	\
	src/mux/ipCmd	\

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


LWIP_MUX_SRC += \
	src/lwip/protocols/tftpServer.c \
	src/lwip/protocols/lwiperf.c \

#	src/lwip/jsonUtils.c \

MUX_LWIP_SRC += \
	src/mux/lwip/muxLwipStart.c \
	src/mux/lwip/muxLwipUtils.c \
	src/mux/lwip/muxLwipTelnetd.c \


MDNS_SRC += \
	src/mux/mdns/muxMdns.c \
	src/mux/mdns/muxMdnsClient.c \


WEB_SRC += \
	src/mux/http/muxHttpPost.c \
	src/mux/http/muxHttpFs.c \
	src/mux/http/muxHttpConns.c \
	src/mux/http/muxHttpRequest.c \
	src/mux/http/muxHttpResponseFile.c \
	src/mux/http/muxHttpSend.c \
	src/mux/http/muxHttpWebSocket.c \
	src/mux/http/muxHttpWebPages.c \
	src/mux/http/muxHttpMain.c \


CMN_SRC += \
	src/mux/cmn/muxCmnUtils.c	\
	src/mux/cmn/muxCmnJsonUtils.c \
	src/mux/cmn/muxCmnSysConfig.c \
	src/mux/cmn/muxUploadFile.c \
	src/mux/cmn/jsmn.c \



NMOS_SRC += \
	src/mux/nmos/muxNmosHttp.c \
	src/mux/nmos/muxNmosHttpErrors.c \
	src/mux/nmos/muxNmosPost.c \
	src/mux/nmos/muxNmosNode.c \
	src/mux/nmos/muxNmosConn.c \
	src/mux/nmos/muxNmosUtils.c \
	src/mux/nmos/muxNmosInit.c \
	src/mux/nmos/muxNmosSdp.c \


IPCMD_SRC += \
	src/mux/ipCmd/muxIpCmdRequest.c \
	src/mux/ipCmd/muxIpCmdResponses.c \
	src/mux/ipCmd/muxIpCmdExecs.c \
	src/mux/ipCmd/muxIpCmdSetup.c \
	src/mux/ipCmd/muxIpCmdMain.c \
	

MUX_SRC += \
	$(MUX_LWIP_SRC) \
	$(MDNS_SRC) \
	$(WEB_SRC) \
	$(NMOS_SRC) \
	$(CMN_SRC) \
	$(IPCMD_SRC) \
	src/mux/base64.c \

#	src/lwip/frozen.c \

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

