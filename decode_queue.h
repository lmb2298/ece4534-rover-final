/*
 * decode_queue.h
 *
 *  Created on: Mar 4, 2021
 *      Author: Ian Boll
 */

#ifndef DECODE_QUEUE_H_
#define DECODE_QUEUE_H_

#include <FreeRTOS.h>

// The maximum number of characters that can make up a topic name or payload.
// This doesn't count the null terminator, which is always expected.
#define MAX_TOPIC_LEN   39
#define MAX_PAYLOAD_LEN 99

typedef struct DecodeMessage {
    char topic[MAX_TOPIC_LEN + 1];
    char payload[MAX_PAYLOAD_LEN + 1];
} DecodeMessage_t;

/**
 * Initializes the internal queue. Must be called before using any other functions from this header.
 * Returns pdTRUE on success and pdFALSE on failure.
 */
extern BaseType_t decodeQueueCreate();

/**
 * Sends the message on the queue in a non-blocking manner.
 * Returns pdTRUE if the message was successfully sent. Otherwise, may return any other value.
 */
extern BaseType_t decodeQueueSend(DecodeMessage_t message);

/**
 * Reads a message from the queue into the message buffer. This function blocks execution.
 * Returns pdTRUE if successful, otherwise pdFALSE.
 */
extern BaseType_t decodeQueueReceive(DecodeMessage_t *message);

#endif /* DECODE_QUEUE_H_ */
