#
# $Author$
#


include $(RULE_DIR)/Makefile.pre

CODE_SUBDIRS = \
		.


SRC:= \
		jsonSimple.c \
		jsonTest.c \
	
LOCAL_ALL := \
		jsonTest \
		jsonSimple \
		

include $(ROOT_DIR)/Makefile.post

LDFLAGS := -L$(BIN_DIR)/lib 


LDFLAGS += -lLwip -pthread -lm

#-lLwip 

jsonSimple:$(OBJ_DIR)/jsonSimple.o 
	$(CC) -s -o $@ $^ $(LDFLAGS) 
	$(MOVE) $@ $(BIN_DIR)/usr/bin  

jsonTest:$(OBJ_DIR)/jsonTest.o 
	$(CC) -s -o $@ $^ $(LDFLAGS)
	$(MOVE) $@ $(BIN_DIR)/usr/bin  

