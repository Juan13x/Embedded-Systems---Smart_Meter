#ifndef simulationADC_H
#define simulationADC_H

#include <random>
#include "program_variables.h"

#define k_simulation_VOLTAGE_MIN 110.0f
#define k_simulation_VOLTAGE_MAX 240.0f
#define k_simulation_CURRENT_MIN 1.0f
#define k_simulation_CURRENT_MAX 20.0f
#define k_simulation_POWER_MIN 110.0f
#define k_simulation_POWER_MAX 240.0f
#define k_simulation_APPARENT_POWER_MIN 80.0f
#define k_simulation_APPARENT_POWER_MAX 100.0f
#define k_simulation_REACTIVE_POWER_MIN 20.0f
#define k_simulation_REACTIVE_POWER_MAX 50.0f
#define k_simulation_FREQUENCY_MIN 59.0f
#define k_simulation_FREQUENCY_MAX 61.0f
#define k_simulation_HARMONICS_MIN 1.0f
#define k_simulation_HARMONICS_MAX 20.0f

float randomValue(float range_from, float range_to);

float simulationGenerateRandomVoltage();
float simulationGenerateRandomCurrent();
float simulationGenerateRandomPower();
float simulationGenerateRandomApparentPower();
float simulationGenerateRandomReactivePower();
float simulationGenerateRandomFrequency();
float simulationGenerateRandomHarmonics();

#endif //simulationADC_H