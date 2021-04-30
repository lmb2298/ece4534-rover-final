/*
 * mqtt_thread.h
 *
 *  Created on: Feb 22, 2021
 *      Author: Logan Bradbury
 */

#ifndef MQTT_THREAD_H_
#define MQTT_THREAD_H_

#include <stdint.h>
#include <stddef.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>

/* Driver configuration */
#include "ti_drivers_config.h"

/* Project Header files */
//#include <mqtt_thread_queue.h>


extern void *mqtt_thread(void *arg0);


#endif /* MQTT_THREAD_H_ */
