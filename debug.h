#ifndef DEBUG_H_
#define DEBUG_H_

// The below identify locations where stop_everything or dbgEvent can be called
// from.
#define ERROR_EVENT_MAIN_INIT              0x01
#define ERROR_EVENT_SENSOR_UPDATE          0x02
#define ERROR_EVENT_SENSOR_RECEIVE         0x03
#define ERROR_EVENT_SENSOR_QUEUE_INIT      0x04
#define ERROR_EVENT_ADC_READ               0x05
#define ERROR_EVENT_ADC_OPEN               0x06
#define ERROR_EVENT_SENSOR_SEND            0x07
#define ERROR_EVENT_MAIN_THREAD_CONF       0x08
#define ERROR_EVENT_DBG_CODE_RANGE         0x09
#define ERROR_EVENT_COMPUTE_QUEUE_INIT     0x0A
#define ERROR_EVENT_COMPUTE_RECEIVE        0x0B
#define ERROR_EVENT_TIMER500_OPEN          0x0C
#define ERROR_EVENT_TIMER500_START         0x0D
#define ERROR_EVENT_TIMER70_OPEN           0x0E
#define ERROR_EVENT_TIMER70_START          0x0F
#define ERROR_EVENT_MQTT_SLNET_CONF        0x10
#define ERROR_EVENT_MQTT_LED_TIMER_OPEN    0x11
#define ERROR_EVENT_MQTT_WIFI_INIT         0x12
#define ERROR_EVENT_MQTT_IF_INIT           0x13
#define ERROR_EVENT_MQTT_IF_CONNECT        0x14
#define ERROR_EVENT_MQTT_TIMER_OPEN        0x15
#define ERROR_EVENT_MQTT_TIMER_START       0x16
#define ERROR_EVENT_MQTT_SUBSCRIBE         0x17
#define ERROR_EVENT_MQTT_RECEIVE           0x18
#define ERROR_EVENT_DECODE_RECEIVE         0x19
#define ERROR_EVENT_DECODE_SEND            0x1A
#define ERROR_EVENT_DECODE_QUEUE_INIT      0x1B
#define ERROR_EVENT_ENCODE_RECEIVE         0x1C
#define ERROR_EVENT_ENCODE_SEND            0x1D
#define ERROR_EVENT_ENCODE_QUEUE_INIT      0x1E
#define ERROR_EVENT_MESSAGE_CALLBACK       0x1F
#define ERROR_EVENT_MQTT_PUBLISH           0x26

#define COMPUTE_ENTER                0x20
#define COMPUTE_ENTER_LOOP           0x21
#define COMPUTE_QUEUE_BEFORE_SEND    0x22
#define COMPUTE_QUEUE_AFTER_SEND     0x23
#define COMPUTE_QUEUE_BEFORE_RECEIVE 0x24
#define COMPUTE_QUEUE_AFTER_RECEIVE  0x25

#define SENSOR_ENTER                0x30
#define SENSOR_ENTER_LOOP           0x31
#define SENSOR_QUEUE_BEFORE_SEND    0x33
#define SENSOR_QUEUE_AFTER_SEND     0x34
#define SENSOR_QUEUE_BEFORE_RECEIVE 0x35
#define SENSOR_QUEUE_AFTER_RECEIVE  0x36

#define MQTT_ENTER                0x40
#define MQTT_ENTER_LOOP           0x41
#define MQTT_QUEUE_BEFORE_SEND    0x42
#define MQTT_QUEUE_AFTER_SEND     0x43
#define MQTT_QUEUE_BEFORE_RECEIVE 0x44
#define MQTT_QUEUE_AFTER_RECEIVE  0x45
#define MQTT_BLINK_TIMER_OPEN     0x46
#define MQTT_WIFI_INIT            0x47
#define MQTT_IF_INIT              0x48
#define MQTT_IF_CONNECT           0x49
#define MQTT_IF_SUBSCRIBE         0x4A

#define DECODE_ENTER                0x50
#define DECODE_ENTER_LOOP           0x51
#define DECODE_QUEUE_BEFORE_SEND    0x52
#define DECODE_QUEUE_BEFORE_RECEIVE 0x53

#define ENCODE_ENTER                0x54
#define ENCODE_ENTER_LOOP           0x55
#define ENCODE_QUEUE_BEFORE_SEND    0x56
#define ENCODE_QUEUE_BEFORE_RECEIVE 0x57

#define DLOC_TIMER500_INIT_BEGIN    0x60
#define DLOC_TIMER500_INIT_END      0x61
#define DLOC_TIMER500_CALLBACK_SENDTOQUEUE_BEFORE 0x62
#define DLOC_TIMER500_CALLBACK_SENDTOQUEUE_AFTER  0x63

#define DLOC_TIMER70_INIT_BEGIN     0x70
#define DLOC_TIMER70_INIT_END       0x71
#define DLOC_TIMER70_DIST_BEGIN     0x72
#define DLOC_TIMER70_CALLBACK_BEGIN 0x73
#define DLOC_TIMER70_CALLBACK_SENDTOQUEUE_BEFORE 0x74
#define DLOC_TIMERTWO_CALLBACK_SENDTOQUEUE_AFTER 0x75
#define DLOC_TIMER70_CALLBACK_END   0x76

#define DEBUG_POINT                 0x7E
// an easily triggerable value on the gpio. all bits set to 1
#define MAIN_AFTER_GPIO             0x7F

#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>

void Debug_init();
void dbgEvent(unsigned int outputLoc);
void dbgStopAll(unsigned int stopVal);

#endif /* DEBUG_H_ */
