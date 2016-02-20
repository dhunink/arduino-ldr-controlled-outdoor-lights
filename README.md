# arduino-ldr-controlled-outdoor-lights
Control outdoor lights using an Arduino, 220v relais and a LDR sensor

<strong>Step 1. Hardware</strong>
For this project we use the following hardware:
- 1x Arduino Uno or simmilair; actually used a much cheaper version in this project, shipped from china. See http://www.gearbest.com/boards-shields/pp_235013.html 
- 1x Light Intensity Sensor Module, like http://www.gearbest.com/sensors/pp_218153.html or https://www.adafruit.com/product/161 (in the last case, add the right resistor)
- 1x DS3231 Real-time Clock Module
- 1x 3mm LED 
- 1x resistor for the LED, for example 330Ohm
- Wire
- 12V power supply

<strong>Step 2. Prototyping</strong>
Wire everything up; see the Fritzing file for the correct schema and PCB view

<strong>Step 3. Configure the software</strong>
The Arduino software includes several variables that can be used. In essence:
At a set interval the LDR light sensor value will be checked. If this value is below a certain threshold, the relay will be set so the 220V circuit is turned on and the lights will be on.
A control LED blinks at the same interval as the light-sensor's value is read
The script includes a certain delay mechanism to avoid false positive readings: the sensor's readings should be below a certain value for a set ammount of time, before anything happens
In addition: if the lights are turned on, they will stay turned on at least a given ammount of time.

Make sure to include the correct library for the time-module!

<strong>Step 4. Putting it all together</strong>
It's up to your imagination how you'll fit the finished and soldered project together. 
