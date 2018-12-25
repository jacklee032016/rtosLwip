#
#  $
#


include $(RULE_DIR)/Rules.mak


LWIP_SRC:=src
EXT_HOME:=$(LWIP_SRC)/exts

###################################################################
# define directory for object files
###################################################################

#	$(LWIP_HOME)/ports/sam \


CODE_SUBDIRS = \
	port/same \
	$(LWIP_SRC)/netif/ppp/polarssl \
	$(LWIP_SRC)/api \
	$(LWIP_SRC)/core/ipv4 \
	$(EXT_HOME)/client	\
	$(EXT_HOME)/cmn	\
	$(EXT_HOME)/http	\
	$(EXT_HOME)/iperf	\
	$(EXT_HOME)/mdns	\
	$(EXT_HOME)/mqtt	\
	$(EXT_HOME)/nmos	\
	$(EXT_HOME)/misc	\
	$(EXT_HOME)/cmd	\
	$(EXT_HOME)/ptp/dep	\


# 
CFLAGS += \
	$(LWIP_FLAGS) \
	$(LWIP_HEADER) \
	$(CPACK_FLAGS) \
	$(RTOS_HEADER) \
	$(BSP_HEADER) \
	-Iinclude \
	-I. \
	$(UCOS_HEADER) \
	


###################################################################
# define source files
###################################################################

LWIP_CORE_SRC += \
	$(LWIP_SRC)/core/def.c \
	$(LWIP_SRC)/core/dns.c \
	$(LWIP_SRC)/core/inet_chksum.c \
	$(LWIP_SRC)/core/mem.c \
	$(LWIP_SRC)/core/memp.c \
	$(LWIP_SRC)/core/netif.c \
	$(LWIP_SRC)/core/ip.c \
	$(LWIP_SRC)/core/pbuf.c \
	$(LWIP_SRC)/core/stats.c \
	$(LWIP_SRC)/core/sys.c \
	$(LWIP_SRC)/core/tcp.c \
	$(LWIP_SRC)/core/tcp_in.c \
	$(LWIP_SRC)/core/raw.c \
	$(LWIP_SRC)/core/tcp_out.c \
	$(LWIP_SRC)/core/udp.c \
	$(LWIP_SRC)/core/init.c \
	$(LWIP_SRC)/core/timeouts.c \

# timeouts.c replace timer.c
	#	$(LWIP_SRC)/core/timers.c \
	
LWIP_CORE_V4_SRC += \
	$(LWIP_SRC)/core/ipv4/autoip.c \
	$(LWIP_SRC)/core/ipv4/dhcp.c \
	$(LWIP_SRC)/core/ipv4/etharp.c \
	$(LWIP_SRC)/core/ipv4/icmp.c \
	$(LWIP_SRC)/core/ipv4/igmp.c \
	$(LWIP_SRC)/core/ipv4/ip4.c \
	$(LWIP_SRC)/core/ipv4/ip4_addr.c \
	$(LWIP_SRC)/core/ipv4/ip4_frag.c \

LWIP_API_SRC += \
	$(LWIP_SRC)/api/api_lib.c \
	$(LWIP_SRC)/api/api_msg.c \
	$(LWIP_SRC)/api/err.c \
	$(LWIP_SRC)/api/netbuf.c \
	$(LWIP_SRC)/api/netdb.c \
	$(LWIP_SRC)/api/tcpip.c \
	$(LWIP_SRC)/api/sockets.c \

LWIP_IF_SRC += \
	$(LWIP_SRC)/netif/ethernet.c \
	$(LWIP_SRC)/netif/ppp/polarssl/sha1.c \


EXTS_MISC_SRC += \
	$(EXT_HOME)/misc/tftpServer.c \
	$(EXT_HOME)/misc/extTelnetd.c \
	$(EXT_HOME)/misc/extPing.c \

#	src/lwip/jsonUtils.c \

EXTS_MDNS_SRC += \
	$(EXT_HOME)/mdns/mdns.c \
	$(EXT_HOME)/mdns/mdnsClient.c \
	$(EXT_HOME)/mqtt/mqtt.c \
	$(EXT_HOME)/mqtt/mqttClient.c \


EXTS_WEB_SRC += \
	$(EXT_HOME)/http/httpPost.c \
	$(EXT_HOME)/http/httpFs.c \
	$(EXT_HOME)/http/httpConns.c \
	$(EXT_HOME)/http/httpRequest.c \
	$(EXT_HOME)/http/httpResponseFile.c \
	$(EXT_HOME)/http/httpSend.c \
	$(EXT_HOME)/http/httpWebSocket.c \
	$(EXT_HOME)/http/httpWebPages.c \
	$(EXT_HOME)/http/httpFsm.c \
	$(EXT_HOME)/http/httpSdpApi.c \
	$(EXT_HOME)/http/httpParseData.c \
	$(EXT_HOME)/http/httpMain.c \
	$(EXT_HOME)/client/httpClientFsm.c \
	$(EXT_HOME)/client/httpClientMain.c \


EXTS_CMN_SRC += \
	$(EXT_HOME)/cmn/cmnUtils.c	\
	$(EXT_HOME)/cmn/cmnJsonUtils.c \
	$(EXT_HOME)/cmn/cmnLwipUtils.c \

#	$(EXT_HOME)/cmn/base64.c \
#	$(EXT_HOME)/cmn/cmnHttpUtils.c \
#	$(EXT_HOME)/cmn/jsmn.c \
#	$(EXT_HOME)/cmn/cmnCmdLine.c \
#	$(EXT_HOME)/cmn/cmnUploadFile.c \
#	$(EXT_HOME)/cmn/cmnSysConfig.c \


EXTS_NMOS_SRC += \
	$(EXT_HOME)/nmos/nnmosHttp.c \
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

EXTS_PTPD_SRC += \
	$(EXT_HOME)/ptp/ptpArith.c \
	$(EXT_HOME)/ptp/ptpBmc.c \
	$(EXT_HOME)/ptp/ptpMain.c \
	$(EXT_HOME)/ptp/ptpProtocol.c \
	$(EXT_HOME)/ptp/ptpMsg.c \
	$(EXT_HOME)/ptp/dep/ptpNet.c \
	$(EXT_HOME)/ptp/dep/ptpServo.c \
	$(EXT_HOME)/ptp/dep/ptpSysTime.c \
	$(EXT_HOME)/ptp/dep/ptpTimer.c \
	$(EXT_HOME)/ptp/extPtp.c \


EXTS_LWIP_SRC += \
	$(EXTS_MISC_SRC) \
	$(EXTS_WEB_SRC) \
	$(EXTS_CMD_SRC) \
	$(EXT_HOME)/extFsm.c \
	$(EXT_HOME)/extMediaTask.c \
	$(EXT_HOME)/extSys.c \
	$(EXT_HOME)/extSysCtrl.c \

#	$(EXTS_IPERF_SRC) \
#	$(EXTS_MDNS_SRC) \
#	$(EXTS_NMOS_SRC) \
#	$(EXTS_PTPD_SRC) \

#	lwip/frozen.c \

SRC += \
	$(LWIP_CORE_SRC) \
	$(LWIP_CORE_V4_SRC) \
	$(LWIP_API_SRC) \
	$(LWIP_SRC)/netif/ethernet.c \
	$(EXTS_CMN_SRC) \
	$(EXT_HOME)/extLwipStart.c \
	port/same/sys_arch.c \
	lwipStart.c \
	cmnSysConfig.c \


#	$(LWIP_IF_SRC) \
#	$(EXTS_LWIP_SRC) \


TARGET_LIB:=libLwip

TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a
		 
LOCAL_ALL:=

include $(RULE_DIR)/Makefile.post
