/*
 * device_drivers.h
 *
 *  Created on: Oct 16, 2025
 *      Author: nxf63523
 */

#ifndef DEVICE_DRIVERS_H_
#define DEVICE_DRIVERS_H_

#include "Lpuart_Uart_Ip.h"
#include "Lpuart_Uart_Ip_Irq.h"
#include "Clock_Ip.h"
#include "IntCtrl_Ip.h"
#include "Siul2_Port_Ip.h"
#include "string.h"
#include "Stm_Ip.h"
#include "Siul2_Dio_Ip.h"

/**********************************************************/
/**********			     SIUL					***********/
/**********************************************************/

#define SIUL2_INSTANCE				(0U)

/**********************************************************/
/**********			     UART					***********/
/**********************************************************/
/* Length of the message to be received from the console */
#define MAX_LEN  					(64U)

/* UART to be used with the AT SIM7080G */
#define AT_UART_INSTANCE  			(6U)

/* UART to send the log to the main computer */
#define DEBUG_UART_INSTANCE  		(6U)

#define MAX_TIMEOUT					(0xFFFFFFFFU)

/**********************************************************/
/**********			      STM					***********/
/**********************************************************/
/* STM instance used - 12 */
#define STM_INST_0 					(0U)

/* STM Channel used - 0 */
#define CH_0 						(0U)

#define STM_START_CNT_VALUE			(0U)

/* System Clock Frequency used */
#define STM_CLOCK_FREQ 				(48000000U)

/* STM0 Clock Prescaler */
#define STM_CLK_PRESCALER			(48U)

#define MODEM_MUX_COUNT 			(12U)

typedef enum {
  SIM_ERROR            = 0,
  SIM_READY            = 1,
  SIM_LOCKED           = 2,
  SIM_ANTITHEFT_LOCKED = 3,
} SimStatus;

typedef union {
	uint8_t bytes[4];  // IPv4 address
	uint32_t dword;
} IP_address;

typedef enum {
  REG_NO_RESULT    = -1,
  REG_UNREGISTERED = 0,
  REG_SEARCHING    = 2,
  REG_DENIED       = 3,
  REG_OK_HOME      = 1,
  REG_OK_ROAMING   = 5,
  REG_UNKNOWN      = 4,
}SIM70xxRegStatus;

/**********************************************************/
/**********		    Function definition	      	***********/
/**********************************************************/
/* Function prototype for the initialization of the S32K3 drivers */
void init_device_drivers(void);
void streamWrite(uint8_t* pAT, uint8_t* pCmd, uint8_t *pAT_NL);
bool TestAT(uint32_t timeout_ms);
SimStatus getSimStatus(uint32_t timeout_ms);
bool setNetworkMode(char* pMode);
int getNetworkMode(void);
bool setPreferredMode(char* pMode);
int getPreferredMode(void);
SIM70xxRegStatus getRegistrationStatus(void);
bool isGprsConnected(void);
IP_address IPAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);
IP_address localIP(void);
void getLocalIP(uint8_t* localIP);
IP_address IpFromString(uint8_t* IPstring);

/** @brief 	Implementation of StreamSkipUntil, the idea is to allow to detec specific characters in the reception
 * 	@details
 * 			Iteration over the reception array to detect a specific character and continue the operation indicating to the
 * 			application that either the character was found or there was a timeout.
 */
uint8_t streamSkipUntil
(
	uint8_t expectedChar,
	uint32_t timeout_ms
);
#endif /* DEVICE_DRIVERS_H_ */
