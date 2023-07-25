#ifndef REST_H
#define REST_H

#include "program_variables.h"
#include "databases/database_variables.h"
#include "internet/protocol/http/http.h"
#include "classes/measurements/measurements.h"
#include "databases/firebase/firebase_constants.h"

void httpRestWithUrl(void);

class c_RestApi_Firebase{
protected:
    char mURL_mask[sizeof(n_firebase::k_database_restApi_FORMAT_UPDATE_MASK) * k_main_AMOUNT_MEASUREMENTS_FOR_PAYLOAD + 1];
    char mDocumentName[n_firebase::k_database_MAXIMUM_SIZE_OF_FILE_NAME];
    char mMeasurementOrAuxOrURL[n_firebase::k_database_ONE_MEASUREMENT_LENGTH];
    char mHarmonicSTR[(k_main_MAX_NUM_HARMONICS + 1)* (k_main_LENGTH_FORMAT_HARMONICS-1)+1];
    char mAuxHarmonicsSTR[k_main_LENGTH_FORMAT_HARMONICS];
    s_DatabasesControlData * mControlDataApi;
    unsigned char mCounter;
    void createOneJsonMeasurement(char *payload, s_SampleData * data);
protected:
    int requestHttpRestPATCH(char * urlMask, char * payload);
    int createPayloadMany(char * payload, unsigned char numOfMeasurementsToAddToJson, s_SampleData * arrayOfMeasurements);
    int appendToPayload(char *payload, s_SampleData * data, unsigned char numOfMeasurementsToAddToJson);
    c_RestApi_Firebase(){ mCounter = 0;}
};

#endif //REST_H