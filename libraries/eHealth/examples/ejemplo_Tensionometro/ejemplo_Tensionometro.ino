/*
  *  eHelath board from Libelium for Arduino.
  *  Explanation : 
  *
  *  Copyright (C) 2012 Libelium Comunicaciones Distribuidas S.L.
  *  http://www.libelium.com
  *
  *  This program is free software: you can redistribute it and/or modify
  *  it under the terms of the GNU General Public License as published by
  *  the Free Software Foundation, either version 3 of the License, or
  *  (at your option) any later version.
  *
  *  This program is distributed in the hope that it will be useful,
  *  but WITHOUT ANY WARRANTY; without even the implied warranty of
  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *  GNU General Public License for more details.
  *
  *  You should have received a copy of the GNU General Public License
  *  along with this program.  If not, see .
  *
  *  Version 0.1
  *  Author: Ahmad Saad & Luis Martin
  */


#include <eHealth.h>


void setup() {
 Serial.begin(115200);
   Serial.println("Presione el boton ON/OFF para iniciar ");

}

void loop() {
  
  eHealth.initTensionometer();
  
  Serial.println("****************************");
  Serial.print("Valor de presion Sistolica :  ");
  Serial.println(eHealth.getSystolicPressure());
  delay(10);
        
  Serial.println("****************************");
  Serial.print("Valor presion sistolica : ");
  Serial.println(eHealth.getDiastolicPressure());
  delay(10); 
 
  delay(2000);   
}
