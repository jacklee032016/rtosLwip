=========================
Usage of Python utilites
=========================

run './run.py'

::
 ./run.py --help
 ./run.py COMMAND --help, 

for example, `./run.py httpOs --help
     

Usage
-----

Search all nodes in LAN:
::
 ./run.py search -d


Send JSON IP command to node
::
 ./run.py setParams '{"ip":"192.168.168.121","isDhcp":1}' 192.168.168.120 -d

  