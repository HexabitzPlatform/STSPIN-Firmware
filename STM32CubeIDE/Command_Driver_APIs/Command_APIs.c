/*
 * Command_APIS.C
 * Description: receiving commands and sending message about BLDC motor parameters.
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

#include "Command_APIs.h"
#include "stm32f0xx_ll_usart.h"

uint8_t SetPosition(float Position, float Duration);
uint8_t GetPosition(float* Position);
uint8_t SetSpeed(uint16_t Time, int16_t Speed);
uint8_t SetTorque(uint16_t Time, int16_t Torque);
uint8_t GetMoveDuration(float* Duration);
uint8_t GetControlMode(uint8_t* Mode);
uint8_t Test_Function();

uint8_t SendingMessage[11];

/**************************************************************************/
/* Local Functions  *******************************************************/
/**************************************************************************/

/**
 * @brief get value of four bytes n which float variable is stored
 * @param1: *arg :  pointer of float value.
 * @param2: *fourBytes :  pointer of four uint8_t bytes.
 */
uint8_t ConvertFloatTwoBytes(float arg, uint8_t *fourBytes) {
	uint8_t *ptr, i;
	ptr = (uint8_t*) &(arg);
	/* casting float variable into four bytes */
	for (i = 0; i < 4; i++) {
		fourBytes[i] = (*(ptr + i));
	}
	return 0;
}

/**********************************************************************/

/**
 * @brief get value of two bytes in which uint16_t variable is stored
 * @param1: *arg :  pointer of uint16_t value.
 * @param2: *twoBytes :  pointer of four uint8_t bytes.
 */
uint8_t ConvertUint16TwoBytes(uint16_t arg, uint8_t *twoBytes) {
	uint8_t *ptr1, i;
	ptr1 = (uint8_t*) &(arg);
	/* casting uint16_t variable into two bytes */
	for (i = 0; i < 2; i++) {
		twoBytes[i] = (*(ptr1 + i));
	}

	return 0;
}

/**********************************************************************/

/**
 * @brief get value of two bytes in which int16_t variable is stored
 * @param1: *arg :  pointer of int16_t value.
 * @param2: *twoBytes :  pointer of four uint8_t bytes.
 */
uint8_t ConvertInt16TwoBytes(int16_t arg, uint8_t *twoBytes) {
	uint8_t *ptr1, i;
	ptr1 = (uint8_t*) &(arg);
	/* casting int16_t variable into two bytes */
	for (i = 0; i < 2; i++) {
		twoBytes[i] = (*(ptr1 + i));
	}

	return 0;
}

/**********************************************************************/

/**
 * @brief processing received message from stspin
 * @param1: *ReceivedBytes : pointer of Received Bytes in the received message .
 * @param2: *arg1 :  pointer of argument in received result .
 */
uint8_t ProcessReceivedUint16(uint8_t *ReceivedBytes, uint16_t *arg1) {

	uint16_t temp = 0;
	/* shifting four bytes and casting them into float */
	temp = ((ReceivedBytes[0] << 0) | (ReceivedBytes[1] << 8));
	*arg1 = *((uint16_t*) &temp);

	return 0;
}

/**********************************************************************/

/**
 * @brief processing received message from stspin
 * @param1: *ReceivedBytes : pointer of Received Bytes in the received message .
 * @param2: *arg1 :  pointer of argument in received result .
 */
uint8_t ProcessReceivedInt16(uint8_t *ReceivedBytes, int16_t *arg1) {

	uint16_t temp = 0;
	/* shifting four bytes and casting them into float */
	temp = ((ReceivedBytes[0] << 0) | (ReceivedBytes[1] << 8));
	*arg1 = *((int16_t*) &temp);

	return 0;
}

/**********************************************************************/

/**
 * @brief processing received message from stspin
 * @param1: *ReceivedBytes : pointer of Received Bytes in the received message .
 * @param2: *arg1 :  pointer of argument in received result .
 */
uint8_t ProcessReceivedFloat(uint8_t *ReceivedBytes, float *arg1) {

	uint32_t temp = 0;
	/* shifting four bytes and casting them into float */
	temp = ((ReceivedBytes[0] << 0) | (ReceivedBytes[1] << 8)
			| (ReceivedBytes[2] << 16) | ReceivedBytes[3] << 24);
	*arg1 = *((float*) &temp);

	return 0;

}

/**********************************************************************/

/**
 * @brief preparing message command sent to stspin
 * so it will consists of 11 bytes
 * "H" + "Z" + command (one byte typedef) +
 * + four bytes first argument + four bytes second argument
 * @param1: command :  command need to be sent .
 */
uint8_t SendMessage(uint8_t command, uint8_t FirstBytesSentMSG[4], uint8_t SecondBytesSentMSG[4]) {

	SendingMessage[0] = 'H';
	SendingMessage[1] = 'Z';

	SendingMessage[2] = command;

	SendingMessage[3] = FirstBytesSentMSG[0];  // LS
	SendingMessage[4] = FirstBytesSentMSG[1];
	SendingMessage[5] = FirstBytesSentMSG[2];
	SendingMessage[6] = FirstBytesSentMSG[3];

	SendingMessage[7] = SecondBytesSentMSG[0];
	SendingMessage[8] = SecondBytesSentMSG[1];
	SendingMessage[9] = SecondBytesSentMSG[2];
	SendingMessage[10] = SecondBytesSentMSG[3];

	int i = 0;
	while (i < 11) {
		LL_USART_TransmitData8(USART1, SendingMessage[i]);

		while (!LL_USART_IsActiveFlag_TXE(USART1)) {

		}
		i++;
	}

	return 0;
}

/* Exported functions  ---------------------------------------------*/


