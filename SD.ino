boolean eventlogOpen = false;

//The following functions handle both opening files and controlling the data indicator LED

void openEventlog() {
  if (!eventlogOpen) {
    eventlog = SD.open(filename, FILE_WRITE);
   // eventlogB = SD.open(eventfileB, FILE_WRITE);
    eventlogOpen = true;
    digitalWrite(ledSD, HIGH);
  }
}

void closeEventlog() {
  if (eventlogOpen) {
    eventlog.close();
   // eventlogB.close();
    eventlogOpen = false;
    if (!eventlogOpen)
      digitalWrite(ledSD, LOW);
  }
}

//Takes a string describing any event that takes place and records it in the eventlog with a timestamp. 
void logAction(String event) {
  openEventlog();
  eventlog.println(flightTimeStr() + "  AC  " + event);
 // eventlogB.println(flightTimeStr() + "  AC  " + event);
  closeEventlog();
}
