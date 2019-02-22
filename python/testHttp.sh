#/bin/bash

# test http service
# Feb.20,2019

echo "HTTP service testing begin..."

DEST_IP=192.168.168.104

sdpList=(
     'video.sdp'
     'audio.sdp'
     'anc.sdp'
   )
   
i=0

echo "start testing..."

START=$(date +%s);

while [ 1 ] # $i -lt $COUNT ]
do

	index=$((i%3))
	CURRENT=$(date +%s);
	SECONDS=$((CURRENT-START)) 
	echo "Loop No.$i on ${sdpList[index]} within $SECONDS seconds..."
	
	CMD="curl --silent --output /dev/null --write-out  "%{http_code}" http://$DEST_IP/${sdpList[index]}"
	#echo "     $CMD"
	STATUS_CODE=`$CMD`
	
	echo "     response: $STATUS_CODE "

	if [[ "$STATUS_CODE" -ne 200 ]] ; then
     echo "Site status changed to $STATUS_CODE"
     echo "total %i in $SECONDS seconds"
     exit 0
	# else
	fi
	
	# sleep 1
	let i+=1
	
done

#
