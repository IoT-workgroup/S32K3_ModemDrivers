/*
 * device_drivers.c
 *
 *  Created on: Oct 16, 2025
 *      Author: nxf63523
 */
#include "device_drivers.h"
#include "AT.h"
#include <string.h>
#include <stdlib.h>

char Resp_Ready[] 	= "READY";
char Resp_Pin[] 	= "SIM PIN";
char Resp_Puk[] 	= "SIM PUK";
char Resp_Insrt[] 	= "NOT INSERTED";
char Resp_NotReady[] = "NOT READY";

static uint8_t *sockets[MODEM_MUX_COUNT];
static uint8_t *certificate[MODEM_MUX_COUNT];



static bool TestATImpl(uint32_t timeout_ms);
static SimStatus getSimStatusImpl(uint32_t timeout_ms);
static int8_t getRegistrationStatusXREG(const char* regCommand);
static bool isGprsConnectedImpl(void);
static void getLocalIPImpl(uint8_t* localIP);

void streamWrite(uint8_t* pAT, uint8_t* pCmd, uint8_t *pAT_NL){
	/* Send AT start of command */
	Lpuart_Uart_Ip_SyncSend(AT_UART_INSTANCE, pAT, (uint32_t)strlen(pAT), MAX_TIMEOUT);

	/* Send AT command */
	Lpuart_Uart_Ip_SyncSend(AT_UART_INSTANCE, pCmd, (uint32_t)strlen(pCmd), MAX_TIMEOUT);

	/* Send AT New Line */
	Lpuart_Uart_Ip_SyncSend(AT_UART_INSTANCE, pAT_NL, (uint32_t)strlen(pAT_NL), MAX_TIMEOUT);
}

/** @brief 	Implementation of StreamSkipUntil, the idea is to allow to detec specific characters in the reception
 * 	@details
 * 			Iteration over the reception array to detect a specific character and continue the operation indicating to the
 * 			application that either the character was found or there was a timeout.
 */
uint8_t streamSkipUntil
(
	uint8_t expectedChar,
	uint32_t timeout_ms
)
{
	/* Initialize local variables to 0 */
	uint8_t receivedChar = 0;
	uint32_t startMillis = 0;
	uint8_t status = 0;

	/* Verify if timeout defined by user is valid, if no timeout selected, then go with the default */
	if(timeout_ms == 0){
			timeout_ms = 1000;
	}

	/* Initialize timeout timer to reach expected timeout value */
	InitTimeoutTimer(timeout_ms);

	/* Main process of the function, verify received character */
	do{
		Lpuart_Uart_Ip_SyncReceive(AT_UART_INSTANCE,(uint8 *)&receivedChar, 1, 10000);
		if (receivedChar == expectedChar){
			status = true;
			break;
		}
	}while(GetCurrentTime() < timeout_ms);

	/* De-Initialize timer as operation has finished */
	DeinitTimeoutTimer();

	/* Return current status */
	return status;
}


void init_device_drivers(void){
    /* Init clock  */
    Clock_Ip_Init(&Clock_Ip_aClockConfig[0]);

    /* Initialize all pins */
    Siul2_Port_Ip_Init(NUM_OF_CONFIGURED_PINS_PortContainer_0_VS_0, g_pin_mux_InitConfigArr_PortContainer_0_VS_0);

    /*Initial STM instance 0 - Channel 0*/
	Stm_Ip_Init(STM_INST_0, &STM_0_InitConfig_PB_VS_0);

	/*Initial channel 0 */
	Stm_Ip_InitChannel(STM_INST_0, STM_0_ChannelConfig_PB_VS_0);

	/* Enable the configured UART interrupts */
	IntCtrl_Ip_Init(&IntCtrlConfig_0);

    /* Initializes an UART driver*/
    Lpuart_Uart_Ip_Init(DEBUG_UART_INSTANCE, &Lpuart_Uart_Ip_xHwConfigPB_3_VS_0);
}

static bool TestATImpl(uint32_t timeout_ms){
	uint32_t time_measure = 0;
	at_status status = AT_ERROR;
	uint8_t response = 0;

	/* Initialize the timeout timer to trigger timeout if required */
	InitTimeoutTimer(timeout_ms);

	do{
		/* Send the test AT command over serial with a delay of 100ms */
		status = sendAT("AT", strlen("AT"), 100);

		if (status == AT_SENT){

			response = waitResponseImpl(200, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);

			if (response = 1){
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



bool TestAT(uint32_t timeout_ms) {
  return TestATImpl(timeout_ms);
}

static SimStatus getSimStatusImpl(uint32_t timeout_ms) {
	uint32_t time_measure = 0;
	uint8_t status = 0;
	char * pExpectedResponse[5]= {Resp_Ready, Resp_Pin, Resp_Puk, Resp_Insrt, Resp_NotReady};

	/* Initialize the timeout timer to trigger timeout if required */
	InitTimeoutTimer(timeout_ms);

	/* Proceed with the retrieve sequence until the timeout is reached or
	 * a response is obtained from the SIM
	 */
	do{
		/* Send AT command to retrieve the status of the SIM */
		send_at("+CPIN?", NULL, strlen("+CPIN?"), 0, 1, 1000);

		/* Verify that the response obtained first corresponds to +CPIN */
		if (verifyResponse("+CPIN:", 1) != 1) {
			delay_at(1000);
			continue;
		}

		/* Verify that the response obtained matches one of the possible 5 answers */
		status = verifyResponse(pExpectedResponse, 5);

		/* Wait until next response is obtained */
		verifyResponse(NULL_PTR, (uint8_t)0);

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

SimStatus getSimStatus(uint32_t timeout_ms){
	/* Call internally implemented function and pass back the return value */
	return getSimStatusImpl(timeout_ms);
}

bool setNetworkMode(char* pMode) {
	// 2 Automatic
	// 13 GSM only
	// 38 LTE only
	send_at("+CNMP=", pMode, strlen("+CNMP="), strlen(pMode), 2, 100);

	if(verifyResponse((char*)NULL_PTR, 0) == 1){
		return true;
	} else {
		return false;
	}

	return false;
}

bool setPreferredMode(char* pMode) {
	// 1 CAT-M
	// 2 NB-IoT
	// 3 CAT-M and NB-IoT
	send_at("+CMNB=",  pMode, strlen("+CMNB="), strlen(pMode), 2, 100);

	if(verifyResponse((char*)NULL_PTR, 0) == 1){
		return true;
	} else {
		return false;
	}
	return false;
}

int getPreferredMode(void) {
	int mode = 0;

	send_at("+CMNB?", NULL, strlen("+CMNB?"), 0, 1, 100);

	if (verifyResponse("\r\n+CMNB:", 1) != 1){
		mode = -1;
		return mode;
	}

	mode = GetIntResponse();

	verifyResponse((char*)NULL_PTR, 0);

	return mode;
}

int getNetworkMode(void) {
	int mode = 0;

	send_at("+CNMP?", NULL, strlen("+CNMP?"), 0, 1, 100);

	if (verifyResponse("\r\n+CNMP?", 1) != 1){
		mode = -9999;
		return mode;
	}

	mode = GetIntResponse();

	verifyResponse((char*)NULL_PTR, 0);

	return mode;
}

SIM70xxRegStatus getRegistrationStatus(void){
	SIM70xxRegStatus epsStatus = REG_NO_RESULT;

	return epsStatus;
}

/****************************************************************
 * 	Gets the modem's registration status via CREG/CGREG/CEREG:  *
 * 		CREG = Generic network registration                     *
 * 		CGREG = GPRS service registration                       *
 * 		CEREG = EPS registration for LTE modules                *
 ****************************************************************/
static int8_t getRegistrationStatusXREG(const char* regCommand){
	uint8_t CREG[] = "+CREG:";
	uint8_t CGREG[] = "+CGREG:";
	uint8_t CEREG[] = "+CEREG:";
	uint8_t resp = 0;
	int8_t status = 0;

	/* Read the expected response comparing against CREG, CGREG and CEREG */
	resp = waitResponseImpl(0, NULL_PTR, CREG, CGREG, CEREG, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	if(resp != 1 && resp != 2 && resp != 3){
		status = 0;
	} else {
		streamSkipUntil(',', 0);

		status = GetIntResponse();
		waitResponseImpl(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	}


	return status;
}

bool isGprsConnected(void) {
  return isGprsConnectedImpl();
}

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
}

boolean compareIP(IP_address IP1, IP_address IP2){
	return IP1.dword != IP2.dword;
}

IP_address localIP(void){
	uint8_t tmpIP[4] = {0};
	IP_address localIP;

	localIP.bytes[0] = 0;
	localIP.bytes[1] = 0;
	localIP.bytes[2] = 0;
	localIP.bytes[3] = 0;

	getLocalIP(tmpIP);
	localIP = IpFromString(tmpIP);

	return localIP;
}

void getLocalIP(uint8_t* localIP){
	getLocalIPImpl(localIP);
}

static void getLocalIPImpl(uint8_t* localIP){
	sendAT("+CNACT?", sizeof("+CNACT?"), 0);
	if(1 != waitResponseImpl(0, NULL_PTR, "\r\n+CNACT:", NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
		localIP[0] = '\0';
	}
	/* Wait for the message to send \" */
	waitResponseImpl(0, NULL_PTR, "\"", NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	/* Store the received string in received localIP */
	waitResponseImpl(0, localIP, "", "\"", NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	/* Wait for the OK response */
	waitResponseImpl(0, localIP, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
}

IP_address IpFromString(uint8_t* IPstring){
	uint8_t Parts[4] = {0};

	Parts[0] = IPstring[0];
	Parts[1] = IPstring[1];
	Parts[2] = IPstring[2];
	Parts[3] = IPstring[3];

	return IPAddress(Parts[0], Parts[1], Parts[2], Parts[3]);
}

static bool isGprsConnectedImpl(void) {
	IP_address localIP;
	IP_address cmpIP;

	int res = 0;
	sendAT("+CGATT?", sizeof("+CGATT?"), 0);
	if(1 != waitResponseImpl(0, NULL_PTR, "+CGATT:", NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
		return false;
	}
	res = GetIntBefore('\n');
	waitResponseImpl(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	if(res != 1){
		return false;
	}

	//localIP = localIP(localIP);

	return false;
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
