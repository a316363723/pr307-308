#include "app_dac_self_adapt.h"
#include "api_rs485_proto.h"
#include "hal_iic.h"
#include <string.h>
#include <stdint.h>
#include "dev_power.h"
#include "app_power.h"
#include "hal_timer.h"
#include "hc32_ddl.h"
#include "perf_counter.h"
#include "app_data_center.h"


#define USE_NEW_DIM 1
#define RS485_ANALOG_FADE_TIME      (1000)
static uint8_t read_buff[200] = {0};


#if PROJECT_TYPE==307
struct dac_change dac_fade =
{
  .analog_dac_init = {.r= 300,.g = 300,.b = 300,.ww = 300,.cw = 300},
  .analog_dac_start = {0},
  .analog_dac_current = {0},
  .analog_dac_end = {0},
  .run_time = 0,
  .fade_start = 0,

};


#elif PROJECT_TYPE==308
struct dac_change dac_fade =
{
  .analog_dac_init = {.r= 200,.g = 200,.b = 200,.ww = 200,.cw = 200},
  .analog_dac_start = {0},
  .analog_dac_current = {0},
  .analog_dac_end = {0},
  .run_time = 0,
  .fade_start = 0,

};


#endif

void analog_dim_fade_restart(void)
{
  dac_fade.run_time = 0;
  dac_fade.fade_start = 1;
}



void analog_dim_fade_stop(void)
{
  dac_fade.fade_start = 0;
}

void analog_dim_output( rs485_dac_dim_t * dac_body )
{

  uint8_t iic_buff[2] = {0};
#if PROJECT_TYPE==307

  iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body->cw);
  iic_buff[1] = GET_16BIT_LOW8BIT(dac_body->cw);
  hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,(uint8_t)IIC_WW1_ADDRESS,iic_buff,2);
  hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,(uint8_t)IIC_WW2_ADDRESS,iic_buff,2);

  iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body->ww);
  iic_buff[1] = GET_16BIT_LOW8BIT(dac_body->ww);
  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_CW1_ADDRESS,iic_buff,2);
  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_CW2_ADDRESS,iic_buff,2);

  iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body->b);
  iic_buff[1] = GET_16BIT_LOW8BIT(dac_body->b);
  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_G_ADDRESS,iic_buff,2);

  iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body->g);
  iic_buff[1] = GET_16BIT_LOW8BIT(dac_body->g);
  hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,(uint8_t)IIC_B_ADDRESS,iic_buff,2);

  iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body->r);
  iic_buff[1] = GET_16BIT_LOW8BIT(dac_body->r);
  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_R_ADDRESS,iic_buff,2);
#endif
#if PROJECT_TYPE==308
#if USE_NEW_DIM == 1

  iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body->cw);
  iic_buff[1] = GET_16BIT_LOW8BIT(dac_body->cw);
  hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,(uint8_t)IIC_WW1_ADDRESS,iic_buff,2);
  hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,(uint8_t)IIC_B_ADDRESS,iic_buff,2);

  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_CW2_ADDRESS,iic_buff,2);

  iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body->ww);
  iic_buff[1] = GET_16BIT_LOW8BIT(dac_body->ww);
  hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,(uint8_t)IIC_WW2_ADDRESS,iic_buff,2);
  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_R_ADDRESS,iic_buff,2);
  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_G_ADDRESS,iic_buff,2);

  iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body->g);
  iic_buff[1] = GET_16BIT_LOW8BIT(dac_body->g);
  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_CW1_ADDRESS,iic_buff,2);
#endif
#if USE_NEW_DIM == 0

  iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body->g);
  iic_buff[1] = GET_16BIT_LOW8BIT(dac_body->g);
  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_R_ADDRESS,iic_buff,2);

  iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body->cw);
  iic_buff[1] = GET_16BIT_LOW8BIT(dac_body->cw);
  hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,(uint8_t)IIC_WW1_ADDRESS,iic_buff,2);
  hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,(uint8_t)IIC_B_ADDRESS,iic_buff,2);
  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_CW2_ADDRESS,iic_buff,2);

  iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body->ww);
  iic_buff[1] = GET_16BIT_LOW8BIT(dac_body->ww);
  hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,(uint8_t)IIC_WW2_ADDRESS,iic_buff,2);
  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_G_ADDRESS,iic_buff,2);
  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_CW1_ADDRESS,iic_buff,2);
#endif
#endif

}


static uint8_t light_linear_deal(rs485_dac_dim_t dac_start,  rs485_dac_dim_t dac_end,
                                 rs485_dac_dim_t *dac_current, uint32_t *run_time, float whole_time)
{
  float ratio_r;
  float ratio_g;
  float ratio_b;
  float ratio_ww;
  float ratio_cw;

  ratio_r = (dac_end.r - dac_start.r) / whole_time;
  ratio_g = (dac_end.g - dac_start.g) / whole_time;
  ratio_b = (dac_end.b - dac_start.b) / whole_time;
  ratio_ww = (dac_end.ww - dac_start.ww) / whole_time;
  ratio_cw = (dac_end.cw - dac_start.cw) / whole_time;


  if (*run_time >= whole_time)
    {
      *run_time = whole_time;
    }
  else
    (*run_time)+=2;

  dac_current->r = dac_start.r + (ratio_r * (*run_time) + 0.5f);
  dac_current->g = dac_start.g + (ratio_g * (*run_time) + 0.5f);
  dac_current->b = dac_start.b + (ratio_b * (*run_time) + 0.5f);
  dac_current->ww = dac_start.ww + (ratio_ww * (*run_time) + 0.5f);
  dac_current->cw = dac_start.cw + (ratio_cw * (*run_time) + 0.5f);

  if (*run_time == whole_time)
    {
      return 1;
    }

  return 0;
}


void rs485_dac_fade_handle(void)
{
	struct sys_info_power info_power;
	if(dac_fade.fade_start == 1 && dac_fade.run_time < (RS485_ANALOG_FADE_TIME/2) && dev_lamp.Connected == 1)
    {
		if(dac_fade.run_time == 0)
        {
          if(dac_fade.analog_dac_current.r == 0 && dac_fade.analog_dac_end.r!=0) dac_fade.analog_dac_current.r = dac_fade.analog_dac_init.r;
          if(dac_fade.analog_dac_current.g == 0 && dac_fade.analog_dac_end.g!=0) dac_fade.analog_dac_current.g = dac_fade.analog_dac_init.g;
          if(dac_fade.analog_dac_current.b == 0 && dac_fade.analog_dac_end.b!=0) dac_fade.analog_dac_current.b = dac_fade.analog_dac_init.b;
          if(dac_fade.analog_dac_current.ww == 0 && dac_fade.analog_dac_end.ww!=0) dac_fade.analog_dac_current.ww = dac_fade.analog_dac_init.ww;
          if(dac_fade.analog_dac_current.cw == 0 && dac_fade.analog_dac_end.cw!=0) dac_fade.analog_dac_current.cw = dac_fade.analog_dac_init.cw;
          memcpy(&dac_fade.analog_dac_start, &dac_fade.analog_dac_current, sizeof(dac_fade.analog_dac_current));
        }
		light_linear_deal(dac_fade.analog_dac_start, dac_fade.analog_dac_end, &dac_fade.analog_dac_current, &dac_fade.run_time, (RS485_ANALOG_FADE_TIME/2));
#if PROJECT_TYPE == 307
      if((dac_fade.analog_dac_current.g != 0) &&  ( dac_fade.analog_dac_current.g >= dac_fade.analog_dac_init.g))
        {
          dev_analog_green_ctrl(true);
        }
      else
        {
          dev_analog_green_ctrl(false);
        }
#endif
		data_center_read_sys_info(SYS_INFO_POWER, &info_power);
		if(info_power.indoor_powe >= 1)
		{
			if(info_power.ratio == 0)
			{
				memset(&dac_fade.analog_dac_current, 0, sizeof(dac_fade.analog_dac_current));
				analog_dim_output(&dac_fade.analog_dac_current);
			}
			else
			{
				analog_dim_output(&dac_fade.analog_dac_current);
			}
		}
		else
		{
			analog_dim_output(&dac_fade.analog_dac_current);
		}
    }
}


void analog_dim_output_test( rs485_dac_dim_t * dac_body )
{

  uint8_t iic_buff[2] = {0};
#if PROJECT_TYPE==307

  iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body->cw);
  iic_buff[1] = GET_16BIT_LOW8BIT(dac_body->cw);
  hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,(uint8_t)IIC_WW1_ADDRESS,iic_buff,2);
  hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,(uint8_t)IIC_WW2_ADDRESS,iic_buff,2);
  delay_ms(10);
  iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body->ww);
  iic_buff[1] = GET_16BIT_LOW8BIT(dac_body->ww);
  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_CW1_ADDRESS,iic_buff,2);
  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_CW2_ADDRESS,iic_buff,2);
  delay_ms(10);
  iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body->b);
  iic_buff[1] = GET_16BIT_LOW8BIT(dac_body->b);
  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_G_ADDRESS,iic_buff,2);
  delay_ms(10);
  iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body->g);
  iic_buff[1] = GET_16BIT_LOW8BIT(dac_body->g);
  hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,(uint8_t)IIC_B_ADDRESS,iic_buff,2);
  delay_ms(10);
  iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body->r);
  iic_buff[1] = GET_16BIT_LOW8BIT(dac_body->r);
  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_R_ADDRESS,iic_buff,2);
  delay_ms(10);
#endif
#if PROJECT_TYPE==308
#if USE_NEW_DIM == 1
//???????????
  iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body->g);
  iic_buff[1] = GET_16BIT_LOW8BIT(dac_body->g);
  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_CW1_ADDRESS,iic_buff,2);

  iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body->cw);
  iic_buff[1] = GET_16BIT_LOW8BIT(dac_body->cw);
  hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,(uint8_t)IIC_WW1_ADDRESS,iic_buff,2);
  hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,(uint8_t)IIC_B_ADDRESS,iic_buff,2);
  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_CW2_ADDRESS,iic_buff,2);

  iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body->ww);
  iic_buff[1] = GET_16BIT_LOW8BIT(dac_body->ww);
  hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,(uint8_t)IIC_WW2_ADDRESS,iic_buff,2);
  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_R_ADDRESS,iic_buff,2);
  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_G_ADDRESS,iic_buff,2);
#endif
#if USE_NEW_DIM == 0
//???????????
  iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body->g);
  iic_buff[1] = GET_16BIT_LOW8BIT(dac_body->g);
  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_R_ADDRESS,iic_buff,2);

  iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body->cw);
  iic_buff[1] = GET_16BIT_LOW8BIT(dac_body->cw);
  hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,(uint8_t)IIC_WW1_ADDRESS,iic_buff,2);
  hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,(uint8_t)IIC_B_ADDRESS,iic_buff,2);
  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_CW2_ADDRESS,iic_buff,2);

  iic_buff[0] = GET_16BIT_HIGH8BIT(dac_body->ww);
  iic_buff[1] = GET_16BIT_LOW8BIT(dac_body->ww);
  hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,(uint8_t)IIC_WW2_ADDRESS,iic_buff,2);
  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_G_ADDRESS,iic_buff,2);
  hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,(uint8_t)IIC_CW1_ADDRESS,iic_buff,2);
#endif
#endif

}

void light_dim_test_funtion(void)
{
  static uint8_t test_admi_flag = 0;
  uint8_t analog_value111[2] = {0x03,0XE8};
  uint8_t analog_value122[2] = {0x00,0X00};
  if(test_admi_flag == 0)
    {

    }
  else if(test_admi_flag == 1)
    {
      hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_WW1_ADDRESS,(uint8_t* )analog_value111,2);
      hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_WW2_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_B_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_CW1_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_CW2_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_R_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_G_ADDRESS,(uint8_t* )analog_value122,2);
    }
  else if(test_admi_flag == 2)
    {
      hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_WW1_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_WW2_ADDRESS,(uint8_t* )analog_value111,2);
      hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_B_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_CW1_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_CW2_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_R_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_G_ADDRESS,(uint8_t* )analog_value122,2);
    }
  else if(test_admi_flag == 3)
    {
      hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_WW1_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_WW2_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_B_ADDRESS,(uint8_t* )analog_value111,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_CW1_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_CW2_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_R_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_G_ADDRESS,(uint8_t* )analog_value122,2);
    }
  else if(test_admi_flag == 4)
    {
      hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_WW1_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_WW2_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_B_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_CW1_ADDRESS,(uint8_t* )analog_value111,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_CW2_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_R_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_G_ADDRESS,(uint8_t* )analog_value122,2);
    }
  else if(test_admi_flag == 5)
    {
      hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_WW1_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_WW2_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_B_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_CW1_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_CW2_ADDRESS,(uint8_t* )analog_value111,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_R_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_G_ADDRESS,(uint8_t* )analog_value122,2);
    }
  else if(test_admi_flag == 6)
    {
      hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_WW1_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_WW2_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_B_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_CW1_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_CW2_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_R_ADDRESS,(uint8_t* )analog_value111,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_G_ADDRESS,(uint8_t* )analog_value122,2);
    }
  else if(test_admi_flag == 7)
    {
      hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_WW1_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_WW2_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_WW1_WW2_B,IIC_B_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_CW1_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_CW2_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_R_ADDRESS,(uint8_t* )analog_value122,2);
      hal_iic_send_data(HAL_IIC_ANALOG_CW1_CW2_R_G,IIC_G_ADDRESS,(uint8_t* )analog_value111,2);
    }
//	if(dev_lamp.Connected == 1 )
//	{
//		if(scount_test++ > flag_ms)
//		{
//			scount_test = 0;
//			if(flag_test) //testing
//			{
//				flag_test = 0;
//				light_boby.cct_arg.cct.cct  = 4700;
//				light_boby.cct_arg.cct.duv  = 0;
//				light_boby.cct_arg.lightness  = 1000;
//			}
//			else
//			{
//				flag_test = 1;
//				light_boby.cct_arg.cct.cct  = 1000;
//				light_boby.cct_arg.cct.duv  = 0;
//				light_boby.cct_arg.lightness  = 0;
//			}
//		}
//		package_485_light_data(&cmd_body_1, date_center_get_light_mode(), &light_boby);
//		dev_lamp.Connected = rs485_light_control(&dev_lamp, true, 5, 100, &cmd_body_1);
//	}
}








