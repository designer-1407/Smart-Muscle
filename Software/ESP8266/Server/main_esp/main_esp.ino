#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include <FS.h>
#include <Wire.h>
#include <espnow.h>
#define MCP3421_ADDR 0x68


const char* ssid = "Electronic_mob";
const char* password = "Jupiter2020";

#define BOARD_ID 0
#define Nmbr_Devs 5

unsigned long startTime;
const int analogInPin = A0;

typedef struct struct_message {
  int id;
  int temp;
  int bettery;
} struct_message;

struct_message incomingReadings;
struct_message devices[Nmbr_Devs];


AsyncWebServer server(80);

void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  devices[incomingReadings.id] = incomingReadings;
}

String getSensorReadings() {
  int time_of_pack = (millis() - startTime)/10;
  String message = String(time_of_pack) + ";";

  devices[BOARD_ID].id = BOARD_ID;
  devices[BOARD_ID].temp = read_ADC_data(MCP3421_ADDR);
  devices[BOARD_ID].bettery = analogRead(analogInPin);
  
  for(int i = 0; i < Nmbr_Devs; i++){
    message += String(devices[i].id) + "|" + String(devices[i].temp) + "|" + String(devices[i].bettery) + ";";
  }
  Serial.println(devices[0].temp);
  Serial.println(message);
  return String(message);
}

void write_ADC_config(int dev_addr, byte set) {
  Wire.beginTransmission(dev_addr);
  Wire.write(set);
  Wire.endTransmission();
}

int read_ADC_data(int dev_addr) {
  Wire.requestFrom(dev_addr, 3);
  byte c[3];
  int vadc;
  
  if (Wire.available() >= 3) {
    c[0] = Wire.read();
    c[1] = Wire.read();
    c[2] = Wire.read();
    vadc = (c[0] << 8) | c[1];
  }
  return vadc;
}

void setup () {
  // Serial port for debugging purposes
  Serial.begin (115200);
  Wire.begin();
  write_ADC_config(MCP3421_ADDR, 0x94); //10010100

  for(int i = 0; i < Nmbr_Devs; i++){
    devices[i].id = i;
    devices[i].temp = i;
    devices[i].bettery = i;
  }

  // Initialize SPIFFS
  if (! SPIFFS.begin ()) {
    Serial.println ("An Error has occurred while mounting SPIFFS");
    return;
  }

  WiFi.mode(WIFI_AP_STA);
  Serial.print("Setting AP (Access Point)…");
  
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb(OnDataRecv);

  server.on ("/", HTTP_GET, [] (AsyncWebServerRequest * request) {
    request-> send (SPIFFS, "/index.html");
  });
  server.on ("/emg", HTTP_GET, [] (AsyncWebServerRequest * request) {
    request-> send_P (200, "text / plain", getSensorReadings(). c_str ());
  });

  // start server
  server.begin ();

  startTime = millis();
}
void loop() {
}
