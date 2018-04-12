

/*   
 Tungsten cut v 1.1.0
 
 Arduino Program for Tungsten Cut Flight Termination system
 written by Simon Peterson for the Minnesota Space Grant Consortium Eclipse originally
 for use during Eclipse ballooning missions. Later edited for use during
 the MURI stratospheric research characterization project under the direction
 of dr. Graham Candler.

 OVERVIEW: This code can terminate flights at high altitudes
 ******IMPORTANT PLEASE READ******
 LIBRARIES- the TinyGPS++ library is not the original TinyGPS++ library, it 
 was edited by Simon Peterson for specific use in this program and can be found at:
 https://github.com/simonpeterson/TinyGPS
  */
//sensor initiation
#include <SparkFun_ADXL345.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <TinyGPS++.h>

/*
     Component                    | Pins used             | Notes

     xBee serial(Rx, Tx)          | D0-1                  | IMPORTANT- is hardware serial (controls xBee and hard serial line), cannot upload with xBee plugged in
     Data LED (BLUE)              | D3                    | "action" LED (Blue), tells us what the payload is doing
     SD                           | D4, D50-52            | 50-52 do not not have wires but they are used!
     SD LED (RED)                 | D5                    | "SD" LED (Red). Only on when the file is open in SD card
     fix led                      | D6                    | whether or not we have a GPS fix, must be used with copernicus GPS unit
     razorcutter pin              | D7                    | High to this pin spins razor blade
     Fireburner                   | D8                    | High to this pin fires tungsten burner
     Tempread                     | D9                    | temperature sensor reading (power from other sources)
     Accel I2C                    | SDA,SCL               | I2C communication for accelerometer (pins 20 and 21)
*/
//pin defines
#define DATA_LED 3
#define SD_LED 5
#define FIX_LED 6
#define CUTTER 7
#define BURNER 8
#define TEMP_PIN 9

//sensor update delay times in ms
#define ACCEL_UPDATE_DELAY 1000
#define TEMP_UPDATE_DELAY 1000
//baud rate for copernicus, changes depending on GPS used
#define GPS_BAUD 4800
//global reading variables for easy access to sensor data (not having to go through functions
int accelerations[3] = {};
String temperature = "";

//sensor setup
//accelerometer
Accelerometer Accel = Accelerometer("Accel", ACCEL_UPDATE_DELAY, &accelerations[0]);
//temp sensor
temperatureSensor tempSensor = temperatureSensor("temp_sensor_1", TEMP_PIN, TEMP_UPDATE_DELAY, &temperature );
GPS gps = GPS("GPS", &Serial1, GPS_BAUD);
#define TEMP_1_PIN 9 


void setup(){
  
}
void loop(){
  
}

