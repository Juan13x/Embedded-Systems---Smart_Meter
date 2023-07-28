#ifndef HTTP_H
#define HTTP_H

#include "program_variables.h"
#include "esp_tls.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_http_client.h"


static const unsigned int k_internet_http_MAX_RECV_BUFFER = 512;
static const unsigned int k_internet_http_MAX_OUTPUT_BUFFER = 1000 * k_main_AMOUNT_MEASUREMENTS_FOR_PAYLOAD;
static const char k_internet_http_TAG[] = "HTTP_CLIENT";

esp_err_t _http_event_handler(esp_http_client_event_t *evt);

#endif //HTTP_H
