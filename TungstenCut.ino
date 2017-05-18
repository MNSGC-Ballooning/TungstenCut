
#include <SoftwareSerial.h>

//==============================================================
//               Code For Tungsten Cutter
//                 Danny Toth May/June 2017 - tothx051
//==============================================================
 
//Version Description: Includes improved less-intrusive timer/LED's, and preliminary code for later xbee implementation

// Use: When payload is powered (i.e. batteries plugged in and switch in "on" position), it will be in flight mode.
//     
//     Flight Mode:
//                 Payload will count up in milliseconds from zero until a specified time. Pushing the button will cause the payload to 
//                 display the time remaining. One blink equals 5 minutes remaining. Once specified time has passed, payload will then 
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



//~~~~~~~~~~~~~~~Pin Variables~~~~~~~~~~~~~~~
int ledPin = 13; //Pin which controls the DATA LED, which blinks differently depending on what payload is doing

int powerLEDPin = 11; //Pin which constantly blinks steadily when payload is on

int fireBurner = 4;// Pin which opens the relay to fire. High = Fire!


//~~~~~~~~~~~~~~~Command Variables~~~~~~~~~~~~~~~
int first = 1; //int used for 'if navigation'
boolean burnAttempt = false; //stores whether burn has been attempted
int cutNow=0; //loop maneuvering variable (1 if cutter will cut, 0 if timer countdown)
int countThree=0; //Int used to blink 3 times during recovery mode
boolean burnSuccess=false;  //Stores whether burn was successful

//~~~~~~~~~~~~~~~Timing Variables~~~~~~~~~~~~~~~
unsigned long burnDelay = burn_Delay*1000; //a burnDelay in milliseconds, which will be the primary currency from here on out.
unsigned long timer;                       //Used in recovery mode as the countdown to cut reattempt
long timerLED=0;                           //This should be obvious, but it's used for LED blinky-blinky
boolean LEDon = false;                    //^that

SoftwareSerial xBee(2,3); //RX, TX
const String xBeeID = "W1"; //xBee ID






void setup() {
  // initialize pins.
  pinMode(ledPin, OUTPUT);
  pinMode(fireBurner, OUTPUT);
  pinMode(powerLEDPin, OUTPUT);

  xBee.begin(9600);
  Serial.begin(9600);


  digitalWrite(powerLEDPin, HIGH); //The Green/red LED should remain on the entire time the payload is still powered.
  digitalWrite(fireBurner, LOW); //sets burner to off just in case

  //####################Startup#####################
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
}


void loop() {

    xBeeCommand(); //Checks for xBee commands

    if(!burnAttempt){  //Blinks LED every second to convey normal flight operation (countdown)
      countdownBlink();
    }

    if((cutNow==0)&&(millis()>=burnDelay)){   //Check to see if timer has run out or if cut has been commanded
      cutNow=1;
    }

    //...........................Firing Burner.......................  
   
    if((cutNow==1)){
      for(int j=0;j<5;j++){               //LED blinks rapidly before firing burner
      digitalWrite(ledPin, HIGH); 
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200);
      }
        for(int l=0;l<3;l++){               //The actual burner activation.  It will attempt burn 3 times.
          digitalWrite(fireBurner, HIGH);
          delay(800+l*200);                 //Each burn is longer than the last.
          digitalWrite(fireBurner, LOW);
        }
      
      burnAttempt=true;    //these two will happen every time for loop navigation
      
      
      
      //Continuity check to decide burn or no burn
      //Continuity check will decide whether to set burnSuccess to true or to order another cutNow=1
      //For now, let's assume every burn works every time
      burnSuccess=1;
      cutNow=0;
      
      
      
    }
    //...............................................................

//=======================Recovery Mode============================  
    else{
      //- - - - - - - - - - - - - - Case Successful Cut - - - - - - - - - - - - - - 
      if(burnSuccess){
        recoveryBlink();
       

        //More stuff can go here. 
        
     }
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
  /*
    
    
  
    //=======================Recovery Mode============================  
    else if(burn != 0 && burnDelay<=0){
      digitalWrite(fireBurner, HIGH);     //Absolutely decimates burner circuit JUST IN CASE
      delay(1000);
      digitalWrite(fireBurner, LOW);

      int recoveryDelay = 100;
    
      while(recoveryDelay>0){                           //blink-blink....wait....blink-blink....wait..... Burn every 400 seconds
        digitalWrite(ledPin, HIGH);       //Payload waits for its knight in shining armor to rescue it from itself
        delay(300);                       //Payload waits, alone and wet in a muddy field somewhere
        digitalWrite(ledPin, LOW);
        delay(300);                       //              all alone.
        digitalWrite(ledPin, HIGH);
        delay(300);                       //                                Its batteries slowly dying.
        digitalWrite(ledPin, LOW);        //                                                              It hopes it performed to your expectations.
        delay(4000);                      //          
        recoveryDelay--;        
      }
      cut=1;
    }
  }
}
*/
//The graveyard (old code relics I think I might need later)
//int flightMode = 8; //Pin which is low when in config mode and high when in flight mode
//bool inFlight; //Variable which saves whether or not payload is flying
//int contiTest = 6; //Pin which checks for burner continuity. Currently unused.
//int stimer = 0;
//
//nowTime=millis();
//if((nowTime-prevTime)>=1500)
//{
//  prevTime=nowTime;
//  //action
//}

/*
    if (digitalRead(buttonPress)==LOW){
      nowTime=millis();
      showTime=1;
    }
      if((nowTime-prevTime)>=15)
      {
          prevTime=nowTime;
          showTime
          for(int i=0;i<((burnDelay-burnDelay%300)/300);i++){    //Blinks blue LED once for every 5 minutes of burn delay
            digitalWrite(ledPin, HIGH);
            delay(500);
            digitalWrite(ledPin, LOW);
            delay(200);
          }
       }
      

      delay(100);
      
    }

    //This is where continuity check would go. IF I HAD ONE.
    */
