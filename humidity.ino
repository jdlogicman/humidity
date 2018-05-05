#include "Adafruit_HTU21DF.h"
// #include <DHT.h>


class HumidityController
{
  const float _onIfBelow;
  const float _offIfAbove;
  const unsigned long _maxRunMs;
  const unsigned long _waitMs;
  const int _pin;
  long _offMs;
  long _triggeredMs;
  
  void turn_on()
  {
    digitalWrite(_pin, LOW);
    _triggeredMs = millis();
    _offMs = 0;
  }
  void turn_off()
  {
    digitalWrite(_pin, HIGH);
    _triggeredMs = 0;
    _offMs= millis();
  }
  public:
    HumidityController(float onIfBelow, float offIfAbove, unsigned long maxRunMs, unsigned long waitMs, int pin)
    : _onIfBelow(onIfBelow), _offIfAbove(offIfAbove), _maxRunMs(maxRunMs), _waitMs(waitMs), _pin(pin),
    _offMs(0), _triggeredMs(0)
    {
      _offMs = millis();
      pinMode(_pin, OUTPUT);
      turn_off();
    }
    bool is_on() const
    {
      return _triggeredMs != 0;
    }
    void control(float humidity)
    {
      unsigned long now = millis();
      
      if (_triggeredMs)
      {
        // it's on - see if we can turn it off
        if (humidity >= _offIfAbove || (now-_triggeredMs) >= _maxRunMs)
        {
          Serial.println("Turning off humidifier");
          turn_off();
        }
      }
      else if (now - _offMs >= _waitMs && humidity <= _onIfBelow)
      {
        Serial.println("Turning on humidifier");
        turn_on();
      }
    }
};

HumidityController humidityController(90, 95, 60000, 60000, 3);

/*
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT sensor(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
*/
Adafruit_HTU21DF sensor;



void setup()
{
  Serial.begin(9600);
  if (!sensor.begin()) {
    Serial.println("Couldn't find sensor!");
    delay(1000);
  }
}

void loop()
{
    float hum = sensor.readHumidity();
    float temp = sensor.readTemperature() * 9.0/5 + 32;
    
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" F");
    humidityController.control(hum);
    Serial.print("Humidifier is ");
    Serial.println(humidityController.is_on() ? "ON" : "OFF");
    delay(2000); //Delay 2 sec.
}

   
