#!python

###!/usr/bin/env python

import json
from time import sleep

import sys,getopt

from cmds.http.HttpClient import WebClient, RestApiClient

from utils import ColorMsg

def testReboot():
    #testHost = "192.168.166.2"
    testHost = "192.168.168.120"
    webClient = WebClient(debug=True )
    response = webClient.get(ip=testHost, uri="/reboot")

def testUploadFpga1():
    #testHost = "192.168.166.2"
    testHost = "192.168.168.120"
    webClient = WebClient(debug=True )

    #files = {'file': ('top_Angelica_N_767_tx.bin', open('../../top_Angelica_N_767_tx.bin', 'rb'), 'application/vnd.ms-excel', {'Expires': '0'})}
    files = { 'file': open('../../Binary/top_Angelica_N_767_rx1.bin', 'rb') }
    response = webClient.post(ip=testHost, uri="/fpgaUpdate", files=files)

def testUploadFpga2():
    #testHost = "192.168.166.2"
    testHost = "192.168.168.120"
    webClient = WebClient(debug=True )

    #files = {'file': ('top_Angelica_N_767_tx.bin', open('../../top_Angelica_N_767_tx.bin', 'rb'), 'application/vnd.ms-excel', {'Expires': '0'})}
    files = { 'file': open('../../Binary/top_Angelica_N_767_rx2.bin', 'rb') }
    response = webClient.post(ip=testHost, uri="/fpgaUpdate", files=files)

def testUploadMcu():
    #testHost = "192.168.166.2"
    testHost = "192.168.168.120"
    webClient = WebClient(debug=True )

    #files = {'file': ('top_Angelica_N_767_tx.bin', open('../../top_Angelica_N_767_tx.bin', 'rb'), 'application/vnd.ms-excel', {'Expires': '0'})}
    files = { 'file': open('../../an767/BIN/images.AN767/rtosLwipAN767.bin.bin', 'rb') }
    response = webClient.post(ip=testHost, uri="/mcuUpdate", files=files)

def testHttp():
    #testHost = "192.168.166.2"
    testHost = "192.168.168.120"
    webClient = WebClient(debug=False )
    # text
    response = webClient.get(ip=testHost, uri="/upgradeFpga.html")
    response = webClient.get(ip=testHost, uri="/styles.css")
    response = webClient.get(ip=testHost, uri="/load_html.js")

    # images
    response = webClient.get(ip=testHost, uri="/logo.jpg")
    response = webClient.get(ip=testHost, uri="/header_bg.png")
    response = webClient.get(ip=testHost, uri="/loading.gif")


    response = webClient.get(ip=testHost, uri="/upgradeFpga.html")
    response = webClient.get(ip=testHost, uri="/styles.css")
    response = webClient.get(ip=testHost, uri="/load_html.js")

    # images
    response = webClient.get(ip=testHost, uri="/logo.jpg")
    response = webClient.get(ip=testHost, uri="/header_bg.png")
    response = webClient.get(ip=testHost, uri="/loading.gif")

    response = webClient.get(ip=testHost, uri="/logo.jpg")
    response = webClient.get(ip=testHost, uri="/header_bg.png")
    response = webClient.get(ip=testHost, uri="/loading.gif")

    response = webClient.get(ip=testHost, uri="/logo.jpg")
    response = webClient.get(ip=testHost, uri="/header_bg.png")
    response = webClient.get(ip=testHost, uri="/loading.gif")

    # CGIs
    #response = webClient.get(ip=testHost, uri="/info")
    #response = webClient.get(ip=testHost, uri="/media")
    #response = webClient.get(ip=testHost, uri="/reboot")

#    response = client.get(ip="192.168.166.2", uri="/x-nmos/node/v1.2/self")

def usage():
    print("Usage: %s: -m MCU -f FPGA -r reboot -t req -h help"%(sys.argv[0]))

if __name__ == "__main__":
    print(sys.argv[0] )
    print(sys.argv[1:] )
    options, remainder = getopt.getopt(sys.argv[1:], 'm:t:efrh', ['mcu=', 'fpga=', "test=", 'reboot', "help'"])
    if len(options) == 0 or options is None:
        usage()
        sys.exit(1)

    print('OPTIONS   :',options )
    print('remainder   :',remainder  )

    for opt, arg in options:
        print(opt, arg)
        if opt in ('-e', '--egpga'):
            fpgaFilename = arg
            testUploadFpga1()
        if opt in ('-f', '--fpga'):
            fpgaFilename = arg
            testUploadFpga2()
        elif opt in ('-m', '--mcu'):
            mcuFilename = arg
            testUploadMcu()
            sleep(0.5)
            testReboot()

        elif opt in ('-r', '--reboot'):
            testReboot()
        elif opt in ('-t', '--test'):
            testOpt = arg
            testHttp()
        else:
            usage()

#    print('FPGA :', fpgaFilename)
#    print('MCU :', mcuFilename)
#    print('Test :', testOpt)

    #blocksize = a
    #testHttp()
    #testUploadFpga()
    #testUploadMcu()
    #testReboot()
