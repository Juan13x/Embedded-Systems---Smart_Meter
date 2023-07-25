#include "databases/persistent_data/persistent_data.h"

c_Persistent_Data myMemoryManagement;

void c_Persistent_Data::incrementNvsInputCounter(){
    mNvsInputCounter = (mNvsInputCounter == mMaxValueNvsCounters) 
                    ? mNvsInputCounter : (mNvsInputCounter + 1);
}
void c_Persistent_Data::incrementNvsOutputCounter(){
    mNvsOutputCounter = (mNvsOutputCounter == mMaxValueNvsCounters) 
                    ? mNvsOutputCounter : (mNvsOutputCounter + 1);
}

void c_Persistent_Data::resetNvsCounters(){
    mNvsInputCounter = mNvsOutputCounter = 0;
}

// Initialize NVS
esp_err_t c_Persistent_Data::init_NVS(){
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    return err;
}

/*"Opening Non-Volatile Storage (NVS) handle... ")
The NVS should be already initialized
*/
bool c_Persistent_Data::create_NVS_Handle(){
    // Handle will automatically close when going out of scope or when it's reset.
    esp_err_t result;
    handle = nvs::open_nvs_handle("storage", NVS_READWRITE, &result);
    return (result == ESP_OK);
}

esp_err_t c_Persistent_Data::clearAllData(){
    return nvs_flash_erase();
}