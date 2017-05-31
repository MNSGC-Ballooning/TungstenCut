
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

                                         int burn_Delay = 3600; //Countdown timer in seconds!
                                          //Default is 60m, some modules fly on 70m (4200s)

//=============================================================================================================================================
//=============================================================================================================================================

/*  Mega ADK pin connections:
     -------------------------------------------------------------------------------------------------------------------------
     Component                    | Pins used             | Notes

     Xbee serial                  | D0-1                  | IMPORTANT- is hardware serial, cannot upload with Xbee plugged in
     Fireburner                   | D2                    | 
     Data LED (BLUE)              | D3                    |  "action" LED, tells us what the payload is doing
     SD                           | D4, D11-13            |  11-13 not not have wires but they are used!
     SD LED (RED)                 | D5                    | only on when the file is openn in SD card
     GPS serial                   | 8,9 (Rx, Tx)          | serial for GPS
     Arrow Actuator               | D2-3, D15 (A1)        | Set D3 high to exend, D2 high to retract. D15/A1 is feedback
     -------------------------------------------------------------------------------------------------------------------------
*/

//~~~~~~~~~~~~~~~Pin Variables~~~~~~~~~~~~~~~
#define ledPin 3          //Pin which controls the DATA LED, which blinks differently depending on what payload is doing

#define fireBurner 2       // Pin which opens the relay to fire. High = Fire!

#define ledSD 5            //Pin which controls the SD LED

#define chipSelect 4      //SD Card pin

/*
 * pins 8,9 are Rx and Tx for the GPS
 */

//~~~~~~~~~~~~~~~Command Variables~~~~~~~~~~~~~~~
int first = 1;                          //int used for 'if navigation'
boolean burnAttempt = false;           //stores whether burn has been attempted
int cutNow=0;                         //loop maneuvering variable (1 if cutter will cut, 0 if timer countdown)
boolean burnSuccess=false;          //Stores whether burn was successful

//~~~~~~~~~~~~~~~Timing Variables~~~~~~~~~~~~~~~
unsigned long burnDelay = long(burn_Delay)*1000;   //a burnDelay in milliseconds, which will be the primary currency from here on out.
unsigned long timer;                        //Used in recovery mode as the countdown to cut reattempt
long timerLED=0;                           //This should be obvious, but it's used for LED blinky-blinky
boolean LEDon = false;                    //^that





//xBee Stuff
const String xBeeID = "W1"; //xBee ID


//GPS Stuff
SoftwareSerial gpsSerial(8,9);
Adafruit_GPS GPS(&gpsSerial); //Constructor for GPS object
int GPSstartTime;
int days = 0;
boolean newDay = false;
boolean firstFix = false;



//SD Stuff
File eventlog;
String filename = "";




void setup() {
 // initialize pins
  pinMode(ledPin, OUTPUT);
  pinMode(fireBurner, OUTPUT);
  pinMode(ledSD, OUTPUT);
  pinMode(chipSelect, OUTPUT);    // this needs to be be declared as output for data logging to work



  Serial.println("Pins Initialized");


//Initiate xBee Data lines
  Serial.begin(9600);

Serial.println("xBee begin");


//Initiate GPS Data lines
  GPS.begin(9600);
  gpsSerial.begin(9600);

  Serial.println("GPS begin");



//GPS setup and config
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);

Serial.println("GPS config");


  //initialize SD card
  while (!SD.begin(chipSelect)) {            //power LED will blink if no card is inserted
      Serial.println("No SD");
      digitalWrite(ledSD, HIGH);
      delay(500);
      digitalWrite(ledSD, LOW);
      delay(500);
    }

  Serial.println("Checking for existing file");

  
   for(int i=0;i<100;i++){
    if(!SD.exists("WCut" + String(i/10) + String(i%10))){
       filename = "WCut" + String(i/10) + String(i%10);
       openEventlog();
       break;
       }
    }

Serial.println("filename created: " + filename);

 
  while (!eventlog) {                   //both power and data LEDs will blink together if card is inserted but file fails to be created
      
Serial.println("file creation failed");

      
      digitalWrite(ledSD, HIGH);
      digitalWrite(ledPin, HIGH);
      delay(500);
      digitalWrite(ledSD, LOW);
      digitalWrite(ledPin, LOW);
      delay(500);
    }



  digitalWrite(fireBurner, LOW); //sets burner to off just in case

  /*/####################Startup#####################
    for(int i=0;i<7;i++){          //Blinks blue LED 7 times separated by .3 seconds to inicate "hello"
      digitalWrite(ledPin, HIGH);
      delay(300);
      digitalWrite(ledPin, LOW);
      delay(300);
      }
    delay(2000);
      /*for(int i=0;i<(burnDelay-(burnDelay%30000))/30000;i++){    //Blinks blue LED once for every 5 minutes of burn delay
        digitalWrite(ledPin, HIGH);
        delay(100);
        digitalWrite(ledPin, LOW);
        delay(200);
        }*/

 String Header = "Flight Time, Lat, Long, Altitude (ft), Date, Hour:Min:Sec, Fix,";
  eventlog.println(Header);//set upeventlog format
    

  Serial.println("Eventlog header added");

   
  closeEventlog();

  sendXBee("Setup Complete");

}

void loop() {

    xBeeCommand(); //Checks for xBee commands

 
    if(!burnAttempt){  //Blinks LED every second to convey normal flight operation (countdown)
      countdownBlink();
      updateGPS();
    }

    if((!cutNow)&&(millis()>=burnDelay)){   //Check to see if timer has run out or if cut has been commanded
      cutNow=1;
      Serial.println("Cutdown initiated");

    }

    //...........................Firing Burner.......................  
   
    if((cutNow)){

        flamingGuillotine();
        contiCheck();
    }
    //...............................................................

//=======================Recovery Mode============================  
    if(burnSuccess){
      //- - - - - - - - - - - - - - Case Successful Cut - - - - - - - - - - - - - -
        recoveryBlink();
       Serial.println("Recovery");
        //More stuff can go here. 
     //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
     //_ _ _ _ _ _ _ _ _ _ _ _ _ Case Unsuccessful Cut_ _ _ _ _ _ _ _ __ _ _ _ _ _
      /*
      if(!burnSuccess){
        for(int i=0;i<3;i++){         //blinks LED 3 times long to indicate retry
          digitalWrite(ledPin, HIGH);
          delay(1000);
          digitalWrite(ledPin, LOW);
          delay(300);
        }
        cutNow=1; //orders another cutdown
      }
      */

      
  }
}
