

//Libraries
//this requires a special modded version of the TinyGPS library because for whatever
//reason the TinyGPS library does not include a "Fix" variable. the library can be found here:
//https://github.com/simonpeterson/TinyGPS/tree/master
#include <SPI.h>
#include <SD.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SparkFun_ADXL345.h>      //accelerometer library
//==============================================================
//               Code For Tungsten/Razor Cutter
//                 Danny Toth Summer 2017 - tothx051 and Simon Peterson- pet00291
//                 Edited for MURI Project by Garrett Ailts- ailts008
//==============================================================

//Version Description: Working xBee with limited commands (add/sub time, request time/cutdown). SD logging with poor formatting.

// Use: When payload is powered (i.e. batteries plugged in and switch in "on" position), it will be in flight mode.
//
//     Flight Mode:
//                 Payload will count up in milliseconds from zero until a specified time. One blink equals 5 minutes remaining. Once specified time has passed, payload will then
//                 fire the burner continuously until the burner breaks. Payload will then enter Recovery Mode.
//    Recovery Mode:
//                 Payload will continuously flash LED's to indicate its status until it is recovered or powered off. If it decides that
//                 the burn was unsuccessful, it will attempt to re-try burning until it decides it worked.

//=============================================================================================================================================
//=============================================================================================================================================
//      ____                      ____       __               ______            _____                        __  _
//     / __ )__  ___________     / __ \___  / /___ ___  __   / ____/___  ____  / __(_)___ ___  ___________ _/ /_(_)___  ____
//    / __  / / / / ___/ __ \   / / / / _ \/ / __ `/ / / /  / /   / __ \/ __ \/ /_/ / __ `/ / / / ___/ __ `/ __/ / __ \/ __ \
//   / /_/ / /_/ / /  / / / /  / /_/ /  __/ / /_/ / /_/ /  / /___/ /_/ / / / / __/ / /_/ / /_/ / /  / /_/ / /_/ / /_/ / / / /
//  /_____/\__,_/_/  /_/ /_/  /_____/\___/_/\__,_/\__, /   \____/\____/_/ /_/_/ /_/\__, /\__,_/_/   \__,_/\__/_/\____/_/ /_/
//                                               /____/                           /____/

int Master_Timer = 7200; //Flight master timer that terminates flight when the timer runs out! Changeable via xBee.
bool judgementDay = true;   //set to true to activate master timer. can be changed through Xbee
int float_Time = 1800; //Float Duration in seconds
bool marryPoppins = true;
const String xBeeID = "WB"; //xBee ID, change second letter to "B" and "C" for their respective stacks, see Readme for more
long cutAlt = 80000; //Default cutdown altitude in feet! Changeable via xBee.
boolean altCut = true;  //set to true to perfom an altitude cutdown. can be toggled through Xbee.



//=============================================================================================================================================
//=============================================================================================================================================

/*  Mega ADK pin connections:
     -------------------------------------------------------------------------------------------------------------------------
     Component                    | Pins used             | Notes

     xBee serial                  | D0-1                  | IMPORTANT- is hardware serial (controls xBee and hard serial line), cannot upload with xBee plugged in
     Fireburner                   | D8                    | High to this pin fires tungsten burner
     Data LED (BLUE)              | D3                    | "action" LED (Blue), tells us what the payload is doing
     SD                           | D4, D50-52            | 50-52 do not not have wires but they are used!
     SD LED (RED)                 | D5                    | "SD" LED (Red). Only on when the file is open in SD card
     Continutity Check OUTPUT     | D6                    | Outputs a voltatge for the continuity check
     razorcutter pin              | D7                    | High to this pin spins razor blade
     GPS serial                   | serial 1              | serial for GPS (pins 18 and 19 on the mega
     fix                          | D6                    | whether or not we have a GPS fix, must be used with copernicus GPS unit
     Tempread                     | D9                    | temperature sensor reading
     Accel I2C                    | SDA,SCL               | I2C communication for accelerometer (pins 20 and 21)

     -------------------------------------------------------------------------------------------------------------------------
*/

//~~~~~~~~~~~~~~~Pin Variables~~~~~~~~~~~~~~~
#define fireBurner 8      // Pin which opens the relay to fire. High = Fire!
#define razorCutter 7     // Pin which turns servo with razor blade. High = Fire! 
#define ledPin 3          //Pin which controls the DATA LED, which blinks differently depending on what payload is doing
#define chipSelect 4      //SD Card pin
#define ledSD 5           //Pin which controls the SD LED
#define fix_led 6         //led  which blinks for fix
#define TempPin    9      //temperature reading pin

//~~~~~~~~~~~~~~~Command Variables~~~~~~~~~~~~~~~
//variables for the altitude cutting command
boolean bacon = true;  //true for beacon updates
//~~~~~~~~~~~~~~~Timing Variables~~~~~~~~~~~~~~~
unsigned long beaconTimer= 0;
boolean burnerON = false;
unsigned long masterTimer = long(Master_Timer) * 1000;
unsigned long floatTimer = long(float_Time)* 1000;
unsigned long floatStart = 0;
boolean floating = false;
boolean recovery = false;
int altDelay = 5;
boolean delayBurn = false;
//blinnking variables
boolean LEDon = false;
//variables for LED fix blinking time
#define FixDelay 1000
#define noFixDelay 15000
//temperature sensor setup
OneWire oneWire(TempPin);                  //setup onewire bus for temp sensor
DallasTemperature TempSensors(&oneWire);   //declare the sensor
String Temperature = " ";              //the temperature
#define TEMPTIME 5000                      //how often the temperature should be read
class action {
  protected:
    unsigned long Time;
    String nam;
  public:
    String getName();
};
class Blink: public action {
  protected:
    int ondelay;
    int offdelay;
    int ontimes;
  public:
    friend void blinkMode();
    void BLINK();
    Blink();
    Blink(int on, int off, int times, String NAM, unsigned long tim);
    int getOnTimes();
};
class burnAction: public action {
  private:
    int ondelay;
    int offdelay;
    int ontimes;
    int stagger;
  public:
    void Burn();
    burnAction(int on, int off, int ont, int stag, unsigned long tim);
    int getOnTimes();
};


//main action LED blinking commands
Blink recoveryBlink = Blink(200, 2000, -1, "recoveryBlink", 0);
Blink countdownBlink = Blink(200, 850, -1, "countdownBlink", 0);
Blink* currentBlink = &countdownBlink;
burnAction idleBurn = burnAction(0, 0, -1, 200, 0);
burnAction* currentBurn = &idleBurn;

//GPS Stuff
//copernicus version
//Adafruit_GPS GPS(&Serial1); //Constructor for GPS object
TinyGPSPlus GPS;
int GPSstartTime;
boolean newDay = false;
boolean firstFix = false;
int days = 0;          //used to store previous altitude values to check for burst
boolean sliced = false;
boolean checkingCut = false;
boolean newData = false;
int checkTime;
//SD Stuff
File eventLog;
File GPSlog;
String Ename = "";
String GPSname = "";
boolean SDcard = true;

//Accelerometer Stuff
ADXL345 adxl = ADXL345();
boolean shift = false;
int x,y,z;

void setup() {
  // initialize pins
  pinMode(ledPin, OUTPUT);
  pinMode(fireBurner, OUTPUT);
  pinMode(razorCutter, OUTPUT);
  pinMode(ledSD, OUTPUT);
  pinMode(chipSelect, OUTPUT);    // this needs to be be declared as output for data logging to work
  pinMode(fix_led, OUTPUT);
  TempSensors.begin();               //set up temperature sensors
  
  //initiate GPS serial
   Serial1.begin(4800);    //

  

  // initiate xbee
  Serial.begin(9600);
  sendXBee("xBee begin");
  //Initiate GPS Data lines
  sendXBee("GPS begin");

  //GPS setup and config
  sendXBee("GPS configured");

  adxl.powerOn();
  adxl.setRangeSetting(16);
  adxl.setSpiBit(0);

  //initialize SD card
  while (!SD.begin(chipSelect)) {            //power LED will blink if no card is inserted
    Serial.println("No SD");
    digitalWrite(ledSD, HIGH);
    delay(500);
    digitalWrite(ledSD, LOW);
    delay(500);
    SDcard = false;
  }
  SDcard = true;
  sendXBee("Checking for existing file");
  //Check for existing event logs and creates a new one
  for (int i = 0; i < 100; i++) {
    if (!SD.exists("Elog" + String(i / 10) + String(i % 10))) {
      Ename = "Elog" + String(i / 10) + String(i % 10);
      openEventlog();
      break;
    }
  }

  sendXBee("event log created: " + Ename);

  //Same but for GPS
  for (int i = 0; i < 100; i++) {
    if (!SD.exists("GPS" + String(i / 10) + String(i % 10) + ".csv")) {
      GPSname = "GPS" + String(i / 10) + String(i % 10) + ".csv";
      openGPSlog();
      break;
    }
  }

  sendXBee("GPS log created: " + GPSname);

  /*  while (!eventLog) {                   //both power and data LEDs will blink together if card is inserted but file fails to be created                 /
        sendXBee("Eventlog file creation failed");
        digitalWrite(ledSD, HIGH);
        digitalWrite(ledPin, HIGH);
        delay(500);
        digitalWrite(ledSD, LOW);
        digitalWrite(ledPin, LOW);
        delay(500);
    }
    while(!GPSlog){
        sendXBee("GPS file creation failed");
        digitalWrite(ledSD, HIGH);
        digitalWrite(ledPin, HIGH);
        delay(1500);
        digitalWrite(ledSD, LOW);
        digitalWrite(ledPin, LOW);
        delay(1500);
    }*/
  digitalWrite(fireBurner, LOW); //sets burner to off just in case
  String GPSHeader = "Flight Time, Lat, Long, Altitude (ft), Date, Hour:Min:Sec, Fix, Temperature";
  GPSlog.println(GPSHeader);//set up GPS log format
  sendXBee("GPS header added");


  String eventLogHeader = "Time, Sent/Received, Command";
  eventLog.println(eventLogHeader);
  sendXBee("Eventlog header added");

  closeEventlog();
  closeGPSlog();
  

  sendXBee("Setup Complete");

}
void loop() {
  
  xBeeCommand(); //Checks for xBee commands
  updateGPS();   //updates the GPS
  autopilot();   //autopilot function that checks status and runs actions
}
