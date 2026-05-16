/*
 * AT.c
 *
 *  Created on: 2 oct. 2025
 *      Author: jesus
 */

#include <string.h>
#include <stdlib.h>
#include "AT.h"
#include "Lpuart_Uart_Ip.h"
#include "Stm_Ip.h"
#include "device_drivers.h"

uint32 remainingBytes = 0;
uint8 Rx_Buffer[MAX_LEN] = {0};

static const char GSM_OK[] = "OK\r\n";
static const char GSM_ERROR[] = "ERROR\r\n";

static bool handleURCs(const char *str);

static bool endsWith(const char *str, const char *suffix);

void InitTimeoutTimer(uint32_t milliseconds){
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

uint32_t GetCurrentTime(void){
	uint32_t milliseconds = 0;

	milliseconds = Stm_Ip_GetCounterValue(STM_INST_0);
	milliseconds = milliseconds/(STM_CLOCK_FREQ/(STM_CLK_PRESCALER*1000));

	return milliseconds;
}

boolean GetTimeoutStatus(void){
	return Stm_Ip_GetInterruptStatusFlag(STM_INST_0, CH_0);
}

void DeinitTimeoutTimer(void){
	/* Disable the compare channel as the comparison is no longer needed */
	Stm_Ip_DisableChannel(STM_INST_0, CH_0);

	/* Stop the timer */
	Stm_Ip_StopTimer(STM_INST_0);
}

void delay_at(uint32_t milliseconds){
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

at_status send_at(char* s_buf1, char* s_buf2, uint8_t com1length, uint8_t com2length, uint8_t command_num, uint16_t delay_ms)
{
	at_status command_status = AT_ERROR;
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

		delay_at(1000);

	}else if(2 ==command_num){
		/* Send 2 AT commands */
		Lpuart_Uart_Ip_SyncSend(DEBUG_UART_INSTANCE,(uint8_t *)s_buf1, com1length, AT_TRANSMIT_TIMEOUT);
		delay_at(1000);

		Lpuart_Uart_Ip_SyncSend(DEBUG_UART_INSTANCE,(uint8_t *)s_buf2, com2length, AT_TRANSMIT_TIMEOUT);
		delay_at(2100);

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

	command_status = AT_RESP_OK;
	delay_at(delay_ms);

	return command_status;

}

at_status sendAT(char* s_buf1, uint8_t com1length, uint16_t delay_ms)
{
	at_status command_status = AT_ERROR;
	uint32 T_timeout = 0x0000FFFF;
	Lpuart_Uart_Ip_StatusType lpuartStatus = LPUART_UART_IP_STATUS_ERROR;

	lpuartStatus = Lpuart_Uart_Ip_SyncSend(AT_UART_INSTANCE,(uint8_t *)s_buf1, com1length, T_timeout);
	if (LPUART_UART_IP_STATUS_SUCCESS != lpuartStatus)
	{
		return command_status;
	}

	if(delay_ms > 0){
		delay_at(delay_ms);
	}

	command_status = AT_SENT;

	return command_status;
}


void send_AT(uint8_t* pCmd){
	uint8_t AT[] = "AT";
	uint8_t AT_NL[] = "\r\n";

	/* Send a stream through serial with "AT", the command and the New Line */
	streamWrite(AT, pCmd, AT_NL);
}

uint8_t waitResponseImpl(
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
	uint8_t index = 0;
	uint8_t response = 0;
	uint32_t startMillis = 0;
	uint8_t character = 0;

	if(timeout_ms == 0){
		timeout_ms = 1000;
	}
	/* Verify for default values on r1 and r2 */
	if(NULL_PTR == data){
		data = Rx_Buffer;
	}
	if(NULL_PTR == r1){
		r1 = GSM_OK;
	}
	if(NULL_PTR == r2){
		r1 = GSM_ERROR;
	}

	InitTimeoutTimer(timeout_ms);

	do{
		Lpuart_Uart_Ip_SyncReceive(AT_UART_INSTANCE,(uint8 *)&character, 1, 10000);
		if (character <= 0){
			continue;
		}
		*(data+index) = (uint8_t)character;
		if(r1 && endsWith((const char*)data, (const char*)r1)){
			response = 1;
			break;
		} else if (r2 && endsWith((const char*)data, (const char*)r2)){
			response = 2;
			break;
		} else if (r3 && endsWith((const char*)data, (const char*)r3)){
			response = 3;
			break;
		} else if (r4 && endsWith((const char*)data, (const char*)r4)){
			response = 4;
			break;
		} else if (r5 && endsWith((const char*)data, (const char*)r5)){
			response = 5;
			break;
		} else if (r6 && endsWith((const char*)data, (const char*)r6)){
			response = 6;
			break;
		} else if (r7 && endsWith((const char*)data, (const char*)r7)){
			response = 7;
			break;
		} else if (handleURCs((const char*)data)){

		}

	}while(GetCurrentTime() < timeout_ms);

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

static bool handleURCs(const char *str){
	bool result;

	if(endsWith(str, "something")){
		result = FALSE;
	}

	return result;

}

static bool endsWith(const char *str, const char *suffix){
	if (!str || !suffix)
		return 0;
	size_t lenstr = strlen(str);
	size_t lensuffix = strlen(suffix);
	if (lensuffix >  lenstr)
		return 0;
	return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
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
