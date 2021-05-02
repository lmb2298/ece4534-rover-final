#include <FreeRTOS.h>
#include <task.h>
#include <ti_drivers_config.h>
#include <debug.h>
#include <ti/drivers/GPIO.h>

void gpioWrite(unsigned int outputLoc);

void Debug_init()
{
	/*
	// initialize GPIO pins for debugging
	GPIO_setConfig(CONFIG_GPIO_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_2, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_3, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_4, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_5, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_6, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_7, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
	
	// setup onboard LED
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
	GPIO_setConfig(CONFIG_GPIO_LED_1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
	GPIO_setConfig(CONFIG_GPIO_LED_2, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF); // it defaults to OFF
	GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF); // it defaults to OFF
	GPIO_write(CONFIG_GPIO_LED_2, CONFIG_GPIO_LED_OFF); // it defaults to OFF
	*/
}

void dbgEvent(unsigned int outputLoc)
{
    /*
	if (outputLoc > 127) {
        // We don't use the outputLoc here b/c 1) it won't fit onto the GPIO lines and 2)
        // dbgStopAll calls this function, which would lead to infinite recursion. Also,
        // this way we're notified that we have an out-of-range code.
        dbgStopAll(ERROR_EVENT_DBG_CODE_RANGE);
    }

    // notify that we're modifying the output
    GPIO_write(CONFIG_GPIO_7, 1);

    // set the GPIO pins
    GPIO_write(CONFIG_GPIO_0, (outputLoc >> 0) & 1);
    GPIO_write(CONFIG_GPIO_1, (outputLoc >> 1) & 1);
    GPIO_write(CONFIG_GPIO_2, (outputLoc >> 2) & 1);
    GPIO_write(CONFIG_GPIO_3, (outputLoc >> 3) & 1);
    GPIO_write(CONFIG_GPIO_4, (outputLoc >> 4) & 1);
    GPIO_write(CONFIG_GPIO_5, (outputLoc >> 5) & 1);
    GPIO_write(CONFIG_GPIO_6, (outputLoc >> 6) & 1);

    // notify that we've finished the modifications
    GPIO_write(CONFIG_GPIO_7, 0);
	*/
}

void dbgStopAll(unsigned int stopVal)
{
    dbgEvent(stopVal);

    // stop interrupts and running threads
    taskDISABLE_INTERRUPTS();
    vTaskSuspendAll();

    // never return and blink the LED
    while (1) {
        GPIO_toggle(CONFIG_GPIO_LED_0);
        int i;
        for (i = 0; i < 100000; ++i) {
            // spin here for some amount of time to create the blink effect
        }
    }
}
