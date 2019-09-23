/*
	Config.h
	
	*Copyright 2019: Lawrence Jones
	* Version 1.0

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
	
	Version	Date	Description
	v1.0	09/2019	Initial Release for Proof of concept.
*/

#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

/*
	Version Configs
*/
#define HARDWAREVERSION	0
#define FIRMWAREVERSION	0

/*
 * Wiring Config Definitions used in declaration of MIFARE class
 *  IRQ: Pin 8
 SS: pin 10     SLAVE SELECT
 MOSI: pin 11   //irrelevant on most arduino devices (static assignment anyway!)
 MISO: pin 12   //irrelevant on most arduino devices (static assignment anyway!)
 SCK: pin 13    SERIAL CLOCK  //irrelevant on most arduino devices(static assignment anyway!)
 IntSel: pin 9
 */
#define IRQPIN		8
#define RSTPIN		9
#define	SSPIN		10
#define	MOSIPIN		11
#define	MISOPIN		12
#define	SCKPIN		13
/*
	Clock Configs
	Leonardo: 7
	UNO: 2
*/
#define	RTCALARMPIN	2

/*
 * Mifare Values
 */
#define MF_KEY_SIZE_CONFIG	6
#define ACCESSKEY	0xFFFFFFFFFFFF
/*
	ODOTS Card Data Storage Block Locations
*/
#define DOT_INFO_BLOCK		1
#define DOT_INFO_TRAILER	3
#define	DOT_INFO_SECTOR		0
#define DOT_FIRST_VISIT_BLOCK 4

/*
	EEPROM Persistent Settings Register Locations
*/
#define	DEVICEID1ADDR	0x01
#define	DEVICEID2ADDR	0x02
#define	FLAGSADDR		0x03
#define DOWNLOADSTATUSMASK	0x01
#define CLEARSTATUSMASK		0x02


/*
 * Buzzer Configs  
 */
#define BUZZERPIN	5
/*
	LED pins
	UNO: 6,7
	Leonardo: not yet defined
*/
#define LEDPIN1	6
#define LEDPIN2	7
/*
 *	Serial (UART) Configs
*/
#define UARTBAUD	9600
#define SERIALTIMEOUT	1200

/*
	ADC battery reading configs
	for UNO:
#define ADCREF INTERNAL
#define ADCPIN A1
*/  
#define ADCREF INTERNAL
#define ADCSWITCHPIN	9
#define ADCPIN A1
//uint8_t ODOTS_DeviceID[2]={0};Created weird linker errors now defined in ODOTS RTC.h
extern uint8_t ODOTS_DeviceID[2];
extern bool IsUnitConfiguredForDownload;
extern bool IsUnitConfiguredForClear;


#endif /* SRC_CONFIG_H_ */