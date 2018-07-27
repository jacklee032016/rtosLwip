
				Test of LWIP(LightWeight IP) 
											 January, 2018	Jack Lee

May.21st, 2018
apiClient: 
	Only JSON data parsing is dependent on LwIP library, network operations are only dependent on Linux Library;



Test performance with lwiperf
		iperf -c host 
				-i 5	: report interval, 5 seconds;
				-t 50	: runing time, 50 seconds;
				-w 8K : receiving window size, 8K;
		

sendTftpFile,sh
	tftp client script, send file of RTOS/FPGA images to AN767
		: modify IP addrss to board;
		: modify DEST for RTOS or FPGA;
		: privide file name of image;

startup.sh
		: start simhost first time after virtual Linux boots: add virtual ethernet interface of tap0 and routes corresponding to it;
		: after that (tap0 can be seen), only 'simhost' command is needed;

test767.sh

		

ip route add 239.100.1.0/8 dev tap0
ip route add 239.100.1.0/8 dev tap1

iptables -A INPUT -d 239.100.1.1 -j ACCEPT
iptables -A FORWARD -d 239.100.1.1 -j ACCEPT
iptables -A OUTPUT -d 239.100.1.1 -j ACCEPT


iptables -A INPUT   -m pkttype --pkt-type multicast -j ACCEPT
iptables -A FORWARD -m pkttype --pkt-type multicast -j ACCEPT
iptables -A OUTPUT  -m pkttype --pkt-type multicast -j ACCEPT

Or:

iptables -A INPUT   -s 224.0.0.0/4 -j ACCEPT
iptables -A FORWARD -s 224.0.0.0/4 -d 224.0.0.0/4 -j ACCEPT
iptables -A OUTPUT -d 224.0.0.0/4 -j ACCEPT


iptables -A INPUT -p udp --dport 3600

Capture source ip
		tcpdump -n src host 192.168.168.102
Capture any packet from/to source/dest
		tcpdump -n host 192.168.168.102

		tcpdump -n host 192.168.167.1
		
		tcpdump -i tap1 -n udp
		

Multicast packet with nc
June,15, 2018
	1. multicast server and group registering:
	mdump was used in another session to confirm that the data was being sent:
			mdump 239.100.1.1 3600 192.168.166.1

		Equiv cmd line: mdump -p0 -Q0 -r4194304 239.100.1.1 3600 192.168.166.1
		WARNING: tried to set SO_RCVBUF to 4194304, only got 327680

	2. send multicast packet to group
	nc -w 1 -v -u -s 192.168.166.1 239.100.1.1 3600 < init.sh
		-w : wait timeout
		-v : verbose
		-u : UDP
		-4 : IPv4
		-s : source address
		

	nc -w 1 -v -u -s 192.168.167.1 239.100.1.1 3600 < init.sh

route add -net 224.0.0.0 netmask 240.0.0.0 tap0

mdump 239.255.0.1 30001 127.0.0.1


UDP boardcast packet in Linux
	echo "test" | nc -4u 192.168.166.255 3600
	
	nc -4u 192.168.166.2 3600 < test.txt

	Test UDP, echo and 'ncat':
			echo "test" | nc -4u 192.168.166.2 7
				result: 
						return 'test' displayed in console of Linux
						debug info in LwIP;

	echo "test" | nc -4u 192.168.166.2 3600
	
	When 'ncat: Connection refused.', means no service point can be accessable;
	When ncat is used to access unicast port, everything is OK;
	When ncat is used to access boardcast port, UDP server can receive and reply message, but ncat can't receive reply;
	Normal UDP client can receive reply of boardcast message in unicast port;
	


Directories:
		lwip/src:
				all source files (including lwIP and its extensions) crossing platform;
				lwip/Makefile: Makefile for Unix platform;
				lwip/src/Makefile: Makefile for ARM-Cortex-M7;
				
					:LwIP extension: such as json and HTTP client/server;
							:src/lwip	: source files;
							:include/	: header files for extensions;
					:others: standard LwIP

		ports/sam:
				port for SAM E70
				
		unix:
				LwIP on Unix;
				
						:port			: port of unix
						:sim			: unix simulators, such as node, router and host;
						:programs	: utilities and testings
								:mkfs 	: make file system for http server;
								:json		: json testing programs;
								

Usage of Unix port:
		simnode and simrouter: testing router function of LwIP;
		simhost: LwIP testing program in Linux;

Jan,13rd, Saturday
	How to build for different program:
		lwIP is defined by lwipopt.h, which is defined by every program, eg. exists in the directory of this program;
		So run 'make' one time, only build one program: header file point to that directory;
		
