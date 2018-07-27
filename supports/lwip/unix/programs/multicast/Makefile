#
# $Author$
#


include $(RULE_DIR)/Makefile.pre

CODE_SUBDIRS = \
		.


SRC:= \
		broadcastClient.c \
		broadcastServer.c \
	
LOCAL_ALL := \
		mcSender \
		mcListener \
		

include $(ROOT_DIR)/Makefile.post

LDFLAGS := -L$(BIN_DIR)/lib 


LDFLAGS += -lLwip -pthread -lm

#-lLwip 

mcSender:$(OBJ_DIR)/multicastSender.o
	$(CC) -s -o $@ $^ $(LDFLAGS) 
	$(MOVE) $@ $(BIN_DIR)/usr/bin  

mcListener:$(OBJ_DIR)/multicastListener.o
	$(CC) -s -o $@ $^ $(LDFLAGS) 
	$(MOVE) $@ $(BIN_DIR)/usr/bin  
