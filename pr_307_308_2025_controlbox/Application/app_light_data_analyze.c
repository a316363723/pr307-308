#include "app_light_data_analyze.h"
#include "app_data_center.h"
#include "base_type.h"
#include "ui_data.h"

#define RS485_FADE_TIME      (350)
static uint8_t mdx_loss_start = 0;



uint8_t get_one_effect_mode_back( uint8_t effect_type)
{
	uint8_t eid = 0;
	switch(effect_type)
	{
		case PARAM_SETTING_CCT:
			eid =  RS485_FX_Mode_CCT;
			break;
		case PARAM_SETTING_HSI:
			eid =  RS485_FX_Mode_HSI;
			break;
		case PARAM_SETTING_GEL:	
			eid =  RS485_FX_Mode_GEL ;
			break;
		case PARAM_SETTING_SOURCE:	
			eid =  RS485_FX_Mode_Source;
			break;
		case PARAM_SETTING_XY:	
			eid =  RS485_FX_Mode_XY;
			break;
		case PARAM_SETTING_RGB:	
			eid =  RS485_FX_Mode_Null;
			break;
		default :
			eid  = RS485_FX_Mode_Null;
			break;
	}
	return eid ;
}

uint8_t get_two_effect_mode_back( uint8_t effect_type)
{
	uint8_t eid = 0;
	switch(effect_type)
	{
		case PARAM_SETTING_CCT:
			eid =  RS485_FX_II_Mode_CCT;
			break;
		case PARAM_SETTING_HSI:
			eid =  RS485_FX_II_Mode_HSI;
			break;
		case PARAM_SETTING_GEL:	
			eid =  RS485_FX_II_Mode_GEL ;
			break;
		case PARAM_SETTING_SOURCE:	
			eid =  RS485_FX_II_Mode_SOUYRCE;
			break;
		case PARAM_SETTING_XY:	
			eid =  RS485_FX_II_Mode_XY;
			break;
		case PARAM_SETTING_RGB:	
			eid =  RS485_FX_II_Mode_RGB;
			break;
		default :
			eid  = RS485_FX_II_Mode_Null;
			break;
	}
	return eid ;
}

void package_485_light_data(rs485_cmd_body_t *p_cmd_body, uint8_t mode, light_boby_t * arg )
{
    uint8_t Fx_state;
    uint16_t gel_cct[2] = {3200, 5600};
	uint8_t series;
	uint8_t brand;
	static	uint8_t  explosion_strobe_or_fx_state = 0;
	static	uint8_t  lightning_strobe_or_fx_state = 0;
    switch(mode)
    {
        case LIGHT_MODE_CCT:
            // cct_analysis(&p_cmd_body->cmd_arg.cct, &arg->cct_arg);
            p_cmd_body->header.cmd_type = RS485_Cmd_CCT;
            p_cmd_body->cmd_arg.cct.cct = arg->cct_arg.cct.cct;
//			data_center_read_config_data(SYS_CONFIG_HIGH_SPEED_MODE, &light_hs_mode);
//			if(light_hs_mode == 1)
//				arg->cct_arg.lightness = arg->cct_arg.lightness < 500 ? 500 : arg->cct_arg.lightness;
            p_cmd_body->cmd_arg.cct.intensity = arg->cct_arg.lightness / 1000.0f;
		#if PROJECT_TYPE==307
            p_cmd_body->cmd_arg.cct.duv =  arg->cct_arg.cct.duv * 0.0002;
		#elif PROJECT_TYPE==308
//			p_cmd_body->cmd_arg.cct.duv =  arg->cct_arg.cct.duv * 0.0001;
			if(arg->cct_arg.cct.duv <= 0)
				p_cmd_body->cmd_arg.cct.duv =  arg->cct_arg.cct.duv * 0.0001;
			else
				p_cmd_body->cmd_arg.cct.duv =  arg->cct_arg.cct.duv * 0.00013;
		#endif
            p_cmd_body->cmd_arg.cct.fade = RS485_FADE_TIME;
            break;
        case LIGHT_MODE_HSI:
            p_cmd_body->header.cmd_type = RS485_Cmd_HSI;
            p_cmd_body->cmd_arg.hsi.hue = arg->hsi_arg.hsi.hue * 10;
            p_cmd_body->cmd_arg.hsi.intensity = arg->hsi_arg.lightness / 1000.0f;
            p_cmd_body->cmd_arg.hsi.sat = arg->hsi_arg.hsi.sat * 10;
            p_cmd_body->cmd_arg.hsi.cct = arg->hsi_arg.hsi.cct;
			p_cmd_body->cmd_arg.hsi.duv = 0;
            p_cmd_body->cmd_arg.hsi.fade = RS485_FADE_TIME;
            break;
        case LIGHT_MODE_GEL:
            p_cmd_body->header.cmd_type = RS485_Cmd_GEL;
            p_cmd_body->cmd_arg.gel.intensity = arg->gel_arg.lightness / 1000.0f;

            p_cmd_body->cmd_arg.gel.cct = arg->gel_arg.gel.cct;

            p_cmd_body->cmd_arg.gel.origin = arg->gel_arg.gel.brand == 0 ? 1 : 0 ;
            p_cmd_body->cmd_arg.gel.series = arg->gel_arg.gel.type[arg->gel_arg.gel.brand];
            p_cmd_body->cmd_arg.gel.color = arg->gel_arg.gel.color[arg->gel_arg.gel.brand][p_cmd_body->cmd_arg.gel.series];
            p_cmd_body->cmd_arg.gel.fade = RS485_FADE_TIME;
            break;
        case LIGHT_MODE_XY:
            p_cmd_body->header.cmd_type = RS485_Cmd_XY_Coordinate;
            p_cmd_body->cmd_arg.xy_coord.intensity  = arg->xy_arg.lightness / 1000.0f;
            p_cmd_body->cmd_arg.xy_coord.x  = arg->xy_arg.xy.x;
            p_cmd_body->cmd_arg.xy_coord.y  = arg->xy_arg.xy.y;
            p_cmd_body->cmd_arg.xy_coord.fade = RS485_FADE_TIME;
            break;
        case LIGHT_MODE_RGB:
            p_cmd_body->header.cmd_type = RS485_Cmd_RGB;
            p_cmd_body->cmd_arg.rgb.intensity = arg->rgb_arg.lightness / 1000.0f;
            p_cmd_body->cmd_arg.rgb.red_ratio     = arg->rgb_arg.rgb.r;
            p_cmd_body->cmd_arg.rgb.green_ratio  = arg->rgb_arg.rgb.g;
            p_cmd_body->cmd_arg.rgb.blue_ratio   = arg->rgb_arg.rgb.b;
            p_cmd_body->cmd_arg.rgb.fade  =  RS485_FADE_TIME;
            break;
		case LIGHT_MODE_RGBWW:
            p_cmd_body->header.cmd_type = RS485_Cmd_RGBWW;
            p_cmd_body->cmd_arg.rgbww.intensity = arg->rgbww_arg.lightness / 1000.0f;
            p_cmd_body->cmd_arg.rgbww.red_ratio     = arg->rgbww_arg.rgbww.red_ratio;
            p_cmd_body->cmd_arg.rgbww.green_ratio  = arg->rgbww_arg.rgbww.green_ratio;
            p_cmd_body->cmd_arg.rgbww.blue_ratio   = arg->rgbww_arg.rgbww.blue_ratio;
			p_cmd_body->cmd_arg.rgbww.cw_ratio   = arg->rgbww_arg.rgbww.cw_ratio;
			p_cmd_body->cmd_arg.rgbww.ww_ratio   = arg->rgbww_arg.rgbww.ww_ratio;
            p_cmd_body->cmd_arg.rgbww.fade  =  RS485_FADE_TIME;
            break;
        case LIGHT_MODE_SOURCE:
            p_cmd_body->header.cmd_type = RS485_Cmd_Source;
            p_cmd_body->cmd_arg.source.intensity   = arg->source_arg.lightness / 1000.0f;
            p_cmd_body->cmd_arg.source.type  =  arg->source_arg.source.type;
            p_cmd_body->cmd_arg.source.x_coord  = arg->source_arg.source.x;
            p_cmd_body->cmd_arg.source.y_coord  = arg->source_arg.source.y;
            p_cmd_body->cmd_arg.source.fade   = RS485_FADE_TIME;
            break;
        case LIGHT_MODE_FX_CLUBLIGHTS:
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_ClubLights;

            if(true == arg->club_lights.state)
            {
                p_cmd_body->cmd_arg.sys_fx.arg.club_lights.intensity = arg->club_lights.lightness;
                p_cmd_body->cmd_arg.sys_fx.arg.club_lights.color = arg->club_lights.color;
                p_cmd_body->cmd_arg.sys_fx.arg.club_lights.frq   = arg->club_lights.spd;
            }
            else
            {
                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
            }
            break;
        case LIGHT_MODE_FX_PAPARAZZI:
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Paparazzi;

            if(true == arg->paparazzi.state)
            {
                p_cmd_body->cmd_arg.sys_fx.arg.paparazzi.intensity = arg->paparazzi.lightness;
                p_cmd_body->cmd_arg.sys_fx.arg.paparazzi.frq = arg->paparazzi.frq;
                p_cmd_body->cmd_arg.sys_fx.arg.paparazzi.gm =  arg->paparazzi.gm;
                p_cmd_body->cmd_arg.sys_fx.arg.paparazzi.cct =  arg->paparazzi.cct;
            }
            else
            {
                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
            }
            break;
        case LIGHT_MODE_FX_LIGHTNING:
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Lightning;

            if(arg->lightning.trigger == 2 )
            {
                Fx_state =RS485_FX_Trigger_Continue;
                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Lightning;
            }
            else if(arg->lightning.trigger == 1)
            {
                Fx_state = RS485_FX_Trigger_Once;
                arg->lightning.trigger = 0;
                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Lightning;
            }
            else
            {
                Fx_state = RS485_FX_Trigger_None;
                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
            }

            p_cmd_body->cmd_arg.sys_fx.arg.lightning.intensity = arg->lightning.lightness;
            p_cmd_body->cmd_arg.sys_fx.arg.lightning.frq = arg->lightning.frq;
            p_cmd_body->cmd_arg.sys_fx.arg.lightning.gm =  arg->lightning.gm;
            p_cmd_body->cmd_arg.sys_fx.arg.lightning.cct =  arg->lightning.cct;
            p_cmd_body->cmd_arg.sys_fx.arg.lightning.spd = arg->lightning.speed;
            p_cmd_body->cmd_arg.sys_fx.arg.lightning.trigger  = (rs485_fx_trigger_enum)Fx_state;
            break;
        case LIGHT_MODE_FX_LIGHTNING_II :
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX_II;

            if(arg->lightning_2.state == 1 )
            {
                Fx_state = 2;
            }
            else if(arg->lightning_2.trigger == 1)
            {
                Fx_state = 1;
                arg->lightning_2.trigger = 0;
				if(get_lfm_port_mode() == 0)
				ui_set_lightning_2_trigger(0);
            }
            else
            {
                Fx_state = 0;
            }

            p_cmd_body->cmd_arg.sys_fx_2.type = RS485_FX_II_Lightning_2;
            p_cmd_body->cmd_arg.sys_fx_2.arg.lightning_2.intensity = arg->lightning_2.lightness;
            p_cmd_body->cmd_arg.sys_fx_2.arg.lightning_2.frq = arg->lightning_2.frq;
            p_cmd_body->cmd_arg.sys_fx_2.arg.lightning_2.spd = arg->lightning_2.spd;
            p_cmd_body->cmd_arg.sys_fx_2.arg.lightning_2.state  = (rs485_fx_2_trigger_enum)Fx_state;

            switch((uint8_t)arg->lightning_2.mode)
            {
                case FX_MODE_CCT:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.lightning_2.mode = RS485_FX_II_Mode_CCT;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.lightning_2.mode_arg.cct.cct = arg->lightning_2.mode_arg.cct.cct;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.lightning_2.mode_arg.cct.duv  = (uint8_t)arg->lightning_2.mode_arg.cct.duv ;
                    break;
                case FX_MODE_HSI:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.lightning_2.mode = RS485_FX_II_Mode_HSI;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.lightning_2.mode_arg.hsi.cct =  arg->lightning_2.mode_arg.hsi.cct ;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.lightning_2.mode_arg.hsi.hue  = arg->lightning_2.mode_arg.hsi.hue ;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.lightning_2.mode_arg.hsi.sat = arg->lightning_2.mode_arg.hsi.sat ;
                    break;
            }
            break;
        case LIGHT_MODE_FX_TV:
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_TV;

            if(true == arg->tv.state)
            {
                p_cmd_body->cmd_arg.sys_fx.arg.tv.intensity = arg->tv.lightness;
                p_cmd_body->cmd_arg.sys_fx.arg.tv.frq = arg->tv.frq;
                p_cmd_body->cmd_arg.sys_fx.arg.tv.cct =  arg->tv.cct;
            }
            else
            {
                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
            }
            break;
        case LIGHT_MODE_FX_TV_II :
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX_II;
            p_cmd_body->cmd_arg.sys_fx_2.type = RS485_FX_II_TV_2;
            p_cmd_body->cmd_arg.sys_fx_2.arg.tv_2.intensity = arg->tv_2.lightness;
            p_cmd_body->cmd_arg.sys_fx_2.arg.tv_2.spd = arg->tv_2.spd;
            p_cmd_body->cmd_arg.sys_fx_2.arg.tv_2.state = (rs485_fx_2_trigger_enum)(( arg->tv_2.state == 0) ? 0 : 2);

            switch((uint8_t)arg->tv_2.mode)
            {
                case FX_MODE_CCT:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.tv_2.mode = RS485_FX_II_Mode_CCT_Range;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.tv_2.mode_arg.cct_range.cct_lower = arg->tv_2.mode_arg.cct.min_cct;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.tv_2.mode_arg.cct_range.cct_upper = arg->tv_2.mode_arg.cct.max_cct ;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.tv_2.mode_arg.cct_range.duv  = (uint8_t) arg->tv_2.mode_arg.cct.duv ;
                    break;
                case FX_MODE_HSI:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.tv_2.mode  =  RS485_FX_II_Mode_HSI_Range;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.tv_2.mode_arg.hsi_range.cct = arg->tv_2.mode_arg.hsi.cct ;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.tv_2.mode_arg.hsi_range.sat =  arg->tv_2.mode_arg.hsi.sat ;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.tv_2.mode_arg.hsi_range.hue_lower =  arg->tv_2.mode_arg.hsi.min_hue;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.tv_2.mode_arg.hsi_range.hue_upper = arg->tv_2.mode_arg.hsi.max_hue;
                    break;
            }
            break;

        case LIGHT_MODE_FX_CANDLE:
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Candle;

            if(true == arg->candle.state)
            {
                p_cmd_body->cmd_arg.sys_fx.arg.candle.intensity = arg->candle.lightness;
                p_cmd_body->cmd_arg.sys_fx.arg.candle.frq = arg->candle.spd;
                p_cmd_body->cmd_arg.sys_fx.arg.candle.cct =  arg->candle.cct;
            }
            else
            {
                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
            }
            break;
        case LIGHT_MODE_FX_FIRE:
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Fire;

            if(true == arg->fire.state)
            {
                p_cmd_body->cmd_arg.sys_fx.arg.fire.intensity = arg->fire.lightness;
                p_cmd_body->cmd_arg.sys_fx.arg.fire.frq = arg->fire.frq;
                p_cmd_body->cmd_arg.sys_fx.arg.fire.cct =  arg->fire.cct;
            }
            else
            {
                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
            }
            break;
        case LIGHT_MODE_FX_FIRE_II :
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX_II;
            p_cmd_body->cmd_arg.sys_fx_2.type = RS485_FX_II_Fire_2;
            p_cmd_body->cmd_arg.sys_fx_2.arg.fire_2.intensity = arg->fire_2.lightness;
            p_cmd_body->cmd_arg.sys_fx_2.arg.fire_2.spd = arg->fire_2.spd;
            p_cmd_body->cmd_arg.sys_fx_2.arg.fire_2.state = (arg->fire_2.state == 0 ) ? RS485_FX_II_Trigger_None : RS485_FX_II_Trigger_Continue;

            switch((uint8_t)arg->fire_2.mode)
            {
                case FX_MODE_CCT:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.fire_2.mode = RS485_FX_II_Mode_CCT_Range;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.fire_2.mode_arg.cct_range.cct_lower = arg->fire_2.mode_arg.cct.min_cct;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.fire_2.mode_arg.cct_range.cct_upper = arg->fire_2.mode_arg.cct.max_cct ;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.fire_2.mode_arg.cct_range.duv  = (uint8_t) arg->fire_2.mode_arg.cct.duv ;
                    break;
                case FX_MODE_HSI:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.fire_2.mode  =  RS485_FX_II_Mode_HSI_Range;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.fire_2.mode_arg.hsi_range.cct = arg->fire_2.mode_arg.hsi.cct ;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.fire_2.mode_arg.hsi_range.sat =  arg->fire_2.mode_arg.hsi.sat ;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.fire_2.mode_arg.hsi_range.hue_lower =  arg->fire_2.mode_arg.hsi.min_hue;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.fire_2.mode_arg.hsi_range.hue_upper = arg->fire_2.mode_arg.hsi.max_hue;
                    break;
            }
            break;
        case LIGHT_MODE_FX_STROBE:
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Strobe;

            if(true == arg->strobe.state)
            {
                p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode = get_one_effect_mode_back((rs485_fx_mode_enum)arg->strobe.mode);
                p_cmd_body->cmd_arg.sys_fx.arg.strobe.frq  = arg->strobe.frq;

                switch(p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode)
                {
                    case RS485_FX_Mode_CCT:
					
                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.cct.intensity = arg->strobe.lightness  ; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_CCT].CCT.intensity;
                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.cct.cct = arg->strobe.mode_arg.cct.cct;
                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.cct.gm  = (uint8_t)arg->strobe.mode_arg.cct.duv ;
					
                        break;
                    case RS485_FX_Mode_HSI:
                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.hsi.intensity  = arg->strobe.lightness ;//g_tUIAllData.strobe_model.setting[PARAM_SETTING_HSI].HSI.hsi.intensity;
                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.hsi.hue = arg->strobe.mode_arg.hsi.hue;
                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.hsi.sat = arg->strobe.mode_arg.hsi.sat;
                        break;
                    case RS485_FX_Mode_XY:
                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.xy.intensity = arg->strobe.lightness ; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_XY].XY.intensity;
                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.xy.x  = arg->strobe.mode_arg.xy.x;
                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.xy.y = arg->strobe.mode_arg.xy.y;
                        break;
                    case RS485_FX_Mode_GEL:
					{
						p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.gel.intensity = arg->strobe.lightness ;
						p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.gel.cct = arg->strobe.mode_arg.gel.cct;
						brand  = arg->strobe.mode_arg.gel.brand;
						p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.gel.origin  = brand == 0 ? 1 : 0;
						series     = arg->strobe.mode_arg.gel.type[brand];
						p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.gel.series  = series;
						p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.gel.color   = arg->strobe.mode_arg.gel.color[brand][series];
						break;
					}
                    case RS485_FX_Mode_Source:
                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.source.intensity = arg->strobe.lightness ; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_SOURCE].SOURCE.intensity ;
                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.source.type = arg->strobe.mode_arg.source.type;
                        break;
                    default:break;
                }
            }
            else
            {
                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
            }
            break;
        case LIGHT_MODE_FX_STROBE_II :
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX_II;
            p_cmd_body->cmd_arg.sys_fx_2.type = RS485_FX_II_Strobe_2;
            p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.intensity = arg->strobe_2.lightness;
            p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode = get_two_effect_mode_back((rs485_fx_2_mode_enum)arg->strobe_2.mode);
            p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.spd  = arg->strobe_2.spd;
            p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.state =  (arg->strobe_2.state == 0 ) ? RS485_FX_II_Trigger_None : RS485_FX_II_Trigger_Continue;;

            switch(p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode)
            {
                case RS485_FX_II_Mode_CCT:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.cct.cct = arg->strobe_2.mode_arg.cct.cct;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.cct.duv  = (uint8_t)arg->strobe_2.mode_arg.cct.duv ;
                    break;
                case RS485_FX_II_Mode_HSI:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.hsi.hue = arg->strobe_2.mode_arg.hsi.hue;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.hsi.sat = arg->strobe_2.mode_arg.hsi.sat;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.hsi.cct = arg->strobe_2.mode_arg.hsi.cct;
                    break;
                case RS485_FX_II_Mode_XY:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.xy.x = arg->strobe_2.mode_arg.xy.x;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.xy.y = arg->strobe_2.mode_arg.xy.y;
                    break;
                case RS485_FX_II_Mode_GEL:
                     {
						p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.gel.cct = arg->strobe_2.mode_arg.gel.cct;
						brand  = arg->strobe_2.mode_arg.gel.brand;
						p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.gel.origin  = brand == 0 ? 1 : 0;
						series     = arg->strobe_2.mode_arg.gel.type[brand];
						p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.gel.series  = series;
						p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.gel.color   = arg->strobe_2.mode_arg.gel.color[brand][series];
						break;
					}
                case RS485_FX_II_Mode_SOUYRCE:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.source.type = arg->strobe_2.mode_arg.source.type;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.source.x_coord = arg->strobe_2.mode_arg.source.x;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.source.y_coord = arg->strobe_2.mode_arg.source.y;
                    break;
                case RS485_FX_II_Mode_RGB:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.rgb.blue_ratio = arg->strobe_2.mode_arg.rgb.b;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.rgb.green_ratio = arg->strobe_2.mode_arg.rgb.g;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.rgb.red_ratio = arg->strobe_2.mode_arg.rgb.r;;
                    break;
                default:break;
            }
            break;
        case LIGHT_MODE_FX_EXPLOSION:
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Explosion;

            if(true == arg->explosion.state)
            {
                p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode = get_one_effect_mode_back(((rs485_fx_mode_enum)arg->explosion.mode));
                p_cmd_body->cmd_arg.sys_fx.arg.explosion.frq  = arg->explosion.frq;

                if(arg->explosion.trigger >= 1)
                {
                    p_cmd_body->cmd_arg.sys_fx.arg.explosion.trigger = RS485_FX_Trigger_Once;
                    arg->explosion.trigger = 0;
                }
                else
                {
                    p_cmd_body->cmd_arg.sys_fx.arg.explosion.trigger = RS485_FX_Trigger_None;
                }

                switch(p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode)
                {
                    case RS485_FX_Mode_CCT:
                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.cct.intensity = arg->explosion.lightness  ; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_CCT].CCT.intensity;
                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.cct.cct = arg->explosion.mode_arg.cct.cct;
                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.cct.gm  = (uint8_t)arg->explosion.mode_arg.cct.duv ;
                        break;
                    case RS485_FX_Mode_HSI:
                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.hsi.intensity  = arg->explosion.lightness ;//g_tUIAllData.strobe_model.setting[PARAM_SETTING_HSI].HSI.hsi.intensity;
                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.hsi.hue = arg->explosion.mode_arg.hsi.hue;
                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.hsi.sat = arg->explosion.mode_arg.hsi.sat;
                        break;
                    case RS485_FX_Mode_XY:
                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.xy.intensity = arg->explosion.lightness ; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_XY].XY.intensity;
                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.xy.x  = arg->explosion.mode_arg.xy.x;
                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.xy.y = arg->explosion.mode_arg.xy.y;
                        break;
                    case RS485_FX_Mode_GEL:
                    {
						p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.gel.intensity = arg->explosion.lightness ;
						p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.gel.cct = arg->explosion.mode_arg.gel.cct;
						brand  = arg->explosion.mode_arg.gel.brand;
						p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.gel.origin  = brand == 0 ? 1 : 0;
						series     = arg->explosion.mode_arg.gel.type[brand];
						p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.gel.series  = series;
						p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.gel.color   = arg->explosion.mode_arg.gel.color[brand][series];
						break;
					}
                    
                    case RS485_FX_Mode_Source:
                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.source.intensity = arg->explosion.lightness ; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_SOURCE].SOURCE.intensity ;
                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.source.type = arg->explosion.mode_arg.source.type;
                        break;
                    default:break;
                }
				arg->explosion.state = false;
            }
            else
            {
                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
            }
            break;
        case LIGHT_MODE_FX_EXPLOSION_II:
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX_II;
            p_cmd_body->cmd_arg.sys_fx_2.type = RS485_FX_II_Explosion_2;

            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX_II;
            p_cmd_body->cmd_arg.sys_fx_2.type = RS485_FX_II_Explosion_2;
            p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.intensity = arg->explosion_2.lightness;
            p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode = get_two_effect_mode_back((rs485_fx_2_mode_enum)arg->explosion_2.mode);
            p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.decay = arg->explosion_2.decay;
            p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.state = (rs485_fx_2_trigger_enum)arg->explosion_2.state;

            if(arg->explosion_2.state >= 1)
            {
                arg->explosion_2.state = 0;
				if(get_lfm_port_mode() == 0)
			    ui_set_explosion_2_state(0);
            }
            switch(p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode)
            {
                case RS485_FX_II_Mode_CCT:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.cct.cct = arg->explosion_2.mode_arg.cct.cct;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.cct.duv  = (uint8_t)arg->explosion_2.mode_arg.cct.duv ;
                    break;
                case RS485_FX_II_Mode_HSI:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.hsi.hue = arg->explosion_2.mode_arg.hsi.hue;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.hsi.sat = arg->explosion_2.mode_arg.hsi.sat;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.hsi.cct = arg->explosion_2.mode_arg.hsi.cct;
                    break;
                case RS485_FX_II_Mode_XY:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.xy.x = arg->explosion_2.mode_arg.xy.x;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.xy.y = arg->explosion_2.mode_arg.xy.y;
                    break;
                case RS485_FX_II_Mode_GEL:
                {
					p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.gel.cct = arg->explosion_2.mode_arg.gel.cct;
					brand  = arg->explosion_2.mode_arg.gel.brand;
					p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.gel.origin  = brand == 0 ? 1 : 0;
					series     = arg->explosion_2.mode_arg.gel.type[brand];
					p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.gel.series  = series;
					p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.gel.color   = arg->explosion_2.mode_arg.gel.color[brand][series];
					break;
				}
                case RS485_FX_II_Mode_SOUYRCE:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.source.type = arg->explosion_2.mode_arg.source.type;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.source.x_coord = arg->explosion_2.mode_arg.source.x;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.source.y_coord = arg->explosion_2.mode_arg.source.y;
                    break;
                case RS485_FX_II_Mode_RGB:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.rgb.blue_ratio = arg->explosion_2.mode_arg.rgb.b;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.rgb.green_ratio = arg->explosion_2.mode_arg.rgb.g;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.rgb.red_ratio = arg->explosion_2.mode_arg.rgb.r;;
                    break;
                default:break;
            }
            break;
        case LIGHT_MODE_FX_FAULT_BULB:
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_FaultBulb;

            if(true == arg->fault_bulb.state)
            {
                p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode = get_one_effect_mode_back(((rs485_fx_mode_enum)arg->fault_bulb.mode));
                p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.frq  = arg->fault_bulb.frq;
                p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.spd = arg->fault_bulb.speed;

                switch(p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode)
                {
                    case RS485_FX_Mode_CCT:
                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.cct.intensity = arg->fault_bulb.lightness  ; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_CCT].CCT.intensity;
                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.cct.cct = arg->fault_bulb.mode_arg.cct.cct;
                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.cct.gm  = (uint8_t)arg->fault_bulb.mode_arg.cct.duv ;
                        break;
                    case RS485_FX_Mode_HSI:
                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.hsi.intensity  = arg->fault_bulb.lightness ;//g_tUIAllData.strobe_model.setting[PARAM_SETTING_HSI].HSI.hsi.intensity;
                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.hsi.hue = arg->fault_bulb.mode_arg.hsi.hue;
                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.hsi.sat = arg->fault_bulb.mode_arg.hsi.sat;
                        break;
                    case RS485_FX_Mode_XY:
                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.xy.intensity = arg->fault_bulb.lightness ; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_XY].XY.intensity;
                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.xy.x  = arg->fault_bulb.mode_arg.xy.x;
                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.xy.y = arg->fault_bulb.mode_arg.xy.y;
                        break;
                    case RS485_FX_Mode_GEL:
						p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.gel.intensity = arg->fault_bulb.lightness ;
                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.gel.cct = arg->fault_bulb.mode_arg.gel.cct;
						brand  = arg->fault_bulb.mode_arg.gel.brand;
						p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.gel.origin  = brand == 0 ? 1 : 0;
						series     = arg->fault_bulb.mode_arg.gel.type[brand];
						p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.gel.series  = series;
						p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.gel.color   = arg->fault_bulb.mode_arg.gel.color[brand][series];
                        break;
                    case RS485_FX_Mode_Source:
                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.source.intensity = arg->fault_bulb.lightness ; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_SOURCE].SOURCE.intensity ;
                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.source.type = arg->fault_bulb.mode_arg.source.type;
                        break;
                    default:break;
                }
            }
            else
            {
                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
            }
            break;
        case LIGHT_MODE_FX_FAULT_BULB_II :
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX_II;
            p_cmd_body->cmd_arg.sys_fx_2.type = RS485_FX_II_Fault_Bulb_2;
            p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.intensity = arg->fault_bulb_2.lightness;
            p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode = get_two_effect_mode_back((rs485_fx_2_mode_enum)arg->fault_bulb_2.mode);
            p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.spd = arg->fault_bulb_2.spd;
            p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.state =  (rs485_fx_2_trigger_enum)((arg->fault_bulb_2.state == 0 ) ? 0 : 2);
            p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.frq =  arg->fault_bulb_2.frq;

            switch(p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode)
            {
                case RS485_FX_II_Mode_CCT:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.cct.cct = arg->fault_bulb_2.mode_arg.cct.cct;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.cct.duv  = (uint8_t)arg->fault_bulb_2.mode_arg.cct.duv ;
                    break;
                case RS485_FX_II_Mode_HSI:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.hsi.hue = arg->fault_bulb_2.mode_arg.hsi.hue;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.hsi.sat = arg->fault_bulb_2.mode_arg.hsi.sat;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.hsi.cct = arg->fault_bulb_2.mode_arg.hsi.cct;
                    break;
                case RS485_FX_II_Mode_XY:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.xy.x = arg->fault_bulb_2.mode_arg.xy.x;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.xy.y = arg->fault_bulb_2.mode_arg.xy.y;
                    break;
                case RS485_FX_II_Mode_GEL:
                     {
						p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.gel.cct = arg->fault_bulb_2.mode_arg.gel.cct;
						brand  = arg->fault_bulb_2.mode_arg.gel.brand;
						p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.gel.origin  = brand == 0 ? 1 : 0;
						series     = arg->fault_bulb_2.mode_arg.gel.type[brand];
						p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.gel.series  = series;
						p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.gel.color   = arg->fault_bulb_2.mode_arg.gel.color[brand][series];
						break;
					}
                case RS485_FX_II_Mode_SOUYRCE:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.source.type = arg->fault_bulb_2.mode_arg.source.type;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.source.x_coord = arg->fault_bulb_2.mode_arg.source.x;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.source.y_coord = arg->fault_bulb_2.mode_arg.source.y;
                    break;
                case RS485_FX_II_Mode_RGB:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.rgb.blue_ratio = arg->fault_bulb_2.mode_arg.rgb.b;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.rgb.green_ratio = arg->fault_bulb_2.mode_arg.rgb.g;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.rgb.red_ratio = arg->fault_bulb_2.mode_arg.rgb.r;
                    break;
                default:break;
            }
            break;
        case LIGHT_MODE_FX_PULSING:
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Pulsing;

            if(true == arg->pulsing.state)
            {
                p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode = get_one_effect_mode_back(((rs485_fx_mode_enum)arg->pulsing.mode));
                p_cmd_body->cmd_arg.sys_fx.arg.pulsing.frq  = arg->pulsing.frq;
                p_cmd_body->cmd_arg.sys_fx.arg.pulsing.spd = arg->pulsing.speed;

                switch(p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode)
                {
                    case RS485_FX_Mode_CCT:
                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.cct.intensity = arg->pulsing.lightness  ; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_CCT].CCT.intensity;
                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.cct.cct = arg->pulsing.mode_arg.cct.cct;
                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.cct.gm  = (uint8_t)arg->pulsing.mode_arg.cct.duv ;
                        break;
                    case RS485_FX_Mode_HSI:
                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.hsi.intensity  = arg->pulsing.lightness ;//g_tUIAllData.strobe_model.setting[PARAM_SETTING_HSI].HSI.hsi.intensity;
                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.hsi.hue = arg->pulsing.mode_arg.hsi.hue;
                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.hsi.sat = arg->pulsing.mode_arg.hsi.sat;
                        break;
                    case RS485_FX_Mode_XY:
                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.xy.intensity = arg->pulsing.lightness ; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_XY].XY.intensity;
                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.xy.x  = arg->pulsing.mode_arg.xy.x;
                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.xy.y = arg->pulsing.mode_arg.xy.y;
                        break;
                    case RS485_FX_Mode_GEL:
						p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.gel.intensity = arg->pulsing.lightness;
                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.gel.cct = arg->pulsing.mode_arg.gel.cct;
						brand  = arg->pulsing.mode_arg.gel.brand;
						p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.gel.origin  = brand == 0 ? 1 : 0;
						series     = arg->pulsing.mode_arg.gel.type[brand];
						p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.gel.series  = series;
						p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.gel.color   = arg->pulsing.mode_arg.gel.color[brand][series];
                        break;
                    case RS485_FX_Mode_Source:
                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.source.intensity = arg->pulsing.lightness ; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_SOURCE].SOURCE.intensity ;
                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.source.type = arg->pulsing.mode_arg.source.type;
                        break;
                    default:break;
                }
            }
            else
            {
                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
            }
            break;
        case LIGHT_MODE_FX_PULSING_II :
		{	
			
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX_II;
            p_cmd_body->cmd_arg.sys_fx_2.type = RS485_FX_II_Pulsing_2;
            p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.intensity = arg->pulsing_2.lightness;
            p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode = get_two_effect_mode_back((rs485_fx_2_mode_enum)arg->pulsing_2.mode);
            p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.spd = arg->pulsing_2.spd;
            p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.state = (rs485_fx_2_trigger_enum) ((arg->pulsing_2.state == 0 ) ? 0 : 2);
            p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.frq =  arg->pulsing_2.frq;

            switch(p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode)
            {
                case RS485_FX_II_Mode_CCT:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.cct.cct = arg->pulsing_2.mode_arg.cct.cct;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.cct.duv  = (uint8_t)arg->pulsing_2.mode_arg.cct.duv ;
                    break;
                case RS485_FX_II_Mode_HSI:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.hsi.hue = arg->pulsing_2.mode_arg.hsi.hue;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.hsi.sat = arg->pulsing_2.mode_arg.hsi.sat;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.hsi.cct = arg->pulsing_2.mode_arg.hsi.cct;
                    break;
                case RS485_FX_II_Mode_XY:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.xy.x = arg->pulsing_2.mode_arg.xy.x;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.xy.y = arg->pulsing_2.mode_arg.xy.y;
                    break;
                case RS485_FX_II_Mode_GEL:
				{
                    p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.gel.cct = arg->pulsing_2.mode_arg.gel.cct;
					brand  = arg->pulsing_2.mode_arg.gel.brand;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.gel.origin  = brand == 0 ? 1 : 0;
					series     = arg->pulsing_2.mode_arg.gel.type[brand];
                    p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.gel.series  = series;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.gel.color   = arg->pulsing_2.mode_arg.gel.color[brand][series];
                    break;
				}
                case RS485_FX_II_Mode_SOUYRCE:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.source.type = arg->pulsing_2.mode_arg.source.type;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.source.x_coord = arg->pulsing_2.mode_arg.source.x;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.source.y_coord = arg->pulsing_2.mode_arg.source.y;
                    break;
                case RS485_FX_II_Mode_RGB:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.rgb.blue_ratio = arg->pulsing_2.mode_arg.rgb.b;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.rgb.green_ratio = arg->pulsing_2.mode_arg.rgb.g;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.rgb.red_ratio = arg->pulsing_2.mode_arg.rgb.r;;
                    break;
                default:break;
            }
            break;
		}
        case LIGHT_MODE_FX_WELDING:
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Welding;

            if(true == arg->welding.state)
            {
                p_cmd_body->cmd_arg.sys_fx.arg.welding.mode = get_one_effect_mode_back(((rs485_fx_mode_enum)arg->welding.mode));
                p_cmd_body->cmd_arg.sys_fx.arg.welding.frq  = arg->welding.frq;
                p_cmd_body->cmd_arg.sys_fx.arg.welding.min_int = arg->welding.min;

                switch(p_cmd_body->cmd_arg.sys_fx.arg.welding.mode)
                {
                    case RS485_FX_Mode_CCT:
                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.cct.intensity = arg->welding.lightness  ; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_CCT].CCT.intensity;
                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.cct.cct = arg->welding.mode_arg.cct.cct;
                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.cct.gm  = (uint8_t)arg->welding.mode_arg.cct.duv ;
                        break;
                    case RS485_FX_Mode_HSI:
                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.hsi.intensity  = arg->welding.lightness ;//g_tUIAllData.strobe_model.setting[PARAM_SETTING_HSI].HSI.hsi.intensity;
                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.hsi.hue = arg->welding.mode_arg.hsi.hue;
                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.hsi.sat = arg->welding.mode_arg.hsi.sat;
                        break;
                    case RS485_FX_Mode_XY:
                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.xy.intensity = arg->welding.lightness ; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_XY].XY.intensity;
                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.xy.x  = arg->welding.mode_arg.xy.x;
                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.xy.y = arg->welding.mode_arg.xy.y;
                        break;
                    case RS485_FX_Mode_GEL:
						p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.gel.intensity = arg->welding.lightness ;
                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.gel.cct = arg->welding.mode_arg.gel.cct;
						brand  = arg->welding.mode_arg.gel.brand;
						p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.gel.origin  = brand == 0 ? 1 : 0;
						series     = arg->welding.mode_arg.gel.type[brand];
						p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.gel.series  = series;
						p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.gel.color   = arg->welding.mode_arg.gel.color[brand][series];
                        break;
                    case RS485_FX_Mode_Source:
                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.source.intensity = arg->welding.lightness ; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_SOURCE].SOURCE.intensity ;
                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.source.type = arg->welding.mode_arg.source.type;
                        break;
                    default:break;
                }
            }
            else
            {
                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
            }
            break;
        case LIGHT_MODE_FX_WELDING_II :
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX_II;
            p_cmd_body->cmd_arg.sys_fx_2.type = RS485_FX_II_Welding_2;
            p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.intensity = arg->welding_2.lightness;
            p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode = get_two_effect_mode_back((rs485_fx_2_mode_enum)arg->welding_2.mode);
            p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.min_intensity =  arg->welding_2.min_lightness;
            p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.state =  (rs485_fx_2_trigger_enum)((arg->welding_2.state == 0 ) ? 0 : 2);
            p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.frq =  arg->welding_2.frq;

            switch(p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode)
            {
                case RS485_FX_II_Mode_CCT:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.cct.cct = arg->welding_2.mode_arg.cct.cct;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.cct.duv  = (uint8_t)arg->welding_2.mode_arg.cct.duv ;
                    break;
                case RS485_FX_II_Mode_HSI:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.hsi.hue = arg->welding_2.mode_arg.hsi.hue;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.hsi.sat = arg->welding_2.mode_arg.hsi.sat;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.hsi.cct = arg->welding_2.mode_arg.hsi.cct;
                    break;
                case RS485_FX_II_Mode_XY:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.xy.x = arg->welding_2.mode_arg.xy.x;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.xy.y = arg->welding_2.mode_arg.xy.y;
                    break;
                case RS485_FX_II_Mode_GEL:
                    {
						p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.gel.cct = arg->welding_2.mode_arg.gel.cct;
						brand  = arg->welding_2.mode_arg.gel.brand;
						p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.gel.origin  = brand == 0 ? 1 : 0;
						series     = arg->welding_2.mode_arg.gel.type[brand];
						p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.gel.series  = series;
						p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.gel.color   = arg->welding_2.mode_arg.gel.color[brand][series];
						break;
					}
                case RS485_FX_II_Mode_SOUYRCE:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.source.type = arg->welding_2.mode_arg.source.type;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.source.x_coord = arg->welding_2.mode_arg.source.x;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.source.y_coord = arg->welding_2.mode_arg.source.y;
                    break;
                case RS485_FX_II_Mode_RGB:
                    p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.rgb.blue_ratio = arg->welding_2.mode_arg.rgb.b;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.rgb.green_ratio = arg->welding_2.mode_arg.rgb.g;
                    p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.rgb.red_ratio = arg->welding_2.mode_arg.rgb.r;;
                    break;
                default:break;
            }
            break;
        case LIGHT_MODE_FX_COP_CAR:
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_CopCar;

            if(1 == arg->cop_car.state)
            {
                p_cmd_body->cmd_arg.sys_fx.arg.cop_car.intensity = arg->cop_car.lightness;
                p_cmd_body->cmd_arg.sys_fx.arg.cop_car.frq = arg->cop_car.frq;
                p_cmd_body->cmd_arg.sys_fx.arg.cop_car.color =  arg->cop_car.color;
            }
            else
            {
                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;

            }
            break;
        case LIGHT_MODE_FX_COP_CAR_II :
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX_II;
            p_cmd_body->cmd_arg.sys_fx_2.type = RS485_FX_II_Cop_Car_2;
            p_cmd_body->cmd_arg.sys_fx_2.arg.cop_car_2.state = (rs485_fx_2_trigger_enum)(( arg->cop_car_2.state == 0 ) ? 0 : 2);
            p_cmd_body->cmd_arg.sys_fx_2.arg.cop_car_2.intensity = arg->cop_car_2.lightness;
            p_cmd_body->cmd_arg.sys_fx_2.arg.cop_car_2.frq = arg->cop_car_2.frq;
            p_cmd_body->cmd_arg.sys_fx_2.arg.cop_car_2.color =  arg->cop_car_2.color;
            break;
        case LIGHT_MODE_FX_COLOR_CHASE:
            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_ColorChase;
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;

            if(1 == arg->color_chase.state)
            {
                p_cmd_body->cmd_arg.sys_fx.arg.color_chase.intensity = arg->color_chase.lightness;
                p_cmd_body->cmd_arg.sys_fx.arg.color_chase.frq = arg->color_chase.spd;
                p_cmd_body->cmd_arg.sys_fx.arg.color_chase.sat =  arg->color_chase.sat;
            }
            else
            {
                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
            }
            break;
        case LIGHT_MODE_FX_PARTY_LIGHTS:
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_PartyLights;

            if(1 == arg->party_lights.state)
            {

                p_cmd_body->cmd_arg.sys_fx.arg.party_lights.intensity = arg->party_lights.lightness;
                p_cmd_body->cmd_arg.sys_fx.arg.party_lights.frq = arg->party_lights.spd;
                p_cmd_body->cmd_arg.sys_fx.arg.party_lights.sat =  arg->party_lights.sat;
            }
            else
            {
                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
            }
            break;
        case LIGHT_MODE_FX_PARTY_LIGHTS_II:
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX_II;
            p_cmd_body->cmd_arg.sys_fx_2.type = RS485_FX_II_Party_Lights_2;
            p_cmd_body->cmd_arg.sys_fx_2.arg.party_lights_2.state = (rs485_fx_2_trigger_enum)arg->party_lights_2.state;
            p_cmd_body->cmd_arg.sys_fx_2.arg.party_lights_2.intensity = arg->party_lights_2.lightness;
            p_cmd_body->cmd_arg.sys_fx_2.arg.party_lights_2.spd = arg->party_lights_2.spd;
            p_cmd_body->cmd_arg.sys_fx_2.arg.party_lights_2.sat =  arg->party_lights_2.sat;
            break;
        case LIGHT_MODE_FX_FIREWORKS:
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Fireworks;

            if(1 == arg->party_lights.state)
            {
                p_cmd_body->cmd_arg.sys_fx.arg.fireworks.intensity = arg->fireworks.lightness;
                p_cmd_body->cmd_arg.sys_fx.arg.fireworks.frq = arg->fireworks.frq;
                p_cmd_body->cmd_arg.sys_fx.arg.fireworks.type =  arg->fireworks.type;
            }
            else
            {
                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
            }
            break;
        case LIGHT_MODE_FX_FIREWORKS_II:
            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX_II;
            p_cmd_body->cmd_arg.sys_fx_2.type = RS485_FX_II_Fireworks_2;
            p_cmd_body->cmd_arg.sys_fx_2.arg.fireworks_2.state = (rs485_fx_2_trigger_enum)arg->fireworks_2.state;
            p_cmd_body->cmd_arg.sys_fx_2.arg.fireworks_2.intensity = arg->fireworks_2.lightness;
            p_cmd_body->cmd_arg.sys_fx_2.arg.fireworks_2.gap_time = arg->fireworks_2.gap_time;
            p_cmd_body->cmd_arg.sys_fx_2.arg.fireworks_2.mode =  arg->fireworks_2.mode;
            p_cmd_body->cmd_arg.sys_fx_2.arg.fireworks_2.min_gap_time = arg->fireworks_2.min_gap_time;
            break;
        case LIGHT_MODE_DMX:
        {
            uint16_t    dmx_fade_time = 0;
            
            data_center_read_config_data(SYS_CONFIG_DMX_FADETIME, &dmx_fade_time);
            if(dmx_fade_time == 0)
                dmx_fade_time  = 0;
            else if(dmx_fade_time == 1)
                dmx_fade_time  = 350;
            else if(dmx_fade_time == 2)
                dmx_fade_time  = 1000;
			if(mdx_loss_start == 1)
			{
				dmx_fade_time = 0;
			}
            switch(arg->dmx.dmx_data.type)
            {
                case DMX_TYPE_CCT:
                    if(arg->dmx.strobe_or_fx_state == 0 )
                    {
                        p_cmd_body->header.cmd_type = RS485_Cmd_CCT;
                        p_cmd_body->cmd_arg.cct.intensity =  arg->dmx.dmx_data.lightness / 1000.0f;
                        p_cmd_body->cmd_arg.cct.cct = arg->dmx.dmx_data.arg.cct.cct ;
                        p_cmd_body->cmd_arg.cct.duv  = arg->dmx.dmx_data.arg.cct.duv;
                        p_cmd_body->cmd_arg.cct.fade  = dmx_fade_time;
                    }
                    else
                    {
						p_cmd_body->header.cmd_type = RS485_Cmd_DMX_Strobe;
                        p_cmd_body->cmd_arg.dmx_strobe.frq = arg->dmx.strobe_or_fx_state;
                        p_cmd_body->cmd_arg.dmx_strobe.mode = RS485_DMX_Strobe_Mode_CCT;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.cct.intensity = arg->dmx.dmx_data.lightness / 1000.0f;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.cct.cct = arg->dmx.dmx_data.arg.cct.cct;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.cct.duv = arg->dmx.dmx_data.arg.cct.duv;
                    }
                    break;
                case DMX_TYPE_HSI:
                    if(arg->dmx.strobe_or_fx_state == 0 )
                    {
                        p_cmd_body->header.cmd_type = RS485_Cmd_HSI;
                        p_cmd_body->cmd_arg.hsi.intensity = arg->dmx.dmx_data.lightness / 1000.0f;
                        p_cmd_body->cmd_arg.hsi.hue = arg->dmx.dmx_data.arg.hsi.hue * 100;
                        p_cmd_body->cmd_arg.hsi.sat = arg->dmx.dmx_data.arg.hsi.sat * 100;
                        p_cmd_body->cmd_arg.hsi.cct = arg->dmx.dmx_data.arg.hsi.cct;
                        p_cmd_body->cmd_arg.hsi.fade  = dmx_fade_time;
                    }
                    else
                    {
						p_cmd_body->header.cmd_type = RS485_Cmd_DMX_Strobe;
                        p_cmd_body->cmd_arg.dmx_strobe.frq = arg->dmx.strobe_or_fx_state;
                        p_cmd_body->cmd_arg.dmx_strobe.mode = RS485_DMX_Strobe_Mode_HSI;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.hsi.intensity = arg->dmx.dmx_data.lightness / 1000.0f;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.hsi.hue = arg->dmx.dmx_data.arg.hsi.hue * 100;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.hsi.sat = arg->dmx.dmx_data.arg.hsi.sat * 100;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.hsi.cct = arg->dmx.dmx_data.arg.hsi.cct;
                    }
                    break;

                case DMX_TYPE_RGB://JaggerHe_RGB_Add_Int ??
                    if(arg->dmx.strobe_or_fx_state == 0 )
                    {
                        p_cmd_body->header.cmd_type = RS485_Cmd_RGB;
                        p_cmd_body->cmd_arg.rgb.red_ratio = arg->dmx.dmx_data.arg.rgb.red*1000;
                        p_cmd_body->cmd_arg.rgb.green_ratio = arg->dmx.dmx_data.arg.rgb.green*1000;
                        p_cmd_body->cmd_arg.rgb.blue_ratio = arg->dmx.dmx_data.arg.rgb.blue*1000;
                        p_cmd_body->cmd_arg.rgb.fade  = dmx_fade_time;
                        p_cmd_body->cmd_arg.rgb.intensity  = arg->dmx.dmx_data.lightness / 1000.0f;
                    }
                    else
                    {
						p_cmd_body->cmd_arg.dmx_strobe.frq = arg->dmx.strobe_or_fx_state;
						p_cmd_body->header.cmd_type = RS485_Cmd_DMX_Strobe;
                        p_cmd_body->cmd_arg.dmx_strobe.mode = RS485_DMX_Strobe_Mode_RGB;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.rgb.intensity = arg->dmx.dmx_data.lightness / 1000.0f;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.rgb.red_ratio = arg->dmx.dmx_data.arg.rgb.red*1000;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.rgb.green_ratio = arg->dmx.dmx_data.arg.rgb.green*1000;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.rgb.blue_ratio = arg->dmx.dmx_data.arg.rgb.blue*1000;
                    }
                    break;
                case DMX_TYPE_GEL:
                    if(arg->dmx.strobe_or_fx_state == 0 )
                    {
                        p_cmd_body->header.cmd_type = RS485_Cmd_GEL;
                        p_cmd_body->cmd_arg.gel.intensity   	  = arg->dmx.dmx_data.lightness / 1000.0f;
                        p_cmd_body->cmd_arg.gel.cct        = gel_cct[arg->dmx.dmx_data.arg.gel.cct];
                        p_cmd_body->cmd_arg.gel.origin = arg->dmx.dmx_data.arg.gel.brand == 0 ? 1 : 0;
                        p_cmd_body->cmd_arg.gel.series   = arg->dmx.dmx_data.arg.gel.type;
                        p_cmd_body->cmd_arg.gel.color  = arg->dmx.dmx_data.arg.gel.color;
                        p_cmd_body->cmd_arg.gel.fade  = dmx_fade_time;
                    }
                    else
                    {
						p_cmd_body->cmd_arg.dmx_strobe.frq = arg->dmx.strobe_or_fx_state;
						p_cmd_body->header.cmd_type = RS485_Cmd_DMX_Strobe;
                        p_cmd_body->cmd_arg.dmx_strobe.mode = RS485_DMX_Strobe_Mode_GEL;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.gel.intensity = arg->dmx.dmx_data.lightness / 1000.0f;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.gel.cct  = gel_cct[arg->dmx.dmx_data.arg.gel.cct];
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.gel.origin  = arg->dmx.dmx_data.arg.gel.brand == 0 ? 1 : 0;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.gel.series = arg->dmx.dmx_data.arg.gel.type;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.gel.color = arg->dmx.dmx_data.arg.gel.color;
                    }
                    break;
				case DMX_TYPE_RGBWW:
					if(arg->dmx.strobe_or_fx_state == 0 )
                    {
                        p_cmd_body->header.cmd_type = RS485_Cmd_RGBWW;
                        p_cmd_body->cmd_arg.rgbww.red_ratio = arg->dmx.dmx_data.arg.rgb.red*1000;
                        p_cmd_body->cmd_arg.rgbww.green_ratio = arg->dmx.dmx_data.arg.rgb.green*1000;
                        p_cmd_body->cmd_arg.rgbww.blue_ratio = arg->dmx.dmx_data.arg.rgb.blue*1000;
						p_cmd_body->cmd_arg.rgbww.cw_ratio = arg->dmx.dmx_data.arg.rgb.cw*1000;
						p_cmd_body->cmd_arg.rgbww.ww_ratio = arg->dmx.dmx_data.arg.rgb.ww*1000;
                        p_cmd_body->cmd_arg.rgbww.intensity  = arg->dmx.dmx_data.lightness / 1000.0f;
						p_cmd_body->cmd_arg.rgbww.fade  = dmx_fade_time;
                    }
                    else
                    {
						p_cmd_body->cmd_arg.dmx_strobe.frq = arg->dmx.strobe_or_fx_state;
						p_cmd_body->header.cmd_type = RS485_Cmd_DMX_Strobe;
                        p_cmd_body->cmd_arg.dmx_strobe.mode = RS485_DMX_Strobe_Mode_RGBWW;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.rgbww.intensity = arg->dmx.dmx_data.lightness / 1000.0f;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.rgbww.red_ratio = arg->dmx.dmx_data.arg.rgb.red*1000;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.rgbww.green_ratio = arg->dmx.dmx_data.arg.rgb.green*1000;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.rgbww.blue_ratio = arg->dmx.dmx_data.arg.rgb.blue*1000;
						p_cmd_body->cmd_arg.dmx_strobe.mode_arg.rgbww.cw_ratio = arg->dmx.dmx_data.arg.rgb.cw*1000;
						p_cmd_body->cmd_arg.dmx_strobe.mode_arg.rgbww.ww_ratio = arg->dmx.dmx_data.arg.rgb.ww*1000;
                    }
					break;
                case DMX_TYPE_FX:
                    switch( (uint8_t)arg->dmx.dmx_data.arg.fx.type)
                    {
                        case DMX_FX_CLUB_LIGHTS:
                            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
							explosion_strobe_or_fx_state = 0;
							lightning_strobe_or_fx_state = 0;
                            if(arg->dmx.strobe_or_fx_state > 0)
                            {
                                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_ClubLights;
                                p_cmd_body->cmd_arg.sys_fx.arg.club_lights.intensity  = arg->dmx.dmx_data.lightness;
                                p_cmd_body->cmd_arg.sys_fx.arg.club_lights.color = arg->dmx.dmx_data.arg.fx.arg.clue_lights.color;
                                p_cmd_body->cmd_arg.sys_fx.arg.club_lights.frq   = arg->dmx.dmx_data.arg.fx.arg.clue_lights.frq;
                            }
                            else
                            {
                                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
                            }
                            break;
                        case DMX_FX_CANDLE:
                            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
							explosion_strobe_or_fx_state = 0;
							lightning_strobe_or_fx_state = 0;
                            if(arg->dmx.strobe_or_fx_state > 0)
                            {
                                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Candle;
                                p_cmd_body->cmd_arg.sys_fx.arg.candle.intensity = arg->dmx.dmx_data.lightness;
                                p_cmd_body->cmd_arg.sys_fx.arg.candle.frq = arg->dmx.dmx_data.arg.fx.arg.candle.frq;
                                p_cmd_body->cmd_arg.sys_fx.arg.candle.cct = arg->dmx.dmx_data.arg.fx.arg.candle.cct_rang;
                            }
                            else
                            {
                                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
                            }
                            break;
                        case DMX_FX_PAPARAZZI:
						{
                            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
							explosion_strobe_or_fx_state = 0;
							lightning_strobe_or_fx_state = 0;
                            if(arg->dmx.strobe_or_fx_state > 0 )
                            {
                                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Paparazzi;
                                p_cmd_body->cmd_arg.sys_fx.arg.paparazzi.intensity = arg->dmx.dmx_data.lightness;
                                p_cmd_body->cmd_arg.sys_fx.arg.paparazzi.frq = arg->dmx.dmx_data.arg.fx.arg.paparazzi.frq;
                                p_cmd_body->cmd_arg.sys_fx.arg.paparazzi.gm =  (uint8_t)arg->dmx.dmx_data.arg.fx.arg.paparazzi.duv;
								#if PROJECT_TYPE==308
                                p_cmd_body->cmd_arg.sys_fx.arg.paparazzi.cct =  g_tUIAllData.paparazzi_model.cct;
								#elif PROJECT_TYPE==307
								p_cmd_body->cmd_arg.sys_fx.arg.paparazzi.cct =  arg->dmx.dmx_data.arg.fx.arg.paparazzi.cct;
								#endif
                            }
                            else
                            {
                                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
                            }
						}
                            break;
                        case DMX_FX_LIGHTNING:
                            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
							explosion_strobe_or_fx_state = 0;
                            if(arg->dmx.strobe_or_fx_state > 0 )
                            {
								if(arg->dmx.strobe_or_fx_state == 1 && lightning_strobe_or_fx_state == 1)
								{
									break;
								}
								else
								{
									lightning_strobe_or_fx_state = arg->dmx.strobe_or_fx_state;
								}
                                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Lightning;
                                p_cmd_body->cmd_arg.sys_fx.arg.lightning.intensity = arg->dmx.dmx_data.lightness;
                                p_cmd_body->cmd_arg.sys_fx.arg.lightning.frq = arg->dmx.dmx_data.arg.fx.arg.lightning.frq;
                                p_cmd_body->cmd_arg.sys_fx.arg.lightning.gm =  (uint8_t)arg->dmx.dmx_data.arg.fx.arg.lightning.duv;
								#if PROJECT_TYPE==308
                                p_cmd_body->cmd_arg.sys_fx.arg.lightning.cct =  g_tUIAllData.lightning_model.cct;
								#elif PROJECT_TYPE==307
								p_cmd_body->cmd_arg.sys_fx.arg.lightning.cct =  arg->dmx.dmx_data.arg.fx.arg.lightning.cct;
								#endif
								#if PROJECT_TYPE==308
                                p_cmd_body->cmd_arg.sys_fx.arg.lightning.spd =  5;
								#endif
                                p_cmd_body->cmd_arg.sys_fx.arg.lightning.trigger = arg->dmx.strobe_or_fx_state;
                            }
                            else
                            {
								lightning_strobe_or_fx_state = arg->dmx.strobe_or_fx_state;
                                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
                            }
                            break;
                        case DMX_FX_COLOR_CHASE:
                            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
							explosion_strobe_or_fx_state = 0;
							lightning_strobe_or_fx_state = 0;
                            if( arg->dmx.strobe_or_fx_state > 0)
                            {
                                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_ColorChase;
                                p_cmd_body->cmd_arg.sys_fx.arg.color_chase.intensity = arg->dmx.dmx_data.lightness;
                                p_cmd_body->cmd_arg.sys_fx.arg.color_chase.frq = arg->dmx.dmx_data.arg.fx.arg.color_chase.spd;
                                p_cmd_body->cmd_arg.sys_fx.arg.color_chase.sat =  arg->dmx.dmx_data.arg.fx.arg.color_chase.sat;
                            }
                            else
                            {
                                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
                            }
                            break;
                        case DMX_FX_PARTY_LIGHTS:
                            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
                            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_PartyLights;
							explosion_strobe_or_fx_state = 0;
							lightning_strobe_or_fx_state = 0;
                            if( arg->dmx.strobe_or_fx_state > 0)
                            {

                                p_cmd_body->cmd_arg.sys_fx.arg.party_lights.intensity =  arg->dmx.dmx_data.lightness;
                                p_cmd_body->cmd_arg.sys_fx.arg.party_lights.frq = arg->dmx.dmx_data.arg.fx.arg.party_lights.frq;
                                p_cmd_body->cmd_arg.sys_fx.arg.party_lights.sat = arg->dmx.dmx_data.arg.fx.arg.party_lights.sat;

                            }
                            else
                            {
                                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;

                            }
                            break;
                        case DMX_FX_FIREWORKS:
                            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
                            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Fireworks;
							explosion_strobe_or_fx_state = 0;
							lightning_strobe_or_fx_state = 0;
                            if( arg->dmx.strobe_or_fx_state > 0)
                            {
                                p_cmd_body->cmd_arg.sys_fx.arg.fireworks.intensity = arg->dmx.dmx_data.lightness;
								#if PROJECT_TYPE==308
                                p_cmd_body->cmd_arg.sys_fx.arg.fireworks.type = g_tUIAllData.fireworks_model.type;
								#elif PROJECT_TYPE==307
								p_cmd_body->cmd_arg.sys_fx.arg.fireworks.type =  arg->dmx.dmx_data.arg.fx.arg.fireworks.type;
								#endif
                                p_cmd_body->cmd_arg.sys_fx.arg.fireworks.frq = arg->dmx.dmx_data.arg.fx.arg.fireworks.frq;
								
                            }
                            else
                            {
                                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
                            }
                            break;
                        case DMX_FX_TV:
                            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
                            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_TV;
							explosion_strobe_or_fx_state = 0;
							lightning_strobe_or_fx_state = 0;
                            if( arg->dmx.strobe_or_fx_state > 0)
                            {
                                p_cmd_body->cmd_arg.sys_fx.arg.tv.intensity = arg->dmx.dmx_data.lightness;
								#if PROJECT_TYPE==308
                                p_cmd_body->cmd_arg.sys_fx.arg.tv.cct = g_tUIAllData.tv_model.cct;
								#elif PROJECT_TYPE==307
								p_cmd_body->cmd_arg.sys_fx.arg.tv.cct = arg->dmx.dmx_data.arg.fx.arg.tv.cct_range;
								#endif
                                p_cmd_body->cmd_arg.sys_fx.arg.tv.frq = arg->dmx.dmx_data.arg.fx.arg.tv.frq;
                            }
                            else
                            {
                                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
                            }
                            break;
                        case DMX_FX_FIRE:
                            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
                            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Fire;
							explosion_strobe_or_fx_state = 0;
							lightning_strobe_or_fx_state = 0;
                            if( arg->dmx.strobe_or_fx_state > 0)
                            {
                                p_cmd_body->cmd_arg.sys_fx.arg.fire.intensity = arg->dmx.dmx_data.lightness;
								#if PROJECT_TYPE==308
                                p_cmd_body->cmd_arg.sys_fx.arg.fire.cct = g_tUIAllData.fire_model.cct;
								#elif PROJECT_TYPE==307
								p_cmd_body->cmd_arg.sys_fx.arg.fire.cct = arg->dmx.dmx_data.arg.fx.arg.fire.cct_range;
								#endif
                                p_cmd_body->cmd_arg.sys_fx.arg.fire.frq = arg->dmx.dmx_data.arg.fx.arg.fire.frq;
                            }
                            else
                            {
                                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
                            }
                            break;
                        case DMX_FX_COP_CAR:
                            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
                            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_CopCar;
							lightning_strobe_or_fx_state = 0;
							explosion_strobe_or_fx_state = 0;
                            if( arg->dmx.strobe_or_fx_state > 0)
                            {
                                p_cmd_body->cmd_arg.sys_fx.arg.cop_car.intensity = arg->dmx.dmx_data.lightness;
                                p_cmd_body->cmd_arg.sys_fx.arg.cop_car.color = arg->dmx.dmx_data.arg.fx.arg.copcar.color;
                                p_cmd_body->cmd_arg.sys_fx.arg.cop_car.frq = arg->dmx.dmx_data.arg.fx.arg.copcar.frq;
                            }
                            else
                            {
                                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
                            }
                            break;
                        case DMX_FX_FAULT_BULB:
                            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
                            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_FaultBulb;
							explosion_strobe_or_fx_state = 0;
							lightning_strobe_or_fx_state = 0;
                            if( arg->dmx.strobe_or_fx_state > 0)
                            {
                                p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode = (uint8_t)arg->dmx.dmx_data.arg.fx.arg.fault_bulb.mode;
                                switch((uint8_t)arg->dmx.dmx_data.arg.fx.arg.fault_bulb.mode)
                                {
                                    case RS485_FX_Mode_CCT:
                                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.cct.intensity = arg->dmx.dmx_data.lightness; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_CCT].CCT.intensity;
										#if PROJECT_TYPE==308
										p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.cct.cct = g_tUIAllData.faultybulb_model.mode_arg.cct.cct;
										#elif PROJECT_TYPE==307
										p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.cct.cct = arg->dmx.dmx_data.arg.fx.arg.fault_bulb.mode_arg.cct.cct;
										#endif
                                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.cct.gm  = (uint8_t)arg->dmx.dmx_data.arg.fx.arg.fault_bulb.mode_arg.cct.duv ;
                                        break;
                                    case RS485_FX_Mode_HSI:
                                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.hsi.intensity  = arg->dmx.dmx_data.lightness ;//g_tUIAllData.strobe_model.setting[PARAM_SETTING_HSI].HSI.hsi.intensity;
                                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.hsi.hue = arg->dmx.dmx_data.arg.fx.arg.fault_bulb.mode_arg.hsi.hue;
                                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.hsi.sat = arg->dmx.dmx_data.arg.fx.arg.fault_bulb.mode_arg.hsi.sat;
                                        break;
                                    case RS485_FX_Mode_XY:
                                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.xy.intensity = arg->dmx.dmx_data.lightness; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_XY].XY.intensity;
                                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.xy.x  = arg->dmx.dmx_data.arg.fx.arg.fault_bulb.mode_arg.xy.x;
                                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.xy.y = arg->dmx.dmx_data.arg.fx.arg.fault_bulb.mode_arg.xy.y;
                                        break;
                                    case RS485_FX_Mode_GEL:
                                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.gel.intensity = arg->dmx.dmx_data.lightness; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_GEL].GEL.intensity;
                                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.gel.cct = arg->dmx.dmx_data.arg.fx.arg.fault_bulb.mode_arg.gel.cct;
                                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.gel.origin  = arg->dmx.dmx_data.arg.fx.arg.fault_bulb.mode_arg.gel.brand == 0 ? 1 : 0;
                                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.gel.series     = arg->dmx.dmx_data.arg.fx.arg.fault_bulb.mode_arg.gel.type;
                                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.gel.color   = arg->dmx.dmx_data.arg.fx.arg.fault_bulb.mode_arg.gel.color;
                                        break;
                                    case RS485_FX_Mode_Source:
                                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.source.intensity = arg->dmx.dmx_data.lightness; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_SOURCE].SOURCE.intensity ;
                                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.source.type = arg->dmx.dmx_data.arg.fx.arg.fault_bulb.mode_arg.socue.type;
                                        break;
									case RS485_FX_Mode_RGB:
										p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.source.intensity = arg->dmx.dmx_data.lightness;
										p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.rgb.r = arg->dmx.dmx_data.arg.fx.arg.fault_bulb.mode_arg.rgb.r_ratio; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_SOURCE].SOURCE.intensity ;
                                        p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.rgb.g = arg->dmx.dmx_data.arg.fx.arg.fault_bulb.mode_arg.rgb.g_ratio;
										p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.mode_arg.rgb.b = arg->dmx.dmx_data.arg.fx.arg.fault_bulb.mode_arg.rgb.b_ratio;
										break;
                                    default:break;
                                }
                                p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.frq = arg->dmx.dmx_data.arg.fx.arg.fault_bulb.frq;
                                p_cmd_body->cmd_arg.sys_fx.arg.fault_bulb.spd = arg->dmx.dmx_data.arg.fx.arg.fault_bulb.spd;
                            }
                            else
                            {
                                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
                            }
							break;
//								p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX_II;
//								p_cmd_body->cmd_arg.sys_fx_2.type = RS485_FX_II_Fault_Bulb_2;
//								p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.intensity = arg->dmx.dmx_data.lightness;
//								p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode = ((rs485_fx_2_mode_enum)arg->dmx.dmx_data.arg.fx2.arg.fault_bulb2.mode);
//								p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.spd  = arg->dmx.dmx_data.arg.fx2.arg.fault_bulb2.spd;
//								p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.frq = arg->dmx.dmx_data.arg.fx2.arg.fault_bulb2.frq;
//								p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.state =  arg->dmx.strobe_or_fx_state;
//								switch(p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode)
//								{
//									case RS485_FX_II_Mode_CCT:
//										p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.cct.cct = arg->dmx.dmx_data.arg.fx2.arg.fault_bulb2.arg.cct.cct;
//										p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.cct.duv  = (uint8_t)arg->dmx.dmx_data.arg.fx2.arg.fault_bulb2.arg.cct.duv ;
//									break;
//									case RS485_FX_II_Mode_HSI:
//										p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.hsi.cct = arg->dmx.dmx_data.arg.fx2.arg.fault_bulb2.arg.hsi.cct;
//										p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.hsi.hue = arg->dmx.dmx_data.arg.fx2.arg.fault_bulb2.arg.hsi.hue;
//										p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.hsi.sat = arg->dmx.dmx_data.arg.fx2.arg.fault_bulb2.arg.hsi.sat;
//									break;
//									 case RS485_FX_II_Mode_XY:
//									
//										p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.xy.x  = arg->dmx.dmx_data.arg.fx2.arg.fault_bulb2.arg.xy.x;
//										p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.xy.y = arg->dmx.dmx_data.arg.fx2.arg.fault_bulb2.arg.xy.y;
//									break;
//									case RS485_FX_II_Mode_GEL:
//                                        p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.gel.cct = arg->dmx.dmx_data.arg.fx2.arg.fault_bulb2.arg.gel.cct;
//                                        p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.gel.origin  = arg->dmx.dmx_data.arg.fx2.arg.fault_bulb2.arg.gel.brand == 0 ? 1 : 0;
//                                        p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.gel.series     = arg->dmx.dmx_data.arg.fx2.arg.fault_bulb2.arg.gel.type;
//                                        p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.gel.color   = arg->dmx.dmx_data.arg.fx2.arg.fault_bulb2.arg.gel.color;
//									break;
//									case RS485_FX_II_Mode_SOUYRCE:
//									
//									
//										p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.source.type = arg->dmx.dmx_data.arg.fx2.arg.fault_bulb2.arg.source.type;
//										p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.source.x_coord = arg->dmx.dmx_data.arg.fx2.arg.fault_bulb2.arg.source.x;
//										p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.source.y_coord = arg->dmx.dmx_data.arg.fx2.arg.fault_bulb2.arg.source.y;
//									break;
//									case RS485_FX_II_Mode_RGB:
//										p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.rgb.blue_ratio = arg->dmx.dmx_data.arg.fx2.arg.fault_bulb2.arg.rgb.blue;
//										p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.rgb.green_ratio = arg->dmx.dmx_data.arg.fx2.arg.fault_bulb2.arg.rgb.green;
//										p_cmd_body->cmd_arg.sys_fx_2.arg.fault_bulb_2.mode_arg.rgb.red_ratio = arg->dmx.dmx_data.arg.fx2.arg.fault_bulb2.arg.rgb.red;
//									break;

//									default:break;
//								} 
//                            break;
                        case DMX_FX_PULSING:
                            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
                            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Pulsing;
							explosion_strobe_or_fx_state = 0;
							lightning_strobe_or_fx_state = 0;
                            if( arg->dmx.strobe_or_fx_state > 0)
                            {
                                p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode = (uint8_t)arg->dmx.dmx_data.arg.fx.arg.pulsing.mode;
                                switch((uint8_t)arg->dmx.dmx_data.arg.fx.arg.pulsing.mode)
                                {
                                    case RS485_FX_Mode_CCT:
                                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.cct.intensity = arg->dmx.dmx_data.lightness; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_CCT].CCT.intensity;
										#if PROJECT_TYPE==308
										p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.cct.cct = g_tUIAllData.pulsing_model.mode_arg.cct.cct;
										#elif PROJECT_TYPE==307
										p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.cct.cct = arg->dmx.dmx_data.arg.fx.arg.pulsing.mode_arg.cct.cct;
										#endif
                                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.cct.gm  = (uint8_t)arg->dmx.dmx_data.arg.fx.arg.pulsing.mode_arg.cct.duv ;
                                        break;
                                    case RS485_FX_Mode_HSI:
                                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.hsi.intensity  = arg->dmx.dmx_data.lightness ;//g_tUIAllData.strobe_model.setting[PARAM_SETTING_HSI].HSI.hsi.intensity;
                                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.hsi.hue = arg->dmx.dmx_data.arg.fx.arg.pulsing.mode_arg.hsi.hue;
                                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.hsi.sat = arg->dmx.dmx_data.arg.fx.arg.pulsing.mode_arg.hsi.sat;
                                        break;
                                    case RS485_FX_Mode_XY:
                                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.xy.intensity = arg->dmx.dmx_data.lightness; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_XY].XY.intensity;
                                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.xy.x  = arg->dmx.dmx_data.arg.fx.arg.pulsing.mode_arg.xy.x;
                                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.xy.y = arg->dmx.dmx_data.arg.fx.arg.pulsing.mode_arg.xy.y;
                                        break;
                                    case RS485_FX_Mode_GEL:
                                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.gel.intensity = arg->dmx.dmx_data.lightness; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_GEL].GEL.intensity;
                                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.gel.cct = arg->dmx.dmx_data.arg.fx.arg.pulsing.mode_arg.gel.cct;
                                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.gel.origin  = arg->dmx.dmx_data.arg.fx.arg.pulsing.mode_arg.gel.brand == 0 ? 1 : 0;
                                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.gel.series     = arg->dmx.dmx_data.arg.fx.arg.pulsing.mode_arg.gel.type;
                                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.gel.color   = arg->dmx.dmx_data.arg.fx.arg.pulsing.mode_arg.gel.color;
                                        break;
                                    case RS485_FX_Mode_Source:
                                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.source.intensity = arg->dmx.dmx_data.lightness; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_SOURCE].SOURCE.intensity ;
                                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.source.type = arg->dmx.dmx_data.arg.fx.arg.pulsing.mode_arg.socue.type;
                                        break;
									case RS485_FX_Mode_RGB:
										p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.source.intensity = arg->dmx.dmx_data.lightness;
										p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.rgb.r = arg->dmx.dmx_data.arg.fx.arg.pulsing.mode_arg.rgb.r_ratio; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_SOURCE].SOURCE.intensity ;
                                        p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.rgb.g = arg->dmx.dmx_data.arg.fx.arg.pulsing.mode_arg.rgb.g_ratio;
										p_cmd_body->cmd_arg.sys_fx.arg.pulsing.mode_arg.rgb.b = arg->dmx.dmx_data.arg.fx.arg.pulsing.mode_arg.rgb.b_ratio;
										break;
                                    default:break;
                                }
                                p_cmd_body->cmd_arg.sys_fx.arg.pulsing.frq = arg->dmx.dmx_data.arg.fx.arg.pulsing.frq;
                                p_cmd_body->cmd_arg.sys_fx.arg.pulsing.spd = arg->dmx.dmx_data.arg.fx.arg.pulsing.spd;
                            }
                            else
                            {
                                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
                            }
                            break;
//							p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX_II;
//                            p_cmd_body->cmd_arg.sys_fx_2.type = RS485_FX_II_Pulsing_2;
//							p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.intensity = arg->dmx.dmx_data.lightness;
//							p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode = (uint8_t)arg->dmx.dmx_data.arg.fx2.arg.pulsing2.mode;
//							p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.spd  = arg->dmx.dmx_data.arg.fx2.arg.pulsing2.spd;;
//							p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.frq = arg->dmx.dmx_data.arg.fx2.arg.pulsing2.frq;
//							p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.state =  arg->dmx.strobe_or_fx_state;
//							switch(p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode)
//							{
//								case RS485_FX_II_Mode_CCT:
//									p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.cct.cct = arg->dmx.dmx_data.arg.fx2.arg.pulsing2.arg.cct.cct;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.cct.duv  = (uint8_t)arg->dmx.dmx_data.arg.fx2.arg.pulsing2.arg.cct.duv ;
//								break;
//								case RS485_FX_II_Mode_HSI:
//									p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.hsi.cct = arg->dmx.dmx_data.arg.fx2.arg.pulsing2.arg.hsi.cct;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.hsi.hue = arg->dmx.dmx_data.arg.fx2.arg.pulsing2.arg.hsi.hue;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.hsi.sat = arg->dmx.dmx_data.arg.fx2.arg.pulsing2.arg.hsi.sat;
//								break;
//								 case RS485_FX_II_Mode_XY:
//								
//									p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.xy.x  = arg->dmx.dmx_data.arg.fx2.arg.pulsing2.arg.xy.x;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.xy.y = arg->dmx.dmx_data.arg.fx2.arg.pulsing2.arg.xy.y;
//								break;
//								case RS485_FX_II_Mode_GEL:
//									p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.gel.cct = arg->dmx.dmx_data.arg.fx2.arg.pulsing2.arg.gel.cct;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.gel.origin  = arg->dmx.dmx_data.arg.fx2.arg.pulsing2.arg.gel.brand == 0 ? 1 : 0;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.gel.series     = arg->dmx.dmx_data.arg.fx2.arg.pulsing2.arg.gel.type;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.gel.color   = arg->dmx.dmx_data.arg.fx2.arg.pulsing2.arg.gel.color;
//								break;
//								case RS485_FX_II_Mode_SOUYRCE:
//								
//								
//									p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.source.type = arg->dmx.dmx_data.arg.fx2.arg.pulsing2.arg.source.type;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.source.x_coord = arg->dmx.dmx_data.arg.fx2.arg.pulsing2.arg.source.x;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.source.y_coord = arg->dmx.dmx_data.arg.fx2.arg.pulsing2.arg.source.y;
//								break;
//								case RS485_FX_II_Mode_RGB:
//									p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.rgb.blue_ratio = arg->dmx.dmx_data.arg.fx2.arg.pulsing2.arg.rgb.blue;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.rgb.green_ratio = arg->dmx.dmx_data.arg.fx2.arg.pulsing2.arg.rgb.green;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.pulsing_2.mode_arg.rgb.red_ratio = arg->dmx.dmx_data.arg.fx2.arg.pulsing2.arg.rgb.red;
//								break;

//								default:break;
//							} 
//							break;
                            break;
                        case DMX_FX_EXPLOSION:
                            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
							p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
							lightning_strobe_or_fx_state = 0;

                            if( arg->dmx.strobe_or_fx_state > 0)
                            {
								if(arg->dmx.strobe_or_fx_state == 1 && explosion_strobe_or_fx_state == 1)
								{
									break;
								}
								else
								{
									explosion_strobe_or_fx_state = arg->dmx.strobe_or_fx_state;
								}
								 p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Explosion;
                                p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode = (uint8_t)arg->dmx.dmx_data.arg.fx.arg.explosion.mode;
                                switch((uint8_t)arg->dmx.dmx_data.arg.fx.arg.explosion.mode)
                                {
                                    case RS485_FX_Mode_CCT:
                                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.cct.intensity = arg->dmx.dmx_data.lightness; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_CCT].CCT.intensity;
										#if PROJECT_TYPE==308
										p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.cct.cct = g_tUIAllData.explosion_model.mode_arg.cct.cct;
										#elif PROJECT_TYPE==307
										p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.cct.cct = arg->dmx.dmx_data.arg.fx.arg.explosion.mode_arg.cct.cct;
										#endif
                                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.cct.gm  = (uint8_t)arg->dmx.dmx_data.arg.fx.arg.explosion.mode_arg.cct.duv ;
                                        break;
                                    case RS485_FX_Mode_HSI:
                                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.hsi.intensity  = arg->dmx.dmx_data.lightness ;//g_tUIAllData.strobe_model.setting[PARAM_SETTING_HSI].HSI.hsi.intensity;
                                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.hsi.hue = arg->dmx.dmx_data.arg.fx.arg.explosion.mode_arg.hsi.hue;
                                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.hsi.sat = arg->dmx.dmx_data.arg.fx.arg.explosion.mode_arg.hsi.sat;
                                        break;
                                    case RS485_FX_Mode_XY:
                                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.xy.intensity = arg->dmx.dmx_data.lightness; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_XY].XY.intensity;
                                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.xy.x  = arg->dmx.dmx_data.arg.fx.arg.explosion.mode_arg.xy.x;
                                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.xy.y = arg->dmx.dmx_data.arg.fx.arg.explosion.mode_arg.xy.y;
                                        break;
                                    case RS485_FX_Mode_GEL:
                                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.gel.intensity = arg->dmx.dmx_data.lightness; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_GEL].GEL.intensity;
                                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.gel.cct = arg->dmx.dmx_data.arg.fx.arg.explosion.mode_arg.gel.cct;
                                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.gel.origin  = arg->dmx.dmx_data.arg.fx.arg.explosion.mode_arg.gel.brand == 0 ? 1 : 0;
                                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.gel.series     = arg->dmx.dmx_data.arg.fx.arg.explosion.mode_arg.gel.type;
                                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.gel.color   = arg->dmx.dmx_data.arg.fx.arg.explosion.mode_arg.gel.color;
                                        break;
                                    case RS485_FX_Mode_Source:
                                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.source.intensity = arg->dmx.dmx_data.lightness; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_SOURCE].SOURCE.intensity ;
                                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.source.type = arg->dmx.dmx_data.arg.fx.arg.explosion.mode_arg.socue.type;
                                        break;
									case RS485_FX_Mode_RGB:
										p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.source.intensity = arg->dmx.dmx_data.lightness;
										p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.rgb.r = arg->dmx.dmx_data.arg.fx.arg.explosion.mode_arg.rgb.r_ratio; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_SOURCE].SOURCE.intensity ;
                                        p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.rgb.g = arg->dmx.dmx_data.arg.fx.arg.explosion.mode_arg.rgb.g_ratio;
										p_cmd_body->cmd_arg.sys_fx.arg.explosion.mode_arg.rgb.b = arg->dmx.dmx_data.arg.fx.arg.explosion.mode_arg.rgb.b_ratio;
										break;
                                    default:break;
                                }
                                p_cmd_body->cmd_arg.sys_fx.arg.explosion.frq = arg->dmx.dmx_data.arg.fx.arg.explosion.decay;
                                p_cmd_body->cmd_arg.sys_fx.arg.explosion.trigger = (uint8_t)arg->dmx.strobe_or_fx_state;
                            }
                            else
                            {
                                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
								explosion_strobe_or_fx_state = arg->dmx.strobe_or_fx_state;
                            }
							 break;
//							p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX_II;
//                            p_cmd_body->cmd_arg.sys_fx_2.type = RS485_FX_II_Explosion_2;
//							p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.intensity = arg->dmx.dmx_data.lightness;
//							p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode = (uint8_t)arg->dmx.dmx_data.arg.fx2.arg.explosion2.mode;
//							p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.decay  = arg->dmx.dmx_data.arg.fx2.arg.explosion2.decay;
//							p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.state =  arg->dmx.strobe_or_fx_state;
//							switch(p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode)
//							{
//								case RS485_FX_II_Mode_CCT:
//									p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.cct.cct = arg->dmx.dmx_data.arg.fx2.arg.explosion2.arg.cct.cct;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.cct.duv  = (uint8_t)arg->dmx.dmx_data.arg.fx2.arg.explosion2.arg.cct.duv ;
//								break;
//								case RS485_FX_II_Mode_HSI:
//									p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.hsi.cct = arg->dmx.dmx_data.arg.fx2.arg.explosion2.arg.hsi.cct;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.hsi.hue = arg->dmx.dmx_data.arg.fx2.arg.explosion2.arg.hsi.hue;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.hsi.sat = arg->dmx.dmx_data.arg.fx2.arg.explosion2.arg.hsi.sat;
//								break;
//								 case RS485_FX_II_Mode_XY:
//								
//									p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.xy.x  = arg->dmx.dmx_data.arg.fx2.arg.explosion2.arg.xy.x;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.xy.y = arg->dmx.dmx_data.arg.fx2.arg.explosion2.arg.xy.y;
//								break;
//								case RS485_FX_II_Mode_GEL:
//									p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.gel.cct = arg->dmx.dmx_data.arg.fx2.arg.explosion2.arg.gel.cct;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.gel.origin  = arg->dmx.dmx_data.arg.fx2.arg.explosion2.arg.gel.brand == 0 ? 1 : 0;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.gel.series     = arg->dmx.dmx_data.arg.fx2.arg.explosion2.arg.gel.type;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.gel.color   = arg->dmx.dmx_data.arg.fx2.arg.explosion2.arg.gel.color;
//								break;
//								case RS485_FX_II_Mode_SOUYRCE:
//								
//								
//									p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.source.type = arg->dmx.dmx_data.arg.fx2.arg.explosion2.arg.source.type;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.source.x_coord = arg->dmx.dmx_data.arg.fx2.arg.explosion2.arg.source.x;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.source.y_coord = arg->dmx.dmx_data.arg.fx2.arg.explosion2.arg.source.y;
//								break;
//								case RS485_FX_II_Mode_RGB:
//									p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.rgb.blue_ratio = arg->dmx.dmx_data.arg.fx2.arg.explosion2.arg.rgb.blue;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.rgb.green_ratio = arg->dmx.dmx_data.arg.fx2.arg.explosion2.arg.rgb.green;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.explosion_2.mode_arg.rgb.red_ratio = arg->dmx.dmx_data.arg.fx2.arg.explosion2.arg.rgb.red;
//								break;

//								default:break;
//							} 
//                            break;
                        case DMX_FX_STROBE:
							p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
                            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Strobe;
							explosion_strobe_or_fx_state = 0;
							lightning_strobe_or_fx_state = 0;
                            if( arg->dmx.strobe_or_fx_state > 0)
                            {
                                p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode = (uint8_t)arg->dmx.dmx_data.arg.fx.arg.strobe.mode;
                                switch((uint8_t)arg->dmx.dmx_data.arg.fx.arg.strobe.mode)
                                {
                                    case RS485_FX_Mode_CCT:
                                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.cct.intensity = arg->dmx.dmx_data.lightness; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_CCT].CCT.intensity;
										#if PROJECT_TYPE==308
										p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.cct.cct = g_tUIAllData.strobe_model.mode_arg.cct.cct;
										#elif PROJECT_TYPE==307
										p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.cct.cct = arg->dmx.dmx_data.arg.fx.arg.strobe.mode_arg.cct.cct;
										#endif
                                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.cct.gm  = (uint8_t)arg->dmx.dmx_data.arg.fx.arg.strobe.mode_arg.cct.duv ;
                                        break;
                                    case RS485_FX_Mode_HSI:
                                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.hsi.intensity  = arg->dmx.dmx_data.lightness ;//g_tUIAllData.strobe_model.setting[PARAM_SETTING_HSI].HSI.hsi.intensity;
                                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.hsi.hue = arg->dmx.dmx_data.arg.fx.arg.strobe.mode_arg.hsi.hue;
                                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.hsi.sat = arg->dmx.dmx_data.arg.fx.arg.strobe.mode_arg.hsi.sat;
                                        break;
                                    case RS485_FX_Mode_XY:
                                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.xy.intensity = arg->dmx.dmx_data.lightness; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_XY].XY.intensity;
                                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.xy.x  = arg->dmx.dmx_data.arg.fx.arg.strobe.mode_arg.xy.x;
                                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.xy.y = arg->dmx.dmx_data.arg.fx.arg.strobe.mode_arg.xy.y;
                                        break;
                                    case RS485_FX_Mode_GEL:
                                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.gel.intensity = arg->dmx.dmx_data.lightness; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_GEL].GEL.intensity;
                                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.gel.cct = arg->dmx.dmx_data.arg.fx.arg.strobe.mode_arg.gel.cct;
                                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.gel.origin  = arg->dmx.dmx_data.arg.fx.arg.strobe.mode_arg.gel.brand == 0 ? 1 : 0;
                                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.gel.series     = arg->dmx.dmx_data.arg.fx.arg.strobe.mode_arg.gel.type;
                                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.gel.color   = arg->dmx.dmx_data.arg.fx.arg.strobe.mode_arg.gel.color;
                                        break;
                                    case RS485_FX_Mode_Source:
                                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.source.intensity = arg->dmx.dmx_data.lightness; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_SOURCE].SOURCE.intensity ;
                                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.source.type = arg->dmx.dmx_data.arg.fx.arg.strobe.mode_arg.socue.type;
                                        break;
									case RS485_FX_Mode_RGB:
										p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.source.intensity = arg->dmx.dmx_data.lightness;
										p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.rgb.r = arg->dmx.dmx_data.arg.fx.arg.strobe.mode_arg.rgb.r_ratio; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_SOURCE].SOURCE.intensity ;
                                        p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.rgb.g = arg->dmx.dmx_data.arg.fx.arg.strobe.mode_arg.rgb.g_ratio;
										p_cmd_body->cmd_arg.sys_fx.arg.strobe.mode_arg.rgb.b = arg->dmx.dmx_data.arg.fx.arg.strobe.mode_arg.rgb.b_ratio;
										break;
                                    default:break;
                                }
                                p_cmd_body->cmd_arg.sys_fx.arg.strobe.frq = arg->dmx.dmx_data.arg.fx.arg.strobe.spd;
                            }
                            else
                            {
                                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
                            }
                            break;
//							p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX_II;
//                            p_cmd_body->cmd_arg.sys_fx_2.type = RS485_FX_II_Explosion_2;
//							p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.intensity = arg->dmx.dmx_data.lightness;
//							p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode = (uint8_t)arg->dmx.dmx_data.arg.fx2.arg.strobe2.mode;
//							p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.spd  = arg->dmx.dmx_data.arg.fx2.arg.strobe2.spd;
//							p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.state =  arg->dmx.strobe_or_fx_state;
//							switch(p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode)
//							{
//								case RS485_FX_II_Mode_CCT:
//									p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.cct.cct = arg->dmx.dmx_data.arg.fx2.arg.strobe2.arg.cct.cct;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.cct.duv  = (uint8_t)arg->dmx.dmx_data.arg.fx2.arg.strobe2.arg.cct.duv ;
//								break;
//								case RS485_FX_II_Mode_HSI:
//									p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.hsi.cct = arg->dmx.dmx_data.arg.fx2.arg.strobe2.arg.hsi.cct;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.hsi.hue = arg->dmx.dmx_data.arg.fx2.arg.strobe2.arg.hsi.hue;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.hsi.sat = arg->dmx.dmx_data.arg.fx2.arg.strobe2.arg.hsi.sat;
//								break;
//								 case RS485_FX_II_Mode_XY:
//								
//									p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.xy.x  = arg->dmx.dmx_data.arg.fx2.arg.strobe2.arg.xy.x;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.xy.y = arg->dmx.dmx_data.arg.fx2.arg.strobe2.arg.xy.y;
//								break;
//								case RS485_FX_II_Mode_GEL:
//									p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.gel.cct = arg->dmx.dmx_data.arg.fx2.arg.strobe2.arg.gel.cct;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.gel.origin  = arg->dmx.dmx_data.arg.fx2.arg.strobe2.arg.gel.brand == 0 ? 1 : 0;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.gel.series     = arg->dmx.dmx_data.arg.fx2.arg.strobe2.arg.gel.type;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.gel.color   = arg->dmx.dmx_data.arg.fx2.arg.strobe2.arg.gel.color;
//								break;
//								case RS485_FX_II_Mode_SOUYRCE:
//									p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.source.type = arg->dmx.dmx_data.arg.fx2.arg.strobe2.arg.source.type;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.source.x_coord = arg->dmx.dmx_data.arg.fx2.arg.strobe2.arg.source.x;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.source.y_coord = arg->dmx.dmx_data.arg.fx2.arg.strobe2.arg.source.y;
//								break;
//								case RS485_FX_II_Mode_RGB:
//									p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.rgb.blue_ratio = arg->dmx.dmx_data.arg.fx2.arg.strobe2.arg.rgb.blue;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.rgb.green_ratio = arg->dmx.dmx_data.arg.fx2.arg.strobe2.arg.rgb.green;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.strobe_2.mode_arg.rgb.red_ratio = arg->dmx.dmx_data.arg.fx2.arg.strobe2.arg.rgb.red;
//								break;

//								default:break;
//							}

                        case DMX_FX_WELDING:
                            p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX;
                            p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Welding;
							explosion_strobe_or_fx_state = 0;
							lightning_strobe_or_fx_state = 0;
                            if( arg->dmx.strobe_or_fx_state > 0)
                            {
                                p_cmd_body->cmd_arg.sys_fx.arg.welding.mode = (uint8_t)arg->dmx.dmx_data.arg.fx.arg.welding.mode;
                                switch((uint8_t)arg->dmx.dmx_data.arg.fx.arg.welding.mode)
                                {
                                    case RS485_FX_Mode_CCT:
                                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.cct.intensity = arg->dmx.dmx_data.lightness; //g_tUIAllData.welding_model.setting[PARAM_SETTING_CCT].CCT.intensity;
                                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.cct.cct = arg->dmx.dmx_data.arg.fx.arg.welding.mode_arg.cct.cct;
                                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.cct.gm  = (uint8_t)arg->dmx.dmx_data.arg.fx.arg.welding.mode_arg.cct.duv ;
                                        break;
                                    case RS485_FX_Mode_HSI:
                                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.hsi.intensity  = arg->dmx.dmx_data.lightness ;//g_tUIAllData.welding_model.setting[PARAM_SETTING_HSI].HSI.hsi.intensity;
                                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.hsi.hue = arg->dmx.dmx_data.arg.fx.arg.welding.mode_arg.hsi.hue;
                                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.hsi.sat = arg->dmx.dmx_data.arg.fx.arg.welding.mode_arg.hsi.sat;
                                        break;
                                    case RS485_FX_Mode_XY:
                                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.xy.intensity = arg->dmx.dmx_data.lightness; //g_tUIAllData.welding_model.setting[PARAM_SETTING_XY].XY.intensity;
                                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.xy.x  = arg->dmx.dmx_data.arg.fx.arg.welding.mode_arg.xy.x;
                                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.xy.y = arg->dmx.dmx_data.arg.fx.arg.welding.mode_arg.xy.y;
                                        break;
                                    case RS485_FX_Mode_GEL:
                                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.gel.intensity = arg->dmx.dmx_data.lightness; //g_tUIAllData.welding_model.setting[PARAM_SETTING_GEL].GEL.intensity;
                                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.gel.cct = arg->dmx.dmx_data.arg.fx.arg.welding.mode_arg.gel.cct;
                                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.gel.origin  = arg->dmx.dmx_data.arg.fx.arg.welding.mode_arg.gel.brand == 0 ? 1 : 0;
                                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.gel.series     = arg->dmx.dmx_data.arg.fx.arg.welding.mode_arg.gel.type;
                                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.gel.color   = arg->dmx.dmx_data.arg.fx.arg.welding.mode_arg.gel.color;
                                        break;
                                    case RS485_FX_Mode_Source:
                                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.source.intensity = arg->dmx.dmx_data.lightness; //g_tUIAllData.welding_model.setting[PARAM_SETTING_SOURCE].SOURCE.intensity ;
                                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.source.type = arg->dmx.dmx_data.arg.fx.arg.welding.mode_arg.socue.type;
                                        break;
									case RS485_FX_Mode_RGB:
										p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.source.intensity = arg->dmx.dmx_data.lightness;
										p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.rgb.r = arg->dmx.dmx_data.arg.fx.arg.welding.mode_arg.rgb.r_ratio; //g_tUIAllData.strobe_model.setting[PARAM_SETTING_SOURCE].SOURCE.intensity ;
                                        p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.rgb.g = arg->dmx.dmx_data.arg.fx.arg.welding.mode_arg.rgb.g_ratio;
										p_cmd_body->cmd_arg.sys_fx.arg.welding.mode_arg.rgb.b = arg->dmx.dmx_data.arg.fx.arg.welding.mode_arg.rgb.b_ratio;
										break;
                                    default:break;
                                }
                                p_cmd_body->cmd_arg.sys_fx.arg.welding.frq = arg->dmx.dmx_data.arg.fx.arg.welding.frq;
                                p_cmd_body->cmd_arg.sys_fx.arg.welding.min_int = arg->dmx.dmx_data.arg.fx.arg.welding.min_lightness;
                            }
                            else
                            {
                                p_cmd_body->cmd_arg.sys_fx.type = RS485_FX_Effect_Off;
                            }
                            break;
//							p_cmd_body->header.cmd_type = RS485_Cmd_Sys_FX_II;
//                            p_cmd_body->cmd_arg.sys_fx_2.type = RS485_FX_II_Welding_2;
//							p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.intensity = arg->dmx.dmx_data.lightness;
//							p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode = (uint8_t)arg->dmx.dmx_data.arg.fx2.arg.welding2.mode;
//							p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.frq  = arg->dmx.dmx_data.arg.fx2.arg.welding2.frq;
//							p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.min_intensity = arg->dmx.dmx_data.arg.fx2.arg.welding2.min_intensity;
//							p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.state =  arg->dmx.strobe_or_fx_state;
//							switch(p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode)
//							{
//								case RS485_FX_II_Mode_CCT:
//									p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.cct.cct = arg->dmx.dmx_data.arg.fx2.arg.welding2.arg.cct.cct;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.cct.duv  = (uint8_t)arg->dmx.dmx_data.arg.fx2.arg.welding2.arg.cct.duv ;
//								break;
//								case RS485_FX_II_Mode_HSI:
//									p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.hsi.cct = arg->dmx.dmx_data.arg.fx2.arg.welding2.arg.hsi.cct;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.hsi.hue = arg->dmx.dmx_data.arg.fx2.arg.welding2.arg.hsi.hue;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.hsi.sat = arg->dmx.dmx_data.arg.fx2.arg.welding2.arg.hsi.sat;
//								break;
//								 case RS485_FX_II_Mode_XY:
//								
//									p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.xy.x  = arg->dmx.dmx_data.arg.fx2.arg.welding2.arg.xy.x;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.xy.y = arg->dmx.dmx_data.arg.fx2.arg.welding2.arg.xy.y;
//								break;
//								case RS485_FX_II_Mode_GEL:
//									p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.gel.cct = arg->dmx.dmx_data.arg.fx2.arg.welding2.arg.gel.cct;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.gel.origin  = arg->dmx.dmx_data.arg.fx2.arg.welding2.arg.gel.brand == 0 ? 1 : 0;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.gel.series     = arg->dmx.dmx_data.arg.fx2.arg.welding2.arg.gel.type;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.gel.color   = arg->dmx.dmx_data.arg.fx2.arg.welding2.arg.gel.color;
//								break;
//								case RS485_FX_II_Mode_SOUYRCE:
//									p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.source.type = arg->dmx.dmx_data.arg.fx2.arg.welding2.arg.source.type;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.source.x_coord = arg->dmx.dmx_data.arg.fx2.arg.welding2.arg.source.x;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.source.y_coord = arg->dmx.dmx_data.arg.fx2.arg.welding2.arg.source.y;
//								break;
//								case RS485_FX_II_Mode_RGB:
//									p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.rgb.blue_ratio = arg->dmx.dmx_data.arg.fx2.arg.welding2.arg.rgb.blue;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.rgb.green_ratio = arg->dmx.dmx_data.arg.fx2.arg.welding2.arg.rgb.green;
//									p_cmd_body->cmd_arg.sys_fx_2.arg.welding_2.mode_arg.rgb.red_ratio = arg->dmx.dmx_data.arg.fx2.arg.welding2.arg.rgb.red;
//								break;

//								default:break;
//							}
//							break;
//                        default:break;
                    }
                break;
                case DMX_TYPE_FX_II:

                break;
                case DMX_TYPE_MIXING:
                    p_cmd_body->header.cmd_type = RS485_Cmd_Color_Mixing;
                    p_cmd_body->cmd_arg.color_mixing.fade =  dmx_fade_time;

                    if(arg->dmx.strobe_or_fx_state == 0 )
                    {
                        p_cmd_body->cmd_arg.color_mixing.color_1_ratio = arg->dmx.dmx_data.arg.mixing.ratio;

                        switch ((uint8_t)arg->dmx.dmx_data.arg.mixing.color1_arg.mode )
                        {
                            case DMX_ColorMixingMode_CCT :
                                p_cmd_body->cmd_arg.color_mixing.color_1_arg.mode = RS485_ColorMixingMode_CCT;
                                p_cmd_body->cmd_arg.color_mixing.color_1_arg.mode_arg.cct.intensity = arg->dmx.dmx_data.lightness / 1000.0f;
                                p_cmd_body->cmd_arg.color_mixing.color_1_arg.mode_arg.cct.cct = arg->dmx.dmx_data.arg.mixing.color1_arg.arg.cct.cct;
                                p_cmd_body->cmd_arg.color_mixing.color_1_arg.mode_arg.cct.duv = arg->dmx.dmx_data.arg.mixing.color1_arg.arg.cct.duv;
                                break;
                            case DMX_ColorMixingMode_HSI :
                                p_cmd_body->cmd_arg.color_mixing.color_1_arg.mode = RS485_ColorMixingMode_HSI;
                                p_cmd_body->cmd_arg.color_mixing.color_1_arg.mode_arg.hsi.intensity = arg->dmx.dmx_data.lightness / 1000.0f;
                                p_cmd_body->cmd_arg.color_mixing.color_1_arg.mode_arg.hsi.hue = arg->dmx.dmx_data.arg.mixing.color1_arg.arg.hsi.hue * 100;
                                p_cmd_body->cmd_arg.color_mixing.color_1_arg.mode_arg.hsi.sat = arg->dmx.dmx_data.arg.mixing.color1_arg.arg.hsi.sat * 100;
                                p_cmd_body->cmd_arg.color_mixing.color_1_arg.mode_arg.hsi.cct = arg->dmx.dmx_data.arg.mixing.color1_arg.arg.hsi.cct ;
                                break;
                            case DMX_ColorMixingMode_RGB :
                                p_cmd_body->cmd_arg.color_mixing.color_1_arg.mode = RS485_ColorMixingMode_CCT;
                                p_cmd_body->cmd_arg.color_mixing.color_1_arg.mode_arg.rgb.intensity = arg->dmx.dmx_data.lightness / 1000.0f;
                                p_cmd_body->cmd_arg.color_mixing.color_1_arg.mode_arg.rgb.red_ratio = arg->dmx.dmx_data.arg.mixing.color1_arg.arg.rgb.red*1000 ;
                                p_cmd_body->cmd_arg.color_mixing.color_1_arg.mode_arg.rgb.green_ratio =  arg->dmx.dmx_data.arg.mixing.color1_arg.arg.rgb.green*1000;
                                p_cmd_body->cmd_arg.color_mixing.color_1_arg.mode_arg.rgb.blue_ratio = arg->dmx.dmx_data.arg.mixing.color1_arg.arg.rgb.blue*1000;
                                break;
                        }
                        switch ((uint8_t)arg->dmx.dmx_data.arg.mixing.color2_arg.mode )
                        {
                            case DMX_ColorMixingMode_CCT :
                                p_cmd_body->cmd_arg.color_mixing.color_2_arg.mode = RS485_ColorMixingMode_CCT;
                                p_cmd_body->cmd_arg.color_mixing.color_2_arg.mode_arg.cct.intensity = arg->dmx.dmx_data.lightness / 1000.0f;
                                p_cmd_body->cmd_arg.color_mixing.color_2_arg.mode_arg.cct.cct = arg->dmx.dmx_data.arg.mixing.color2_arg.arg.cct.cct;
                                p_cmd_body->cmd_arg.color_mixing.color_2_arg.mode_arg.cct.duv = (uint8_t)arg->dmx.dmx_data.arg.mixing.color2_arg.arg.cct.duv;
                                break;
                            case DMX_ColorMixingMode_HSI :
                                p_cmd_body->cmd_arg.color_mixing.color_2_arg.mode = RS485_ColorMixingMode_HSI;
                                p_cmd_body->cmd_arg.color_mixing.color_2_arg.mode_arg.hsi.intensity = arg->dmx.dmx_data.lightness / 1000.0f;
                                p_cmd_body->cmd_arg.color_mixing.color_2_arg.mode_arg.hsi.hue = arg->dmx.dmx_data.arg.mixing.color2_arg.arg.hsi.hue * 100;
                                p_cmd_body->cmd_arg.color_mixing.color_2_arg.mode_arg.hsi.sat = arg->dmx.dmx_data.arg.mixing.color2_arg.arg.hsi.sat * 100;
                                p_cmd_body->cmd_arg.color_mixing.color_2_arg.mode_arg.hsi.cct = arg->dmx.dmx_data.arg.mixing.color2_arg.arg.hsi.cct ;
                                break;
                            case DMX_ColorMixingMode_RGB :
                                p_cmd_body->cmd_arg.color_mixing.color_2_arg.mode = RS485_ColorMixingMode_RGB;
                                p_cmd_body->cmd_arg.color_mixing.color_2_arg.mode_arg.rgb.intensity = arg->dmx.dmx_data.lightness / 1000.0f;
                                p_cmd_body->cmd_arg.color_mixing.color_2_arg.mode_arg.rgb.red_ratio = arg->dmx.dmx_data.arg.mixing.color2_arg.arg.rgb.red*1000 ;
                                p_cmd_body->cmd_arg.color_mixing.color_2_arg.mode_arg.rgb.green_ratio =  arg->dmx.dmx_data.arg.mixing.color2_arg.arg.rgb.green*1000;
                                p_cmd_body->cmd_arg.color_mixing.color_2_arg.mode_arg.rgb.blue_ratio = arg->dmx.dmx_data.arg.mixing.color2_arg.arg.rgb.blue*1000;
                                break;
                        }
                    }
                    else
                    {
						p_cmd_body->header.cmd_type = RS485_Cmd_DMX_Strobe;
					
                        switch ((uint8_t)arg->dmx.dmx_data.arg.mixing.color1_arg.mode )
                        {
                            case DMX_ColorMixingMode_CCT :
                                p_cmd_body->cmd_arg.dmx_strobe.frq = arg->dmx.strobe_or_fx_state;
                                p_cmd_body->cmd_arg.dmx_strobe.mode = RS485_DMX_Strobe_Mode_Color_Mixing;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_1_mode = RS485_DMX_Strobe_Mode_CCT;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_1_arg.cct.intensity  = arg->dmx.dmx_data.lightness / 1000.0f;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_1_arg.cct.cct  = arg->dmx.dmx_data.arg.mixing.color1_arg.arg.cct.cct;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_1_arg.cct.duv  = arg->dmx.dmx_data.arg.mixing.color1_arg.arg.cct.duv;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_1_ratio = arg->dmx.dmx_data.arg.mixing.ratio;
                                break;
                            case DMX_ColorMixingMode_HSI :
                                p_cmd_body->cmd_arg.dmx_strobe.frq = arg->dmx.strobe_or_fx_state;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_1_mode = RS485_DMX_Strobe_Mode_HSI;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_1_arg.hsi.intensity = arg->dmx.dmx_data.lightness / 1000.0f;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_1_arg.hsi.hue = arg->dmx.dmx_data.arg.mixing.color1_arg.arg.hsi.hue * 100;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_1_arg.hsi.sat = arg->dmx.dmx_data.arg.mixing.color1_arg.arg.hsi.sat * 100;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_1_arg.hsi.cct = arg->dmx.dmx_data.arg.mixing.color1_arg.arg.hsi.cct;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_1_ratio = arg->dmx.dmx_data.arg.mixing.ratio;
                                break;

                            case DMX_ColorMixingMode_RGB :
                                p_cmd_body->cmd_arg.dmx_strobe.frq = arg->dmx.strobe_or_fx_state;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_1_mode = RS485_DMX_Strobe_Mode_RGB;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_1_arg.rgb.intensity = arg->dmx.dmx_data.lightness / 1000.0f;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_1_arg.rgb.blue_ratio = arg->dmx.dmx_data.arg.mixing.color1_arg.arg.rgb.blue*1000;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_1_arg.rgb.green_ratio = arg->dmx.dmx_data.arg.mixing.color1_arg.arg.rgb.green*1000;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_1_arg.rgb.red_ratio = arg->dmx.dmx_data.arg.mixing.color1_arg.arg.rgb.red*1000;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_1_ratio = arg->dmx.dmx_data.arg.mixing.ratio;
                                break;
                        }
                        switch ((uint8_t)arg->dmx.dmx_data.arg.mixing.color2_arg.mode )
                        {
                            case DMX_ColorMixingMode_CCT :

                                p_cmd_body->cmd_arg.dmx_strobe.frq = arg->dmx.strobe_or_fx_state;
                                p_cmd_body->cmd_arg.dmx_strobe.mode = RS485_DMX_Strobe_Mode_Color_Mixing;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_2_mode = RS485_DMX_Strobe_Mode_CCT;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_2_arg.cct.intensity  = arg->dmx.dmx_data.lightness / 1000.0f;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_2_arg.cct.cct  = arg->dmx.dmx_data.arg.mixing.color2_arg.arg.cct.cct;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_2_arg.cct.duv  = arg->dmx.dmx_data.arg.mixing.color2_arg.arg.cct.duv;

                                break;

                            case DMX_ColorMixingMode_HSI :
                                p_cmd_body->cmd_arg.dmx_strobe.frq = arg->dmx.strobe_or_fx_state;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_2_mode = RS485_DMX_Strobe_Mode_HSI;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_2_arg.hsi.intensity = arg->dmx.dmx_data.lightness / 1000.0f;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_2_arg.hsi.hue = arg->dmx.dmx_data.arg.mixing.color2_arg.arg.hsi.hue * 100;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_2_arg.hsi.sat = arg->dmx.dmx_data.arg.mixing.color2_arg.arg.hsi.sat * 100;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_2_arg.hsi.cct = arg->dmx.dmx_data.arg.mixing.color2_arg.arg.hsi.cct;

                                break;

                            case DMX_ColorMixingMode_RGB :
                                p_cmd_body->cmd_arg.dmx_strobe.frq = arg->dmx.strobe_or_fx_state;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_2_mode = RS485_DMX_Strobe_Mode_RGB;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_2_arg.rgb.intensity = arg->dmx.dmx_data.lightness / 1000.0f;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_2_arg.rgb.blue_ratio = arg->dmx.dmx_data.arg.mixing.color2_arg.arg.rgb.blue*1000;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_2_arg.rgb.green_ratio = arg->dmx.dmx_data.arg.mixing.color2_arg.arg.rgb.green*1000;
                                p_cmd_body->cmd_arg.dmx_strobe.mode_arg.color_mixing.color_2_arg.rgb.red_ratio = arg->dmx.dmx_data.arg.mixing.color2_arg.arg.rgb.red*1000;

                                break;
                        }
                    }
                break;
                case DMX_TYPE_XY:
                    if(arg->dmx.strobe_or_fx_state == 0 )
                    {
						
                        p_cmd_body->header.cmd_type = RS485_Cmd_XY_Coordinate;
                        p_cmd_body->cmd_arg.xy_coord.intensity  = arg->dmx.dmx_data.lightness / 1000.0f;
                        p_cmd_body->cmd_arg.xy_coord.x  = arg->dmx.dmx_data.arg.xy.x*10000;
                        p_cmd_body->cmd_arg.xy_coord.y  = arg->dmx.dmx_data.arg.xy.y*10000;
                        p_cmd_body->cmd_arg.xy_coord.fade =  dmx_fade_time;
                    }
                    else
                    {
						p_cmd_body->cmd_arg.dmx_strobe.frq = arg->dmx.strobe_or_fx_state;
						p_cmd_body->header.cmd_type = RS485_Cmd_DMX_Strobe;
                        p_cmd_body->cmd_arg.dmx_strobe.mode = RS485_DMX_Strobe_Mode_XY;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.xy.intensity = arg->dmx.dmx_data.lightness / 1000.0f;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.xy.x = arg->dmx.dmx_data.arg.xy.x*10000;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.xy.y = arg->dmx.dmx_data.arg.xy.y*10000;

                    }
                    break;
                case DMX_TYPE_SOUCE:
                    if(arg->dmx.strobe_or_fx_state == 0 )
                    {
                        p_cmd_body->header.cmd_type = RS485_Cmd_Source;
                        p_cmd_body->cmd_arg.source.intensity   = arg->dmx.dmx_data.lightness / 1000.0f;
                        p_cmd_body->cmd_arg.source.type  =  arg->dmx.dmx_data.arg.souce.type;
                        p_cmd_body->cmd_arg.source.x_coord  = arg->dmx.dmx_data.arg.souce.x;
                        p_cmd_body->cmd_arg.source.y_coord  = arg->dmx.dmx_data.arg.souce.y;
                        p_cmd_body->cmd_arg.source.fade   =  dmx_fade_time;
                    }
                    else
                    {
						p_cmd_body->cmd_arg.dmx_strobe.frq = arg->dmx.strobe_or_fx_state;
						p_cmd_body->header.cmd_type = RS485_Cmd_DMX_Strobe;
                        p_cmd_body->cmd_arg.dmx_strobe.mode = RS485_DMX_Strobe_Mode_SOUYRCE;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.source.intensity = arg->dmx.dmx_data.lightness / 1000.0f;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.source.x_coord = arg->dmx.dmx_data.arg.souce.x;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.source.y_coord = arg->dmx.dmx_data.arg.souce.y;
                        p_cmd_body->cmd_arg.dmx_strobe.mode_arg.source.type = arg->dmx.dmx_data.arg.souce.type;
                    }
                    break;
                default:break;
            }
        }
        break;
        case  LIGHT_MODE_SIDUS_PFX:
            switch(arg->pfx.mode)
            {
                case SIDUS_PFX_FLASH:
                {
                    p_cmd_body->header.cmd_type = RS485_Cmd_PFX_Ctrl;
                    p_cmd_body->cmd_arg.pfx_ctrl.pfx_type = SIDUS_PFX_FLASH;
                    p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.flash.base = arg->pfx.mode_arg.flash.base;
                    p_cmd_body->cmd_arg.pfx_ctrl.light_type = 0;  //0x00:PFX_Flash  0x01:PFX_ColorChase  0x02:PFX_Continue

                    switch(arg->pfx.mode_arg.flash.base)
                    {
                        case SIDUS_FX_BASE_CCT:
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.flash.base_arg.cct.cct = arg->pfx.mode_arg.flash.base_arg.cct.cct;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.flash.base_arg.cct.frq = arg->pfx.mode_arg.flash.fx_arg.frq;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.flash.base_arg.cct.gm = (uint8_t)arg->pfx.mode_arg.flash.base_arg.cct.gm;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.flash.base_arg.cct.intensity = arg->pfx.mode_arg.flash.base_arg.cct.lightness;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.flash.base_arg.cct.times = arg->pfx.mode_arg.flash.fx_arg.times;
                            break;
                        case SIDUS_FX_BASE_HSI:
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.flash.base_arg.hsi.times = arg->pfx.mode_arg.flash.fx_arg.times;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.flash.base_arg.hsi.frq = arg->pfx.mode_arg.flash.fx_arg.frq;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.flash.base_arg.hsi.intensity = arg->pfx.mode_arg.flash.base_arg.hsi.lightness;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.flash.base_arg.hsi.hue = arg->pfx.mode_arg.flash.base_arg.hsi.hue;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.flash.base_arg.hsi.sat = arg->pfx.mode_arg.flash.base_arg.hsi.sat;
                            break;
                        default:break;
                    }
                }
                break;

                case SIDUS_PFX_Chase:
                {
                    p_cmd_body->header.cmd_type = RS485_Cmd_PFX_Ctrl;
                    p_cmd_body->cmd_arg.pfx_ctrl.pfx_type = SIDUS_PFX_Chase;  //0x00:PFX_Flash  0x01:PFX_ColorChase  0x02:PFX_Continue
                    p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base =  arg->pfx.mode_arg.chase.base;
                    p_cmd_body->cmd_arg.pfx_ctrl.light_type = 0 ;

                    // TxPacket->CommandBody.PFX_Ctrl_Body.Light_Type = user_data_str.Local_SidusFX_Data.SFX_Arg.PFX.Light_Type; /*????, 0x00:normal  0x01:tube  0x01:pixel mapping*/
                    switch(arg->pfx.mode_arg.chase.base)
                    {

                        case SIDUS_FX_BASE_CCT:
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base = arg->pfx.mode_arg.chase.base;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base_arg.cct.loop = arg->pfx.mode_arg.chase.fx_arg.loop;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base_arg.cct.times = arg->pfx.mode_arg.chase.fx_arg.time;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base_arg.cct.cct_seq = arg->pfx.mode_arg.chase.base_arg.cct_range.cct_seq;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base_arg.cct.int_seq = arg->pfx.mode_arg.chase.base_arg.cct_range.int_seq;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base_arg.cct.gm_seq = arg->pfx.mode_arg.chase.base_arg.cct_range.gm_seq;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base_arg.cct.max_cct = arg->pfx.mode_arg.chase.base_arg.cct_range.cct_max;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base_arg.cct.min_cct = arg->pfx.mode_arg.chase.base_arg.cct_range.cct_mini;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base_arg.cct.max_intensity = arg->pfx.mode_arg.chase.base_arg.cct_range.int_max;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base_arg.cct.min_intensity = arg->pfx.mode_arg.chase.base_arg.cct_range.int_mimi;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base_arg.cct.max_gm = arg->pfx.mode_arg.chase.base_arg.cct_range.gm_max;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base_arg.cct.min_gm = arg->pfx.mode_arg.chase.base_arg.cct_range.gm_mini;
                            break;
                        case SIDUS_FX_BASE_HSI:
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base = arg->pfx.mode_arg.chase.base;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base_arg.hsi.loop = arg->pfx.mode_arg.chase.fx_arg.loop;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base_arg.hsi.times = arg->pfx.mode_arg.chase.fx_arg.time;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base_arg.hsi.hue_seq = arg->pfx.mode_arg.chase.base_arg.hsi_range.hue_seq;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base_arg.hsi.int_seq = arg->pfx.mode_arg.chase.base_arg.hsi_range.int_seq;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base_arg.hsi.sat_Seq = arg->pfx.mode_arg.chase.base_arg.hsi_range.sat_seq;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base_arg.hsi.max_hue = arg->pfx.mode_arg.chase.base_arg.hsi_range.hue_max;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base_arg.hsi.min_hue = arg->pfx.mode_arg.chase.base_arg.hsi_range.hue_mini;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base_arg.hsi.max_intensity = arg->pfx.mode_arg.chase.base_arg.hsi_range.int_max;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base_arg.hsi.min_intensity = arg->pfx.mode_arg.chase.base_arg.hsi_range.int_mimi;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base_arg.hsi.max_sat = arg->pfx.mode_arg.chase.base_arg.hsi_range.sat_max;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.chase.base_arg.hsi.min_sat = arg->pfx.mode_arg.chase.base_arg.hsi_range.sat_mini;
                            break;
                        default:break;
                    }
                }
                break;
                case SIDUS_PFX_Continue:
                {
                    p_cmd_body->header.cmd_type = RS485_Cmd_PFX_Ctrl;
                    p_cmd_body->cmd_arg.pfx_ctrl.pfx_type = SIDUS_PFX_Continue;  //0x00:PFX_Flash  0x01:PFX_ColorChase  0x02:PFX_Continue
                    p_cmd_body->cmd_arg.pfx_ctrl.light_type = 0 ; /*????, 0x00:normal  0x01:tube  0x01:pixel mapping*/

                    switch(arg->pfx.mode_arg.chase.base)
                    {
                        case SIDUS_FX_BASE_CCT:
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.continues.base = arg->pfx.mode_arg.continues.base;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.continues.base_arg.cct.fade_in_time = arg->pfx.mode_arg.continues.fx_arg.fade_in_time;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.continues.base_arg.cct.fade_in_curve = arg->pfx.mode_arg.continues.fx_arg.fade_incurve;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.continues.base_arg.cct.fade_out_time = arg->pfx.mode_arg.continues.fx_arg.fade_out_time;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.continues.base_arg.cct.fade_out_curve = arg->pfx.mode_arg.continues.fx_arg.fade_out_curve;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.continues.base_arg.cct.continue_time = arg->pfx.mode_arg.continues.fx_arg.continue_time;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.continues.base_arg.cct.intensity = arg->pfx.mode_arg.continues.base_arg.cct.lightness;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.continues.base_arg.cct.cct = arg->pfx.mode_arg.continues.base_arg.cct.cct;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.continues.base_arg.cct.gm = (uint8_t)arg->pfx.mode_arg.continues.base_arg.cct.gm;
                            break;
                        case SIDUS_FX_BASE_HSI:
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.continues.base = arg->pfx.mode_arg.chase.base;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.continues.base_arg.hsi.fade_in_time = arg->pfx.mode_arg.continues.fx_arg.fade_in_time;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.continues.base_arg.hsi.fade_in_curve = arg->pfx.mode_arg.continues.fx_arg.fade_incurve;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.continues.base_arg.hsi.fade_out_time = arg->pfx.mode_arg.continues.fx_arg.fade_out_time;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.continues.base_arg.hsi.fade_out_curve = arg->pfx.mode_arg.continues.fx_arg.fade_out_curve;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.continues.base_arg.hsi.continue_time = arg->pfx.mode_arg.continues.fx_arg.continue_time;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.continues.base_arg.hsi.intensity = arg->pfx.mode_arg.continues.base_arg.hsi.lightness;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.continues.base_arg.hsi.hue = arg->pfx.mode_arg.continues.base_arg.hsi.hue;
                            p_cmd_body->cmd_arg.pfx_ctrl.pfx_arg.continues.base_arg.hsi.sat = arg->pfx.mode_arg.continues.base_arg.hsi.sat;
                            break;
                        default:break;
                    }
                }
                break;
            }
            break;
        case LIGHT_MODE_SIDUS_MFX:
        {
            p_cmd_body->header.cmd_type = RS485_Cmd_MFX_Ctrl;
            p_cmd_body->cmd_arg.mfx_ctrl.state = arg->mfx.ctrl;

            switch( arg->mfx.base)
            {
                case SIDUS_FX_BASE_CCT:
                    p_cmd_body->cmd_arg.mfx_ctrl.base_arg.base = SIDUS_FX_BASE_CCT;
                    p_cmd_body->cmd_arg.mfx_ctrl.base_arg.base_arg.cct.min_intensity = arg->mfx.base_arg.cct_range.int_mimi;
                    p_cmd_body->cmd_arg.mfx_ctrl.base_arg.base_arg.cct.max_intensity = arg->mfx.base_arg.cct_range.int_max;
                    p_cmd_body->cmd_arg.mfx_ctrl.base_arg.base_arg.cct.int_seq = arg->mfx.base_arg.cct_range.int_seq;
                    p_cmd_body->cmd_arg.mfx_ctrl.base_arg.base_arg.cct.min_cct = arg->mfx.base_arg.cct_range.cct_mini;
                    p_cmd_body->cmd_arg.mfx_ctrl.base_arg.base_arg.cct.max_cct = arg->mfx.base_arg.cct_range.cct_max;
                    p_cmd_body->cmd_arg.mfx_ctrl.base_arg.base_arg.cct.cct_seq = arg->mfx.base_arg.cct_range.cct_seq;
                    p_cmd_body->cmd_arg.mfx_ctrl.base_arg.base_arg.cct.min_gm = arg->mfx.base_arg.cct_range.gm_mini;
                    p_cmd_body->cmd_arg.mfx_ctrl.base_arg.base_arg.cct.max_gm = arg->mfx.base_arg.cct_range.gm_max;
                    p_cmd_body->cmd_arg.mfx_ctrl.base_arg.base_arg.cct.gm_seq = arg->mfx.base_arg.cct_range.gm_seq;
                    break;
                case SIDUS_FX_BASE_HSI:
                    p_cmd_body->cmd_arg.mfx_ctrl.base_arg.base = SIDUS_FX_BASE_HSI;
                    p_cmd_body->cmd_arg.mfx_ctrl.base_arg.base_arg.hsi.min_intensity = arg->mfx.base_arg.hsi_range.int_mimi;
                    p_cmd_body->cmd_arg.mfx_ctrl.base_arg.base_arg.hsi.max_intensity =  arg->mfx.base_arg.hsi_range.int_max;
                    p_cmd_body->cmd_arg.mfx_ctrl.base_arg.base_arg.hsi.int_seq = arg->mfx.base_arg.hsi_range.int_seq;
                    p_cmd_body->cmd_arg.mfx_ctrl.base_arg.base_arg.hsi.min_hue = arg->mfx.base_arg.hsi_range.hue_mini;
                    p_cmd_body->cmd_arg.mfx_ctrl.base_arg.base_arg.hsi.max_hue = arg->mfx.base_arg.hsi_range.hue_max;
                    p_cmd_body->cmd_arg.mfx_ctrl.base_arg.base_arg.hsi.hue_seq = arg->mfx.base_arg.hsi_range.hue_seq;
                    p_cmd_body->cmd_arg.mfx_ctrl.base_arg.base_arg.hsi.min_sat = arg->mfx.base_arg.hsi_range.sat_mini;
                    p_cmd_body->cmd_arg.mfx_ctrl.base_arg.base_arg.hsi.max_sat = arg->mfx.base_arg.hsi_range.sat_max;
                    p_cmd_body->cmd_arg.mfx_ctrl.base_arg.base_arg.hsi.sat_seq = arg->mfx.base_arg.hsi_range.sat_seq;
                    break;
            }
            switch (arg->mfx.fx_arg.mode)
            {
                case SIDUS_MFX_FLASH:
                {
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.effect_mode = SIDUS_MFX_FLASH;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.flash.free_time_seq = arg->mfx.fx_arg.mode_arg.flash.free_time_seq;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.flash.max_free_time = arg->mfx.fx_arg.mode_arg.flash.free_time_max;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.flash.min_free_time = arg->mfx.fx_arg.mode_arg.flash.free_time_mini ;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.flash.cycle_time_seq = arg->mfx.fx_arg.mode_arg.flash.cycle_time_seq;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.flash.max_cycle_time = arg->mfx.fx_arg.mode_arg.flash.cycle_time_max;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.flash.min_cycle_time = arg->mfx.fx_arg.mode_arg.flash.cycle_time_mini;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.flash.loop_times = arg->mfx.fx_arg.mode_arg.flash.loop_times;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.flash.loop_mode = arg->mfx.fx_arg.mode_arg.flash.loop_mode;
                }
                break;
                case SIDUS_MFX_CONTINUE:
                {
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.effect_mode = SIDUS_MFX_CONTINUE;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.continues.fade_in_curve = arg->mfx.fx_arg.mode_arg.continues.fade_in_curve;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.continues.max_fade_in_time = arg->mfx.fx_arg.mode_arg.continues.fade_in_time_max;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.continues.min_fade_in_time = arg->mfx.fx_arg.mode_arg.continues.fade_in_time_mini;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.continues.fade_in_time_seq = arg->mfx.fx_arg.mode_arg.continues.fade_in_time_seq;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.continues.cycle_time_seq = arg->mfx.fx_arg.mode_arg.continues.cycle_time_seq;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.continues.max_cycle_time = arg->mfx.fx_arg.mode_arg.continues.cycle_time_max;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.continues.min_cycle_time = arg->mfx.fx_arg.mode_arg.continues.cycle_time_mini;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.continues.loop_times = arg->mfx.fx_arg.mode_arg.continues.loop_times;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.continues.loop_mode = arg->mfx.fx_arg.mode_arg.continues.loop_mode;
                }
                break;
                case SIDUS_MFX_PARAGRAPH:
                {
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.effect_mode = SIDUS_MFX_PARAGRAPH;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.unit_time_seq =  arg->mfx.fx_arg.mode_arg.paragraph.unit_time_seq;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.free_time_seq = arg->mfx.fx_arg.mode_arg.paragraph.free_time_seq;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.over_lap_seq = arg->mfx.fx_arg.mode_arg.paragraph.overlap_seq;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.olr_seq = arg->mfx.fx_arg.mode_arg.paragraph.olp_seq;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.max_olr = arg->mfx.fx_arg.mode_arg.paragraph.olr_max;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.min_olr = arg->mfx.fx_arg.mode_arg.paragraph.olr_mini;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.cycle_time_seq = arg->mfx.fx_arg.mode_arg.paragraph.cycle_time_seq;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.max_cycle_time = arg->mfx.fx_arg.mode_arg.paragraph.cycle_time_max;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.min_cycle_time = arg->mfx.fx_arg.mode_arg.paragraph.cycle_time_mini;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.loop_times = arg->mfx.fx_arg.mode_arg.paragraph.loop_times;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.loop_mode = arg->mfx.fx_arg.mode_arg.paragraph.loop_mode;
                }
                break;
                default:break;
            }
            switch(arg->mfx.fx_arg.mode)
            {
                case SIDUS_MFX_FLASH:
                {
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.effect_mode = SIDUS_MFX_FLASH;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.flash.frq_Seq =  arg->mfx.fx_arg.mode_arg.flash.frq_seq;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.flash.max_frq =  arg->mfx.fx_arg.mode_arg.flash.frq_max;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.flash.min_frq =  arg->mfx.fx_arg.mode_arg.flash.frq_mini;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.flash.unit_time_seq =  arg->mfx.fx_arg.mode_arg.flash.unit_time_seq;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.flash.max_unit_uime =  arg->mfx.fx_arg.mode_arg.flash.unit_time_max;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.flash.min_unit_uime =  arg->mfx.fx_arg.mode_arg.flash.unit_time_mini;
                }
                break;
                case SIDUS_MFX_CONTINUE:
                {
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.effect_mode = SIDUS_MFX_CONTINUE;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.continues.flash_frq = arg->mfx.fx_arg.mode_arg.continues.flicker_frq;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.continues.fade_out_curve = arg->mfx.fx_arg.mode_arg.continues.fade_out_curve;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.continues.fade_out_time_seq = arg->mfx.fx_arg.mode_arg.continues.fade_out_time_seq;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.continues.max_fade_out_time = arg->mfx.fx_arg.mode_arg.continues.fade_out_time_max;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.continues.min_fade_out_time = arg->mfx.fx_arg.mode_arg.continues.fade_out_time_mini;
                }
                break;
                case SIDUS_MFX_PARAGRAPH:
                {
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.effect_mode = SIDUS_MFX_PARAGRAPH;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.olp_seq = arg->mfx.fx_arg.mode_arg.paragraph.olp_seq;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.max_olp = arg->mfx.fx_arg.mode_arg.paragraph.olp_max;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.min_olp = arg->mfx.fx_arg.mode_arg.paragraph.olp_mini;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.max_unit_uime = arg->mfx.fx_arg.mode_arg.paragraph.unit_time_max;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.min_unit_uime = arg->mfx.fx_arg.mode_arg.paragraph.unit_time_mini;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.max_free_time = arg->mfx.fx_arg.mode_arg.paragraph.free_time_max;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.min_free_time = arg->mfx.fx_arg.mode_arg.paragraph.free_time_mini;
                }
                break;
                default:break;
            }
            switch(arg->mfx.fx_arg.mode)
            {
                case SIDUS_MFX_FLASH:
                    
                    break;
                case SIDUS_MFX_CONTINUE:
                    
                    break;
                case SIDUS_MFX_PARAGRAPH:
                {
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.effect_mode = SIDUS_MFX_PARAGRAPH;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.flash_frq = arg->mfx.fx_arg.mode_arg.paragraph.flicker_frq;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.fade_out_curve = arg->mfx.fx_arg.mode_arg.paragraph.fade_out_curve;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.fade_out_time_seq = arg->mfx.fx_arg.mode_arg.paragraph.fade_out_time_seq;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.max_fade_out_time = arg->mfx.fx_arg.mode_arg.paragraph.fade_out_time_max;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.min_fade_out_time = arg->mfx.fx_arg.mode_arg.paragraph.fade_out_time_mini;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.fade_in_curve = arg->mfx.fx_arg.mode_arg.paragraph.fade_in_curve;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.fade_in_time_seq = arg->mfx.fx_arg.mode_arg.paragraph.fade_in_time_seq;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.max_fade_in_time = arg->mfx.fx_arg.mode_arg.paragraph.fade_in_time_max;
                    p_cmd_body->cmd_arg.mfx_ctrl.mode_arg.mode.paragraph.min_fade_in_time = arg->mfx.fx_arg.mode_arg.paragraph.fade_in_time_mini;
                }
                break;
                default:break;
            }
        } 
        break;
        case LIGHT_MODE_SIDUS_CFX:
        {
            p_cmd_body->header.cmd_type = RS485_Cmd_CFX_Ctrl;
            p_cmd_body->cmd_arg.cfx_ctrl.effect_type = arg->cfx.type;
            p_cmd_body->cmd_arg.cfx_ctrl.bank = arg->cfx.bank;
            p_cmd_body->cmd_arg.cfx_ctrl.intensity = arg->cfx.lightness;
            p_cmd_body->cmd_arg.cfx_ctrl.spd = arg->cfx.speed;
            p_cmd_body->cmd_arg.cfx_ctrl.chaos = arg->cfx.chaos;
            p_cmd_body->cmd_arg.cfx_ctrl.sequence = arg->cfx.sequence;
            p_cmd_body->cmd_arg.cfx_ctrl.loop = arg->cfx.loop;
            p_cmd_body->cmd_arg.cfx_ctrl.state = arg->cfx.ctrl;
        }
        break;
        case LIGHT_MODE_SIDUS_CFX_PREVIEW: /*????*/
        {
            p_cmd_body->header.cmd_type = RS485_Cmd_CFX_Preview;

            switch(arg->cfx_preview.frame_1.base)
            {
                case SIDUS_FX_BASE_CCT:
                {
                    p_cmd_body->cmd_arg.cfx_preview.frame_1.base = SIDUS_FX_BASE_CCT;
                    p_cmd_body->cmd_arg.cfx_preview.frame_1.base_arg.cct.intensity = arg->cfx_preview.frame_1.base_arg.cct.lightness;
                    p_cmd_body->cmd_arg.cfx_preview.frame_1.base_arg.cct.cct = arg->cfx_preview.frame_1.base_arg.cct.cct;
                    p_cmd_body->cmd_arg.cfx_preview.frame_1.base_arg.cct.gm = (uint8_t)arg->cfx_preview.frame_1.base_arg.cct.gm;
                }
                break;
                case SIDUS_FX_BASE_HSI:
                {
                    p_cmd_body->cmd_arg.cfx_preview.frame_1.base = SIDUS_FX_BASE_HSI;
                    p_cmd_body->cmd_arg.cfx_preview.frame_1.base_arg.hsi.intensity = arg->cfx_preview.frame_1.base_arg.hsi.lightness;
                    p_cmd_body->cmd_arg.cfx_preview.frame_1.base_arg.hsi.hue = arg->cfx_preview.frame_1.base_arg.hsi.hue;
                    p_cmd_body->cmd_arg.cfx_preview.frame_1.base_arg.hsi.sat = arg->cfx_preview.frame_1.base_arg.hsi.sat;
                }
                break;
                default:break;
            }
            switch(arg->cfx_preview.frame_2.base)
            {
                case SIDUS_FX_BASE_CCT:
                {
                    p_cmd_body->cmd_arg.cfx_preview.frame_2.base = SIDUS_FX_BASE_CCT;
                    p_cmd_body->cmd_arg.cfx_preview.frame_2.base_arg.cct.intensity = arg->cfx_preview.frame_2.base_arg.cct.lightness;
                    p_cmd_body->cmd_arg.cfx_preview.frame_2.base_arg.cct.cct = arg->cfx_preview.frame_2.base_arg.cct.cct;
                    p_cmd_body->cmd_arg.cfx_preview.frame_2.base_arg.cct.gm = (uint8_t)arg->cfx_preview.frame_2.base_arg.cct.gm;
                }
                break;
                case SIDUS_FX_BASE_HSI:
                {
                    p_cmd_body->cmd_arg.cfx_preview.frame_2.base = SIDUS_FX_BASE_HSI;
                    p_cmd_body->cmd_arg.cfx_preview.frame_2.base_arg.hsi.intensity = arg->cfx_preview.frame_2.base_arg.hsi.lightness;
                    p_cmd_body->cmd_arg.cfx_preview.frame_2.base_arg.hsi.hue = arg->cfx_preview.frame_2.base_arg.hsi.hue;
                    p_cmd_body->cmd_arg.cfx_preview.frame_2.base_arg.hsi.sat = arg->cfx_preview.frame_2.base_arg.hsi.sat;
                }
                break;
                default:break;
            }
        }
        break;
        case LIGHT_MODE_LOCAL_CFX:
        {
            p_cmd_body->header.cmd_type = RS485_Cmd_CFX_Name;
            struct sys_info_cfx_name cfx_name_info;
            
            cfx_name_info.cfx_type = arg->local_cfx_arg.type;
            for(uint8_t i = 0; i < 10; i++)
            {
                switch(arg->local_cfx_arg.type)
                {
                    case 0:
                        rs485_get_cfx_name(&dev_lamp, true, 3, 1000, 0, i, (char *)&cfx_name_info.cfx_name[arg->local_cfx_arg.type].name[i]);
                    break;
                    case 2:
                        rs485_get_cfx_name(&dev_lamp, true, 3, 1000, 2, i, (char *)&cfx_name_info.cfx_name[arg->local_cfx_arg.type].name[i]);
                    break;
                    default:break;
                }
            }
            data_center_write_sys_info(SYS_INFO_CFX_NAME, &cfx_name_info);
        }
        break;
        default:break;
    }
}