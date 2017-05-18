void sendXBee(String out) {
  xBee.println(xBeeID + ";" + out + "!");}


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
  

  
   if (Com.equals("WA")) {
    //Add an amount of time equal to 3 minutes
    addTime(69);
    Serial.println("69 Minutes Added");
  }

   else if(Com.equals("WB")){
    //blinks the LED so you know it's connected
    testBlink();
    xBee.println("Hey you figured it out. Took you long enough");
   }
  
  else if (Com.equals("WR")) {
    //Removes an amount of time equal to 3 minutes
    Serial.println("69 Minutes Removed");
    removeTime(69);
  }

  else if (Com.equals("WX")) {
    //Begins Cutdown Mode
    initiateCutdown();
    Serial.println("Starting Cut. Watch your heads!");
  }
  
 
  else if (Com.equals("WT")) {
    //Poll for cutdown timer remaining, returns minutes:seconds
    unsigned long t = burnDelay;
    String tStr = String(t / 60) + ":";
    t %= 60;
    tStr += String(t / 10) + String(t % 10);
    sendXBee(tStr);
  }

      
  else {
    //If no recognizable command was received, inform ground station
    //logCommand(Com, "Unknown Command");
    sendXBee(String(Com) + ":  Command Not Recognized");
  }
}
