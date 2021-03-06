//Takes a string to send out via xBee, and logs the transmission to the SD card with a timestamp
void sendXBee(String out) {
  xBee.send(out);
  openEventlog();
  if(SDcard){
    eventLog.println(flightTimeStr() + "  TX  " + out);
  // eventlogB.println(flightTimeStr() + "  TX  " + out);
  }
  closeEventlog();  
  }

//Takes the string of the xBee command as well as a description and logs to the SD card with a timestamp
void logCommand(String com, String command) {
  openEventlog();
  if(SDcard){
    eventLog.println(flightTimeStr() + "  RX  " + com + "  " + command);
  // eventlogB.println(flightTimeStr() + "  RX  " + com + "  " + command);
  }
  closeEventlog();
}

//Primary xBee function that looks for incoming messages, parses them, and executes the corresponding commands
void xBeeCommand(){
  //check for incoming xBeeCommands
  String Com = xBee.receive();

  //receive() returns empty string if no commands sent
  if (Com.equals("")) return;

  //Find and execute correct command
  if ((Com.substring(0,2)).equals("WA")) {
    //Add time in minutes to failsafe
    unsigned long addedTime = atol((Com.substring(2, Com.length())).c_str());
    logCommand(Com, "Added time to failsafe");
    sendXBee("added Time: "+ String(addedTime)+ " Minutes");
    burnDelay += (addedTime*60*1000);  //Converts minutes to milliseconds
    sendXBee("Time until cutdown: " + timeLeft());
  }

 else if ((Com.substring(0,2)).equals("WR")) {
    //Remove time in minutes to failsafe
    unsigned long remTime = atol((Com.substring(2, Com.length())).c_str());
    logCommand(Com, "Removed time from failsafe");
    sendXBee("Time removed: "+ String(remTime)+ " Minutes");
    burnDelay -= (remTime*60*1000);  //Converts minutes to milliseconds
    int timeLeft = int((burnDelay-millis())/1000);
    String timeLeftStr = (String(timeLeft/60) + ":");
    timeLeft %= 60;
    timeLeftStr += (String(timeLeft / 10) + String(timeLeft % 10));
    sendXBee("Time until cutdown: " + timeLeftStr);
  }



   else if(Com.equals("WB")){
    //blinks the LED so you know it's connected
    testBlink();
    logCommand(Com, "Loggy log-log");
    sendXBee("Hey you figured it out. Took you long enough");
   }
  
  

  else if (Com.equals("WX")) {
    //Burns the Tungsten, enters "recovery mode" after cutdown confirmed
    runBurn();
    if(GPS.fix){
      logCommand(Com, "Cuttdown Attempted at " + flightTimeStr() + "," + String(GPS.latitudeDegrees, 4) + "," + String(GPS.longitudeDegrees, 4) + ", Altitude: " + String(GPS.altitude * 3.28048) + "ft. FIX");  
      sendXBee("Starting Cut at Altitude " + String(GPS.altitude * 3.28048) + "ft. Watch your heads!");
    }
    else{
      logCommand(Com, "Cuttdown Attempted at " + flightTimeStr() + "," + String(GPS.latitudeDegrees, 4) + "," + String(GPS.longitudeDegrees, 4) + ", Altitude: " + String(GPS.altitude * 3.28048) + "ft. NO FIX");
      sendXBee("Starting Cut at unknown altitude, Watch your heads!");
    }
  }
  
 
  else if (Com.equals("WT")) {
    //Poll for cutdown timer remaining, returns minutes:seconds
    logCommand(Com, "Poll Remaining Time");
    if(timeBurn){
      sendXBee(timeLeft());
    }
    else{
      sendXBee("timed cut is currently not enabled");
    }
    
  }

  
   else if (Com.equals("GPS")) {
    //Poll most recent GPS data
    logCommand(Com, "Request GPS data");
    String message = "Time: " + String(GPS.hour) + ":" + String(GPS.minute) + ":" + String(GPS.seconds)+ ",";   
    message += "latitude: " + String(GPS.latitudeDegrees) + "," + "logitude: " + String(GPS.longitudeDegrees) + "," + "altitude: " + String(GPS.altitude * 3.28048) + ",";
    if (GPS.fix) message += "Fix";
    else message += "No Fix";
    sendXBee(message);
  }

   else if((Com.substring(0,2)).equals("WD")){   //disable altitude cut
    altCut = false;
    logCommand(Com, "altitude cut disabled");
    sendXBee("Altitude cutdown is now disabled");
   }
   else if ((Com.substring(0,2)).equals("WU")) {   //sets new cutdown Altitude
    //Set Cutdown Altitude
    long newAlt = atol((Com.substring(2, Com.length())).c_str());
    logCommand(Com, "New Cutdown Altitude: "+ String(newAlt)+ " Feet");
    sendXBee("New Cutdown Altitude: "+ String(newAlt)+ " Feet");
    cutAlt = newAlt;                                              
  }
  else if((Com.substring(0,2)).equals("WE")){   //enable altitude cut
    altCut = true;
    logCommand(Com, "altitude cut enabled");
    sendXBee("Altitude cutdown is now enabled, will cut at: " + String(cutAlt) + "feet");
   }
  else if((Com.substring(0,2)).equals("WC")){   //enable time burn     
    timeBurn = true;
    logCommand(Com, "timed cut enabled");
    burnDelay = millis() + 3600000;                //make the default timer 60 minutes
    sendXBee("timed cut enabled, time until cutdown: " + timeLeft());
  }

  else if((Com.substring(0,2)).equals("WS")){   //disable time burn
    timeBurn = false;
    logCommand(Com, "timed cut disabled");
    sendXBee("timed cut disabled");
  }

  else if((Com.substring(0,2)).equals("WF")){   //poll cutdown altitude
    logCommand(Com, "poll cutdown altitude");
    String toSend = "Cutdown altitude: " + String(cutAlt);
    if(altCut){
      toSend += ",  altitude cut is enabled";
    }
    else{
      toSend += ", altitude cut is disabled";
    }
    sendXBee(toSend);
  }
  
     else if(Com.substring(0,2).equals("WQ")){
    bacon = true;
    logCommand(Com, "beacon enabled");
   sendXBee("beacon enabled");
  }
  else if(Com.substring(0,2).equals("WP")){
    bacon = false;
    logCommand(Com, "beacon disabled");
    sendXBee("beacon disabled");
  }
        
  else {
    //If no recognizable command was received, inform ground station
    logCommand(Com, "Unknown Command");
    sendXBee(String(Com) + ":  Command Not Recognized");
  }


}
