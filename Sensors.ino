class Sensor {
  protected:
    virtual Sensor();
    const char * Name;
    unsigned int Delay;          //delay for reading sensor
    virtual void sendCommand();  //for sending commands to a sensor
    template <class sensor_data_format> class sensorData
    {
      public:
        sensorData(sensor_data_format);
        sensor_data_format getData();
      
    };
    boolean newData;             //whether or not the sensor has new data. relevant for GPS
    virtual void setup();
};
class Accelerometer: public Sensor {

};
class TemperatureSensor: public Sensor {

};
class GPS: public Sensor {

};
