#
#  $
#


include $(RULE_DIR)/Rules.mak

###################################################################
# define directory for object files
###################################################################


RTK_DIRS += \
	asicdrv/basic \

CODE_SUBDIRS = \
	$(RTK_DIRS) \
	

# 
CFLAGS += \
	$(RTK_FLAGS) \
	$(RTK_HEADER) \
	$(CPACK_FLAGS) \



###################################################################
# define source files
###################################################################

ifeq ($(CPU_E70Q20),YES)
NET_BSP_SRC += \
	asicdrv/basic/rtl8307h_regField_list.c \
	asicdrv/basic/rtl8307h_reg_list.c \
	asicdrv/basic/rtl8307h_tableField_list.c \
	asicdrv/basic/rtl8307h_table_list.c \
	asicdrv/rtl8307h_asicdrv.c \
	asicdrv/rtl8307h_asicdrv_acl.c \
	asicdrv/rtl8307h_asicdrv_dot1x.c \
	asicdrv/rtl8307h_asicdrv_gpio.c \
	asicdrv/rtl8307h_asicdrv_igmp.c \
	asicdrv/rtl8307h_asicdrv_lock.c \
	asicdrv/rtl8307h_asicdrv_lut.c \
	asicdrv/rtl8307h_asicdrv_mem_ctrl.c \
	asicdrv/rtl8307h_asicdrv_mib.c \
	asicdrv/rtl8307h_asicdrv_misc.c \
	asicdrv/rtl8307h_asicdrv_phy.c \
	asicdrv/rtl8307h_asicdrv_port.c \
	asicdrv/rtl8307h_asicdrv_rma.c \
	asicdrv/rtl8307h_asicdrv_svlan.c \
	asicdrv/rtl8307h_asicdrv_vlan.c \
	asicdrv/rtl8307h_asicdrv_wol.c \
	rtk_api.c \
	rtk_error.c \
	rtk_irom_init.c 
	
else

endif

SRC += \
	$(NET_BSP_SRC) \


TARGET_LIB:=libRtk

TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a
		 
LOCAL_ALL:=

include $(ROOT_DIR)/Makefile.post
