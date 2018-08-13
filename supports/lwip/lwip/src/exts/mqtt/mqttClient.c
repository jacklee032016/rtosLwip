
#include "lwip/apps/mqtt.h"
#include "lwip/timeouts.h"

#include <string.h>

#include "lwipExt.h"

#if LWIP_EXT_MQTT_CLIENT

#define	MQTT_CLIENT_DEBUG		EXT_DBG_ON

#define	MQTT_CLIENT_NAME_SIZE			16

#define	MQTT_CLIENT_TIMEOUT_1S			1000*2

/* The idea is to demultiplex topic and create some reference to be used in data callbacks
   Example here uses a global variable, better would be to use a member in arg
   If RAM and CPU budget allows it, the easiest implementation might be to just take a copy of
   the topic string and use it in mqtt_incoming_data_cb
*/

const char *clientId = EXT_MQTT_CLIENT_ID;
const char *clientName = EXT_MQTT_USER_NAME;
const char *clientPasswd = EXT_MQTT_PASSWORD;

const char *clientTopic = EXT_MQTT_CLIENT_ID;

struct MQTT_CLIENT
{
	ip4_addr_t						svrIp;
	int								inpubId;
	const char						*topic;
	
	
	mqtt_client_t						client;
	struct mqtt_connect_client_info_t 	clientInfo;
};

static  struct MQTT_CLIENT _mqttClient;

static void _mqttClientTimer(void *arg)
{
	struct MQTT_CLIENT *_client = (struct MQTT_CLIENT *)arg;

	EXT_DEBUGF(MQTT_CLIENT_DEBUG, ("MqttClient timeout"));

	mqttClientConnect(_client->svrIp.addr);
}

/* Called when publish is complete either with sucess or failure */
static void _mqttClientPublishRequestCb(void *arg, err_t result)
{
	if(result != ERR_OK)
	{
		EXT_ERRORF(("Publish result: %d", result));
	}
}

static void mqttClientPublish(mqtt_client_t *client, void *arg)
{
	//const char *pub_payload= "Hola mundo de mierda!";
	const char *pub_payload= arg;
	err_t err;
	u8_t qos = 0; /* 0 1 or 2, see MQTT specification */
	u8_t retain = 0; /* No don't retain such crappy payload... */
	
	err = mqtt_publish(client, "placa", pub_payload, strlen(pub_payload), qos, retain, _mqttClientPublishRequestCb, arg);
	if(err != ERR_OK)
	{
		EXT_ERRORF(("Publish err: %d", err));
	}
}


static void _mqttClientIncomingPublishCb(void *arg, const char *topic, u32_t tot_len)
{
	struct MQTT_CLIENT *_client = (struct MQTT_CLIENT *) arg;
	EXT_DEBUGF(MQTT_CLIENT_DEBUG, ("Incoming publish at topic %s with total length %u", topic, (unsigned int)tot_len) );

	/* Decode topic string into a user defined reference */
	if(strcmp(topic, "print_payload") == 0)
	{
		_client->inpubId = 0;
	}
	else if(topic[0] == 'A')
	{
		/* All topics starting with 'A' might be handled at the same way */
		_client->inpubId = 1;
	}
	else if(strcmp(topic, "placa/led2") == 0)
	{
		_client->inpubId = 2;
	}
	else if(strcmp(topic, "placa/barra") == 0)
	{
		_client->inpubId = 3;
	}
	else
	{/* For all other topics */
		_client->inpubId = 9;
	}

}


static void _mqttClientIncomingDataCb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
	struct MQTT_CLIENT *_client = (struct MQTT_CLIENT *) arg;
	EXT_DEBUGF(MQTT_CLIENT_DEBUG, ("Incoming publish payload with length %d, flags %u", len, (unsigned int)flags));

	if(flags & MQTT_DATA_FLAG_LAST)
	{
		/* Last fragment of payload received (or whole part if payload fits receive buffer See MQTT_VAR_HEADER_BUFFER_LEN)  */

		/* Call function or do action depending on reference, in this case inpub_id */
		if(_client->inpubId == 0)
		{
			/* Don't trust the publisher, check zero termination */
			if(data[len-1] == 0)
			{
				EXT_DEBUGF(MQTT_CLIENT_DEBUG, ("mqtt_incoming_data: '%s'", (const char *)data));
			}
		}
		else if(_client->inpubId == 1)
		{
			/* Call an 'A' function... */
		}
		else if(_client->inpubId == 2)
		{
			if(strcmp((const char *)data, "0") == 0)
			{ //Then, turn off LED3
			}
			else if (strcmp((const char *)data, "1") == 0)
			{ //Then turn on LED3
			}
			/* Call an 'A' function... */
		}
		else if(_client->inpubId == 3)
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
			EXT_DEBUGF(MQTT_CLIENT_DEBUG, ("mqtt_incoming_data_cb: Ignoring payload..."));
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
	EXT_DEBUGF(MQTT_CLIENT_DEBUG, ("Subscribe result: %d", result));
	mqtt_client_t * client = (mqtt_client_t *)arg;

	mqttClientPublish(client, arg);
}


/* when connect is OK, register incoming PUBLISH/Data callback, and subscribe to one topics */
static void _mqttClientConnectionCb(mqtt_client_t * client, void * arg, mqtt_connection_status_t status)
{
	err_t err;
	struct MQTT_CLIENT *_client = (struct MQTT_CLIENT *) arg;

	if (status == MQTT_CONNECT_ACCEPTED)
	{
		EXT_DEBUGF(MQTT_CLIENT_DEBUG, ("mqtt_connection: Successfully connected"));

		/* Setup callback for incoming publish requests */
		mqtt_set_inpub_callback(client, _mqttClientIncomingPublishCb, _mqttClientIncomingDataCb, arg);

		/* Subscribe to a topic named "subtopic" with QoS level 1, call mqtt_sub_request_cb with result */
		err = mqtt_subscribe(client, _client->topic, 1, _mqttClientSubscribeRequestCb, client);
		if (err != ERR_OK)
		{
			EXT_ERRORF(("mqtt_subscribe return failed: %d", err));
		}
	}
	else
	{
		EXT_DEBUGF(MQTT_CLIENT_DEBUG, ("mqtt_connection: Disconnected, reason: %d", status));
		if(status == MQTT_CONNECT_TIMEOUT)
		{
			sys_timeout(MQTT_CLIENT_TIMEOUT_1S, _mqttClientTimer, _client);
		}
		else
		{
			/* Its more nice to be connected, so try to reconnect */
			mqttClientConnect(_client->svrIp.addr );
		}
	}
}


/* connect and register callback for connection */
void mqttClientConnect(unsigned int svrIp)
{
	err_t err;
	struct mqtt_connect_client_info_t *ci;
	struct MQTT_CLIENT *_client = &_mqttClient;

	memset(_client, 0, sizeof(struct MQTT_CLIENT));

	_client->topic = clientTopic;
	_client->svrIp.addr = svrIp;
	ci =  &_client->clientInfo;

	/* Minimal amount of information required is client identifier, so set it here */
	ci->client_id = clientId;
	ci->client_user = clientName;
	ci->client_pass = clientPasswd;

	/* Initiate client and connect to server, if this fails immediately an error code is returned
	otherwise mqtt_connection_cb will be called with connection result after attempting
	to establish a connection with the server.
	For now MQTT version 3.1.1 is always used */

	err = mqtt_client_connect(&_client->client, &_client->svrIp, MQTT_PORT, _mqttClientConnectionCb, _client, ci );
	/* For now just print the result code if something goes wrong */
	if (err != ERR_OK)
	{
		EXT_ERRORF(("mqtt_connect return failed %d", err));
	}

}

#endif

