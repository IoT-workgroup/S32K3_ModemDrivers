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
char Rx_Buffer[MAX_LEN] = {0};

/**
* @brief       	Proceeds with hardware initialization for the SIM7080G Modem.
*
* @api
* @param[in]	N/A
* @return       N/A
* implements    Modem initialization
*/
void initModem(void){
	ModemResetImpl();
}

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
	uint8_t Parts[IPV4_ELEMENTS] = {0};
	uint8_t StringIndex = 0;
	uint8_t IpIndex = 0;

	/* Look for the start of the IP address */
	for(StringIndex = 0; (StringIndex < MAX_LEN && IpIndex < IPV4_ELEMENTS); StringIndex++){
		if((IPstring[StringIndex] == '"') || (IPstring[StringIndex] == '.')){
			Parts[IpIndex++] = atoi((const char *)&IPstring[StringIndex+1]);
		}
	}

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
	uint16_t dataLength,
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
		status = sendAT("AT\r", strlen("AT\r"), 0);

		if (status == 0){

			response = waitResponse(200, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);

			if (response == 1){
				DeinitTimeoutTimer();
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

/**
* @brief         Retrieve the LocalIP value from the modem
* @api
* @param[in]     N/A.
* @return        IP Address
* @retval        IP Address structure filled out with the obtained values
* implements     getLocalIP
*/
IP_address getLocalIP(void){
	IP_address localIP;

	/* Clean the local IP structure */
	localIP.bytes[0] = 0;
	localIP.bytes[1] = 0;
	localIP.bytes[2] = 0;
	localIP.bytes[3] = 0;

	/* Clear Rx_Buffer for later analysis */
	memset(Rx_Buffer, 0, MAX_LEN);

	/* Send the command to retrieve the local IP of the SIM */
	sendAT("+CNACT?\r", strlen("+CNACT?\r"), 0);
	/* Wait for the response to finish with OK or another value and store it in internal buffer */
	if(1 == waitResponse(0, Rx_Buffer, "OK\r\n", NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
		/* Translate the IP obtained from the response to IP structure */
		localIP = IpFromString((uint8*)&Rx_Buffer[0]);
	}

	/* Return local IP with the specified structure */
	return localIP;
}

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
SimStatus getSimStatus(uint32_t timeout_ms){
	uint32_t time_measure = 0;
	uint8_t status = 0;
	SimStatus RetVal = SIM_ERROR;

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
		time_measure = GetCurrentTime();
		/* Send AT command to retrieve the status of the SIM */
		sendAT("+CPIN?\r", strlen("+CPIN?\r"), 0);

		/* Verify that the response obtained first corresponds to +CPIN */
		status = waitResponse(0, NULL_PTR, "+CPIN: READY", "+CPIN: SIM PIN", "+CPIN: SIM PUK", "+CPIN: NOT INSERTED", "+CPIN: NOT READY", NULL_PTR, NULL_PTR);

		/* Analyze if the response obtained matches "READY",
		 * otherwise it will report and error code
		 */
		switch (status) {
		case 1:
			RetVal = SIM_READY;
			break;
		case 2:
			RetVal = SIM_LOCKED;
			break;
		case 3:
			RetVal = SIM_LOCKED;
			break;
		default:
			RetVal = SIM_ERROR;
			break;
		}
	}while((time_measure < timeout_ms) && (status == 0));

	DeinitTimeoutTimer();

	/* If a timeout ocurred, the return value is SIM_ERROR */
	return RetVal;
}

/**
* @brief         Indicates the corresponding Network Mode in which the Modem will be configured
* @api
* @param[in]     Pointer to the string that contains the mode that will be used.
* @return        Boolean Value to validate communication
* @retval		 True - Communication successful
* @retval		 False - Communication failed
* implements     setNetworkMode
*/
bool setNetworkMode(const char* pMode) {
	uint8_t status = 0;
	bool RetVal = false;

	// 2 Automatic
	// 13 GSM only
	// 38 LTE only
	sendAT("+CNMP=", strlen("+CNMP="), 0);
	sendAT(pMode, strlen(pMode), 0);
	sendAT("\r", strlen("\r"), 0);

	status = waitResponse(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);

	if(1 == status){
		RetVal = true;
	} else {
		RetVal = false;
	}

	return RetVal;
}

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
int getNetworkMode(void) {
	int mode = 0;

	sendAT("+CNMP?\r", strlen("+CNMP?\r"), 100);

	if (1 != waitResponse(0, NULL_PTR, "\r\n+CNMP:", NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
		mode = -9999;
		return mode;
	}

	mode = GetIntResponse();

	waitResponse(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);

	return mode;
}

/**
* @brief         Indicates the preferred Mode in which the Modem will be configured
* @api
* @param[in]     Pointer to the string that contains the mode that will be used.
* @return        Boolean Value to validate communication
* @retval		 1 - Communication successful
* @retval		 2 - Communication failed
* implements     setPreferredMode
*/
bool setPreferredMode(const char* pMode) {
	// 1 CAT-M
	// 2 NB-IoT
	// 3 CAT-M and NB-IoT
	uint8_t resp = 0;
	bool RetVal = false;

	/* Send the AT command to set the preferred mode in the SIM */
	sendAT("+CMNB=", strlen("+CMNB="), 0);
	/* Send the specific mode that needs to be set in the SIM */
	sendAT(pMode, strlen(pMode), 0);
	sendAT("\r", strlen("\r"), 0);

	resp = waitResponse(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);

	/* Wait to receive an OK response, with default value */
	if(1 == resp){
		RetVal = true;
	} else {
		RetVal = false;
	}

	/* Return end status */
	return RetVal;
}

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
int getPreferredMode(void) {
	int mode = 0;

	/* Send the AT command to request the preferred mode configured in the SIM */
	sendAT("+CMNB?\r", strlen("+CMNB?\r"), 100);

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

/**
* @brief         Verifies if there is a valid connection
* @api
* @param[in]     N/A
* @return        Boolean Value to validate connection
* @retval		 True - Device Connected
* @retval		 False - Not Connected
* implements     setPreferredMode
*/
bool isGprsConnected(void) {
	bool RetVal = false;
	IP_address localIP;
	IP_address cmpIP = {
		.bytes[0] = 0,
		.bytes[1] = 0,
		.bytes[2] = 0,
		.bytes[3] = 0
	};

	sendAT("+CGATT?\r", strlen("+CGATT?\r"), 0);
	if(2 == waitResponse(0, NULL_PTR, "+CGATT: 0\r\n", "+CGATT: 1\r\n", NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
		/* Wait 10ms before sending next command */
		Delay(10);
		localIP = getLocalIP();
		if (!compareIP(localIP, cmpIP)){
			RetVal = true;
		}
	}

	return RetVal;
}

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
int8_t getRegistrationStatusXREG(const char* regCommand){
	char CREG[] = "+CREG:";
	char CGREG[] = "+CGREG:";
	char CEREG[] = "+CEREG:";
	uint8_t resp = 0;
	int8_t status = 0;

	sendAT("+", strlen("+"), 0);
	sendAT(regCommand, strlen(regCommand), 0);
	sendAT("?\r", strlen("?\r"), 0);

	/* Read the expected response comparing against CREG, CGREG and CEREG */
	resp = waitResponse(0, NULL_PTR, ",0", ",1", ",2", ",3", ",4", ",5", NULL_PTR);
	//	resp = waitResponse(0, Response, CREG, CGREG, CEREG, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	if(resp == 0){
		status = REG_UNKNOWN;
	} else {
		status = resp-1;
//		/* Clear internal RX buffer before using it */
//		memset((uint8_t*)Response, 0, MAX_LEN);
//
//		resp = waitResponse(0, Response, ",", NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
//
//		status = GetIntResponse();
//		waitResponse(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	}


	return status;
}

/**
* @brief         Retrieves the current registration status of the modem:
* @api
* @param[in]     Command to use to retrieve the current status of the device.
* @return        SIM70xxRegStatus according to the different possibilities
* implements     getRegistrationStatus
*/
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

/**
* @brief         Enables the GNSS functionality of the modem
* @api
* @param[in]     Void - N/A
* @return        Retrieves the whether the command to enable th GPS was sucessful or not
* implements     enableGPS
*/
bool enableGPS(void){
	bool retVal = false;
	sendAT("+CGNSPWR=1\r", strlen("+CGNSPWR=1\r"), 0);
	if(1 != waitResponse(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
		retVal = false;
	} else {
		retVal = true;
	}

	return retVal;
}

/**
* @brief         Disables the GNSS functionality of the modem
* @api
* @param[in]     Void - N/A
* @return        Retrieves the whether the command to disable the GPS was sucessful or not
* implements     disableGPS
*/
bool disableGPS(void){
	bool retVal = false;
	sendAT("+CGNSPWR=0\r", strlen("+CGNSPWR=0\r"), 0);
	if(1 != waitResponse(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
		retVal = false;
	} else {
		retVal = true;
	}

	return retVal;
}

uint8_t getGPSraw(char* ResultBuffer, uint8_t length){
	uint8_t retVal = 0;
	const uint8_t emptyVal = 0xAA;
	

	/* Verify that the Result Buffer is not poiting to a null pointer */
	if (NULL_PTR == ResultBuffer){
		retVal = 1;	
	}

	if(retVal == 0){
		/* Verify that the length of the given buffer is less or equal to the max Size */
		if(length > GPS_MAX_CHARS){
			retVal = 1;
		} else {
			/* Verify that all the elements in the array are initalized and the memory is reserved */
			for(uint8_t i = 0; i < GPS_MAX_CHARS; i++){\
				/* If any of the elements is different than the expected value, then break execution */
				if(ResultBuffer[i] != emptyVal){
					retVal = 1;
					break;	
				} else {
					continue;
				}
			}
		}
	}

	if(retVal == 0){
		sentAT("+CGNSINF\r", strlen("+CGNSINF\r"), 0);
		waitResponse(10000, ResultBuffer, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	}

	return retVal;
}

uint8_t getGPS(float* lat, float* lon, float* speed, 
			   float* alt, int* vsat, int* usat, 
			   float* accuracy, int* year, int* month, 
			   int* day, int* hour, int* minute, int* second){
	uint8_t retVal = 0;
	uint8_t TempBuffer[GPS_MAX_CHARS] = {0};
	uint8_t DateBuffer[5] = {0};
	uint8_t StringIndex = 0;
	uint8_t ParamIndex = GPS_COMMAND_SIZE;

	/* Tempooral variables to retrieve the information from the GPS */
	float 	tempLat 		= 0;
	float 	tempLon 		= 0;
	float 	tempSpeed 		= 0;
	float 	tempAlt 		= 0;
	int 	tempVsat 		= 0;
	int 	tempUsat 		= 0;
	float 	tempAccuracy 	= 0;
	int 	tempYear 		= 0;
	int 	tempMonth 		= 0;
	int 	tempDay 		= 0;
	int 	tempHour 		= 0;
	int 	tempMin 		= 0;
	float 	tempSeconds 	= 0;

	/* Send request to transmit the data */
	sentAT("+CGNSINF\r", strlen("+CGNSINF\r"), 0);

	/* Store the response in the Tempooral Buffer */
	if(1 != waitResponse(10000, TempBuffer, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
		/* Analyze Temporal Buffer with the information retrieved */
	
	/* Check on the GNSS run status */
	if((retVal != 1){
		if (1 == TempBuffer[ParamIndex++]){
			/* This indicates that the GNSS is turned on */
			/* Next element is a comma, so skip it */
			ParamIndex++;
		} else {
			/* GNSS is turned OFF, indicate error */
			retVal = 1;
		}
	} 

/* Check on the Fix Status */
	if(1 == TempBuffer[ParamIndex++]){
		/* This indicates that there is a fixed position obtained */
		/* Skip the next element as it is a comma */
		ParamIndex++;
	} else {
		/* There is no fixed position obtained */
		retVal = 1;
	}

	if(',' == TempBuffer[ParamIndex]){
		/* No date retrieved from the GPS reading */
		ParamIndex++;
	} else {
		memcpy(DateBuffer, &TempBuffer[ParamIndex], GPS_YEAR_OFFSET);
		/* Retrieve the Year value and convert from string to int */
		tempYear = atoi(DateBuffer);

		/* Move through the array to the next element of the date */			
		ParamIndex += GPS_YEAR_OFFSET;

		/* Clear the Date Buffer for next data */
		memset(DateBuffer, 0, sizeof(DateBuffer));
		
		/* Fill the Date Buffer with the month */
		memcpy(DateBuffer, &TempBuffer[ParamIndex], GPS_MONTH_OFFSET);

		/* Retrieve the Month value from the temporal buffer */
		tempMonth = atoi(DateBuffer);

		/* Move through the array to the next element of the date */			
		ParamIndex += GPS_MONTH_OFFSET;

		memset(DateBuffer, 0, sizeof(DateBuffer));

		memcpy(DateBuffer, &TempBuffer[ParamIndex], GPS_DAY_OFFSET);

		tempDay = atoi(DateBuffer);

		ParamIndex += GPS_DAY_OFFSET;

		memset(DateBuffer, 0, sizeof(DateBuffer));

		memcpy(DateBuffer, &TempBuffer[ParamIndex], GPS_HOUR_OFFSET);

		tempHour = atoi(DateBuffer);

		ParamIndex += GPS_HOUR_OFFSET;

		memset(DateBuffer, 0, sizeof(DateBuffer));

		memcpy(DateBuffer, &TempBuffer[ParamIndex], GPS_MINUTES_OFFSET);

		tempMin = atoi(DateBuffer);

		ParamIndex += GPS_MINUTES_OFFSET;
		
		memset(DateBuffer, 0, sizeof(DateBuffer));

		memcpy(DateBuffer, &TempBuffer[ParamIndex], GPS_SECONDS_OFFSET);

		tempSeconds = atof(DateBuffer);

		ParamIndex += GPS_SECONDS_OFFSET;

		ParamIndex++;
	}

	tempLat = atof((uint8_t*) &TempBuffer[ParamIndex]);

	/* Skip over the latitude to find the next parameter received */
	for(ParamIndex; ParamIndex < GPS_MAX_CHARS; ParamIndex++){
		TempBuffer[ParamIndex] == ',';
		break;
	}

	/* Obtain the value for Longitude*/
	tempLon = atof((uint8_t*) &TempBuffer[ParamIndex]);

	/* Skip over the longitude to find the next parameter received */
	for(ParamIndex; ParamIndex < GPS_MAX_CHARS; ParamIndex++){
		TempBuffer[ParamIndex] == ',';
		break;
	}

	tempAlt = atof((uint8_t*) &TempBuffer[ParamIndex]);

	/* Skip over the Altitude to find the next parameter received */
	for(ParamIndex; ParamIndex < GPS_MAX_CHARS; ParamIndex++){
		TempBuffer[ParamIndex] == ',';
		break;
	}

	tempSpeed = atof((uint8_t*) &TempBuffer[ParamIndex]);

	/* Skip over the Speed Over Ground to find the next parameter received */
	for(ParamIndex; ParamIndex < GPS_MAX_CHARS; ParamIndex++){
		TempBuffer[ParamIndex] == ',';
		break;
	}

	/* Skip over the Course Over Ground to find the next parameter received */
	for(ParamIndex; ParamIndex < GPS_MAX_CHARS; ParamIndex++){
		TempBuffer[ParamIndex] == ',';
		break;
	}

	/* Skip over the Fix Mode to find the next parameter received */
	for(ParamIndex; ParamIndex < GPS_MAX_CHARS; ParamIndex++){
		TempBuffer[ParamIndex] == ',';
		break;
	}

	/* Skip over the Reserved 1 param to find the next parameter received */
	for(ParamIndex; ParamIndex < GPS_MAX_CHARS; ParamIndex++){
		TempBuffer[ParamIndex] == ',';
		break;
	}

	tempAccuracy = atof((uint8_t*) &TempBuffer[ParamIndex]);

	/* Skip over the Horizontal Dilution of Precision param to find the next parameter received */
	for(ParamIndex; ParamIndex < GPS_MAX_CHARS; ParamIndex++){
		TempBuffer[ParamIndex] == ',';
		break;
	}

	/* Skip over the Position Dilution of Precision param to find the next parameter received */
	for(ParamIndex; ParamIndex < GPS_MAX_CHARS; ParamIndex++){
		TempBuffer[ParamIndex] == ',';
		break;
	}

	/* Skip over the Vertical Dilution of Precision param to find the next parameter received */
	for(ParamIndex; ParamIndex < GPS_MAX_CHARS; ParamIndex++){
		TempBuffer[ParamIndex] == ',';
		break;
	}

	/* Skip over the Reserved2 param to find the next parameter received */
	for(ParamIndex; ParamIndex < GPS_MAX_CHARS; ParamIndex++){
		TempBuffer[ParamIndex] == ',';
		break;
	}

	tempVsat = atoi((uint8_t*) &TempBuffer[ParamIndex]);

	/* Skip over the GPS Satellites in View param to find the next parameter received */
	for(ParamIndex; ParamIndex < GPS_MAX_CHARS; ParamIndex++){
		TempBuffer[ParamIndex] == ',';
		break;
	}

	tempUsat = atoi((uint8_t*) &TempBuffer[ParamIndex]);
			
	} else {
		retVal = 1;
	}
	
	

	return retVal;
}
