#/bin/bash

# test every command of IP commands

API_CMD=apiClient

SIMULATOR=YES
VERBOSE=

if [ "$VERBOSE" == "yes" ] ; then
	OUTPUT=" -v "
else
	OUTPUT=" python -mjson.tool"
fi		

if [ "$SIMULATOR" == "YES" ] ; then
	TX_IP=192.168.166.2
	TX_MAC=00:04:25:1c:10:02
	RX_IP=192.168.167.3
	RX_MAC=00:04:25:1c:a0:02
else
	TX_IP=192.168.168.120
	RX_IP=192.168.168.130
fi


testFind()
{

	echo "testing 'find' command..."
	$API_CMD -c find -o dev=tap0

	$API_CMD -c find -o dev=tap1
}


testSetupSys()
{
	echo ""
	echo "testing 'setupSys' command..."
	echo "   TX..."
	apiClient -a $TX_IP -c setupSys -o target=$TX_MAC,mac="00:04:25:1c:10:05",ip="192.168.166.5",dhcp=1,dips=0,name="tx-01"
	echo ""
	echo "   RX..."
	apiClient -a $RX_IP -c setupSys -o target=$RX_MAC,mac="00:04:25:1c:10:05",ip="192.168.167.5",dhcp=1,dips=0,name="rx-01"
	echo ""
}

testSetupRs232()
{
	echo ""
	echo "testing 'setupRs232' command..."
	echo "   TX..."
	apiClient -a $TX_IP -c setupRs232 -o target=$TX_MAC,bps=19200,data=8,stop=1,parity=none
	echo ""
	echo "   RX..."
	apiClient -a $RX_IP -c setupRs232 -o target=$RX_MAC,bps=9600,data=8,stop=1,parity=none
	echo ""
}


testSetupProtocol()
{
	echo ""
	echo "testing 'setupProtocol' command..."
	echo "   TX..."
	apiClient -a $TX_IP -c setupProtocol -o target=$TX_MAC,mcast=1,ip=239.101.2.2,vport=10018,aport=10019,dport=10020,sport=10021
	echo ""
	echo "   RX..."
	apiClient -a $RX_IP -c setupProtocol -o target=$RX_MAC,mcast=1,ip=239.101.1.2,vport=10008,aport=10009,dport=10010,sport=10011
	echo ""
}


testSetupMedia()
{
	echo ""
	echo "testing 'setupMedia' command..."
	echo "   TX..."
	apiClient -a $TX_IP -c setupMedia -o target=$TX_MAC,vW=720,vH=480,vFps=27,vDepth=10,vColorSpace="RGB",vInterlaced=1,vSegment=1,aFre=48000,aDepth=16,aCh=2
	echo ""
	echo "   RX..."
	apiClient -a $RX_IP -c setupMedia -o target=$RX_MAC,vW=1080,vH=720,vFps=59,vDepth=12,vColorSpace="YCbCr-420",vInterlaced=0,vSegment=0,aFre=44100,aDepth=24,aCh=1
	echo ""
}


testConnect()
{
	echo ""
	echo "testing 'connect' command..."
	echo "   TX..."
	apiClient -a $TX_IP -c connect -o target=$TX_MAC,action=start
	echo ""
	echo "   RX..."
	apiClient -a $RX_IP -c connect -o target=$RX_MAC,action=start
	echo ""
}


testRs232Data()
{
	echo ""
	echo "testing 'rs232' command..."
	echo "   TX..."
	apiClient -a $TX_IP -c rs232 -o target=$TX_MAC,hex="99:88:77:66:55:44",feed=0,wait=90
	echo ""
	echo "   RX..."
	apiClient -a $RX_IP -c rs232 -o target=$RX_MAC,hex="11:22:33:44:55:66",feed=1,wait=101
	echo ""
}


testSecurity()
{
	echo ""
	echo "testing 'secure' command..."
	echo "   TX data..."
	apiClient -a $TX_IP -c secure -o target=$TX_MAC,hex="11:32:43:54:65:76"
	echo ""
	echo "   RX data..."
	apiClient -a $RX_IP -c secure -o target=$RX_MAC,hex="12:34:45:56:67:78"
	echo ""

	echo "   TX get_ID..."
	apiClient -a $TX_IP -c secure -o target=$TX_MAC,id=0
	echo ""
	echo "   RX get_ID..."
	apiClient -a $RX_IP -c secure -o target=$RX_MAC,id=0
	echo ""

	echo "   TX getStatus..."
	apiClient -a $TX_IP -c secure -o target=$TX_MAC,status=0
	echo ""
	echo "   RX getStatus..."
	apiClient -a $RX_IP -c secure -o target=$RX_MAC,status=0
	echo ""
}


echo "$#"
if [ $# -le 0 ]; then
		echo $0: Missing argument, 'find|sys|rs232|protocol|media|conn|232Data|secure'
		echo $0: usage: myscript name
		exit 1
fi

case $1 in
    find)
    testFind
    exit 1
    ;;

    sys)
    testSetupSys
    exit 1
    ;;
    
    rs232)
    testSetupRs232
    exit 1
    ;;
    
    protocol)
    testSetupProtocol
    exit 1
    ;;
    
    media)
    testSetupMedia
    exit 1
    ;;
    
    conn)
    testConnect
    exit 1
    ;;
    

    232Data)
    testRs232Data
    exit 1
    ;;
    
    secure)
    testSecurity
    exit 1
    ;;
    

    
    *)    # unknown option
    echo "find|unicast|multicast|transfer"
#    shift # past argument
		exit 1
    ;;
esac

