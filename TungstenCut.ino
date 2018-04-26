#include <SD.h>
#include <Vector.h>
#include <MuriSensors.h>

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
#define RAZOR 7
#define BURNER 8
#define TEMP_PIN 9
#define CHIPSELECT 4

//CONTROL VARIABLES AND DEFINES
long masterTimer = 7200000;       //master timer in milliseconds, time until cutdown
#define FLOATTIME 1200000        //20 minutes of floattime is the default
//sensor update delay times in ms
#define ACCEL_UPDATE_DELAY 1000
#define TEMP_UPDATE_DELAY 1000

//baud rate for copernicus, changes depending on GPS used
#define GPS_BAUD 4800

//global reading variables for easy access to sensor data (not having to go through functions
int accelerations[3] = {};
String temperature = "";

//SD card filenames
#define GPS_FILENAME "GPS"
#define EVENTLOG_FILENAME "ELOG"
String Ename = " ";
String GPSname = " ";
File eventLog;
File GPSlog;
//SETUP FOR SENSORS AND THEIR CLASSES
//accelerometer
Accelerometer ACCEL = Accelerometer("Accel", ACCEL_UPDATE_DELAY, &accelerations[0]);
AbstractSensor * Accel = &ACCEL;
//temp sensor
temperatureSensor TEMPSENSOR = temperatureSensor("temp_sensor_1", TEMP_PIN, TEMP_UPDATE_DELAY, &temperature );
AbstractSensor * TempSensor = &TEMPSENSOR;
TinyGPSPlus GPS;
GPS_sensor GpS = GPS_sensor("GPS", &Serial1, GPS_BAUD, &GPS);
AbstractSensor * gps = &GpS;
#define TEMP_1_PIN 9 
//sensor array
Vector<AbstractSensor*> sensors;

//XBEE setup
//"W" stands for tungsten. Change the second character to correspond with the
//comms relay being used- A, B, or C. remnant of the 2017 eclipse ballooning project
const String xBeeID = "WA";
// LED SETUP
class LED {
  public:
      LED(uint8_t pin);
      LED(uint8_t pin, unsigned int onTime, unsigned int offTime);
      void turn_on();
      void turn_off();
      void blink();
      void change_blink(unsigned int onTime, unsigned int offTime, uint8_t onTimes);
      
  private:
    uint8_t state; //if the LED is on or off
    uint8_t pin;
    uint8_t onTimes;
    unsigned int onTime;
    unsigned int offTime;
    unsigned int holderOnTime;
    unsigned int holderOffTime;
    unsigned long timer;
};

//create all of the LEDS 
LED sd_led = LED(SD_LED);
LED data_led = LED(DATA_LED);
LED fix_led = LED(FIX_LED);

//CREATE CUTTER CLASS TO MANAGE BURNERS AND RAZORS
// **NOTE**
//It is called a cutter when it is really a razor and a burner rolled into one.
class Cutter {
  public:
     Cutter(uint8_t razor, uint8_t burner);
     void runCut();
     void runCut(unsigned int onTime, uint8_t times);
     void checkAction();
  private:
     uint8_t burner;       //burner pin
     uint8_t razor;        //razor pin
     unsigned int onTime;  //time burner is on
     unsigned int offTime; //time burner is off in cycles
     uint8_t times;        //0 when no burns, > 0 when burns in queue
     unsigned long timer;  //keeps track of time for burns
     uint8_t state;        //whether the burner is on
};

//DECLARE THE CUTTERS

Cutter cutter1 = Cutter(RAZOR, BURNER);


/****FLOAT WATCHING CLASS***
 * class to watch and manage the floating of the balloon. built in with 
 * actions to make sure that faulty GPS hits don't mess up the functionality of the system
 * 
 */
class FloatWatch {
  public:
     FloatWatch(unsigned int floatTime);
     void updateStatus();

  private:
     unsigned long Timer;
     unsigned long floatTime;
     uint8_t floatState;             //the current state of the floatwatch
     unsigned long prevAlt;
     uint8_t checkFloat;        //how many times we've had decreasing hits
     unsigned long floatStartTime;
};


//START OF SETUP CODE
//WHERE EVERYTHING ACTUALLY HAPPENS

void setup(){
  
//add the sensors to the vector
sensors.push_back(Accel);
sensors.push_back(gps);
sensors.push_back(TempSensor);

//initialize all of the sensors
for (int i = 0; i < sensors.size(); i++){
   sensors[i] -> init();
}


//create the SD files. Blink if something goes wrong
  while (!SD.begin(CHIPSELECT)) {            //power LED will blink if no card is inserted
    Serial.println("No SD");
    sd_led.turn_on();
    delay(500);
    sd_led.turn_off();
    delay(500);
  }
  //sendXBee("Checking for existing file");
  //Check for existing event logs and creates a new one
  for (int i = 0; i < 100; i++) {
    if (!SD.exists(EVENTLOG_FILENAME + String(i / 10) + String(i % 10))) {
      Ename = EVENTLOG_FILENAME + String(i / 10) + String(i % 10);
      openEventlog();
      break;
    }
  }

  //sendXBee("event log created: " + Ename);

  //Same but for GPS
  for (int i = 0; i < 100; i++) {
    if (!SD.exists("GPS" + String(i / 10) + String(i % 10) + ".csv")) {
      GPSname = "GPS" + String(i / 10) + String(i % 10) + ".csv";
      openGPSlog();
      break;
    }
  }

  //sendXBee("GPS log created: " + GPSname);

}
void loop(){
  UpdateSensors();
  cutter1.checkAction();
  
  }

