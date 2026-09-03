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
  SIM_LOCKED           = 2
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
* @brief       	Proceeds with hardware initialization for the SIM7080G Modem.
*
* @api
* @param[in]	N/A
* @return       N/A
* implements    Modem initialization
*/
void initModem(void);

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

/**
* @brief         Retrieve the LocalIP value from the modem
* @api
* @param[in]     N/A.
* @return        IP Address
* @retval        IP Address structure filled out with the obtained values
* implements     getLocalIP
*/
IP_address getLocalIP(void);

/**
* @brief         Retrieve the current status of the SIM and if there is any pending action
* @api
* @param[in]     timeout value in milleconds before the function fails.
* @return        Sim Status value
* @retval		 SIM ERROR = 0. There was an error with the SIM or the communication
* @retval		 SIM_READY = 1. SIM is ready to be used.
* @retval 		 SIM_LOCKED	= 2. SIM is locked and expects user action.
* implements     getSimStatus
*/
SimStatus getSimStatus(uint32_t timeout_ms);

/**
* @brief         Indicates the corresponding Network Mode in which the Modem will be configured
* @api
* @param[in]     Pointer to the string that contains the mode that will be used.
* @return        Boolean Value to validate communication
* @retval		 True - Communication successful
* @retval		 False - Communication failed
* implements     setNetworkMode
*/
bool setNetworkMode(const char* pMode);

/**
* @brief         Reads the current Network Mode in which the modem is configured to operate
* @api
* @param[in]     N/A.
* @return        Integer value indicating the different possible Network Modes
* @retval		 2 - Automatic
* @retval		 13 - GSM only
* @retval		 38 - LTE only
* implements     getNetworkMode
*/
int getNetworkMode(void);

/**
* @brief         Indicates the preferred Mode in which the Modem will be configured
* @api
* @param[in]     Pointer to the string that contains the mode that will be used.
* @return        Boolean Value to validate communication
* @retval		 1 - Communication successful
* @retval		 2 - Communication failed
* implements     setPreferredMode
*/
bool setPreferredMode(char* pMode);

/**
* @brief         Reads the current Preferred Mode in which the modem is configured to operate
* @api
* @param[in]     N/A.
* @return        Integer value indicating the different possible Preferred Modes
* @retval		 1 - CAT-M
* @retval		 2 - NB-IoT
* @retval		 3 - CAT-M and NB-IoT
* implements     getPreferredMode
*/
int getPreferredMode(void);

/**
* @brief         Verifies if there is a valid connection
* @api
* @param[in]     N/A
* @return        Boolean Value to validate connection
* @retval		 True - Device Connected
* @retval		 False - Not Connected
* implements     setPreferredMode
*/
bool isGprsConnected(void);

/**
* @brief         Gets the modem's registration status via CREG/CGREG/CEREG:
* @api
* @param[in]     Command to use to retrieve the Status XREG.
* @return        Integer value indicating the different possible Preferred Modes
* @retval		 1 - CREG = Generic network registration
* @retval		 2 - CGREG = GPRS service registration
* @retval		 3 - CEREG = EPS registration for LTE modules
* implements     getRegistrationStatusXREG
*/
int8_t getRegistrationStatusXREG(const char* regCommand);

/**
* @brief         Retrieves the current registration status of the modem:
* @api
* @param[in]     Command to use to retrieve the current status of the device.
* @return        SIM70xxRegStatus according to the different possibilities
* implements     getRegistrationStatus
*/
SIM70xxRegStatus getRegistrationStatus(void);



#endif /* SIM7080G_H_ */
