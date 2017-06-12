void addTime(int addition){
  burnDelay+=(addition*1000);
}

void removeTime(int subtraction){
  burnDelay -=(subtraction*1000);
}
void runBurn(){
  currentBlink= new Blink(200,500,5, "burnBlink", millis());
}

void checkBurst(){
  if(!bursted){
    if(GPS.fix&&!checkingburst){
      checkAlt = (GPS.altitude*3.28048);
      checkingburst= true;
      checkTime= getLastGPS();
    }
    else if(newData&&altDelay<5&&(getLastGPS()-checkTime)<2000){                //is there good new data, is it part of the 5 seccond period, and is this good new data coming in with 2 seconds of the other data
      checkTime = getLastGPS();
      altDelay++;
      }
    else if(newData&&altDelay==5&&(getLastGPS()-checkTime)<2000){
      if(checkAlt-(GPS.altitude*3.28048)>100){                                   // a five second difference greater than 100 feet(not absolute value, so it still rises)
        sendXBee("burst detected");
        logAction("burst detected");
       bursted = true;
      }
      }
    else if(!GPS.fix){                   //if no fix reset the whole process
      checkingburst = false;
      altDelay = 0;
    }
   }
}
void contiCheck(){
}

void autopilot(){
<<<<<<< HEAD
   if(testblink){
    testBlink();
   }
   if(!burnAttempt){  //Blinks LED every second to convey normal flight operation (countdown)
      countdownBlink();
      altTheSingleLadies();
    }
=======
   checkBurst();
   blinkMode();
   burnMode();
>>>>>>> classsBlink

    if((millis()>=burnDelay)&&!delayBurn){   //Check to see if timer has run out or if cut has been commanded and if it is not currenlty in a delayed burn
      runBurn();
      delayBurn=true;
      GPSaction("timed cutdown attempt");
    }
    contiCheck();
    //...........................Firing Burner.......................  
<<<<<<< HEAD
   
    if((cutNow)){
        flamingGuillotine();        //Cutdown Function. The name was relevant to whatever conversation we were having at the time...
        contiCheck();               //Temporary Continuity check. Currently returns positive-cut every time.
    }
    //...............................................................
    //=======================Recovery Mode============================  
    if(burnSuccess){
      //- - - - - - - - - - - - - - Case Successful Cut - - - - - - - - - - - - - -
       recoveryBlink();
        //More stuff can go here. 
     //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
     //_ _ _ _ _ _ _ _ _ _ _ _ _ Case Unsuccessful Cut_ _ _ _ _ _ _ _ __ _ _ _ _ _
      /*
      if(!burnSuccess){                                                              //To be uncommented when continuity check is implemented.
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

void altTheSingleLadies(){          //function which makes decisions based on altitude
  if(GPS.fix){
    
    if((GPS.altitude * 3.28048>= (cutAlt-3000))&&gatePass==false){
      gatePass=true;
      prevAlt=GPS.altitude * 3.28048;
      sendXBee("Within 3000ft of Cutdown Altitude");
      logAction("Within 3000ft of Cutdown Altitude");
    }
    else if((GPS.altitude * 3.28048>= (cutAlt-3000))&&gatePass==true){
        
        if(!altCheck&&(millis()-altTimer >=1000)){    //if it's been 1 second 
          altTimer=millis();          //Reset timer
          altCheck=true;             //Do nothing in particular
          prevAlt=GPS.altitude * 3.28048;
          }
        if(altCheck&&(millis()-timerLED>=1000)){ //If it's been 1 second again...
          sendXBee("Verifying proximity to Cutdown Altitude");
          logAction("Verifying proximity to Cutdown Altitude");
          if((GPS.altitude * 3.28048)-prevAlt>= 200){
            sendXBee("GPS hits too far apart, resetting altitude decision-making");
            logAction("GPS hits too far apart, resetting altitude decision-making");
            gatePass=false; //Should stop if GPS hits are more than 200ft apart. 
          }
          altCheck=false;
      }

     else if(GPS.altitude * 3.28048>=cutAlt&&gatePass){
      sendXBee("Activating GPS Altitude Triggered Cutdown");
      logAction("Activating GPS Altitude Triggered Cutdown");
      //runburn();
     }
      
    }
=======
    //for now, conticcheck disables Cutnow, so burncurrent lets us know if we are attempting a burn

}
void burnAction::Burn(){
  if(ontimes>0){
   if((millis()-Time>=offdelay)&&!burnerON){
    digitalWrite(fireBurner, HIGH);
    Time= millis();
    burnerON = true;
  }
  if(millis()-Time>=(ondelay+stagger*(3-ontimes))&&burnerON){
    digitalWrite(fireBurner, LOW);
    burnerON = false;
    Time = millis(); 
    ontimes--;
>>>>>>> classsBlink
  }
}
}
int burnAction::getOnTimes(){
  return ontimes;
}
burnAction::burnAction(int on, int off, int ont, int stag, unsigned long tim){
  ondelay = on;
  offdelay = off;
  ontimes = ont;
  stagger = stag;
  Time = tim;
}
void burnMode(){
  if(currentBlink->getName()=="burnBlink"&&currentBlink->getOnTimes()==0)
  {
    currentBurn = new burnAction(500,200,3,1000, millis());
  }
  if(currentBurn->getOnTimes()==0){
    digitalWrite(fireBurner, LOW);
    delete currentBurn;
    currentBurn = &idleBurn;
  }
  currentBurn->Burn();
}
