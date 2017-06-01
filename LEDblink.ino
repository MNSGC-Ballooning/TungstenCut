//This file houses functions which control various LED blinking patterns
//The only reason it's in its own file is to cut down on scrolling in the main TungstenCut

///\/\/\/\/\/\/\/\/\/\/\/\/\Countdown Blink/\/\/\/\/\/\/\/\/\/\/\/\/\
//"Chirps" the LED once per second during normal flight countdown (e.g. before cutting/recovery)
void countdownBlink(){
  if(!testblink){
    if(!LEDon&&(millis()-timerLED >=850)){    //if it's been 1 second 
        
        digitalWrite(ledPin, HIGH);
        LEDon=true;                 //turn LED on and remember
        timerLED=millis();          //Reset timer
        }
    if(LEDon&&(millis()-timerLED>=150)){ //If it's been .1s
       
        digitalWrite(ledPin, LOW); //turn off LED and remember
        LEDon=false;
        timerLED=millis(); //reset timer
    }
  }
}
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/


///\/\/\/\/\/\/\/\/\/\/\/\/\Recovery Blink/\/\/\/\/\/\/\/\/\/\/\/\/\/\
//Similar to countdown blink, but slower  
void recoveryBlink(){
  if(!testblink){
    if(!LEDon&&(millis()-timerLED >=2000)){    //if it's been 2 second 
        
        digitalWrite(ledPin, HIGH);
        LEDon=true;                 //turn LED on and remember
        timerLED=millis();          //Reset timer
       
        }
    if(LEDon&&(millis()-timerLED>=150)){ //If it's been .1s
       
        digitalWrite(ledPin, LOW); //turn off LED and remember
        LEDon=false;
        timerLED=millis(); //reset timer
    }
  }
}
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/   


///\/\/\/\/\/\/\/\/\/\/\/\/\Retry Cut Blink/\/\/\/\/\/\/\/\/\/\/\/\/\/\
//Similar to countdown blink, but slower  
void retryBlink(){
  if(!testblink){
    if(!LEDon&&(millis()-timerLED >=300)){    //if it's been off for .3 second 
        
        digitalWrite(ledPin, HIGH);
        LEDon=true;                 //turn LED on and remember
        timerLED=millis();          //Reset timer
        }
    if(LEDon&&(millis()-timerLED>=1000)){ //If it's been on for 1s
       
        digitalWrite(ledPin, LOW); //turn off LED and remember
        LEDon=false;
        timerLED=millis(); //reset timer
    }
  }
}
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/ 

void testBlink(){
  if(ontimes<10){
    if((millis()-testBlinkTime>=300)&&!LEDon){
     digitalWrite(ledPin, HIGH);
     LEDon=true;                 //turn LED on and remember
     ontimes++;
     testBlinkTime= millis();
    }
    if((millis()-testBlinkTime>=150)&&LEDon){
      digitalWrite(ledPin, LOW);
      LEDon = false;
      testBlinkTime = millis();
    }
  }
  else{
    testblink = false;
    ontimes = 0;
  }
  }
  

