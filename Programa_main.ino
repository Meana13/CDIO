#include <Adafruit_ADS1X15.h>
Adafruit_ADS1115 ads1015;

#include <ESP8266WiFi.h>

// Comentar/Descomentar para ver mensajes de depuracion en monitor serie y/o respuesta del HTTP server
#define PRINT_DEBUG_MESSAGES
//#define PRINT_HTTP_RESPONSE

// Comentar/Descomentar para conexion Fuera/Dentro de UPV
#define WiFi_CONNECTION_UPV

// Selecciona que servidor REST quieres utilizar entre ThingSpeak y Dweet
#define REST_SERVER_THINGSPEAK //Selecciona tu canal para ver los datos en la web (https://thingspeak.com/channels/1948866)
//#define REST_SERVER_DWEET //Selecciona tu canal para ver los datos en la web (http://dweet.io/follow/cdiocurso2022g06)

///////////////////////////////////////////////////////
/////////////// WiFi Definitions /////////////////////
//////////////////////////////////////////////////////

#ifdef WiFi_CONNECTION_UPV //Conexion UPV
const char WiFiSSID[] = "GTI1";
const char WiFiPSK[] = "1PV.arduino.Toledo";
#else //Conexion fuera de la UPV
const char WiFiSSID[] = "";
const char WiFiPSK[] = "MyPassWord";
#endif



///////////////////////////////////////////////////////
/////////////// SERVER Definitions /////////////////////
//////////////////////////////////////////////////////

#if defined(WiFi_CONNECTION_UPV) //Conexion UPV
const char Server_Host[] = "proxy.upv.es";
const int Server_HttpPort = 8080;
#elif defined(REST_SERVER_THINGSPEAK) //Conexion fuera de la UPV
const char Server_Host[] = "api.thingspeak.com";
const int Server_HttpPort = 80;
#else
const char Server_Host[] = "dweet.io";
const int Server_HttpPort = 80;
#endif

WiFiClient client;

///////////////////////////////////////////////////////
/////////////// HTTP REST Connection ////////////////
//////////////////////////////////////////////////////

#ifdef REST_SERVER_THINGSPEAK
const char Rest_Host[] = "api.thingspeak.com";
String MyWriteAPIKey = "OHLX8R0KJH6MAZJ8"; // Escribe la clave de tu canal ThingSpeak
#else
const char Rest_Host[] = "dweet.io";
String MyWriteAPIKey = "cdiocurso2022g06"; // Escribe la clave de tu canal Dweet
#endif

#define NUM_FIELDS_TO_SEND 4 //Numero de medidas a enviar al servidor REST (Entre 1 y 8)

/////////////////////////////////////////////////////
/////////////// Pin Definitions ////////////////
//////////////////////////////////////////////////////

const int LED_PIN = 5; // Thing's onboard, green LED

/////////////////////////////////////////////////////
/////////////// WiFi Connection ////////////////
//////////////////////////////////////////////////////

void connectWiFi()
{
  byte ledStatus = LOW;

#ifdef PRINT_DEBUG_MESSAGES
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
#endif

  WiFi.begin(WiFiSSID, WiFiPSK);

  while (WiFi.status() != WL_CONNECTED)
  {
    // Blink the LED
    digitalWrite(LED_PIN, ledStatus); // Write LED high/low
    ledStatus = (ledStatus == HIGH) ? LOW : HIGH;
#ifdef PRINT_DEBUG_MESSAGES
    Serial.println(".");
#endif
    delay(500);
  }
#ifdef PRINT_DEBUG_MESSAGES
  Serial.println( "WiFi Connected" );
  Serial.println(WiFi.localIP()); // Print the IP address
#endif
}

/////////////////////////////////////////////////////
/////////////// HTTP POST  ThingSpeak////////////////
//////////////////////////////////////////////////////

void HTTPPost(String fieldData[], int numFields) {

  // Esta funcion construye el string de datos a enviar a ThingSpeak mediante el metodo HTTP POST
  // La funcion envia "numFields" datos, del array fieldData.
  // Asegurate de ajustar numFields al número adecuado de datos que necesitas enviar y activa los campos en tu canal web

  if (client.connect( Server_Host , Server_HttpPort )) {

    // Construimos el string de datos. Si tienes multiples campos asegurate de no pasarte de 1440 caracteres

    String PostData = "api_key=" + MyWriteAPIKey ;
    for ( int field = 1; field < (numFields + 1); field++ ) {
      PostData += "&field" + String( field ) + "=" + fieldData[ field ];
    }

    // POST data via HTTP
#ifdef PRINT_DEBUG_MESSAGES
    Serial.println( "Connecting to ThingSpeak for update..." );
#endif
    client.println( "POST http://" + String(Rest_Host) + "/update HTTP/1.1" );
    client.println( "Host: " + String(Rest_Host) );
    client.println( "Connection: close" );
    client.println( "Content-Type: application/x-www-form-urlencoded" );
    client.println( "Content-Length: " + String( PostData.length() ) );
    client.println();
    client.println( PostData );
#ifdef PRINT_DEBUG_MESSAGES
    Serial.println( PostData );
    Serial.println();
    //Para ver la respuesta del servidor
#ifdef PRINT_HTTP_RESPONSE
    delay(500);
    Serial.println();
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
    Serial.println();
    Serial.println();
#endif
#endif
  }
}

////////////////////////////////////////////////////
/////////////// HTTP GET  ////////////////
//////////////////////////////////////////////////////

void HTTPGet(String fieldData[], int numFields) {

  // Esta funcion construye el string de datos a enviar a ThingSpeak o Dweet mediante el metodo HTTP GET
  // La funcion envia "numFields" datos, del array fieldData.
  // Asegurate de ajustar "numFields" al número adecuado de datos que necesitas enviar y activa los campos en tu canal web

  if (client.connect( Server_Host , Server_HttpPort )) {
#ifdef REST_SERVER_THINGSPEAK
    String PostData = "GET https://api.thingspeak.com/update?api_key=";
    PostData = PostData + MyWriteAPIKey ;
#else
    String PostData = "GET http://dweet.io/dweet/for/";
    PostData = PostData + MyWriteAPIKey + "?" ;
#endif

    for ( int field = 1; field < (numFields + 1); field++ ) {
      PostData += "&field" + String( field ) + "=" + fieldData[ field ];
    }


#ifdef PRINT_DEBUG_MESSAGES
    Serial.println( "Connecting to Server for update..." );
#endif
    client.print(PostData);
    client.println(" HTTP/1.1");
    client.println("Host: " + String(Rest_Host));
    client.println("Connection: close");
    client.println();
#ifdef PRINT_DEBUG_MESSAGES
    Serial.println( PostData );
    Serial.println();
    //Para ver la respuesta del servidor
#ifdef PRINT_HTTP_RESPONSE
    delay(500);
    Serial.println();
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
    Serial.println();
    Serial.println();
#endif
#endif
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////
/////////////// Variables Sensores /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

// El circuito de salinidad se conecta al puerto 5
#define power_pin 5
int channelValue = 0;
#define Offset 7-6.25
#define samplingInterval 20
#define printInterval 600
#define ArrayLenght 40 //numero de muestras
int sensorValue = 0;
int humidityValue = 0;
int Sal = 0;
int Humedad = 0;
int NivelPh = 0;
float Temperatura = 0;
int pHArray[ArrayLenght];
int pHArrayIndex = 0;
void setup()
{
  Serial.begin(9600);
  //inicializamos la com. serie(UART) a 9600 baudios
  Serial.println("Inicializamos las mediciones");
  ads1015.begin();
  ads1015.setGain(GAIN_ONE);
  pinMode(power_pin, OUTPUT);

  connectWiFi();
  digitalWrite(LED_PIN, HIGH);
#ifdef PRINT_DEBUG_MESSAGES
  Serial.print("Server_Host: ");
  Serial.println(Server_Host);
  Serial.print("Port: ");
  Serial.println(String( Server_HttpPort ));
  Serial.print("Server_Rest: ");
  Serial.println(Rest_Host);
#endif
}

float Calcular_temperatura(int channelValue)
{
  int16_t adc0 = ads1015.readADC_SingleEnded(channelValue);
  double temperatura;
  float m = 33 * pow(10, -3);
  float b = 0.95;
  double Vo = (adc0 * 4.096 / 32767);
  temperatura = ((Vo - b) / m);
  return (temperatura);
}
int Calcular_humedad(int channelValue)
{
  //Leemos del canal channelValue a través del ADS1115
  sensorValue = ads1015.readADC_SingleEnded(channelValue);
  //mapeamos el valor leido para un porcentaje entre 0 a 100
  humidityValue = map(sensorValue,22484 , 10100, 0, 100);
  //imprimimos por pantalla si esta seco o humedo
//  if (sensorValue >)
//  {
//    Serial.println("El medidor esta seco");
//  }
//  else
//  {
//    Serial.println("El medidor esta mojado");
//  }
  //limitamos los valores entre 0 y 100 para minimizar errores
  if (humidityValue >= 100)
  {
    humidityValue = 100;
  }
  if (humidityValue <= 0)
  {
    humidityValue = 0;
  }
  return (humidityValue);
}
//diseñamos la funcion de calcular salinidad
int Calcular_salinidad()
{
  int16_t adc0;
  digitalWrite(power_pin, HIGH);
  delay(100);

  adc0 = analogRead(A0);
  digitalWrite(power_pin, LOW);
  delay(100);
  //mapeamos el valor leido para un porcentaje entre 0 a 100
  Sal = map(adc0, 540, 910, 0, 100);
  //limitamos los valores entre 0 y 100 para minimizar errores
  if (Sal >= 100)
  {
    Sal = 100;
  }
  if (Sal <= 0)
  {
    Sal = 0;
  }
  return (Sal);
}
float averageSample(int lista[ArrayLenght],int channelValue)
{
  int media = 0;
  for (int i = 0; i <= (ArrayLenght - 1); i++)
  {
    lista[i]=ads1015.readADC_SingleEnded(channelValue);
    media = (lista[i] + media);
  }
  media = media / ArrayLenght;
  return (media);
}
float Calcular_Ph(int channelValue){//
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  static float pHValue, voltage;
  if (millis() - samplingTime > samplingInterval)
  {
    //pHArray[pHArrayIndex++] = ads1015.readADC_SingleEnded(channelValue);
    if (pHArrayIndex == ArrayLenght)
    {
      pHArrayIndex = 0;
    } 
    voltage = averageSample(pHArray,channelValue)* 4.096 / 32767;
//    voltage=pHArray[pHArrayIndex++];
    pHValue = 3.5 * voltage + Offset;
    samplingTime = millis();
    Serial.print("Voltage:");
    Serial.print(voltage);
    Serial.print(" pH Value:");
    Serial.println(pHValue);
  }
  if (millis() - printTime > printInterval)
  {
    Serial.print("Voltage:");
    Serial.print(voltage, 2);
    Serial.print(" pH Value:");
    Serial.println(pHValue, 2);
    printTime = millis();
  }
//  if(pHValue>=6.51)
//  {
//    pHValue=7;
//  }
  return(pHValue);
}
void EnviarDatos(int Humedad,int Sal,float Temperatura,int Ph) {

  String data[ NUM_FIELDS_TO_SEND + 1];  // Podemos enviar hasta 8 datos
  data[ 1 ] = String(Humedad); //Escribimos el dato 1. Recuerda actualizar numFields
#ifdef PRINT_DEBUG_MESSAGES
  Serial.print( "Porcentaje de humedad = " );
  Serial.print( data[ 1 ] );
  Serial.println("%");
#endif

  data[ 2 ] = String(Sal); //Escribimos el dato 2. Recuerda actualizar numFields
#ifdef PRINT_DEBUG_MESSAGES
  Serial.print( "Porcentaje de sal = " );
  Serial.print( data[ 2 ] );
  Serial.println("%");
#endif
  data[ 3 ] = String(Temperatura); //Escribimos el dato 3. Recuerda actualizar numFields
#ifdef PRINT_DEBUG_MESSAGES
  Serial.print( "Nivel de temperatura(en grados centigrados): " );
  Serial.print( data[ 3 ] );
  Serial.println("º");
#endif

  data[ 4 ] = String(Ph); //Escribimos el dato 4. Recuerda actualizar numFields
#ifdef PRINT_DEBUG_MESSAGES
  Serial.print( "Nivel de Ph = " );
  Serial.println( data[ 4 ] );
#endif


  //Selecciona si quieres enviar con GET(ThingSpeak o Dweet) o con POST(ThingSpeak)
  //HTTPPost( data, NUM_FIELDS_TO_SEND );
  HTTPGet( data, NUM_FIELDS_TO_SEND );

  //Selecciona si quieres un retardo de 15seg para hacer pruebas o dormir el SparkFun
  delay( 1500 );
  //Serial.print( "Goodnight" );
  //ESP.deepSleep( sleepTimeSeconds * 1000000 );
}

void loop()
{
  //llamamos a la funcion de calcular de humedad
  Humedad = Calcular_humedad(0);
  delay(500);
  //llamamos a la funcion de calcular de salinidad
  Sal = Calcular_salinidad();
  delay(500);
  
  //llamamos a la funcion de calcular de temperatura
  Temperatura= Calcular_temperatura(2);
  delay(500);
  
  //llamamos a la funcion de calcular de el nivel de Ph Puerto 3
  NivelPh=Calcular_Ph(3);
  
  delay(500);

  //Una vez calculados todos los datos los enviamos a la nube
  EnviarDatos(Humedad,Sal,Temperatura,NivelPh);
}
