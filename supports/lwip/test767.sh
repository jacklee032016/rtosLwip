#/bin/bash


COUNT=100
LOG_FILE="/tmp/apiClient767Tests.log"

TX='192.168.168.120'
RX="192.168.168.130"

declare -a TXS=('192.168.168.120' '192.168.168.123' )
declare -a RXS=("192.168.168.130" "192.168.168.132" )


EXE=./Linux.bin.X86/usr/bin/apiClient


mkNames()
{
	i=0

	for t in "${TXS[@]}"
	do
		echo $t
		let i+=1
		#$EXE -a $t -c setup -o mac='00:04:25:1c:a1:01',ip="192.168.168.20$i",vport=40000,aport=40004,name="tx767-$i"
	done

	i=0
	for t in "${RXS[@]}"
	do
		echo $t
		let i+=1
		#$EXE -a $t -c setup -o mac='00:04:25:1c:b1:01',ip="192.168.168.21$i",vport=40000,aport=40004,name="rx767-$i"
	done

}

startAllTx()
{
	i=0

	for t in "${TXS[@]}"
	do
		echo $t
		let i+=1
		$EXE -a $t -c transfer -o action=start
	done
}


stopAllTx()
{
	i=0

	for t in "${TXS[@]}"
	do
		echo $t
		let i+=1
		$EXE -a $t -c transfer -o action=stop
	done
}


configChangeUnicast()
{
		declare -a MACS=('00:04:25:1c:a0:01' '00:04:25:1c:a0:02' )
		declare -a IPS=('192.168.168.130' '192.168.168.132' )
		declare -a NEW_RXS=("192.168.168.132" "192.168.168.130" )
		declare -a NEW_TXS=("192.168.168.120" "192.168.168.123" )

		#echo "arg2 $1"
		if [ "$1" -eq "1" ]; then
		#	declare -a NEW_RXS=("192.168.168.132" "192.168.168.130" )
			#echo "TRUE"
			declare -a NEW_TXS=("192.168.168.123" "192.168.168.120" )
		#else
			#echo "FALSE"
#			exit;
		fi

		# 0x98 58
		VPORT=47000
		APORT=47004

		i=0
#		for r in "${NEW_RXS[@]}"
		for t in "${NEW_TXS[@]}"
		do
			#t=${TXS[$i]}
			m=${MACS[$i]}
			p=${IPS[$i]}

			echo "	Configuring TX $t now....."
			let i+=1
#			echo "		configuring RX $r $m $p $VPORT $APORT..."
#			$EXE -c setup -a $r -o mac=$m,ip=$p,vport=$VPORT,aport=$APORT

			echo "		configuring TX $t $m $p $VPORT $APORT..."
			$EXE -c setup -a $t -o mac=$m,ip=$p,vport=$VPORT,aport=$APORT >> $LOG_FILE 2>&1
			res=$?
			#echo $res
			if [ "$res" -ne "0" ]; then
					if [ "$res" -eq "2" ]; then
							echo "#################################Timeout when send command, please check your network connection!!!!"
					else
							echo "#################################Something wrong, so exit!!!!"
					fi
							
					exit;
			#else		
					#echo "Correct" "$?"
			fi
			
		done
}


configAllUnicast()
{
		declare -a MACS=('00:03:03:03:03:03' '00:03:03:03:04:05' )
		declare -a IPS=('192.168.168.221' '192.168.168.222' )
		
		# 0x98 58
		VPORT=47000
		APORT=47004

		i=0
		for r in "${RXS[@]}"
		do
			t=${TXS[$i]}
			m=${MACS[$i]}
			p=${IPS[$i]}

			echo "Configuring RX $r now....."
			let i+=1
			echo "		configuring RX $r $m $p $VPORT $APORT..."
			$EXE -c setup -a $r -o mac=$m,ip=$p,vport=$VPORT,aport=$APORT

			echo "		configuring TX $t $m $p $VPORT $APORT..."
			$EXE -c setup -a $t -o mac=$m,ip=$p,vport=$VPORT,aport=$APORT
			
		done
}

RED='\033[0;31m'
NC='\033[0m' # No Color

testFind()
{
	index=0
	timeouts=0
	
	echo "Start finding test..."
	
	while [ 1 ]
	do
		let index+=1
		echo "	Loop No.$index finding test..."

		$EXE -c find 
		#>> $LOG_FILE 2>&1
		res=$?
		# echo $res
		if [ "$res" -ne "0" ]; then
				if [ "$res" -eq "2" ]; then
						let timeouts+=1
						echo -e "${RED}#################################No.$timeouts Timeout in total $index send msg when send command, please check your network connection!!!!${NC}"
				else
						echo -e "${RED}#################################Something wrong, so exit!!!!${NC}"
						exit;
				fi
							
		#else		
				#echo "Correct" "$?"
		fi

		sleep 1

	done
}

testExchange()
{
	index=0

	echo "start testing..."

	while [ 1 ] # $index -lt $COUNT ]
	do
		n=$((index%2))
		echo "Loop No.$index Testing No.$n..."
	
		configChangeUnicast $n

		sleep 2
		let index+=1
		echo ""
	
	done
}

#if [ $# -le 1 ]; then
#		echo $0: Missing argument, 'find|unicast|multicast|start|stop'
#		echo $0: usage: myscript name
#		exit 1
#fi

case $1 in
    find)
    echo "FIND....."
    $EXE -c find 
    exit 1
    ;;


    name)
    echo "Rename devices...."
    mkNames
    exit 1
    ;;
    
    start)
    echo "START...."
    #$EXE -c transfer -a $TX -o action=start
    startAllTx
    exit 1
    ;;
    
    stop)
    echo "STOP...."
    #$EXE -c transfer -a $TX -o action=stop
    stopAllTx
    exit 1
    ;;
    
    unicast)
    echo "UNICAST"
    #MAC='00:04:25:2c:22:01'
		#IP='192.168.168.92'
		#VPORT=36000
		#APORT=36004
		#configAllUnicast
		configChangeUnicast $2
    exit 1
    ;;


    testFind)
    echo "TEST FIND"
		testFind
    exit 1
    ;;

    test)
    echo "TEST"
		testExchange
    exit 1
    ;;
    
    multicast)
    echo "MULTICAST....."
		MAC='00:03:03:03:03:03'
		IP='239.0.0.1'
		
		# 0x98 58
		VPORT=40696
		APORT=40700
    
    ;;
    
    *)    # unknown option
    echo "find|unicast|multicast|transfer"
#    shift # past argument
		exit 1
    ;;
esac



echo "configuring TX..."
$EXE -c setup -a $TX -o mac=$MAC,ip=$IP,vport=$VPORT,aport=$APORT,name="tx767-1"

echo "configuring RX..."
$EXE -c setup -a $RX -o mac=$MAC,ip=$IP,vport=$VPORT,aport=$APORT,name="rx767-1"


