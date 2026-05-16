/*
 * SIM7080.h
 *
 *  Created on: 1 oct. 2025
 *      Author: jesus
 */

#ifndef SIM7080G_H_
#define SIM7080G_H_

#include "Mcal.h"

#define CSQ							"AT+CSQ\r\n"
#define QUERY_CPSI					"AT+CPSI?\r\n"
#define EN_CNACT					"AT+CNACT=0,1\r\n"
#define CACID						"AT+CACID=0\r\n"
#define CAOPEN						"AT+CAOPEN=0,0,\"TCP\",\"116.30.219.149\",5001\r\n"
#define CASEND						"AT+CASEND=0,17,2000\r\n"
#define MESSAGES					"www.waveshare.com"
#define CACLOSE						"AT+CLOSE=0\r\n"
#define DIS_CNCAT					"AT+CNACT=0,0\r\n"

void power_dowm(void);
void power_up(void);
void test_functionality(uint8_t *msg_buffer, uint8_t msg_length );
void tcp_test(void);
bool isConnect(void);

#endif /* SIM7080G_H_ */
