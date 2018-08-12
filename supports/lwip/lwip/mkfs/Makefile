#
# $Author$
#


ROOT_DIR:=$(shell pwd)
	
LOCAL_ALL := \
		mkfsdata \
		

LDFLAGS += -lm


mkfsdata:makeHttpFsData.c
	gcc -s -o $@ -I../src/include -I../../unix/port/include -I../  $^ $(LDFLAGS) 

#	mv $@ $(BIN_DIR)/usr/bin  

clean:
	rm -rf mkfsdata.exe
	