// library for sending data to Zabbix server
#include <ESP8266ZabbixSender.h>
ESP8266ZabbixSender zSender;

// https://github.com/beegee-tokyo/DHTesp
// library for reading the DHT family of temperature and humidity sensors
#include "DHTesp.h"
DHTesp dht;

/* WiFi settings */
String ssid = "PUT_HERE_WIFI_NET_NAME";
String pass = "PUT_HERE_WIFI_NET_PASS";

/* Zabbix server setting */
#define SERVERADDR 127, 0, 0, 1     // Zabbix server Address, exemple 109.195.131.40 as 109, 195, 131, 40
#define ZABBIXPORT 10051            // Zabbix erver Port
#define ZABBIXAGHOST "ESP_BOARD_01" // Zabbix item's host name

boolean checkConnection();

void setup() {

  // Set Built-in LED to output
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.begin(ssid.c_str(), pass.c_str());
  while (!checkConnection()) {
  }

  // Init DHT sensor
  // https://www.wemos.cc/en/latest/d1/d1_mini_lite.html
  dht.setup(16, DHTesp::DHT22); // Connect DHT sensor to GPIO (WEMOS Board pin D0, GPIO 16)
  
  // Init zabbix server information
  zSender.Init(IPAddress(SERVERADDR), ZABBIXPORT, ZABBIXAGHOST);
}

void loop() {
  digitalWrite(LED_BUILTIN, LOW); // turn the LED on
  static float WIFI_level;
  static float Temperature;
  static float Humidity;
  
  // calculate Wifi level RSSI to percent
  if (WiFi.RSSI() <= -100) {
    WIFI_level = 0;
  } 
  else if (WiFi.RSSI() >= -50){
    WIFI_level = 100;
  }
  else {
    WIFI_level = 2 * (WiFi.RSSI() + 100);
  }

  Temperature = dht.getTemperature();
  Humidity = dht.getHumidity();
  


  checkConnection();                // Check wifi connection
  zSender.ClearItem();              // Clear ZabbixSender's item list
  
  zSender.AddItem("WIFI_level", (float)WIFI_level); // Exmaple value of zabbix trapper item
  zSender.AddItem("Temperature", (float)Temperature); // Exmaple value of zabbix trapper item
  zSender.AddItem("Humidity", (float)Humidity); // Exmaple value of zabbix trapper item
  
  if (zSender.Send() == EXIT_SUCCESS) {     // Send zabbix items
    Serial.println("ZABBIX SEND: OK");
  } else {
    Serial.println("ZABBIX SEND: NG");
  }
  
  digitalWrite(LED_BUILTIN, HIGH); // turn the LED off
  delay(60000); // wait 60sec
}

boolean checkConnection() {
  int count = 0;
  Serial.print("Waiting for Wi-Fi connection");
  while (count < 300) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.println("Connected!");
      return (true);
    }
    delay(500);
    Serial.print(".");
    count++;
  }
  Serial.println("Timed out.");
  return false;
}
