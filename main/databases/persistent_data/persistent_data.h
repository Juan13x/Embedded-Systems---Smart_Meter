#ifndef PERSISTENT_DATA_H
#define PERSISTENT_DATA_H

#include "program_variables.h"

#define k_persistentData_NVS_NAME_INPUT_COUNTER "NVS_IN"
#define k_persistentData_NVS_NAME_OUTPUT_COUNTER "NVS_OUT"
#define k_persistentData_NVS_NAME_DEVICE_ID "NVS_ID"

class c_Persistent_Data{
public:
    esp_err_t init_NVS();
    bool create_NVS_Handle();

    template <class T> esp_err_t readPersistentDataAsValue(const char * nvsDataTag, T & nvsData) { return handle->get_item(nvsDataTag, nvsData); } //Templates are better to be defined in the header file
    template <class T> esp_err_t writePersistentDataAsValue(const char * nvsDataTag, T & nvsData) { return handle->set_item(nvsDataTag, nvsData); } //Templates are better to be defined in the header file
    esp_err_t writePersistentDataAsBytes(const char *nvsTag, const void *data, size_t len) { return handle->set_blob(nvsTag, data, len); }
    esp_err_t readPersistentDataAsBytes(const char *nvsTag, void *data, size_t len) { return handle->get_blob(nvsTag, data, len); }
    esp_err_t clearAllData();
    esp_err_t eraseItem(const char *nvsTag){ return handle->erase_item(nvsTag);}
    esp_err_t commitChanges(){ return handle->commit(); }

    unsigned short getValueNvsInputCounter(){ return mNvsInputCounter; }
    //get current output identifier for NVS
    unsigned short getValueNvsOutputCounter(){ return mNvsOutputCounter; }
    void setNvsInputCounter(unsigned short value){ mNvsInputCounter = value; }
    //get current output identifier for NVS
    void setNvsOutputCounter(unsigned short value){ mNvsOutputCounter = value; }
    //increment input identifier for NVS
    void incrementNvsInputCounter();
    //increment output identifier for NVS
    void incrementNvsOutputCounter();
    //reset input identifier for NVS
    void resetNvsInputCounter(){ mNvsInputCounter = 0; }
    //reset output counter for NVS
    void resetNvsOutputCounter(){ mNvsOutputCounter = 0; }
    //reset NVS counters
    void resetNvsCounters();
    bool areNvsCountersEqual(){ return mNvsInputCounter == mNvsOutputCounter; }
    void setMaxValueForNvsCounters(unsigned short value){ mMaxValueNvsCounters = value; }
private:
    unsigned short mMaxValueNvsCounters;
    unsigned short mNvsInputCounter = 0;
    unsigned short mNvsOutputCounter = 0;
    std::shared_ptr<nvs::NVSHandle> handle;
};

extern c_Persistent_Data myMemoryManagement;
#endif //PERSISTENT_DATA_H