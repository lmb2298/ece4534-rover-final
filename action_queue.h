/*
 * action_queue.h
 *
 *  Created on: Mar 17, 2021
 *      Author: Logan Bradbury
 */

#ifndef ACTION_QUEUE_H_
#define ACTION_QUEUE_H_

#include <stddef.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <debug.h>

typedef enum ActionMessageTopic {
	NAV,
	SENSORS,
	ARM,
	PID
} ActionMessageTopic_t;

typedef struct NavData {
	int distance;
	int heading;
} NavData_t;

typedef struct SensorData {
	int heading;
	int collision;
} SensorData_t;

typedef struct ActionMessage {
	ActionMessageTopic_t topic;
	NavData_t            navData;
	SensorData_t         sensorData;
	int                  armState;
	int                  pidData;
	
} ActionMessage_t;

/**
 * Initializes the internal queue. Must be called before using any other functions from this header.
 * Returns pdTRUE on success and pdFALSE on failure.
 */
extern BaseType_t actionQueueCreate();

/**
 * Sends the message on the queue in a blocking manner.
 * Returns pdTRUE if the message was successfully sent. Otherwise, may return any other value.
 */
extern BaseType_t actionQueueSend(ActionMessage_t message);

/**
 * Reads a message from the queue into the message buffer. This function blocks execution.
 * Returns pdTRUE if successful, otherwise pdFALSE.
 */
extern BaseType_t actionQueueReceive(ActionMessage_t *message);

#endif /* ACTION_QUEUE_H_ */
