/*
	ODOTSRTC.cpp
	
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
#include	"DS1337.h"
#include <Arduino.h>
#include "Config.h"
	
	
	

	
	
	unsigned long RefMillis = 0;
	volatile unsigned long InterruptMillis = 0;
	unsigned long RTCMillisOffset = 0;
	volatile bool RTCFlag = false;
	struct time_t RTCReference;
	
	void RTC_InterruptServiceRoutine();//Local definition of function
	
	void InitRTC()
	{
		
		InitDS1337();

	}
	void RTCEnableAlarm()
	{
		pinMode(RTCALARMPIN,INPUT);
		attachInterrupt(digitalPinToInterrupt(RTCALARMPIN),RTC_InterruptServiceRoutine,FALLING);
		//ds1337_set_alarm_1_every_second();
		ds1337_set_alarm_1_at_second(5);
		ds1337_set_control(0b00000101);//enable alarm interrupt output from clock!
	}
	
	struct time_t CalculateTimeStamp()
	{
		
		uint8_t HourStep = 0;
		uint8_t MinuteStep = 0;
		uint8_t SecondStep = 0;
		uint16_t MilliStep = 0;
		
		MilliStep = millis()-RTCMillisOffset;//Number of Milliseconds since RTC update
		
		
		//Note the following rely on the fact that c++ divisor operators truncate by default when passed integers
		HourStep = (MilliStep/3600000)%24; //Number of hours since RTC update
		MinuteStep = (MilliStep/60000)%60;	//Number of minutes since RTC update
		SecondStep = (MilliStep/1000)%60;//Number of seconds since RTC update
		MilliStep = MilliStep%1000;//Number of Milliseconds since RTC update
		//These operations may take a while, there may be some fine tuning required in the future! - at the moment this will work with any update period.
		return {(RTCReference.millis+MilliStep)%1000,(RTCReference.second+SecondStep)%60,(RTCReference.minute+MinuteStep)%60,(RTCReference.hour+HourStep)%24,(RTCReference.day),RTCReference.month,RTCReference.year};
		//This return is broken, it will not track updates over time boundaries, however we are only writing millis, seconds minutes and hours to clock
	}
	
	bool CreateNewTimeStampString(struct time_t* PunchTime, uint8_t* OutputBuffer)
	{
		OutputBuffer[0] = ODOTS_DeviceID[0];
		OutputBuffer[1] = ODOTS_DeviceID[1];
		OutputBuffer[2] = 0b01010101;
		format_mifare_write_time_str(PunchTime, OutputBuffer);
		for(int i = 0;i<8;i++)
		{
			Serial.print(OutputBuffer[i],DEC);
			Serial.print(',');
			
		}
		Serial.print('\n');
		return true;
	}
	/*
		@brief	Updates RTC time reference struct, 
	*/
	uint8_t UpdateRTCReference()
	{
		//Assumes that RTC produced interrupt is being serviced inside a second of it being raised
		//store RTC time in global struct
		ds1337_read_time(&RTCReference);
		RTCMillisOffset = millis();//Set GLOBAL REFERENCE MILLISECOND TIME STAMP 
		RTCReference.millis = (RTCMillisOffset-InterruptMillis)%1000;//Set millisecond RTC time, not possible with RTC directly
		
		RTCFlag=false;
		ds1337_set_status(0x00);
		return 0;
	}
	/*
		ISR for RTC alarm
	*/
	void RTC_InterruptServiceRoutine()
	{
		InterruptMillis = millis();//get millisecond time reference for ISR
		RTCFlag = true;
		return;
	}
	bool RTCAlarm()
	{
		return RTCFlag;
	}
	void RTC_SetTime(uint8_t Year, uint8_t Month, uint8_t Day,uint8_t Hour,uint8_t Minute, uint8_t Second)
	{
		time_t SerialTime;
		make_time(&SerialTime, Year, Month, Day, Hour, Minute, Second);
		SerialTime.millis =0; 
		RTCMillisOffset=millis(); //this is to do with the fact that the we are updating the reference however the i2c exchange does take entire milliseconds to complete
		ds1337_write_time(&SerialTime);
		//Might as well update internal reference as well!
		RTCReference = SerialTime;
		
	}
	bool RTC_ReadTime(uint8_t* Buffer)
	{
		time_t SerialCallTime;
		SerialCallTime = CalculateTimeStamp();
		
		Buffer[0]= SerialCallTime.year;
		Buffer[1]= SerialCallTime.month;
		Buffer[2]=SerialCallTime.day;
		Buffer[3]=SerialCallTime.hour;
		Buffer[4]=SerialCallTime.minute;
		Buffer[5]=SerialCallTime.second;
		return true;
	}