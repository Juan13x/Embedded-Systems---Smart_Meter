#include "program_variables.h"
#include "classes/simulation/simulation_adc.h"
#include "lorawan/lorawan.h"
#include "bluetooth/bluetooth.h"
#include "classes/measurements/measurements.h"
#include "databases/cloud_repository/cloud_repository.h"
#include "databases/persistent_data/persistent_data.h"
#include "internet/internet/internet.h"

#define SIMULATION

void taskDSP(void* arg);
void taskUpdateData(void* arg);
void taskSendOverInternet(void* arg);
void taskSendOverBluetooth(void* arg);
void taskSendOverLorawan(void* arg);
void taskConfiguration(void * args);

esp_err_t timerSystemCreate();
esp_err_t timerSystemInit(c_Measurements & measurementsData);
bool IRAM_ATTR timerSystemOnAlarm(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data);

int timerDspInit();
int timerInternetInit();
int timerLoraInit();
int timerBluetoothInit();
void timerCallbackDspTaskReady( TimerHandle_t pxTimer );
void timerCallbackInternetTaskReady( TimerHandle_t pxTimer );
void timerCallbackLoraTaskReady( TimerHandle_t pxTimer );
void timerCallbackBluetoothTaskReady( TimerHandle_t pxTimer );

char initSemaphoresAndMutexes();

struct s_InterfaceData{
    void * data;
    unsigned char control = 0;
};