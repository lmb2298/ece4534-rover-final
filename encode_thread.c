/*
 * encode_thread.c
 *
 *  Created on: Feb 10, 2021
 *      Author: Ian Boll
 */

#include <string.h>

#include <ti/drivers/Timer.h>
#include "ti_drivers_config.h"

#include <encode_queue.h>
#include <mqtt_send_queue.h>
#include <debug.h>
#include <helpers.h>

static EncodeMessage_t received;
static MQTTMsg outbound;

/**
 * Returns true if a message was recognized and successfully encoded.
 **/
bool encodeMessage();

void *encodeThread(void *arg0)
{
    dbgEvent(ENCODE_ENTER);
    while (1) {
        if (encodeQueueReceive(&received) != pdTRUE) {
            dbgStopAll(ERROR_EVENT_SENSOR_RECEIVE);
        }
        if (encodeMessage()) {
            mqttSendQueueSend(outbound);
        }
		else dbgStopAll(ERROR_EVENT_SENSOR_RECEIVE);
    }
}

bool encodeMessage()
{
    //char *state;
	dbgEvent(ENCODE_ENTER_LOOP);
	
	switch (received.topic) {
    case ENCODE_TOPIC_STATE:
		outbound.topic = "rover/state";
		if (received.rover.state == IDLE)
			snprintf(outbound.payload, sizeof(outbound.payload), "{\"state\":IDLE}");
		else if (received.rover.state == MOVING)
			snprintf(outbound.payload, sizeof(outbound.payload), "{\"state\":MOVING}");
		else if (received.rover.state == TURNING)
			snprintf(outbound.payload, sizeof(outbound.payload), "{\"state\":TURNING}");
		else if (received.rover.state == WAITING)
			snprintf(outbound.payload, sizeof(outbound.payload), "{\"state\":WAITING}");
        break;
	case ENCODE_TOPIC_MOTOR:
		outbound.topic = "rover/motor";
		snprintf(outbound.payload, sizeof(outbound.payload), "{\"motor1\":%d, \"motor2\":%d, \"motor3\":%d}", 
				 received.motorMsg.m1.checksum, received.motorMsg.m2.checksum, received.motorMsg.m3.checksum);
		break;
	case ENCODE_TOPIC_PID:
		outbound.topic = "rover/pid";
		snprintf(outbound.payload, sizeof(outbound.payload), "{\"encoder1\":%d, \"encoder2\":%d, \"encoder3\":%d}", 
				 received.pidMsg.encoder1, received.pidMsg.encoder2, received.pidMsg.encoder3);
		break;
	case ENCODE_TOPIC_ACTION:
		outbound.topic = "rover/action";
		if (received.actionMsg.topic == NAV)
			snprintf(outbound.payload, sizeof(outbound.payload), "{\"topic\":NAV}");
		else if (received.actionMsg.topic == SENSORS)
			snprintf(outbound.payload, sizeof(outbound.payload), "{\"topic\":SENSORS}");
		else if (received.actionMsg.topic == ARM)
			snprintf(outbound.payload, sizeof(outbound.payload), "{\"topic\":ARM}");
		else if (received.actionMsg.topic == PID)
			snprintf(outbound.payload, sizeof(outbound.payload), "{\"topic\":PID}");
		break;
    default:
        // don't send unrecognized messages
        return false;
    }
	
	
    // make sure we're always null terminated, including when doing strncpy
    outbound.payload[sizeof(outbound.payload) - 1] = '\0';
    return true;
}
