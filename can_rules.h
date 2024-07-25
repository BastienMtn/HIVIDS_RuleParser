#ifndef CAN_RULES_H
#define CAN_RULES_H

#include "cansec_rules.h"

CANRule ruleTable[] = {
    { ALERT, false, 256, false, TRANSMIT, {{Message,"Rule 1 : Alert on ID 100"},{UpLimit,"0-0|255"},{DownLimit,"0-3|0"},{Length,"8"},{Contains,"00101011"}},5},
    { DROP, false, 272, false, TRANSMIT, {{UpLimit,"0-3|1024"},{DownLimit,"0-2|-1024"},{Length,"8"},{Format,"FFFF0000FF00FF00"}},4},
    { BLOCK, false, 384, false, BIDIRECTIONAL, {{UpLimit,"0-4|512"},{DownLimit,"0-0|0"},{Length,"8"}},3},
    { REWRITE, false, 960, false, BIDIRECTIONAL, {{Message,"Rewrite 3C0"},{UpLimit,"0-3|8388607"},{Length,"8"}},3},
};

int ruleCount = 4;

#endif // CAN_RULES_H
