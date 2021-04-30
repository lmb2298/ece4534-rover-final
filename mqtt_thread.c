/*
 * mqtt_thread.c
 *
 *  Created on: Feb 22, 2021
 *      Author: Logan Bradbury
 */

#include <unistd.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <ti/drivers/net/wifi/simplelink.h>

/* TI Driver Includes */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/Timer.h>

/* Application Headers */
#include "network_if.h"
#include "mqtt_if.h"

#include <mqtt_send_queue.h>
#include <decode_queue.h>
#include <debug.h>
#include <helpers.h>

/* Driver configuration */
#include "ti_drivers_config.h"

#define MQTT_MODULE_TASK_PRIORITY   2
#define MQTT_MODULE_TASK_STACK_SIZE 2048

#define SL_TASKSTACKSIZE            2048
#define SPAWN_TASK_PRIORITY         9

#define MQTT_WILL_TOPIC             "rover/Status"
#define MQTT_WILL_MSG               "disconnected"
#define MQTT_WILL_QOS               MQTT_QOS_2
#define MQTT_WILL_RETAIN            false

#define MQTT_CLIENT_PASSWORD        "mo9hgakm"
#define MQTT_CLIENT_USERNAME        "kpdhpbdc"
#define MQTT_CLIENT_KEEPALIVE       0
#define MQTT_CLIENT_CLEAN_CONNECT   true
#define MQTT_CLIENT_MQTT_V3_1       true
#define MQTT_CLIENT_BLOCKING_SEND   true

#ifndef MQTT_SECURE_CLIENT
#define MQTT_CONNECTION_FLAGS           MQTTCLIENT_NETCONN_URL
#define MQTT_CONNECTION_ADDRESS         "s1.airmqtt.com"
#define MQTT_CONNECTION_PORT_NUMBER     10013
#endif

extern int32_t ti_net_SlNet_initConfig();

Timer_Handle timer0;
int publish = 0;

/* Client ID                                                                 */
/* If ClientId isn't set, the MAC address of the device will be copied into  */
/* the ClientID parameter.                                                   */
char ClientId[13] = {'\0'};

MQTT_IF_InitParams_t mqttInitParams =
{
     MQTT_MODULE_TASK_STACK_SIZE,   // stack size for mqtt module - default is 2048
     MQTT_MODULE_TASK_PRIORITY      // thread priority for MQTT   - default is 2
};

MQTTClient_Will mqttWillParams =
{
     MQTT_WILL_TOPIC,    // will topic
     MQTT_WILL_MSG,      // will message
     MQTT_WILL_QOS,      // will QoS
     MQTT_WILL_RETAIN    // retain flag
};

MQTT_IF_ClientParams_t mqttClientParams =
{
     ClientId,                  // client ID
     MQTT_CLIENT_USERNAME,      // user name
     MQTT_CLIENT_PASSWORD,      // password
     MQTT_CLIENT_KEEPALIVE,     // keep-alive time
     MQTT_CLIENT_CLEAN_CONNECT, // clean connect flag
     MQTT_CLIENT_MQTT_V3_1,     // true = 3.1, false = 3.1.1
     MQTT_CLIENT_BLOCKING_SEND, // blocking send flag
     &mqttWillParams            // will parameters
};

MQTTClient_ConnParams mqttConnParams =
{
     MQTT_CONNECTION_FLAGS,         // connection flags
     MQTT_CONNECTION_ADDRESS,       // server address
     MQTT_CONNECTION_PORT_NUMBER,   // port number of MQTT server
     0,                             // method for secure socket
     0,                             // cipher for secure socket
     0,                             // number of files for secure connection
     NULL                           // secure files
};

//*****************************************************************************
//!
//! Set the ClientId with its own mac address
//! This routine converts the mac address which is given
//! by an integer type variable in hexadecimal base to ASCII
//! representation, and copies it into the ClientId parameter.
//!
//! \param  macAddress  -   Points to string Hex.
//!
//! \return void.
//!
//*****************************************************************************
int32_t SetClientIdNamefromMacAddress()
{
    int32_t ret = 0;
    uint8_t Client_Mac_Name[2];
    uint8_t Index;
    uint16_t macAddressLen = SL_MAC_ADDR_LEN;
    uint8_t macAddress[SL_MAC_ADDR_LEN];

    /*Get the device Mac address */
    ret = sl_NetCfgGet(SL_NETCFG_MAC_ADDRESS_GET, 0, &macAddressLen,
                       &macAddress[0]);

    /*When ClientID isn't set, use the mac address as ClientID               */
    if(ClientId[0] == '\0')
    {
        /*6 bytes is the length of the mac address                           */
        for(Index = 0; Index < SL_MAC_ADDR_LEN; Index++)
        {
            /*Each mac address byte contains two hexadecimal characters      */
            /*Copy the 4 MSB - the most significant character                */
            Client_Mac_Name[0] = (macAddress[Index] >> 4) & 0xf;
            /*Copy the 4 LSB - the least significant character               */
            Client_Mac_Name[1] = macAddress[Index] & 0xf;

            if(Client_Mac_Name[0] > 9)
            {
                /*Converts and copies from number that is greater than 9 in  */
                /*hexadecimal representation (a to f) into ascii character   */
                ClientId[2 * Index] = Client_Mac_Name[0] + 'a' - 10;
            }
            else
            {
                /*Converts and copies from number 0 - 9 in hexadecimal       */
                /*representation into ascii character                        */
                ClientId[2 * Index] = Client_Mac_Name[0] + '0';
            }
            if(Client_Mac_Name[1] > 9)
            {
                /*Converts and copies from number that is greater than 9 in  */
                /*hexadecimal representation (a to f) into ascii character   */
                ClientId[2 * Index + 1] = Client_Mac_Name[1] + 'a' - 10;
            }
            else
            {
                /*Converts and copies from number 0 - 9 in hexadecimal       */
                /*representation into ascii character                        */
                ClientId[2 * Index + 1] = Client_Mac_Name[1] + '0';
            }
        }
    }
    return(ret);
}

// this timer callback toggles the LED once per second until the device connects to an AP
void timerLEDCallback(Timer_Handle myHandle)
{
    GPIO_toggle(CONFIG_GPIO_LED_2);
}

void MQTT_EventCallback(int32_t event){
    switch(event){

        case MQTT_EVENT_CONNACK:
        {
            //LOG_INFO("MQTT_EVENT_CONNACK\r\n");
            break;
        }

        case MQTT_EVENT_SUBACK:
        {
            //LOG_INFO("MQTT_EVENT_SUBACK\r\n");
            break;
        }

        case MQTT_EVENT_PUBACK:
        {
            //LOG_INFO("MQTT_EVENT_PUBACK\r\n");
            break;
        }

        case MQTT_EVENT_UNSUBACK:
        {
            //LOG_INFO("MQTT_EVENT_UNSUBACK\r\n");
            break;
        }

        case MQTT_EVENT_CLIENT_DISCONNECT:
        {
            //LOG_INFO("MQTT_EVENT_CLIENT_DISCONNECT\r\n");
            break;
        }

        case MQTT_EVENT_SERVER_DISCONNECT:
        {
            //LOG_INFO("MQTT_EVENT_SERVER_DISCONNECT\r\n");
            break;
        }

        case MQTT_EVENT_DESTROY:
        {
            //LOG_INFO("MQTT_EVENT_DESTROY\r\n");
            break;
        }
    }
}

/*
 * Subscribe topic callbacks
 * Topic and payload data is deleted after topic callbacks return.
 * User must copy the topic or payload data if it needs to be saved.
 */
void MessageCB(char* topic, char* payload, uint8_t qos) {
    //LOG_INFO("TOPIC: %s PAYLOAD: %s QOS: %d\r\n", topic, payload, qos);
    DecodeMessage_t msg;
    GPIO_toggle(CONFIG_GPIO_LED_2);
    strncpy(msg.topic, topic, MAX_TOPIC_LEN);
    strncpy(msg.payload, payload, MAX_PAYLOAD_LEN);
    // make sure we always null terminate, because strncpy doesn't
    msg.topic[MAX_TOPIC_LEN] = '\0';
    msg.payload[MAX_PAYLOAD_LEN] = '\0';
    if (decodeQueueSend(msg) != pdTRUE) {
        dbgStopAll(ERROR_EVENT_MESSAGE_CALLBACK);
    }
}

void ToggleLED1CB(char* topic, char* payload, uint8_t qos) {
    GPIO_toggle(CONFIG_GPIO_LED_1);
    //LOG_INFO("TOPIC: %s PAYLOAD: %s QOS: %d\r\n", topic, payload, qos);
}

/* Publishes given messages to the given handle. */
void PublishMsg(MQTTClient_Handle handle, char* topic, char* payload, unsigned short payloadLen, int flags) {
	MQTT_IF_Publish(handle, topic, payload, payloadLen, flags);
}

/* Publish connected message and turn on green LED to display connected status */
void Connected(MQTTClient_Handle handle) {
	// Turn on green LED to show connection confirmed
    GPIO_write(CONFIG_GPIO_LED_2, CONFIG_GPIO_LED_ON);
	// Publish connected message
	PublishMsg(handle, "rover/Status", "connected", strlen("connected"), MQTT_QOS_2);
}

int WifiInit(){

    int32_t ret;
    SlWlanSecParams_t securityParams;
    pthread_t spawn_thread = (pthread_t) NULL;
    pthread_attr_t pattrs_spawn;
    struct sched_param pri_param;

    pthread_attr_init(&pattrs_spawn);
    pri_param.sched_priority = SPAWN_TASK_PRIORITY;
    ret = pthread_attr_setschedparam(&pattrs_spawn, &pri_param);
    ret |= pthread_attr_setstacksize(&pattrs_spawn, SL_TASKSTACKSIZE);
    ret |= pthread_attr_setdetachstate(&pattrs_spawn, PTHREAD_CREATE_DETACHED);
    ret = pthread_create(&spawn_thread, &pattrs_spawn, sl_Task, NULL);
    if(ret != 0){
        //LOG_ERROR("could not create simplelink task\n\r");
        //ERROR
    }

    Network_IF_ResetMCUStateMachine();

    Network_IF_DeInitDriver();

    ret = Network_IF_InitDriver(ROLE_STA);
    if(ret < 0){
        //LOG_ERROR("Failed to start SimpleLink Device\n\r");
        //ERROR
    }

    SetClientIdNamefromMacAddress();

    securityParams.Key = (signed char*)SECURITY_KEY;
    securityParams.KeyLen = strlen(SECURITY_KEY);
    securityParams.Type = SECURITY_TYPE;

    ret = Timer_start(timer0);
    if(ret < 0){
        //LOG_ERROR("failed to start the timer\r\n");
    }

    ret = Network_IF_ConnectAP(SSID_NAME, securityParams);
    if(ret < 0){
        //LOG_ERROR("Connection to an AP failed\n\r");
    }
    else{
        ret = sl_WlanProfileAdd((signed char*)SSID_NAME, strlen(SSID_NAME), 0, &securityParams, NULL, 7, 0);
        if(ret < 0){
            //LOG_ERROR("failed to add profile %s\r\n", SSID_NAME);
        }
        else{
            //LOG_INFO("profile added %s\r\n", SSID_NAME);
        }
    }

    Timer_stop(timer0);
    Timer_close(timer0);

    return ret;
}

void mqtt_thread(void * args){

    int32_t ret;
    Timer_Params params;
    MQTTClient_Handle mqttClientHandle;
	MQTTMsg message;

	dbgEvent(MQTT_ENTER);

    ret = ti_net_SlNet_initConfig();
    if(0 != ret)
    {
        dbgStopAll(ERROR_EVENT_MQTT_SLNET_CONF);
    }

    // configuring the timer to toggle an LED until the AP is connected
    Timer_Params_init(&params);
    params.period = 1000000;
    params.periodUnits = Timer_PERIOD_US;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = (Timer_CallBackFxn)timerLEDCallback;

    timer0 = Timer_open(CONFIG_TIMER_1, &params);
    if (timer0 == NULL) {
        dbgStopAll(ERROR_EVENT_MQTT_LED_TIMER_OPEN);
    }

    ret = WifiInit();
    if(ret < 0){
        dbgStopAll(ERROR_EVENT_MQTT_WIFI_INIT);
    }

    ret = MQTT_IF_Init(mqttInitParams);
    if(ret < 0){
        dbgStopAll(ERROR_EVENT_MQTT_IF_INIT);
    }

    mqttClientHandle = MQTT_IF_Connect(mqttClientParams, mqttConnParams, MQTT_EventCallback);
    if(mqttClientHandle < 0){
        dbgStopAll(ERROR_EVENT_MQTT_IF_CONNECT);
    }
	
	Connected(mqttClientHandle);
	
	/* SUBSCRIBE HERE */
    ret = MQTT_IF_Subscribe(mqttClientHandle, "rover/ToggleLED", MQTT_QOS_2, ToggleLED1CB);
	ret |= MQTT_IF_Subscribe(mqttClientHandle, "rover/navData", MQTT_QOS_2, MessageCB);
    ret |= MQTT_IF_Subscribe(mqttClientHandle, "rover/sensorsData", MQTT_QOS_2, MessageCB);
	ret |= MQTT_IF_Subscribe(mqttClientHandle, "rover/armData", MQTT_QOS_2, MessageCB);
    if(ret < 0){
        dbgStopAll(ERROR_EVENT_MQTT_SUBSCRIBE);
    }

    //timer500Init();
    // timer70Init();
    dbgEvent(MQTT_ENTER_LOOP);
    while(1){
		if (mqttSendQueueReceive(&message) != pdTRUE) {
			dbgStopAll(ERROR_EVENT_MQTT_RECEIVE);
        }
		PublishMsg(mqttClientHandle, message.topic, message.payload, strlen(message.payload), MQTT_QOS_2);
		
    }
}

