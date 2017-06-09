//Takes a string to send out via xBee, and logs the transmission to the SD card with a timestamp
void sendXBee(String out) {
  Serial.println(xBeeID + ";" + out + "!");
  openEventlog();
  eventLog.println(flightTimeStr() + "  TX  " + out);
 // eventlogB.println(flightTimeStr() + "  TX  " + out);
  closeEventlog();  
  }

//Takes the string of the xBee command as well as a description and logs to the SD card with a timestamp
void logCommand(String com, String command) {
  openEventlog();
  eventLog.println(flightTimeStr() + "  RX  " + com + "  " + command);
 // eventlogB.println(flightTimeStr() + "  RX  " + com + "  " + command);
  closeEventlog();
}

//Current method of sending an acknowledgement via RFD
void acknowledge() {
  Serial.println(xBeeID + "\n");
}

String lastCommand = "";
unsigned long commandTime = 0;

//Primary xBee function that looks for incoming messages, parses them, and executes the corresponding commands
void xBeeCommand(){
  boolean complete = false;                  //Stuff we don't care about
  String command = "";
  char inChar;
  while (Serial.available() > 0) {
    inChar = (char)Serial.read();
    if (inChar != ' ') {
      command += inChar;
      if (inChar == '!') { 
        complete = true;
        break;
      }
    }
    delay(10);
  }

  if (!complete) return;                                                                   //Stuff we don't care about
  if (command.equals(lastCommand) && (millis() - commandTime < 30000)) return; 
  int split = command.indexOf('?');
  if (!(command.substring(0, split)).equals(xBeeID)) return;
  lastCommand = command;
  String Com = command.substring(split + 1, command.length() - 1);
  acknowledge();
  commandTime = millis();
  

  
  if ((Com.substring(0,2)).equals("WA")) {
    //Add time in minutes to failsafe
    unsigned long addedTime = atol((Com.substring(2, Com.length())).c_str());
    sendXBee("added Time: "+ String(addedTime)+ " Minutes");
    logCommand(Com, "Added time to failsafe");
    burnDelay += (addedTime*60*1000);  //Converts minutes to milliseconds
    int timeLeft = int((burnDelay-millis())/1000);
    String timeLeftStr = (String(timeLeft/60) + ":");
    timeLeft %= 60;
    timeLeftStr += (String(timeLeft / 10) + String(timeLeft % 10));
    sendXBee("Time until cutdown: " + timeLeftStr);
  }

 else if ((Com.substring(0,2)).equals("WR")) {
    //Remove time in minutes to failsafe
    unsigned long remTime = atol((Com.substring(2, Com.length())).c_str());
    sendXBee("Time removed: "+ String(remTime)+ " Minutes");
    logCommand(Com, "Removed time from failsafe");
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
    unsigned long t = (burnDelay-millis())/1000;
    String tStr = String(t / 60) + ":";
    t %= 60;
    tStr += String(t / 10) + String(t % 10);
    sendXBee(tStr);
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
   

      
  else {
    //If no recognizable command was received, inform ground station
    logCommand(Com, "Unknown Command");
    sendXBee(String(Com) + ":  Command Not Recognized");
  }
}
