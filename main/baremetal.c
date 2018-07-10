/*
 * baremetal.c
 *
 *  Created on: Jul 6, 2018
 *      Author: tetenkov
 */


#include <stdio.h>
#include "esp_sleep.h"
#include "esp_deep_sleep.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp32/ulp.h"
#include "ulp_app.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern const uint8_t bin_start[] asm("_binary_ulp_app_bin_start");
extern const uint8_t bin_end[]   asm("_binary_ulp_app_bin_end");

static void init_ulp_program();
static void update_pulse_count();


void bareMetal(void)
{
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    if (cause == ESP_SLEEP_WAKEUP_ULP) {
//    if (cause != ESP_SLEEP_WAKEUP_TIMER) {
        printf("ULP wakeup, saving pulse count\n");
        update_pulse_count();

    } else if (cause == ESP_SLEEP_WAKEUP_TIMER) {
        printf("edge_count = %d\n",ulp_edge_count& UINT16_MAX );
        printf("ulp_wake_time = %d\n",ulp_wake_time & UINT16_MAX );
        printf("ulp_wake_set = %d\n",ulp_wake_set & UINT16_MAX );
        printf("ulp_test = %d\n",ulp_test & UINT16_MAX );
    } else
    {
    	printf("Not ULP wakeup, initializing ULP\n");
        init_ulp_program();
    }
    printf("Entering deep sleep\n\n");
    ESP_ERROR_CHECK( esp_sleep_enable_ulp_wakeup() );
    ESP_ERROR_CHECK(esp_deep_sleep_enable_timer_wakeup(1000000));
    vTaskDelay(10);
    esp_deep_sleep_start();

}

static void init_ulp_program()
{
    esp_err_t err = ulp_load_binary(0, bin_start,
            (bin_end - bin_start) / sizeof(uint32_t));
    ESP_ERROR_CHECK(err);

    /* Initialize some variables used by ULP program.
     * Each 'ulp_xyz' variable corresponds to 'xyz' variable in the ULP program.
     * These variables are declared in an auto generated header file,
     * 'ulp_main.h', name of this file is defined in component.mk as ULP_APP_NAME.
     * These variables are located in RTC_SLOW_MEM and can be accessed both by the
     * ULP and the main CPUs.
     *
     * Note that the ULP reads only the lower 16 bits of these variables.
     */
    printf("edge_count = %d\n",ulp_edge_count);
    ulp_wake_period = 25;
    ulp_debounce_counter = 3;
    ulp_debounce_max_count = 3;
    ulp_next_edge = 0;
    ulp_io_number = 11; /* GPIO0 is RTC_IO 11 */
    ulp_edge_count_to_wake_up = 10;
    /* Initialize GPIO0 as RTC IO, input, disable pullup and pulldown */
    gpio_num_t gpio_num = GPIO_NUM_0;
    rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_INPUT_ONLY);
    rtc_gpio_pulldown_dis(gpio_num);
    rtc_gpio_pullup_dis(gpio_num);
    rtc_gpio_hold_en(gpio_num);

    /* Disconnect GPIO12 and GPIO15 to remove current drain through
     * pullup/pulldown resistors.
     * GPIO15 may be connected to ground to suppress boot messages.
     * GPIO12 may be pulled high to select flash voltage.
     */
    rtc_gpio_isolate(GPIO_NUM_12);
    rtc_gpio_isolate(GPIO_NUM_15);

    /* Set ULP wake up period to T = 20ms (3095 cycles of RTC_SLOW_CLK clock).
     * Minimum pulse width has to be T * (ulp_debounce_counter + 1) = 80ms.
     */
    REG_SET_FIELD(SENS_ULP_CP_SLEEP_CYC0_REG, SENS_SLEEP_CYCLES_S0, 3095);

    /* Start the program */
    err = ulp_run((&ulp_entry - RTC_SLOW_MEM) / sizeof(uint32_t));
    ESP_ERROR_CHECK(err);
}

static void update_pulse_count()
{
    const char* namespace = "plusecnt";
    const char* count_key = "count";
    printf("edge_count = %d\n",ulp_edge_count& UINT16_MAX );
    printf("ulp_wake_time = %d\n",ulp_wake_time & UINT16_MAX );
    printf("ulp_wake_set = %d\n",ulp_wake_set & UINT16_MAX );
    printf("ulp_test = %d\n",ulp_test & UINT16_MAX );
    ESP_ERROR_CHECK( nvs_flash_init() );
    nvs_handle handle;
    ESP_ERROR_CHECK( nvs_open(namespace, NVS_READWRITE, &handle));
    uint32_t pulse_count = 0;
    esp_err_t err = nvs_get_u32(handle, count_key, &pulse_count);
    assert(err == ESP_OK || err == ESP_ERR_NVS_NOT_FOUND);
    printf("Read pulse count from NVS: %5d\n", pulse_count);

    /* ULP program counts signal edges, convert that to the number of pulses */
    uint32_t pulse_count_from_ulp = (ulp_edge_count & UINT16_MAX) / 2;
    /* In case of an odd number of edges, keep one until next time */
    ulp_edge_count = ulp_edge_count % 2;
    printf("Pulse count from ULP: %5d\n", pulse_count_from_ulp);

    /* Save the new pulse count to NVS */
    pulse_count += pulse_count_from_ulp;
    ESP_ERROR_CHECK(nvs_set_u32(handle, count_key, pulse_count));
    ESP_ERROR_CHECK(nvs_commit(handle));
    nvs_close(handle);
    printf("Wrote updated pulse count to NVS: %5d\n", pulse_count);
}
