
#include "Application.h"
#include "SIM7080G.h"
#include "S32K3_Drivers.h"
#include <string.h>
#include <stdio.h>

/* Adafruit username */
#define IO_USERNAME  "serviti"

/*  Adafruit IO Key */

#define IO_TOPIC	"Use-Case"

const char Automatic_Network[] = "2";
const char Modem_CATM[] = "1";
const char NB_IOT[]="2";
const char NB_IOT_AND_CATM[]="3";
char buffer[1024] = {0};

// cayenne server address and port
const char server[]   = "io.adafruit.com";
const int  port       = 8883;
char username[] = IO_USERNAME;
char password[] = IO_KEY;
char clientID[] = "ESP32";
char topic[] = IO_TOPIC;


static void connect_mqtt(void);

static bool isConnect()
{
	uint8_t appStatus = 0;

    sendAT("+SMSTATE?\r", strlen("+SMSTATE?\r"), 0);
    appStatus = waitResponse(3000, "+SMSTATE: ", NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
    if(appStatus == 1){
    	return GetIntResponse();
    }


    return false;
}

static void connect_mqtt(void){
	uint8_t appStatus = 0;

	snprintf(buffer, 1024, "+SMCONF=\"URL\",\"%s\",%d", server, port);
	sendAT(buffer, strlen(buffer), 0);
	appStatus = waitResponse(1000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	while(appStatus != 1){
	}

	snprintf(buffer, 1024, "+SMCONF=\"USERNAME\",\"%s\"", username);
	sendAT(buffer, strlen(buffer), 0);
	appStatus = waitResponse(1000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	while(appStatus != 1){
	}

	snprintf(buffer, 1024, "+SMCONF=\"PASSWORD\",\"%s\"", password);
	sendAT(buffer, strlen(buffer), 0);
	appStatus = waitResponse(1000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	while(appStatus != 1){
	}

	snprintf(buffer, 1024, "+SMCONF=\"CLIENTID\",\"%s\"", clientID);
	sendAT(buffer, strlen(buffer), 0);
	appStatus = waitResponse(1000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	while(appStatus != 1){
	}

	do{
		sendAT("+SMCONN\r\n", strlen("+SMCONN\r\n"), 0);
		appStatus = waitResponse(3000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
		while(appStatus != 1){
		}
	}while(appStatus != 1);

	/* MQTT client connected */
}

/* Function to retrieve overall status */
static getAllStatus(void){
	/* Display Product Information */
	sendAT("ATI\r", strlen("ATI\r"), 1000);

	/* Verify PIN related information */
	sendAT("+CPIN?\r", strlen("+CPIN?\r"), 1000);

	/* Verify RF functionality to analyze initial state */
	/* If result obtained from the command is 0 or 4, then the radio is turned off */
	sendAT("+CFUN?\r", strlen("+CFUN?\r"), 1000);

	/* Verify preferred mode and network mode */
	sendAT("+CNMP?\r", strlen("+CNMP?\r"), 1000);
	sendAT("+CMNB?\r", strlen("+CMNB?\r"), 1000);

	/* Verify which bands are available */
	sendAT("+CBANDCFG?\r", strlen("+CBANDCFG?\r"), 20000);

	/* Verify EPS Network Registration Status */
	sendAT("+CEREG?\r", strlen("+CEREG?\r"), 1000);

	/* Verify Network Registration Status */
	sendAT("+CGREG?\r", strlen("+CGREG?\r"), 1000);

	/* Verify quality signal from the Antenna, expected a different value from 99,99 */
	sendAT("+CSQ\r", strlen("+CSQ\r"), 1000);

	/* Obtain Inquiry UE System Information */
	sendAT("+CPSI?\r", strlen("+CPSI?\r"), 1000);

	/* Request for the available bands */
	sendAT("+ANTENALLCFG?\r", strlen("+ANTENALLCFG?\r"), 1000);

	/* Verify the current mode and list of available operators */
	sendAT("+COPS=?\r", strlen("+COPS=?\r"), 200000);

	/* Wait until a response comes from the COPS command */
	waitResponse(999999, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);

}

void test_all_functionality(void){
	uint8_t function_status = 0;
	boolean functionality = FALSE;
	SimStatus Sim_Status = SIM_ERROR;
	IP_address localIP = {0};
	int networkMode = 0;
	int preferredMode = 0;
	SIM70xxRegStatus RegStatus = REG_UNKNOWN;

	initModem();

	/* Initial validation of Send AT */
	while(1 != function_status){
//		function_status = sendAT("A", strlen("A"), 500);
//		function_status = sendAT("T", strlen("T"), 500);
//		function_status = sendAT("A", strlen("A"), 500);
//		function_status = sendAT("T", strlen("T"), 500);
		function_status = sendAT("AT\r", strlen("AT\r"), 0);
		function_status = waitResponse(5000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	}

	function_status = sendAT("Probando\r\n", strlen("Probando\r\n"), 0);
	function_status = waitResponse(100000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	while(1 != function_status);

	function_status = sendAT("Probando\r\n", strlen("Probando\r\n"), 0);
	function_status = waitResponse(100000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	while(1 != function_status);

	function_status = sendAT("OK\r\n", strlen("OK\r\n"), 100);
	/* Try out the Wait Response implementation, expectation is to receive an OK so default parameters are given */
	function_status = waitResponse(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	while(1 != function_status);

	function_status = sendAT("ERROR\r\n", strlen("ERROR\r\n"), 0);
	/* Try out the Wait Response implementation, expectation is to receive an ERROR so default parameters are given */
	function_status = waitResponse(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	while(2 != function_status);

	function_status = sendAT("OUTOFBOUNDS\r\n", strlen("OUTOFBOUNDS\r\n"), 0);
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

/* Purpose is to analyze the commands signals using Logic Analyzer */
void application_diagnosis(void){
	SimStatus simStatus = SIM_ERROR;
	SIM70xxRegStatus regStatus = REG_NO_RESULT;
	uint8_t appStatus = 0;
	uint8_t GetReg = 1;

	/* Enable modem with reset trigger */
	initModem();

	/* Wait for the Auto-Baudrate to occur */
	while(1 != appStatus){
		sendAT("AT\r", strlen("AT\r"), 0);
		appStatus = waitResponse(5000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	}

	/* Disable ECHO */
	sendAT("ATE0\r", strlen("ATE0\r"), 0);
	appStatus = waitResponse(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	while(1 != appStatus){
		/* Turn-on Blue LED - ECHO was not disabled */
		Siul2_Dio_Ip_SetGPDO(SIUL2_INSTANCE, 31);
	}

	getAllStatus();

	/* Indicate that the RF functionality will be disabled and wait for 10s for the reset to occur */
	sendAT("+CFUN=0\r", strlen("+CFUN=0\r"), 10000);

	/* Send command to set the CAT-M functionality on both preferred mode and network mode */
	sendAT("+CNMP=38\r", strlen("+CNMP=38\r"), 0);
	waitResponse(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);

	/* Send command to set the CAT-M functionality on both preferred mode and network mode */
	sendAT("+CMNB=1\r", strlen("+CMNB=1\r"), 0);
	waitResponse(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);

	/* Send command to enable RF functionality and wait for 10s for the reset */
	sendAT("+CFUN=1\r", strlen("+CFUN=1\r"), 10000);

	sendAT("+CBANDCFG=\"CAT-M\",1,2,3,4,5,8,12,13,14,18,19,20,25,26,27,28,66,85\r", strlen("+CBANDCFG=\"CAT-M\",1,2,3,4,5,8,12,13,14,18,19,20,25,26,27,28,66,85\r"), 1000);

	/* Verify the current mode and list of available operators */
	sendAT("+COPS=?\r", strlen("+COPS=?\r"), 200000);

	/* Wait until a response comes from the COPS command */
	waitResponse(999999, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);


	while(1){
		/* Reach the end of the diagnosis application */
		/* Keep sending the CEREG command to verify connection every ten seconds */
		while(GetReg != 0){
			sendAT("+CEREG?\r", strlen("+CEREG?\r"), 1000);
			sendAT("+CGREG?\r", strlen("+CGREG?\r"), 10000);
		}

		sendAT("+CGATT?\r", sizeof("+CGATT?\r"), 1000);

		/* Obtain the local IP */
		sendAT("+CNACT?\r", sizeof("+CNACT?\r"), 1000);

		sendAT("+CNACT=0,1\r", strlen("+CNACT=0,1\r"), 1000);

		sendAT("+CNACT?\r", sizeof("+CNACT?\r"), 1000);

	}
}

void application_iot(void){
	SimStatus simStatus = SIM_ERROR;
	SIM70xxRegStatus regStatus = REG_NO_RESULT;
	uint8_t appStatus = 0;
	initModem();

	/* Wait for the Auto-Baudrate to occur */
	while(1 != appStatus){
		sendAT("AT\r", strlen("AT\r"), 0);
		appStatus = waitResponse(5000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	}

	/* Disable ECHO */
	sendAT("ATE0\r", strlen("ATE0\r"), 0);
	appStatus = waitResponse(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	while(1 != appStatus){
		/* ECHO was not disabled */
		Siul2_Dio_Ip_SetGPDO(SIUL2_INSTANCE, 31);
	}

	sendAT("+CFUN=?\r", strlen("+CFUN=?\r"), 0);


	sendAT("+CSQ\r", strlen("+CSQ\r"), 0);

	sendAT("+CFUN=1\r", strlen("+CFUN=1\r"), 0);

	Delay(10000);

	/* Verify if the SIM CARD is inserted */
	simStatus = getSimStatus(1000);
	if(simStatus != SIM_READY){
		/* Send status message to indicate is not working */
		Siul2_Dio_Ip_SetGPDO(SIUL2_INSTANCE, 31);
	}

//	sendAT("\r", strlen("\r"), 0);
//	Delay(5000);

//	/* Disable RF functionality of the modem */
	sendAT("+CFUN=0\r", strlen("+CFUN=0\r"), 0);
//	appStatus = waitResponse(1000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
//	if(appStatus != 1){
//		/* Disable RF failed */
//		Siul2_Dio_Ip_SetGPDO(SIUL2_INSTANCE, 31);
//		while(1);
//	}

	Delay(10000);



	if(!setNetworkMode(Automatic_Network)){
		/* Network Mode Configuration failed */
		Siul2_Dio_Ip_SetGPDO(SIUL2_INSTANCE, 31);
		while(1);
	}

	Delay(5000);

	if(!setPreferredMode(NB_IOT_AND_CATM)){
		/* Preferred Mode Configuration failed */
		Siul2_Dio_Ip_SetGPDO(SIUL2_INSTANCE, 31);
		while(1);
	}

	Delay(10000);

//	// Set the APN manually. Some operators need to set APN first when registering the network.
//	/* TODO: modify emnify information for personal implementation */
	sendAT("+CGDCONT=1,\"IP\",\"internet.itelcel.com\"\r", strlen("+CGDCONT=1,\"IP\",\"internet.itelcel.com\"\r"), 0);
	appStatus = waitResponse(1000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	if(appStatus != 1){
		/* Set Operators failed */
	}

	// Set the APN manually. Some operators need to set APN first when registering the network.
	sendAT("+CNCFG=0,1,\"internet.itelcel.com\"\r", strlen("+CNCFG=0,1,\"internet.itelcel.com\"\r"), 0);
//	sendAT("+CNCFG=0,1,\"internet.itelcel.com\",\"webgprs\",\"webgprs2002\"\r", strlen("+CNCFG=0,1,\"internet.itelcel.com\",\"webgprs\",\"webgprs2002\"\r"), 0);
	appStatus = waitResponse(1000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
	if(appStatus != 1){
		/* Config apn Failed */
	}

	/* Enable RF functionality of the modem */
	sendAT("+CFUN=1\r", strlen("+CFUN=1\r"), 0);
//	appStatus = waitResponse(1000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
//	if(appStatus != 1){
//		/* Enable RF failed */
//	}

	Delay(10000);

//	/* Define PLMN to look for the TELCEL one */
	sendAT("+COPS=0\r", strlen("+COPS=0\r"), 0);
	appStatus = waitResponse(1200000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);

	while (regStatus != REG_OK_HOME && regStatus != REG_OK_ROAMING){
		regStatus = getRegistrationStatus();
		Delay(1000);
	}


	// Activate network bearer, APN can not be configured by default,
	// if the SIM card is locked, please configure the correct APN and user password, use the gprsConnect() method

	if(!isGprsConnected()){

		sendAT("+CNACT=0,1\r", strlen("+CNACT=0,1\r"), 0);
		appStatus = waitResponse(1000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
		if(appStatus != 1){
			/* Activate network bearer Failed! */
		}
	}

    // Before connecting, you need to confirm that the time has been synchronized.
	sendAT("+CCLK?\r", strlen("+CCLK?\r"), 0);
	appStatus = waitResponse(30000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);

	// If it is already connected, disconnect it first
	sendAT("+SMDISC\r", strlen("+SMDISC\r"), 0);
	appStatus = waitResponse(30000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);

	connect_mqtt();

	for(;;){
		if(!isConnect()){
			/* Try again the connection with the MQTT server */
			connect_mqtt();
		} else {
			/* Here will go the desired application, in the use-case it was a moisture sensor */
			snprintf(buffer, 1024, "+SMPUB=\"%s/feeds/%s\",%d,1,1", username, topic, 5);
			sendAT(buffer, strlen(buffer), 0);
			if(1 == waitResponse(30000, NULL_PTR, ">", NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
				sendAT("5\r", strlen("5\r"), 0);
				if (1 == waitResponse(1000,  NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
					/* Successful to publish */
				} else {
					/* Failed to publish */
				}
			}
		}
	}
}

