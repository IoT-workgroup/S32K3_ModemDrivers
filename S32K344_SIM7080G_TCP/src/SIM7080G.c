#include "sim7080G.h"
#include "AT.h"
#include "string.h"
#include "device_drivers.h"

uint32 test_remainingBytes = 0;
uint8 test_Rx_Buffer[MAX_LEN] = {0};

extern char Resp_Ready[];
extern char Resp_Pin[];
extern char Resp_Puk[];
extern char Resp_Insrt[];
extern char Resp_NotReady[];

void power_down(void)
{
	Siul2_Dio_Ip_SetGPDO(SIUL2_INSTANCE, 30);
	delay_at(2000);
	Siul2_Dio_Ip_ClearGPDO(SIUL2_INSTANCE, 30);
	delay_at(2000);
}

void power_sequence(void)
{
	Siul2_Dio_Ip_SetGPDO(SIUL2_INSTANCE, 30);
	delay_at(2000);
	Siul2_Dio_Ip_ClearGPDO(SIUL2_INSTANCE, 30);
	delay_at(2000);
}

void test_functionality(uint8_t *msg_buffer, uint8_t msg_length ){
	uint32 T_timeout = 0xFFFFFF;
	Lpuart_Uart_Ip_StatusType lpuartStatus = LPUART_UART_IP_STATUS_ERROR;
	uint8_t delay_msg[] = "End of delay\r\nWaiting to receive 50 characters\r\n";
	uint8_t end_msg[] = "\r\nTest successful\n\r";
	volatile uint8_t response = 0;
	volatile int value = 0;
	char * pExpectedResponse[5]= {Resp_Ready, Resp_Pin, Resp_Puk, Resp_Insrt, Resp_NotReady};

	/* TODO: Test out UART RX */
	/* Enable the UART reception with Async interface */
	lpuartStatus = Lpuart_Uart_Ip_AsyncReceive(DEBUG_UART_INSTANCE, test_Rx_Buffer, MAX_LEN);
	while (LPUART_UART_IP_STATUS_SUCCESS != lpuartStatus)
	{

	}

	/* Verify the UART sent */
	lpuartStatus = Lpuart_Uart_Ip_SyncSend(DEBUG_UART_INSTANCE,(uint8_t *)msg_buffer, msg_length, AT_TRANSMIT_TIMEOUT);
	while (LPUART_UART_IP_STATUS_SUCCESS != lpuartStatus)
	{
	}

	/* Delay in milliseconds */
	delay_at(1000);

	/* Verify the UART sent */
	lpuartStatus = Lpuart_Uart_Ip_SyncSend(DEBUG_UART_INSTANCE,(uint8_t *)delay_msg, sizeof(delay_msg)-1, AT_TRANSMIT_TIMEOUT);
	while (LPUART_UART_IP_STATUS_SUCCESS != lpuartStatus)
	{
	}

	// Verify that the UART message was received and that it has finished
	do
	{
		lpuartStatus = Lpuart_Uart_Ip_GetReceiveStatus(DEBUG_UART_INSTANCE, &test_remainingBytes);
	} while (LPUART_UART_IP_STATUS_BUSY == lpuartStatus && 0 < T_timeout--);

	/* Verify the UART sent */
	lpuartStatus = Lpuart_Uart_Ip_SyncSend(DEBUG_UART_INSTANCE,(uint8_t *)end_msg, sizeof(end_msg)-1, AT_TRANSMIT_TIMEOUT);
	while (LPUART_UART_IP_STATUS_SUCCESS != lpuartStatus)
	{
	}

	/* Probar respuesta */
	response = verifyResponse((char*)NULL_PTR, 0);

	response = verifyResponse(pExpectedResponse, 5);

	response = verifyResponse("+PRUEBA:", 1);

	value = GetIntResponse();

	while(1){

	}
}

void tcp_test(void)
{
	power_sequence();
	Lpuart_Uart_Ip_SyncSend(DEBUG_UART_INSTANCE,(uint8_t *)"\r\nwait 15 seconds for signal\r\n\r\n",sizeof("\r\nwait 15 seconds for signal\r\n\r\n")-1, 0xFFFF);
	delay_at(15000);

//	HAL_UART_Transmit(&huart2,(uint8_t *)CSQ,sizeof(CSQ)-1,0xff);
	send_at(CSQ, NULL, sizeof(CSQ)-1, 0, 1, 1000);

//	HAL_UART_Transmit(&huart2,(uint8_t *)QUERY_CPSI,sizeof(QUERY_CPSI)-1,0xff);
	send_at(QUERY_CPSI, NULL, sizeof(QUERY_CPSI)-1, 0, 1, 1000);

//	HAL_UART_Transmit(&huart2,(uint8_t *)EN_CNACT,sizeof(EN_CNACT)-1,0xff);
	send_at(EN_CNACT, NULL, sizeof(EN_CNACT)-1, 0, 1, 1000);

//	HAL_UART_Transmit(&huart2,(uint8_t *)CACID,sizeof(CACID)-1,0xff);
	send_at(CACID, NULL, sizeof(CACID)-1, 0, 1, 1000);

//	HAL_UART_Transmit(&huart2,(uint8_t *)CAOPEN,sizeof(CAOPEN)-1,0xff);
	send_at(CAOPEN, NULL, sizeof(CAOPEN)-1, 0, 1, 1000);

//	HAL_UART_Transmit(&huart2,(uint8_t *)CASEND,sizeof(CASEND)-1,0xff);
//	HAL_UART_Transmit(&huart2,(uint8_t *)MESSAGES,sizeof(MESSAGES)-1,0xff);
//	HAL_UART_Transmit(&huart2,(uint8_t *)"\r\n",2,0xff);
	send_at(CASEND, MESSAGES, sizeof(CASEND)-1, sizeof(MESSAGES)-1, 2, 1000);

//	HAL_UART_Transmit(&huart2,(uint8_t *)CACLOSE,sizeof(CACLOSE)-1,0xff);
	send_at(CACLOSE, NULL, sizeof(CACLOSE)-1, 0, 1, 1000);

//	HAL_UART_Transmit(&huart2,(uint8_t *)DIS_CNCAT,sizeof(DIS_CNCAT)-1,0xff);
	send_at(DIS_CNCAT, NULL, sizeof(DIS_CNCAT)-1, 0, 1, 1000);
	power_sequence();
}

bool isConnect(void){
	sendAT("+SMSTATE?", sizeof("+SMSTATE?"), 0);
	if(1 == waitResponseImpl(0, NULL_PTR, "+SMSTATE: ", NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
		if(1 == waitResponseImpl(0, NULL_PTR, "\r", NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
			return GetIntResponse();
		}
	}

	return false;
}

uint8_t connectMQTT(void){
	uint8_t status = 0;

	sendAT("+SMCONF=\"URL\",\"io.adafruit.com\",8883", sizeof("+SMCONF=\"URL\",\"io.adafruit.com\",8883"), 0);
	if (1 != waitResponseImpl(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
		status = 1;
		return status;
	}

	sendAT("+SMCONF=\"USERNAME\",\"ie714410\"", sizeof("+SMCONF=\"USERNAME\",\"ie714410"), 0);
	if (1 != waitResponseImpl(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
		status = 1;
		return status;
	}

	sendAT("+SMCONF=\"PASSWORD\",\"aio_aLwI06Dmz09Q9D1IPzy8ioq7RgM8\"", sizeof("+SMCONF=\"PASSWORD\",\"aio_aLwI06Dmz09Q9D1IPzy8ioq7RgM8\""), 0);
	if (1 != waitResponseImpl(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
		status = 1;
		return status;
	}

	sendAT("+SMCONF=\"CLIENTID\",\"ESP32\"", sizeof("+SMCONF=\"CLIENTID\",\"ESP32\""), 0);
	if (1 != waitResponseImpl(0, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR)){
		status = 1;
		return status;
	}

	do{
		sendAT("+SMCONN", sizeof("+SMCONN"), 0);
		/* Attempting to connect */
	} while(1 != waitResponseImpl(3000, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR, NULL_PTR));

	return status;
}

