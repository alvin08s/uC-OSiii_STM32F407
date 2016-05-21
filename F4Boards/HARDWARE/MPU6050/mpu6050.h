#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "stdint.h"
#include "stm32f4xx_i2c.h"


/***define reg of mpu6050**/
#define I2C1_ADDR    ((uint8_t)0xE3)  //b1101000
#define MPU6050_ADDR ((uint8_t)0xD0)
#define MPU6050_WHO_AM_I ((uint8_t)0x75)
#define MPU6050_ADDRESS_AD0_LOW ((uint8_t)0x68)
#define MPU6050_ADDRESS_AD0_HIGH ((uint8_t)0x69)

#define  SMPRT_DIV    0X19  //0X07
#define  CONFIG    0X1A     //0X06
#define  GYRO_CONFIG   0X1B  //0X18
#define  ACCEL_CONFIG  0X1C   //0X01
#define  ACCEL_XOUT_H   ((uint8_t)0X3B)
#define  ACCEL_XOUT_L  ((uint8_t)0X3C)
#define  ACCEL_YOUT_H   ((uint8_t)0X3D)
#define  ACCEL_YOUT_L  ((uint8_t)0X3E)
#define  ACCEL_ZOUT_H   ((uint8_t)0X3F)
#define  ACCEL_ZOUT_L  ((uint8_t)0X40)
#define  TEMP_OUT_H    ((uint8_t)0X41)
#define  TEMP_OUT_L    ((uint8_t)0X42)
#define  GYRO_XOUT_H   ((uint8_t)0X43)
#define  GYRO_XOUT_L   ((uint8_t)0X44)
#define  GYRO_YOUT_H   ((uint8_t)0X45)
#define  GYRO_YOUT_L   ((uint8_t)0X46)
#define  GYRO_ZOUT_H   ((uint8_t)0X47)
#define  GYRO_ZOUT_L   ((uint8_t)0X48)
#define  PWR_MGMT_1    0X6B  //0X00


void mpu6050_init(void);
uint8_t Read_MPU6050_Data(uint8_t reg_addr,uint16_t *data,u8 len);
uint8_t I2C_Write(I2C_TypeDef *I2Cx,uint8_t slave_addr,uint8_t reg_addr,uint8_t value);
uint8_t I2C_Read(I2C_TypeDef *I2Cx,uint8_t slave_addr,uint8_t reg_addr,uint8_t *buf,uint16_t length);


#endif
