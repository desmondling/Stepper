/*
---- Transmitter Code ----
*/

#include <esp_now.h>                      //the library which enable us to use ESP32 board
#include <WiFi.h>                         //the library which enable us to use wifi protocol of ESP32

#define JoyStick_X A0

unint8_t broadcastAddress[] = {0x10, 0x97, 0xBD, 0x19, 0x62, 0xBC};

typedef struct struct_message {

  float JS_Reading;

} struct_message;

struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup(){
  
  //Init Serial Monitor
  Serial.begin(115200);

  //Set device as a Wi_Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() !=ESP_OK){
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  //Once ESPNow is successfully Init, we will register for Send CB to
  //get the status of Transmitted packet
  esp_now_register_send_cb(OnDataSent);

  //Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  //Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  void loop(){
    //put main code here, to run repeatedly:

    float JS;
    JS=analogRead(JoyStick_X);
    myData.JS_Reading = JS;
    Serial.println(myData.JS_Reading);
    //Serial.println(x)


    //Send message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

    if (result == ESP_OK){
      Serial.println("Sent with success");
    }
    else{
      Serial.println("Error sending the data");
    }
  }
