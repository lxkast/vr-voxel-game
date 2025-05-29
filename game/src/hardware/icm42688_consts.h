#pragma once



#define REG_WHO_AM_I (uint8_t) 0x75
#define WHO_AM_I_RES (uint8_t) 0x47

#define REG_PWR_MGMT0 (uint8_t) 0x4E
#define GYRO_EN_BITS (uint8_t) 0x0C

#define REG_GYRO_DATA_X_HIGH (uint8_t) 0x25
#define REG_GYRO_DATA_X_LOW (uint8_t) 0x26
#define REG_TEMP_DATA_LOW (uint8_t) 0x1E
#define REG_TEMP_DATA_HIGH (uint8_t) 0x1D

#define REG_FIFO_CONFIG1 (uint8_t) 0x5F
#define FIFO_CONFIG1_PARTIAL_READ (uint8_t) 0x40
#define FIFO_CONFIG1_FIFO_WM_GT_TH (uint8_t) 0x20
#define FIFO_CONFIG1_HIRES_EN (uint8_t) 0x10
#define FIFO_CONFIG1_TMST_FYSNC_EN (uint8_t) 0x08
#define FIFO_CONFIG1_TEMP_EN (uint8_t) 0x04
#define FIFO_CONFIG1_GYRO_EN (uint8_t) 0x02
#define FIFO_CONFIG1_ACCEL_EN (uint8_t) 0x01

#define FIFO_CONFIG (uint8_t) 0x16
#define FIFO_CONFIG_STREAM (uint8_t) 0x40
#define FIFO_CONFIG_STOP_FULL (uint8_t) 0xC0
