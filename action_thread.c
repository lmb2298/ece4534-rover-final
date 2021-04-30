/*
 * action_thread.c
 *
 *  Created on: Mar 17, 2021
 *      Author: Logan Bradbury
 */

#include <string.h>

#include "ti_drivers_config.h"

#include <action_thread.h>
#include <action_queue.h>
#include <encode_queue.h>
#include <motor_queue.h>
#include <debug.h>
#include <helpers.h>

#define M1 128
#define M2 129
#define M3 130

static ActionMessage_t  received;
static EncodeMessage_t  mqttMsg;
static MotorMessage_t   motorMsg;
static RoverVariables_t rover;
static NavData_t		target;

BaseType_t determineAction();
MotorPacket_t setMotorSpeed(unsigned char address, unsigned char command, unsigned char data);

void *actionThread(void *arg0)
{
	rover.heading = 0;
	while (1) {
		if (actionQueueReceive(&received) != pdTRUE) {
            dbgStopAll(ERROR_EVENT_SENSOR_RECEIVE);
        }
        if (determineAction()) {
			encodeQueueSend(mqttMsg);
			motorQueueSend(motorMsg);
		}
		
		//mqttMsg.topic = ENCODE_TOPIC_ACTION;
		//mqttMsg.actionMsg = received;
		//encodeQueueSend(mqttMsg);
	}
}

BaseType_t determineAction()
{
	if (received.topic == SENSORS) {
		rover.heading = received.sensorData.heading;
	}
	
	switch (rover.state) {
		case IDLE:
			if (received.topic == NAV) {
				target = received.navData;
			}
			else if (received.topic == ARM && received.armState == 1) {
				rover.state = WAITING;
			}
			
			if (target.heading != rover.heading) {
				rover.speed = 30;
				rover.state = TURNING;
			}
			else if (target.distance > 0) {
				target.distance = 0;
				rover.speed = 40;
				rover.state = MOVING;
			}
			break;
		
		case MOVING:
			if (received.topic == PID) {
				rover.speed = rover.speed - received.pidData;
				if (rover.speed <= 30) {
					target.distance = 0;
					rover.speed = 0;
					rover.state = IDLE;
				}
			}
			else if (received.topic == SENSORS && received.sensorData.collision == 1) {
				rover.speed = 0;
				rover.state = IDLE;
			}
			
			motorMsg.m1 = setMotorSpeed(M1, 1, rover.speed);
			motorMsg.m2 = setMotorSpeed(M2, 0, rover.speed);
			motorMsg.m3 = setMotorSpeed(M3, 0, 0);
			break;
		
		case TURNING:
			if (target.heading == rover.heading) {
				target.heading = rover.heading;
				rover.speed = 0;
				rover.state = IDLE;
			}
			
			if (target.heading > rover.heading) {
				motorMsg.m1 = setMotorSpeed(M1, 0, rover.speed);
				motorMsg.m2 = setMotorSpeed(M2, 0, rover.speed);
				motorMsg.m3 = setMotorSpeed(M3, 0, rover.speed);
			}
			else if (target.heading < rover.heading) {
				motorMsg.m1 = setMotorSpeed(M1, 1, rover.speed);
				motorMsg.m2 = setMotorSpeed(M2, 1, rover.speed);
				motorMsg.m3 = setMotorSpeed(M3, 1, rover.speed);
			}
			break;
		
		case WAITING:
			if (received.topic == ARM && received.armState == 0) rover.state = IDLE;
			break;
		
		default:
			rover.state = IDLE;
			break;
	}
	
	mqttMsg.topic = ENCODE_TOPIC_STATE;
	mqttMsg.rover = rover;
	
	return pdTRUE;
}

MotorPacket_t setMotorSpeed(unsigned char address, unsigned char command, unsigned char data) {
	MotorPacket_t mp;
	
	mp.address = address;
	mp.command = command;
	mp.data = data;
	mp.checksum = (address + command + data) & 0b01111111;
	
	return mp;
}
