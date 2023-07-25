#include "databases/cloud_repository/cloud_repository.h"
#include "cloud_repository.h"

s_DatabasesControlData myDatabaseControlData;
c_CloudRepository myCloudRepository = c_CloudRepository(&myDatabaseControlData);

bool c_CloudRepository::setNumOfMeasumentsToSend(unsigned char value)
{
    if(value > k_main_AMOUNT_MEASUREMENTS_FOR_PAYLOAD) return false;
    mNumOfMeasumentsToSend = value;
    return true;
}

bool c_CloudRepository::setDatabaseName(char *databaseName, char databaseSize)
{
    if(databaseSize > n_database::k_database_MAXIMUM_SIZE_OF_FILE_NAME) return false;
    else strcpy(mDatabaseName, databaseName);
    return true;
}

int c_CloudRepository::appendToPayload(s_SampleData * data,  unsigned char numOfMeasurementsToAdd){
    if(numOfMeasurementsToAdd > mNumOfMeasumentsToSend) numOfMeasurementsToAdd = mNumOfMeasumentsToSend;
    return c_Rest_API::appendToPayload(mPayload, data, numOfMeasurementsToAdd); 
}

int c_CloudRepository::createPayloadMany(s_SampleData *arrayOfMeasurements, unsigned char numOfMeasurementsToAdd) { 
    if(numOfMeasurementsToAdd > mNumOfMeasumentsToSend) numOfMeasurementsToAdd = mNumOfMeasumentsToSend;
    return c_Rest_API::createPayloadMany(mPayload, numOfMeasurementsToAdd, arrayOfMeasurements);
}