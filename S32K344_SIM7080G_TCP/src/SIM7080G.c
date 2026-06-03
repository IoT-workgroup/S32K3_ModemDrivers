/*
 * AT.c
 *
 *  Created on: 2 oct. 2025
 *      Author: jesus
 */

#include "SIM7080G.h"

#include <string.h>
#include <stdlib.h>
#include "Lpuart_Uart_Ip.h"
#include "Stm_Ip.h"

#include "S32K3_Drivers.h"

uint32 remainingBytes = 0;
uint8 Rx_Buffer[MAX_LEN] = {0};


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

void InitTimeoutTimer(uint32_t milliseconds){

	/* Calls for the implementation of the initialization of the timeout timer on the specific MCU */
	InitTimeoutTimerImpl(milliseconds);

}

uint32_t GetCurrentTime(void){
	uint32_t milliseconds = 0;

	milliseconds = GetCurrentTimeImpl();

	return milliseconds;
}

void DeinitTimeoutTimer(void){
	DeinitTimeoutTimerImpl();
}



uint8_t send_at(char* s_buf1, char* s_buf2, uint8_t com1length, uint8_t com2length, uint8_t command_num, uint16_t delay_ms)
{
	uint8_t command_status = 1;
	uint32 T_timeout = 0x0000FFFF;
	Lpuart_Uart_Ip_StatusType lpuartStatus = LPUART_UART_IP_STATUS_ERROR;
	uint8_t error_msg[] = "\r\nNo response obtained\r\n";
	uint8_t *p_ok = NULL;

	/* Enable the UART reception with Async interface */
	do
	{
		lpuartStatus = Lpuart_Uart_Ip_AsyncReceive(DEBUG_UART_INSTANCE, Rx_Buffer, MAX_LEN);
	}while(LPUART_UART_IP_STATUS_SUCCESS != lpuartStatus);

	/* Verify the type of AT command to send */
	if(1 == command_num){
		/* Send only 1 AT command */

		lpuartStatus = Lpuart_Uart_Ip_SyncSend(DEBUG_UART_INSTANCE,(uint8_t *)s_buf1, com1length, AT_TRANSMIT_TIMEOUT);
		if (LPUART_UART_IP_STATUS_SUCCESS != lpuartStatus)
		{
			return command_status;
		}

		DelayImpl(1000);

	}else if(2 ==command_num){
		/* Send 2 AT commands */
		Lpuart_Uart_Ip_SyncSend(DEBUG_UART_INSTANCE,(uint8_t *)s_buf1, com1length, AT_TRANSMIT_TIMEOUT);
		DelayImpl(1000);

		Lpuart_Uart_Ip_SyncSend(DEBUG_UART_INSTANCE,(uint8_t *)s_buf2, com2length, AT_TRANSMIT_TIMEOUT);
		DelayImpl(2100);

	}

	/* TODO: Analizar la similitud con el codigo de ST */
	// Verify that the UART message was received and that it has finished
	do
	{
		lpuartStatus = Lpuart_Uart_Ip_GetReceiveStatus(DEBUG_UART_INSTANCE, &remainingBytes);
	} while (LPUART_UART_IP_STATUS_BUSY == lpuartStatus && 0 < T_timeout--);

	// Analyze the received buffer and make sure that it contains an OK response.
	p_ok = (uint8_t*)strstr((char *)Rx_Buffer,"OK");

	if(NULL != p_ok){
		// Print out the buffer received from the modem to the debugging interface
		Lpuart_Uart_Ip_SyncSend(DEBUG_UART_INSTANCE,(uint8_t *)Rx_Buffer,(uint32)(p_ok-Rx_Buffer), AT_TRANSMIT_TIMEOUT);
		Lpuart_Uart_Ip_AbortReceivingData(DEBUG_UART_INSTANCE);

	}else{
		Lpuart_Uart_Ip_SyncSend(DEBUG_UART_INSTANCE,(uint8_t *)error_msg, sizeof(error_msg)-1, AT_TRANSMIT_TIMEOUT);
		Lpuart_Uart_Ip_AbortReceivingData(DEBUG_UART_INSTANCE);

		return command_status;
	}

	command_status = 0;
	DelayImpl(delay_ms);

	return command_status;
}

uint8_t sendAT(char* s_buf1, uint8_t com1length, uint16_t delay_ms)
{
	uint8_t status = 1;

	status = sendATImpl(s_buf1, com1length, delay_ms);

	return status;
}


void send_AT(uint8_t* pCmd){
	uint8_t AT[] = "AT";
	uint8_t AT_NL[] = "\r\n";

	/* Send a stream through serial with "AT", the command and the New Line */
	streamWrite(AT, pCmd, AT_NL);
}

uint8_t waitResponse(
		uint32_t timeout_ms,
		uint8_t *data,
		uint8_t *r1,
		uint8_t *r2,
		uint8_t *r3,
		uint8_t *r4,
		uint8_t *r5,
		uint8_t *r6,
		uint8_t *r7)
{
	uint8_t response = 0;

	response = waitResponseImpl(timeout_ms, data, r1, r2, r3, r4, r5, r6, r7);

	return response;
}

uint8_t verifyResponse(char * pExpectedAnswers, uint8_t TotalExpAns){
	char lastChar = 0;
	Lpuart_Uart_Ip_StatusType lpuartStatus = LPUART_UART_IP_STATUS_ERROR;
	uint8_t i = 0;
	uint8_t currentIndex = 0;
	uint8_t match = 0;

	memset(Rx_Buffer, 0, MAX_LEN);

	/* Enable the UART reception with Async interface */
	do
	{
		lpuartStatus = Lpuart_Uart_Ip_AsyncReceive(AT_UART_INSTANCE, Rx_Buffer, MAX_LEN);
	}while(LPUART_UART_IP_STATUS_SUCCESS != lpuartStatus);

	// Verify that the UART message was received and that it has finished
	do
	{
		lpuartStatus = Lpuart_Uart_Ip_GetReceiveStatus(AT_UART_INSTANCE, &remainingBytes);
		currentIndex = MAX_LEN - remainingBytes - 1;
		if(currentIndex > 1){
			lastChar = Rx_Buffer[currentIndex];
		}
	} while ((LPUART_UART_IP_STATUS_BUSY == lpuartStatus) && (lastChar != '\n'));

	/* Finish the UART communication as the last Line Feed \n was received */
	Lpuart_Uart_Ip_AbortReceivingData(AT_UART_INSTANCE);

	if (TotalExpAns == 0){
		if(NULL_PTR != strstr((char*)Rx_Buffer, "OK")){
			match = 1;
		} else if(NULL_PTR != strstr((char*)Rx_Buffer, "ERROR")){
			match = 2;
		}
	}
	if (TotalExpAns == 1){
		if(NULL_PTR != strstr((char*)Rx_Buffer, pExpectedAnswers)){
			match = 1;
		}else if(NULL_PTR != strstr((char*)Rx_Buffer, "ERROR")){
			match = 2;
		}
	} else if (TotalExpAns > 1){
		/* Iterate over all the expected answers */
		for(i = 0; i < TotalExpAns; i++){
			/* Compare the content of the element against the received response */
			if(NULL_PTR != strstr((const char*)Rx_Buffer, (const char *)pExpectedAnswers[i])){
				match = i+1;
			}
		}
	}

	return match;
}

int GetIntResponse(void){
	int value = 0;
	uint8_t i = 0;
	bool numberDetected = false;
	uint8_t receivedChar = 0;
	uint32_t timeout_ms = 1000;
	Lpuart_Uart_Ip_StatusType ReceiveStatus = LPUART_UART_IP_STATUS_ERROR;

	/* Initialize timeout timer to reach expected timeout value */
	InitTimeoutTimer(timeout_ms);

	/* Clear with 0 the reception buffer */
	memset((uint8_t*)Rx_Buffer, 0, MAX_LEN);

	/* Receive the stream to convert later */
	ReceiveStatus = Lpuart_Uart_Ip_SyncReceive(AT_UART_INSTANCE,(uint8 *)&Rx_Buffer[0], MAX_LEN, 10000);

	/* Main process of the function, verify received character and store into the buffer if it is a number */
	do{
		ReceiveStatus = Lpuart_Uart_Ip_SyncReceive(AT_UART_INSTANCE,(uint8 *)&receivedChar, 1, 10000);
		if((receivedChar >= 48) && (receivedChar <=57) && (LPUART_UART_IP_STATUS_SUCCESS == ReceiveStatus)){
			numberDetected = true;
			Rx_Buffer[i++] = receivedChar;
		} else {
			numberDetected = false;
		}
	}while((GetCurrentTime() < timeout_ms) && numberDetected);

	/* De-Initialize timer as operation has finished */
	DeinitTimeoutTimer();

	/* If i is greater than 0, it indicates that there was at least a number received */
	if(i != 0){
		/* If possible, transform the received buffer to the corresponding integer value */
		value = atoi((const char*)Rx_Buffer);
	} else {
		/* Saturate return value to indicate error occurred */
		value = 0xFFFF;
	}

	return value;
}

int GetIntBefore(char lastChar){
	volatile int value = 0;
	char buffer[7] = {0};
	char character = 0;
	uint8_t i = 0;
	uint8_t numberStart = 0;
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



//// TODO(vshymanskyy): Optimize this!
//int8_t waitResponseImpl(uint32_t timeout_ms, String& data,
//                        GsmConstStr r1 = GFP(GSM_OK),
//                        GsmConstStr r2 = GFP(GSM_ERROR),
//                        GsmConstStr r3 = nullptr, GsmConstStr r4 = nullptr,
//                        GsmConstStr r5 = nullptr, GsmConstStr r6 = nullptr,
//                        GsmConstStr r7 = nullptr) {
//  data.reserve(64);
//
//#ifdef TINY_GSM_DEBUG_DEEP
//  DBG(GF("r1 <"), r1 ? r1 : GF("NULL"), GF("> r2 <"), r2 ? r2 : GF("NULL"),
//      GF("> r3 <"), r3 ? r3 : GF("NULL"), GF("> r4 <"), r4 ? r4 : GF("NULL"),
//      GF("> r5 <"), r5 ? r5 : GF("NULL"), GF("> r6 <"), r6 ? r6 : GF("NULL"),
//      GF("> r7 <"), r7 ? r7 : GF("NULL"), '>');
//#endif
//  uint8_t  index       = 0;
//  uint32_t startMillis = millis();
//  do {
//    TINY_GSM_YIELD();
//    while (thisModem().stream.available() > 0) {
//      TINY_GSM_YIELD();
//      int8_t a = thisModem().stream.read();
//      if (a <= 0) continue;  // Skip 0x00 bytes, just in case
//      data += static_cast<char>(a);
//      if (r1 && data.endsWith(r1)) {
//        index = 1;
//        goto finish;
//      } else if (r2 && data.endsWith(r2)) {
//        index = 2;
//        goto finish;
//      } else if (r3 && data.endsWith(r3)) {
//        index = 3;
//        goto finish;
//      } else if (r4 && data.endsWith(r4)) {
//        index = 4;
//        goto finish;
//      } else if (r5 && data.endsWith(r5)) {
//        index = 5;
//        goto finish;
//      } else if (r6 && data.endsWith(r6)) {
//        index = 6;
//        goto finish;
//      } else if (r7 && data.endsWith(r7)) {
//        index = 7;
//        goto finish;
//      }
//#if defined TINY_GSM_DEBUG
//      else if (data.endsWith(GFP(GSM_VERBOSE)) ||
//               data.endsWith(GFP(GSM_VERBOSE_2))) {
//        // check how long the new line is
//        // should be either 1 ('\r' or '\n') or 2 ("\r\n"))
//        int len_atnl = strnlen(AT_NL, 3);
//        // Read out the verbose message, until the last character of the new
//        // line
//        data += thisModem().stream.readStringUntil(AT_NL[len_atnl]);
//#ifdef TINY_GSM_DEBUG_DEEP
//        data.trim();
//        DBG(GF("Verbose details <<<"), data, GF(">>>"));
//#endif
//        data = "";
//        goto finish;
//      }
//#endif
//      else if (thisModem().handleURCs(data)) {
//        data = "";
//      }
//    }
//  } while (millis() - startMillis < timeout_ms);
//finish:
//#ifdef TINY_GSM_DEBUG_DEEP
//  data.replace("\r", "←");
//  data.replace("\n", "↓");
//#endif
//  if (!index) {
//    data.trim();
//    if (data.length()) { DBG("### Unhandled:", data); }
//    data = "";
//  } else {
//#ifdef TINY_GSM_DEBUG_DEEP
//    DBG('<', index, '>', data);
//#endif
//  }
//  return index;
//}
