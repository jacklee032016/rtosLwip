# $Id$


# configuration options for build this project
NAME=an767

ROOT_DIR:=$(shell pwd)
RULE_DIR:=$(ROOT_DIR)/

ARCH=arm
EXTENSION=


export	ROOT_DIR
export	RULE_DIR
export	EXTENSION
export	ARCH


include $(RULE_DIR)/Rules.mak


SUBDIRS += libs/os3
SUBDIRS += libs/bsp/src
SUBDIRS += app/uart

SUBDIRS += $(OS_DIRS)

#OBJECTS = $(wildcard $(OBJ_DIR)/*.o)
#OBJECTS = $(patsubst $(SOURCEDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))
OBJECTS := $(shell find $(OBJ_DIR) -name '*.o')

all: BUILD_OBJS


#		$(ROOT_DIR)/check.awk



#	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) $@ ) ; done
BUILD_OBJS:
	for i in $(SUBDIRS) ; do ( cd $$i && $(MAKE) all ) ; done

clean: 
	rm -rf $(NAME)_*
	rm -rf pkg
	rm -rf $(OBJ_DIR) 
	- find . -name $(OBJ_DIR) -prune -exec rm -r -f {} \;
	- find . -name a.out -prune -exec rm -r -f {} \;
	rm -rf BIN
	rm -rf *.log

	
# all	
install:
	@$(SHELL) $(ROOT_DIR)/buildver.sh $(ROOT_DIR)
	@$(SHELL) $(ROOT_DIR)/install.sh $(ROOT_DIR)/Linux.bin.$(ARCH) $(ROOT_DIR)/releases  

#	@$(SHELL) $(ROOT_DIR)/buildver.sh $(ROOT_DIR)

package:clean
	cd ..; tar -cvjf $(NAME).$(BUILDTIME).tar.bz2 $(NAME)

	
