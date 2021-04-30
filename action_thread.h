/*
 * action_thread.h
 *
 *  Created on: Mar 17, 2021
 *      Author: Logan Bradbury
 */

#ifndef ACTION_THREAD_H_
#define ACTION_THREAD_H_

typedef enum RoverState {
	IDLE,
	MOVING,
	TURNING,
	WAITING
} RoverState_t; 

typedef struct RoverVariables {
	RoverState_t  state;
	int           heading;
	unsigned char speed;
	unsigned char direction;
} RoverVariables_t;

extern void *actionThread(void *arg0);

#endif /* ACTION_THREAD_H_ */
