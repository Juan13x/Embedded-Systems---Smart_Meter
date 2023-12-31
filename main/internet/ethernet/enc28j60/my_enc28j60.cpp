#include "internet/ethernet/enc28j60/my_enc28j60.h"

/** Event handler for Ethernet events */
void eth_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    uint8_t mac_addr[6] = {0};
    /* we can get the ethernet driver handle from event data */
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

    switch (event_id) {
    case ETHERNET_EVENT_CONNECTED:
        esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
        ESP_LOGI(CONFIG_EXAMPLE_ENC28J60_TAG, "Ethernet Link Up");
        ESP_LOGI(CONFIG_EXAMPLE_ENC28J60_TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(CONFIG_EXAMPLE_ENC28J60_TAG, "Ethernet Link Down");
        break;
    case ETHERNET_EVENT_START:
        ESP_LOGI(CONFIG_EXAMPLE_ENC28J60_TAG, "Ethernet Started");
        break;
    case ETHERNET_EVENT_STOP:
        ESP_LOGI(CONFIG_EXAMPLE_ENC28J60_TAG, "Ethernet Stopped");
        break;
    default:
        break;
    }
}

/** Event handler for IP_EVENT_ETH_GOT_IP */
void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    ESP_LOGI(CONFIG_EXAMPLE_ENC28J60_TAG, "Ethernet Got IP Address");
    ESP_LOGI(CONFIG_EXAMPLE_ENC28J60_TAG, "~~~~~~~~~~~");
    ESP_LOGI(CONFIG_EXAMPLE_ENC28J60_TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(CONFIG_EXAMPLE_ENC28J60_TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(CONFIG_EXAMPLE_ENC28J60_TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(CONFIG_EXAMPLE_ENC28J60_TAG, "~~~~~~~~~~~");
    states.enableInternet();
}

void enc28j60_Init(void)
{
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
    esp_netif_t *eth_netif = esp_netif_new(&netif_cfg);

    spi_bus_config_t buscfg = spi_bus_config_t();
    buscfg.miso_io_num = CONFIG_EXAMPLE_ENC28J60_MISO_GPIO;
    buscfg.mosi_io_num = CONFIG_EXAMPLE_ENC28J60_MOSI_GPIO;
    buscfg.sclk_io_num = CONFIG_EXAMPLE_ENC28J60_SCLK_GPIO;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = k_internet_http_MAX_OUTPUT_BUFFER;
    
    ESP_ERROR_CHECK(spi_bus_initialize(CONFIG_EXAMPLE_ENC28J60_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));

    /* ENC28J60 ethernet driver is based on spi driver */
    spi_device_interface_config_t spi_devcfg = spi_device_interface_config_t();
    spi_devcfg.mode = 0;
    spi_devcfg.clock_speed_hz = CONFIG_EXAMPLE_ENC28J60_SPI_CLOCK_MHZ * 1000 * 1000;
    spi_devcfg.spics_io_num = CONFIG_EXAMPLE_ENC28J60_CS_GPIO;
    spi_devcfg.queue_size = 20;
    spi_devcfg.cs_ena_posttrans = enc28j60_cal_spi_cs_hold_time(CONFIG_EXAMPLE_ENC28J60_SPI_CLOCK_MHZ);

    eth_enc28j60_config_t enc28j60_config = ETH_ENC28J60_DEFAULT_CONFIG(CONFIG_EXAMPLE_ENC28J60_SPI_HOST, &spi_devcfg);
    enc28j60_config.int_gpio_num = CONFIG_EXAMPLE_ENC28J60_INT_GPIO;

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    esp_eth_mac_t *mac = esp_eth_mac_new_enc28j60(&enc28j60_config, &mac_config);

    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    phy_config.autonego_timeout_ms = 0; // ENC28J60 doesn't support auto-negotiation
    phy_config.reset_gpio_num = -1; // ENC28J60 doesn't have a pin to reset internal PHY
    esp_eth_phy_t *phy = esp_eth_phy_new_enc28j60(&phy_config);

    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    ESP_ERROR_CHECK(esp_eth_driver_install(&eth_config, &eth_handle));

    /* ENC28J60 doesn't burn any factory MAC address, we need to set it manually.
       02:00:00 is a Locally Administered OUI range so should not be used except when testing on a LAN under your control.
    */
    uint8_t address[] = {0x02, 0x00, 0x00, 0x12, 0x34, 0x56};
    mac->set_addr(mac, address);

    // ENC28J60 Errata #1 check
    if (emac_enc28j60_get_chip_info(mac) < ENC28J60_REV_B5 && CONFIG_EXAMPLE_ENC28J60_SPI_CLOCK_MHZ < 8) {
        ESP_LOGE(CONFIG_EXAMPLE_ENC28J60_TAG, "SPI frequency must be at least 8 MHz for chip revision less than 5");
        ESP_ERROR_CHECK(ESP_FAIL);
    }

    /* attach Ethernet driver to TCP/IP stack */
    ESP_ERROR_CHECK(esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle)));
    // Register user defined event handers
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL));

    /* It is recommended to use ENC28J60 in Full Duplex mode since multiple errata exist to the Half Duplex mode */
#if CONFIG_EXAMPLE_ENC28J60_DUPLEX_FULL
    eth_duplex_t duplex = ETH_DUPLEX_FULL;
    ESP_ERROR_CHECK(esp_eth_ioctl(eth_handle, ETH_CMD_S_DUPLEX_MODE, &duplex));
#endif

    /* start Ethernet driver state machine */
    ESP_ERROR_CHECK(esp_eth_start(eth_handle));
}


// void http_test_task(void *pvParameters)
// {
//     while(1){
//         vTaskDelay(pdMS_TO_TICKS(3000));
//         http_rest_with_url();
//     }
    
// }