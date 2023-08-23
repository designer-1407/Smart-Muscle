#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>

#define MCP3421_ADDR 0x68 

//Each new sensor has unic ID starting from 2
//ID 1 has main ESP
#define BOARD_ID 1  

const int analogInPin = A0;
int emg_sensor_data = 0;

//MAC Address of receiver
uint8_t broadcastAddress[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //!!

//Must match the receiver structure
typedef struct struct_message {
  int id;
  int temp;
  int bettery;
} struct_message;

// Create a struct_message called EMGsensor to hold sensor readings
struct_message EMGsensor;

unsigned long previousMillis = 0;   // Stores last time temperature was published
const long interval = 50;        // Interval at which to publish sensor readings

unsigned int readingId = 0;

// SSID of wi-fi network
constexpr char WIFI_SSID[] = "Electronic_mob";

int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
    for (uint8_t i=0; i<n; i++) {
      if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
        return WiFi.channel(i);
      }
    }
  }
  return 0;
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


// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  if (sendStatus == 0){
    Serial.print("Sensor ");
    Serial.print(BOARD_ID);
    Serial.print(" :");
    Serial.println(EMGsensor.temp);
  }
  else{
    Serial.println("Delivery fail");
  }
}

int getReadings(){
  // Read emg data
 return(analogRead(analogInPin));
}

void setup () {
  // Serial port for debugging purposes
  Serial.begin (115200);
  Wire.begin();
  write_ADC_config(MCP3421_ADDR, 0x94); //10010100

  // Set device as a Wi-Fi Station and set channel
  WiFi.mode(WIFI_STA);

  int32_t channel = getWiFiChannel(WIFI_SSID);
  Serial.println(channel);
  
  WiFi.printDiag(Serial); // Uncomment to verify channel number before
  wifi_promiscuous_enable(1);
  wifi_set_channel(channel);
  wifi_promiscuous_enable(0);
  WiFi.printDiag(Serial); // Uncomment to verify channel change after

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
   esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  esp_now_register_send_cb(OnDataSent);
  
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}
void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // Save the last time a new reading was published
    previousMillis = currentMillis;
    //Set values to send
    EMGsensor.id = BOARD_ID;
    EMGsensor.temp = read_ADC_data(MCP3421_ADDR);
    EMGsensor.bettery = getReadings();
     
    esp_now_send(broadcastAddress, (uint8_t *) &EMGsensor, sizeof(EMGsensor));
  }
}
