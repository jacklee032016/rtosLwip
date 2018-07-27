#
#  $
#


include $(RULE_DIR)/Rules.mak


###################################################################
# define directory for object files
###################################################################


CODE_SUBDIRS = \
	drivers \
	hw \
	utils \
	modem \
	

# dependent on CMSIS 
CFLAGS += \
	$(BSP_HEADER) \
	-Iinclude/drivers \
	-Iinclude \
	$(LWIP_HEADER) \
	$(CPACK_FLAGS) \



###################################################################
# define source files
###################################################################

UTILS_SRC += \
	utils/rwOps.c \
	utils/sysUtils.c \


DRIVERS_SRC += \
	drivers/flashd.c \
	drivers/pio.c \
	drivers/pio_handler.c \
	drivers/pmc.c \
	drivers/spi.c \
	drivers/sysclk.c \
	drivers/twihs.c \
	drivers/uart.c \
	drivers/usart.c \
	drivers/flashd.c \

# hw lauer based on driver layer
HW_SRC += \
	hw/bspHwSystem.c \
	hw/bspHwInit.c \
	hw/bspHwI2c.c \
	hw/bspHwSpi.c \
	hw/bspHwSpiFlash.c \
	hw/bspHwEfcFlash.c \
	hw/bspHwClock.c \
	hw/bspHwRs232.c \


BSP_SRC += \
	bspCmds.c \
	bspCmdBist.c \
	bspConsole.c \
	bspConfig.c \
	bspSpiFlash.c \


MODEM_SRC += \
	modem/xyModem.c \
	modem/crc16.c \

	

SRC += \
	$(UTILS_SRC) \
	$(DRIVERS_SRC) \
	$(HW_SRC) \
	$(BSP_SRC) \
	$(MODEM_SRC) \


TARGET_LIB:=libBsp

TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a
		 
LOCAL_ALL:=

include $(ROOT_DIR)/Makefile.post
