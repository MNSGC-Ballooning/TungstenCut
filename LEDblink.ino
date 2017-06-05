//This file houses functions which control various LED blinking patterns
//The only reason it's in its own file is to cut down on scrolling in the main TungstenCut

///\/\/\/\/\/\/\/\/\/\/\/\/\Countdown Blink/\/\/\/\/\/\/\/\/\/\/\/\/\
//"Chirps" the LED once per second during normal flight countdown (e.g. before cutting/recovery)
Blink::Blink(){
  ondelay = 0;
  offdelay = 0;
  ontimes = 0;
  Time = 0;
  
}
Blink::Blink(int on, int off, int times){
  ondelay = on;
  offdelay = off;
  ontimes = times;
  Time= 0;
  
}
void Blink::BLINK(){
  if((millis()-Time>=offdelay)&&!LEDon){
    switchLED();
    Time= millis();
    Serial.println(String(millis()));
    if(ontimes>0){
      ontimes--;
    }
  }
  if((millis()-Time>=ondelay)&&LEDon){
    switchLED();
    Time = millis(); 
    Serial.println(String(millis()));
  }
}
int Blink::getOnTimes(){
  return ontimes;
}

void switchLED(){
  if(LEDon){
    LEDon = false;
    digitalWrite(ledPin, LOW);
  }
  else{
    LEDon= true;
    digitalWrite(ledPin, HIGH);
  }
}

void blinkMode(){
  if(currentBlink->ontimes==0){
    delete currentBlink;
    if(recovery){
      currentBlink = &recoveryBlink;
    }
    else{
      currentBlink = &countdownBlink;
    }
   }
   if(burnBlink){
    burnBlink = false;
    currentBlink = new Blink(200,200,5);                //sets to burning blinking
   }
   if(testBlink){
    testBlink = false;                                  //sets to testing blinking
    currentBlink = new Blink(150,300,10);
   }
   

}

  

