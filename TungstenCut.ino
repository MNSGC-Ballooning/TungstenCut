
//Libraries
#include <SD.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
//==============================================================
//               Code For Tungsten Cutter
//                 Danny Toth May/June 2017 - tothx051
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

                             long cutAlt = 100000; //Default cutdown altitude in feet! Changeable via xBee.

//=============================================================================================================================================
//=============================================================================================================================================

/*  Mega ADK pin connections:
     -------------------------------------------------------------------------------------------------------------------------
     Component                    | Pins used             | Notes

     xBee serial                  | D0-1                  | IMPORTANT- is hardware serial (controls xBee and hard serial line), cannot upload with xBee plugged in
     Fireburner                   | D2                    | 
     Data LED (BLUE)              | D3                    |  "action" LED (Blue), tells us what the payload is doing
     SD                           | D4, D11-13            |  11-13 not not have wires but they are used!
     SD LED (RED)                 | D5                    | "SD" LED (Red). Only on when the file is open in SD card
     GPS serial                   | serial 1              | serial for GPS
     -------------------------------------------------------------------------------------------------------------------------
*/

//~~~~~~~~~~~~~~~Pin Variables~~~~~~~~~~~~~~~
#define ledPin 3          //Pin which controls the DATA LED, which blinks differently depending on what payload is doing

#define fireBurner 2       // Pin which opens the relay to fire. High = Fire!

#define ledSD 5            //Pin which controls the SD LED

#define chipSelect 4      //SD Card pin

//~~~~~~~~~~~~~~~Command Variables~~~~~~~~~~~~~~~                 //int used for 'if navigation'
boolean gatePass;                   //Stores whether or not altitude gate has been passed
unsigned long prevAlt;                      //Used in altitude cutdown decision

//~~~~~~~~~~~~~~~Timing Variables~~~~~~~~~~~~~~~                        
boolean altCheck;   
unsigned long altTimer=0;                                          //stores whether burn has been attempted
boolean burnerON = false;                                       //loop maneuvering variable (1 if cutter will cut, 0 if timer countdown)    
unsigned long burnDelay = long(burn_Delay)*1000;                        //Used in recovery mode as the countdown to c                         //^that             
boolean recovery = false;         //tells whether revovery mode is on 
int altDelay = 5;                  // time between checking for a burst in seconds
boolean delayBurn = false;         //tells whether or not the timer burn has occured
//xBee Stuff
const String xBeeID = "W1"; //xBee ID
//blinnking variables
boolean LEDon = false;  

class action{
  protected:
    unsigned long Time;
    String nam;
   public:
    String getName();
};
class Blink:public action{
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
class burnAction:public action{
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


  
Blink recoveryBlink = Blink(200,2000,-1, "recoveryBlink",0);
Blink countdownBlink = Blink(200,850,-1, "countdownBlink",0);
Blink* currentBlink = &countdownBlink;
burnAction idleBurn = burnAction(0,0,-1, 200,0);
burnAction* currentBurn = &idleBurn;

//GPS Stuff
Adafruit_GPS GPS(&Serial1); //Constructor for GPS object
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

void setup() {
 // initialize pins
  pinMode(ledPin, OUTPUT);
  pinMode(fireBurner, OUTPUT);
  pinMode(ledSD, OUTPUT);
  pinMode(chipSelect, OUTPUT);    // this needs to be be declared as output for data logging to work
  sendXBee("Pins Initialized");
  
// initiate xbee
  Serial.begin(9600);
  sendXBee("xBee begin");
  
//Initiate GPS Data lines
  GPS.begin(9600);
  sendXBee("GPS begin");
 
//GPS setup and config
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  sendXBee("GPS configured");

  //initialize SD card
  while (!SD.begin(chipSelect)) {            //power LED will blink if no card is inserted
      Serial.println("No SD");
      digitalWrite(ledSD, HIGH);
      delay(500);
      digitalWrite(ledSD, LOW);
      delay(500);
  }
  sendXBee("Checking for existing file");
  //Check for existing event logs and creates a new one
   for(int i=0;i<100;i++){
    if(!SD.exists("Elog" + String(i/10) + String(i%10))){
       Ename = "Elog" + String(i/10) + String(i%10);
       openEventlog();
       break;
       }
    }
    
   sendXBee("event log created: " + Ename);
     
   //Same but for GPS
   for(int i=0;i<100;i++){
    if(!SD.exists("GPS" + String(i/10) + String(i%10) + ".csv")){
       GPSname = "GPS" + String(i/10) + String(i%10) + ".csv";
       openGPSlog();
       break;
       }
    }

  sendXBee("GPS log created: " + GPSname);

  while (!eventLog) {                   //both power and data LEDs will blink together if card is inserted but file fails to be created                 /
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
  }
  digitalWrite(fireBurner, LOW); //sets burner to off just in case
  String GPSHeader = "Flight Time, Lat, Long, Altitude (ft), Date, Hour:Min:Sec, Fix,";
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
