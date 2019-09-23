/*
	ODOTS.h The glue header holding clock, rfid and data management together...
	
	*Copyright 2019: Lawrence Jones
	* Version 1.0
	* This sketch is licensed under Creative Commons Attribution-ShareAlike 3.0
	* Unported License.
	
	Version	Date	Description
	v1.0	09/2019	Initial Release for Proof of concept.
*/

#ifndef SRC_ODOTS_H_
#define SRC_ODOTS_H_


#include "ODOTSRTC.h"
#include "ODOTSSerialUtils.h"
#include "BuzzerUtils.h"
#include "DS1337.h"
#include "Config.h"

struct time_t PunchTime;

bool CheckIsUnitConfiguredForDownload();
bool CheckIsUnitConfiguredForClear();


void InitRFID();
bool CheckIfNEWRFIDCardPresent();
bool SelectCard();


uint16_t ParseCardIDandInfo();
void HaltCard();

bool ResetCard();


bool WriteVisitStamp(struct time_t* VisitTime, uint16_t WriteBlock);

void UpdateStatusFromEEPROM();
bool DumpCardToSerial();

//Power saving Functions
void SendRFIDModuleToSleep();
void WakeRFIDModule();
void SendUnitToSleep();
#endif