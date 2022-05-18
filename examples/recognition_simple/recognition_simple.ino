#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Mecha_VoiceRecognition.h>

#define PIN_LED_GREEN 4
#define PIN_LED_YELLOW 5
#define PIN_LED_RED 6

SoftwareSerial uart_voice(2, 3);
Mecha_VoiceRecognition voice(&uart_voice);

void setup() {
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_YELLOW, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);

  Serial.begin(9600);
  
  voice.setDebugOn(&Serial);
  voice.setGroup(VOICE_GROUP_1);
  voice.init();

  voice.setMode(VOICE_MODE_RECOGNITION);
}

void loop() {
  VOICE_InstructionTypeDef response = voice.getInstruction();
  switch(response) {
    case VOICE_INSTRUCTION_1:
      Serial.println("Red");
      digitalWrite(PIN_LED_GREEN, LOW);
      digitalWrite(PIN_LED_YELLOW, LOW);
      digitalWrite(PIN_LED_RED, HIGH);
      break;
    case VOICE_INSTRUCTION_2:
      Serial.println("Yellow");
      digitalWrite(PIN_LED_GREEN, LOW);
      digitalWrite(PIN_LED_YELLOW, HIGH);
      digitalWrite(PIN_LED_RED, LOW);
      break;
    case VOICE_INSTRUCTION_3:
      Serial.println("Green");
      digitalWrite(PIN_LED_GREEN, HIGH);
      digitalWrite(PIN_LED_YELLOW, LOW);
      digitalWrite(PIN_LED_RED, LOW);
      break;
    case VOICE_INSTRUCTION_4:
      Serial.println("White");
      digitalWrite(PIN_LED_GREEN, HIGH);
      digitalWrite(PIN_LED_YELLOW, HIGH);
      digitalWrite(PIN_LED_RED, HIGH);
      break;
    case VOICE_INSTRUCTION_5:
      Serial.println("Black");
      digitalWrite(PIN_LED_GREEN, LOW);
      digitalWrite(PIN_LED_YELLOW, LOW);
      digitalWrite(PIN_LED_RED, LOW);
      break;
    case VOICE_INSTRUCTION_ERROR:
      Serial.println("error");
      break;
    case VOICE_INSTRUCTION_NOTHING:
      // no data recieved
      break;
  }
}