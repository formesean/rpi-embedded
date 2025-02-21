#ifndef SPICO_HPP
#define SPICO_HPP

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <cstdint>

// Documentation: https://www.raspberrypi.com/documentation/pico-sdk/hardware.html#group_hardware_spi

class SPICO
{
public:
    SPI(spi_inst_t *spi, uid baudrate);
    ~SPI();

    void setFormat(uint data_bits, spi_cpol_t cpol, spi_cpha_t cpha, spi_order_t order);
    void setBaudrate(uint baudrate);
    uint getBaudrate() const;
    void setMasterSlave(bool slave);

    bool isWritable() const;
    bool isReadable() const;
    bool isBusy() const;

    int write(const uint8_t *data, size_t len);
    int read(uint8_t repeated_tx_data, uint8_t *buffer, size_t len);
    int transfer(const uint8_t *tx_data, uint8_t *rx_data, size_t len);

private:
    spi_inst_t *spi_instance;
};

#endif

// EOF
