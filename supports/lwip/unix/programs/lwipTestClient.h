
#ifndef	__LWIP_TEST_CLIENT_H__
#define	__LWIP_TEST_CLIENT_H__


#define	LWIP_FOR_SIMHOST			0	/* test for simhost, otherwise test amtel*/

#if LWIP_FOR_SIMHOST
#define	DEVICE_NAME  	"tap0"
#else
#define	DEVICE_NAME  	"enp0s3"
#endif

#define	API_CLIENT_NEW_LINE			"\r\n"

#define LWIP_TEST_GROUP_PORT			12345
#define LWIP_TEST_GROUP_ADDR			"225.0.0.37"


#define	BC_CLIETN_TIMEOUT		2	/* second */

#define	BC_CLIETN_DELAY		5*1000*1000	/*  */


#define	BC_CLIENT_ERROR_TIMEOUT		2


#define	API_CLIENT_DEBUG_OUT			0

#ifndef	IPADDR_NONE
#define IPADDR_NONE         ((unsigned int)0xffffffffUL)
#endif


struct _AN767_DEVICE;

struct _AN767_DEVICE
{
	struct _AN767_DEVICE		*next;

	EXT_RUNTIME_CFG		cfg;
};

struct API_PARAMETERS
{
	char 				address[128];
	int					port;

	char					cmd[128];
	EXT_MAC_ADDRESS	target;

	EXT_VIDEO_CONFIG	vCfg;

	/* video configuration */	
	char					mediaMac[128];
	char					mediaIP[128];

	unsigned short		vport;
	unsigned short		aport;

	char					name[32];
	char					isDhcp;
	char					isDipSwitch;

	char					isMcast;


	MuxRunTimeParam		mediaParam;

	/* rs232 */
	unsigned int			rs232bps;
	unsigned char			rs232data;
	unsigned char			rs232stop;
	char					rs232Parity[32];

	EXT_MAC_ADDRESS	hexData;
	char					isFeed;
	unsigned	short		waitMs;

	/* Security Check */
	unsigned char			isGetId;
	unsigned char			isGetStatus;
	
};



typedef	struct _AN767_DEVICE  AN767_DEV_T;

typedef	struct
{
	int						state;
	
	int						port;
	int 						bcSocket;
	
	int						socket;
//	int						peerAddress;
	struct sockaddr_in			peerAddress;
	EXT_UUID_T				uuid;
	EXT_MAC_ADDRESS		macAddress;

	char						buffer[8192];
	int						size;
	int						bufIndex;

	EXT_JSON_PARSER  		parser;

	EXT_RUNTIME_CFG		*clientCfg;
	struct API_PARAMETERS	*params;

	AN767_DEV_T				*devs;
}API_CLIENT;


struct API_CLIENT_CMD_HANDLER;

typedef	struct API_CLIENT_CMD_HANDLER
{
	char		*name;
	char		*ipCmdName;

	/* validate parameters of this command. return 0: success; others: fail */
	int	(*validate)(struct API_CLIENT_CMD_HANDLER *, API_CLIENT *, char *);

	/* execute this command */
	int	(*execute)(struct API_CLIENT_CMD_HANDLER *, API_CLIENT *);
}API_CLIENT_CMD_HANDLER;


#define	API_CMD_FIND						"find"
#define	API_CMD_SETUP_SYS					"setupSys"
#define	API_CMD_SETUP_RS232				"setupRs232"
#define	API_CMD_SETUP_PROTOCOL			"setupProtocol"
#define	API_CMD_SETUP_MEDIA				"setupMedia"
#define	API_CMD_SETUP_ACTION				"connect"

#define	API_CMD_RS232						"rs232"
#define	API_CMD_SECURITY					"secure"


#define	API_CMD_CLIENT_TARGET				"target"


#define	CLIENT_OPTIONS_SETUP_SYS			"target='xx:xx:xx:xx:xx:xx',mac='xx:xx:xx:xx:xx:xx',ip='yyy.yyy.yyy.yyy',dhcp='1|0',dips='1|0',name='ProdName"
#define	CLIENT_OPTIONS_SETUP_RS232		"target='xx:xx:xx:xx:xx:xx',bsp=9600|19200|38400|57600|115200,data=5|6|7|8,stop=1|2,parity='none|even|odd'"
#define	CLIENT_OPTIONS_SETUP_PROTOCOL	"target='xx:xx:xx:xx:xx:xx',mcast=1|0,ip='yyy.yyy.yyy.yyy',vport=xx,aport=yy,dport=zz,sport=xy"
#define	CLIENT_OPTIONS_SETUP_MEDIA		"target='xx:xx:xx:xx:xx:xx',vW='',vH='',vFps='',vColorSpace='STR',vDepth='',vInterlaced='',vSegment='',aFre='',aDepth='',aCh=''"
#define	CLIENT_OPTIONS_SETUP_ACTION		"target='xx:xx:xx:xx:xx:xx',action='start/stop'"

#define	CLIENT_OPTIONS_RS232				"target='xx:xx:xx:xx:xx:xx',hex='xx:xx:xx:xx:xx:xx',feed=1|0,wait=ms"
#define	CLIENT_OPTIONS_SECURITY			"target='xx:xx:xx:xx:xx:xx',hex='xx:xx:xx:xx:xx:xx',id=xx,status=yy"

#define	CLIENT_OPTIONS_FIND				"dev=enp0s3(board)|tap0(TX)|tap1(RX)"


int apiClientSetupSys(struct API_CLIENT_CMD_HANDLER *handle, API_CLIENT *apiClient);
int apiClientSetupRs232(struct API_CLIENT_CMD_HANDLER *handle, API_CLIENT *apiClient);
int apiClientSetupProtocol(struct API_CLIENT_CMD_HANDLER *handle, API_CLIENT *apiClient);
int apiClientSetupMedia(struct API_CLIENT_CMD_HANDLER *handle, API_CLIENT *apiClient);
int apiClientSetupAction(struct API_CLIENT_CMD_HANDLER *handle, API_CLIENT *apiClient);


int apiClientRs232Data(struct API_CLIENT_CMD_HANDLER *handle, API_CLIENT *apiClient);
int apiClientSecurityCheck(struct API_CLIENT_CMD_HANDLER *handle, API_CLIENT *apiClient);


int apiClientFind(struct API_CLIENT_CMD_HANDLER *handle, API_CLIENT *apiClient);

int	apiClientParseSubOptions(char *optarg, struct API_PARAMETERS *clientParams);


/* common */
int apiSendout(API_CLIENT *apiClient, void *data, unsigned int size);
int apiRecvCheck(struct sockaddr_in *peerAddress, void *data, unsigned int size);
int apiClientReceive(API_CLIENT *apiClient);

char apiClientCmdHeaderPrint(struct API_CLIENT_CMD_HANDLER *handle, API_CLIENT *apiClient);


#endif

