#include <FreeRTOS.h>
#include <task.h>
#include "timer500.h"
#include <debug.h>
#include <pid_queue.h>

/* Callback used for toggling the LED. */
void timer500Callback(Timer_Handle myHandle);

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
    dbgEvent(DLOC_TIMER500_INIT_END);
}

void timer500Callback(Timer_Handle myHandle) {
    dbgEvent(DLOC_TIMER500_CALLBACK_SENDTOQUEUE_BEFORE);
	PID_Message_t pidMsg;
	
	pidMsg.encoder1 = 5;
	pidMsg.encoder2 = 10;
	pidMsg.encoder3 = 15;
	
	// Read Encoder Values
	
	pidQueueSendISR(pidMsg);
    dbgEvent(DLOC_TIMER500_CALLBACK_SENDTOQUEUE_AFTER);
}
