=========================
Usage of Python utilites
=========================

  curl -H "Content-Type: text/html" -d 'rs232Baudrate=57600&rs232DataBit=8&rs232Parity=0&rs232StopBit=1' http://192.168.168.101/rs232Update -i


Jan.7th, 2019 Monday
 ./run getId 192.168.168.106 -d

 ./run getSecSta 192.168.168.106 -d

 ./run setKey 430786dc44514ad86aff88e1465449e467078ae0554a4cd36d138cde58495cbe 192.168.168.106 -d
 

43 07 86 dc 44 51 4a d8 6a ff 88 e1 46 54 49 e4 67 07 8a e0 55 4a 4c d3 6d 13 8c de 58 49 5c be

430786dc44514ad86aff88e1465449e467078ae0554a4cd36d138cde58495cbe

Jan.3rd, 2019
  curl -H "Content-Type: application/json" POST -d '{"sdpVideoIp":"192.168.168.106", "sdpVideoPort":80, "sdpVideoUri":"video/23/45.sdp", \
    "sdpAudioIp":"192.168.168.101", "sdpAudioPort":8080, "sdpAudioUri":"audio/23/34.sdp",}' http://192.168.168.101/service -i


Jan.2nd, 2019

 ./run rs232 11223344 192.168.168.60



12.31, 2018

./run setParams '{"reboot":1}' 192.168.168.103 -d
./run setParams '{"blink_led":1}' 192.168.168.103 -d


run './run.py'

::
 ./run.py --help
 ./run.py COMMAND --help, 

for example, `./run.py httpOs --help
     

Usage
--------

1. Search all nodes in LAN:
::

 ./run search -d

2. Lookup one node info:
::

 run find $IP


3. Update firmware 
 * by HTTP
::

	run httpOs --help
	run httpOs 192.168.168.130
	run httpFpga 192.168.168.130

 * by TFTP
::

  run tftpOs 192.168.168.130
  run tftpFpga 192.168.168.130



Send JSON IP command to node
::

 ./run setParams '{"cName":"JackLeeRX01"}' 192.168.168.60 -d

 ./run setParams '{"ip":"192.168.168.121","isDhcp":1}' 192.168.168.120 -d

 ./run setParams '{"IsConnect":0}' 192.168.168.60 -d

 ./run setParams '{"isDipOn":0}' 192.168.168.60 -d
 ./run setParams '{"isDipOn":1}' 192.168.168.60 -d

 ./run setParams '{"MCASTip":"239.100.1.61"}' 192.168.168.60 -d


 ./run setParams '{"RS232Baudrate":115200}' 192.168.168.60 -d

./run setParams '{"RS232Parity":"even"}' 192.168.168.60 -d
./run setParams '{"RS232Stopbits":0}' 192.168.168.60 -d
./run setParams '{"RS232Databits":7}' 192.168.168.60 -d




 ./run rs232 11223344 192.168.168.60

About RS232 command format:

 string "11223344": every char is used as nibble of one hexadecimal, so send 0x11, 0x22("), 0x33(3), and 0x44(D);
 so "4142434445" will send ABCDE in RS232;

::
 ./run rs232 4142434445 192.168.168.60
