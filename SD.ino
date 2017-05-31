boolean eventlogOpen = false;
boolean GPSlogOpen = false;
//The following functions handle both opening files and controlling the data indicator LED

void openEventlog() {
  if (!eventlogOpen) {
    eventLog = SD.open(Ename, FILE_WRITE);;
    eventlogOpen = true;
    digitalWrite(ledSD, HIGH);
  }
}

void closeEventlog() {
  if (eventlogOpen) {
    eventLog.close();
    // eventlogB.close();
    eventlogOpen = false;
    if (!eventlogOpen)
      digitalWrite(ledSD, LOW);
  }
}
void openGPSlog() {
  if (!GPSlogOpen) {
    GPSlog = SD.open(Ename, FILE_WRITE);;
    GPSlogOpen = true;
    digitalWrite(ledSD, HIGH);
  }
}

void closeGPSlog() {
  if (GPSlogOpen) {
    GPSlog.close();
    // eventlogB.close();
    GPSlogOpen = false;
    if (!GPSlogOpen)
      digitalWrite(ledSD, LOW);
  }
}

//Takes a string describing any event that takes place and records it in the eventlog with a timestamp. 
void logAction(String event) {
  openEventlog();
  eventLog.println(flightTimeStr() + "  AC  " + event);
 // eventlogB.println(flightTimeStr() + "  AC  " + event);
  closeEventlog();
}
