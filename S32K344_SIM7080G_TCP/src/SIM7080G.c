/*
 * AT.c
 *
 *  Created on: 2 oct. 2025
 *      Author: jesus
 */

#include "SIM7080G.h"
#include "S32K3_Drivers.h"
#include <string.h>
#include <stdlib.h>

static uint8_t *sockets[MODEM_MUX_COUNT];
static uint8_t *certificate[MODEM_MUX_COUNT];

uint32 remainingBytes = 0;
uint8 Rx_Buffer[MAX_LEN] = {0};

/**
* @brief       	Generate a blocking delay to wait for a given amount of milliseconds.
*
* @api
* @param[in]	milliseconds to wait in the delay
* @return       N/A
* implements    Blocking Delay
*/
void Delay(uint32_t milliseconds){
	DelayImpl(milliseconds);
}

/**
* @brief       	Calls for the MCU implementation to initialize the assigned timeout timer.
*
* @api
* @param[in]	milliseconds to indicate the timeout
* @return       N/A
* implements    InitTimeoutTimer
*/
void InitTimeoutTimer(uint32_t milliseconds){

	/* Calls for the implementation of the initialization of the timeout timer on the specific MCU */
	InitTimeoutTimerImpl(milliseconds);

}

/**
* @brief       	Reads the current value of the timeout timer.
*
* @api
* @param[in]	N/A
* @return       The current ellapsed time of the timeout timer
* implements    GetCurrentTime
*/
uint32_t GetCurrentTime(void){
	uint32_t milliseconds = 0;

	milliseconds = GetCurrentTimeImpl();

	return milliseconds;
}

/**
* @brief       	Calls for the De-Initialization of the timeout timer defined by the MCU.
*
* @api
* @param[in]	N/A
* @return       N/A
* implements    DeinitTimeoutTimer
*/
void DeinitTimeoutTimer(void){
	DeinitTimeoutTimerImpl();
}

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
)
{
	bool status = 0;

	/* Call for the specific implementation due to the reception being managed by the MCU */
	status = streamSkipUntilImpl(expectedChar, timeout_ms);

	/* Returned the obtained status, if the character was found or a timeout occurred */
	return status;
}

/**
* @brief       Calls for the implementation to obtain the integer value from reception until reaching the indicated last character value.
*
* @api
* @param[in]	Character that indicates the ends of the integer.
* @return       Integer value that is obtained before the indicated character.
* implements    GetIntBefore
*/
int GetIntBefore(char lastChar){
	int value = 0;

	/* Call for the implementation of the GetIntBefore as it depends on the reception managed by the MCU */
	value = GetIntBeforeImpl(lastChar);

	return value;
}

/**
* @brief       Calls for the implementation to obtain the integer value from reception until no more integers are received.
*
* @api
* @param[in]	N/A
* @return       Integer value that is obtained from the serial reception.
* 				If 0xFFFF, then an error occurred.
* implements    GetIntBefore
*/
int GetIntResponse(void){
	int value = 0;

	/* Call for the implementation of the GetIntResponse as it depends on the reception managed by the MCU */
	value = GetIntResponseImpl();

	return value;
}

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
IP_address IPAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet) {
    IP_address tempIP;

    tempIP.bytes[0] = 0;
	tempIP.bytes[1] = 0;
	tempIP.bytes[2] = 0;
	tempIP.bytes[3] = 0;

    tempIP.bytes[0] = first_octet;
    tempIP.bytes[1] = second_octet;
    tempIP.bytes[2] = third_octet;
    tempIP.bytes[3] = fourth_octet;

    return tempIP;
}

/**
* @brief       Verifies if two IP Addresses are the same
*
* @api
* @param[in]	First IP address to analyze
* @param[in]	Second IP address to analyze
* @return       Boolean value indicating if the two IP addresses are the same or not
* implements    compareIP
*/
bool compareIP(IP_address IP1, IP_address IP2){
	return (IP1.dword == IP2.dword);
}

/**
* @brief       Generates an IP Address structure from an array
*
* @api
* @param[in]	Pointer to the address of the first element for the IP String Array
* @return       IP Address structure filled out with the values of the string.
* implements    IpFromString
*/
IP_address IpFromString(uint8_t* IPstring){
	uint8_t Parts[4] = {0};

	Parts[0] = IPstring[0];
	Parts[1] = IPstring[1];
	Parts[2] = IPstring[2];
	Parts[3] = IPstring[3];

	return IPAddress(Parts[0], Parts[1], Parts[2], Parts[3]);
}

/**
* @brief        Calls for the implementation of the Wait Response on the specific device.
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
* implements    WaitResponse
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
)
{
	uint8_t response = 0;

	response = waitResponseImpl(timeout_ms, pData, pR1, pR2, pR3, pR4, pR5, pR6, pR7);

	return response;
}

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
)
{
	uint8_t status = 1;

	/* Send the indicated data through implemented interface */
	status = sendATImpl(pData, dataLength);

	/* Wait for a specific amount of time */
	if(delay_ms > 0){
		Delay(delay_ms);
	}

	return status;
}

/**
* @brief        Tests the functionality of the AT commands with the modem in serial communication.
*
* @api
* @param[in]     Indicates the time in milliseconds to wait before a timeout occurs.
* @return        Boolean value
* @retval        True: AT commands communication received response successfully.
* @retval        False: AT commands communication failed.
* implements     Test AT
*/
bool TestAT(uint32_t timeout_ms) {
	uint32_t time_measure = 0;
	uint8_t status = 1;
	uint8_t response = 0;

	/* Initialize the timeout timer to trigger timeout if required */
	InitTimeoutTimer(timeout_ms);

	do{
		/* Send the test AT command over serial with a delay of 100ms */
		status = sendAT("AT\n\r", strlen("AT\n\r"), 100);

		if (status == 0){

			response = waitResponse(200, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);

			if (response == 1){
				return true;
			}
		}
		/* Get current time passed */
		time_measure = GetCurrentTime();
	}while(time_measure < timeout_ms);


	// TODO: Implement timeout function
	/* De-initialize the timer in case further is required */
	DeinitTimeoutTimer();

	return false;
}

IP_address getLocalIP(void){
	uint8_t tmpIP[4] = {0};
	IP_address localIP;

	/* Clean the local IP structure */
	localIP.bytes[0] = 0;
	localIP.bytes[1] = 0;
	localIP.bytes[2] = 0;
	localIP.bytes[3] = 0;

	/* Send the command to retrieve the local IP of the SIM */
	sendAT("+CNACT?", sizeof("+CNACT?"), 0);
	if(1 != waitResponseImpl(0, NULL_PTR, "\r\n+CNACT:", NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
		tmpIP[0] = '\0';
	}
	/* Wait for the message to send \" */
	waitResponse(0, NULL_PTR, "\"", NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	/* Store the received string in received localIP */
	waitResponse(0, (char*)tmpIP, "", "\"", NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	/* Wait for the OK response */
	waitResponse(0, (char*)tmpIP, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);

	/* Translate the IP from string to IP structure */
	localIP = IpFromString(tmpIP);

	/* Return local IP with the specified structure */
	return localIP;
}

SimStatus getSimStatus(uint32_t timeout_ms){
	uint32_t time_measure = 0;
	uint8_t status = 0;

	char Resp_Ready[] 	= "READY";
	char Resp_Pin[] 	= "SIM PIN";
	char Resp_Puk[] 	= "SIM PUK";
	char Resp_Insrt[] 	= "NOT INSERTED";
	char Resp_NotReady[] = "NOT READY";

	/* Initialize the timeout timer to trigger timeout if required */
	InitTimeoutTimer(timeout_ms);

	/* Proceed with the retrieve sequence until the timeout is reached or
	 * a response is obtained from the SIM
	 */
	do{
		/* Send AT command to retrieve the status of the SIM */
		sendAT("+CPIN?", strlen("+CPIN?"), 1000);

		/* Verify that the response obtained first corresponds to +CPIN */
		if (waitResponse(0, NULL_PTR, "+CPIN:", NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR) != 1) {
			Delay(1000);
			continue;
		}

		/* Verify that the response obtained matches one of the possible 5 answers */
		status = waitResponse(0, NULL_PTR, Resp_Ready, Resp_Pin, Resp_Puk, Resp_Insrt, Resp_NotReady, NULL_PTR, NULL_PTR);

		/* Wait until next response is obtained */
		waitResponse(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);

		/* Analyze if the response obtained matches "READY",
		 * otherwise it will report and error code
		 */
		switch (status) {
		case 2:
			return SIM_ERROR;
		case 3:
			return SIM_LOCKED;
		case 1:
			return SIM_READY;
		default:
			return SIM_ERROR;
		}
	}while(time_measure < timeout_ms);

	/* If a timeout ocurred, the return value is SIM_ERROR */
	return SIM_ERROR;
}


bool setNetworkMode(const char* pMode) {
	// 2 Automatic
	// 13 GSM only
	// 38 LTE only
	sendAT("+CNMP=", strlen("+CNMP="), 0);
	sendAT(pMode, strlen(pMode), 100);

	if(1 == waitResponse(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
		return true;
	} else {
		return false;
	}

	return false;
}

int getNetworkMode(void) {
	int mode = 0;

	sendAT("+CNMP?", strlen("+CNMP?"), 100);

	if (1 != waitResponse(0, NULL_PTR, "\r\n+CNMP?", NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
		mode = -9999;
		return mode;
	}

	mode = GetIntResponse();

	waitResponse(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);

	return mode;
}

bool setPreferredMode(char* pMode) {
	// 1 CAT-M
	// 2 NB-IoT
	// 3 CAT-M and NB-IoT
	bool status = false;

	/* Send the AT command to set the preferred mode in the SIM */
	sendAT("+CMNB=", strlen("+CMNB="), 0);
	/* Send the specific mode that needs to be set in the SIM */
	sendAT(pMode, strlen(pMode), 100);

	/* Wait to receive an OK response, with default value */
	if(1 == waitResponse(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
		status = true;
	} else {
		status = false;
	}

	/* Return end status */
	return status;
}

int getPreferredMode(void) {
	int mode = 0;

	/* Send the AT command to request the preferred mode configured in the SIM */
	sendAT("+CMNB?", strlen("+CMNB?"), 100);

	/* Wait to receive the specific response indicating that the following number is the mode that is assigned*/
	if (1 != waitResponse(0, NULL_PTR, "\r\n+CMNB:", NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
		mode = -1;
		return mode;
	}

	/* Wait to retrieve the specific mode from the SIM */
	mode = GetIntResponse();

	/* Wait to receive a successful response */
	waitResponse(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);

	/* Return end status */
	return mode;
}

bool isGprsConnected(void) {
	IP_address localIP;
	IP_address cmpIP = {0};

	int res = 0;
	sendAT("+CGATT?", sizeof("+CGATT?"), 0);
	if(1 != waitResponse(0, NULL_PTR, "+CGATT:", NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
		return false;
	}
	res = GetIntBefore('\n');
	waitResponse(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	if(res != 1){
		return false;
	}

	localIP = getLocalIP();

	if (!compareIP(localIP, cmpIP)){

	}

	return false;
}

/****************************************************************
 * 	Gets the modem's registration status via CREG/CGREG/CEREG:  *
 * 		CREG = Generic network registration                     *
 * 		CGREG = GPRS service registration                       *
 * 		CEREG = EPS registration for LTE modules                *
 ****************************************************************/

int8_t getRegistrationStatusXREG(const char* regCommand){
	char CREG[] = "+CREG:";
	char CGREG[] = "+CGREG:";
	char CEREG[] = "+CEREG:";
	uint8_t resp = 0;
	int8_t status = 0;

	sendAT("+", strlen("+"), 0);
	sendAT(regCommand, strlen(regCommand), 0);
	sendAT("?", strlen("?"), 0);

	/* Read the expected response comparing against CREG, CGREG and CEREG */
	resp = waitResponse(0, NULL_PTR, CREG, CGREG, CEREG, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	if(resp != 1 && resp != 2 && resp != 3){
		status = 0;
	} else {
		streamSkipUntil(',', 0);

		status = GetIntResponse();
		waitResponse(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	}


	return status;
}

SIM70xxRegStatus getRegistrationStatus(void){
	SIM70xxRegStatus epsStatus = REG_NO_RESULT;

	epsStatus = (SIM70xxRegStatus)getRegistrationStatusXREG("CEREG");
	  // If we're connected on EPS, great!
	  if ((epsStatus == REG_OK_HOME) || (epsStatus == REG_OK_ROAMING)) {
	    return epsStatus;
	  } else {
	    // Otherwise, check GPRS network status
	    // We could be using GPRS fall-back or the board could be being moody
	    return (SIM70xxRegStatus)getRegistrationStatusXREG("CGREG");
	  }

	return epsStatus;
}

