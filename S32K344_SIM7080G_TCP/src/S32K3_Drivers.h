/*
 * device_drivers.h
 *
 *  Created on: Oct 16, 2025
 *      Author: nxf63523
 */

#ifndef S32K3_DRIVERS_H_
#define S32K3_DRIVERS_H_

#include "Lpuart_Uart_Ip.h"
#include "Lpuart_Uart_Ip_Irq.h"
#include "Clock_Ip.h"
#include "IntCtrl_Ip.h"
#include "Siul2_Port_Ip.h"
#include "string.h"
#include "Stm_Ip.h"
#include "Siul2_Dio_Ip.h"

/**********************************************************/
/**********			     SIUL					***********/
/**********************************************************/

#define SIUL2_INSTANCE				(0U)

/**********************************************************/
/**********			     UART					***********/
/**********************************************************/
/* Length of the message to be received from the console */
#define MAX_LEN  					(64U)

/* UART to be used with the AT SIM7080G */
#define AT_UART_INSTANCE  			(6U)

/* UART to send the log to the main computer */
#define DEBUG_UART_INSTANCE  		(6U)

#define MAX_TIMEOUT					(0xFFFFFFFFU)

/**********************************************************/
/**********			      STM					***********/
/**********************************************************/
/* STM instance used - 12 */
#define STM_INST_0 					(0U)

/* STM Channel used - 0 */
#define CH_0 						(0U)

#define STM_START_CNT_VALUE			(0U)

/* System Clock Frequency used */
#define STM_CLOCK_FREQ 				(48000000U)

/* STM0 Clock Prescaler */
#define STM_CLK_PRESCALER			(48U)

#define MODEM_MUX_COUNT 			(12U)




/*****************************************************************************************
 ************************* GLOBAL FUNCTION DEFINITIONS ***********************************
 *****************************************************************************************/

/**
* @brief       	Initialize S32K3 Low Level Drivers for serial communication and timers usage.
*
* @api
* @return        N/A
* implements     S32K3 Initialization
*/
void init_device_drivers(void);

/**
* @brief       	Generate a blocking delay to wait for a given amount of milliseconds.
*
* @api
* @param[in]	milliseconds to wait in the delay
* @return       N/A
* implements    Blocking Delay
*/
void DelayImpl(uint32_t milliseconds);

/**
* @brief       	Initialize the timer that will be used for timeout operations.
*
* @api
* @param[in]	 milliseconds
* @return        N/A
* implements     Init Timeout Timer
*/
void InitTimeoutTimerImpl(uint32_t milliseconds);

/**
* @brief       	Gets the ellapsed time on the timer used for the timeout.
*
* @api
* @param[in]	 N/A
* @return        Returns the ellapsed time in milliseconds of the running timer
* implements     Get Current Time
*/
uint32_t GetCurrentTimeImpl(void);


/**
* @brief       	DeInitialize the timer used for the timeout.
*
* @api
* @param[in]	 N/A
* @return        N/A
* implements     DeInitialize Timeout timer
*/
void DeinitTimeoutTimerImpl(void);

/**
* @brief       	Implementation of the Wait Response logic implemented using S32K3 serial reception functions.
*
* @api
* @param[in]	Timeout value in milliseconds to wait for the full response to be received
* @param[in]	Pointer to the Data buffer where the received string will be stored and compared from. If NULL_PTR is given, then internal RX buffer is used.
* @param[in]	Pointer to the first expected response. If NULL_PTR is given, then it will be default to expected GSM_OK.
* @param[in]	Pointer to the second expected response. If NULL_PTR is given, then it will be default to expected GSM_ERROR.
* @param[in]	Pointer to the third expected response. No default value given.
* @param[in]	Pointer to the fourth expected response. No default value given.
* @param[in] 	Pointer to the fifth expected response. No default value given.
* @param[in]	Pointer to the sixth expected response. No default value given.
* @param[in]	Pointer to the seventh expected response. No default value given.
* @return       The number of the expected response that matches the actually received response. A value of 0 indicates that a timeout occurred and there was no match found.
* implements    WaitResponseImpl
*/
uint8_t waitResponseImpl
(
		uint32_t timeout_ms,
		char *pData,
		const char *pR1,
		const char *pR2,
		const char *pR3,
		const char *pR4,
		const char *pR5,
		const char *pR6,
		const char *pR7
);

/**
* @brief       	S32K3 implementation of the function to send specific AT command over serial interface.
*
* @api
* @param[in]	Pointer to the buffer containing the command to be sent
* @param[in]	Length in bytes of the command to send
* @return       Status value to indicate if the command was successfully transmitted (0 - OK | 1 - ERROR)
* implements    Send AT Impl
*/
uint8_t sendATImpl
(
	const char* pData,
	uint8_t dataLength
);

/**
* @brief       Implementation of StreamSkipUntil, it allows to detect specific characters in the reception and skip over until that character is received.
*
* @api
* @param[in]	Expected character to find in the serial reception.
* @param[in]	Delay to wait once the command was transferred.
* @return       Boolean value to indicate if the character was found or a timeout occurred.
* implements    StreamSkipUntilImpl
*/
bool streamSkipUntilImpl
(
	const char expectedChar,
	uint32_t timeout_ms
);

/**
* @brief       Obtain the integer value from reception until reaching the indicated last character value.
*
* @api
* @param[in]	Character that indicates the ends of the integer.
* @return       Integer value that is obtained before the indicated character.
* implements    GetIntBeforeImpl
*/
int GetIntBeforeImpl
(
	const char lastChar
);

/**
* @brief       Obtain the integer value from reception until no more integers are received.
*
* @api
* @param[in]	N/A
* @return       Integer value that is obtained from the serial reception.
* 				If 0xFFFF, then an error occurred.
* implements    GetIntBeforeImpl
*/
int GetIntResponseImpl(void);


#endif /* S32K3_DRIVERS_H_ */
