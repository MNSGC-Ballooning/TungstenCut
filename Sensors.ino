 template <class sensor_data_format>class Sensor {
  protected:
    virtual Sensor();
    const char * Name;
    //delay for reading sensor
    unsigned int Delay;        
    //for sending commands to a sensor
    virtual void sendCommand();  
    virtual void init();
    //the actual data
    sensor_data_format Data;              
    virtual sensor_data_format getData();
    virtual setData(sensor_data_format toSet);
    //whether or not the sensor has new data. relevant for GPS
    boolean newData = 0;            
};
class Accelerometer: public Sensor<int *> {  
  public:
      ADXL345 adxl;
      int accelerations [3] = {0,0,0};
      //array for the orientation of the sensor
      Accelerometer(char * Name){
        this -> Name = Name;
        adxl = ADXL345();
        this -> Data = &accelerations[0];
      }
      void init(){
        adxl.powerOn();    
        //what does this do? ask Garret         
        adxl.setRangeSetting(16);  
        adxl.setSpiBit(0);                                  //once again ask Garret what the hell this does
      }
      
      
};

