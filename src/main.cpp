/*
PINOUT Wemos D1 -> https://bre.is/9H5nddhG
*/

#include <Arduino.h>
#include "    .h"
#include <SPI.h>
#include <ESP8266WiFi.h> 
#include <PubSubClient.h>  

////////////////////////////////////////////////////////////////////// PCF8574 Adresse
PCF8574 pcf8574(0x20);
////////////////////////////////////////////////////////////////////// Der nächste PCF8574
//PCF8574 pcf8574_2(0x21);
// Beachter die Adressjumper auf dem PCF8574

////////////////////////////////////////////////////////////////////// Kartendaten
const char* kartenID = "WIFI_Relaiskarte_XXX";
char mqtt_text[10];

/////////////////////////////////////////////////////////////////////////// Wifi einrichten
WiFiClient espClient;
PubSubClient client(espClient);

// Connect to the WiFi
const char* ssid = "SSID";
const char* password = "PASS";
const char* mqtt_server = "IPtoMQTT";

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
    String clientId = "Wifi-Relaiskarte-";
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


//****************************************************************************************** VOID LOOP
void loop()
{
  // mqtt Daten senden     
  if (!client.connected()) {
      reconnect();
    }
    client.loop();  

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
