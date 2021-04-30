/*
 * pid_thread.c
 *
 *  Created on: Mar 17, 2021
 *      Author: Logan Bradbury
 */

#include <string.h>

#include "ti_drivers_config.h"

#include <pid_thread.h>

#include <encode_queue.h>
#include <pid_queue.h>
#include <action_queue.h>
#include <debug.h>
#include <helpers.h>
#include <timer500.h>

static PID_Message_t received;
static EncodeMessage_t mqttMsg;
static ActionMessage_t actionMsg;

void *pidThread(void *arg0)
{
	dbgEvent(SENSOR_ENTER);
	
	while (1) {
		dbgEvent(SENSOR_QUEUE_BEFORE_SEND);
		if (pidQueueReceive(&received) != pdTRUE) {
			dbgStopAll(ERROR_EVENT_SENSOR_RECEIVE);
		}
		
		// Do stuff here
		
		
		// Send Messages
		actionMsg.topic = PID;
		actionMsg.pidData = 1;
		actionQueueSend(actionMsg);
		
		//mqttMsg.topic = ENCODE_TOPIC_PID;
		//mqttMsg.pidMsg = received;
		//encodeQueueSend(mqttMsg);
		dbgEvent(SENSOR_QUEUE_AFTER_SEND);
	}
}
