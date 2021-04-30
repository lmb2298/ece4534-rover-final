/*
 * motor_queue.h
 *
 *  Created on: Mar 17, 2021
 *      Author: Logan Bradbury
 */

#ifndef MOTOR_QUEUE_H_
#define MOTOR_QUEUE_H_

#include <stddef.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <debug.h>

typedef struct MotorPacket {
	unsigned char address;
	unsigned char command;
	unsigned char data;
	unsigned char checksum;
} MotorPacket_t;

typedef struct MotorMessage {
	MotorPacket_t m1;
	MotorPacket_t m2;
	MotorPacket_t m3;
} MotorMessage_t;

/**
 * Initializes the internal queue. Must be called before using any other functions from this header.
 * Returns pdTRUE on success and pdFALSE on failure.
 */
extern BaseType_t motorQueueCreate();

/**
 * Sends the message on the queue in a blocking manner.
 * Returns pdTRUE if the message was successfully sent. Otherwise, may return any other value.
 */
extern BaseType_t motorQueueSend(MotorMessage_t message);

/**
 * Reads a message from the queue into the message buffer. This function blocks execution.
 * Returns pdTRUE if successful, otherwise pdFALSE.
 */
extern BaseType_t motorQueueReceive(MotorMessage_t *message);

#endif /* MOTOR_QUEUE_H_ */
