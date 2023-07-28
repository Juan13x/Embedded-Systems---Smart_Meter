#include "setup/setup.h"

void setupRtosVariables(c_Measurements & measurementsData){
    initSemaphoresAndMutexes();
    
    timerSystemCreate();
    timerSystemInit(measurementsData);
    
    timerDspInit();
    timerInternetInit();
    timerLoraInit();
    timerBluetoothInit();
}

void setupMemoryMangament(){
    //Use next line only for the first time configurating this MCU. A command to do this is mandatory to be added.
    myMemoryManagement.clearAllData();
    myMemoryManagement.setMaxValueForNvsCounters(40);
    myMemoryManagement.resetNvsCounters();
    int nvsInitValue = 0;

    if(myMemoryManagement.init_NVS() != ESP_OK){ /*TODO: ERROR*/ }
    if(!myMemoryManagement.create_NVS_Handle()){ /*TODO: ERROR*/ }
    uint32_t * nvsReturn = new uint32_t;
    if(myMemoryManagement.readPersistentDataAsValue(k_persistentData_NVS_NAME_INPUT_COUNTER, *nvsReturn) == ESP_ERR_NVS_NOT_FOUND)
    {
        myMemoryManagement.clearAllData();
        if(myMemoryManagement.init_NVS() != ESP_OK){ /*TODO: ERROR*/ }
        if(!myMemoryManagement.create_NVS_Handle()){ /*TODO: ERROR*/ }
        myMemoryManagement.writePersistentDataAsValue(k_persistentData_NVS_NAME_INPUT_COUNTER, nvsInitValue);
        myMemoryManagement.writePersistentDataAsValue(k_persistentData_NVS_NAME_OUTPUT_COUNTER, nvsInitValue);
        nvsInitValue = k_main_DEV_ID;
        myMemoryManagement.writePersistentDataAsValue(k_persistentData_NVS_NAME_DEVICE_ID, nvsInitValue);
    }
    else{
        myMemoryManagement.setNvsInputCounter(*nvsReturn);
        myMemoryManagement.readPersistentDataAsValue(k_persistentData_NVS_NAME_OUTPUT_COUNTER, *nvsReturn);
        myMemoryManagement.setNvsOutputCounter(*nvsReturn);
        myMemoryManagement.readPersistentDataAsValue(k_persistentData_NVS_NAME_DEVICE_ID, *nvsReturn);
        ///TODO: set dev_ID in object (maybe config)
    }
    delete nvsReturn;
}

void setupInternetInterface(){
   internetInit();
}

void setupInternetInternalSystem(){
    // Initialize TCP/IP network interface (should be called only once in application)
    ESP_ERROR_CHECK(esp_netif_init());
    // Create default event loop that running in background
    ESP_ERROR_CHECK(esp_event_loop_create_default());
}
void setupBluetooth(){
    myBluetooth.setUpBluetooth();
}
void setupLora(){}
