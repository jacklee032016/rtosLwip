===============================================
Test and Utils python project for RTOS and LwIP
===============================================

code in `src` directory, tested on Python v3.

src
===

Usage
-----

build and enter virtual environment of python3;
:pip install -f requirements.txt:

run 


Unit Tests
----------

`nosetests -vv --exe -w tests/cases -s`


`nosetests tests.cases.unit_tests:HttpUnitTestCases.testAllStatic -s': use to test performance, especially on TPC_PCB OOM;


./run.py setParams "{\"cName\":\"JackTx\"}" 192.168.168.120 -d

./run.py setParams "[{"ip":"192.168.168.121","isDhcp":1}]" 192.168.168.120 -d

  