/**
 * @file record.ino
 * @author Mechasolution Tech Support Team (techms5499@gmail.com)
 * @brief record example
 * @version 0.2
 * @date 2024-06-04
 *
 * @copyright Copyright (c) 2024 Mechasolution (https://mechasolution.com)
 *
 */

#include <SoftwareSerial.h>

#include <Mecha_VoiceRecognition.h>

SoftwareSerial uart_voice(2, 3);
Mecha_VoiceRecognition voice(uart_voice);

void setup() {
  Serial.begin(9600);
  uart_voice.begin(9600);

  voice.setDebugOn(Serial);
  voice.setGroup(VOICE_GROUP_1);

  if (voice.begin() == false) {
    Serial.println("Init error!");
    Serial.println("Please check wiring");
    while (true) {
    }
  }

  if (voice.setMode(VOICE_MODE_RECORDING) == false) {
    Serial.println("Communication error!");
    Serial.println("Press reset button on dev board might be helpful");
    while (true) {
    }
  }
}

void loop() {
}
