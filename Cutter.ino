Cutter::Cutter(uint8_t razor, uint8_t burner){
  this -> razor = razor;
  this -> burner = burner;
  onTime = 0;
  offTime = 0;
  times = 0;
  state = 0;
}

void Cutter::runCut(){
  //basic function for spinning razor and terminating cutter.
  timer = millis();       //set timer to be current time
  times = 1;              //run one long burn
  onTime = 15000;         //for 15 seconds
  
}

void Cutter::checkAction(){
  //checks to see if the cutter should be 
  if(times){
    if(millis()-timer > onTime && state){
      digitalWrite(burner, LOW);
      digitalWrite(razor, LOW);
      times--;
      state = 0;
    }
    else if(millis()- timer > offTime && !state){
      digitalWrite(burner, HIGH);
      digitalWrite(razor, HIGH);
      state = 1;
    }
  }
}

