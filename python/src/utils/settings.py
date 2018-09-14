"""
Tests settings module
"""

DEBUG = False
TEST_DEBUG = False
TESTING = True

FIRMWARE_TYPE_TFTP_OS="firmOs"
FIRMWARE_TYPE_TFTP_FPGA="firmFpga"


FIRMWARE_BIN_OS='../../an767/BIN/images.AN767/rtosLwipAN767.bin.bin'

FIRMWARE_BIN_FPGA_RX='/media/sf_rtos/Binary/top_Angelica_N_767_rx1.bin'
FIRMWARE_BIN_FPGA_TX='/media/sf_rtos/Binary/top_Angelica_N_767_rx2.bin'

IP_CMD_FIND="get_param"
IP_CMD_SET_PARAM = "set_param"
IP_CMD_RS232_DATA= "send_data_rs232"
IP_CMD_SECURE="security_check"

# seconds
IP_CMD_FIND_TIMEOUT=5

FIRMWARE_URL_OS="/mcuUpdate"
FIRMWARE_URL_FPGA="/fpgaUpdate"

STATIC_PAGES=[
    "/upgradeFpga.html",
    "/upgradeMcu.html",
    "/styles.css",
    "/load_html.js",
    "/logo.jpg",
    "/header_bg.png",
    "/loading.gif",
    "/404.html"
]

DYNAMIC_PAGES=[
    "/",
    "/info",
    "/media",
    "/",
    "/info",
    "/media",
    "/",
    "/info",
    "/media"
]

DYNAMIC_PAGES2=[
    "/mcuUpdate",
    "/fpgaUpdate",
    "/reboot"
]

SIMULATOR_GATEWAT="192.168.166.1"
