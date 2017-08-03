boolean eventlogOpen = false;
boolean GPSlogOpen = false;
//The following functions handle both opening files and controlling the data indicator LED

void openEventlog() {
  if (!eventlogOpen&&SDcard) {
    eventLog = SD.open(Ename, FILE_WRITE);;
    eventlogOpen = true;
    digitalWrite(ledSD, HIGH);
  }
}

void closeEventlog() {
  if (eventlogOpen&&SDcard) {
    eventLog.close();
    eventlogOpen = false;
    if (!eventlogOpen)
      digitalWrite(ledSD, LOW);
  }
}
void openGPSlog() {
  if (!GPSlogOpen&&SDcard) {
    GPSlog = SD.open(GPSname, FILE_WRITE);;
    GPSlogOpen = true;
    digitalWrite(ledSD, HIGH);
  }
}

void closeGPSlog() {
  if (GPSlogOpen&&SDcard) {
    GPSlog.close();
    GPSlogOpen = false;
    if (!GPSlogOpen)
      digitalWrite(ledSD, LOW);
  }
}

//Takes a string describing any event that takes place and records it in the eventlog with a timestamp. 
void logAction(String event) {
  if(SDcard){
  openEventlog();
  eventLog.println(flightTimeStr() + "  AC  " + event);
  closeEventlog();
  }
}

void GPSaction(String action){
    if(GPS.fix){
      logAction(action + ", " + flightTimeStr() + "," + String(GPS.latitudeDegrees, 4) + "," + String(GPS.longitudeDegrees, 4) + ", Altitude: " + String(GPS.altitude * 3.28048) + "ft. FIX");  
      sendXBee(action + ", " + String(GPS.altitude * 3.28048) + "ft. Watch your heads!");
    }
    else{
      logAction(action + ", " + flightTimeStr() + "," + String(GPS.latitudeDegrees, 4) + "," + String(GPS.longitudeDegrees, 4) + ", Altitude: " + String(GPS.altitude * 3.28048) + "ft. NO FIX");
      sendXBee(action + ", " + "altitude unknown" + " Watch your heads!");
    }
  }

