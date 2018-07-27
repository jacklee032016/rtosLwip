# /bin/bash

IP=192.168.166.2
#IP=192.168.168.130
#IP=192.168.168.120
#IP=192.168.168.121
PORT=69
#MODE=ascii
MODE=binary

#DEST=muxFpga
DEST=muxOs


#if [ $# -le 1 ]; then
if [ $# -ge 1 ]; then
		echo "TFTP send $1 to $IP:$PORT as $DEST..."
else
		#echo $0: Missing argument, 'find|unicast|multicast|start|stop'
		echo $0: usage: myscript binaryFileName $#
		exit 1
fi

tftp -m $MODE $IP $PORT -c put $1 $DEST

