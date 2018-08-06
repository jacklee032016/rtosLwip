/*
* 
*/

#ifndef __MQTT_CLIENT_H__
#define __MQTT_CLIENT_H__


#include "lwipMux.h"
#include "lwip/apps/mqtt.h"

#define	MQTT_CLIENT_DEBUG		MUX_DBG_OFF


void example_do_connect(mqtt_client_t *client, const char * topic);
void example_publish(mqtt_client_t *client, void *arg);


#endif


