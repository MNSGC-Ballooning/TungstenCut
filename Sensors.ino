//ACCELEROMETER

Accelerometer::Accelerometer(int Delay, int * accelerations){
  this -> &accelerations[0] = &accelerations[0];
  this -> Delay = Delay;
  adxl = ADXL345();
}

void Accelerometer::init(){
  adxl.powerOn();     //what does this do? ask Garret to find out
  adxl.setRangeSetting(16);
  adxl.setSpiBit(0);
}

void Accelerometer::update(){
  adxl.readAccel(&accelerations[0], &accelerations[1], &accelerations[2]);
}

TemperatureSensor::TemperatureSensor(uint8_t Pin, int Delay, String *temperature){
  this -> temperature = temperature;
  this -> Delay = Delay;
  OneWire oneWire(Pin);
  sensor = DallasTemperature(&oneWire);
}

void TemperatureSensor::update(){
  if(millis()-Timer>Delay){
    sensor.requestTemperatures();
    temperature = sensor.getTempByIndex(0);
    Timer = millis();
  }
}








void UpdateSensors(){
   for (int i = 0; i < sensors.size(); i++){
      sensors[i] -> update();
  }
}


