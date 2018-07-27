							test IP Commands in simhost
																June,13, 2018

July 11, 2018 Wednesday
		route add 192.168.166.0 mask 255.255.255.0 192.168.168.102
		route add 192.168.167.0 mask 255.255.255.0 192.168.168.102
		route add 172.17.0.0 mask 255.255.255.0 192.168.168.102
		
		Notes:
				In windows, command prompt must be run as administrator;


curl -d '{"key1":"value1", "key2":"value2"}' -H "Content-Type: application/json" POST http://192.168.166.2/x-nmos/connection/single/senders -v


simhost
		simhost --help
		simhost -r : run ad RX
		simhost : run as TX
		

Commands List:
	'find'					: look up;
	'rs232' 				: send data to RS232;
	'secure' 				: command for security chip;
	'setupSys'			: setup parameters of system;
	'setupRs232' 		: setup parameters of RS232;
	'setupProtocol'	: setup parameters for media protocol;
	'setupMedia'		: setup parameters for video/audio media;


IP commands tests for ARM board:
apiClient -a 192.168.168.120 -c rs232 -o mac="00:04:25:1c:10:01",hex="11:22:33:44:55:66",feed=1,wait=101

apiClient -a 192.168.168.120 -c setup -o mac="00:04:25:1c:10:01",hex="11:22:33:44:55:66",feed=1,wait=101


setupProtocol: 
	start/stop
apiClient -a 192.168.166.2 -c connect -o target="00:04:25:1c:10:02",action=start


setupMedia: 
	Color Space: YCbCr-422|YCbCr-444|RGB|YCbCr-420|XYZ|KEY|CLYCbCr-422|CLYCbCr-444|CLYCbCr-420
apiClient -a 192.168.166.2 -c setupMedia -o target="00:04:25:1c:10:02",vW=720,vH=480,vFps=27,vDepth=10,vColorSpace="RGB",vInterlaced=1,vSegment=1,aFre=48000,aDepth=16,aCh=2


setupProtocol: 
	
apiClient -a 192.168.166.2 -c setupProtocol -o target="00:04:25:1c:10:02",mcast=1,ip=239.101.1.2,vport=10008,aport=10009,dport=10010,sport=10011


setupRs232: 
	MAC/IP/DHCP/DIP Switch/Name
apiClient -a 192.168.166.2 -c setupRs232 -o target="00:04:25:1c:10:02",bps=9600,data=8,stop=1,parity=none


setupSys: 
	MAC/IP/DHCP/DIP Switch/Name
apiClient -a 192.168.166.2 -c setupSys -o target="00:04:25:1c:10:02",mac="00:04:25:1c:10:05",ip="192.168.166.5",dhcp=1,dips=0,name="New Name"


IP commands tests for simhost:
Security Check
		set_key
apiClient -a 192.168.166.2 -c secure -o mac="00:04:25:1c:10:02",hex="11:32:43:54:65:76"

		get_id
apiClient -a 192.168.166.2 -c secure -o mac="00:04:25:1c:10:02",id=0

		set_status
apiClient -a 192.168.166.2 -c secure -o mac="00:04:25:1c:10:02",status=0


Find:
apiClient -a 192.168.166.2 -c find

RS232
apiClient -a 192.168.166.2 -c rs232 -o mac="00:04:25:1c:10:02",hex="11:22:33:44:55:66",feed=1,wait=101



echo "test" | nc -4u 192.168.166.2 3600
