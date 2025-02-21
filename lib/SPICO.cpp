#include "SPICO.hpp"

SPICO::SPI(spi_inst_t *spi, uint baudrate) : spi_instance(spi)
{
    spi_init(spi_instance, baudrate);
}

SPICO::~SPI()
{
    spi_deinit(spi_instance);
}

void SPICO::setFormat(uint data_bits, spi_cpol_t cpol, spi_cpha_t cpha, spi_order_t order)
{
    spi_set_format(spi_instance, data_bits, cpol, cpha, order);
}

void SPICO::setBaudrate(uint baudrate)
{
    spi_set_baudrate(spi_instance, baudrate);
}

uint SPICO::getBaudrate() const
{
    return spi_get_baudrate(spi_instance);
}

void SPICO::setMasterSlave(bool slave)
{
    spi_set_slave(spi_instance, slave);
}

bool SPICO::isWritable() const
{
    return spi_is_writable(spi_instance);
}

bool SPICO::isReadable() const
{
    return spi_is_readable(spi_instance);
}

bool SPICO::isBusy() const
{
    return spi_is_busy(spi_instance);
}

int SPICO::write(const uint8_t *data, size_t len)
{
    return spi_write_blocking(spi_instance, data, len);
}

int SPICO::read(uint8_t repeated_tx_data, uint8_t *buffer, size_t len)
{
    return spi_read_blocking(spi_instance, repeated_tx_data, buffer, len);
}

int SPICO::transfer(const uint8_t *tx_data, uint8_t *rx_data, size_t len)
{
    return spi_write_read_blocking(spi_instance, tx_data, rx_data, len);
}

// EOF
