void addTime(int addition){
  burnDelay+=(addition*1000);
}

void removeTime(int subtraction){
  burnDelay -=(subtraction*1000);
}

void initiateCutdown(){
  cutNow=1;
}

void flamingGuillotine(){       
  //Cutdown. Blinks, burns etc.
      sendXBee("Cutdown initiation received");
      
      for(int j=0;j<5;j++){               //LED blinks rapidly before firing burner
      digitalWrite(ledPin, HIGH); 
      delay(200);
      digitalWrite(ledPin, LOW);
      delay(200);
      }
        
        for(int u=0;u<3;u++){               //The actual burner activation.  It will attempt burn 3 times.
          digitalWrite(fireBurner, HIGH);
          delay(800+u*200);                 //Each burn is longer than the last.
          digitalWrite(fireBurner, LOW);
          delay(200);
        }
      
      burnAttempt=true;    //these two will happen every time for loop navigation
      
      sendXBee("Burner fired");
}

void contiCheck(){
      //Continuity check to decide burn or no burn
      //Continuity check will decide whether to set burnSuccess to true or to order another cutNow=1
      //For now, let's assume every burn works every time
      burnSuccess=1;
      cutNow=0;
      logAction("Burner Spent");
      //logAction("Burner not spent, re-attempting burn");
}
void autopilot(){
   if(testblink){
    testBlink();
   }
   if(!burnAttempt){  //Blinks LED every second to convey normal flight operation (countdown)
      countdownBlink();
      altTheSingleLadies();
    }

    if((!cutNow)&&(millis()>=burnDelay)){   //Check to see if timer has run out or if cut has been commanded
      cutNow=1;
    }
    //...........................Firing Burner.......................  
   
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
          timerLED=millis(); //reset timer 
      }

     else if((GPS.altitude * 3.28048>=cutAlt)&&gatePass){
      sendXBee("Activating GPS Altitude Triggered Cutdown");
      logAction("Activating GPS Altitude Triggered Cutdown");
      //cutNow=1;
     }
      
    }
  }
}

