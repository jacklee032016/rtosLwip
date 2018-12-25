#
#  $
#


include $(RULE_DIR)/Rules.mak


###################################################################
# define directory for object files
###################################################################
RTOS_SUB_DIRS += \
	ucCpu/ARM-Cortex-M4/GNU/ \
	ucLib \
	ucos3/Ports/ARM-Cortex-M/Generic/GNU \
	ucos3/Source/ \
	ucLib/Ports/ARM-Cortex-M4/GNU \


CODE_SUBDIRS = \
	$(RTOS_SUB_DIRS) \
	

# 
CFLAGS += \
	$(RTOS_FLAGS) \
	$(RTOS_HEADER) \
	$(CPACK_FLAGS) \
	-Iinclude 



###################################################################
# define source files
###################################################################

RTOS_SRC += \
	ucCpu/ARM-Cortex-M4/GNU/cpu_c.c \
	ucCpu/cpu_core.c \
	ucLib/lib_ascii.c \
	ucLib/lib_math.c \
	ucLib/lib_mem.c \
	ucLib/lib_str.c \
	ucos3/Ports/ARM-Cortex-M/Generic/GNU/os_cpu_c.c \
	ucos3/Source/os_cfg_app.c \
	ucos3/Source/os_core.c \
	ucos3/Source/os_dbg.c \
	ucos3/Source/os_flag.c \
	ucos3/Source/os_int.c \
	ucos3/Source/os_mem.c \
	ucos3/Source/os_mon.c \
	ucos3/Source/os_msg.c \
	ucos3/Source/os_mutex.c \
	ucos3/Source/os_pend_multi.c \
	ucos3/Source/os_prio.c \
	ucos3/Source/os_q.c \
	ucos3/Source/os_sem.c \
	ucos3/Source/os_stat.c \
	ucos3/Source/os_task.c \
	ucos3/Source/os_tick.c \
	ucos3/Source/os_time.c \
	ucos3/Source/os_tmr.c \
	ucos3/Source/os_var.c


ASM_SRCS +=  \
	ucLib/Ports/ARM-Cortex-M4/GNU/lib_mem_a.s \
	ucCpu/ARM-Cortex-M4/GNU/cpu_a.s \
	ucos3/Ports/ARM-Cortex-M/Generic/GNU/os_cpu_a.S \


SRC += \
	$(ASM_SRCS) \
	$(RTOS_SRC) \


TARGET_LIB:=libRtos

TARGET_LIB_SHARED:=$(TARGET_LIB).so
TARGET_LIB_STATIC:=$(TARGET_LIB).a
		 
LOCAL_ALL:=

include $(RULE_DIR)/Makefile.post
