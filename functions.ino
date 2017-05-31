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
      Serial.println("Cutdown initiation received");

      
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
      
      Serial.println("Burner fired");
}

void contiCheck(){
      //Continuity check to decide burn or no burn
      //Continuity check will decide whether to set burnSuccess to true or to order another cutNow=1
      //For now, let's assume every burn works every time
      burnSuccess=1;
      cutNow=0;
}

