/*
 * decode_thread.c
 *
 *  Created on: Mar 4, 2021
 *      Author: Ian Boll
 */

// Welcome to the JSON decode thread! If you're here, you probably want to add
// support for a new JSON message. There are a couple things you should know
// about how this code works.
// 1. We intentionally ignore invalid JSON and unknown topics. Do not call
//    `dbgStopAll` for these cases.
// 2. This file contains several comments with instructions on how to add
//    parsers. They start with "STEP #some number#" and continue from there.
//    Follow these instructions, and you should be able to easily add new parsers.

#include <ti/drivers/GPIO.h>
#include "ti_drivers_config.h"

#include <FreeRTOS.h>
#include <string.h>
#include <mjson.h>
#include <decode_thread.h>
#include <decode_queue.h>
#include <debug.h>
#include <helpers.h>

// STEP 1: Include the header for the queue you want to send to. This should
// also contain the definition for the message struct you want to build.
#include <action_queue.h>

// STEP 2: Add a function to this list, but with a returned pointer for your
// message type. This function will be called by the decode thread when it
// receives messages for your topic.
ActionMessage_t *decodeNavigationMessage(char *json_text);
ActionMessage_t *decodeSensorsMessage(char *json_text);
ActionMessage_t *decodeArmMessage(char *json_text);

static ActionMessage_t action_parse_buffer;

void *decodeThread(void *arg0)
{
    DecodeMessage_t received;
    BaseType_t send_status;
    dbgEvent(DECODE_ENTER);

    dbgEvent(DECODE_ENTER_LOOP);
    while (1) {
        if (decodeQueueReceive(&received) != pdTRUE) {
            dbgStopAll(ERROR_EVENT_DECODE_RECEIVE);
        }

        // STEP 3: Add an `else if` to this list for your topic. Feel free to
        // copy one of the existing cases, but make sure to change the function
        // calls.
        if (strcmp("rover/navData", received.topic) == 0) {
            ActionMessage_t *parsed = decodeNavigationMessage(received.payload);
            if (parsed != NULL) {
                send_status = actionQueueSend(*parsed);
            }
        }
		else if (strcmp("rover/sensorsData", received.topic) == 0) {
            ActionMessage_t *parsed = decodeSensorsMessage(received.payload);
            if (parsed != NULL) {
                send_status = actionQueueSend(*parsed);
            }
        }
		else if (strcmp("rover/armData", received.topic) == 0) {
            ActionMessage_t *parsed = decodeArmMessage(received.payload);
            if (parsed != NULL) {
                send_status = actionQueueSend(*parsed);
            }
        }
        else {
            GPIO_toggle(CONFIG_GPIO_LED_1);
        }
        // unknown subscriptions get ignored

        if (send_status != pdTRUE) {
            dbgStopAll(ERROR_EVENT_DECODE_SEND);
        }
    }
}

ActionMessage_t *decodeNavigationMessage(char *json_text)
{
    static const struct json_attr_t JSON_ACTION_PARSER[] = {
        {"distance", t_integer, .addr.integer = &action_parse_buffer.navData.distance},
		{"heading" , t_integer, .addr.integer = &action_parse_buffer.navData.heading},
        {NULL}, // null terminator. required by microjson
    };
	action_parse_buffer.topic = NAV;

    int status = json_read_object(json_text, JSON_ACTION_PARSER, NULL);
    if (status == 0) {
        return &action_parse_buffer;
    }
	
    return NULL;
}

ActionMessage_t *decodeSensorsMessage(char *json_text)
{
    static const struct json_attr_t JSON_ACTION_PARSER[] = {
        {"heading"  , t_integer, .addr.integer = &action_parse_buffer.sensorData.heading},
		{"collision", t_integer, .addr.integer = &action_parse_buffer.sensorData.collision},
        {NULL}, // null terminator. required by microjson
    };
	action_parse_buffer.topic = SENSORS;

    int status = json_read_object(json_text, JSON_ACTION_PARSER, NULL);
    if (status == 0) {
        return &action_parse_buffer;
    }
	
    return NULL;
}

ActionMessage_t *decodeArmMessage(char *json_text)
{
    static const struct json_attr_t JSON_ACTION_PARSER[] = {
        {"state", t_integer, .addr.integer = &action_parse_buffer.armState},
        {NULL}, // null terminator. required by microjson
    };
	action_parse_buffer.topic = ARM;

    int status = json_read_object(json_text, JSON_ACTION_PARSER, NULL);
    if (status == 0) {
        return &action_parse_buffer;
    }
	
    return NULL;
}

// STEP 4: Copy one of the above functions and use it as a template to
// implement your decoder.  For documentation on writing parsers, read
// 'http://www.catb.org/~esr/microjson/microjson.html'
