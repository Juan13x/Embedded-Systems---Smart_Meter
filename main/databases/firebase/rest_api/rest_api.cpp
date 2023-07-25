#include "databases/firebase/rest_api/rest_api.h"

extern const uint8_t certificate_pem_start[] asm("_binary_certificate_pem_start");
extern const uint8_t certificate_pem_end[]   asm("_binary_certificate_pem_end");
char local_response_buffer[k_internet_http_MAX_OUTPUT_BUFFER] = {0};

int c_RestApi_Firebase::requestHttpRestPATCH(char *urlMask, char *payload)
{
    // PATCH
    /**
     * NOTE: All the configuration parameters for http_client must be spefied either in URL or as host and path parameters.
     * If host and path parameters are not set, query parameter will be ignored. In such cases,
     * query parameter should be specified in URL.
     *
     * If URL as well as host and path parameters are specified, values of host and path will be considered.
     */
    esp_http_client_config_t config= esp_http_client_config_t();
    config.url = urlMask;
    config.method=HTTP_METHOD_PATCH;
    config.cert_pem = (const char *)certificate_pem_start;
    config.event_handler = _http_event_handler;
    config.timeout_ms = n_firebase::k_database_TIMEOUT;
    config.user_data = local_response_buffer;        // Pass address of local buffer to get response
    config.disable_auto_redirect = true;
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_post_field(client, payload, strlen(payload));
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(k_internet_http_TAG, "HTTP PATCH Status = %d, content_length = %lld",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(k_internet_http_TAG, "HTTP PATCH request failed: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);

    return 1;
}

void c_RestApi_Firebase::createOneJsonMeasurement(char *payload, s_SampleData * data)
{
    mHarmonicSTR[0] = 0;
    mMeasurementOrAuxOrURL[0] = 0;
    mDocumentName[0] = 0;
    for(unsigned char currentHarmonics = 0; currentHarmonics <= mControlDataApi->maxNumOfHarmonics; currentHarmonics++){
        sprintf(mAuxHarmonicsSTR, "%i:%x,", currentHarmonics, data->harmonics[currentHarmonics]._i);
        strcat(mHarmonicSTR, mAuxHarmonicsSTR);
    }
    sprintf(mDocumentName, n_firebase::k_database_restApi_JSON_ID, data->year, data->day, data->lastTime);
    sprintf(mMeasurementOrAuxOrURL, n_firebase::k_database_restApi_JSON_NO_DEVICE_ID,
        mDocumentName,
        data->voltage._i,
        data->current._i,
        data->power._i,
        data->apparentPower._i,
        data->reactivePower._i,
        data->frequency._i,
        mHarmonicSTR);
    strcat(payload, mMeasurementOrAuxOrURL);
    mMeasurementOrAuxOrURL[0] = 0;
    sprintf(mMeasurementOrAuxOrURL, n_firebase::k_database_restApi_URL_MASK_FORMAT, mDocumentName);
    strcat(mURL_mask, mMeasurementOrAuxOrURL);
}

int c_RestApi_Firebase::appendToPayload(char *payload, s_SampleData * data, unsigned char numOfMeasurementsToAddToJson)
{   
    if(mCounter == 0){
        payload[0] = 0;
        mURL_mask[0] = 0;
        
        //"{\"fields\":{%s}}"
        strcat(payload, "{\"fields\":{");
    }
    createOneJsonMeasurement(payload, data);
    mCounter++;
    
    if(mCounter == numOfMeasurementsToAddToJson){
        mCounter = 0;
        strcat(payload, "}}");
        sprintf(mMeasurementOrAuxOrURL, n_firebase::k_database_restApi_URL_FORMAT, mControlDataApi->dataBaseName, mControlDataApi->deviceID, mURL_mask);
        return 2;
    }
    return 1;  
}

int c_RestApi_Firebase::createPayloadMany(char * payload, unsigned char numOfMeasurementsToAddToJson, s_SampleData * arrayOfMeasurements)
{
    payload[0] = 0;
    mURL_mask[0] = 0;
    mCounter = 0;
    
    //"{\"fields\":{%s}}"
    strcat(payload, "{\"fields\":{");
    while(mCounter < numOfMeasurementsToAddToJson){
        s_SampleData * data = arrayOfMeasurements+ mCounter;
        createOneJsonMeasurement(payload, data);
        mCounter++;
    }
    strcat(payload, "}}");
    sprintf(mMeasurementOrAuxOrURL, n_firebase::k_database_restApi_URL_FORMAT, mControlDataApi->dataBaseName, mControlDataApi->deviceID, mURL_mask);
    return 1;
}