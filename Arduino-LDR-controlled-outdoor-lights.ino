/* 
 *  At a set interval the LDR light sensor value will be checked. If this value is below a certain threshold,
 *  the relay will be set so the 220V circuit is turned on and the lights will be on.
 *  
 *  A control LED blinks at the same interval as the light-sensor's value is read
 *  
 *  The script includes a certain delay mechanism to avoid false positive readings:
 *  the sensor's readings should be below a certain value for a set ammount of time, before anything happens
 *  In addition: if the lights are turned on, they will stay turned on at least a given ammount of time
 *  
 *  Author: Dennis Hunink
 *  Contact: dennishunink@me.com / twitter.com/dennishunink / github.com/dhunink
 *  
 *  Download the RTClib from https://github.com/adafruit/RTClib
 */
#include <Wire.h>
#include <RTClib.h>

RTC_DS1307 RTC;

//////////////////////////////////
//SETTINGS: edit variables below//
//////////////////////////////////

////////
//Time//
//The battry-powered clock needs a time at first use; this is the time that will be rememberd.
//If you set the following variable to 'true', the current PC's time will be written to the clock chip when the sketch is uploaded
//Use this on the initial upload, or if any errors occur
boolean writeTime = false;

//Set the pins at which the LED is connected
const int ledPin =  8; 

//Set the pin at which the LDR light sensor is connected
const int sensorPin = A3;

//Set the pin at which the signal wire for the relay is connected
const int relayPin = 9;

//At which interval (in milliseconds) should we read the sensor? 
//Note that the control LED will blink at the same interval.
const long interval = 400;

//Threshold value for the light sensor: readings below this value will be considered as 'dark outside',
//readings above are considered 'bright outside'. Enter a value between 0 and 1000
const long sensorThresholdValue = 200;

//To prevent false-positive or false-negative readings, set a ammount of time in milliseconds
//Only if readings return the same value during this ammount of time, a change in brightness of the light
//outside has really occured
const long sensorThresholdTime = 10000; //Time in ms before the thresholdValue is actually acknowledged

//We also use a clock function: the lights will never turn on between certain hours, even if it is dark outside
//Set the hour at which to enable; before this hour, lights will stay off
const long timeStart = 8;
//Set the hour at which to disable; after this hour, lights will stay off
const long timeEnd = 23;

///////////////
//!!WARNING: do not edit below this line//
///////////////

// Variables will change during exceution of the script
int ledState = LOW;
int relayState = LOW;
boolean ledWarning = false                                                                       ;
unsigned long nowHour = 0;
unsigned long previousMillisLed = 0;      // will store last time LED was updated
unsigned long previousMillisSensor  = 0;  // will store last time the sensor was read
unsigned long initialMillisSensorDark = 0; // will store the first time it appears to be dark
unsigned long initialMillisSensorBright = 0; // will store the first time it appears to be bright

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  Wire.begin();
  RTC.begin();

  if(!RTC.isrunning()){
    Serial.println("WARNING: problem with the clock, time is NOT running");
    ledWarning = true;
  }

  if(writeTime == true){
    Serial.println("Resetting the clock's time tot the current PC's time");
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  
  Serial.println("Arduino booted");
}

void loop()
{
  
  unsigned long currentMillis = millis();

  //Reading the lightsensor's value and handeling this value
  if(currentMillis - previousMillisSensor >= interval) {
    previousMillisSensor = currentMillis; 
    previousMillisLed = currentMillis;

    int sensorValue = analogRead(sensorPin);
    
    Serial.print("Sensor value=");
    Serial.print(sensorValue);
    Serial.println("");

    //Blink the LED: warning if needed (8 times faster), otherwise just pulse-wise
    if(ledWarning == true){
      digitalWrite(ledPin, HIGH);
      delay(interval/8);
      digitalWrite(ledPin, LOW);
      delay(interval/8);
      digitalWrite(ledPin, HIGH);
      delay(interval/8);
      digitalWrite(ledPin, LOW);
      delay(interval/4);
      digitalWrite(ledPin, HIGH);
      delay(interval/8);
      digitalWrite(ledPin, LOW);
      delay(interval/8);
      digitalWrite(ledPin, HIGH);
      delay(interval/8);
      digitalWrite(ledPin, LOW);
    }else{
      // if the LED is off turn it on and vice-versa:
      if (ledState == LOW){
        ledState = HIGH;
      }else{
        ledState = LOW;
      }
    }
  
    if(sensorValue < sensorThresholdValue && relayState == LOW){
      //It's dark outside, but the lights are currently off!
      Serial.println("relay is LOW, but it's light outside");
      if(initialMillisSensorDark == 0){
        //If this is the first time this situation occurred, let's store that
        initialMillisSensorDark = currentMillis;
        Serial.println("waiting for confirmation of this reading");
      }else if(currentMillis - initialMillisSensorDark >= sensorThresholdTime){
        //This reading seems to be constant; let's turn the light on!
        relayState = HIGH;
        initialMillisSensorDark = 0;
        Serial.println("Turn on the lights, it's dark outside!");
      }
      
    }else if(sensorValue > sensorThresholdValue && relayState == HIGH){
      //Its no longer dark outside, but the lights are still on
      Serial.println("relay is HIGH, but light is bright");
      if(initialMillisSensorBright == 0){
        //If this is the first time this situation occurred, let's store that
        initialMillisSensorBright = currentMillis;
        Serial.println("waiting for confirmation of this reading");
      }else if(currentMillis - initialMillisSensorBright >= sensorThresholdTime){
        //This reading seems to be constant; let's turn the light on!
        relayState = LOW;
        initialMillisSensorBright = 0;
        Serial.println("Turn off the lights, it's not dark outside.");
      }
      
    }else{
      //We have no reason to change anything to the lighting settings
      Serial.println("All okay!");
      initialMillisSensorDark = 0;
      initialMillisSensorBright = 0;
    }

    //Check time against the timesettings
    DateTime now = RTC.now();
    nowHour = now.hour();
    
    if(nowHour < timeStart || nowHour >= timeEnd ){
      Serial.println("Turn off the lights, as set in the time-settings");
      relayState = LOW;
    }
    
    digitalWrite(relayPin, relayState);
    digitalWrite(ledPin, ledState);
    
  }

}

