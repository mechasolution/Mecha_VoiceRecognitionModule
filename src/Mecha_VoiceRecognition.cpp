#include "Mecha_VoiceRecognition.h"

#include "key_types.h"

Mecha_VoiceRecognition::Mecha_VoiceRecognition(SoftwareSerial *uart_voice) {
  _uart_voice = uart_voice;
  _group = VOICE_GROUP_1;
}

void Mecha_VoiceRecognition::_flushRxBuffer() {
  while (_uart_voice->available())
    _uart_voice->read();
}

int Mecha_VoiceRecognition::_getRxResponse() {
  unsigned long start_time = millis();
  int rxData = 0;
  bool isReceive = false;
  while (true) {
    if (start_time + 1000 <= millis())
      break;
    if (_uart_voice->available()) {
      isReceive = true;
      rxData = _uart_voice->read();
    }
  }
  if (isReceive == true)
    return rxData;
  else
    return VOICE_RESPONSE_ERROR_TIMEOUT;
}

void Mecha_VoiceRecognition::_sendCommand(int data) {
  _uart_voice->write(VOICE_TRANSMIT_COMMAND_HEAD);
  _uart_voice->write(data);
}

int Mecha_VoiceRecognition::_sendCommandAndWaitSUccessResponse(int data) {
  _flushRxBuffer();
  int ret;

  for (int i = 0; i < 5; i++) {
    _uart_voice->write(VOICE_TRANSMIT_COMMAND_HEAD);
    _uart_voice->write(data);

    ret = _getRxResponse();
    if (ret == VOICE_RESPONSE_SUCCESS) {
      return ret;
    } else if (ret == VOICE_RESPONSE_ERROR_TIMEOUT) {
      _DEBUG("Voice Recognition Debug - Command Failed. Retry after 100ms");
      delay(100);
    } else {
      return ret;
    }
  }
  return VOICE_RESPONSE_ERROR_TIMEOUT;
}

bool Mecha_VoiceRecognition::init() {
  _uart_voice->begin(9600);

  while (!_uart_voice) {
    delay(100);
  }

  delay(1000);

  bool ret = true;

  ret = _sendCommandAndWaitSUccessResponse(VOICE_TRANSMIT_WAITING) == VOICE_RESPONSE_SUCCESS;
  if (ret != true) {
    _DEBUG("Voice Recognition Debug - INIT ERROR 1");
    return false;
  }
  ret = _sendCommandAndWaitSUccessResponse(VOICE_TRANSMIT_SWITCH_COMPACT) == VOICE_RESPONSE_SUCCESS;
  if (ret != true) {
    _DEBUG("Voice Recognition Debug - INIT ERROR 2");
    return false;
  }

  _DEBUG("Voice Recognition Debug - INIT SUCCESS");
  return true;
}

VOICE_InstructionTypeDef Mecha_VoiceRecognition::getInstruction() {
  char buff = _getRxResponse();
  _flushRxBuffer();
  switch (buff) {
  case VOICE_RESPONSE_INSTRUCTION_G1_S1:
  case VOICE_RESPONSE_INSTRUCTION_G2_S1:
  case VOICE_RESPONSE_INSTRUCTION_G3_S1:
    return VOICE_INSTRUCTION_1;
  case VOICE_RESPONSE_INSTRUCTION_G1_S2:
  case VOICE_RESPONSE_INSTRUCTION_G2_S2:
  case VOICE_RESPONSE_INSTRUCTION_G3_S2:
    return VOICE_INSTRUCTION_2;
  case VOICE_RESPONSE_INSTRUCTION_G1_S3:
  case VOICE_RESPONSE_INSTRUCTION_G2_S3:
  case VOICE_RESPONSE_INSTRUCTION_G3_S3:
    return VOICE_INSTRUCTION_3;
  case VOICE_RESPONSE_INSTRUCTION_G1_S4:
  case VOICE_RESPONSE_INSTRUCTION_G2_S4:
  case VOICE_RESPONSE_INSTRUCTION_G3_S4:
    return VOICE_INSTRUCTION_4;
  case VOICE_RESPONSE_INSTRUCTION_G1_S5:
  case VOICE_RESPONSE_INSTRUCTION_G2_S5:
  case VOICE_RESPONSE_INSTRUCTION_G3_S5:
    return VOICE_INSTRUCTION_5;
  case VOICE_RESPONSE_ERROR_TIMEOUT:
    return VOICE_INSTRUCTION_NOTHING;
  default:
    return VOICE_INSTRUCTION_ERROR;
  }
}
VOICE_GroupTypeDef Mecha_VoiceRecognition::getGroup() {
  return _group;
}

void Mecha_VoiceRecognition::setGroup(VOICE_GroupTypeDef group) {
  char buff[100];
  _group = group;
  sprintf(buff, "Voice Recognition Debug - Set group number as %d", _group);
  _DEBUG(buff);
}

void Mecha_VoiceRecognition::setDebugOn(HardwareSerial *uart_debug) {
  _uart_debug = uart_debug;
  _isDebugOn = true;
  _DEBUG("Voice Recognition Debug - Set serial debug on");
}

void Mecha_VoiceRecognition::setDebugOff() {
  _DEBUG("Voice Recognition Debug - Set serial debug off");
  _isDebugOn = false;
}

bool Mecha_VoiceRecognition::setMode(VOICE_ModeTypeDef mode) {
  bool ret = false;

  switch (mode) {
  case VOICE_MODE_WAITING:
    ret = _sendCommandAndWaitSUccessResponse(VOICE_TRANSMIT_WAITING) == VOICE_RESPONSE_SUCCESS;
    break;
  case VOICE_MODE_RECORDING:
    ret = _record();
    break;
  case VOICE_MODE_RECOGNITION:
    ret = _import();
    break;

  default:
    break;
  }

  return ret;
}

void Mecha_VoiceRecognition::_DEBUG(const char *message) {
  if (_isDebugOn == true)
    _uart_debug->println(message);
}

bool Mecha_VoiceRecognition::_record() {
  int i = 1;
  char buff[100];
  bool need_to_off_debug = false;

  bool ret = false;

  switch (_group) {
  case VOICE_GROUP_1:
    ret = _sendCommandAndWaitSUccessResponse(VOICE_TRANSMIT_DELETE_G1) == VOICE_RESPONSE_SUCCESS;
    _sendCommand(VOICE_TRANSMIT_RECORD_G1);
    break;
  case VOICE_GROUP_2:
    ret = _sendCommandAndWaitSUccessResponse(VOICE_TRANSMIT_DELETE_G2) == VOICE_RESPONSE_SUCCESS;
    _sendCommand(VOICE_TRANSMIT_RECORD_G2);
    break;
  case VOICE_GROUP_3:
    ret = _sendCommandAndWaitSUccessResponse(VOICE_TRANSMIT_DELETE_G3) == VOICE_RESPONSE_SUCCESS;
    _sendCommand(VOICE_TRANSMIT_RECORD_G3);
    break;
  }

  if (ret == false) {
    return false;
  }

  sprintf(buff, "Starts recording group %d", _group);
  _DEBUG(buff);
  while (true) {
    int response = -1;
    bool isFinish = false;

    if (_uart_voice->available()) {
      response = _uart_voice->read();
    }
    switch (response) {
    case VOICE_RESPONSE_RECORD_START:
      sprintf(buff, "(%d/5) SPEAK", i);
      _DEBUG(buff);
      break;
    case VOICE_RESPONSE_RECORD_NO_VOICE:
      _DEBUG("No voice, repeat after \"SPEAK\" sign.");
      break;
    case VOICE_RESPONSE_RECORD_AGAIN:
      _DEBUG("Repeat after \"SPEAK\" sign.");
      break;
    case VOICE_RESPONSE_RECORD_TOO_LOUD:
      _DEBUG("Too loud, repeat after \"SPEAK\" sign.");
      break;
    case VOICE_RESPONSE_RECORD_DIFFERENT:
      _DEBUG("Confirming failed, repeat after \"SPEAK\" sign.");
      break;
    case VOICE_RESPONSE_RECORD_FINISH_ONE:
      sprintf(buff, "%d of 5 finished, ready for record next instruction after \"SPEAK\" sign.", i);
      _DEBUG(buff);
      i++;
      break;
    case VOICE_RESPONSE_RECORD_FINISH_G1:
    case VOICE_RESPONSE_RECORD_FINISH_G2:
    case VOICE_RESPONSE_RECORD_FINISH_G3:
      _DEBUG("Finished recording");
      isFinish = true;
      break;
    }
    if (isFinish == true)
      break;
  }

  return true;
}

bool Mecha_VoiceRecognition::_import() {
  char buff[100];
  int ret;

  switch (_group) {
  case VOICE_GROUP_1:
    ret = _sendCommandAndWaitSUccessResponse(VOICE_TRANSMIT_IMPORT_G1);
    break;
  case VOICE_GROUP_2:
    ret = _sendCommandAndWaitSUccessResponse(VOICE_TRANSMIT_IMPORT_G2);
    break;
  case VOICE_GROUP_3:
    ret = _sendCommandAndWaitSUccessResponse(VOICE_TRANSMIT_IMPORT_G3);
    break;
  }

  if (ret != VOICE_RESPONSE_SUCCESS) {
    _DEBUG("Voice Recognition Debug - Failed to import group");

    if (ret == VOICE_RESPONSE_ERROR_IMPORING_FAILED) {
      _DEBUG("Voice Recognition Debug - maybe theres no record data for this group");
    }

    return false;
  }

  sprintf(buff, "Voice Recognition Debug - Import group %d, starts listening", _group);
  _DEBUG(buff);

  return true;
}
