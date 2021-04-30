/*
 * encode_thread.c
 *
 *  Created on: Mar 6, 2021
 *      Author: Ian Boll
 */

#include <FreeRTOS.h>
#include <queue.h>
#include <debug.h>
#include <encode_queue.h>

static QueueHandle_t queue_handle = NULL;

BaseType_t encodeQueueCreate()
{
    queue_handle = xQueueCreate(10, sizeof(EncodeMessage_t));
    if (queue_handle == NULL) {
        return pdFALSE;
    }
    return pdTRUE;
}

BaseType_t encodeQueueSend(EncodeMessage_t message)
{
    dbgEvent(ENCODE_QUEUE_BEFORE_SEND);
    BaseType_t status = xQueueSendToBack(queue_handle, &message, portMAX_DELAY);
    return status;
}

BaseType_t encodeQueueReceive(EncodeMessage_t *message)
{
    dbgEvent(ENCODE_QUEUE_BEFORE_RECEIVE);
    BaseType_t status = xQueueReceive(queue_handle, (void *)message, portMAX_DELAY);
    return status;
}
