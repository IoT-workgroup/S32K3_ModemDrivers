/*
 * device_drivers.c
 *
 *  Created on: Oct 16, 2025
 *      Author: nxf63523
 */
#include <string.h>
#include <stdlib.h>

#include "S32K3_Drivers.h"
#include "SIM7080G.h"

static const char GSM_OK[] = "OK\r\n";
static const char GSM_ERROR[] = "ERROR\r\n";

uint8 InternalRx_Buffer[MAX_LEN] = {0};

/****************************************************************************************
 ************************* LOCAL FUNCTION DEFINITIONS **********************************
 ****************************************************************************************/
static bool handleURCs(const char *str);

/**
* @brief       	Compare that the given string ends with the specified suffix.
*
* @api
* @param[in]	String to analyze
* @param[in[	Suffix to look for at the end of the String
* @return       True if the suffix is at the end of the string and False if it's not found.
* implements    endsWith comparison API
*/
static bool endsWith(const char *str, const char *suffix);


/****************************************************************************************
 ************************* LOCAL FUNCTION DECLARATIONS **********************************
 ****************************************************************************************/
static bool handleURCs(const char *str){
	bool result;

	if(endsWith(str, "something")){
		result = FALSE;
	}

	return result;

}

/**
* @brief       	Compare that the given string ends with the specified suffix.
*
* @api
* @param[in]	String to analyze
* @param[in[	Suffix to look for at the end of the String
* @return       True if the suffix is at the end of the string and False if it's not found.
* implements    endsWith comparison API
*/
static bool endsWith(const char *str, const char *suffix){
	if (!str || !suffix)
		return 0;
	size_t lenstr = strlen(str);
	size_t lensuffix = strlen(suffix);
	if (lensuffix >  lenstr)
		return 0;
	return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

/*****************************************************************************************
 ************************* GLOBAL FUNCTION DECLARATIONS **********************************
 *****************************************************************************************/

/**
* @brief       	Initialize S32K3 Low Level Drivers for serial communication and timers usage.
*
* @api
* @param[in]	 N/A
* @return        N/A
* implements     S32K3 Initialization
*/
void init_device_drivers(void){
    /* Initialize S32K3 clock for the configured operation frequency  */
    Clock_Ip_Init(&Clock_Ip_aClockConfig[0]);

    /* Initialize all pins that are used for the application*/
    Siul2_Port_Ip_Init(NUM_OF_CONFIGURED_PINS_PortContainer_0_VS_0, g_pin_mux_InitConfigArr_PortContainer_0_VS_0);

    /*Initialize STM driver for STM instance 0 */
	Stm_Ip_Init(STM_INST_0, &STM_0_InitConfig_PB_VS_0);

	/*Initialize STM Instance 0 - channel 0 */
	Stm_Ip_InitChannel(STM_INST_0, STM_0_ChannelConfig_PB_VS_0);

	/* Enable the configured UART interrupts if any */
	IntCtrl_Ip_Init(&IntCtrlConfig_0);

    /* Initializes the UART driver */
    Lpuart_Uart_Ip_Init(DEBUG_UART_INSTANCE, &Lpuart_Uart_Ip_xHwConfigPB_3_VS_0);
}

/**
* @brief       	Generate a blocking delay to wait for a given amount of milliseconds.
*
* @api
* @param[in]	milliseconds to wait in the delay
* @return       N/A
* implements    Blocking Delay
*/
void DelayImpl(uint32_t milliseconds){
	uint32_t ms_count_conversion = 0;

	/* Convert the milliseconds parameters to the specific counts required to reach */
	ms_count_conversion = milliseconds*(STM_CLOCK_FREQ/(STM_CLK_PRESCALER*1000));

	/* Start the STM timer with a defined start value */
	Stm_Ip_StartTimer(STM_INST_0, STM_START_CNT_VALUE);

	/* Enable the compare channel as comparison will be monitored*/
	Stm_Ip_EnableChannel(STM_INST_0, CH_0);

	/* Set the compare value to monitor the counter value and raise the flag once the values are the same */
	Stm_Ip_StartCounting(STM_INST_0, CH_0, ms_count_conversion);

	while(!Stm_Ip_GetInterruptStatusFlag(STM_INST_0, CH_0)){
		/* Wait for the counter to reach the compare value, in this case the ms_conversion */
	}
	/* Once complete, there is no need to clear the flag as it will be cleared in the next call, and the channel and
	 * instance are disabled next
	 */

	/* Disable the compare channel as the comparison is no longer needed */
	Stm_Ip_DisableChannel(STM_INST_0, CH_0);

	/* Stop the timer */
	Stm_Ip_StopTimer(STM_INST_0);
}

/**
* @brief       	Initialize the timer that will be used for timeout operations.
*
* @api
* @param[in]	 milliseconds
* @return        N/A
* implements     Init Timeout Timer
*/
void InitTimeoutTimerImpl(uint32_t milliseconds){
	uint32_t ms_count_conversion = 0;

	/* Convert the milliseconds parameters to the specific counts required to reach */
	ms_count_conversion = milliseconds*(STM_CLOCK_FREQ/(STM_CLK_PRESCALER*1000));

	/* Start the STM timer with a defined start value */
	Stm_Ip_StartTimer(STM_INST_0, STM_START_CNT_VALUE);

	/* Enable the compare channel as comparison will be monitored*/
	Stm_Ip_EnableChannel(STM_INST_0, CH_0);

	/* Set the compare value to monitor the counter value and raise the flag once the values are the same */
	Stm_Ip_StartCounting(STM_INST_0, CH_0, ms_count_conversion);
}

/**
* @brief       	Gets the ellapsed time on the timer used for the timeout.
*
* @api
* @param[in]	 N/A
* @return        Returns the ellapsed time in milliseconds of the running timer
* implements     Get Current Time
*/
uint32_t GetCurrentTimeImpl(void){
	uint32_t milliseconds = 0;

	milliseconds = Stm_Ip_GetCounterValue(STM_INST_0);
	milliseconds = milliseconds/(STM_CLOCK_FREQ/(STM_CLK_PRESCALER*1000));

	return milliseconds;
}

/**
* @brief       	DeInitialize the timer used for the timeout.
*
* @api
* @param[in]	 N/A
* @return        N/A
* implements     DeInitialize Timeout timer
*/
void DeinitTimeoutTimerImpl(void){
	/* Disable the compare channel as the comparison is no longer needed */
	Stm_Ip_DisableChannel(STM_INST_0, CH_0);

	/* Stop the timer */
	Stm_Ip_StopTimer(STM_INST_0);
}

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
uint8_t waitResponseImpl(
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
	uint8_t index = 0;
	uint8_t response = 0;
	char character = 0;

	/* Verify if any timeout value different from 0 was provided */
	if(timeout_ms == 0){
		/* Default timeout value is set to 1000ms */
		timeout_ms = 1000;
	}

	/* Verify for default value on data*/
	if(NULL_PTR == pData){

		/* Clear internal RX buffer before using it */
		memset((uint8_t*)InternalRx_Buffer, 0, MAX_LEN);

		/* If no data buffer is provided, then use internal RX buffer */
		pData = (char *)InternalRx_Buffer;
	}

	/* Verify for default value on r1*/
	if(NULL_PTR == pR1){
		/* If no Response 1 is provided, then default it to GSM_OK */
		pR1 = GSM_OK;
	}

	/* Verify for default value on r2*/
	if(NULL_PTR == pR2){
		/* If no Response 2 is provided, then default it to GSM_ERROR */
		pR2 = GSM_ERROR;
	}

	/* Initialize the timeout timer */
	InitTimeoutTimerImpl(timeout_ms);

	do{
		/* Retrieve the UART character for the reception */
		Lpuart_Uart_Ip_SyncReceive(AT_UART_INSTANCE,(uint8 *)&character, 1, 10000);

		/* Verify that the character received is not NULL, if it's NULL then skip it */
		if (character <= 0){
			continue;
		}

		/* Store the character in the indicated data buffer */
		*(pData+(index++)) = character;

		/* Verify if the Data Buffer ends with the string found in pR1 */
		if(pR1 && endsWith((const char*)pData, (const char*)pR1)){
			response = 1;
			break;

		/* Verify if the Data Buffer ends with the string found in pR2 */
		} else if (pR2 && endsWith((const char*)pData, (const char*)pR2)){
			response = 2;
			break;

		/* Verify if the Data Buffer ends with the string found in pR3 */
		} else if (pR3 && endsWith((const char*)pData, (const char*)pR3)){
			response = 3;
			break;

		/* Verify if the Data Buffer ends with the string found in pR4 */
		} else if (pR4 && endsWith((const char*)pData, (const char*)pR4)){
			response = 4;
			break;

		/* Verify if the Data Buffer ends with the string found in pR5 */
		} else if (pR5 && endsWith((const char*)pData, (const char*)pR5)){
			response = 5;
			break;

		/* Verify if the Data Buffer ends with the string found in pR6 */
		} else if (pR6 && endsWith((const char*)pData, (const char*)pR6)){
			response = 6;
			break;

		/* Verify if the Data Buffer ends with the string found in pR7 */
		} else if (pR7 && endsWith((const char*)pData, (const char*)pR7)){
			response = 7;
			break;
		} else if (handleURCs((const char*)pData)){

		}

	/* This loop will occur until a expected response is received or until a timeout occurs */
	}while(GetCurrentTimeImpl() < timeout_ms);

	/* De-initialize the timeout timer once out of the function main execution */
	DeinitTimeoutTimerImpl();

	/* Return obtained response number, if no matching case then default value 0 will be returned */
	return response;
}

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
)
{
	uint8_t command_status = 0;
	uint32 T_timeout = 0x0000FFFF;
	Lpuart_Uart_Ip_StatusType lpuartStatus = LPUART_UART_IP_STATUS_ERROR;

	/* Verify if the Buffer is an existing command to send, to include the AT. Otherwise just send the received command */
	if((*pData) == '+'){
		/* Add the AT to the beginning of the transmission */
		lpuartStatus = Lpuart_Uart_Ip_SyncSend(AT_UART_INSTANCE,(uint8_t *)"AT", strlen("AT"), T_timeout);
		if (LPUART_UART_IP_STATUS_SUCCESS != lpuartStatus)
		{
			command_status = 1;
		}

		/* Send the received buffer if the AT command was sent previously */
		if(command_status != 1){
			lpuartStatus = Lpuart_Uart_Ip_SyncSend(AT_UART_INSTANCE,(uint8_t *)pData, dataLength, T_timeout);
			if (LPUART_UART_IP_STATUS_SUCCESS != lpuartStatus)
			{
				command_status = 1;
			}
		}
	} else {

		/* Only send the received buffer */
		lpuartStatus = Lpuart_Uart_Ip_SyncSend(AT_UART_INSTANCE,(uint8_t *)pData, dataLength, T_timeout);
		if (LPUART_UART_IP_STATUS_SUCCESS != lpuartStatus)
		{
			command_status = 1;
		}
	}

	return command_status;
}

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
)
{
	/* Initialize local variables to 0 */
	char receivedChar = 0;
	bool status = FALSE;

	/* Verify if timeout defined by user is valid, if no timeout selected, then go with the default */
	if(timeout_ms == 0){
		timeout_ms = 1000;
	}

	/* Initialize timeout timer to reach expected timeout value */
	InitTimeoutTimerImpl(timeout_ms);

	/* Main process of the function, verify received character */
	do{
		Lpuart_Uart_Ip_SyncReceive(AT_UART_INSTANCE,(uint8 *)&receivedChar, 1, 10000);
		if (expectedChar == receivedChar){
			status = TRUE;
			break;
		}
	}while(GetCurrentTimeImpl() < timeout_ms);

	/* De-Initialize timer as operation has finished */
	DeinitTimeoutTimerImpl();

	/* Return current status */
	return status;
}

/**
* @brief       Obtain the integer value from reception until reaching the indicated last character value.
*
* @api
* @param[in]	Character that indicates the ends of the integer.
* @return       Integer value that is obtained before the indicated character.
* implements    GetIntBeforeImpl
*/
int GetIntBeforeImpl(const char lastChar){
	volatile int value = 0;
	char buffer[7] = {0};
	char character = 0;
	uint8_t index = 0;

	do{
		Lpuart_Uart_Ip_SyncReceive(AT_UART_INSTANCE,(uint8 *)&character, 1, 10000);
		if (character != lastChar){
			buffer[index++] = character;
		}
	}while(character != lastChar);

	if(index && index < 7){
		buffer[index] = '\0';
		value = atoi(buffer);

		return value;
	}

	return -9999;
}

/**
* @brief       Obtain the integer value from reception until no more integers are received.
*
* @api
* @param[in]	N/A
* @return       Integer value that is obtained from the serial reception.
* 				If 0xFFFF, then an error occurred.
* implements    GetIntBeforeImpl
*/
int GetIntResponseImpl(void){
	int value = 0;
	uint8_t i = 0;
	bool numberDetected = false;
	uint8_t receivedChar = 0;
	uint32_t timeout_ms = 1000;
	Lpuart_Uart_Ip_StatusType ReceiveStatus = LPUART_UART_IP_STATUS_ERROR;

	/* Initialize timeout timer to reach expected timeout value */
	InitTimeoutTimerImpl(timeout_ms);

	/* Clear with 0 the reception buffer */
	memset((uint8_t*)InternalRx_Buffer, 0, MAX_LEN);

	/* Main process of the function, verify received character and store into the buffer if it is a number */
	do{
		ReceiveStatus = Lpuart_Uart_Ip_SyncReceive(AT_UART_INSTANCE,(uint8 *)&receivedChar, 1, 10000);
		if((receivedChar >= 48) && (receivedChar <=57) && (LPUART_UART_IP_STATUS_SUCCESS == ReceiveStatus)){
			numberDetected = true;
			InternalRx_Buffer[i++] = receivedChar;
		} else {
			numberDetected = false;
		}
	}while((GetCurrentTimeImpl() < timeout_ms) && numberDetected);

	/* De-Initialize timer as operation has finished */
	DeinitTimeoutTimerImpl();

	/* If i is greater than 0, it indicates that there was at least a number received */
	if(i != 0){
		/* If possible, transform the received buffer to the corresponding integer value */
		value = atoi((const char*)InternalRx_Buffer);
	} else {
		/* Saturate return value to indicate error occurred */
		value = 0xFFFF;
	}

	return value;
}






/* Implementation taken from Arduino module */
//bool testATImpl(uint32_t timeout_ms = 10000L) {
//  for (uint32_t start = millis(); millis() - start < timeout_ms;) {
//    thisModem().sendAT(GF(""));
//    if (thisModem().waitResponse(200) == 1) { return true; }
//    delay(100);
//  }
//  return false;
//}
//
//bool testAT(uint32_t timeout_ms = 10000L) {
//  return thisModem().testATImpl(timeout_ms);
//}

//SimStatus getSimStatusImpl(uint32_t timeout_ms = 10000L) {
//	for (uint32_t start = millis(); millis() - start < timeout_ms;) {
//		thisModem().sendAT(GF("+CPIN?"));
//		if (thisModem().waitResponse(GF("+CPIN:")) != 1) {
//			delay(1000);
//			continue;
//		}
//		int8_t status =
//				thisModem().waitResponse(GF("READY"), GF("SIM PIN"), GF("SIM PUK"),
//						GF("NOT INSERTED"), GF("NOT READY"));
//		thisModem().waitResponse();
//
//		switch (status) {
//		case 2:
//		case 3: return SIM_LOCKED;
//		case 1: return SIM_READY;
//		default: return SIM_ERROR;
//		}
//	}
//	return SIM_ERROR;
//}
//
//SimStatus getSimStatus(uint32_t timeout_ms = 10000L) {
//  return thisModem().getSimStatusImpl(timeout_ms);
//}

//bool setNetworkMode(uint8_t mode) {
//  // 2 Automatic
//  // 13 GSM only
//  // 38 LTE only
//  // 51 GSM and LTE only
//  thisModem().sendAT(GF("+CNMP="), mode);
//  return thisModem().waitResponse() == 1;
//}

//bool setPreferredMode(uint8_t mode) {
//  // 1 CAT-M
//  // 2 NB-IoT
//  // 3 CAT-M and NB-IoT
//  thisModem().sendAT(GF("+CMNB="), mode);
//  return thisModem().waitResponse() == 1;
//}

//int16_t getPreferredMode() {
//  thisModem().sendAT(GF("+CMNB?"));
//  if (thisModem().waitResponse(GF(AT_NL "+CMNB:")) != 1) { return false; }
//  int16_t mode = thisModem().streamGetIntBefore('\n');
//  thisModem().waitResponse();
//  return mode;
//}
//
//int16_t getNetworkMode() {
//  thisModem().sendAT(GF("+CNMP?"));
//  if (thisModem().waitResponse(GF(AT_NL "+CNMP:")) != 1) { return false; }
//  int16_t mode = thisModem().streamGetIntBefore('\n');
//  thisModem().waitResponse();
//  return mode;
//}

//
//SIM70xxRegStatus getRegistrationStatus() {
//  SIM70xxRegStatus epsStatus =
//      (SIM70xxRegStatus)thisModem().getRegistrationStatusXREG("CEREG");
//  // If we're connected on EPS, great!
//  if (epsStatus == REG_OK_HOME || epsStatus == REG_OK_ROAMING) {
//    return epsStatus;
//  } else {
//    // Otherwise, check GPRS network status
//    // We could be using GPRS fall-back or the board could be being moody
//    return (SIM70xxRegStatus)thisModem().getRegistrationStatusXREG("CGREG");
//  }
//}
//
//// Gets the modem's registration status via CREG/CGREG/CEREG
//// CREG = Generic network registration
//// CGREG = GPRS service registration
//// CEREG = EPS registration for LTE modules
//int8_t getRegistrationStatusXREG(const char* regCommand) {
//  thisModem().sendAT('+', regCommand, '?');
//  // check for any of the three for simplicity
//  int8_t resp = thisModem().waitResponse(GF("+CREG:"), GF("+CGREG:"),
//                                         GF("+CEREG:"));
//  if (resp != 1 && resp != 2 && resp != 3) { return -1; }
//  thisModem().streamSkipUntil(','); /* Skip format (0) */
//  int status = thisModem().stream.parseInt();
//  thisModem().waitResponse();
//  return status;
//}
//
//// Checks if current attached to GPRS/EPS service
//bool isGprsConnected() {
//  return thisModem().isGprsConnectedImpl();
//}
//
//// Checks if current attached to GPRS/EPS service
//bool isGprsConnectedImpl() {
//  thisModem().sendAT(GF("+CGATT?"));
//  if (thisModem().waitResponse(GF("+CGATT:")) != 1) { return false; }
//  int8_t res = thisModem().streamGetIntBefore('\n');
//  thisModem().waitResponse();
//  if (res != 1) { return false; }
//
//  return thisModem().localIP() != IPAddress(0, 0, 0, 0);
//}

//static inline IPAddress TinyGsmIpFromString(const String& strIP) {
//  int Parts[4] = {
//      0,
//  };
//  int Part = 0;
//  for (uint8_t i = 0; i < strIP.length(); i++) {
//    char c = strIP[i];
//    if (c == '.') {
//      Part++;
//      if (Part > 3) { return IPAddress(0, 0, 0, 0); }
//      continue;
//    } else if (c >= '0' && c <= '9') {
//      Parts[Part] *= 10;
//      Parts[Part] += c - '0';
//    } else {
//      if (Part == 3) break;
//    }
//  }
//  return IPAddress(Parts[0], Parts[1], Parts[2], Parts[3]);
//}
