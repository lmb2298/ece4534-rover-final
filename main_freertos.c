/*
 * Copyright (c) 2016-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== main_freertos.c ========
 */
#include <stdint.h>

#ifdef __ICCARM__
#include <DLib_Threads.h>
#endif

/* POSIX Header files */
#include <pthread.h>

/* RTOS header files */
#include <FreeRTOS.h>
#include <task.h>

/* TI-RTOS Header files */
#include <ti/drivers/Board.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/Timer.h>

/* TI-DRIVERS Header files */
#include "ti_drivers_config.h"

/* Project Header files */
#include <mqtt_thread.h>
#include <mqtt_send_queue.h>
#include <decode_thread.h>
#include <decode_queue.h>
#include <encode_queue.h>
#include <encode_thread.h>
#include <pid_thread.h>
#include <pid_queue.h>
#include <action_thread.h>
#include <action_queue.h>
#include <motor_thread.h>
#include <motor_queue.h>
#include <timer500.h>
#include <debug.h>

/* Stack size in bytes */
#define DEFAULT_STACK_SIZE   1024
#define OTHER_STACK_SIZE	 2048
#define MQTT_STACK_SIZE		 4096

static int init_thread(pthread_attr_t *attrs, struct sched_param priority, size_t stack_size);

/*
 *  ======== main ========
 */
int main(void)
{
    pthread_t          mqtt_pthread,  decode_pthread,  encode_pthread,  pid_pthread,  action_pthread,  motor_pthread;
    pthread_attr_t     mqtt_attrs,    decode_attrs,    encode_attrs,    pid_attrs,    action_attrs,    motor_attrs;
    struct sched_param mqtt_priority, decode_priority, encode_priority, pid_priority, action_priority, motor_priority;
    int                retc = 0;

    /* initialize the system locks */
#ifdef __ICCARM__
    __iar_Initlocks();
#endif

    Board_init();
    GPIO_init();
    Timer_init();
    UART_init();
	SPI_init();
	Debug_init();

    dbgEvent(MAIN_AFTER_GPIO);

	// Create queues
	if (mqttSendQueueCreate() != pdTRUE) {
        dbgStopAll(ERROR_EVENT_COMPUTE_QUEUE_INIT);
    }
    if (decodeQueueCreate() != pdTRUE) {
        dbgStopAll(ERROR_EVENT_DECODE_QUEUE_INIT);
    }
    if (encodeQueueCreate() != pdTRUE) {
        dbgStopAll(ERROR_EVENT_ENCODE_QUEUE_INIT);
    }
	if (pidQueueCreate() != pdTRUE) {
        dbgStopAll(ERROR_EVENT_ENCODE_QUEUE_INIT);
    }
	if (actionQueueCreate() != pdTRUE) {
        dbgStopAll(ERROR_EVENT_ENCODE_QUEUE_INIT);
    }
	if (motorQueueCreate() != pdTRUE) {
        dbgStopAll(ERROR_EVENT_ENCODE_QUEUE_INIT);
    }
	
	timer500Init();

    // Assign thread priority
	mqtt_priority.sched_priority   = 1;
    decode_priority.sched_priority = 1;
    encode_priority.sched_priority = 1;
	pid_priority.sched_priority    = 1;
	action_priority.sched_priority = 1;
	motor_priority.sched_priority  = 1;

    // configure the pthread structs
    retc =  init_thread(&mqtt_attrs, mqtt_priority, MQTT_STACK_SIZE);
    retc |= init_thread(&decode_attrs, decode_priority, DEFAULT_STACK_SIZE);
    retc |= init_thread(&encode_attrs, encode_priority, DEFAULT_STACK_SIZE);
	retc |= init_thread(&pid_attrs, pid_priority, DEFAULT_STACK_SIZE);
	retc |= init_thread(&action_attrs, action_priority, DEFAULT_STACK_SIZE);
	retc |= init_thread(&motor_attrs, motor_priority, DEFAULT_STACK_SIZE);
    if (retc != 0) {
        /* failed to set attributes */
        dbgStopAll(ERROR_EVENT_MAIN_THREAD_CONF);
    }

    retc = pthread_create(&mqtt_pthread, &mqtt_attrs, mqtt_thread, NULL);
    retc |= pthread_create(&decode_pthread, &decode_attrs, decodeThread, NULL);
    retc |= pthread_create(&encode_pthread, &encode_attrs, encodeThread, NULL);
	retc |= pthread_create(&pid_pthread, &pid_attrs, pidThread, NULL);
	retc |= pthread_create(&action_pthread, &action_attrs, actionThread, NULL);
	retc |= pthread_create(&motor_pthread, &motor_attrs, motorThread, NULL);
    if (retc != 0) {
        /* pthread_create() failed */
        dbgStopAll(ERROR_EVENT_MAIN_INIT);
    }

    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();

    return 0;
}

int init_thread(pthread_attr_t *attrs, struct sched_param priority, size_t stack_size)
{
    int retc;

    pthread_attr_init(attrs);
    retc = pthread_attr_setschedparam(attrs, &priority);
    retc |= pthread_attr_setdetachstate(attrs, PTHREAD_CREATE_DETACHED);
    retc |= pthread_attr_setstacksize(attrs, stack_size);
    return retc;
}
