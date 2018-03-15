void addTime(int addition){
  masterTimer +=(addition*1000);
}

void removeTime(int subtraction){
  masterTimer -=(subtraction*1000);
}
void runBurn(){
  currentBlink= new Blink(200,500,5, "burnBlink", millis());
  sendXBee("running burn");
  recovery = true;
}

void checkCut(){
  if(!sliced){
    if(GPS.Fix&&GPS.altitude.feet()!= 0 &&!checkingCut){   //
      checkAlt = (GPS.altitude.feet());
      checkingCut= true;
      checkTime= getLastGPS();
    }
    else if(GPS.Fix && GPS.altitude.feet()!= 0 &&altDelay<5&&(getLastGPS()-checkTime)>1){   //GPS.fix
      if((GPS.altitude.feet()-checkAlt)<-30){
        checkTime = getLastGPS();
        checkAlt =  (GPS.altitude.feet());                                 
        altDelay++;
      }
      else{
        checkingCut = false;
      }
     }
    else if(GPS.Fix && GPS.altitude.feet()!= 0 &&altDelay==5&&(getLastGPS()-checkTime>1)){    //GPS.fix
      if(checkAlt-(GPS.altitude.feet())>30){                                   // a five second difference greater than 100 feet(not absolute value, so it still rises)
        sendXBee("cut detected");
        logAction("cut detected");
        sliced = true;
      }
      else{
        checkingCut = false;
        altDelay = 0;
         
      }
    }
    else if(!GPS.Fix || GPS.altitude.feet() == 0 ){        //GPS.fix              //if no fix reset the whole process
      checkingCut = false;
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
   checkCut();
   blinkMode();
   burnMode();
   Fixblink();
   readTemp();
   if(bacon){
    beacon();
   }
   if(shift==false){
    detectShift(x,y,z);
   }
   if(altCut){
    altTheSingleLadies();
   }
   if(floating){
    deathScythe();
   }

   if((millis()>=masterTimer)&&!delayBurn&&judgementDay){   //Check to see if timer has run out or if cut 
     runBurn();                                                 //has been commanded and if it is not currenlty in a delayed burn, 
     delayBurn=true;                                            //or if it even was a delayed burn
     GPSaction("You are TERMINATED!");
   }
}


void altTheSingleLadies(){
  static bool cutCheck = false;
  static bool altset = false;
  static byte checkTimes = 0;
  static byte checkFloat = 0;
  static unsigned long prevAlt = 200000;
  static unsigned long altTimer = 0;
  static bool sent = false;
  if(GPS.Fix&&GPS.altitude.feet()!= 0 ){    //GPS.fix
    if((getLastGPS()-altTimer> 2) && !floating){
      if(GPS.altitude.feet()< prevAlt){
        sendXBee("checking for float: " + String(checkFloat));
        checkFloat++;
        prevAlt=GPS.altitude.feet();
        altTimer = getLastGPS();
      if(checkFloat>15){
        floating=true;
        floatStart=millis();
        sendXBee("Burst detected, float timer started");
      }
      }
      else if (checkFloat > 0){
        
        altTimer = getLastGPS();
        checkFloat = 0;
        sendXBee("checkfloat reset");
      }
    }
    if(floating==true){
        if(!altset && GPS.altitude.feet() != 0 && GPS.Fix && GPS.altitude.feet()<cutAlt){
          sendXBee("Burst occured early, setting altCut to 1000 feet below current altitude");
          cutAlt=GPS.altitude.feet()-1000;
          altset = true;
        }
      if(!cutCheck){
        prevAlt = GPS.altitude.feet();
        cutCheck = true;
        checkTimes = 0;
        altTimer = getLastGPS();
        sent = false;
      }
      else if((getLastGPS()-altTimer > 2)&& prevAlt-GPS.altitude.feet() > 3000){
        cutCheck = false;
        sendXBee("GPS jump detected, resetting cutdown decisions");
      }
      else if((getLastGPS()-altTimer> 2) && GPS.altitude.feet()-cutAlt<3000 && !sent ){
        sendXBee("within 3000 feet of cutdown altitude");
        sent = true;
        prevAlt = GPS.altitude.feet(); 
        altTimer = getLastGPS();
      }
      else if (checkTimes < 15 && getLastGPS()-altTimer > 2&& GPS.altitude.feet()<cutAlt){
        String toSend = "veryifying proximity to cutdown. will cut in " + String(14-checkTimes) + " GPS hits above cut altitude";
        sendXBee(toSend);
        checkTimes++;
        prevAlt = GPS.altitude.feet(); 
        altTimer = getLastGPS();
      }
      else if(checkTimes < 15 && checkTimes >2 && getLastGPS()-altTimer > 2 && GPS.altitude.feet() > cutAlt){
        sendXBee("GPS hit above cut altitude, resetting GPS hit counter");
        cutCheck = false;
        
        
      }
      else if(checkTimes == 15){
        sendXBee("running altitude burn");
        runBurn();
        floating=false;
        cutCheck = false;
      }
      
  
    else{
      cutCheck = false;
    }
   }
   
}
}

void deathScythe(){
  if((millis()-floatStart)>floatTimer){
    runBurn();
    floating=false;
  }
}

void detectShift(int x, int y, int z){
  static byte shiftCheck=0;
  static unsigned long currentTime=millis();
  static unsigned long prevTime=millis();
  if(x>19 | x<-26){
    currentTime=millis();
    if(z<20 && currentTime-prevTime>1000){
     shiftCheck++;
     prevTime=currentTime;
    }
  }
  if(shiftCheck>15){
    sendXBee("Orientation shift detected!");
    shift=true;
  }
}

void burnAction::Burn(){
  if(ontimes>0){
   if((millis()-Time>=offdelay)&&!burnerON){
    digitalWrite(fireBurner, HIGH);
    digitalWrite(razorCutter, HIGH);
    Time= millis();
    burnerON = true;
  }
  if(millis()-Time>=(ondelay+stagger*(3-ontimes))&&burnerON){
    digitalWrite(fireBurner, LOW);
    digitalWrite(razorCutter, LOW);
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
void readTemp(){
  static unsigned long Timer = 0;
  if(millis()-Timer>= TEMPTIME){
    TempSensors.requestTemperatures();
    Temperature = TempSensors.getTempCByIndex(0);
    Timer = millis();
  }
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
    if(GPS.Fix && GPS.altitude.feet()!= 0){
      toSend += (String(GPS.time.hour())+ "," + String(GPS.time.minute()) + "," + String(GPS.time.second()) + ","
      + String(GPS.location.lat()) + "," + String(GPS.location.lng()) + "," + String(GPS.altitude.feet()) +
      "," + String(0) + "," + Temperature + "," + ("Accel (x,y,z): " + String(x) + ", " + String(y) + ", " + String(z)));
      sendXBee(toSend);
      }
    else{
      toSend += (String(GPS.time.hour()) + "," + String(GPS.time.minute()) + "," + String(GPS.time.second()) + ","
      + "0" + "," + "0" + "," + "0" + String(0)+ "," + Temperature + ",") + ("Accel (x,y,z): " + String(x) + ", " + String(y) + ", " + String(z));
      sendXBee(toSend);
      
    }
    beaconTimer = millis();
  }
}

  
  

