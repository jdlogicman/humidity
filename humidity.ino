/* How to use the DHT-22 sensor with Arduino uno
   Temperature and humidity sensor
   More info: http://www.ardumotive.com/how-to-use-dht-22-sensor-en.html
   Dev: Michalis Vasilakis // Date: 1/7/2015 // www.ardumotive.com */

//Libraries
#include <DHT.h>

//Constants
#define HUMIDITY_ON_IF_BELOW        90.0        // on iff 90% or below
#define HUMIDITY OFF_IF_ABOVE       95.0        // off if 95% or above
#define HUMIDITY_MAX_ON_TIME_MS     60000       // run humidifier for max 1 min at a time
#define HUMIDITY_WAIT_INTERVAL_MS   60000       // and don't run it again for at least a minute
#define FAN_ON_DURATION_MS          30000       // 30 seconds
#define FAN_ON_INTERVAL_MS          60*60*1000  // one per hour

class HumidityController
{
  long _triggeredMs;
  long _offMs;
  const float _onIfBelow;
  const float _offIfAbove;
  const unsigned long _waitMs;
  const unsigned long _maxRunMs;
  const int _pin;

  void turn_on()
  {
    digitalWrite(_pin, HIGH);
    _triggeredMs = millis();
    _offMs = 0;
  }
  void turn_off()
  {
    digitalWrite(_pin, LOW);
    _triggeredMs = 0;
    _offMs= millis();
  }
  public:
    HumidityController(float onIfBelow, float offIfAbove, unsigned long maxRunMs, unsigned long waitMs, int pin)
    : _onIfBelow(onIfBelow), _offIfAbove(offIfAbove), _maxRunMs(maxRunMs), _waitMs(waitMs), _pin(pin),
    _offMs(0), _triggeredMs(0)
    {
      _offMs = millis();
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
          Serial.println(humidity >= _offIfAbove);
          Serial.println((now-_triggeredMs) >= _maxRunMs);
          Serial.println(now);
          Serial.println(_triggeredMs);
          Serial.println(_maxRunMs);
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

HumidityController humidityController(90, 95, 60000, 6000, 3);


#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino





void setup()
{
  Serial.begin(9600);
  dht.begin();
}

void loop()
{
    float hum = dht.readHumidity();
    float temp = dht.readTemperature() * 9.0/5 + 32;
    
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" F");
    humidityController.control(hum);
    delay(2000); //Delay 2 sec.
}

   
