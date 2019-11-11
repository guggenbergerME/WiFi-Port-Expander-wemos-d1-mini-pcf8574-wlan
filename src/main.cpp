/*
PINOUT Wemos D1 -> https://bre.is/9H5nddhG
*/

#include <Arduino.h>
#include "    .h"
#include <SPI.h>
#include <ESP8266WiFi.h> 
#include <PubSubClient.h>  
#include <DallasTemperature.h>
#include <OneWire.h>



////////////////////////////////////////////////////////////////////// PCF8574 Adresse
PCF8574 pcf8574(0x20);
//PCF8574 pcf8574_2(0x21);


///////////////////////////////////////////////////////////////////// WIRE Bus
#define ONE_WIRE_BUS D1
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
/////////////////////////////////////////////////////////////////////////// mqtt

///////////////////////////////////////////////////////////////////// DS18B20 ansteuern

DeviceAddress Thermometer;
int deviceCount = 0;
float tempC; 
String dataString = "";
/////////////////////////////////////////////////////////////////////

// Kartendaten
const char* kartenID = "WIFI_Sensor_XX";
char mqtt_text[10];

/////////////////////////////////////////////////////////////////////////// Wifi einrichten
WiFiClient espClient;
PubSubClient client(espClient);

/////////////////////////////////////////////////////////////////////////// Intervall der Steuerung
unsigned long previousMillis_Temperatur = 0;
unsigned long interval_Temperatur = 30000; 
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////// Variablen sonstige
char buffer1[100];
char buffer2[100];
/////////////////////////////////////////////////////////////////////

// Connect to the WiFi
const char* ssid = "GuggenbergerLinux";
const char* password = "xxx";
const char* mqtt_server = "192.168.150.1";

///////////////////////////////////////////////////////////////////////////

void callback(char* topic, byte* payload, unsigned int length) {

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Verbindung zu mqtt IP: ");
    Serial.print(mqtt_server);
    Serial.println("");
    // Create a random client ID
    String clientId = "Wifi-Tastenerkennung-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("Verbunden ....");
      // ... and resubscribe
          
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



//////////////////////////////////////////////////////////////////////////////////   SETUP
void setup() {

/////////////////////////////////////////////////////////////////////////// Serial
  Serial.begin(115200);

/////////////////////////////////////////////////////////////////////////// mqtt Server
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

/////////////////////////////////////////////////////////////////////////// Sensor Start
  sensors.begin();
  Serial.println("Locating devices...");
  Serial.print("Found ");
  deviceCount = sensors.getDeviceCount();
  Serial.print(deviceCount, DEC);
  Serial.println(" devices.");
  Serial.println("");
///////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////// Konfig Relais
  pcf8574.pinMode(P0, OUTPUT);
  pcf8574.pinMode(P1, OUTPUT);
  pcf8574.pinMode(P2, OUTPUT);
  pcf8574.pinMode(P3, OUTPUT);
  pcf8574.pinMode(P4, OUTPUT);
  pcf8574.pinMode(P5, OUTPUT);
  pcf8574.pinMode(P6, OUTPUT);
  pcf8574.pinMode(P7, OUTPUT);        
  pcf8574.begin();
/////////////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////////////// Relais OFF
pcf8574.digitalWrite(P0, !LOW);
pcf8574.digitalWrite(P1, !LOW);
pcf8574.digitalWrite(P2, !LOW);
pcf8574.digitalWrite(P3, !LOW);
pcf8574.digitalWrite(P4, !LOW);
pcf8574.digitalWrite(P5, !LOW);
pcf8574.digitalWrite(P6, !LOW);
pcf8574.digitalWrite(P7, !LOW);
/////////////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////////////// Wifi

  Serial.print("Verbindung zu SSID -> ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
/*
  IPAddress ip(192, 168, 33, 90);
	IPAddress gateway(192, 168, 1, 1);
	IPAddress subnet(255, 255, 255, 0);
	IPAddress dns(192, 168, 1, 1);
	WiFi.config(ip, dns, gateway, subnet);
*/
  // Wifi AP deaktivieren
  WiFi.mode(WIFI_STA);

  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Erfolgreich mit dem WiFi verbunden!");
  Serial.print("DHCP Adresse der Karte : ");
  Serial.println(WiFi.localIP());
  Serial.print("ID der Karte: ");
  Serial.println(kartenID);

}

//************************************************************************* Wire Adresse ausgeben 
void printAddress(DeviceAddress deviceAddress)
{ 
  Serial.println("Adresse Temp. auslesen");
  dataString = "WIFI_Sensor_01/Temperatur/";
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 0x10) dataString += String("0");
    Serial.print(deviceAddress[i], HEX);
    dataString += String(deviceAddress[i], HEX);
    if (i < 7) Serial.print("_");
    dataString += String("_");
  }
  dataString += String("/");
  Serial.println("");
}

/////////////////////////////////////////////////////////////////////////// Temp auslesen
void temperaturAUSLESEN(void){
Serial.println("Temperatur auslesen");
    for (int i = 0;  i < deviceCount;  i++)
  {
    sensors.requestTemperatures();
    sensors.getAddress(Thermometer, i);
    printAddress(Thermometer);

    tempC = sensors.getTempCByIndex(i);

//////////////////////////////////////////////////////////////////////////// Ausgaben auf mqtt
    dtostrf(tempC,2, 1, buffer2); 
    char topic[100];
    dataString.toCharArray(topic, sizeof(topic));
    client.publish(topic, buffer2); 

  }

}

//****************************************************************************************** VOID LOOP
void loop()
{
  // mqtt Daten senden     
  if (!client.connected()) {
      reconnect();
    }
    client.loop();  

/*
  ///////////////////////////////////////////////////////////////////////// Daten schreiben im Intervall Temperatur
  if (millis() - previousMillis_Temperatur > interval_Temperatur) {

      previousMillis_Temperatur = millis();   // aktuelle Zeit abspeichern
 
   // Temperatur auslesen
   temperaturAUSLESEN();
    }
*/


pcf8574.digitalWrite(P0, !LOW);
pcf8574.digitalWrite(P1, !LOW);
pcf8574.digitalWrite(P2, !LOW);
pcf8574.digitalWrite(P3, !LOW);
delay(1000);
pcf8574.digitalWrite(P0, !HIGH);
pcf8574.digitalWrite(P1, !HIGH);
pcf8574.digitalWrite(P2, !HIGH);
pcf8574.digitalWrite(P3, !HIGH);
delay(1000);
}
