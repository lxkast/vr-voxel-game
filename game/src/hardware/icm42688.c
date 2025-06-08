#include <sys/ioctl.h>
#include <logging.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "icm42688.h"
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
    uint8_t buf[2] = { address | 0x80, 0x0};

    struct spi_ioc_transfer transfer = {
        .tx_buf = (unsigned long) buf,
        .rx_buf = (unsigned long) buf,
        .len = 2,
        .speed_hz = SPI_SPEED,
        .bits_per_word = 8,
        .delay_usecs = 0
    };
    
    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &transfer) == -1) {
        LOG_FATAL("SPI transfer failed");
    }

    return buf[1];
}

void read_nbytes(uint8_t address, uint16_t length, uint8_t *res_buf) {
    uint8_t *buf = malloc(length+1); // we need a new buffer as we have to set the address
    memset(buf, 0, length+1);
    buf[0] = address | 0x80;
    struct spi_ioc_transfer transfer = {
        .tx_buf = (unsigned long) buf,
        .rx_buf = (unsigned long) buf, 
        .len = length + 1,
        .speed_hz = SPI_SPEED,
        .bits_per_word = 8,
        .delay_usecs = 0
    };

    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &transfer) == -1) {
        LOG_FATAL("SPI transfer failed");
    }
    memcpy(res_buf, buf+1, length);
    free(buf);
}

void write_byte(uint8_t address, uint8_t byte) {
    uint8_t buf[2] = { address & 0x7F, byte};

    struct spi_ioc_transfer transfer = {
        .tx_buf = (unsigned long) buf,
        .rx_buf = (unsigned long) buf,
        .len = 2,
        .speed_hz = SPI_SPEED,
        .bits_per_word = 8,
        .delay_usecs = 10
    };

    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &transfer) == -1) {
        LOG_FATAL("SPI transfer failed");
    }
}

uint16_t read_uint16(uint8_t address1, uint8_t address2) {
    uint8_t high = read_byte(address1);
    uint8_t low = read_byte(address2);
    
    return (((uint16_t) high) << 8) + read_byte(address2);
}

void enableAccel() {
    write_byte(REG_ACCEL_CONFIG0, ACCEL_CONFIG0_FS_4G | 0xB);

    uint8_t current = read_byte(REG_PWR_MGMT0);
    write_byte(REG_PWR_MGMT0, current | PWR_MGMT0_ACCEL_EN);

    usleep(ENABLE_DISABLE_DELAY);
}

void enableGyro() {
    write_byte(REG_GYRO_CONFIG0, GYRO_CONFIG0_FS_125DPS | GYRO_CONFIG0_ODR_100HZ);
    
    uint8_t current = read_byte(REG_PWR_MGMT0);
    write_byte(REG_PWR_MGMT0, current | PWR_MGMT0_GYRO_EN);
    usleep(ENABLE_DISABLE_DELAY);
}

bool isGyroEnabled() {
    return read_byte(REG_PWR_MGMT0) & PWR_MGMT0_GYRO_EN;
}

float readGyro() {
    return ((float) ((int16_t) read_uint16(REG_GYRO_DATA_X_HIGH, REG_GYRO_DATA_X_LOW))) * 2000 / (1<<16);
}

float readTemp() {
    return ((float) ((int16_t)  read_uint16(REG_TEMP_DATA_HIGH, REG_TEMP_DATA_LOW))) / 132.48 + 25;
}


/*
 *  Configures the FIFO how we want it. TODO generalise this more
 */
void configureFIFO() {
    uint8_t data = 0;
    data |= FIFO_CONFIG1_PARTIAL_READ;
    data |= FIFO_CONFIG1_TEMP_EN;
    data |= FIFO_CONFIG1_GYRO_EN;
    data |= FIFO_CONFIG1_ACCEL_EN;

    write_byte(REG_FIFO_CONFIG1, data);

    write_byte(REG_FIFO_CONFIG, FIFO_CONFIG_STREAM);
}

uint16_t readFIFOLength() {
    return read_uint16(REG_FIFO_COUNT_HIGH, REG_FIFO_COUNT_LOW);
}

void resetDevice() {
    write_byte(REG_DEVICE_CONFIG, DEVICE_CONFIG_RESET);
    usleep(2000);
}

bool readVec3(uint8_t *data, double scale, double *dest) {
    int16_t comp1 = ((int16_t) data[0] << 8) + data[1];
    int16_t comp2 = ((int16_t) data[2] << 8) + data[3];
    int16_t comp3 = ((int16_t) data[4] << 8) + data[5];
    // check if only the first bit is set, in which case data is invalid
    if (comp1 == (int16_t) 0x8000 || comp2 == (int16_t) 0x8000 || comp3 == (int16_t) 0x8000) {
        return false;
    }

    dest[0] = ((double) comp1) * scale;
    dest[1] = ((double) comp2) * scale;
    dest[2] = ((double) comp3) * scale;
    return true;
}

void readPacket(uint8_t *data, uint16_t *read_amount, fifodata_t *res) {
    static long time_resets = 0;
    static uint16_t last_time = 0;

    if ((data[0] & 0xFC) == 0x68) {
        if (readVec3(data + 7, GYRO_SCALE_FACTOR, res->gyro)) {
            res->gyroValid = true;
        } else {
            res->gyroValid = false;
        }
    
        if (readVec3(data + 1, ACCEL_SCALE_FACTOR, res->accel)) {
            res->accelValid = true;
        } else {
            res->accelValid = false;
        }
        uint16_t current_time = (((uint16_t) data[14]) << 8) + data[15];
        if (current_time < last_time) {
            time_resets ++;
        }
        last_time = current_time;
        double partialTime = ((double) current_time) * TMSP_SCALE_FACTOR;
        res->timestamp = partialTime + time_resets * TMSP_PER_RESET;
        *read_amount = 16;
    } else {
        LOG_FATAL("Unsupported packet type %x", data[0]);
    }
}

bool readFIFOData(fifodata_t *res) {
    uint16_t bytes = readFIFOLength();
    if (bytes > 0) {
        uint8_t *data = malloc(bytes);
        read_nbytes(REG_FIFO_DATA, bytes, data);
        
        uint16_t length = 0;  // for now assume only one packet
        readPacket(data, &length, res); 
        return true;
    }
    return false;
}

void init_imu(void) {
    spi_init();
    uint8_t who = read_byte(REG_WHO_AM_I);
    if (who != WHO_AM_I_RES) {
        LOG_FATAL("Device mismatch verify that the imu is correctly connected (expected: 0x%x, got 0x%x", WHO_AM_I_RES, who);
    }
    resetDevice();
    LOG_DEBUG("Device match, proceed with IMU startup");
    configureFIFO();
    enableGyro();
    enableAccel();
    usleep(1000); // No register access allowed for at least 300us.
}
