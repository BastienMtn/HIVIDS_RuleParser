#ifndef CAN_RULES_DEFINITIONS_H
#define CAN_RULES_DEFINITIONS_H

enum Action {
    ALERT,
    BLOCK,
    DROP,
    LOG,
    PASS,
    REWRITE
};

enum Direction {
    RECEIVE,
    TRANSMIT,
    BIDIRECTIONAL
};

enum OptionType{
    UpLimit,
    DownLimit,
    Format,
    Length,
    Message,
    Contains,
};

typedef struct{
    enum OptionType type;
    void* value;
}CANSecOption;

typedef struct {
    enum Action action;
    bool extended;
    long unsigned int id;
    bool isRequest;
    enum Direction dir;
    CANSecOption *options;
} CANRule;

#endif // CAN_RULES_DEFINITIONS_H
