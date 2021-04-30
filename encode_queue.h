/*
 * encode_queue.h
 *
 *  Created on: Mar 6, 2021
 *      Author: Ian Boll
 */

#ifndef ENCODE_QUEUE_H_
#define ENCODE_QUEUE_H_

#include <stddef.h>
#include <mjson.h>
#include <FreeRTOS.h>
#include <action_thread.h>

#include <motor_queue.h>
#include <pid_queue.h>
#include <action_queue.h>

typedef enum EncodeMessageTopic {
    ENCODE_TOPIC_STATE,
	ENCODE_TOPIC_MOTOR,
	ENCODE_TOPIC_PID,
	ENCODE_TOPIC_ACTION,
    ENCODE_TOPIC_XXX1
} EncodeMessageTopic_t;

typedef struct EncodeMessage {
    RoverVariables_t     rover;
	MotorMessage_t		 motorMsg;
	PID_Message_t		 pidMsg;
	ActionMessage_t		 actionMsg;
    EncodeMessageTopic_t topic;
} EncodeMessage_t;

/**
 * Initializes the internal queue. Must be called before using any other functions from this header.
 * Returns pdTRUE on success and pdFALSE on failure.
 */
extern BaseType_t encodeQueueCreate();

/**
 * Sends the message on the queue in a blocking manner.
 * Returns pdTRUE if the message was successfully sent. Otherwise, may return any other value.
 */
extern BaseType_t encodeQueueSend(EncodeMessage_t message);

/**
 * Reads a message from the queue into the message buffer. This function blocks execution.
 * Returns pdTRUE if successful, otherwise pdFALSE.
 */
extern BaseType_t encodeQueueReceive(EncodeMessage_t *message);

#endif /* ENCODE_QUEUE_H_ */
