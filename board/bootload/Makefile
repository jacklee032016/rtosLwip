# $Id$


# configuration options for build this project
NAME=bootload

ROOT_DIR:=$(shell pwd)
RULE_DIR:=$(ROOT_DIR)
#/../

ARCH=arm
EXTENSION=

CPU_E70Q20=YES
CPU_E70Q20=


DEMO_TEST=NO


export	ROOT_DIR
export	RULE_DIR
export	EXTENSION
export	ARCH
export	CPU_E70Q20





#SUBDIRS += asf
#SUBDIRS += thirdparty
SUBDIRS += src

include $(RULE_DIR)/Rules.mak

#OBJECTS = $(wildcard $(OBJ_DIR)/*.o)
#OBJECTS = $(patsubst $(SOURCEDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))
OBJECTS := $(shell find $(OBJ_DIR) -name '*.o')

all: BUILD_OBJS

#	make $(EXE)

#	OBJECTS:=$(shell find $(OBJ_DIR) -name '*.o')

$(EXE):
	$(CC) -mthumb --specs=nano.specs -Wl,--gc-sections -mcpu=cortex-m7  -lm -Wl,--entry=Reset_Handler -T $(LINK_SCRIPT) $(OBJECTS) -o $(EXE).elf 
	$(OBJCOPY) -O binary $(EXE).elf $@
	$(MOVE) $(EXE).elf $(BIN_DIR)/sbin
	$(MOVE) $@ $(BIN_DIR)/sbin


#	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) $@ ) ; done
BUILD_OBJS:
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) all ) ; done

clean: 
	rm -rf $(NAME)_*
	rm -rf pkg
	rm -rf $(BIN_DIR) 
	rm -rf $(OBJ_DIR) 
	rm -rf *.log

#	- find . -name $(OBJ_DIR) -prune -exec rm -r -f {} \;
	
# all	
install:
	@$(SHELL) $(ROOT_DIR)/buildver.sh $(ROOT_DIR)
	@$(SHELL) $(ROOT_DIR)/install.sh $(ROOT_DIR)/Linux.bin.$(ARCH) $(ROOT_DIR)/releases  

#	@$(SHELL) $(ROOT_DIR)/buildver.sh $(ROOT_DIR)

package:clean
	cd ..; tar -cvjf $(NAME).$(BUILDTIME).tar.bz2 $(NAME)

	