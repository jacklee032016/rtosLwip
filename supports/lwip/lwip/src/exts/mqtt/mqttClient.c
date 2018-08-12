#include "lwip/apps/mqtt.h"
#include <string.h>

#include "board.h"


/* The idea is to demultiplex topic and create some reference to be used in data callbacks
   Example here uses a global variable, better would be to use a member in arg
   If RAM and CPU budget allows it, the easiest implementation might be to just take a copy of
   the topic string and use it in mqtt_incoming_data_cb
*/
static int inpub_id;

static void _mqttClientIncomingPublishCb(void *arg, const char *topic, u32_t tot_len)
{
	printf("Incoming publish at topic %s with total length %u\n", topic, (unsigned int)tot_len);

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
	printf("Incoming publish payload with length %d, flags %u\n", len, (unsigned int)flags);

	// }
	if(flags & MQTT_DATA_FLAG_LAST)
	{
		/* Last fragment of payload received (or whole part if payload fits receive buffer See MQTT_VAR_HEADER_BUFFER_LEN)  */

		/* Call function or do action depending on reference, in this case inpub_id */
		if(inpub_id == 0)
		{
			/* Don't trust the publisher, check zero termination */
			if(data[len-1] == 0)
			{
				printf("mqtt_incoming_data_cb: %s\n", (const char *)data);
			}
		}
		else if(inpub_id == 1)
		{
			/* Call an 'A' function... */
		}
		else if(inpub_id == 2)
		{
			if(strcmp(data, "0") == 0){ //Then, turn off LED3
			Board_LED_Set(LEDS_LED3, true);
			}else if (strcmp(data, "1" == 0)){ //Then turn on LED3
			Board_LED_Set(LEDS_LED3, false);
			}
			/* Call an 'A' function... */
		}
		else if(inpub_id == 3)
		{
			/* Not yet done. It's suppossed to turn on/off */
			if(strcmp(data, "0") == 0)
			{ //Then, turn off LED2
				Board_LED_Set(LEDS_LED1, true);
				Board_LED_Set(LEDS_LED2, true);
				Board_LED_Set(LEDS_LED3, true);
				Board_LED_Set(LEDS_LED4, true);
				Board_LED_Set(LEDS_LED5, true);
				Board_LED_Set(LEDS_LED6, true);
				Board_LED_Set(LEDS_LED7, true);
				Board_LED_Set(LEDS_LED8, true);
			}
			else if(strcmp(data, "1") == 0){ //Then turn on LED2
				Board_LED_Set(LEDS_LED1, false);
				Board_LED_Set(LEDS_LED2, true);
				Board_LED_Set(LEDS_LED3, true);
				Board_LED_Set(LEDS_LED4, true);
				Board_LED_Set(LEDS_LED5, true);
				Board_LED_Set(LEDS_LED6, true);
				Board_LED_Set(LEDS_LED7, true);
				Board_LED_Set(LEDS_LED8, true);
			}
			else if(strcmp(data, "2") == 0){ //Then turn on LED2
				Board_LED_Set(LEDS_LED1, false);
				Board_LED_Set(LEDS_LED2, false);
				Board_LED_Set(LEDS_LED3, true);
				Board_LED_Set(LEDS_LED4, true);
				Board_LED_Set(LEDS_LED5, true);
				Board_LED_Set(LEDS_LED6, true);
				Board_LED_Set(LEDS_LED7, true);
				Board_LED_Set(LEDS_LED8, true);
			}
		}
		else
		{
			printf("mqtt_incoming_data_cb: Ignoring payload...\n");
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
	printf("Subscribe result: %d\n", result);
}


/* when connect is OK, register incoming PUBLISH/Data callback, and subscribe to one topics */
static void _mqttClientConnectionCb(mqtt_client_t * client, void * arg, mqtt_connection_status_t status)
{
	err_t err;
	const char * topico = arg;

	if (status == MQTT_CONNECT_ACCEPTED)
	{
		printf("mqtt_connection_cb: Successfully connected\n");

		/* Setup callback for incoming publish requests */
		mqtt_set_inpub_callback(client, _mqttClientIncomingPublishCb, _mqttClientIncomingDataCb, arg);

		/* Subscribe to a topic named "subtopic" with QoS level 1, call mqtt_sub_request_cb with result */
		err = mqtt_subscribe(client, topico, 1, _mqttClientSubscribeRequestCb, arg);
		if (err != ERR_OK)
		{
			printf("mqtt_subscribe return: %d\n", err);
		}
	}
	else
	{
		printf("mqtt_connection_cb: Disconnected, reason: %d\n", status);
		/* Its more nice to be connected, so try to reconnect */
		//mqtt_do_connect(client);
		mqttClientConnect(client);
	}
}


/* connect and register callback for connection */
void mqttClientConnect(mqtt_client_t * client)
{
	ip4_addr_t broker_ipaddr;
	struct mqtt_connect_client_info_t ci;
	err_t err;

	IP4_ADDR( & broker_ipaddr, configBroker_ADDR0, configBroker_ADDR1, configBroker_ADDR2, configBroker_ADDR3);
	IP4_ADDR(&broker_ipaddr, 192, 168, 1, 50);

	/* Setup an empty client info structure */
	memset( & ci, 0, sizeof(ci));

	/* Minimal amount of information required is client identifier, so set it here */
	ci.client_id = configMQTT_CLIENT_NAME;
	ci.client_user = configMQTT_CLIENT_USER;
	ci.client_pass = configMQTT_CLIENT_PWD;

	/* Initiate client and connect to server, if this fails immediately an error code is returned
	otherwise mqtt_connection_cb will be called with connection result after attempting
	to establish a connection with the server.
	For now MQTT version 3.1.1 is always used */

	err = mqtt_client_connect(client, & broker_ipaddr, MQTT_PORT, _mqttClientConnectionCb, 0, & ci);
	/* For now just print the result code if something goes wrong */
	if (err != ERR_OK)
	{
		printf("mqtt_connect return %d\n", err);
	}

}


/* Called when publish is complete either with sucess or failure */
static void _mqttClientPublishRequestCb(void *arg, err_t result)
{
	if(result != ERR_OK)
	{
		printf("Publish result: %d\n", result);
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
		printf("Publish err: %d\n", err);
	}
}


