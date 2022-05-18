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

// module transmit data enum
typedef enum {
    VOICE_TRANSMIT_COMMAND_HEAD = 0xAA,
    
    VOICE_TRANSMIT_WAITING = 0x00,

    VOICE_TRANSMIT_RECORD_G1 = 0x11,
    VOICE_TRANSMIT_RECORD_G2 = 0x12,
    VOICE_TRANSMIT_RECORD_G3 = 0x13,

    VOICE_TRANSMIT_IMPORT_G1 = 0x21,
    VOICE_TRANSMIT_IMPORT_G2 = 0x22,
    VOICE_TRANSMIT_IMPORT_G3 = 0x23,

    VOICE_TRANSMIT_SWITCH_COMMON = 0x36,
    VOICE_TRANSMIT_SWITCH_COMPACT = 0x37,

    VOICE_TRANSMIT_QUERY_VERSION = 0xBB
} VOICE_TransmitTypeDef;

// module response data enum
typedef enum {
    VOICE_RESPONSE_SUCCESS = 0xCC,
    VOICE_RESPONSE_ERROR_TIMEOUT = -1,
    VOICE_RESPONSE_ERROR_INSTRUCTION = 0xE0,
    VOICE_RESPONSE_ERROR_IMPORING_FAILED = 0xE1,

    VOICE_RESPONSE_RECORD_START = 0x40,
    VOICE_RESPONSE_RECORD_NO_VOICE = 0x41,
    VOICE_RESPONSE_RECORD_AGAIN = 0x42,
    VOICE_RESPONSE_RECORD_TOO_LOUD = 0x43,
    VOICE_RESPONSE_RECORD_DIFFERENT = 0x44,
    VOICE_RESPONSE_RECORD_FINISH_ONE = 0x45,
    VOICE_RESPONSE_RECORD_FINISH_G1 = 0x46,
    VOICE_RESPONSE_RECORD_FINISH_G2 = 0x47,
    VOICE_RESPONSE_RECORD_FINISH_G3 = 0x48,

    VOICE_RESPONSE_INSTRUCTION_G1_S1 = 0x11,
    VOICE_RESPONSE_INSTRUCTION_G2_S1 = 0x21,
    VOICE_RESPONSE_INSTRUCTION_G3_S1 = 0x31,

    VOICE_RESPONSE_INSTRUCTION_G1_S2 = 0x12,
    VOICE_RESPONSE_INSTRUCTION_G2_S2 = 0x22,
    VOICE_RESPONSE_INSTRUCTION_G3_S2 = 0x32,

    VOICE_RESPONSE_INSTRUCTION_G1_S3 = 0x13,
    VOICE_RESPONSE_INSTRUCTION_G2_S3 = 0x23,
    VOICE_RESPONSE_INSTRUCTION_G3_S3 = 0x33,

    VOICE_RESPONSE_INSTRUCTION_G1_S4 = 0x14,
    VOICE_RESPONSE_INSTRUCTION_G2_S4 = 0x24,
    VOICE_RESPONSE_INSTRUCTION_G3_S4 = 0x34,

    VOICE_RESPONSE_INSTRUCTION_G1_S5 = 0x15,
    VOICE_RESPONSE_INSTRUCTION_G2_S5 = 0x25,
    VOICE_RESPONSE_INSTRUCTION_G3_S5 = 0x35,
} VOICE_ResponseTypeDef;

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
