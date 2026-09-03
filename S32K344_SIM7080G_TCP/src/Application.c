
#include "Application.h"
#include "SIM7080G.h"
#include <string.h>
#include <stdio.h>

const char Automatic_Network[] = "2";
const char Modem_CATM[] = "1";
char buffer[1024] = {0};

// cayenne server address and port
const char server[]   = "io.adafruit.com";
const int  port       = 8883;
char username[] = "ie714410";
char password[] = "aio_aLwI06Dmz09Q9D1IPzy8ioq7RgM8";
char clientID[] = "ESP32";


static void connect_mqtt(void);

//
//void power_down(void)
//{
//	Siul2_Dio_Ip_SetGPDO(SIUL2_INSTANCE, 30);
//	DelayImpl(2000);
//	Siul2_Dio_Ip_ClearGPDO(SIUL2_INSTANCE, 30);
//	DelayImpl(2000);
//}
//
//void power_sequence(void)
//{
//	Siul2_Dio_Ip_SetGPDO(SIUL2_INSTANCE, 30);
//	DelayImpl(2000);
//	Siul2_Dio_Ip_ClearGPDO(SIUL2_INSTANCE, 30);
//	DelayImpl(2000);
//}

static void connect_mqtt(void){
	uint8_t appStatus = 0;

	snprintf(buffer, 1024, "+SMCONF=\"URL\",\"%s\",%d", server, port);
	sendAT(buffer, sizeof(buffer), 0);
	appStatus = waitResponse(1000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	while(appStatus != 1){
	}

	snprintf(buffer, 1024, "+SMCONF=\"USERNAME\",\"%s\"", username);
	sendAT(buffer, sizeof(buffer), 0);
	appStatus = waitResponse(1000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	while(appStatus != 1){
	}

	snprintf(buffer, 1024, "+SMCONF=\"PASSWORD\",\"%s\"", password);
	sendAT(buffer, sizeof(buffer), 0);
	appStatus = waitResponse(1000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	while(appStatus != 1){
	}

	snprintf(buffer, 1024, "+SMCONF=\"CLIENTID\",\"%s\"", clientID);
	sendAT(buffer, sizeof(buffer), 0);
	appStatus = waitResponse(1000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	while(appStatus != 1){
	}

	do{
		sendAT("+SMCONN\r\n", sizeof("+SMCONN\r\n"), 0);
		appStatus = waitResponse(3000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
		while(appStatus != 1){
		}
	}while(appStatus != 1);

	/* MQTT client connected */
}

void test_all_functionality(void){
	uint8_t function_status = 0;
	boolean functionality = FALSE;
	SimStatus Sim_Status = SIM_ERROR;
	IP_address localIP = {0};
	int networkMode = 0;
	int preferredMode = 0;
	SIM70xxRegStatus RegStatus = REG_UNKNOWN;


	/* Initial validation of Send AT */
	function_status = sendAT("Probando\r\n", sizeof("Probando\r\n"), 0);
	function_status = waitResponse(100000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	while(1 != function_status);

	function_status = sendAT("Probando\r\n", sizeof("Probando\r\n"), 0);
	function_status = waitResponse(100000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	while(1 != function_status);

	function_status = sendAT("Probando\r\n", sizeof("Probando\r\n"), 0);
	function_status = waitResponse(100000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	while(1 != function_status);

	function_status = sendAT("OK\r\n", sizeof("OK\r\n"), 100);
	/* Try out the Wait Response implementation, expectation is to receive an OK so default parameters are given */
	function_status = waitResponse(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	while(1 != function_status);

	function_status = sendAT("ERROR\r\n", sizeof("ERROR\r\n"), 0);
	/* Try out the Wait Response implementation, expectation is to receive an ERROR so default parameters are given */
	function_status = waitResponse(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	while(2 != function_status);

	function_status = sendAT("OUTOFBOUNDS\r\n", sizeof("OUTOFBOUNDS\r\n"), 0);
	/* Try out the Wait Response implementation, expectation is to receive an OUTOFBOUNDS so default parameters are given and expected 0 */
	function_status = waitResponse(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	while(0 != function_status);

	/* Try out the TestAT function to validate the expected behavior */
	functionality = TestAT(10000);
	while(false == functionality);

	functionality = setNetworkMode("2");
	while(false == functionality);

	networkMode = getNetworkMode();
	while(2 != networkMode);

	functionality = setNetworkMode("13");
	while(false == functionality)

	networkMode = getNetworkMode();
	while(13 != networkMode);

	functionality = setNetworkMode("38");
	while(false == functionality)

	networkMode = getNetworkMode();
	while(38 != networkMode);

	functionality = setPreferredMode("1");
	while(false == functionality);

	preferredMode = getPreferredMode();
	while(1 != preferredMode);

	functionality = setPreferredMode("2");
	while(false == functionality);

	preferredMode = getPreferredMode();
	while(2 != preferredMode);

	functionality = setPreferredMode("3");
	while(false == functionality);

	preferredMode = getPreferredMode();
	while(3!= preferredMode);

	functionality = isGprsConnected();
	while(false == functionality);

	localIP = getLocalIP();

	function_status = getRegistrationStatusXREG("CREG");

	function_status = getRegistrationStatusXREG("CEREG");

	function_status = getRegistrationStatusXREG("CGREG");

	RegStatus = getRegistrationStatus();

	RegStatus = getRegistrationStatus();

	RegStatus = getRegistrationStatus();

	RegStatus = getRegistrationStatus();

	RegStatus = getRegistrationStatus();

	RegStatus = getRegistrationStatus();

	RegStatus = getRegistrationStatus();

	while(1){

	}
}

void application_iot(void){
	SimStatus simStatus = SIM_ERROR;
	SIM70xxRegStatus regStatus = REG_NO_RESULT;
	uint8_t appStatus = 0;
	initModem();

	/* Verify if the SIM CARD is inserted */
	simStatus = getSimStatus(1000);
	if(simStatus != SIM_READY){
		/* Send status message to indicate is not working */
	}

	/* Disable RF functionality of the modem */
	sendAT("+CFUN=0\r\n", sizeof("+CFUN=0\r\n"), 0);
	appStatus = waitResponse(1000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	if(appStatus != 1){
		/* Disable RF failed */
	}

	if(!setNetworkMode(Automatic_Network)){
		/* Network Mode Configuration failed */
	}

	if(setPreferredMode(Modem_CATM)){
		/* Preferred Mode Configuration failed */
	}

	// Set the APN manually. Some operators need to set APN first when registering the network.
	sendAT("+CGDCONT=1,\"IP\",\"emnify\"\r\n", sizeof("+CGDCONT=1,\"IP\",emnify\"\r\n"), 0);
	appStatus = waitResponse(1000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	if(appStatus != 1){
		/* Set Operators failed */
	}

	// Set the APN manually. Some operators need to set APN first when registering the network.
	sendAT("+CNCFG=0,1,\"emnify\"\r\n", sizeof("+CNCFG=0,1,\"emnify\"\r\n"), 0);
	appStatus = waitResponse(1000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	if(appStatus != 1){
		/* Config apn Failed */
	}

	/* Enable RF functionality of the modem */
	sendAT("+CFUN=1\r\n", sizeof("+CFUN=0\r\n"), 0);
	appStatus = waitResponse(1000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	if(appStatus != 1){
		/* Disable RF failed */
	}

	while (regStatus != REG_OK_HOME && regStatus != REG_OK_ROAMING){
		regStatus = getRegistrationStatus();
	}


	 // Activate network bearer, APN can not be configured by default,
	    // if the SIM card is locked, please configure the correct APN and user password, use the gprsConnect() method

	if(!isGprsConnected()){

		sendAT("+CNACT=0,1\r\n", sizeof("+CNACT=0,1\r\n"), 0);
		appStatus = waitResponse(1000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
		if(appStatus != 1){
			/* Activate network bearer Failed! */
		}
	}

    // Before connecting, you need to confirm that the time has been synchronized.
	sendAT("+CCLK?\r\n", sizeof("+CCLK?\r\n"), 0);
	appStatus = waitResponse(30000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);

	// If it is already connected, disconnect it first
	sendAT("+SMDISC\r\n", sizeof("+SMDISC\r\n"), 0);
	appStatus = waitResponse(30000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);

	connect_mqtt();
}

//void tcp_test(void)
//{
//	power_sequence();
//	Lpuart_Uart_Ip_SyncSend(DEBUG_UART_INSTANCE,(uint8_t *)"\r\nwait 15 seconds for signal\r\n\r\n",sizeof("\r\nwait 15 seconds for signal\r\n\r\n")-1, 0xFFFF);
//	DelayImpl(15000);
//
////	HAL_UART_Transmit(&huart2,(uint8_t *)CSQ,sizeof(CSQ)-1,0xff);
//	send_at(CSQ, NULL, sizeof(CSQ)-1, 0, 1, 1000);
//
////	HAL_UART_Transmit(&huart2,(uint8_t *)QUERY_CPSI,sizeof(QUERY_CPSI)-1,0xff);
//	send_at(QUERY_CPSI, NULL, sizeof(QUERY_CPSI)-1, 0, 1, 1000);
//
////	HAL_UART_Transmit(&huart2,(uint8_t *)EN_CNACT,sizeof(EN_CNACT)-1,0xff);
//	send_at(EN_CNACT, NULL, sizeof(EN_CNACT)-1, 0, 1, 1000);
//
////	HAL_UART_Transmit(&huart2,(uint8_t *)CACID,sizeof(CACID)-1,0xff);
//	send_at(CACID, NULL, sizeof(CACID)-1, 0, 1, 1000);
//
////	HAL_UART_Transmit(&huart2,(uint8_t *)CAOPEN,sizeof(CAOPEN)-1,0xff);
//	send_at(CAOPEN, NULL, sizeof(CAOPEN)-1, 0, 1, 1000);
//
////	HAL_UART_Transmit(&huart2,(uint8_t *)CASEND,sizeof(CASEND)-1,0xff);
////	HAL_UART_Transmit(&huart2,(uint8_t *)MESSAGES,sizeof(MESSAGES)-1,0xff);
////	HAL_UART_Transmit(&huart2,(uint8_t *)"\r\n",2,0xff);
//	send_at(CASEND, MESSAGES, sizeof(CASEND)-1, sizeof(MESSAGES)-1, 2, 1000);
//
////	HAL_UART_Transmit(&huart2,(uint8_t *)CACLOSE,sizeof(CACLOSE)-1,0xff);
//	send_at(CACLOSE, NULL, sizeof(CACLOSE)-1, 0, 1, 1000);
//
////	HAL_UART_Transmit(&huart2,(uint8_t *)DIS_CNCAT,sizeof(DIS_CNCAT)-1,0xff);
//	send_at(DIS_CNCAT, NULL, sizeof(DIS_CNCAT)-1, 0, 1, 1000);
//	power_sequence();
//}
//
//bool isConnect(void){
//	sendAT("+SMSTATE?", sizeof("+SMSTATE?"), 0);
//	if(1 == waitResponseImpl(0, NULL_PTR, "+SMSTATE: ", NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
//		if(1 == waitResponseImpl(0, NULL_PTR, "\r", NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
//			return GetIntResponse();
//		}
//	}
//
//	return false;
//}
//
//uint8_t connectMQTT(void){
//	uint8_t status = 0;
//
//	sendAT("+SMCONF=\"URL\",\"io.adafruit.com\",8883", sizeof("+SMCONF=\"URL\",\"io.adafruit.com\",8883"), 0);
//	if (1 != waitResponseImpl(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
//		status = 1;
//		return status;
//	}
//
//	sendAT("+SMCONF=\"USERNAME\",\"ie714410\"", sizeof("+SMCONF=\"USERNAME\",\"ie714410"), 0);
//	if (1 != waitResponseImpl(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
//		status = 1;
//		return status;
//	}
//
//	sendAT("+SMCONF=\"PASSWORD\",", sizeof("+SMCONF=\"PASSWORD\","), 0);
//	if (1 != waitResponseImpl(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
//		status = 1;
//		return status;
//	}
//
//	sendAT("+SMCONF=\"CLIENTID\",\"ESP32\"", sizeof("+SMCONF=\"CLIENTID\",\"ESP32\""), 0);
//	if (1 != waitResponseImpl(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
//		status = 1;
//		return status;
//	}
//
//	do{
//		sendAT("+SMCONN", sizeof("+SMCONN"), 0);
//		/* Attempting to connect */
//	} while(1 != waitResponseImpl(3000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR));
//
//	return status;
//}

