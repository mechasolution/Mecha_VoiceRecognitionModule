#ifndef Mecha_VOICERECOGNITION
#define Mecha_VOICERECOGNITION

#include <Arduino.h>
#include <SoftwareSerial.h>

// mode enum
typedef enum {
    VOICE_MODE_WAITING,
    VOICE_MODE_RECORDING,
    VOICE_MODE_RECOGNITION,
} VOICE_ModeTypeDef;

// group enum
typedef enum {
    VOICE_GROUP_1 = 1,
    VOICE_GROUP_2 = 2,
    VOICE_GROUP_3 = 3
} VOICE_GroupTypeDef;

// getInstruction return data enum
typedef enum {
    VOICE_INSTRUCTION_1,
    VOICE_INSTRUCTION_2,
    VOICE_INSTRUCTION_3,
    VOICE_INSTRUCTION_4,
    VOICE_INSTRUCTION_5,
    VOICE_INSTRUCTION_NOTHING,
    VOICE_INSTRUCTION_ERROR
} VOICE_InstructionTypeDef;

class Mecha_VoiceRecognition {
public:
    Mecha_VoiceRecognition(SoftwareSerial *uart_voice);
    void init();

    void setGroup(VOICE_GroupTypeDef group);
    void setDebugOn(HardwareSerial *uart_debug);
    void setDebugOff();
    void setMode(VOICE_ModeTypeDef mode);

    VOICE_InstructionTypeDef getInstruction();
    VOICE_GroupTypeDef getGroup();
    VOICE_ModeTypeDef getMode();


private:
    void _DEBUG(const char *message);

    void _flushRxBuffer();
    int _getRxResponse();
    void _sendCommand(int data);

    void _record();
    void _import();
    SoftwareSerial *_uart_voice;
    HardwareSerial *_uart_debug;
    VOICE_GroupTypeDef _group;

    bool _isDebugOn = false;
};

#endif
