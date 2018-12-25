
#include "ext.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ext2CfgFromFactory( EXT_SYS_T *extSys)
{
	extSys->magic[0] = EXT_MAGIC_VALUE_A;
	extSys->magic[1] = EXT_MAGIC_VALUE_B;

	extSys->version.major = BL_VERSION_MAJOR;
	extSys->version.minor = BL_VERSION_MINOR;
	extSys->version.revision = BL_VERSION_REVISION;
#if 0
	sprintf(extSys->name, "%s", EXT_767_PRODUCT_NAME);
	sprintf(extSys->model, "%s", EXT_767_MODEL);

	snprintf(extSys->user, EXT_USER_SIZE, "%s", EXT_USER);
	snprintf(extSys->password, EXT_PASSWORD_SIZE, "%s", EXT_PASSWORD);

	snprintf(extSys->superUser, EXT_USER_SIZE, "%s", EXT_SUPER_USER);
	snprintf(extSys->superPassword, EXT_PASSWORD_SIZE, "%s", EXT_SUPER_PASSWORD);
#endif

#if 1
//	extSys->isMCast = EXT_TRUE;
#else
	cfg->isMCast = EXT_FALSE;
#endif
//	extSys->ipMulticast = CFG_MAKEU32(MCAST_DEFAULT_IPADDR3, MCAST_DEFAULT_IPADDR2, MCAST_DEFAULT_IPADDR1, MCAST_DEFAULT_IPADDR0);

//	extSys->isUpdate = EXT_FALSE;

	extSys->macAddress.address[0] = ETHERNET_CONF_ETHADDR0;
	extSys->macAddress.address[1] = ETHERNET_CONF_ETHADDR1;
	extSys->macAddress.address[2] = ETHERNET_CONF_ETHADDR2;
	extSys->macAddress.address[3] = ETHERNET_CONF_ETHADDR3;
	extSys->macAddress.address[5] = ETHERNET_CONF_ETHADDR5;

	extSys->ipAddress = CFG_MAKEU32(ETHERNET_CONF_IPADDR3_TX, ETHERNET_CONF_IPADDR2_TX, ETHERNET_CONF_IPADDR1, ETHERNET_CONF_IPADDR0);
	

	extSys->netMode = EXT_TRUE;

//	extSys->httpPort = EXT_HTTP_SVR_PORT;


	extSys->endMagic[0] = EXT_MAGIC_VALUE_B;
	extSys->endMagic[1] = EXT_MAGIC_VALUE_A;
}



