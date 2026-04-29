/*
 ******************************************************************************
 * @file    fifo_irq.c
 * @author  Sensors Software Solution Team
 * @brief   This file how to configure compressed FIFO and to retrieve acc
 *          and gyro data. This sample use a fifo utility library tool
 *          for FIFO decompression.
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/*
 * This example was developed using the following STMicroelectronics
 * evaluation boards:
 *
 * - STEVAL_MKI109V3 + STEVAL-MKI196V1
 * - NUCLEO_F411RE + X_NUCLEO_IKS01A3
 * - DISCOVERY_SPC584B + STEVAL-MKI196V1
 *
 * Used interfaces:
 *
 * STEVAL_MKI109V3    - Host side:   USB (Virtual COM)
 *                    - Sensor side: SPI(Default) / I2C(supported)
 *
 * NUCLEO_STM32F411RE - Host side: UART(COM) to USB bridge
 *                    - I2C(Default) / SPI(supported)
 *
 * DISCOVERY_SPC584B  - Host side: UART(COM) to USB bridge
 *                    - Sensor side: I2C(Default) / SPI(supported)
 *
 * If you need to run this example on a different hardware platform a
 * modification of the functions: `platform_write`, `platform_read`,
 * `tx_com` and 'platform_init' is required.
 *
 */

/* STMicroelectronics evaluation boards definition
 *
 * Please uncomment ONLY the evaluation boards in use.
 * If a different hardware is used please comment all
 * following target board and redefine yours.
 */

//#define STEVAL_MKI109V3  /* little endian */
//#define NUCLEO_F411RE    /* little endian */
//#define SPC584B_DIS      /* big endian */

/* ATTENTION: By default the driver is little endian. If you need switch
 *            to big endian please see "Endianness definitions" in the
 *            header file of the driver (_reg.h).
 */

#if defined(STEVAL_MKI109V3)
/* MKI109V3: Define communication interface */
#define SENSOR_BUS hspi2
/* MKI109V3: Vdd and Vddio power supply values */
#define PWM_3V3 915

#elif defined(NUCLEO_F411RE)
/* NUCLEO_F411RE: Define communication interface */
#define SENSOR_BUS hi2c1

#elif defined(SPC584B_DIS)
/* DISCOVERY_SPC584B: Define communication interface */
#define SENSOR_BUS I2CD1

#endif

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "lsm6dsv16x_reg.h"
#include "hc32_ddl.h"
#include "define.h"
#if defined(NUCLEO_F411RE)
#include "stm32f4xx_hal.h"
#include "usart.h"
#include "gpio.h"
#include "i2c.h"

#elif defined(STEVAL_MKI109V3)
#include "stm32f4xx_hal.h"
#include "usbd_cdc_if.h"
#include "gpio.h"
#include "spi.h"
#include "tim.h"

#elif defined(SPC584B_DIS)
#include "components.h"
#endif

/* Private macro -------------------------------------------------------------*/
/*
 * Select FIFO samples watermark, max value is 512
 * in FIFO are stored acc, gyro and timestamp samples
 */
#define BOOT_TIME         10
#define FIFO_WATERMARK    64

/* Private variables ---------------------------------------------------------*/
static uint8_t whoamI;

/* Private variables ---------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/*
 *   WARNING:
 *   Functions declare in this section are defined at the end of this file
 *   and are strictly related to the hardware platform used.
 *
 */
static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len);
static void platform_delay(uint32_t ms);
static void platform_init(void);

extern void Gyro_Write_value( uint8_t device_address,const uint8_t *value,M4_I2C_TypeDef* I2Cx,uint16_t len,uint8_t reg);
extern void Gyro_read_value(uint8_t device_address,uint8_t *u8RxBuf,M4_I2C_TypeDef* I2Cx,uint16_t len,uint8_t reg);
extern en_result_t Gyro_Mem_Read(uint8_t u8DevAddr, uint8_t *pu8Data,M4_I2C_TypeDef* I2Cx, uint32_t u32Size,uint8_t reg);
extern void sflp_set_gbias(float gbias[3]);


static stmdev_ctx_t dev_ctx;

#if 1

static uint32_t npy_halfbits_to_floatbits(uint16_t h)
{
    uint16_t h_exp, h_sig;
    uint32_t f_sgn, f_exp, f_sig;

    h_exp = (h&0x7c00u);
    f_sgn = ((uint32_t)h&0x8000u) << 16;
    switch (h_exp) {
        case 0x0000u: /* 0 or subnormal */
            h_sig = (h&0x03ffu);
            /* Signed zero */
            if (h_sig == 0) {
                return f_sgn;
            }
            /* Subnormal */
            h_sig <<= 1;
            while ((h_sig&0x0400u) == 0) {
                h_sig <<= 1;
                h_exp++;
            }
            f_exp = ((uint32_t)(127 - 15 - h_exp)) << 23;
            f_sig = ((uint32_t)(h_sig&0x03ffu)) << 13;
            return f_sgn + f_exp + f_sig;
        case 0x7c00u: /* inf or NaN */
            /* All-ones exponent and a copy of the significand */
            return f_sgn + 0x7f800000u + (((uint32_t)(h&0x03ffu)) << 13);
        default: /* normalized */
            /* Just need to adjust the exponent and shift */
            return f_sgn + (((uint32_t)(h&0x7fffu) + 0x1c000u) << 13);
    }
}

static float_t npy_half_to_float(uint16_t h)
{
    union { float_t ret; uint32_t retbits; } conv;
    conv.retbits = npy_halfbits_to_floatbits(h);
    return conv.ret;
}

static void sflp2q(float quat[4], uint16_t sflp[3])
{
  float sumsq = 0;

  quat[0] = npy_half_to_float(sflp[0]);
  quat[1] = npy_half_to_float(sflp[1]);
  quat[2] = npy_half_to_float(sflp[2]);

  for (uint8_t i = 0; i < 3; i++)
    sumsq += quat[i] * quat[i];

  if (sumsq > 1.0f)
    sumsq = 1.0f;

  quat[3] = sqrtf(1.0f - sumsq);
}

#endif


float lsm_yaw,lsm_pitch,lsm_roll = 0;
void lsm6dsv16x_fifo_irq_handler(void)
{
      uint16_t num = 0;
	lsm6dsv16x_fifo_status_t fifo_status;
    /* Read watermark flag */
    lsm6dsv16x_fifo_status_get(&dev_ctx, &fifo_status);

    if (fifo_status.fifo_th == 1) {
      num = fifo_status.fifo_level;

      while (num--) {
        lsm6dsv16x_fifo_out_raw_t f_data;
        float quat[4];

        /* Read FIFO sensor value */
        lsm6dsv16x_fifo_out_raw_get(&dev_ctx, &f_data);

        switch (f_data.tag) {
        case LSM6DSV16X_SFLP_GYROSCOPE_BIAS_TAG:
          break;
        case LSM6DSV16X_SFLP_GRAVITY_VECTOR_TAG:
          break;
        case LSM6DSV16X_SFLP_GAME_ROTATION_VECTOR_TAG:
          sflp2q(quat, (uint16_t *)&f_data.data[0]);
		  #if 1
			float x,y,z,w;
			float compute_x,compute_z;
			x = quat[0];
			y = quat[1];
			z = quat[2];
			w = quat[3];
			compute_x = -2*(x*y - w*z);
			compute_z = w*w-x*x+y*y-z*z;
			g_gyroscope.yaw_angle = (int16_t)(atan2(compute_x,compute_z)*57.29578);
			compute_z = w*w-x*x-y*y+z*z;
			compute_x = 2*(y*z+w*x);
			g_gyroscope.pitch_angle = (int16_t)(atan2(compute_x,compute_z)*57.29578);
			compute_x = -2*(x*z-w*y);
			compute_z = w*w-x*x-y*y+z*z;
			g_gyroscope.roll_angle = (int16_t)(atan2(compute_x,compute_z)*57.29578);
			if((g_gyroscope.pitch_angle > 85 && g_gyroscope.pitch_angle < 95)
				|| (g_gyroscope.pitch_angle < -85 && g_gyroscope.pitch_angle > -95))
			{
				g_gyroscope.roll_angle = 0;
			}
			if((g_gyroscope.roll_angle > 85 && g_gyroscope.roll_angle < 95)
				|| (g_gyroscope.roll_angle < -85 && g_gyroscope.roll_angle > -95))
			{
				g_gyroscope.pitch_angle = 0;
			}
		  #endif
          break;
        default:
         break;
        }
      }
//	  GPIO_SetPins(GPIO_PORT_C,GPIO_PIN_10);//∆µ…¡≤‚ ‘
  }
}

#define SFLP_ODR_15HZ   (0 << 3)
#define SFLP_ODR_30HZ   (1 << 3)
#define SFLP_ODR_60HZ   (2 << 3)
#define SFLP_ODR_120HZ  (3 << 3)
#define SFLP_ODR_240HZ  (4 << 3)
#define SFLP_ODR_480HZ  (5 << 3)
#define SFLP_ODR_MASK   0x38

#define USER_GBIAS_X    (0.0f) // dps
#define USER_GBIAS_Y    (0.0f) // dps
#define USER_GBIAS_Z    (0.0f) // dps



static lsm6dsv16x_fifo_sflp_raw_t fifo_sflp;
/* Main Example --------------------------------------------------------------*/
uint8_t lsm6dsv16x_fifo_irq(void)
{
  lsm6dsv16x_sflp_gbias_t gbias;
  lsm6dsv16x_pin_int_route_t pin_int;
  lsm6dsv16x_reset_t rst;

  /* Initialize mems driver interface */
  dev_ctx.write_reg = platform_write;
  dev_ctx.read_reg = platform_read;
  dev_ctx.mdelay = platform_delay;
//  dev_ctx.handle = &SENSOR_BUS;
  /* Init test platform */
  platform_init();
  /* Wait sensor boot time */
  platform_delay(BOOT_TIME);

//	float gbias[3] = {USER_GBIAS_X, USER_GBIAS_Y, USER_GBIAS_Z};
//	sflp_set_gbias(gbias);
  /* Check device ID */
  lsm6dsv16x_device_id_get(&dev_ctx, &whoamI);

  if (whoamI != LSM6DSV16X_ID)
  {
	  return 0;
  }
//    while (1);

  /* Restore default configuration */
  lsm6dsv16x_reset_set(&dev_ctx, LSM6DSV16X_GLOBAL_RST);
  do {
    lsm6dsv16x_reset_get(&dev_ctx, &rst);
  } while (rst != LSM6DSV16X_READY);

  /* Enable Block Data Update */
  lsm6dsv16x_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
  /* Set full scale */
  
  #if NULL
  lsm6dsv16x_xl_full_scale_set(&dev_ctx, LSM6DSV16X_2g);
  #endif
  
  #if 1
  lsm6dsv16x_xl_full_scale_set(&dev_ctx, LSM6DSV16X_4g);
  lsm6dsv16x_gy_full_scale_set(&dev_ctx, LSM6DSV16X_2000dps);
  #endif

  /*
   * Set FIFO watermark (number of unread sensor data TAG + 6 bytes
   * stored in FIFO) to FIFO_WATERMARK samples
   */
  lsm6dsv16x_fifo_watermark_set(&dev_ctx, FIFO_WATERMARK);
  
  #if 1
  fifo_sflp.game_rotation = 1;
  fifo_sflp.gravity = 1;
  fifo_sflp.gbias = 1;
  lsm6dsv16x_fifo_sflp_batch_set(&dev_ctx, fifo_sflp);
  #endif
  /* Set FIFO batch XL/Gyro ODR to 12.5Hz */
//  lsm6dsv16x_fifo_xl_batch_set(&dev_ctx, LSM6DSV16X_XL_BATCHED_AT_60Hz);
  /* Set FIFO mode to Stream mode (aka Continuous Mode) */
  lsm6dsv16x_fifo_mode_set(&dev_ctx, LSM6DSV16X_STREAM_MODE);
	
  pin_int.fifo_th = PROPERTY_ENABLE;
  lsm6dsv16x_pin_int1_route_set(&dev_ctx, &pin_int);
  //lsm6dsv16x_pin_int2_route_set(&dev_ctx, &pin_int);

  /* Set Output Data Rate */
//  lsm6dsv16x_xl_data_rate_set(&dev_ctx, LSM6DSV16X_ODR_AT_60Hz);
  lsm6dsv16x_fifo_timestamp_batch_set(&dev_ctx, LSM6DSV16X_TMSTMP_DEC_8);
  lsm6dsv16x_timestamp_set(&dev_ctx, PROPERTY_ENABLE);
  
  #if 1
    /* Set Output Data Rate */
  lsm6dsv16x_xl_data_rate_set(&dev_ctx, LSM6DSV16X_ODR_AT_15Hz);
  lsm6dsv16x_gy_data_rate_set(&dev_ctx, LSM6DSV16X_ODR_AT_15Hz);
  lsm6dsv16x_sflp_data_rate_set(&dev_ctx, LSM6DSV16X_SFLP_15Hz);

  lsm6dsv16x_sflp_game_rotation_set(&dev_ctx, PROPERTY_ENABLE);
  #endif

  gbias.gbias_x = 0.0f;
  gbias.gbias_y = 0.0f;
  gbias.gbias_z = 0.0f;
  lsm6dsv16x_sflp_game_gbias_set(&dev_ctx, &gbias);

  return 1;
  /* wait forever (FIFO samples read with irq) */
//  while (1);
}

/*
 * @brief  Write generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to write
 * @param  bufp      pointer to data to write in register reg
 * @param  len       number of consecutive register to write
 *
 */
static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp,
                              uint16_t len)
{
#if defined(NUCLEO_F411RE)
  HAL_I2C_Mem_Write(handle, LSM6DSV16X_I2C_ADD_L, reg,
                    I2C_MEMADD_SIZE_8BIT, (uint8_t*) bufp, len, 1000);
#elif defined(STEVAL_MKI109V3)
  HAL_GPIO_WritePin(CS_up_GPIO_Port, CS_up_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(handle, &reg, 1, 1000);
  HAL_SPI_Transmit(handle, (uint8_t*) bufp, len, 1000);
  HAL_GPIO_WritePin(CS_up_GPIO_Port, CS_up_Pin, GPIO_PIN_SET);
#elif defined(SPC584B_DIS)
  i2c_lld_write(handle,  LSM6DSV16X_I2C_ADD_H & 0xFE, reg, (uint8_t*) bufp, len);
#endif
	Gyro_Write_value(LSM6DSV16X_I2C_ADD_H ,bufp,M4_I2C3,len,reg);
  return 0;
}

/*
 * @brief  Read generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to read
 * @param  bufp      pointer to buffer that store the data read
 * @param  len       number of consecutive register to read
 *
 */
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len)
{
#if defined(NUCLEO_F411RE)
  HAL_I2C_Mem_Read(handle, LSM6DSV16X_I2C_ADD_L, reg,
                   I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
#elif defined(STEVAL_MKI109V3)
  reg |= 0x80;
  HAL_GPIO_WritePin(CS_up_GPIO_Port, CS_up_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(handle, &reg, 1, 1000);
  HAL_SPI_Receive(handle, bufp, len, 1000);
  HAL_GPIO_WritePin(CS_up_GPIO_Port, CS_up_Pin, GPIO_PIN_SET);
#elif defined(SPC584B_DIS)
  i2c_lld_read(handle, LSM6DSV16X_I2C_ADD_H & 0xFE, reg, bufp, len);
#endif
//	Gyro_read_value(LSM6DSV16X_I2C_ADD_H ,bufp,M4_I2C5,len,reg);
	Gyro_Mem_Read(LSM6DSV16X_I2C_ADD_H,bufp,M4_I2C3,len,reg);
  return 0;
}

/*
 * @brief  platform specific delay (platform dependent)
 *
 * @param  ms        delay in ms
 *
 */
static void platform_delay(uint32_t ms)
{
#if defined(NUCLEO_F411RE) | defined(STEVAL_MKI109V3)
  HAL_Delay(ms);
#elif defined(SPC584B_DIS)
  osalThreadDelayMilliseconds(ms);
#endif
  DDL_DelayMS(ms);
}

/*
 * @brief  platform specific initialization (platform dependent)
 */
static void platform_init(void)
{
#if defined(STEVAL_MKI109V3)
  TIM3->CCR1 = PWM_3V3;
  TIM3->CCR2 = PWM_3V3;
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
  HAL_Delay(1000);
#endif
}
