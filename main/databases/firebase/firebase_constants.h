#ifndef FIREBASE_CONSTANTS_H
#define FIREBASE_CONSTANTS_H

namespace n_firebase{
    static const char k_database_NAME[] = "esp32-6f803";
    static const char k_database_TAG[] = "FIREBASE";
    static const unsigned short k_database_TIMEOUT = 1000;
    static const unsigned short k_database_ONE_MEASUREMENT_LENGTH = 750;
    static const char k_database_MAXIMUM_SIZE_OF_FILE_NAME = 50; //it can be maximum 1500 bytes
    static const char k_database_restApi_JSON_ID[] = "_%x%x_%x";
    static const char k_database_restApi_JSON_NO_DEVICE_ID[] = "\"%s\":{\"stringValue\":\"V:%x,C:%x,P:%x,AP:%x,RP:%x,F:%xH:{%s}\"},"; //the first 2 %x are day and time in miliseconds, respectively
    static const char k_database_restApi_URL_MASK_FORMAT[] = "updateMask.fieldPaths=%s&";
    static const char k_database_restApi_FORMAT_UPDATE_MASK[] = "updateMask.fieldPaths=_YYYYDDDD_XXXXXXXX";
    static const char k_database_restApi_URL_FORMAT[] = "https://firestore.googleapis.com/v1/projects/%s/databases/(default)/documents/%x/JSON?%s";
}

#endif //FIREBASE_CONSTANTS_H