/*
 * AT.h
 *
 *  Created on: 2 oct. 2025
 *      Author: jesus
 */

#ifndef AT_H_
#define AT_H_

#include "Mcal.h"

#define AT_TRANSMIT_TIMEOUT 		(0xFFFFU)

typedef enum{
	AT_RESP_OK,
	AT_SENT,
	AT_ERROR
} at_status;


/* Function to initialize the time-out timer */
void InitTimeoutTimer(uint32_t milliseconds);

/* Function to retrieve the current value of the timeout timer */
uint32_t GetCurrentTime(void);

/* Function to de-initialize the time-out timer */
void DeinitTimeoutTimer(void);

/* Function to wait in polling for a specific amount of milliseconds */
void delay_at(uint32_t milliseconds);


uint8_t verifyResponse(char * pExpectedAnswers, uint8_t TotalExpAns);
at_status send_at(char *s_buf1,char*s_buf2,uint8_t com1length,uint8_t com2length,uint8_t command_num,uint16_t delay_ms);
at_status sendAT(char* s_buf1, uint8_t com1length, uint16_t delay_ms);
uint8_t waitResponseImpl(
		uint32_t timeout_ms,
		uint8_t *data,
		uint8_t *r1,
		uint8_t *r2,
		uint8_t *r3,
		uint8_t *r4,
		uint8_t *r5,
		uint8_t *r6,
		uint8_t *r7);
int GetIntResponse(void);
int GetIntBefore(char lastChar);


#endif /* AT_H_ */
