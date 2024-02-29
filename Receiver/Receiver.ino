
#include <esp_now.h>
#include <WiFi.h>

#include <Stepper.h>

const inst stepsPerRevolution = 140;
const int stepPin = 1;
const int dirPin = 3;
int mappedValue;

const int maxSteps = 250;
const int minSteps = -250;
Stepper myStepper(stepsPerRevolution, stepPin, dirPin);

int js_val;

//Structure example to receive data
//Must match the sender structure
tpredef struct struct_message{

  float JS_Reading;

} struct_message;

struct_message myData;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len){
  memcpy(&myData, incomingData, sizeof(myData));

  js_val = myData.JS_Reading;
  Serial.print("JS Reading: ");
  Serial.println(js_val);
  Serial.println();

}


void setup(){

  //Initialize Serial Monitor
  //Serial.begin(115200);
  myStepper.setSpeed(1000);


  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() !=ESP_OK){
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  //Once ESPNow is successfully Init, we will register for recv CB to
  //get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

}

void loop(){

  int Value = myData.JS_Reading;
  //int val = js_val;
  mappedValue = map(Value, 0, 4095, 0, 1000);
  //Serial.println(mappedValue);

  static int stepCount = 0;
  if (mappedValue > 700){
    //Check if the maximum step count is reached
    if (stepCount < maxSteps){
      myStepper.step(10);
      stepCount++;
    }
  }
  else if (mappedValue < 300){
    //Check if the maximum step count is reached
    if (stepCount > minSteps){
      myStepper.step(-10);
      stepCount--;
    }
  }

  else {
    while (stepCount > 0){
      myStepper.step(-10);
      stepCount--;
    }
    while (stepCount < 0){
      myStepper.step(10);
      stepCount++;
    }
  }
  
}