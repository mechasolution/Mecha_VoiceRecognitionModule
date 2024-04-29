#include "Mecha_VoiceRecognition.h"

#include "key_types.h"

Mecha_VoiceRecognition::Mecha_VoiceRecognition(SoftwareSerial *uart_voice) {
    _uart_voice = uart_voice;
    _group = VOICE_GROUP_1;
}

void Mecha_VoiceRecognition::_flushRxBuffer() {
    while(_uart_voice->available())
        _uart_voice->read();
}

int Mecha_VoiceRecognition::_getRxResponse() {
    unsigned long start_time = millis();
    int rxData = 0;
    bool isReceive = false;
    while(true) {
        if(start_time + 1000 <= millis())
            break;
        if(_uart_voice->available()) {
            isReceive = true;
            rxData = _uart_voice->read();
        }
    }
    if(isReceive == true) return rxData;
    else return -1;

}

void Mecha_VoiceRecognition::_sendCommand(int data) {
    delay(3000);
    _uart_voice->write(VOICE_TRANSMIT_COMMAND_HEAD);
    _uart_voice->write(data);
}

void Mecha_VoiceRecognition::init() {
    _uart_voice->begin(9600);
    _sendCommand(VOICE_TRANSMIT_WAITING);
    switch(_getRxResponse()) {
        case VOICE_RESPONSE_ERROR_TIMEOUT:
            _DEBUG("VOICE_RESPONSE_ERROR_TIMEOUT");
            break;
        case VOICE_RESPONSE_SUCCESS:
            _DEBUG("VOICE_RESPONSE_SUCCESS");
            break;
        case VOICE_RESPONSE_ERROR_INSTRUCTION:
            _DEBUG("VOICE_RESPONSE_ERROR_INSTRUCTION");
            break;
    }
    _sendCommand(VOICE_TRANSMIT_SWITCH_COMPACT);
    switch(_getRxResponse()) {
        case VOICE_RESPONSE_ERROR_TIMEOUT:
            _DEBUG("VOICE_RESPONSE_ERROR_TIMEOUT");
            break;
        case VOICE_RESPONSE_SUCCESS:
            _DEBUG("VOICE_RESPONSE_SUCCESS");
            break;
        case VOICE_RESPONSE_ERROR_INSTRUCTION:
            _DEBUG("VOICE_RESPONSE_ERROR_INSTRUCTION");
            break;
    }
}

VOICE_InstructionTypeDef Mecha_VoiceRecognition::getInstruction() {
    char buff = _getRxResponse();
    _flushRxBuffer();
    switch(buff) {
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
    sprintf(buff, "Set group number as %d", _group);
    _DEBUG(buff);
}

void Mecha_VoiceRecognition::setDebugOn(HardwareSerial *uart_debug) {
    _uart_debug = uart_debug;
    _isDebugOn = true;
    _DEBUG("Set serial debug on");
}

void Mecha_VoiceRecognition::setDebugOff() {
    _isDebugOn = false;
}

void Mecha_VoiceRecognition::setMode(VOICE_ModeTypeDef mode) {
    switch(mode) {
        case VOICE_MODE_WAITING:
            _sendCommand(VOICE_TRANSMIT_WAITING);
            _flushRxBuffer();
            break;
        case VOICE_MODE_RECORDING:
            _record();
            _flushRxBuffer();
            break;
        case VOICE_MODE_RECOGNITION:
            _import();
            break;
    }
}

void Mecha_VoiceRecognition::_DEBUG(const char *message) {
    if(_isDebugOn == true)
        Serial.println(message);
}

void Mecha_VoiceRecognition::_record() {
    int i = 1;
    char buff[100];
    switch(_group) {
        case VOICE_GROUP_1:
            _sendCommand(VOICE_TRANSMIT_RECORD_G1);
            break;
        case VOICE_GROUP_2:
            _sendCommand(VOICE_TRANSMIT_RECORD_G2);
            break;
        case VOICE_GROUP_3:
            _sendCommand(VOICE_TRANSMIT_RECORD_G3);
            break;
    }
    sprintf(buff, "Starts recording group %d", _group);
    _DEBUG(buff);
    while(true) {
        int response = -1;
        bool isFinish = false;

        if(_uart_voice->available()) {
            response = _uart_voice->read();
        }
        switch(response) {
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
        if(isFinish == true)
            break;
    }
}

void Mecha_VoiceRecognition::_import() {
    char buff[100];
    switch(_group) {
        case VOICE_GROUP_1:
            _sendCommand(VOICE_TRANSMIT_IMPORT_G1);
            break;
        case VOICE_GROUP_2:
            _sendCommand(VOICE_TRANSMIT_IMPORT_G2);
            break;
        case VOICE_GROUP_3:
            _sendCommand(VOICE_TRANSMIT_IMPORT_G3);
            break;
    }
    sprintf(buff, "Import group %d, starts listening", _group);
    _DEBUG(buff);

    switch(_getRxResponse()) {
        case VOICE_RESPONSE_ERROR_TIMEOUT:
            _DEBUG("VOICE_RESPONSE_ERROR_TIMEOUT");
            break;
        case VOICE_RESPONSE_SUCCESS:
            _DEBUG("VOICE_RESPONSE_SUCCESS");
            break;
        case VOICE_RESPONSE_ERROR_INSTRUCTION:
            _DEBUG("VOICE_RESPONSE_ERROR_INSTRUCTION");
            break;
        case VOICE_RESPONSE_ERROR_IMPORING_FAILED: // Importing Failed
            _DEBUG("VOICE_RESPONSE_ERROR_IMPORING_FAILED");
            break;
    }
}