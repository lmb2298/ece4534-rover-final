/*
 * pid_thread.c
 *
 *  Created on: Mar 17, 2021
 *      Author: Logan Bradbury
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "ti_drivers_config.h"

#include <pid_thread.h>
#include <encode_queue.h>
#include <pid_queue.h>
#include <action_queue.h>
#include <debug.h>
#include <helpers.h>
#include <timer500.h>

/* Message variables */
static PID_Message_t received;
static EncodeMessage_t mqttMsg;
static ActionMessage_t actionMsg;

void *pidThread(void *arg0)
{
	//int Kp, Ki, dt;
	//int e1, e2, e3;
	//static int i1; i2, i3;
	
	dbgEvent(SENSOR_ENTER);
	
	while (1) {
		dbgEvent(SENSOR_QUEUE_BEFORE_SEND);
		if (pidQueueReceive(&received) != pdTRUE) {
			dbgStopAll(ERROR_EVENT_SENSOR_RECEIVE);
		}
		
		// Do stuff here
		/*e1 = target.enc1 - recevied.enc1;
		e2 = target.enc2 - recevied.enc2;
		e3 = target.enc3 - recevied.enc3;
		
		i1 = i1 + e1*dt;
		i2 = i2 + e2*dt;
		i3 = i3 + e3*dt;*/
		
		// Send Messages
		actionMsg.topic = PID;
		actionMsg.pidData = 1;//Kp*e1 + Ki*i1;
		actionQueueSend(actionMsg);
		
		//mqttMsg.topic = ENCODE_TOPIC_PID;
		//mqttMsg.pidMsg = received;
		//encodeQueueSend(mqttMsg);
		dbgEvent(SENSOR_QUEUE_AFTER_SEND);
	}
}
