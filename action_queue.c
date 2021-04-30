/*
 * action_queue.c
 *
 *  Created on: Mar 17, 2021
 *      Author: Logan Bradbury
 */
 
 #include <action_queue.h>
 
 static QueueHandle_t queue_handle = NULL;

BaseType_t actionQueueCreate()
{
    queue_handle = xQueueCreate(10, sizeof(ActionMessage_t));
    if (queue_handle == NULL) {
        return pdFALSE;
    }
    return pdTRUE;
}

BaseType_t actionQueueSend(ActionMessage_t message)
{
    dbgEvent(ENCODE_QUEUE_BEFORE_SEND);
    BaseType_t status = xQueueSendToBack(queue_handle, &message, portMAX_DELAY);
    return status;
}

BaseType_t actionQueueReceive(ActionMessage_t *message)
{
    dbgEvent(ENCODE_QUEUE_BEFORE_RECEIVE);
    BaseType_t status = xQueueReceive(queue_handle, (void *)message, portMAX_DELAY);
    return status;
}
