/*
 * timer500.c
 *
 *  Created on: Mar 17, 2021
 *      Author: Logan Bradbury
 */

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>

#include <FreeRTOS.h>
#include <task.h>
#include "timer500.h"
#include <debug.h>
#include <pid_queue.h>

#define MASTER_MSG      96

unsigned char masterRxBuffer[5];
unsigned char masterTxBuffer[] = "\x60\x00\x00\x00\x00";

SPI_Handle    masterSpi;
PID_Message_t pidMsg;

/* Callback used for toggling the LED. */
void timer500Callback(Timer_Handle myHandle);

void initSPI();
void spiExchange();

void timer500Init() {
    dbgEvent(DLOC_TIMER500_INIT_BEGIN);
	Timer_Handle timer500;
    Timer_Params params500;

    Timer_Params_init(&params500);
    params500.period = 500000;
    params500.periodUnits = Timer_PERIOD_US;
    params500.timerMode = Timer_CONTINUOUS_CALLBACK;
    params500.timerCallback = (Timer_CallBackFxn)timer500Callback;

    timer500 = Timer_open(CONFIG_TIMER_0, &params500);

    if (timer500 == NULL) {
        /* Failed to initialized timer */
        dbgStopAll(ERROR_EVENT_TIMER500_OPEN);
    }

    if (Timer_start(timer500) == Timer_STATUS_ERROR) {
        /* Failed to start timer */
        dbgStopAll(ERROR_EVENT_TIMER500_START);
    }
	
	//initSPI();
    dbgEvent(DLOC_TIMER500_INIT_END);
}

void timer500Callback(Timer_Handle myHandle) {
    dbgEvent(DLOC_TIMER500_CALLBACK_SENDTOQUEUE_BEFORE);
	
	/*pidMsg.encoder1 = 5;
	pidMsg.encoder2 = 10;
	pidMsg.encoder3 = 15;*/
	
	// Read Encoder Values
	//spiExchange();
	
	pidQueueSendISR(pidMsg);
    dbgEvent(DLOC_TIMER500_CALLBACK_SENDTOQUEUE_AFTER);
}


void initSPI() {
    SPI_Params spiParams;
	
	SPI_Params_init(&spiParams);
    spiParams.frameFormat = SPI_POL0_PHA1;
    spiParams.bitRate = 10000000;
    masterSpi = SPI_open(CONFIG_SPI_0, &spiParams);
	if (masterSpi == NULL) {
        //Display_printf(display, 0, 0, "Error initializing master SPI\n");
        //while (1);
    }
	
	GPIO_setConfig(SS0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH);
    GPIO_setConfig(SS1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH);
    GPIO_setConfig(SS2, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH);
}

void spiExchange() {
	SPI_Transaction transaction;
	bool transferOK;
	
	GPIO_toggle(CONFIG_GPIO_LED_2);
	
	GPIO_write(SS0, 0);
	transaction.count = 5;
    transaction.txBuf = (void *) masterTxBuffer;
    transaction.rxBuf = (void *) masterRxBuffer;
	
	/* Perform SPI transfer */
	transferOK = SPI_transfer(masterSpi, &transaction);
	if (transferOK) {
		//Display_printf(display, 0, 0, "Master received: %s", masterRxBuffer);
		pidMsg.encoder1 = atoi(masterRxBuffer);
		pidMsg.e1 = masterRxBuffer[0];
	}
	else {
		pidMsg.encoder1 = 55;
	}
	GPIO_write(SS0, 1);
	
	
	GPIO_write(SS1, 0);
	transaction.count = 5;
    transaction.txBuf = (void *) masterTxBuffer;
    transaction.rxBuf = (void *) masterRxBuffer;
	
	/* Perform SPI transfer */
	transferOK = SPI_transfer(masterSpi, &transaction);
	if (transferOK) {
		//Display_printf(display, 0, 0, "Master received: %s", masterRxBuffer);
		pidMsg.encoder2 = atoi(masterRxBuffer);
		pidMsg.e2 = masterRxBuffer[0];
	}
	else {
		pidMsg.encoder2 = 55;
	}
	GPIO_write(SS1, 1);
	
	
	GPIO_write(SS2, 0);
	transaction.count = 5;
    transaction.txBuf = (void *) masterTxBuffer;
    transaction.rxBuf = (void *) masterRxBuffer;
	
	/* Perform SPI transfer */
	transferOK = SPI_transfer(masterSpi, &transaction);
	if (transferOK) {
		//Display_printf(display, 0, 0, "Master received: %s", masterRxBuffer);
		pidMsg.encoder3 = atoi(masterRxBuffer);
		pidMsg.e3 = masterRxBuffer[0];
	}
	else {
		pidMsg.encoder3 = 55;
	}
	GPIO_write(SS2, 1);
}
