
#include "Application.h"
#include "SIM7080G.h"
#include <string.h>
#include <stdio.h>

/* Adafruit username */
#define IO_USERNAME  "serviti"

/*  Adafruit IO Key */
#define IO_KEY       "aio_kARg3915AhsF7HFg3igXGrVFfsxE"

#define IO_TOPIC	"Use-Case"

const char Automatic_Network[] = "2";
const char Modem_CATM[] = "1";
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

    sendAT("+SMSTATE?\r\n", sizeof("+SMSTATE?\r\n"), 0);
    appStatus = waitResponse(3000, "+SMSTATE: ", NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR);
    if(appStatus == 1){
    	return GetIntResponse();
    }


    return false;
}

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
	/* TODO: modify emnify information for personal implementation */
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
		/* Enable RF failed */
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

	for(;;){
		if(!isConnect()){
			/* Try again the connection with the MQTT server */
			connect_mqtt();
		} else {
			/* Here will go the desired application, in the use-case it was a moisture sensor */
			snprintf(buffer, 1024, "+SMPUB=\"%s/feeds/%s\",%d,1,1", username, topic, 5);
			sendAT(buffer, sizeof(buffer), 0);
			if(1 == waitResponse(30000, ">", NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){

			}
		}
	}
}

