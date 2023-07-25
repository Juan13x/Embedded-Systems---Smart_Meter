#include "classes/simulation/simulation_adc.h"

float randomValue(float range_from, float range_to) {
    uint64_t count;
    gptimer_get_raw_count(g_timer_system, &count);
    std::mt19937                        generator( (uint32_t) count);
    std::uniform_real_distribution<float>    distr(range_from, range_to);
    return distr(generator);
}

float simulationGenerateRandomVoltage() {
    return randomValue(k_simulation_VOLTAGE_MIN, k_simulation_VOLTAGE_MAX);
}
float simulationGenerateRandomCurrent(){
    return randomValue(k_simulation_CURRENT_MIN, k_simulation_CURRENT_MAX);
}
float simulationGenerateRandomPower(){
    return randomValue(k_simulation_POWER_MIN, k_simulation_POWER_MAX);
}
float simulationGenerateRandomApparentPower(){
    return randomValue(k_simulation_APPARENT_POWER_MIN, k_simulation_APPARENT_POWER_MAX);
}
float simulationGenerateRandomReactivePower(){
    return randomValue(k_simulation_REACTIVE_POWER_MIN, k_simulation_REACTIVE_POWER_MAX);
}
float simulationGenerateRandomFrequency(){
    return randomValue(k_simulation_FREQUENCY_MIN, k_simulation_FREQUENCY_MAX);
}
float simulationGenerateRandomHarmonics(){
    return randomValue(k_simulation_HARMONICS_MIN, k_simulation_HARMONICS_MAX);
}

