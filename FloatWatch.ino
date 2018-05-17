//the floatwatch class to watch manage the floating of the system
FloatWatch::FloatWatch(unsigned int floatTime){
  this -> floatTime = floatTime;
  prevAlt = 0;
  floatState = 0;
  floatStartTime = 0;
}

void FloatWatch::updateStatus(){
  //managing the floatwatch to see if we are floating and take action
  if(Timer - millis() < 2000) return;       //exit the function if it hasn't been two seconds
  if(!GPS.Fix && !floatState) floatState = 0;                   //reset to original state if the GPS loses fix
  switch(floatState){
         //the 0 state is if we are ascending
      case 0:
         //check to see if we have started descending, change state if descent detected
         if(GPS.altitude.feet() < prevAlt) floatState = 1;
      case 1:
         //in this state we need 15 consectutive decreasing hits to activate the float mode
         if(GPS.altitude.feet() > prevAlt){ 
           checkFloat = 0;
           floatState = 0;
           //sendXBee("checkfloat reset");
         }
         else if(checkFloat > 15){
          //enter into float mode
          //sendXbee("Float detected, entering floating mode");
          floatStartTime = millis();
          floatState = 2;
          checkFloat = 0;
         }
         else{
          //sendXBee("checking for float: " + String(checkFloat));
          checkFloat++; 
         }
     case 2:
        //the case where a float has been detected
        if(millis()-floatStartTime > floatTime){
          cutter1.runCut();
         }
         
  }
}

