deps_config := \
	/home/tetenkov/esp/esp-idf/components/app_trace/Kconfig \
	/home/tetenkov/esp/esp-idf/components/aws_iot/Kconfig \
	/home/tetenkov/esp/esp-idf/components/bt/Kconfig \
	/home/tetenkov/esp/esp-idf/components/driver/Kconfig \
	/home/tetenkov/esp/esp-idf/components/esp32/Kconfig \
	/home/tetenkov/esp/esp-idf/components/esp_adc_cal/Kconfig \
	/home/tetenkov/esp/esp-idf/components/esp_http_client/Kconfig \
	/home/tetenkov/esp/esp-idf/components/ethernet/Kconfig \
	/home/tetenkov/esp/esp-idf/components/fatfs/Kconfig \
	/home/tetenkov/esp/esp-idf/components/freertos/Kconfig \
	/home/tetenkov/esp/esp-idf/components/heap/Kconfig \
	/home/tetenkov/esp/esp-idf/components/libsodium/Kconfig \
	/home/tetenkov/esp/esp-idf/components/log/Kconfig \
	/home/tetenkov/esp/esp-idf/components/lwip/Kconfig \
	/home/tetenkov/esp/esp-idf/components/mbedtls/Kconfig \
	/home/tetenkov/esp/esp-idf/components/mdns/Kconfig \
	/home/tetenkov/esp/esp-idf/components/openssl/Kconfig \
	/home/tetenkov/esp/esp-idf/components/pthread/Kconfig \
	/home/tetenkov/esp/esp-idf/components/spi_flash/Kconfig \
	/home/tetenkov/esp/esp-idf/components/spiffs/Kconfig \
	/home/tetenkov/esp/esp-idf/components/tcpip_adapter/Kconfig \
	/home/tetenkov/esp/esp-idf/components/vfs/Kconfig \
	/home/tetenkov/esp/esp-idf/components/wear_levelling/Kconfig \
	/home/tetenkov/esp/esp-idf/Kconfig.compiler \
	/home/tetenkov/esp/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/tetenkov/esp/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/tetenkov/esp/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/tetenkov/esp/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
