/*
	ODOTSSerialUtils.cpp
	
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
*/
#include <Arduino.h>
#include <EEPROM.h>
#include <avr/power.h>
#include "Config.h"
#include "ODOTSRTC.h"


//Private functions
uint8_t WaitForSerialWithTimeOut(uint8_t Bytes);
uint8_t ReadADC();
uint8_t SendADCToSleep();
uint8_t WakeADC();


//Functions
void SerialInit()
{
	Serial.begin(UARTBAUD);
	//begin ADC for battery voltage readings, use internal 1.1V reference
	//initialise the ADC 
	analogReference(ADCREF);
	pinMode(ADCSWITCHPIN,OUTPUT);
	digitalWrite(ADCSWITCHPIN, HIGH);
}

uint8_t CheckSerialForContents()
{
	return(Serial.available());
}
uint8_t SerialHandleRequest()
{
	//See Specification and User Guide: Serial Interface for full serial command specification and description
	uint8_t CommandByte = 'Z';
	uint8_t FlagsBuf = 0;
	uint8_t TimeBuf[6]={0};
	while(CommandByte=='Z')
	{
		CommandByte = Serial.read();
	}
	switch(CommandByte)
	{
		case 'A':
			Serial.write('a');
			
			//Request for DeviceID or Write to Device ID Register
			Serial.write(ODOTS_DeviceID[0]);//write Device ID
			Serial.write(ODOTS_DeviceID[1]);//write Device ID
			if(WaitForSerialWithTimeOut(1))
			{
				ODOTS_DeviceID[0] = Serial.read();
				if(WaitForSerialWithTimeOut(1))
				{
					ODOTS_DeviceID[1] = Serial.read();
				}
			}
			EEPROM.update(DEVICEID1ADDR,ODOTS_DeviceID[0]);
			EEPROM.update(DEVICEID2ADDR,ODOTS_DeviceID[1]);
			break;
		case 'B':
			Serial.write('b');
			//Request for Device Operation Flags, or write to device operation flags register
			Serial.write(EEPROM.read(FLAGSADDR));
			if(WaitForSerialWithTimeOut(1))
			{
				FlagsBuf = Serial.read();
				EEPROM.update(FLAGSADDR,FlagsBuf);
				IsUnitConfiguredForDownload = (FlagsBuf&DOWNLOADSTATUSMASK);
				IsUnitConfiguredForClear = (FlagsBuf&CLEARSTATUSMASK);
			}
			break;
		case 'C':
			Serial.write('c');
			//Request for Firmware Version
			Serial.write((uint8_t)FIRMWAREVERSION);
			break;
		case 'D':
			Serial.write('d');
			//Request for Hardware Version
			Serial.write((uint8_t)HARDWAREVERSION);
			break;
		case 'E':
			//RTC Clock Update (from host)
			//this is the tricky one...
			Serial.write('e');
			if(WaitForSerialWithTimeOut(6))
			{
				for(int i=0;i<6;i++)
				{
					TimeBuf[i]=Serial.read();
				}
				RTC_SetTime(TimeBuf[0], TimeBuf[1], TimeBuf[2],TimeBuf[3],TimeBuf[4],TimeBuf[5]);
				Serial.print(">>");
				Serial.write(TimeBuf,6);
				Serial.println('\n');
			}
			break;
		case 'G':
			//RTC clock time request (from host)
			Serial.write('g');
			RTC_ReadTime(TimeBuf);
			Serial.write(TimeBuf,6);
			break;
		case 'H':
			//RTC clock time request (from host)
			Serial.write('h');
			FlagsBuf = ReadADC();
			Serial.write(FlagsBuf);
			break;
		default:
			Serial.print(">>INVALID SERIALCOMMAND");
			Serial.println(CommandByte,HEX);
			break;
	}
}
/*
	Extension of Serial.avaiable with timeout function - prevents program locking up while waiting if someone pulls the plug!
*/
uint8_t WaitForSerialWithTimeOut(uint8_t Bytes)
{
	unsigned long TimeStart = 0;
	TimeStart = millis();
	while(Serial.available()<Bytes)//wait for serial input
	{
		if(millis()>(TimeStart+SERIALTIMEOUT))//but return false if serial timeout occurs
		{
			Serial.println(">>Timeout");
			return 0;
		}
	}
	return 1;
}
uint8_t ReadADC()
{
	/*
		Reads ADC, used
	*/
	int AnalogReading = 0;
	uint8_t ReturnVar = 0;
	WakeADC();
	digitalWrite(ADCSWITCHPIN, LOW);
	delay(1); //let ADC settle a bit...
	AnalogReading = analogRead(ADCPIN);
	SendADCToSleep();
	digitalWrite(ADCSWITCHPIN, HIGH);
	ReturnVar = 0+(AnalogReading >> 2);//10 bit ADC, we really only care about the first 8 bits, the last two are going to be susceptible to noise anyway...
	return ReturnVar;
}

uint8_t SendADCToSleep()
{
	ADCSRA &= ~(1 << ADEN);//tell control registers ADC disabled
	power_adc_disable();//Use AVR library to turn off ADC
}
uint8_t WakeADC()
{
	power_adc_enable();
	ADCSRA |= (1 << ADEN);
}