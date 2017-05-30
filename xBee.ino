//Takes a string to send out via xBee, and logs the transmission to the SD card with a timestamp
void sendXBee(String out) {
  xBee.println(xBeeID + ";" + out + "!");
  openEventlog();
  eventlog.println(flightTimeStr() + "  TX  " + out);
 // eventlogB.println(flightTimeStr() + "  TX  " + out);
  closeEventlog();  
  }

//Takes the string of the xBee command as well as a description and logs to the SD card with a timestamp
void logCommand(String com, String command) {
  openEventlog();
  eventlog.println(flightTimeStr() + "  RX  " + com + "  " + command);
 // eventlogB.println(flightTimeStr() + "  RX  " + com + "  " + command);
  closeEventlog();
}

//Current method of sending an acknowledgement via RFD
void acknowledge() {
  xBee.println(xBeeID + "\n");
}

String lastCommand = "";
unsigned long commandTime = 0;

//Primary xBee function that looks for incoming messages, parses them, and executes the corresponding commands
void xBeeCommand(){
  boolean complete = false;                  //Stuff we don't care about
  String command = "";
  char inChar;
  while (xBee.available() > 0) {
    inChar = (char)xBee.read();
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
    int addedTime = (Com.substring(2, Com.length())).toInt();
    logCommand(Com, "Added time to failsafe");
    burnDelay += (addedTime*60*1000);  //Converts minutes to milliseconds
    int timeLeft = (burnDelay/1000);
    String timeLeftStr = (String(timeLeft / 60) + ":");
    timeLeft %= 60;
    timeLeftStr += (String(timeLeft / 10) + String(timeLeft % 10));
    sendXBee(timeLeftStr);
  }

  if ((Com.substring(0,2)).equals("WR")) {
    //Remove time in minutes to failsafe
    int remTime = (Com.substring(2, Com.length())).toInt();
    logCommand(Com, "Removed time from failsafe");
    burnDelay -= (remTime*60*1000);  //Converts minutes to milliseconds
    int timeLeft = (burnDelay/1000);
    String timeLeftStr = (String(timeLeft / 60) + ":");
    timeLeft %= 60;
    timeLeftStr += (String(timeLeft / 10) + String(timeLeft % 10));
    sendXBee(timeLeftStr);
  }



   else if(Com.equals("WB")){
    //blinks the LED so you know it's connected
    testBlink();
    logCommand(Com, "Loggy log-log");
    xBee.println("Hey you figured it out. Took you long enough");
   }
  
  

  else if (Com.equals("WX")) {
    //Burns the Tungsten, enters "recovery mode" after cutdown confirmed
    initiateCutdown();
    logCommand(Com, "Cuttdown Attempted");
    sendXBee("Starting Cut. Watch your heads!");
  }
  
 
  else if (Com.equals("WT")) {
    //Poll for cutdown timer remaining, returns minutes:seconds
    logCommand(Com, "Poll Remaining Time");
    unsigned long t = burnDelay/1000;
    String tStr = String(t / 60) + ":";
    t %= 60;
    tStr += String(t / 10) + String(t % 10);
    sendXBee(tStr);
  }

  /*
   else if (Com.equals("GPS")) {
    //Poll most recent GPS data
    logCommand(Com, "Request GPS data");
    String message = String(GPS.hour) + ":" + String(GPS.minute) + ":" + String(GPS.seconds) + ",";
    message += String(GPS.latitudeDegrees) + "," + String(GPS.longitudeDegrees) + "," + String(GPS.altitude * 3.28048) + ",";
    if (GPS.fix) message += "Fix";
    else message += "No Fix";
    sendXBee(message);
  }
   */

      
  else {
    //If no recognizable command was received, inform ground station
    logCommand(Com, "Unknown Command");
    sendXBee(String(Com) + ":  Command Not Recognized");
  }
}
