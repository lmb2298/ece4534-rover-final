/*
 * decode_queue.c
 *
 *  Created on: Mar 4, 2021
 *      Author: Ian Boll
 */

#include <FreeRTOS.h>
#include <queue.h>
#include <decode_queue.h>
#include <debug.h>

static QueueHandle_t queue_handle = NULL;

BaseType_t decodeQueueCreate()
{
    queue_handle = xQueueCreate(10, sizeof(DecodeMessage_t));
    if (queue_handle == NULL) {
        return pdFALSE;
    }
    return pdTRUE;
}

BaseType_t decodeQueueSend(DecodeMessage_t message)
{
    dbgEvent(DECODE_QUEUE_BEFORE_SEND);
    BaseType_t status = xQueueSendToBack(queue_handle, &message, portMAX_DELAY);
    return status;
}

BaseType_t decodeQueueReceive(DecodeMessage_t *message)
{
    dbgEvent(DECODE_QUEUE_BEFORE_RECEIVE);
    BaseType_t status = xQueueReceive(queue_handle, (void *)message, portMAX_DELAY);
    return status;
}
