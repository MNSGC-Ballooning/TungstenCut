void addTime(int addition){
  burnDelay+=(addition*1000);
}

void removeTime(int subtraction){
  burnDelay -=(subtraction*1000);
}
void runBurn(){
  currentBlink= new Blink(200,500,5, "burnBlink", millis());
  recovery = true;
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
      else{
        checkingburst = false;
        altDelay = 0;
         
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

   checkBurst();
   blinkMode();
   burnMode();
   altTheSingleLadies();
   if((millis()>=burnDelay)&&!delayBurn){   //Check to see if timer has run out or if cut has been commanded and if it is not currenlty in a delayed burn
     runBurn();
     delayBurn=true;
     GPSaction("timed cutdown attempt");
   }
   contiCheck();
}

void altTheSingleLadies(){          //function which makes decisions based on altitude
  if(GPS.fix&&!bursted){
    
    if((GPS.altitude * 3.28048>= (cutAlt-3000))&&!gatePass){
      gatePass=true;
      prevAlt=GPS.altitude * 3.28048;
      sendXBee("Within 3000ft of Cutdown Altitude");
      logAction("Within 3000ft of Cutdown Altitude");
    }
    else if((GPS.altitude * 3.28048>= (cutAlt-3000))&&gatePass){
        
        if(!altCheck&&(millis()-altTimer >=1000)){    //if it's been 1 second 
          altTimer=millis();          //Reset timer
          altCheck=true;             //Do nothing in particular
          prevAlt=GPS.altitude * 3.28048;
          }
        if(altCheck&&(millis()-altTimer>=1000)){ //If it's been 1 second again...
          sendXBee("Verifying proximity to Cutdown Altitude");
          logAction("Verifying proximity to Cutdown Altitude");
          if((GPS.altitude * 3.28048)-prevAlt>= 200){
            sendXBee("GPS hits too far apart, resetting altitude decision-making");
            logAction("GPS hits too far apart, resetting altitude decision-making");
            gatePass=false; //Should stop if GPS hits are more than 200ft apart. 
          }
          altCheck=false;
      }
    }
     else if(GPS.altitude * 3.28048>=cutAlt&&gatePass){
      sendXBee("Activating GPS Altitude Triggered Cutdown");
      logAction("Activating GPS Altitude Triggered Cutdown");
      //runburn();
     
      
    }

    }   
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
  //if we are done with the burnblinking we will start the burn
  if(currentBlink->getName()=="burnBlink"&&currentBlink->getOnTimes()==0)
  {
    currentBurn = new burnAction(500,200,3,1000, millis());
  }
  //if done with the burn go back to idling
  if(currentBurn->getOnTimes()==0){
    digitalWrite(fireBurner, LOW);
    delete currentBurn;
    currentBurn = &idleBurn;
  }
  currentBurn->Burn();
} 
