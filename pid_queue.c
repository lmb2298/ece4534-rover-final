/*
 * pid_queue.c
 *
 *  Created on: Mar 17, 2021
 *      Author: Logan Bradbury
 */
 
 #include <pid_queue.h>
 
 static QueueHandle_t queue_handle = NULL;

BaseType_t pidQueueCreate()
{
    queue_handle = xQueueCreate(10, sizeof(PID_Message_t));
    if (queue_handle == NULL) {
        return pdFALSE;
    }
    return pdTRUE;
}

BaseType_t pidQueueSendISR(PID_Message_t message)
{
    dbgEvent(ENCODE_QUEUE_BEFORE_SEND);
    BaseType_t status = xQueueSendToBackFromISR(queue_handle, &message, NULL);
    return status;
}

BaseType_t pidQueueReceive(PID_Message_t *message)
{
    dbgEvent(ENCODE_QUEUE_BEFORE_RECEIVE);
    BaseType_t status = xQueueReceive(queue_handle, (void *)message, portMAX_DELAY);
    return status;
}
