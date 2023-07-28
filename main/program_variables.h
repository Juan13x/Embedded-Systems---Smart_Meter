#ifndef PROGRAM_VARIABLES_H
#define PROGRAM_VARIABLES_H

#include <stdio.h>
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "freertos/timers.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "driver/gptimer.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "nvs_handle.hpp"
#include "driver/spi_master.h"

#define k_main_DEV_ID (123456U)

#define k_main_TIMER_TASK_DSP_ID (0U)
#define k_main_TIMER_TASK_INTERNET_ID (1U)
#define k_main_TIMER_TASK_LORA_ID (2U)
#define k_main_TIMER_TASK_BLUETOOTH_ID (3U)

#define k_main_MAX_NUM_HARMONICS (50U)
#define k_main_AMOUNT_MEASUREMENTS_FOR_PAYLOAD (8U)
#define k_main_TASK_DSP_PRIORITY (10U)
#define k_main_TASK_UPDATE_DATA_PRIORITY (10U)
#define k_main_TASK_INTERNET_PRIORITY (10U)
#define k_main_TASK_LORA_PRIORITY (10U)
#define k_main_TASK_BLUETOOTH_PRIORITY (10U)
#define k_main_TASK_CONFIGURATION_PRIORITY (10U)
#define k_main_MIN_PERIOD_TO_SAMPLE (500U)
#define k_main_TASK_DSP_DEFAULT_PERIOD pdMS_TO_TICKS(1000)
#define k_main_TASK_INTERNET_DEFAULT_PERIOD pdMS_TO_TICKS(10000)
#define k_main_TASK_LORA_DEFAULT_PERIOD pdMS_TO_TICKS(10000)
#define k_main_TASK_BLUETOOTH_DEFAULT_PERIOD pdMS_TO_TICKS(10000)
#define k_main_LENGTH_FORMAT_HARMONICS sizeof("NNN:XXXXXXXX,\n")

extern gptimer_handle_t g_timer_system;
extern TimerHandle_t g_timer_dsp;
extern TimerHandle_t g_timer_internet;
extern TimerHandle_t g_timer_lora;
extern TimerHandle_t g_timer_bluetooth;
    
extern SemaphoreHandle_t g_semaphore_taskDSP;
extern SemaphoreHandle_t g_mutex_internet;
extern SemaphoreHandle_t g_mutex_mutexLora;
extern SemaphoreHandle_t g_mutex_mutexBluetooth; 
extern SemaphoreHandle_t g_semaphore_UpdateLIFO;

class c_AppStates{
private:    
    static const unsigned int mIS_INTERNET_ENABLE_BIT = 1 << 0;
    static const unsigned int mIS_LORA_ENABLE_BIT = 1 << 1;
    static const unsigned int mIS_BLUETOOTH_ENABLE_BIT = 1 << 2;

    static const unsigned int mIS_TIMER_OVER_LORA_BIT = 1 << 3;
    static const unsigned int mIS_TIMER_OVER_INTERNET_BIT = 1 << 4;
    static const unsigned int mIS_TIMER_OVER_BLUETOOTH_BIT = 1 << 5;

    static const unsigned int mCONFIG_INTERNET_BIT = 1 << 6;
    static const unsigned int mCONFIG_LORA_BIT = 1 << 7;
    static const unsigned int mCONFIG_BLUETOOTH_BIT = 1 << 8;

    static const unsigned int mSEND_INTERNET = 1 << 9;
    static const unsigned int mSEND_LORA = 1 << 10;
    static const unsigned int mSEND_BLUETOOTH = 1 << 11;

    //configUSE_16_BIT_TICKS is 0 by default. if not, set to 0 to use 24 bits for events
    EventGroupHandle_t mEventGroup;
public: 
    c_AppStates(){ mEventGroup = xEventGroupCreate(); }
    
    //INTERFACES ACTIVATION
    bool isInternetEnable(){ return 
        (xEventGroupGetBits(mEventGroup) & mIS_INTERNET_ENABLE_BIT); }
    bool isLoraEnable(){ return 
        (xEventGroupGetBits(mEventGroup) & mIS_LORA_ENABLE_BIT); }
    bool isBluetoothEnable(){ return 
        (xEventGroupGetBits(mEventGroup) & mIS_BLUETOOTH_ENABLE_BIT); }

    void enableInternet(){ 
        xEventGroupSetBits(mEventGroup, mIS_INTERNET_ENABLE_BIT); }
    void enableLora(){ 
        xEventGroupSetBits(mEventGroup, mIS_LORA_ENABLE_BIT); }
    void enableBluetooth(){ 
        xEventGroupSetBits(mEventGroup, mIS_BLUETOOTH_ENABLE_BIT); }

    void disableInternet(){ 
        xEventGroupClearBits(mEventGroup, mIS_INTERNET_ENABLE_BIT); }
    void disableLora(){ 
        xEventGroupClearBits(mEventGroup, mIS_LORA_ENABLE_BIT); }
    void disableBluetooth(){ 
        xEventGroupClearBits(mEventGroup, mIS_BLUETOOTH_ENABLE_BIT); }

    //Transmission state
    void setStateInternetTransmitting(){ xEventGroupSetBits(mEventGroup, mSEND_INTERNET); }
    void setStateLoraTransmitting(){ xEventGroupSetBits(mEventGroup, mSEND_LORA); }
    void setStateBluetoothTransmitting(){ xEventGroupSetBits(mEventGroup, mSEND_BLUETOOTH); }

    bool isStateInternetTransmitting(){ return (xEventGroupWaitBits(mEventGroup, mSEND_INTERNET, pdTRUE, pdTRUE, 1) & mSEND_INTERNET); }
    bool isStateLoraTransmitting(){ return (xEventGroupWaitBits(mEventGroup, mSEND_LORA, pdTRUE, pdTRUE, 1) & mSEND_LORA); }
    bool isStateBluetoothTransmitting(){ return (xEventGroupWaitBits(mEventGroup, mSEND_BLUETOOTH, pdTRUE, pdTRUE, 1) & mSEND_BLUETOOTH); }

    //TIMERS
    BaseType_t setStateTimerInternetExpired_ISR(BaseType_t & pxHigherPriorityTaskWoken){ return 
        xEventGroupSetBitsFromISR(mEventGroup, mIS_TIMER_OVER_INTERNET_BIT, &pxHigherPriorityTaskWoken); }
    BaseType_t setStateTimerLoraExpired_ISR(BaseType_t & pxHigherPriorityTaskWoken){ return 
        xEventGroupSetBitsFromISR(mEventGroup, mIS_TIMER_OVER_LORA_BIT, &pxHigherPriorityTaskWoken); }
    BaseType_t setStateTimerBluetoothExpired_ISR(BaseType_t &pxHigherPriorityTaskWoken){ return 
        xEventGroupSetBitsFromISR(mEventGroup, mIS_TIMER_OVER_BLUETOOTH_BIT, &pxHigherPriorityTaskWoken); }

    bool isTimerLoraOver(){ return 
        (xEventGroupWaitBits(mEventGroup, mIS_TIMER_OVER_LORA_BIT, pdTRUE, pdTRUE, 1) & mIS_TIMER_OVER_LORA_BIT); }
    bool isTimerInternetOver(){ return 
        (xEventGroupWaitBits(mEventGroup, mIS_TIMER_OVER_INTERNET_BIT, pdTRUE, pdTRUE, 1) & mIS_TIMER_OVER_INTERNET_BIT); }
    bool isTimerBluetoothOver(){ return 
        (xEventGroupWaitBits(mEventGroup, mIS_TIMER_OVER_BLUETOOTH_BIT, pdTRUE, pdTRUE, 1) & mIS_TIMER_OVER_BLUETOOTH_BIT); }

    //CONFIGURATION
    bool isLoraBeingConfigured(){ return 
        (xEventGroupGetBits(mEventGroup) & mCONFIG_LORA_BIT);}
    bool isInternetBeingConfigured(){ return
        (xEventGroupGetBits(mEventGroup) & mCONFIG_INTERNET_BIT);}
    bool isBluetoothBeingConfigured(){ return 
        (xEventGroupGetBits(mEventGroup) & mCONFIG_BLUETOOTH_BIT);}

    void setStateLoraConfiguration(){ 
        xEventGroupSetBits(mEventGroup, mCONFIG_LORA_BIT); }
    void setStateInternetConfiguration(){ 
        xEventGroupSetBits(mEventGroup, mCONFIG_INTERNET_BIT); }
    void setStateBluetoothConfiguration(){ 
        xEventGroupSetBits(mEventGroup, mCONFIG_BLUETOOTH_BIT); }

    void clearStateLoraConfiguration(){ 
        xEventGroupClearBits(mEventGroup, mCONFIG_LORA_BIT); }
    void clearStateInternetConfiguration(){ 
        xEventGroupClearBits(mEventGroup, mCONFIG_INTERNET_BIT); }
    void clearStateBluetoothConfiguration(){ 
        xEventGroupClearBits(mEventGroup, mCONFIG_BLUETOOTH_BIT); }
};

extern c_AppStates states;

#endif //PROGRAM_VARIABLES_H