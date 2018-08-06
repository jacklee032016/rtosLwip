#/bin/bash

NMOS_HOST=192.168.166.2
NMOS_PORT=80

VERBOSE=

if [ "$VERBOSE" == "yes" ] ; then
	OUTPUT=" -v "
else
	OUTPUT=" python -mjson.tool"
fi		

COUNT=100
LOG_FILE="/tmp/apiClient767Tests.log"


# string array in bash
#declare -a 
NODE_APIS=(
		"/" 
		"/self" 
		"/devices"
		"/sources"
		"/flows"
		"/senders"
		"/receivers"
		"/unknonw"
)



# using string array
#for API in "${NODE_APIS[@]}"
#do
#	let i=i+1
#	echo "#$i Request: GET http://$NMOS_HOST:$NMOS_PORT/x-nmos/node/v1.2$API"
#	curl -H "Content-Type: application/json" -X GET http://$NMOS_HOST:$NMOS_PORT/x-nmos/node/v1.2$API | python -mjson.tool
#	echo ""
#	done| python -mjson.tool


function iterateOneRoot() {
		#arr="$1"
		NAME=$1
		shift
		arr=("$@")
		i=0

		echo ""
		echo "Iterate API : $NAME"
		for API in "${arr[@]}";
			do
				let i=i+1
				CMD="GET http://$NMOS_HOST:$NMOS_PORT/x-nmos/$NAME/v1.2$API "
				echo "    #$i Request:$CMD"
				curl -H "Content-Type: application/json" -X $CMD | python -mjson.tool
				echo ""
			done
		
		echo ""
}


iterateOneRoot "node" "${NODE_APIS[@]}"


#iterateOneRoot "connection" "${NODE_APIS[@]}"
