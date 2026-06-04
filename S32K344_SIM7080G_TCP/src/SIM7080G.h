/*
 * AT.h
 *
 *  Created on: 2 oct. 2025
 *      Author: jesus
 */

#ifndef SIM7080G_H_
#define SIM7080G_H_

#include "Mcal.h"


typedef union {
	uint8_t bytes[4];  // IPv4 address
	uint32_t dword;
} IP_address;

typedef enum {
  SIM_ERROR            = 0,
  SIM_READY            = 1,
  SIM_LOCKED           = 2,
  SIM_ANTITHEFT_LOCKED = 3,
} SimStatus;

typedef enum {
  REG_NO_RESULT    = -1,
  REG_UNREGISTERED = 0,
  REG_SEARCHING    = 2,
  REG_DENIED       = 3,
  REG_OK_HOME      = 1,
  REG_OK_ROAMING   = 5,
  REG_UNKNOWN      = 4,
}SIM70xxRegStatus;

/**
* @brief       	Generate a blocking delay to wait for a given amount of milliseconds.
*
* @api
* @param[in]	milliseconds to wait in the delay
* @return       N/A
* implements    Blocking Delay
*/
void Delay(uint32_t milliseconds);

/**
* @brief       	Calls for the MCU implementation to initialize the assigned timeout timer.
*
* @api
* @param[in]	milliseconds to indicate the timeout
* @return       N/A
* implements    InitTimeoutTimer
*/
void InitTimeoutTimer(uint32_t milliseconds);

/**
* @brief       	Reads the current value of the timeout timer.
*
* @api
* @param[in]	N/A
* @return       The current ellapsed time of the timeout timer
* implements    GetCurrentTime
*/
uint32_t GetCurrentTime(void);

/**
* @brief       	Calls for the De-Initialization of the timeout timer defined by the MCU.
*
* @api
* @param[in]	N/A
* @return       N/A
* implements    DeinitTimeoutTimer
*/
void DeinitTimeoutTimer(void);

/**
* @brief       Calls for the implementation of streamSkipUntil, it allows to detect specific characters in the reception and skip over until that character is received.
*
* @api
* @param[in]	Expected character to find in the serial reception.
* @param[in]	Delay to wait once the command was transferred.
* @return       Boolean value to indicate if the character was found or a timeout occurred.
* implements    StreamSkipUntil
*/
bool streamSkipUntil
(
	const char expectedChar,
	uint32_t timeout_ms
);

/**
* @brief       Calls for the implementation to obtain the integer value from reception until reaching the indicated last character value.
*
* @api
* @param[in]	Character that indicates the ends of the integer.
* @return       Integer value that is obtained before the indicated character.
* implements    GetIntBefore
*/
int GetIntBefore(char lastChar);

/**
* @brief       Calls for the implementation to obtain the integer value from reception until no more integers are received.
*
* @api
* @param[in]	N/A
* @return       Integer value that is obtained from the serial reception.
* 				If 0xFFFF, then an error occurred.
* implements    GetIntBefore
*/
int GetIntResponse(void);

/**
* @brief       Transforms the received parameters into an IP Address Structure.
*
* @api
* @param[in]	first byte to fill the IP address with
* @param[in]	Second byte to fill the IP address with
* @param[in]	Third byte to fill the IP address with
* @param[in]	Fourth byte to fill the IP address with
* @return       Filled structure with the corresponding values of the IP Address
* implements    IPAddress constructor
*/
IP_address IPAddress
(
	uint8_t first_octet,
	uint8_t second_octet,
	uint8_t third_octet,
	uint8_t fourth_octet
);

/**
* @brief       Verifies if two IP Addresses are the same
*
* @api
* @param[in]	First IP address to analyze
* @param[in]	Second IP address to analyze
* @return       Boolean value indicating if the two IP addresses are the same or not
* implements    compareIP
*/
bool compareIP(IP_address IP1, IP_address IP2);

/**
* @brief       Generates an IP Address structure from an array
*
* @api
* @param[in]	Pointer to the address of the first element for the IP String Array
* @return       IP Address structure filled out with the values of the string.
* implements    IpFromString
*/
IP_address IpFromString(uint8_t* IPstring);

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
uint8_t waitResponse(
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
* @brief       	Implementation of the Send AT function to send specific command over serial interface.
*
* @api
* @param[in]	Pointer to the buffer containing the command to be sent
* @param[in]	Length in bytes of the command to send
* @param[in]	Time in milliseconds to wait once the AT command was sent
* @return       Status value to indicate if the command was successfully transmitted (0 - OK | 1 - ERROR)
* implements    Send AT
*/
uint8_t sendAT
(
	const char* pData,
	uint8_t dataLength,
	uint16_t delay_ms
);

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

IP_address getLocalIP(void);

bool setNetworkMode(const char* pMode);

bool setPreferredMode(char* pMode);

int getPreferredMode(void);

bool isGprsConnected(void);

int8_t getRegistrationStatusXREG(const char* regCommand);

SIM70xxRegStatus getRegistrationStatus(void);



#endif /* SIM7080G_H_ */
