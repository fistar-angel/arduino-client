/*
 * ANGEL PROJECT | SINGULAR LOGIC SA
 * 
 * Description: "The Arduino Uno collects periodically medical signals from a set 
 * of eHealth sensing devices and upload them on the Residential Gateway (otherwise
 * called Measurement Aggregator) using the RN-XV WiFly module. The device consists 
 * of three layers: the Arduino Uno R3, the XBEE included the RN-XV WiFly module and
 * the eHealth platform V2.0. This eHealth platform contains the following sensors:
 * the Pulse and Oxygen in blood sensor, the airflow sensor, the Body Temperature 
 * sensor, the Glucometer sensor and the blood pressure sensor. The measuments is 
 * sent on the Residential Gateway over the HTTP protocol as a JSON object."
 *
 * Copyright 2015 Singular Logic – All Rights Reserved
 *
 * This enabler will be released to external parties (out of the FI-PPP 
 * and FI-STAR project) by following an open source approach.
 *
 * Fall Risk Evaluation Specific Enabler will be made available under 
 * an open source license, the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Panagiotis Athanasoulis
 * Email: pathanasoulis@ep.singularlogic.eu
 * Version: 1.0
 */


#include <stdio.h>                    // library to perform Input/Output operations
#include <string.h>                   // library to manipulate strings and arrays
#include <stdlib.h>                   // standard general utilities library 
#include <WiFlyHQ.h>                  // library for the Wifly RN-XV Wifly module
#include <MySoftwareSerial.h>         // library that provide Software serials
#include <eHealth.h>                  // library for eHealth sensing devices
#include <PinChangeInt.h>             // library for Pin Change interrupts (Arduino environment)
#include <AngelConfig.h>              // library for the network and WIFI settings
                     
                                      
MySoftwareSerial wifiSerial(TX,RX);   // Rx: 3 pin (in Xbee),Tx: 2 pin (in Xbee)
WiFly wifly;                          // create an object


void setup()
{
    //////////////////////////////////
    //  SETUP STEP: run after reboot
    //////////////////////////////////
    
    // blood pressure
    eHealth.readBloodPressureSensor();  
    //delay(20000);
    //eHealth.readGlucometer();
   
    // Hardware and Software Serial initialization
    Serial.begin(HW_BRAUD);
    wifiSerial.begin(SW_BRAUD);
    
    // Initiate the pulsioximeter sensor
    eHealth.initPulsioximeter();  
    // set the debug LED    
    
    pinMode(LED_PIN_MESSSAGES, OUTPUT);   
    digitalWrite(LED_PIN_MESSSAGES, LOW); 
    
    // delay 0.2sec
    delay(200);
}


void loop()
{       
    // Flush buffer in each loop
    flushBuffer();
    flushTempBuffer();
    
    // Collect the measurements. Blink the len in pin4, 5 times for 200msec
    blink(LED_PIN_MESSSAGES, 5, 200);

    // JSON structure
    composeJSON("{\"measurements\":{");

    ///////////////////////////
    //    ANALOG SENSORS
    ///////////////////////////

    // Get temperature
    delay(1000);
    float bTemp = 0.0;
    digitalWrite(LED_PIN_MESSSAGES, HIGH);   // turn the LED on (HIGH is the voltage level)
    // Get the average of ten samples
    for (int i=0; i<10; i++){
      digitalWrite(LED_PIN_MESSSAGES, LOW);
      bTemp += eHealth.getTemperature();
      delay(50);
      digitalWrite(LED_PIN_MESSSAGES, HIGH);
      delay(450);
    }
    composeJSON("\"Body temperature\":");
    itoa((bTemp/10) * 100, temp, 10);
    composeJSON(temp);
    flushTempBuffer();
    digitalWrite(LED_PIN_MESSSAGES, LOW);
    delay(1000);


    // Get respiration rate
    int respRate = 0, sec;    
    digitalWrite(LED_PIN_MESSSAGES, HIGH);
    for (sec=0; sec<30; sec++){
      digitalWrite(LED_PIN_MESSSAGES, LOW);
      if (eHealth.getAirFlow() > 25){
        respRate += 1;
      }
      delay(50);
      digitalWrite(LED_PIN_MESSSAGES, HIGH);
      delay(950);
    }
    digitalWrite(LED_PIN_MESSSAGES, LOW);
    //Serial.println(respRate);
    if (respRate > 0){
      composeJSON(",\"Respiration rate\":");
      itoa(respRate*2*100, temp, 10);
      composeJSON(temp);
      flushTempBuffer();
    }

    
    /////////////////////////////
    //    DIGITAL SENSORS
    ////////////////////////////  
    
    uint8_t noBP = eHealth.getBloodPressureLength();
    if ( noBP > 0 ) {      
      digitalWrite(LED_PIN_MESSSAGES, HIGH);
      
      //=========================
      // Systolic blood pressure
      //=========================
      if ( eHealth.bloodPressureDataVector[noBP - 1].systolic > 0 ){
          composeJSON(",\"Systolic blood pressure\":");
          itoa((eHealth.bloodPressureDataVector[noBP - 1].systolic+30)*100, temp, 10);
          composeJSON(temp);
          flushTempBuffer();
      } 
      
      digitalWrite(LED_PIN_MESSSAGES, LOW);
      delay(50);
      digitalWrite(LED_PIN_MESSSAGES, HIGH);
      delay(100);
      
      //==========================
      // Diastolic blood pressure
      //==========================
      if ( eHealth.bloodPressureDataVector[noBP - 1].diastolic > 0 ){
          composeJSON(",\"Diastolic blood pressure\":");
          itoa(eHealth.bloodPressureDataVector[noBP - 1].diastolic*100, temp, 10);
          composeJSON(temp);
          flushTempBuffer();
      }
      
      digitalWrite(LED_PIN_MESSSAGES, LOW);
      delay(50);
      digitalWrite(LED_PIN_MESSSAGES, HIGH);
      delay(100);
      
      //=============
      // Heart rate
      //=============
      if ( eHealth.bloodPressureDataVector[noBP - 1].pulse > 0 ){
          composeJSON(",\"Heart rate\":");
          itoa(eHealth.bloodPressureDataVector[noBP - 1].pulse*100, temp, 10);
          composeJSON(temp);
          flushTempBuffer();
      } 
      
      digitalWrite(LED_PIN_MESSSAGES, LOW);
    }
    
    
    //==============
    // glucometer
    //==============
    /*
    uint8_t noGL = eHealth.getGlucometerLength();
    if ( eHealth.glucoseDataVector[noGL-1].glucose > 0 ){
        digitalWrite(LED_PIN_MESSSAGES, HIGH);
  
        composeJSON(",\"Blood glucose\":");
        itoa(eHealth.glucoseDataVector[noGL-1].glucose *100, temp, 10);
        composeJSON(temp);
        flushTempBuffer();
        
        digitalWrite(LED_PIN_MESSSAGES, LOW);
    } 
    */
    
    //========================
    //  pulse-oximeter device
    //========================
    int previousPinState=0, currPinState=0;
    boolean readPulseOximeter = false;
    int attempts=0;
    uint8_t heartRate = 0, bloodOxygen = 0;
    
    digitalWrite(LED_PIN_MESSSAGES, HIGH);
    
    // read the signals from the pulseoximeter sensor
    while(attempts < PULSEOXIMETER_ATTEMPTS)
    {      
      // initiate
      uint8_t currHeartRate = 0, currBloodOxygen = 0;
      if (attempts == 0){
        previousPinState = digitalRead(PULSEOXIMETER_PIN);
        currPinState = previousPinState;
      }
      currPinState = digitalRead(PULSEOXIMETER_PIN);
      
      // read the digital PINs 7-13
      eHealth.readPulsioximeter();
            
      // Compare the current and the previous status of PIN       
      if(previousPinState == 0 && currPinState == 1){
          Serial.println("OK\t");
          // in the next cycle collect signals
          readPulseOximeter = true;
      }
      
      // If the edge was changed from 0->1, get the signals
      if (readPulseOximeter){ 
          // blink the LED
          digitalWrite(LED_PIN_MESSSAGES, LOW);
          delay(50);
          digitalWrite(LED_PIN_MESSSAGES, HIGH);
          
          readPulseOximeter = false;
                  
          //currHeartRate = eHealth.getBPM();
          //heartRate = (heartRate > currHeartRate) ? heartRate : currHeartRate;
          currBloodOxygen = eHealth.getOxygenSaturation();
          bloodOxygen = (bloodOxygen > currBloodOxygen) ? bloodOxygen : currBloodOxygen;

          if (DEBUG) {
              Serial.print("#");
              Serial.print(attempts);
              Serial.print("\tHeart=");
              Serial.print(heartRate);
              Serial.print(",\tOxygen=");
              Serial.println(bloodOxygen);
          } 
      }
      
      // proceed to next attempt
      attempts++;
      // keep the previous state of the digital pin 6
      previousPinState = currPinState;     
    }
    digitalWrite(LED_PIN_MESSSAGES, LOW);
    
    /*
    * Get the heart rate from the blood pressure device
    if( heartRate > 0 ){      
        composeJSON(",\"Heart rate\":");
        itoa(heartRate*100, temp, 10);
        composeJSON(temp);
        flushTempBuffer();
    }
    */
    
    //=========================
    // Blood Oxygen saturation
    //=========================
    if( bloodOxygen > 0 ){
        composeJSON(",\"Blood oxygen\":");
        itoa(bloodOxygen*100, temp, 10);
        composeJSON(temp);
        flushTempBuffer();
    }
    
    
    // close JSON object
    composeJSON("}}");
    // upload
    httpPostRequest(buf);
    // blink led fo termination
    blink(LED_PIN_MESSSAGES, 2, 1000);
    // pause 5 minutes
    delay(ARDUINO_CYCLE);
}


/*
 * Clear the main buffer
 */
void flushBuffer(){
  // Empty global buffer
  memset(buf, 0, BUFFER_SIZE);
}

/*
 * Clear the temporary buffer
 */
void flushTempBuffer(){
  // Empty temporary buffer
  memset(temp, 0, TEMP_BUFFER_SIZE);
}

/*
 *  Append measurements to struct the JSON object
 */
void composeJSON(const char *value){
  // append info into buffer
  strcat(buf, value);
}

/*
 * Delay in seconds
 */
void delaySeconds(int seconds){
    delay(seconds * 1000);
}

/*
 * Delay in minutes
 */
void delayMinutes(int minutes){
  delay(minutes * 60 * 1000);
}


/*
 * Initiate the wifly module
 */
void initiateWifly() {  
  Serial.print("INFO: wifly free mem->");
  Serial.print(wifly.getFreeMemory(),DEC);
  Serial.println("B");
  
  if (!wifly.begin(&wifiSerial, &Serial)) {
    Serial.println("ERROR: wifly init");
    terminal();
    //wifly.reboot();
  }
}

/*
 *  Associate the wifly module with a network
 */
void associateWifly(){
    /* Join wifi network if not already associated */
    Serial.println("INFO: Try to join");
    
    // set wlan ssid <SSID>
    wifly.setSSID(mySSID);
    // set wlan phrase <password> & set wlan hide 1
    wifly.setPassphrase(myPassword);
    // set ip dhcp 1
    wifly.enableDHCP();
    // set ip protocol 18 - enables HTTP/TCP
    wifly.setProtocol(18);
    // set wlan join 0
    wifly.setJoin(0);
    // set comm size 1420
    wifly.setFlushSize(1420);
    // set wlan tx 12
    wifly.setTxPower(12);    
    // set timer for join
    wifly.setJoinTimer(15000);
    // save
    wifly.save();

    if (wifly.join()) {
      Serial.println("INFO: Joined");
    } else {
      Serial.println("ERROR: No joined");
      terminal();
    }
}

/*
 * Get the MAC address of wifly module
 */
char *getStatisticsWifly() {
    char *mac;
    Serial.println("Display client stats: ");
    Serial.print("MAC: ");
    mac = wifly.getMAC(buf, sizeof(buf));
    flushBuffer();
    Serial.println(mac);
    Serial.print("IP: ");
    Serial.println(wifly.getIP(buf, sizeof(buf)));
    Serial.print("Netmask: ");
    Serial.println(wifly.getNetmask(buf, sizeof(buf)));
    Serial.print("Gateway: ");
    Serial.println(wifly.getGateway(buf, sizeof(buf)));
    return mac;
}

/*
 *  Terminate possible existing connection
 */
void terminateExistingConnection(){
   if (wifly.isConnected()) {
      wifly.close();
  } 
}

/*
 * postRequest
 * Upload data to residential gateway using HTTP POST request
 */
void httpPostRequest(char *data) {
  int attemp = 0;
  if (DEBUG){
    displayCollectedSignals(data);
  }
  
  // open connection with Residential Gateway  
  while (attemp < TOTAL_WIFI_ATTEMPTS) 
  {
    initiateWifly();
    delay(500);
    associateWifly();
    
    if (wifly.open(site, PORT)) {
        wifly.println("POST /angel/gateway.php HTTP/1.1");
        wifly.print("Host: ");
        wifly.println(site);
        wifly.println("Content-Type: application/json");
        wifly.print( "Content-Length: " );
        wifly.println( strlen(data) );
        wifly.println();
        wifly.print( data );
        wifly.println();        
        Serial.println("INFO: OK");
        attemp = TOTAL_WIFI_ATTEMPTS;
    } else {
        attemp++;
        wifly.close();
    }
  }
  // terminate existing connections  
  terminateExistingConnection();   
  // send wifly to sleep 
  wifly.sleep(WIFI_SLP);
}


/* 
 * Print out the collected signals 
 */
void displayCollectedSignals(char *data){
    Serial.print("INFO: ");
    Serial.println(data);
}


/* 
 * Blink a LED in a specific pin 
 */
void blink(int pin, int loops, int milliseconds){
  // blink the led
  for (int i = 0; i < loops; i ++){
    digitalWrite(pin, HIGH);
    delay(milliseconds);
    digitalWrite(pin, LOW);
    delay(milliseconds);
  }
}

/* 
 * Connect the WiFly serial to the serial monitor. 
 */
void terminal()
{
    while (true) {
	if (wifly.available() > 0) {
	    Serial.write(wifly.read());
	}

	if (Serial.available() > 0) {
	    wifly.write(Serial.read());
	}
    }
}

