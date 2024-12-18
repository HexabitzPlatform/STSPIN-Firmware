/*
 * COMMAND_APIS.h
 * Description: Receiving commands and Sending message from BLDC motor driver.
 *  Created on: Oct 30, 2024
 *      Author: Muhammad Alhaddad @ Hexabitz
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 Hexabitz.
 * All rights reserved.
 *
 ******************************************************************************
 */
#ifndef INC_COMMAND_APIS_H_
#define INC_COMMAND_APIS_H_


/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Module_Status Type Definition */

typedef enum {
	IDEL,
	STOP_MOTOR,
	START_MOTOR,
	SET_SPEED,
	SET_TORQUE,
	SET_POSITION,
	GET_SPEED,
	GET_POSITION,
	GET_CONTROL_MODE,
	GET_MOVE_DURATON
} comState_t;





/* Exported macros -----------------------------------------------------------*/
#define DEFAULT_VELOCITY  100u  /* default velocity is 100 rpm */


/* Exported functions  ---------------------------------------------*/
uint8_t ConvertFloatTwoBytes(float arg, uint8_t *fourBytes);
uint8_t ProcessReceivedUint16(uint8_t *ReceivedBytes, uint16_t *arg1);
uint8_t ProcessReceivedInt16(uint8_t *ReceivedBytes, int16_t *arg1);
uint8_t ProcessReceivedFloat(uint8_t *ReceivedBytes, float *arg1);

uint8_t SendMessage(uint8_t command, uint8_t FirstBytesSentMSG[4], uint8_t SecondBytesSentMSG[4]);


#endif /* INC_COMMAND_APIS_H_ */
