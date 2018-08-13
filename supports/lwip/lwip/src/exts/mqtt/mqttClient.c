
#include "lwip/apps/mqtt.h"
#include <string.h>

#include "lwipExt.h"

#if LWIP_EXT_MQTT_CLIENT

#define	MQTT_CLIENT_DEBUG		MUX_DBG_OFF

/* The idea is to demultiplex topic and create some reference to be used in data callbacks
   Example here uses a global variable, better would be to use a member in arg
   If RAM and CPU budget allows it, the easiest implementation might be to just take a copy of
   the topic string and use it in mqtt_incoming_data_cb
*/
static int inpub_id;

/* Called when publish is complete either with sucess or failure */
static void _mqttClientPublishRequestCb(void *arg, err_t result)
{
	if(result != ERR_OK)
	{
		MUX_ERRORF(("Publish result: %d", result));
	}
}

void mqttClientPublish(mqtt_client_t *client, void *arg)
{
	//const char *pub_payload= "Hola mundo de mierda!";
	const char *pub_payload= arg;
	err_t err;
	u8_t qos = 0; /* 0 1 or 2, see MQTT specification */
	u8_t retain = 0; /* No don't retain such crappy payload... */
	
	err = mqtt_publish(client, "placa", pub_payload, strlen(pub_payload), qos, retain, _mqttClientPublishRequestCb, arg);
	if(err != ERR_OK)
	{
		MUX_ERRORF(("Publish err: %d", err));
	}
}


static void _mqttClientIncomingPublishCb(void *arg, const char *topic, u32_t tot_len)
{
	MUX_DEBUGF(MQTT_CLIENT_DEBUG, ("Incoming publish at topic %s with total length %u", topic, (unsigned int)tot_len) );

	/* Decode topic string into a user defined reference */
	if(strcmp(topic, "print_payload") == 0)
	{
		inpub_id = 0;
	}
	else if(topic[0] == 'A')
	{
		/* All topics starting with 'A' might be handled at the same way */
		inpub_id = 1;
	}
	else if(strcmp(topic, "placa/led2") == 0)
	{
		inpub_id = 2;
	}
	else if(strcmp(topic, "placa/barra") == 0)
	{
		inpub_id = 3;
	}
	else
	{/* For all other topics */
		inpub_id = 9;
	}

}


static void _mqttClientIncomingDataCb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
	MUX_DEBUGF(MQTT_CLIENT_DEBUG, ("Incoming publish payload with length %d, flags %u", len, (unsigned int)flags));

	if(flags & MQTT_DATA_FLAG_LAST)
	{
		/* Last fragment of payload received (or whole part if payload fits receive buffer See MQTT_VAR_HEADER_BUFFER_LEN)  */

		/* Call function or do action depending on reference, in this case inpub_id */
		if(inpub_id == 0)
		{
			/* Don't trust the publisher, check zero termination */
			if(data[len-1] == 0)
			{
				MUX_DEBUGF(MQTT_CLIENT_DEBUG, ("mqtt_incoming_data: '%s'", (const char *)data));
			}
		}
		else if(inpub_id == 1)
		{
			/* Call an 'A' function... */
		}
		else if(inpub_id == 2)
		{
			if(strcmp((const char *)data, "0") == 0)
			{ //Then, turn off LED3
			}
			else if (strcmp((const char *)data, "1") == 0)
			{ //Then turn on LED3
			}
			/* Call an 'A' function... */
		}
		else if(inpub_id == 3)
		{
			/* Not yet done. It's suppossed to turn on/off */
			if(strcmp((const char *)data, "0") == 0)
			{ //Then, turn off LED2
			}
			else if(strcmp((const char *)data, "1") == 0)
			{ //Then turn on LED2
			}
			else if(strcmp((const char *)data, "2") == 0)
			{ //Then turn on LED2
			}
		}
		else
		{
			MUX_DEBUGF(MQTT_CLIENT_DEBUG, ("mqtt_incoming_data_cb: Ignoring payload..."));
		}
	}
	else
	{
		/* Handle fragmented payload, store in buffer, write to file or whatever */
	}

}


static void _mqttClientSubscribeRequestCb(void *arg, err_t result)
{
	/* Just print the result code here for simplicity,
	 normal behaviour would be to take some action if subscribe fails like
	 notifying user, retry subscribe or disconnect from server */
	MUX_DEBUGF(MQTT_CLIENT_DEBUG, ("Subscribe result: %d", result));
}


/* when connect is OK, register incoming PUBLISH/Data callback, and subscribe to one topics */
static void _mqttClientConnectionCb(mqtt_client_t * client, void * arg, mqtt_connection_status_t status)
{
	err_t err;
	const char * topico = arg;
	ip4_addr_t *brokerIp = (ip4_addr_t *)arg;

	if (status == MQTT_CONNECT_ACCEPTED)
	{
		MUX_DEBUGF(MQTT_CLIENT_DEBUG, ("mqtt_connection: Successfully connected"));

		/* Setup callback for incoming publish requests */
		mqtt_set_inpub_callback(client, _mqttClientIncomingPublishCb, _mqttClientIncomingDataCb, arg);

		/* Subscribe to a topic named "subtopic" with QoS level 1, call mqtt_sub_request_cb with result */
		err = mqtt_subscribe(client, topico, 1, _mqttClientSubscribeRequestCb, arg);
		if (err != ERR_OK)
		{
			MUX_ERRORF(("mqtt_subscribe return failed: %d", err));
		}
	}
	else
	{
		MUX_DEBUGF(MQTT_CLIENT_DEBUG, ("mqtt_connection: Disconnected, reason: %d", status));
		/* Its more nice to be connected, so try to reconnect */
		//mqtt_do_connect(client);
		mqttClientConnect(client, brokerIp->addr);
	}
}


/* connect and register callback for connection */
void mqttClientConnect(mqtt_client_t * client, unsigned int svrIp)
{
	ip4_addr_t brokerIp;
	struct mqtt_connect_client_info_t ci;
	err_t err;

	brokerIp.addr = svrIp;

	/* Setup an empty client info structure */
	memset( & ci, 0, sizeof(ci));

	/* Minimal amount of information required is client identifier, so set it here */
	ci.client_id = EXT_MQTT_CLIENT_ID;
	ci.client_user = EXT_MQTT_USER_NAME;
	ci.client_pass = EXT_MQTT_PASSWORD;

	/* Initiate client and connect to server, if this fails immediately an error code is returned
	otherwise mqtt_connection_cb will be called with connection result after attempting
	to establish a connection with the server.
	For now MQTT version 3.1.1 is always used */

	err = mqtt_client_connect(client, &brokerIp, MQTT_PORT, _mqttClientConnectionCb, &brokerIp, & ci);
	/* For now just print the result code if something goes wrong */
	if (err != ERR_OK)
	{
		MUX_ERRORF(("mqtt_connect return failed %d", err));
	}

}

#endif

