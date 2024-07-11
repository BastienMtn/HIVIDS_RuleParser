#ifndef CAN_RULES_H
#define CAN_RULES_H

#include "cansec_rules.h"

CANRule ruleTable[] = {
    { ALERT, false, 256, false, TRANSMIT, {{UpLimit,"255"},{DownLimit,"0"},{Length,"8"},{Message,"Rule 1 : Alert on ID 100"}},4},
    { DROP, false, 272, false, TRANSMIT, {{UpLimit,"1024"},{DownLimit,"-1024"},{Length,"8"},{Format,"FFFF0000FF00FF00"}},4},
    { BLOCK, false, 384, false, BIDIRECTIONAL, {{UpLimit,"512"},{DownLimit,"0"},{Length,"8"}},3},
};

int ruleCount = 3;

#endif // CAN_RULES_H
