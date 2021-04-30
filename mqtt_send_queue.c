/*
 * mqtt_send_queue.c
 *
 *  Created on: Feb 22, 2021
 *      Author: Logan Bradbury
 */


#include <mqtt_send_queue.h>

static QueueHandle_t queue_handle = NULL;

BaseType_t mqttSendQueueCreate()
{
    queue_handle = xQueueCreate(10, sizeof(MQTTMsg));
    if (queue_handle == NULL) {
        return pdFALSE;
    }
    return pdTRUE;
}

BaseType_t mqttSendQueueSend(MQTTMsg message)
{
    dbgEvent(MQTT_QUEUE_BEFORE_SEND);
    BaseType_t status = xQueueSendToBack(queue_handle, &message, portMAX_DELAY);
    dbgEvent(MQTT_QUEUE_AFTER_SEND);
    return status;
}

BaseType_t mqttSendQueueReceive(MQTTMsg *message)
{
    dbgEvent(MQTT_QUEUE_BEFORE_RECEIVE);
    BaseType_t status = xQueueReceive(queue_handle, (void *)message, portMAX_DELAY);
    dbgEvent(MQTT_QUEUE_AFTER_RECEIVE);
    return status;
}

