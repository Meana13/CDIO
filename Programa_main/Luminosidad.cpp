#include "Luminosidad.h"

  int luz::Calcular_luz(int channelValue) {
    
      int NivelLuz = 0;
      float LecturaLuz = ads1015.readADC_SingleEnded(channelValue);
      
      if (LecturaLuz >= 0 && LecturaLuz <= 300 )
      {
        NivelLuz = 0;
      }
      if (LecturaLuz >= 301 && LecturaLuz <= 600 )
      {
        NivelLuz = 1;
      }
      if (LecturaLuz >= 601 && LecturaLuz <= 10000 )
      {
        NivelLuz = 2;
      }
      if (LecturaLuz >= 10001 && LecturaLuz < 50000 )
      {
        NivelLuz = 3;
      }
      return (NivelLuz);
}
