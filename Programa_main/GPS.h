#include <Adafruit_ADS1X15.h>
Adafruit_ADS1115 ads1015;

#include "TinyGPS++.h"
TinyGPSPlus gps;

class GPS{
  
  public:
  void medir(){
      while (pins.available() > 0)
      gps.encode(pins.read());
      if (gps.location.isUpdated())
      {
        Serial.print("LAT="); Serial.print(gps.location.lat(), 6);
        Serial.print("LNG="); Serial.println(gps.location.lng(), 6);
      }
  }
}
