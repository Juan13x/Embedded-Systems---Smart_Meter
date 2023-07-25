#ifndef DATABASE_VARIABLES_H
#define DATABASE_VARIABLES_H

#include "program_variables.h"

struct s_DatabasesControlData{
    const char * dataBaseName;
    uint32_t deviceID;
    unsigned char maxNumOfHarmonics;
};

#endif //DATABASE_VARIABLES_H