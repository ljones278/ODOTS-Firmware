/*
	ODOTS.cpp
	
	*Copyright 2019: Lawrence Jones
	* Version 1.0
	* This sketch is licensed under Creative Commons Attribution-ShareAlike 3.0
	* Unported License.
	
	Version	Date	Description
	v1.0	09/2019	Initial Release for Proof of concept.
*/
#include <Arduino.h>
#include "Config.h"
#include "MFRC522.h"
#include "ODOTSRTC.h"
#include "LowPower.h"
#include <EEPROM.h>
//Private functions...
uint8_t CheckIfTrailerClash(uint8_t Block);


MFRC522 mfrc522(SSPIN, RSTPIN);  // Create MFRC522 instance

MFRC522::MIFARE_Key key;

/**
*	Simple glue function that passes value of local variable, IsUnitConfiguredForDownload, to main program 
@return value of IsUnitConfiguredForDownload, bool
*/
bool CheckIsUnitConfiguredForDownload()
{
	return IsUnitConfiguredForDownload;
}
/**
*	Simple glue function that passes value of local variable, IsUnitConfiguredForClear, to main program 
@return value of IsUnitConfiguredForClear, bool
*/
bool CheckIsUnitConfiguredForClear()
{
	return IsUnitConfiguredForClear;
}
/**
*	Initialises RFID module and SPI interface including access key, currently set to default of 0xffffffffffff
*/

void InitRFID()
{
	SPI.begin();
	mfrc522.PCD_Init();
	//Now to generate the super secret access key of 11111111111....
	for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
}
/**
*	Passes function of MRFC522 library,PICC_IsNewCardPresent(), up hierarchy. Function determines whether new card is present in RFID module field.
@return	Bool, true if there is a new card	
*/
bool CheckIfNEWRFIDCardPresent()
{
	return mfrc522.PICC_IsNewCardPresent();
}
/**
*	Passes function of MRFC522 library,PICC_ReadCardSerial(), up hierarchy. Function selects a single card for communication including implementation of anticollision measures
@return	Bool, true if card selected
*/
bool SelectCard()
{
	return mfrc522.PICC_ReadCardSerial();
}

uint16_t ParseCardIDandInfo()
{
	uint8_t Buffer[18];
	uint8_t size = sizeof(Buffer);
	byte CardType = 0;
	//Interpret info strings from card, card type, next block to write, last unit to write (to prevent the same station filling in multiple blocks...) e.c.t.
	//Have to authenitacte, note that only one authentification is needed as we are only dealing with a single sector and block!
	mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, DOT_INFO_TRAILER, &key, &(mfrc522.uid));
	//read the info block
	mfrc522.MIFARE_Read(DOT_INFO_BLOCK, Buffer, &size);
	/*
	INFO BLOCK LAYOUT
	0  	1	2  	 	3	 4	5	6	7	8	9	10	11	12	13	14	15
	CARDID	LAST 	1st/||RFU->
			BLK	  	2nd	||RFU-> (PROBABLY last PCD encountered to prevent a single PCD filling up the entire memory)
			Writtn 	Flg	||RFU->
	*/
	//Get card type (to detect overfilling)
	CardType = mfrc522.PICC_GetType(mfrc522.uid.sak);
	if(((CardType==MFRC522::PICC_TYPE_MIFARE_1K)&&(Buffer[2]>=62))||((CardType==MFRC522::PICC_TYPE_MIFARE_4K)&&(Buffer[2]>=126)))
	{
		//Card has filled up all our nice first spots card writes will now require filling in of second spots
		if((Buffer[3]&0b00000001)==1)
		{
			//have we already gone back to beginning, THE CARD IS FULL!!!! (like over 100 controls, probably more an error than a likely user scenario)
			
			return 0xffff; //indicate an error
		}
		else{
			Buffer[2] = CheckIfTrailerClash(DOT_FIRST_VISIT_BLOCK-1);//we are now going to write in last half of first time stamp block
			
			Buffer[3] = 0x01;//set flag for second hals wrap around
			mfrc522.MIFARE_Write(DOT_INFO_BLOCK, Buffer, 16);
		}
	}
	else
	{
		Buffer[2] = CheckIfTrailerClash(Buffer[2]+1);
		mfrc522.MIFARE_Write(DOT_INFO_BLOCK, Buffer, 16);
	}
	/* NOTE: PICC is not halted at the end of this function as we are always going to communicate with it later anyway...
	This saves 5ms (required by iso14443 standard between communications for hello and select...)
	// Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();*/
	return ((Buffer[2]<<8)+Buffer[3]);
	//Update info strings on card to tell the next station where to write
	
}
void HaltCard()
{
	// Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
}
uint8_t CheckIfTrailerClash(uint8_t Block)//prevent the next visit time stamp from overwriting a trailer block (which has BAD consequences...)
{
	if((Block%4)==3)
	{
		return Block+1;
	}
	else
	{
		return Block;
	}
}
/*
	Reset info block to ensure 
*/
void ResetCard()
{
	uint8_t Buffer[18] = {0};
	uint8_t size = sizeof(Buffer);
	//Interpret info strings from card, card type, next block to write, last unit to write (to prevent the same station filling in multiple blocks...) e.c.t.
	//Have to authenitacte
	mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, DOT_INFO_TRAILER, &key, &(mfrc522.uid));
	//read the info block
	mfrc522.MIFARE_Read(DOT_INFO_BLOCK, Buffer, &size);
	Buffer[2] = CheckIfTrailerClash(DOT_FIRST_VISIT_BLOCK-1);//Reset indicator to be first available time stamp block...
			
	Buffer[3] = 0x00;//1st seconde flag = 0
	mfrc522.MIFARE_Write(DOT_INFO_BLOCK, Buffer, 16);
	
	if(mfrc522.MIFARE_Write(DOT_INFO_BLOCK, Buffer, 16)==mfrc522.STATUS_OK)//check final RFID operation i.e. the most likely to go wrond as the user runs away :)
	{
		if(mfrc522.PICC_CleanMifareData(&(mfrc522.uid)))
		{
			return true;
		}
	}
	else
	{
		return false;
	}
	// Halt PICC
    /*mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();*/
}
/*
	Generates Visit record string then writes to appropriate PICC block
*/

bool WriteVisitStamp(struct time_t* VisitTime, uint16_t WriteBlock)
{
	uint8_t Buffer[18] = {0};
	uint8_t TempBuffer[8] = {0};
	byte size = sizeof(Buffer);
	/*
		Calculations to work out Write Block trailer
		WriteBlock = (WriteBlock&0xff00)>>8;
		
		trailer = WriteBlock/4 (round down)+3
		
	*/
	uint8_t WriteBlockVar = 0;
	WriteBlockVar = (WriteBlock&0xff00)>>8;
	
	CreateNewTimeStampString(VisitTime,TempBuffer);
	
	mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, ((WriteBlockVar/4)*4)+3, &key, &(mfrc522.uid));
	if((WriteBlock&0x0001)==1)//detect whether we are writing in the second half or not
	{
		
		mfrc522.MIFARE_Read(WriteBlockVar, Buffer, &size);
		for(int i = 8; i<16;i++)
		{
			Buffer[i]=TempBuffer[i-8];
		}
	}
	else
	{
		for(int i = 0; i<8;i++)
		{
			Buffer[i]=TempBuffer[i];
		}
	}
	if(mfrc522.MIFARE_Write(WriteBlockVar, Buffer, 16)==mfrc522.STATUS_OK)//check final RFID operation i.e. the most likely to go wrond as the user runs away :)
	{
		return true;
	}
	else
	{
		return false;
	}
	
    /*// Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();*/
}

void UpdateStatusFromEEPROM()
{
	uint8_t Flags = 0;
	ODOTS_DeviceID[0] = EEPROM.read(DEVICEID1ADDR);
	ODOTS_DeviceID[1] = EEPROM.read(DEVICEID2ADDR);
	Flags = EEPROM.read(FLAGSADDR);
	if(Flags&DOWNLOADSTATUSMASK)
	{
		IsUnitConfiguredForDownload = true;
	}
	else
	{
		IsUnitConfiguredForDownload = false;
	}
	if(Flags&CLEARSTATUSMASK)
	{
		IsUnitConfiguredForClear = true;
	}
	else
	{
		IsUnitConfiguredForClear = false;
	}
}
bool DumpCardToSerial()
{
	return mfrc522.PICC_DumpDataToSerial(&(mfrc522.uid));
}
void SendRFIDModuleToSleep()
{
	mfrc522.PCD_AntennaOff();
}
void WakeRFIDModule()
{
	mfrc522.PCD_AntennaOn();
}
void SendUnitToSleep()
{
	LowPower.idle(SLEEP_30MS, ADC_ON, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, 
                SPI_OFF, USART0_ON, TWI_OFF); //ADC unit sleep handled elsewhere, these are more like bit flags to enable behaviour....
}