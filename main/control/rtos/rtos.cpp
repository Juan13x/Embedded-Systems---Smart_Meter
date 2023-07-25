#include "control/rtos/rtos.h"

#define DELAY_1_MS pdMS_TO_TICKS(1)

gptimer_handle_t g_timer_system = NULL;
TimerHandle_t g_timer_dsp = NULL;
TimerHandle_t g_timer_internet = NULL;
TimerHandle_t g_timer_lora = NULL;
TimerHandle_t g_timer_bluetooth = NULL;
SemaphoreHandle_t g_semaphore_UpdateLIFO = NULL;
SemaphoreHandle_t g_semaphore_taskDSP = NULL;
SemaphoreHandle_t g_mutex_internet = NULL;
SemaphoreHandle_t g_mutex_mutexLora = NULL;
SemaphoreHandle_t g_mutex_mutexBluetooth = NULL;
c_AppStates states = c_AppStates();

esp_err_t timerSystemCreate(){
    gptimer_handle_t & timerSystem = g_timer_system;

    gptimer_config_t timerDspConfiguration;
    timerDspConfiguration.clk_src = GPTIMER_CLK_SRC_DEFAULT;
    timerDspConfiguration.direction = GPTIMER_COUNT_UP;
    timerDspConfiguration.resolution_hz = 2000; // 1 tick=500us

    esp_err_t err;
    err =  gptimer_new_timer(&timerDspConfiguration, &timerSystem);
    if(err != ESP_OK){ return err; }

    return ESP_OK;
}

esp_err_t timerSystemInit(c_Measurements & measurementsData){
    gptimer_handle_t & timerSystem = g_timer_system;
    gptimer_alarm_config_t alarm_config;
    alarm_config.alarm_count = 172800000; // period = 1 day
    alarm_config.reload_count = 0;
    alarm_config.flags.auto_reload_on_alarm = true;

    gptimer_event_callbacks_t cbs = {
        .on_alarm = timerSystemOnAlarm,
    };
    esp_err_t err;
    err = gptimer_register_event_callbacks(timerSystem, &cbs, &measurementsData);
    if(err != ESP_OK) { return err; }
    err = gptimer_enable(timerSystem);
    if(err != ESP_OK) { return err; }
    err = gptimer_set_alarm_action(timerSystem, &alarm_config);
    if(err != ESP_OK) { return err; }
    err = gptimer_start(timerSystem);
    if(err != ESP_OK) { return err; }
    return ESP_OK;
}

bool timerSystemOnAlarm(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data){
    c_Measurements * measurementsData = (c_Measurements *) user_data;
    unsigned short newDay = measurementsData->getDay()+1;
    measurementsData->setDay(newDay);
    
    return false;
}

int timerDspInit(){
    TimerHandle_t & timerDsp = g_timer_dsp;
    xTimerCreate("timerDSP", k_main_TASK_DSP_DEFAULT_PERIOD , true, (void *) k_main_TIMER_TASK_DSP_ID, timerCallbackDspTaskReady);

    if(timerDsp == NULL){ return 1; }
    else{
        if(xTimerStart(timerDsp, 0) == pdFAIL)
        { return 2; }
    }
    return ESP_OK;
}

void timerCallbackDspTaskReady( TimerHandle_t pxTimer ){    
    BaseType_t xHigherPriorityTaskWoken;
    xSemaphoreGiveFromISR(g_semaphore_taskDSP, &xHigherPriorityTaskWoken);

    /// TODO: switch context depending on the value of xHigherPriorityTaskWoken. Right now all tasks have the same priority, so it's ok to work without checking its value.
}

int timerInternetInit(){
    TimerHandle_t & timerInternet = g_timer_internet;
    xTimerCreate("timerInternet", k_main_TASK_INTERNET_DEFAULT_PERIOD , true, (void *) k_main_TIMER_TASK_INTERNET_ID, timerCallbackInternetTaskReady);

    if(timerInternet == NULL){ return 1; }
    else{
        if(xTimerStart(timerInternet, 0) == pdFAIL)
        { return 2; }
    }
    return ESP_OK;
}

void timerCallbackInternetTaskReady( TimerHandle_t pxTimer ){    
    BaseType_t xHigherPriorityTaskWoken;
    states.setStateTimerInternetExpired_ISR(xHigherPriorityTaskWoken);

    /// TODO: switch context depending on the value of xHigherPriorityTaskWoken. Right now all tasks have the same priority, so it's ok to work without checking its value.
}

int timerLoraInit(){
    TimerHandle_t & timerLora = g_timer_lora;
    xTimerCreate("timerLora", k_main_TASK_LORA_DEFAULT_PERIOD , true, (void *) k_main_TIMER_TASK_LORA_ID, timerCallbackLoraTaskReady);

    if(timerLora == NULL){ return 1; }
    else{
        if(xTimerStart(timerLora, 0) == pdFAIL)
        { return 2; }
    }
    return ESP_OK;
}

void timerCallbackLoraTaskReady( TimerHandle_t pxTimer ){    
    BaseType_t xHigherPriorityTaskWoken;
    states.setStateTimerLoraExpired_ISR(xHigherPriorityTaskWoken);

    /// TODO: switch context depending on the value of xHigherPriorityTaskWoken. Right now all tasks have the same priority, so it's ok to work without checking its value.
}

int timerBluetoothInit(){
    TimerHandle_t & timerBluetooth = g_timer_bluetooth;
    xTimerCreate("timerBluetooth", k_main_TASK_BLUETOOTH_DEFAULT_PERIOD , true, (void *) k_main_TIMER_TASK_BLUETOOTH_ID, timerCallbackBluetoothTaskReady);

    if(timerBluetooth == NULL){ return 1; }
    else{
        if(xTimerStart(timerBluetooth, 0) == pdFAIL)
        { return 2; }
    }
    return ESP_OK;
}

void timerCallbackBluetoothTaskReady( TimerHandle_t pxTimer ){    
    BaseType_t xHigherPriorityTaskWoken;
    states.setStateTimerBluetoothExpired_ISR(xHigherPriorityTaskWoken);

    /// TODO: switch context depending on the value of xHigherPriorityTaskWoken. Right now all tasks have the same priority, so it's ok to work without checking its value.
}

char initSemaphoresAndMutexes(){
    g_mutex_internet = xSemaphoreCreateMutex();
    g_mutex_mutexBluetooth = xSemaphoreCreateMutex();
    g_mutex_mutexLora = xSemaphoreCreateMutex();
    g_semaphore_UpdateLIFO = xSemaphoreCreateBinary();
    g_semaphore_taskDSP = xSemaphoreCreateBinary();

    if((g_mutex_internet == NULL) or
    (g_mutex_mutexBluetooth == NULL) or 
    (g_mutex_mutexLora==NULL) or
    (g_semaphore_UpdateLIFO == NULL) or
    (g_semaphore_taskDSP == NULL)) { 
        /** TODO: ERROR */
        return 0;
    }
    return 1;
}


void taskDSP(void* arg){
    c_Measurements & sample = *(c_Measurements *) arg;
    //char message[750];
    char format[10];
    //char aux[14];
    //char harmonicSTR[(m_MAX_NUM_HARMONICS + 1)*13+1] = {0};
    uint64_t count = 0;
    
    while (1)
    {
        if(xSemaphoreTake(g_semaphore_taskDSP, portMAX_DELAY) == pdTRUE){
            ESP_LOGI("taskDSP", "working");
            gptimer_get_raw_count(g_timer_system, &count);
            sample.setLastTime((uint32_t) count);
            count = 0;
#if defined SIMULATION
            if(sample.isVoltageReadingEnable())
                sample.setVoltage(simulationGenerateRandomVoltage());
            if(sample.isCurrentReadingEnable())
                sample.setCurrent(simulationGenerateRandomCurrent());
            if(sample.isPowerReadingEnable())
                sample.setPower(simulationGenerateRandomPower());
            if(sample.isApparentPowerReadingEnable())
                sample.setApparrentPower(simulationGenerateRandomApparentPower());
            if(sample.isReactivePowerReadingEnable())
                sample.setReactivePower(simulationGenerateRandomReactivePower());
            if(sample.isFrequencyReadingEnable())
                sample.setFrequency(simulationGenerateRandomFrequency());
            if(sample.isHormonicsReadingEnable()){
                unsigned char limit = sample.getHarmonicsLimit();
                //harmonicSTR[0] = 0;
                for(unsigned char i = 0; i <= limit; i++){
                    sample.setHarmonic(i, simulationGenerateRandomHarmonics());
                    /*sprintf(aux, "%i:%x,", i, sample.getHarmonicAsInt(i));
                    strcat(harmonicSTR, aux);*/
                }
            }
#else //not simulated
#endif
            /*sprintf(message, JSON_NoDevID,
            sample.getYear(),
            sample.getDay(),
            sample.getLastTime(),
            sample.getVoltageAsInt(),
            sample.getCurrentAsInt(),
            sample.getPowerAsInt(),
            sample.getApparrentPowerAsInt(),
            sample.getReactivePowerAsInt(),
            sample.getFrequencyAsInt(),
            harmonicSTR);*/

            sprintf(format, "%x", myMemoryManagement.getValueNvsInputCounter());
            myMemoryManagement.eraseItem(format);
            esp_err_t err= myMemoryManagement.writePersistentDataAsBytes(
                    format, 
                    sample.getMeasurementData(),
                    sizeof(s_SampleData));
            if(err != ESP_OK)
            { 
                //ESP_ERR_NVS_NOT_ENOUGH_SPACE
                if(err == ESP_ERR_NVS_BASE + 0x05){
                    myMemoryManagement.setMaxValueForNvsCounters(myMemoryManagement.getValueNvsInputCounter());
                    myMemoryManagement.resetNvsInputCounter();
                    strcpy(format, "0");
                    myMemoryManagement.eraseItem(format);
                    esp_err_t err= myMemoryManagement.writePersistentDataAsBytes(
                            format, 
                            sample.getMeasurementData(),
                    sizeof(s_SampleData));
                    if(err != ESP_OK) { ESP_LOGE("SIMULATION:", "did not set sample after retry"); }
                }
                else{ ESP_LOGE("SIMULATION:", "Unknown error %i", err); }
            }
            else{
                if(myMemoryManagement.commitChanges() != ESP_OK) { ESP_LOGE("SIMULATION:", "did not commit"); }
                else{
                    myMemoryManagement.incrementNvsInputCounter();
                    
                    xSemaphoreGive(g_semaphore_UpdateLIFO
                    );
                    //ESP_LOGI("Sample", "%sNVS:%u", message,sample.getValueNvsInputCounter());
                }
            }
        }
        vTaskDelay(1);
    } 
}

/// @brief 
/// @param arg = struct s_SampleArrays
void taskUpdateData(void *arg)
{
    s_SampleArrays * sampleArray = (s_SampleArrays * ) arg;
    s_SampleData & sampleReturn = *(sampleArray->sampleReturn);
    s_SampleDataAndControl & originalData = sampleArray->original;
    s_SampleDataAndControl & internetData = sampleArray->internet;
    s_SampleDataAndControl & loraData = sampleArray->lora;
    s_SampleDataAndControl & bluetoothData = sampleArray->bluetooth;

    auto copySamples = [](s_SampleDataAndControl & dataDst, s_SampleDataAndControl & dataSrc){
        unsigned char currentSampleCounter = dataSrc.counter--;
        unsigned char numCopies = dataDst.counter--;
        unsigned char indexDst = numCopies;
        while(numCopies + 1){
            if(currentSampleCounter >= numCopies){
                dataDst.measurements[numCopies] = dataSrc.measurements[currentSampleCounter - numCopies];
            }
            else{
                dataDst.measurements[numCopies] = dataSrc.measurements[indexDst];
                indexDst++;
            }
            numCopies--;
        }
    };
    
    auto updateSamplesCounters = [&](){
        /// TODO: check num samples bluetooth->counters.bluetooth++;
        /// TODO: check num samples lora->counters.lora++;
        if(myCloudRepository.getCurrentNumOfMeasumentsToSend() > internetData.counter) {internetData.counter++;}
        
        originalData.counter++;
        if(originalData.counter == k_main_AMOUNT_MEASUREMENTS_FOR_PAYLOAD) {originalData.counter = 0;}
    };
    
    while(1){
        if(xSemaphoreTake(g_semaphore_UpdateLIFO, 1)){
            originalData.measurements[originalData.counter] = sampleReturn;
            updateSamplesCounters();
        }

        if(states.isTimerInternetOver() and (internetData.counter > 0)){
            if(states.isInternetEnable() and not(states.isInternetBeingConfigured())){
                xSemaphoreTake(g_mutex_internet, 1);
                copySamples(internetData, originalData);
                states.setStateInternetTransmitting();
                }                
            internetData.counter = 0;
        }

        if(states.isTimerLoraOver() and (loraData.counter > 0)){
            if(states.isLoraEnable() and not(states.isLoraBeingConfigured())){
                xSemaphoreTake(g_mutex_mutexLora, 1);
                copySamples(loraData, originalData);
                states.setStateLoraTransmitting();                
            }
            loraData.counter = 0;
        }

        if(states.isTimerBluetoothOver() and (bluetoothData.counter > 0)){
            if(states.isBluetoothEnable() and not(states.isBluetoothBeingConfigured())){
                xSemaphoreTake(g_mutex_mutexBluetooth, 1);
                copySamples(bluetoothData, originalData);
                states.setStateBluetoothTransmitting();         
            }
            bluetoothData.counter = 0;
        }
    }
}

void taskSendOverInternet(void* arg){
    s_InterfaceData * internetData = (s_InterfaceData * ) arg;
    
    while(1){
        if(states.isStateInternetTransmitting()){
            if(internetData->control == e_cloudRepositoryProtocols::rest){
                s_SampleDataAndControl * internetMeasurements = (s_SampleDataAndControl *) internetData->data;

                myCloudRepository.createPayloadMany(internetMeasurements->measurements, internetMeasurements->counter);
                myCloudRepository.updateDatabase();
                xSemaphoreGive(g_mutex_internet);
            }
        }
    }
}

void taskSendOverBluetooth(void* arg){
    /// TODO:
    while(1){
        if(states.isStateBluetoothTransmitting()){
        /// TODO:
            xSemaphoreGive(g_mutex_mutexBluetooth);
        }
    }
}
void taskSendOverLorawan(void* arg){
    /// TODO:
    while(1){
        if(states.isStateInternetTransmitting()){
            /// TODO:
            xSemaphoreGive(g_mutex_mutexLora);
        }
    }
}
void taskConfiguration(void * args){
    /// TODO:
    while(1){
        vTaskDelay(1);
    }
}