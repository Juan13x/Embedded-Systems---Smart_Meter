#ifndef SETUP_H
#define SETUP_H

#include "control/rtos/rtos.h"

void setupRtosVariables(c_Measurements & measurementsData);
void setupMemoryMangament();
void setupInternetInterface();
void setupInternetInternalSystem();
void setupBluetooth();
void setupLora();

#endif //SETUP_H