/*
 * AT.h
 *
 *  Created on: 2 oct. 2025
 *      Author: jesus
 */

#ifndef SIM7080G_H_
#define SIM7080G_H_

#include "Mcal.h"

#define AT_TRANSMIT_TIMEOUT 		(0xFFFFU)

/**
* @brief        Tests the functionality of the AT commands with the modem in serial communication.
*
* @api
* @param[in]     Indicates the time in milliseconds to wait before a timeout occurs.
* @return        Boolean value
* @retval        True: AT command working as expected obtained.
* @retval        False: AT commands communication failed.
* implements     Test AT
*/
bool TestAT(uint32_t timeout_ms);

/* Function to initialize the time-out timer */
void InitTimeoutTimer(uint32_t milliseconds);

/* Function to retrieve the current value of the timeout timer */
uint32_t GetCurrentTime(void);

/* Function to de-initialize the time-out timer */
void DeinitTimeoutTimer(void);

/* Function to wait in polling for a specific amount of milliseconds */
void delay_at(uint32_t milliseconds);




uint8_t verifyResponse(char * pExpectedAnswers, uint8_t TotalExpAns);
uint8_t send_at(char *s_buf1,char*s_buf2,uint8_t com1length,uint8_t com2length,uint8_t command_num,uint16_t delay_ms);
uint8_t sendAT(char* s_buf1, uint8_t com1length, uint16_t delay_ms);
uint8_t waitResponse(
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


#endif /* SIM7080G_H_ */
