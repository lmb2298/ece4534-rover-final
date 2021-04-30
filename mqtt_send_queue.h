/*
 * mqtt_send_queue.h
 *
 *  Created on: Feb 22, 2021
 *      Author: Logan Bradbury
 */

#ifndef MQTT_SEND_QUEUE_H_
#define MQTT_SEND_QUEUE_H_

#include <stddef.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <debug.h>

typedef struct MQTTMsg {
	char* topic;
	char payload[100];
} MQTTMsg;

/**
 * Initializes the internal queue. Must be called before using any other functions from this header.
 * Returns pdTRUE on success and pdFALSE on failure.
 */
extern BaseType_t mqttSendQueueCreate();

/**
 * Sends the message on the queue in a blocking manner.
 * Returns pdTRUE if the message was successfully sent. Otherwise, may return any other value.
 */
extern BaseType_t mqttSendQueueSend(MQTTMsg message);

/**
 * Reads a message from the queue into the message buffer. This function blocks execution.
 * Returns pdTRUE if successful, otherwise pdFALSE.
 */
extern BaseType_t mqttSendQueueReceive(MQTTMsg *message);

#endif /* MQTT_SEND_QUEUE_H_ */
