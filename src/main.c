#include <string.h>
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/task.h"

#include "sdkconfig.h" // generated by "make menuconfig"

#include "cryptoauthlib.h"
#include "tng_atcacert_client.h"

#define SDA2_PIN GPIO_NUM_18
#define SCL2_PIN GPIO_NUM_19

#define TAG_ECC608 "ECC608"

#define I2C_MASTER_ACK 0
#define I2C_MASTER_NACK 1

void i2c_master_init()
{
	i2c_config_t i2c_config = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = SDA2_PIN,
		.scl_io_num = SCL2_PIN,
		.sda_pullup_en = GPIO_PULLUP_DISABLE,
		.scl_pullup_en = GPIO_PULLUP_DISABLE,
		.master.clk_speed = 100000
		};
			
	i2c_param_config(I2C_NUM_0 , &i2c_config);
	i2c_driver_install(I2C_NUM_0 , I2C_MODE_MASTER, 0, 0, 0);


}

void maintask(void *ignore)
{
    uint8_t devicecertder[600];
    size_t devicecertderlen;
    uint8_t signercertder[600];
    size_t signercertderlen;
    uint8_t rootcertder[600];
    size_t rootcertderlen;


        ATCAIfaceCfg cfg = {
                .iface_type             = ATCA_I2C_IFACE,
                .devtype                = ATECC608A,
                .atcai2c.slave_address  = 0X6A,
                .atcai2c.bus            = 1,
                .atcai2c.baud           = 100000,
                .wake_delay             = 1500,
                .rx_retries             = 20
        };

    ATCA_STATUS status = atcab_init(&cfg);

    if (status != ATCA_SUCCESS) {
        ESP_LOGE(TAG_ECC608, "atcab_init() failed with ret=0x%08d\r\n", status);
    }
	

    uint8_t serial[ATCA_SERIAL_NUM_SIZE];
    status = atcab_read_serial_number(serial);

    if (status != ATCA_SUCCESS) {
	ESP_LOGE(TAG_ECC608, "atcab_read_serial_number() failed with ret=0x%08d/r/n", status);
    }

    
    uint8_t revision[INFO_SIZE];
    status = atcab_info(revision);

    if (status != ATCA_SUCCESS) {
	ESP_LOGE(TAG_ECC608, "atcab_read_serial_number() failed with ret=0x%08d/r/n", status);
    }


    uint8_t config_data[ATCA_ECC_CONFIG_SIZE];
    status = atcab_read_config_zone(config_data);

    if (status != ATCA_SUCCESS) {
	ESP_LOGE(TAG_ECC608, "atcab_read_config_zone() failed with ret=0x%08d/r/n", status);
    }
	

	ESP_LOGI(TAG_ECC608, "Serial Number:");

    char serialstr[18] = {};
    char character[3] = {};

	for ( int i = 0; i< 9; i++){
        sprintf(character , "%02x", serial[i]);
	    strcat(serialstr, character);
        }

    printf(serialstr);
    printf("\n");

	ESP_LOGI(TAG_ECC608, "Revision Number:");

	for ( int i =0; i< 4; i++){
	    ESP_LOGI(TAG_ECC608,"%02x ", revision[i]);
        }

	ESP_LOGI(TAG_ECC608, "Config Zone data:");

        for (int i = 0; i < 16; i++){
           for(int j = 0; j < 8; j++){
	     printf("%02x ", config_data[i * 8 + j]);
           }
	   printf("\n");
        }

    int ret;
    char printout[1200];
    size_t printlen = 1200;

    //get device CA

    ret = tng_atcacert_read_device_cert(devicecertder , &devicecertderlen, NULL);

    if (ret != ATCACERT_E_SUCCESS) {
	    ESP_LOGE(TAG_ECC608, "tng_atcacert_read_device_cert error with ret=0x%08d/r/n", status);
    }
    /* Convert to base 64 */
    status = atcab_base64encode( devicecertder , devicecertderlen , printout, &printlen);

    /* Add a null terminator */
    printout[printlen] = 0;

    printf("\ndevice cert:\n\n");

    /* Print out the key */
    printf("-----BEGIN CERTIFICATE-----\n%s\n-----END CERTIFICATE-----\n", printout);

    //get signer CA
    printlen = 1200;

    ret = tng_atcacert_read_signer_cert(signercertder , &signercertderlen);

    if (ret != ATCACERT_E_SUCCESS) {
	    ESP_LOGE(TAG_ECC608, "tng_atcacert_read_signer_cert error with ret=0x%08d/r/n", status);
    }
    /* Convert to base 64 */
    status = atcab_base64encode( signercertder , signercertderlen , printout, &printlen);

    /* Add a null terminator */
    printout[printlen] = 0;

    printf("\nsigner cert:\n\n");

    /* Print out the key */
    printf("-----BEGIN CERTIFICATE-----\n%s\n-----END CERTIFICATE-----\n", printout);

    //get root CA
    printlen = 1200;

    ret = tng_atcacert_root_cert(rootcertder , &rootcertderlen);

    if (ret != ATCACERT_E_SUCCESS) {
	    ESP_LOGE(TAG_ECC608, "tng_atcacert_root_cert error with ret=0x%08d/r/n", status);
    }
    /* Convert to base 64 */
    status = atcab_base64encode( rootcertder , rootcertderlen , printout, &printlen);

    /* Add a null terminator */
    printout[printlen] = 0;

    printf("\nroot cert:\n\n");

    /* Print out the key */
    printf("-----BEGIN CERTIFICATE-----\n%s\n-----END CERTIFICATE-----\n", printout);




    atcab_release();

	vTaskDelete(NULL);
}



void app_main(void)
{
	i2c_master_init();
	xTaskCreate(&maintask, "maintask",  32768, NULL, 6, NULL);
}
