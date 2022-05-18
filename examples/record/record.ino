#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Mecha_VoiceRecognition.h"

SoftwareSerial uart_voice(2, 3);
Mecha_VoiceRecognition voice(&uart_voice);

void setup() {
  Serial.begin(9600);
  
  voice.setDebugOn(&Serial);
  voice.setGroup(VOICE_GROUP_1);
  voice.init();

  voice.setMode(VOICE_MODE_RECORDING);
}

void loop() {
}