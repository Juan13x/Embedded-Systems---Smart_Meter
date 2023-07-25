#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "program_variables.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"

class c_SppBluetooth_static{
private:
    static const unsigned char mMAX_SIZE_RX_BUFFER = 30;
    static char mDataReceived[mMAX_SIZE_RX_BUFFER];
    
    //FROM IDF examples
    static char *bda2str(uint8_t * bda, char *str, size_t size);
    static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_AUTHENTICATE;
    static const esp_spp_role_t role_slave = ESP_SPP_ROLE_SLAVE;
    static const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_CB;
    static const bool esp_spp_enable_l2cap_ertm = true;
    static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
    static void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);
public:
    static const char * getDataReceived(){ return mDataReceived; }
    bool setUpBluetooth();
};

extern c_SppBluetooth_static myBluetooth;

#endif //BLUETOOTH_H