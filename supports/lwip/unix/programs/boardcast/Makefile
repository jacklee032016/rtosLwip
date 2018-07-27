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
		bcClient \
		bcServer \
		

include $(ROOT_DIR)/Makefile.post

LDFLAGS := -L$(BIN_DIR)/lib 


LDFLAGS += -lLwip -pthread -lm

#-lLwip 

bcClient:$(OBJ_DIR)/broadcastClient.o
	$(CC) -s -o $@ $^ $(LDFLAGS) 
	$(MOVE) $@ $(BIN_DIR)/usr/bin  

bcServer:$(OBJ_DIR)/broadcastServer.o
	$(CC) -s -o $@ $^ $(LDFLAGS) 
	$(MOVE) $@ $(BIN_DIR)/usr/bin  
