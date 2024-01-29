/*
	ODOTS.ino
	
	*Copyright 2019: Lawrence Jones
	* Version 1.0
	* This sketch is licensed under Creative Commons Attribution-ShareAlike 3.0
	* Unported License.
	
	Version	Date	Description
	v1.0	09/2019	Initial Release for Proof of concept.
*/


#include "ODOTS.h"


extern time_t PunchTime;
uint16_t WriteBlock = 0;
void setup()
{
	//init RFID
	InitRFID();
	//init Serial
	SerialInit();
	//Init Clock
	InitRTC();
	RTCEnableAlarm();
	//init Buzzer
	BuzzerInit();
	//Update operation Parameters from EEPROM (Disabled for now)
	UpdateStatusFromEEPROM();
	
}
void loop()
{
  //Serial.println("Hi");cc
	
	
	//Check Serial Input buffer
	if (CheckSerialForContents())
	{
		SerialHandleRequest();
	}
	//Check Clock Interrupt Flag
	if (RTCAlarm())//in RTC_drv.h
	{
		UpdateRTCReference();
		BuzzerBlip();
   Serial.println(">>Debug, RTCChecked");
	}
	//Check RFID read
	WakeRFIDModule();
	if (CheckIfNEWRFIDCardPresent())
	{
		//Select Card
		SelectCard();
		if(CheckIsUnitConfiguredForDownload())
		{
			//Perform download unit tasks
			if(  DumpCardToSerial())
			{
				//This structure is about trying to ensure the user knows if the task performed successfully
				HaltCard();
				BuzzerBleep();
			}
			else
			{
				HaltCard();
				ErrorBuzz();
			}
		}
		else if(CheckIsUnitConfiguredForClear())
		{
				//Bros - we are a clear unit today
			if(ResetCard())
			{
				HaltCard();
				BuzzerBleep();
			}
			else
			{
				HaltCard();
				ErrorBuzz();
			}
		}
     else
     {
      PunchTime = CalculateTimeStamp();
      WriteBlock = ParseCardIDandInfo();
      if((WriteBlock)!=0xffff)//check if the card is full (WHY has it punched 150 controls without clearing is probably a better question!)
      {
		if(WriteVisitStamp(&PunchTime, WriteBlock))
			{
				HaltCard();
				BuzzerBleep();
			}
			else
			{
				HaltCard();
				ErrorBuzz();
			}
      }
      else
      {
        HaltCard();//Card full
        ErrorBuzz();
      }
     }
	}
	SendRFIDModuleToSleep();
	SendUnitToSleep();
}
	
