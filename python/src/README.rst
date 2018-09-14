=========================
Usage of Python utilites
=========================

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
 ./run setParams '{"ip":"192.168.168.121","isDhcp":1}' 192.168.168.120 -d

 ./run setParams '{"IsConnect":0}' 192.168.168.63 -d
  