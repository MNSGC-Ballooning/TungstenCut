 template <class sensor_data_format>class Sensor {
  protected:
    virtual Sensor();
    //identifier
    const char * Name;
    //delay for reading sensor
    unsigned int Delay;        
    //for sending commands to a sensor
    virtual void sendCommand();  
    //for initializing the sensor
    virtual void init();
    //for updating sensor readings
    virtual void update();
    //the actual data
    sensor_data_format Data;              
    virtual sensor_data_format getData();
    virtual setData(sensor_data_format toSet);
                            //whether or not the sensor has new data. relevant for GPS
    boolean newData = 1;    //assume that the sensors always have new data       
};
class Accelerometer: public Sensor<int *> {  
  public:
      Accelerometer(char * Name, int Delay, int * accelerations){
        //feed in so that we are modifying global variables
        this -> Data = &accelerations[0];
        this -> Name = Name;
        this -> Delay = Delay;
        adxl = ADXL345();
        this -> Data = &accelerations[0];
      }
      //accelerometer class
      ADXL345 adxl;
      void init(){
        adxl.powerOn();    
        //what does this do? ask Garret         
        adxl.setRangeSetting(16);  
        adxl.setSpiBit(0);                                  //once again ask Garret what the hell this does
      }
      int getX(){return Data[0];}
      int getY(){return Data[1];}
      int getZ(){return Data[2];}
      void update(){
        adxl.readAccel(&Data[0],&Data[1],&Data[2]);
      }
      int * getData(){return Data;}    
};
class temperatureSensor: public Sensor<String>{
  public:
    DallasTemperature sensor;
    uint8_t Pin;
    temperatureSensor(char* Name, int Pin, int Delay, String *temperature){
      temperature = &Data;
      this -> Name = Name;
      this -> Delay = Delay;
      this -> Pin = Pin;
    }
    void init(){
      OneWire oneWire(Pin);
      sensor = DallasTemperature(&oneWire);
    }
    void update(){
      
    }
};

