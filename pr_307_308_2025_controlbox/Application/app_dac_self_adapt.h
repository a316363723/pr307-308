#ifndef APP_RS485_SELF_ADAPT_H
#define APP_RS485_SELF_ADAPT_H
#include "cmsis_os.h"
#include "project_def.h"
#include "api_rs485_proto.h"
#define DEV_RS485_VA_ARG_END    (NULL)
#define GET_16BIT_HIGH8BIT(a)    ((a >> 8) & 0xff)
#define GET_16BIT_LOW8BIT(a)    (a & 0xff)

typedef struct
{
    uint16_t  r;
	uint16_t  g;
    uint16_t  b;
    uint16_t  ww;
	uint16_t  cw;    
}rs485_dac_dim_t;   


struct dac_change
{
	rs485_dac_dim_t analog_dac_init;
    rs485_dac_dim_t analog_dac_start;
    rs485_dac_dim_t analog_dac_current;
    rs485_dac_dim_t analog_dac_end;
    uint32_t run_time;
    uint8_t  fade_start;
};

extern struct dac_change dac_fade;
extern void analog_dim_output( rs485_dac_dim_t * dac_body );
extern void rs485_dac_fade_handle(void);
extern void analog_dim_output_test( rs485_dac_dim_t * dac_body );
extern void light_dim_test_funtion(void);
extern void analog_dim_fade_stop(void);
extern void analog_dim_fade_restart(void);
#endif

