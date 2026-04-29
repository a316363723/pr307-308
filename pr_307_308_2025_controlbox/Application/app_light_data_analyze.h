#ifndef APP_LIGHT_DATA_ANALYZE_H
#define APP_LIGHT_DATA_ANALYZE_H
#include "cmsis_os.h"
#include "project_def.h"
#include "rs485_protocol.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "os_event.h"
#include "local_data.h"
#include "api_rs485_proto.h"
typedef union 
{
    struct db_cct       cct_arg;
    struct db_hsi       hsi_arg;
    struct db_gel       gel_arg;
    struct db_rgb		rgb_arg;
	struct db_rgbww		rgbww_arg;
    struct db_source    source_arg;
    struct db_xy        xy_arg;
    struct db_fx_club_lights           club_lights;
    struct db_fx_paparazzi             paparazzi;
    struct db_fx_lightning             lightning;
    struct db_fx_tv                    tv;
    struct db_fx_candle                candle;
    struct db_fx_fire                  fire;
    struct db_fx_strobe                strobe;
    struct db_fx_explosion             explosion;
    struct db_fx_fault_bulb            fault_bulb;
    struct db_fx_pulsing               pulsing;
    struct db_fx_welding               welding;
    struct db_fx_cop_car               cop_car;
    struct db_fx_color_chase           color_chase;
    struct db_fx_party_lights          party_lights;
    struct db_fx_fireworks             fireworks;
    struct db_fx_paparazzi_2           paparazzi_2;
    struct db_fx_lightning_2           lightning_2;
    struct db_fx_tv_2                  tv_2;
    struct db_fx_fire_2                fire_2;
    struct db_fx_strobe_2              strobe_2;
    struct db_fx_explosion_2           explosion_2;
    struct db_fx_fault_bulb_2          fault_bulb_2;
    struct db_fx_pulsing_2             pulsing_2;
    struct db_fx_welding_2             welding_2;
    struct db_fx_cop_car_2             cop_car_2;
    struct db_fx_party_lights_2        party_lights_2;
    struct db_fx_fireworks_2           fireworks_2;
    struct db_fx_lightning_3           lightning_3;
    struct db_fx_tv_3                  tv_3;
    struct db_fx_fire_3                fire_3;
    struct db_fx_fault_bulb_3          fault_bulb_3;
    struct db_fx_pulsing_3             pulsing_3;
    struct db_fx_cop_car_3             cop_car_3;
    struct db_manual_fx                  mfx;
    struct db_program_fx                  pfx;
    struct db_custom_fx                  cfx;
    struct db_custom_preview_fx          cfx_preview;
    struct db_local_custom_fx            local_cfx_arg;
    struct db_color_fade          color_fade;
    struct db_color_cycle         color_cycle;
    struct db_rainbow              rainbow;
    struct db_one_color_chase      one_color_chase;
    struct db_two_color_chase      two_color_chase;
    struct db_three_color_chase    three_color_chase;
    struct db_pixel_fire          pixel_fire;
    struct db_dmx                 dmx;
}light_boby_t;


extern void package_485_light_data(rs485_cmd_body_t *p_cmd_body, uint8_t mode, light_boby_t * arg );
#endif

