//This file houses functions which control various LED blinking patterns
//The only reason it's in its own file is to cut down on scrolling in the main TungstenCut

///\/\/\/\/\/\/\/\/\/\/\/\/\Countdown Blink/\/\/\/\/\/\/\/\/\/\/\/\/\
//"Chirps" the LED once per second during normal flight countdown (e.g. before cutting/recovery)
void countdownBlink(){
  if(!LEDon&&(millis()-timerLED >=1000)){    //if it's been 1 second 
        
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
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/


///\/\/\/\/\/\/\/\/\/\/\/\/\Recovery Blink/\/\/\/\/\/\/\/\/\/\/\/\/\/\
//Similar to countdown blink, but slower  
void recoveryBlink(){
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
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/   


///\/\/\/\/\/\/\/\/\/\/\/\/\Retry Cut Blink/\/\/\/\/\/\/\/\/\/\/\/\/\/\
//Similar to countdown blink, but slower  
void retryBlink(){
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
//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/ 

void testBlink(){
  for(int i=0;i<4;i++){
    digitalWrite(ledPin, HIGH);
    delay(1000);
    digitalWrite(ledPin, LOW);
    delay(1000);
  }
}

