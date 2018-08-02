"""
Tests settings module
"""

DEBUG = False
TEST_DEBUG = False
TESTING = True

FIRMWARE_TYPE_OS="firmOs"
FIRMWARE_TYPE_FPGA="firmFpga"


FIRMWARE_BIN_OS='../../an767/BIN/images.AN767/rtosLwipAN767.bin.bin'

FIRMWARE_BIN_FPGA_RX='../../Binary/top_Angelica_N_767_rx1.bin'
FIRMWARE_BIN_FPGA_TX='../../Binary/top_Angelica_N_767_rx2.bin'

IP_CMD_FIND="get_param"
IP_CMD_SET_PARAM = "set_param"
IP_CMD_RS232_DATA= "send_data_rs232"
IP_CMD_SECURE="security_check"


FIRMWARE_URL_OS="/mcuUpdate"
FIRMWARE_URL_FPGA="/fpgaUpdate"

STATIC_PAGES=[
    "/",
    "/upgradeFpga.html",
    "/upgradeMcu.html",
    "/styles.css",
    "/load_html.js",
    "/logo.jpg",
    "/header_bg.png",
    "/loading.gif",
    "/404.html"
]

CTRL_TYPE = "UDP" #"TCP" or "UNIX"

#CTRL_RX = '192.168.168.102'
CTRL_RX = '192.168.168.102'
CTRL_RX_PORT = 3600

NODE_ADDRESS='192.168.166.2'
HTTP_PORT = 80

CYRL_TARGET="22:22:22:22:22:34"
CTRL_USER = "admin"
CTRL_PASSWORD = "admin"

CTRL_MULTICAST = "239.0.0.1"


CTRL_FTP_SERVER = '192.168.168.102'
CTRL_FTP_USERNAME = 'root'
CTRL_FTP_PASSWORD = 'lzj320'

CTRL_FTP_PATH = 'pub'
KEYSTONE_API_VERSION = 'v2.0'

