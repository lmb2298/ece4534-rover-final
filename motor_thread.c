/*
 * motor_thread.c
 *
 *  Created on: Mar 17, 2021
 *      Author: Logan Bradbury
 */

#include <string.h>

#include "ti_drivers_config.h"
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include <FreeRTOS.h>
#include <message_buffer.h>

#include <encode_queue.h>
#include <motor_thread.h>
#include <motor_queue.h>
#include <debug.h>
#include <helpers.h>

static MotorMessage_t received;
static EncodeMessage_t mqttMsg;
static char txBuffer[] = "";
UART_Handle uart;

void driveMotors();
//void uartSendMessage(UART_Handle uart);

void *motorThread(void *arg0)
{
	UART_Params uartParams;

	// Edit parameters and open UART
	UART_Params_init(&uartParams);
	uartParams.writeDataMode = UART_DATA_TEXT;
	uartParams.readDataMode = UART_DATA_TEXT;
	uartParams.readReturnMode = UART_RETURN_NEWLINE;
	uartParams.baudRate = 9600;
	
	uart = UART_open(CONFIG_UART_0, &uartParams);
	
	// Check for open failure
	if (uart == NULL) {
		// UART_open() failed
		dbgStopAll(ERROR_EVENT_SENSOR_SEND);
	}
	
	sprintf(txBuffer, "%c", 170);
	UART_write(uart, &(txBuffer[0]), 1);
	//uartSendMessage(uart);
	
	while (1) {
		if (motorQueueReceive(&received) != pdTRUE) {
            dbgStopAll(ERROR_EVENT_SENSOR_RECEIVE);
        }
		
		driveMotors();
		
		//mqttMsg.topic = ENCODE_TOPIC_MOTOR;
		//mqttMsg.motorMsg = received;
		//encodeQueueSend(mqttMsg);
		
	}
}

void driveMotors() {
	//uartSendMessage(uart);
	sprintf(txBuffer, "%c%c%c%c%c%c%c%c%c%c%c%c", 
			received.m1.address, received.m1.command, received.m1.data, received.m1.checksum,
			received.m2.address, received.m2.command, received.m2.data, received.m2.checksum,
			received.m3.address, received.m3.command, received.m3.data, received.m3.checksum );
	
	if (strlen(txBuffer) > 0) {
		UART_write(uart, &(txBuffer[0]), 1);
		UART_write(uart, &(txBuffer[1]), 1);
		UART_write(uart, &(txBuffer[2]), 1);
		UART_write(uart, &(txBuffer[3]), 1);
		UART_write(uart, &(txBuffer[4]), 1);
		UART_write(uart, &(txBuffer[5]), 1);
		UART_write(uart, &(txBuffer[6]), 1);
		UART_write(uart, &(txBuffer[7]), 1);
		UART_write(uart, &(txBuffer[8]), 1);
		UART_write(uart, &(txBuffer[9]), 1);
		UART_write(uart, &(txBuffer[10]), 1);
		UART_write(uart, &(txBuffer[11]), 1);
		//dbgEvent(UART_MESSAGE_SENT);
	}
}

/*
void uartSendMessage(UART_Handle uart) {
	//dbgEvent(UART_MESSAGE_SENDING);
	if (strlen(txBuffer) > 0) {
		UART_write(uart, txBuffer, strlen(txBuffer));
		//dbgEvent(UART_MESSAGE_SENT);
	}
	else {
		//dbgEvent(UART_MESSAGE_NOT_SENT);
	}
}
*/
