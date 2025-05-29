#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#include "icm42688_consts.h"

#define SPI_SPEED 10000000 // 10 MHz (chip maximum is 24MHz, but my original wiring couldn't handle that
#define SPI_MODE 0

#define SPI_DEVICE "/dev/spidev0.0"


int spi_fd;

void spi_init() {
    spi_fd = open(SPI_DEVICE, O_RDWR);
    if (!spi_fd) {
        LOG_FATAL("IMU issue: failed to open SPI device");
    }


    uint8_t mode = SPI_MODE;
    if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) == -1) {
        LOG_FATAL("Failed to set SPI mode");
    }

    uint8_t bits = 8;
    if (ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1) {
        LOG_FATAL("Filaed to set SPI bits");
    }

    uint32_t speed = SPI_SPEED;
    if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) {
        LOG_FATAL("Failed to set SPI max speed");
    } 
}

uint8_t read_byte(uint8_t address) {
    uint8_t tx_buf[2] = { address | 0x80, 0x0};
    uint8_t rx_buf[2] = {0};

    struct spi_ioc_transfer transfer = {
        .tx_buf = (unsigned long) tx_buf,
        .rx_buf = (unsigned long) rx_buf,
        .len = 2,
        .speed_hz = SPI_SPEED,
        .bits_per_word = 8,
        .delay_usecs = 0
    };
    
    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &transfer) == -1) {
        LOG_FATAL("SPI transfer failed");
    }

    return rx_buf[1];
}

void write_byte(uint8_t address, uint8_t byte) {
    uint8_t tx_buf[2] = { address & 0x7F, byte};
    uint8_t rx_buf[2] = {0};

    struct spi_ioc_transfer transfer = {
        .tx_buf = (unsigned long) tx_buf,
        .rx_buf = (unsigned long) rx_buf,
        .len = 2,
        .speed_hz = SPI_SPEED,
        .bits_per_word = 8,
        .delay_usecs = 10
    };

    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &transfer) == -1) {
        LOG_FATAL("SPI transfer failed");
    }
}

int16_t read_uint16(uint8_t address1, uint8_t address2) {
    uint8_t high = read_byte(address1);
    uint8_t low = read_byte(address2);
    
    return (((int16_t) high) << 8) + read_byte(address2);
}


void enableGyro() {
    uint8_t current = read_byte(REG_PWR_MGMT0);
    write_byte(REG_PWR_MGMT0, current | GYRO_EN_BITS);
}

bool isGyroEnabled() {
    return read_byte(REG_PWR_MGMT0) & GYRO_EN_BITS;
}

float readGyro() {
    return (float) read_uint16(REG_GYRO_DATA_X_HIGH, REG_GYRO_DATA_X_LOW) * 2000 / (1<<16);
}

float readTemp() {
    return ((float) read_uint16(REG_TEMP_DATA_HIGH, REG_TEMP_DATA_LOW)) / 132.48 + 25;
}

void init_imu(void) {
    spi_init();
    uint8_t who = read_byte(REG_WHO_AM_I);
    if (who != WHO_AM_I_RES) {
        LOG_FATAL("Device mismatch verify that the imu is correctly connected (expected: 0x%x, got 0x%x", WHO_AM_I_RES, who);
    }
    LOG_DEBUG("Device match, proceed with IMU startup");
    enableGyro();
}
