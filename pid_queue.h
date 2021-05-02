/*
 * pid_queue.h
 *
 *  Created on: Mar 17, 2021
 *      Author: Logan Bradbury
 */

#ifndef PID_QUEUE_H_
#define PID_QUEUE_H_

#include <stddef.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <debug.h>

typedef struct PID_Message {
	int encoder1;
	int encoder2;
	int encoder3;
	int e1;
	int e2;
	int e3;
} PID_Message_t;

/**
 * Initializes the internal queue. Must be called before using any other functions from this header.
 * Returns pdTRUE on success and pdFALSE on failure.
 */
extern BaseType_t pidQueueCreate();

/**
 * Sends the message on the queue in a non-blocking manner.
 * Returns pdTRUE if the message was successfully sent. Otherwise, may return any other value.
 */
extern BaseType_t pidQueueSendISR(PID_Message_t message);

/**
 * Reads a message from the queue into the message buffer. This function blocks execution.
 * Returns pdTRUE if successful, otherwise pdFALSE.
 */
extern BaseType_t pidQueueReceive(PID_Message_t *message);

#endif /* PID_QUEUE_H_ */
