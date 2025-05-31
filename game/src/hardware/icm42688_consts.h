#pragma once

/*
 * These numbers are taken straight from the datasheet of the ICM42688, I have ommited registers
 * and values that I don't intend on ever using for the sake of clarity and because otherwise
 * this file would be thousands of lines long.
 * NOTE TO FUTURE ME: This is incomplete and likely has some mistakes, verify before using macros
 * for anything else.
 * This header file is deliberately designed to be standalone, so defines PI
 */
#define PI 3.14159
#define G 9.81

#define REG_WHO_AM_I (uint8_t) 0x75
#define WHO_AM_I_RES (uint8_t) 0x47

#define REG_PWR_MGMT0 (uint8_t) 0x4E
#define PWR_MGMT0_GYRO_EN (uint8_t) 0x0C
#define PWR_MGMT0_ACCEL_EN (uint8_t) 0x03

#define ENABLE_DISABLE_DELAY 300 // min 200us, set higher just in case

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

#define REG_FIFO_CONFIG (uint8_t) 0x16
#define FIFO_CONFIG_STREAM (uint8_t) 0x40
#define FIFO_CONFIG_STOP_FULL (uint8_t) 0xC0

#define REG_FIFO_DATA (uint8_t) 0x30

#define REG_FIFO_COUNT_LOW (uint8_t) 0x2F
#define REG_FIFO_COUNT_HIGH (uint8_t) 0x2E

#define REG_GYRO_CONFIG0 (uint8_t) 0x4F
#define GYRO_CONFIG0_FS_125DPS (uint8_t) 0x80
#define GYRO_CONFIG0_ODR_100HZ (uint8_t) 0x08

#define REG_ACCEL_CONFIG0 (uint8_t) 0x50
#define ACCEL_CONFIG0_FS_4G (uint8_t) 0x40
#define ACCEL_CONFIG0_ODR_100HZ (uint8_t) 0x08

#define REG_DEVICE_CONFIG (uint8_t) 0x11
#define DEVICE_CONFIG_RESET (uint8_t) 0x1

#define REG_TMST_CONFIG (uint8_t) 0x54
#define TMST_FSYNC_EN (uint8_t) 0x02
#define TMSP_SCALE_FACTOR ((32.0 / 30000000) * CLOCK_CORRECT_FACTOR)
#define TMSP_PER_RESET (65536.0 * TMSP_SCALE_FACTOR) 
#define GYRO_SCALE_FACTOR (((1.0 / 131) / 180) * PI)
#define ACCEL_SCALE_FACTOR ((1.0 / 8192) * G)


// ALL settings below this point are hardware specific
// These values are obtained experimentally and are only valid for the exact
// IMU which we have got, different IMU's of the same type will have slightly
// different result
#define CLOCK_CORRECT_FACTOR 0.991 // WARNING: this is calculated experimentally for this exact chip, it is hardware specific, so won't be common across multiple chips
