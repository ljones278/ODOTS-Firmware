/*
	ODOTSRTC.h
	
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

#ifndef SRC_ODOTSRTC_H_
#define SRC_ODOTSRTC_H_
#include "DS1337.h"



void InitRTC();
void RTCEnableAlarm();
struct time_t CalculateTimeStamp();
bool CreateNewTimeStampString(struct time_t* PunchTime, uint8_t* OutputBuffer);
uint8_t UpdateRTCReference();
void RTC_SetTime(uint8_t Year, uint8_t Month, uint8_t Day,uint8_t Hour,uint8_t Minute, uint8_t Second);
bool RTC_ReadTime(uint8_t* Buffer);
bool RTCAlarm();
#endif