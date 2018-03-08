
//Libraries
//this requires a special modded version of the TinyGPS library because for whatever
//reason the TinyGPS library does not include a "Fix" variable. the library can be found here:
//https://github.com/simonpeterson/TinyGPS/tree/master
#include <SPI.h>
#include <SD.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
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

int burn_Delay = 3600; //Countdown timer in seconds! Changeable via xBee.
int float_Time = 1800; //Float Duration in seconds
bool timeBurn = false;   //set to true to activate delay burns. can be changed through Xbee
const String xBeeID = "WB"; //xBee ID, change second letter to "B" and "C" for their respective stacks, see Readme for more
long cutAlt = 90000; //Default cutdown altitude in feet! Changeable via xBee.
boolean altCut = false;  //set to true to perfom an altitude cutdown. can be toggled through Xbee.
boolean floatCut = false;
boolean floatEnabled = true;
//=============================================================================================================================================
//=============================================================================================================================================

/*  Mega ADK pin connections:
     -------------------------------------------------------------------------------------------------------------------------
     Component                    | Pins used             | Notes

     xBee serial                  | D0-1                  | IMPORTANT- is hardware serial (controls xBee and hard serial line), cannot upload with xBee plugged in
     Fireburner                   | D8                    |
     Data LED (BLUE)              | D3                    |  "action" LED (Blue), tells us what the payload is doing
     SD                           | D4, D11-13            |  11-13 not not have wires but they are used!
     SD LED (RED)                 | D5                    | "SD" LED (Red). Only on when the file is open in SD card
     Continutity Check OUTPUT     | D6                    | Outputs a voltatge for the continuity check
     razorcutter pin              | D7                    | Reads the voltage for the continuity check
     GPS serial                   | serial 1              | serial for GPS (pins 18 and 19 on the mega
     fix                          | D8                    | whether or not we have a GPS fix, must be used with copernicus GPS unit
     -------------------------------------------------------------------------------------------------------------------------
*/

//~~~~~~~~~~~~~~~Pin Variables~~~~~~~~~~~~~~~
#define fireBurner 8      // Pin which opens the relay to fire. High = Fire!
#define razorCutter 7     // Pin which turns servo with razor blade. High = Fire! 
#define fireBurnerDos 2   // Pin which opens the second relay to fire. High = Fire!
#define razorCutterDos 9  // Pin which turns 2nd servo with razor blade. High = Fire!
#define ledPin 3          //Pin which controls the DATA LED, which blinks differently depending on what payload is doing
#define chipSelect 4      //SD Card pin
#define ledSD 5               //Pin which controls the SD LED
#define fix_led  
const int balloon_1_releaseCheck_out= 22; // Outputs high signal for cut check on balloon 1 by input pin 23          
const int balloon_1_releaseCheck_in=  23; // Reads signal from pin 22 to check for balloon 1 release. Low = Balloon 1 released!
const int balloon_2_releaseCheck_out= 48; // Outputs high signal for release check on balloon 2 by input pin 49  
const int balloon_2_releaseCheck_in=  49; // Reads siganl from pin 48 ro check for balloon 2 release. Low = Balloon 2 released!
//~~~~~~~~~~~~~~~Command Variables~~~~~~~~~~~~~~~
//variables for the altitude cutting command
boolean bacon = true;  //true for beacon updates
//~~~~~~~~~~~~~~~Timing Variables~~~~~~~~~~~~~~~
unsigned long beaconTimer= 0;
boolean burnerON = false;
unsigned long burnDelay = long(burn_Delay) * 1000;
unsigned long floatTimer = long(float_Time)* 1000;
unsigned long floatStart = 0;
boolean secondBurn = false;
boolean recovery = false;
int altDelay = 5;
boolean delayBurn = false;
//blinnking variables
boolean LEDon = false;

//~~~~~~~~~Release Check~~~~~~~~~~~//
boolean check1 = false;
boolean check2 = false;

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
boolean bursted = false;
boolean checkingburst = false;
boolean newData = false;
int checkTime;
//SD Stuff
File eventLog;
File GPSlog;
String Ename = "";
String GPSname = "";
boolean SDcard = true;

void setup() {
  // initialize pins
  pinMode(ledPin, OUTPUT);
  pinMode(fireBurner, OUTPUT);
  pinMode(razorCutter, OUTPUT);
  pinMode(fireBurnerDos, OUTPUT);
  pinMode(razorCutterDos, OUTPUT);
  pinMode(ledSD, OUTPUT);
  pinMode(chipSelect, OUTPUT);    // this needs to be be declared as output for data logging to work
  //initiate GPS serial
   Serial1.begin(4800);    //
  pinMode(balloon_1_releaseCheck_out, OUTPUT);// Balloon 1 release check pins
  pinMode(balloon_1_releaseCheck_in, INPUT);
  pinMode(balloon_2_releaseCheck_out, OUTPUT); // Balloon 2 release check pins
  pinMode(balloon_2_releaseCheck_in, INPUT);
  

  // initiate xbee
  Serial.begin(9600);
  sendXBee("xBee begin");
  //Initiate GPS Data lines
  sendXBee("GPS begin");

  //GPS setup and config
  sendXBee("GPS configured");

  //initialize SD card
  if (!SD.begin(chipSelect)) {            //power LED will blink if no card is inserted
    sendXBee("No SD");
    digitalWrite(ledSD, HIGH);
    delay(500);
    digitalWrite(ledSD, LOW);
    delay(500);
    SDcard = false;
  }
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
  String GPSHeader = "Flight Time, Lat, Long, Altitude (ft), Date, Hour:Min:Sec, Fix,";
  GPSlog.println(GPSHeader);//set up GPS log format
  sendXBee("GPS header added");

  //Set up balloon release checks
  digitalWrite(balloon_1_releaseCheck_in, HIGH);
  digitalWrite(balloon_1_releaseCheck_out, LOW);
  digitalWrite(balloon_2_releaseCheck_in, HIGH);
  digitalWrite(balloon_2_releaseCheck_out, LOW);
  

  String eventLogHeader = "Time, Sent/Received, Command";
  eventLog.println(eventLogHeader);
  sendXBee("Eventlog header added");

  closeEventlog();
  closeGPSlog();
  
  if(digitalRead(balloon_1_releaseCheck_in)==LOW){
    sendXBee("Balloon release check 1 set up properly");
  }

  if(digitalRead(balloon_2_releaseCheck_in)==LOW){
    sendXBee("Balloon release check 2 set up properly");
  }
  sendXBee("Setup Complete");

}
void loop() {
  
  xBeeCommand(); //Checks for xBee commands
  updateGPS();   //updates the GPS
  autopilot();   //autopilot function that checks status and runs actions
}
