
LED::LED(uint8_t pin){
  this -> pin = pin;
}
LED::LED(uint8_t pin, unsigned int onTime, unsigned int offTime){
  this -> pin = pin;
  this -> onTime = onTime;
  this -> offTime = offTime;
}
void LED::turn_on(){
  state = 1;
  digitalWrite(pin, HIGH);
}
void LED::turn_off(){
  state = 0;
  digitalWrite(pin, LOW);
}
void LED::blink(){
  if(millis()-timer > offTime && !state){
    turn_on();
    timer = millis();
  }
  else if (millis()-timer > onTime && state){
    timer = millis();
    turn_off();
    if(onTimes){
      onTimes--;
      if(!onTimes){
        onTime = holderOnTime;
        offTime = holderOffTime;
      }
    }
  }
}
void LED::change_blink(unsigned int onTime, unsigned int offTime, uint8_t onTimes){
  if(onTimes){
    holderOnTime = this -> onTime;
    holderOffTime = this -> offTime;
    this -> onTime = onTime;
  }
  this -> offTime = offTime;
  this -> onTimes = onTimes;
}



