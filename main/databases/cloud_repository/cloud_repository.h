#ifndef CLOUD_REPOSITORY_H
#define CLOUD_REPOSITORY_H

#include "program_variables.h"
#include "databases/database_variables.h"
#include "databases/firebase/rest_api/rest_api.h"
#include "databases/firebase/firebase_constants.h"

#define DATABASE FIREBASE

enum e_cloudRepositoryProtocols{
    rest
};

#if DATABASE==FIREBASE
namespace n_database = n_firebase;
typedef c_RestApi_Firebase c_Rest_API;
class c_CloudRepository: protected c_Rest_API{
#endif //DATABASE

private:
    char mDatabaseName[n_database::k_database_MAXIMUM_SIZE_OF_FILE_NAME + 1];
    char mPayload[n_database::k_database_ONE_MEASUREMENT_LENGTH * k_main_AMOUNT_MEASUREMENTS_FOR_PAYLOAD];
    unsigned char mNumOfMeasumentsToSend = k_main_AMOUNT_MEASUREMENTS_FOR_PAYLOAD;
    s_DatabasesControlData * mControlData;
public:
    unsigned char getCurrentNumOfMeasumentsToSend(){ return mNumOfMeasumentsToSend; }
    bool setNumOfMeasumentsToSend(unsigned char value);

    const char * getDatabaseName(){ return mDatabaseName; }
    bool setDatabaseName(char * databaseName, char databaseSize);
    unsigned int getDeviceIdRegistered(){ return mControlData->deviceID; }
    void setDeviceId(unsigned char value){ mControlData->deviceID = value; }
    unsigned char getMaxNumOfHarmonicsRegisteredToSend(){ return mControlData->maxNumOfHarmonics; }
    void setMaxNumOfHarmonicsToSend(unsigned char value){ mControlData->maxNumOfHarmonics = value; }

    int appendToPayload(s_SampleData * data,  unsigned char numOfMeasurementsToAddToJson);
    int createPayloadMany(s_SampleData *arrayOfMeasurements, unsigned char numOfMeasumentsToAdd);
    int updateDatabase(){ return c_Rest_API::requestHttpRestPATCH(c_Rest_API::mURL_mask, mPayload); }

    c_CloudRepository(s_DatabasesControlData * controlData){
        c_Rest_API::mControlDataApi = mControlData = controlData;
        mControlData->dataBaseName = mDatabaseName;
        mControlData->deviceID = k_main_DEV_ID;
        mControlData->maxNumOfHarmonics = k_main_MAX_NUM_HARMONICS;
        strcpy(mDatabaseName, n_database::k_database_NAME);
    }
};

extern c_CloudRepository myCloudRepository;

#endif //CLOUD_REPOSITORY_H