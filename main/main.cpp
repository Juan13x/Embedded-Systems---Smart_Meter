#include "setup/setup.h"

//HERE ALL GLOBAL VARIABLES

extern "C" void app_main(void)
{
    c_Measurements sample;
    sample.enableAllReading();

    s_SampleArrays measurementsAndControl;
    measurementsAndControl.sampleReturn = sample.getMeasurementData();
    s_InterfaceData internetData;
    s_InterfaceData loraData;
    s_InterfaceData bluetoothData;
    
    internetData.data = (void *) &measurementsAndControl.internet;
    //The next line goes into a configuration function
    internetData.control = e_cloudRepositoryProtocols::rest;
    ///TODO: configure initial state of LORA
    loraData.data = (void *) &measurementsAndControl.lora;
    ///TODO: configure initial state of Bluetooth
    bluetoothData.data = (void *) &measurementsAndControl.bluetooth;

    setupMemoryMangament();
    setupRtosVariables(sample);
    setupInternetInternalSystem();
    setupInternetInterface();
    setupLora();
    setupBluetooth();

    TaskHandle_t generalHandler;

    if(xTaskCreate(
        taskDSP, 
        "taskDSP", 
        16384, &sample, 
        k_main_TASK_DSP_PRIORITY, 
        &generalHandler) == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
        { /*TODO = error*/ }
    if(xTaskCreate(
        taskUpdateData, 
        "taskUpdateData", 
        16384, &measurementsAndControl, 
        k_main_TASK_UPDATE_DATA_PRIORITY, 
        &generalHandler) == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
        { /*TODO = error*/ }
    if(xTaskCreate(
        taskSendOverInternet,
        "taskSendOverInternet", 
        16384, &internetData, 
        k_main_TASK_INTERNET_PRIORITY, 
        &generalHandler) == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
        { /*TODO = error*/ }
    if(xTaskCreate(
        taskSendOverLorawan, 
        "taskSendOverLorawan", 
        16384, &loraData, 
        k_main_TASK_LORA_PRIORITY, 
        &generalHandler) == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
        { /*TODO = error*/ }
    if(xTaskCreate(
        taskSendOverBluetooth, 
        "taskSendOverBluetooth", 
        16384, &bluetoothData, 
        k_main_TASK_BLUETOOTH_PRIORITY, 
        &generalHandler) == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
        { /*TODO = error*/ }
    ///TODO: update parameter "void *pvParameters"
    if(xTaskCreate(
        taskConfiguration,
        "taskConfiguration",
        8192,
        NULL,
        k_main_TASK_CONFIGURATION_PRIORITY,
        &generalHandler) == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
        { /*TODO = error*/ }

    ESP_LOGI("ESP", "starting app");
    while(1){
        vTaskDelay(1);
    }
}