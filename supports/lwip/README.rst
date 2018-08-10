=======================
LwIP Testing and Coding
=======================



----------
UDP perf
----------

^^^^^^^
RX Perf
^^^^^^^

: board plays as UDP server, only receives packets, never reply 

::
 iperf --client 192.168.166.2 ---interval 5 -t 100 -u -b 1G 

  - -c in client mode, connect to <host>
  - -i Interval for bandwidth report
  - -t time in second to transmit
  - -u UDP
  - -b bandwidth to send in bits/second

Tests on board:
::
 iperf --client 192.168.168.120 ---interval 5 -t 10 -u -b 100M 


^^^^^^^^^^^^^^^^^^^^
Test UDP Perf in PX
^^^^^^^^^^^^^^^^^^^^
 UDP Perf server:
::
 iperf -s -i 10 5 -t 100 -u

::
 iperf --client localhost ---interval 5 -t 10 -u -b 1G


tcpdump

:: 
 tcpdump -D : list add devices which can be captured;
 tcpdump -n host localhost -v udp -i lo
 