void addTime(int addition){
  burnDelay+=(addition*1000);
}

void removeTime(int subtraction){
  burnDelay -=(subtraction*1000);
}
void runBurn(){
  currentBlink= new Blink(200,500,5, "burnBlink", millis());
}

/*void flamingGuillotine(){       
  //Cutdown. Blinks, burns etc.
      if(current->getOnTimes()>=5&&current->getOnTimes()<8){      // we will run the burner 3 times (ontimes gets to 5 through burnBlink)
        if(((millis()-burnBlinkTime)>=(800+long((current->getOnTimes-4)*200)))&&burning){      //for increasingly long times, turn on the burner
          digitalWrite(fireBurner, LOW);
          burning = false;                //the burning circuit is open
          ontimes++;                      //counting how many times we have run
          burnBlinkTime = millis();
        }
        if(((millis()-burnBlinkTime)>=200)&&!burning){
          digitalWrite(fireBurner, HIGH);
          burning = true;
          burnBlinkTime= millis();
        }
        }
      else if(ontimes>=8){         //once we have finished attempting to burn 3 times
        burnAttempt=true;         //these two will happen every time for loop navigation
        burning = false;
        sendXBee("Burner fired");
        ontimes = 0;
        burncurrent = false;
        logAction("firing burner attempted");
      }
}*/

void contiCheck(){
      //Continuity check to decide burn or no burn
      //Continuity check will decide whether to set burnSuccess to true or to order another cutNow=1
      //For now, let's assume every burn works every time
      /*burnSuccess=burnAttempt;                    
      if(burnSuccess&&!burncurrent){
        logAction("Burner Spent");
      }
      //logAction("Burner not spent, re-attempting burn");*/
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
void autopilot(){
   checkBurst();
   blinkMode();
   burnMode();

    if((millis()>=burnDelay)&&!delayBurn){   //Check to see if timer has run out or if cut has been commanded and if it is not currenlty in a delayed burn
      runBurn();
      delayBurn=true;
      GPSaction("timed cutdown attempt");
    }
    contiCheck();
    //...........................Firing Burner.......................  
    //for now, conticcheck disables Cutnow, so burncurrent lets us know if we are attempting a burn

}
void burnAction::Burn(){
  if(ontimes>0){
   if((millis()-Time>=offdelay)&&!burnerON){
    digitalWrite(fireBurner, HIGH);
    Time= millis();
    burnerON = true;
    Serial.println(String(millis()));
  }
  if(millis()-Time>=(ondelay+stagger*(3-ontimes))&&burnerON){
    digitalWrite(fireBurner, LOW);
    burnerON = false;
    Time = millis(); 
    Serial.println(String(millis()));
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
