/*
	BuzzerUtils.cpp
	
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
#include <Arduino.h>
#include "Config.h"

void BuzzerInit()
{
	pinMode(BUZZERPIN,OUTPUT);
	pinMode(LEDPIN1,OUTPUT);
	pinMode(LEDPIN2,OUTPUT);
	digitalWrite(BUZZERPIN,HIGH);
	digitalWrite(LEDPIN1,LOW);
	digitalWrite(LEDPIN2,LOW);
}
/*
	Begins Buzzer Operation, Busses fairly pure tone then quite harsh (lots of high harmonics) tone.
*/
void BuzzerBleep(){
  digitalWrite(LEDPIN1,HIGH);
  digitalWrite(LEDPIN2,LOW);
  analogWrite(BUZZERPIN,127);
  
  delay(100);//these delays could be an issue as they will block main execution - eventually an OS will have to be used to prevent this sort of blocking.
  
  digitalWrite(LEDPIN1,LOW);
  digitalWrite(LEDPIN2,HIGH);
  digitalWrite(BUZZERPIN,HIGH);
  delay(100);
  digitalWrite(LEDPIN1,HIGH);
  digitalWrite(LEDPIN2,LOW);
  analogWrite(BUZZERPIN,30);
  delay(100);
  digitalWrite(BUZZERPIN,HIGH);
	digitalWrite(LEDPIN1,LOW);
	digitalWrite(LEDPIN2,LOW);
}
void ErrorBuzz()
{
  
  analogWrite(BUZZERPIN,50);
  delay(100);//these delays could be an issue as they will block main execution - eventually an OS will have to be used to prevent this sort of blocking.
  digitalWrite(BUZZERPIN,LOW);
  delay(100);
  analogWrite(BUZZERPIN,50);
  delay(100);
  digitalWrite(BUZZERPIN,LOW);
  delay(100);
  analogWrite(BUZZERPIN,50);
  delay(100);
  digitalWrite(BUZZERPIN,LOW);
  return;
}
void BuzzerBlip()
{
	analogWrite(BUZZERPIN,150);
	delay(20);
	digitalWrite(BUZZERPIN,HIGH);
}