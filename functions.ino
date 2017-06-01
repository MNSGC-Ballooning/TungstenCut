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
      burnBlink();                    // blink the LED before the burn
      if(ontimes>=5&&ontimes<8){      // we will run the burner 3 times (ontimes gets to 5 through burnBlink)
        if(((millis()-burnBlinkTime)>=(800+long((ontimes-4)*200)))&&burning){      //for increasingly long times, turn on the burner
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
      }
}

void contiCheck(){
      //Continuity check to decide burn or no burn
      //Continuity check will decide whether to set burnSuccess to true or to order another cutNow=1
      //For now, let's assume every burn works every time
      burnSuccess=burnAttempt;                    
      cutNow=0;
      if(burnSuccess){
        logAction("Burner Spent");
      }
      //logAction("Burner not spent, re-attempting burn");
}
void autopilot(){
   if(testblink){
    testBlink();
   }
    
   if(!burnAttempt){  //Blinks LED every second to convey normal flight operation (countdown)
      countdownBlink();
    }

    if((!cutNow)&&(millis()>=burnDelay)){   //Check to see if timer has run out or if cut has been commanded
      cutNow=1;
    }
    //...........................Firing Burner.......................  
    //for now, conticcheck disables Cutnow, so burncurrent lets us know if we are attempting a burn
    if((cutNow||burncurrent)){
        burncurrent = true;         
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

