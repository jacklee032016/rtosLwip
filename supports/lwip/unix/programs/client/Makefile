#
# $Author$
# Only JSON data parsing is dependent on LwIP library, network operations are only dependent on Linux Library;
#


include $(RULE_DIR)/Makefile.pre

CODE_SUBDIRS = \
		.


SRC:= \
		apiClientMain.c \
		apiClientCmds.c \
		apiClientOpts.c \
		apiClientUtils.c \
	
LOCAL_ALL := \
		apiClient \
		

include $(ROOT_DIR)/Makefile.post

LDFLAGS := -L$(BIN_DIR)/lib 


LDFLAGS += -lLwip -pthread -lm

#-lLwip 

apiClient:$(OBJS)
	$(CC) -s -o $@ $^ $(LDFLAGS) 
	$(MOVE) $@ $(BIN_DIR)/usr/bin  

