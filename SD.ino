void openEventlog() {
    eventLog = SD.open(Ename, FILE_WRITE);
    sd_led.turn_on();
}

void closeEventlog() {
    eventLog.close();
    sd_led.turn_off();
}
void openGPSlog() {
    GPSlog = SD.open(GPSname, FILE_WRITE);;
    GPSlogOpen = true;
    sd_led.turn_on();
}

void closeGPSlog() {
    GPSlog.close();
    sd_led.turn_off();
}

//Takes a string describing any event that takes place and records it in the eventlog with a timestamp. 
void logAction(String event) {
  openEventlog();
  eventLog.println(flightTimeStr() + "  AC  " + event);
  closeEventlog();
}

void GPSaction(String action){
    if(GPS.Fix){   //GPS.fix
      logAction(action + ", " + flightTimeStr() + "," + String(GPS.location.lat(), 4) + "," + String(GPS.location.lng(), 4) + ", Altitude: " + String(GPS.altitude.feet()) + "ft. FIX");  
      //sendXBee(action + ", " + String(GPS.altitude.feet()) + "ft. Watch your heads!");
    }
    else{
      logAction(action + ", " + flightTimeStr() + "," + String(GPS.location.lat(), 4) + "," + String(GPS.location.lng(), 4) + ", Altitude: " + String(GPS.altitude.feet()) + "ft. NO FIX");
      //sendXBee(action + ", " + "altitude unknown" + " Watch your heads!");
    }
  }

