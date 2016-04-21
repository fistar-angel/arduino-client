/*
 * ANGEL PROJECT | SINGULAR LOGIC SA
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
 * Operation: collect a set of sensing devices signals and upload them on the Residential Gateway (JSON format)
 * Author: panagiotis athanasoulis
 * Email: pathanasoulis@ep.singularlogic.eu
 *                                                                    
 */


/************************************************************************/
/*			Defines                                                     */
/************************************************************************/
#define PORT				80			// set port in which Residential gateway listens to
#define BUFFER_SIZE			210			// set the size of buffer
#define TEMP_BUFFER_SIZE	38			// set the size of the temporary buffer
#define HW_BRAUD			115200		// set the BAUD rate of HW serial
#define SW_BRAUD			9600		// set the BAUD rate of SW serial
#define TX					2			// set transmit DIGIT PIN
#define RX					3			// set receive DIGIT PIN
#define DEBUG				1			// set debug mode ON/OFF
#define ARDUINO_CYCLE		300000		// arduino sleep time in milliseconds
#define WIFI_SLP			300			// wifly sleep time in seconds
#define TOTAL_WIFI_ATTEMPTS 2			// total number of connection attempts
#define LED_PIN_MESSSAGES   4			// set DIGIT PIN for LEB DEBUG
#define PULSEOXIMETER_PIN   6			// set digital PIN to detect a RISING edge
#define PULSEOXIMETER_ATTEMPTS	1000	// set the number of iterations to read the heart rate and blood oxygen saturation


/************************************************************************/
/*			Prototypes of used functions                                */
/************************************************************************/
void initiateWifly();					// initiate the wifly module
char *getStatisticsWifly();				// retrieve statistics from wifly
void terminateExistingConnection();		// terminate existing connection
void terminal();						// print terminal
void httpPostRequest(char *);			// upload signals via HTTP POST
void flushTempBuffer(void);				// clear temporary buffer
void flushBuffer(void);					// flush buffer
void composeJSON(const char *);			// compose a JSON object
void blink(int , int , int);			// blink a LED with specific rate
void delaySeconds(int);					// delay in seconds
void delayMinutes(int);					// delay in milliseconds
void displayCollectedSignals(char *);	// print out the collected signals in serial


/************************************************************************/
/*			Variables with global scope                                 */
/************************************************************************/
const char mySSID[] = "SF_AP";			// set SSID of wifi network
const char myPassword[] = "sf@k2014";	// set password/key of wifi network
const char site[] = "192.168.1.131";	// set the (static IP) of residential gateway
char buf[BUFFER_SIZE];					// set the global buffer
char temp[TEMP_BUFFER_SIZE];			// set the temporary buffer
