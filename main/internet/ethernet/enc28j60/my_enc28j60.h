#ifndef ENCJ28J60_H
#define ENCJ28J60_H

#include "program_variables.h"
#include "internet/ethernet/enc28j60/esp_eth_enc28j60.h"
#include "internet/protocol/http/http.h"

static const char *CONFIG_EXAMPLE_ENC28J60_TAG = "eth_example";
#define CONFIG_EXAMPLE_ENC28J60_MISO_GPIO 28
#define CONFIG_EXAMPLE_ENC28J60_MOSI_GPIO 25
#define CONFIG_EXAMPLE_ENC28J60_SCLK_GPIO 29
#define CONFIG_EXAMPLE_ENC28J60_SPI_CLOCK_MHZ 8
#define CONFIG_EXAMPLE_ENC28J60_CS_GPIO 26
#define CONFIG_EXAMPLE_ENC28J60_INT_GPIO 27
#define CONFIG_EXAMPLE_ENC28J60_SPI_HOST spi_host_device_t::SPI2_HOST
#define CONFIG_EXAMPLE_ENC28J60_DUPLEX_FULL true

void eth_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
void got_ip_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

void enc28j60_Init(void);
#endif //ENCJ28J60_H