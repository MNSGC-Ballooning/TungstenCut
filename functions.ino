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
    else if(GPS.fix&&altDelay<5&&(getLastGPS()-checkTime)>1){
      if((GPS.altitude*3.28048-checkAlt)<-30){
        checkTime = getLastGPS();
        checkAlt =  (GPS.altitude*3.28048);                                 
        altDelay++;
      }
      else{
        checkingburst = false;
      }
     }
    else if(GPS.fix&&altDelay==5&&(getLastGPS()-checkTime>1)){
      if(checkAlt-(GPS.altitude*3.28048)>30){                                   // a five second difference greater than 100 feet(not absolute value, so it still rises)
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
/*void contiCheck(){
  if(!recovery){
    if(digitalRead(CONTIN) == LOW){
      recovery = true;
      logAction("main line detachment detected");
      sendXBee("main line detachment detected");
    }
  }
}*/

void autopilot(){
   checkBurst();
   blinkMode();
   burnMode();
   if(bacon){
    beacon();
   }
   if(altCut){
    altTheSingleLadies();
   }
   if(floatCut){
    hoverfloat();
   }
   if((millis()>=burnDelay)&&!delayBurn&&timeBurn&&timeBurn){   //Check to see if timer has run out or if cut 
     runBurn();                                       //has been commanded and if it is not currenlty in a delayed burn, 
     delayBurn=true;                                  //or if we even was a delayed burn
     GPSaction("timed cutdown attempt");
   }
   //contiCheck();
}

/*void altTheSingleLadies(){          //function which makes decisions based on altitude
  if(GPS.fix){
    
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
          //sendXBee("beginning altitude verification");               //we dont want to spam ourselves
          //logAction("beginning altitude verification");
          }
        if(altCheck&&(millis()-altTimer>=1000)){ //If it's been 1 second again...
          //sendXBee("Verifying proximity to Cutdown Altitude");     we dont want to spam ourselves
          //logAction("Verifying proximity to Cutdown Altitude");
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
      runBurn();
     
      
    }

    }   
    else{
      gatePass = false;
    }
}*/

void altTheSingleLadies(){
  static bool cutCheck = false;
  static byte checkTimes = 0;
  static unsigned long prevAlt = 0;
  static unsigned long altTimer = 0;
  static bool sent = false;
  if(GPS.fix){
    if(!cutCheck){
      prevAlt = GPS.altitude * 3.28048;
      cutCheck = true;
      checkTimes = 0;
      altTimer = getLastGPS();
      sent = false;
    }
    else if((getLastGPS()-altTimer > 2)&& GPS.altitude * 3.28048-prevAlt > 3000){
      cutCheck = false;
      sendXBee("GPS jump detected, resetting cutdown decisions");
    }
    else if((getLastGPS()-altTimer> 2) && cutAlt-GPS.altitude * 3.28048<3000 && !sent ){
      sendXBee("within 3000 feet of cutdown altitude");
      sent = true;
      prevAlt = GPS.altitude * 3.28048; 
      altTimer = getLastGPS();
    }
    else if (checkTimes < 15 && getLastGPS()-altTimer > 2&& GPS.altitude * 3.28048>cutAlt){
      String toSend = "veryifying proximity to cutdown. will cut in " + String(14-checkTimes) + " GPS hits above cut altitude";
      sendXBee(toSend);
      checkTimes++;
      prevAlt = GPS.altitude * 3.28048; 
      altTimer = getLastGPS();
    }
    else if(checkTimes < 15 && checkTimes >2 && getLastGPS()-altTimer > 2 && GPS.altitude * 3.28048 < cutAlt){
      sendXBee("GPS hit below cut altitude, resetting GPS hit counter");
      cutCheck = false;
      
      
    }
    else if (checkTimes == 15){
      sendXBee("running altitude burn");
      runBurn();
      if(floatEnabled){
        floatCut = true;
        floatStart = millis();
      }
      
      cutCheck = false;
    }
    
  }
  else{
    cutCheck = false;
  }
   
}

void hoverfloat(){
  sendXBee("Float timer started");
  if((millis()-floatStart)>=floatTimer){
    secondBurn = true;
  }
}
void burnAction::Burn(){
  if(ontimes>0){
   if((millis()-Time>=offdelay)&&!burnerON){
    if(floatEnabled){
      if(secondBurn){
        digitalWrite(fireBurnerDos, HIGH);
      }
      else{
        digitalWrite(fireBurner, HIGH);
      }
    }
    else{
      digitalWrite(fireBurner, HIGH);
      digitalWrite(fireBurnerDos, HIGH);
    }
    Time= millis();
    burnerON = true;
  }
  if(millis()-Time>=(ondelay+stagger*(3-ontimes))&&burnerON){
    if(floatEnabled){
      if(secondBurn){
    
        digitalWrite(fireBurnerDos, LOW);
      }
      else{
        digitalWrite(fireBurner, LOW);
      }
    }
    else{
      digitalWrite(fireBurner, LOW);
      digitalWrite(fireBurnerDos, LOW);
    }
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
    currentBurn = new burnAction(10000,200,3,1000, millis());
  }
  //if done with the burn go back to idling
  if(currentBurn->getOnTimes()==0){
    digitalWrite(fireBurner, LOW);
    delete currentBurn;
    currentBurn = &idleBurn;
  }
  currentBurn->Burn();
} 
void beacon(){
  if(millis()-beaconTimer>10000){ //if 10 seconds have passed
    String toSend = "";
    if(GPS.fix)
      xBee.sendGPS(GPS.hour, GPS.minute, GPS.seconds, GPS.latitudeDegrees, GPS.longitudeDegrees, GPS.altitude, GPS.satellites);
    else
      xBee.sendGPS(0,0,0,0,0,0,0);
    beaconTimer = millis();
  }
}

