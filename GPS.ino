/*
float checkAlt;
int lastGPS;

//function to handle both retrieval of data from GPS module and sensors, as well as recording it on the SD card
void updateSensors() {
  
  while (gpsSerial.available() > 0) {
    GPS.read();
  }
  if (GPS.newNMEAreceived()) {
    GPS.parse(GPS.lastNMEA());
    if (!firstFix && GPS.fix) {
      GPSstartTime = GPS.hour * 3600 + GPS.minute * 60 + GPS.seconds;
      firstFix = true;
    }
    if (getGPStime() > lastGPS) {
      openEventlog();
      String data = "";
      data += (flightTimeStr() + "," + String(GPS.latitudeDegrees) + "," + String(GPS.longitudeDegrees) + ",");
      data += (String(GPS.altitude * 3.28048) + ",");    //convert meters to feet for datalogging
      data += (String(GPS.month) + "/" + String(GPS.day) + "/" + String(GPS.year) + ",");
      data += (String(GPS.hour) + ":" + String(GPS.minute) + ":" + String(GPS.seconds) + ",");
      if (GPS.fix) {
        data += "fix,";
        lastGPS = GPS.hour * 3600 + GPS.minute * 60 + GPS.seconds;
      }
      else
        data += ("No fix,");
      
      eventlog.println(data);
     // eventlogB.println(data);
      closeEventlog();
    }
  }
}

int getGPStime() {
  return days * 86400 + GPS.hour * 3600 + GPS.minute * 60 + GPS.seconds;
}

int getLastGPS() {    //returns time in seconds between last successful fix and initial fix. Used to match with altitude data
  if (!newDay && lastGPS < GPSstartTime) {
    days++;
    newDay = true;
  }
  else if (newDay && lastGPS > GPSstartTime)
    newDay = false;
  return days * 86400 + lastGPS;
}
*/
