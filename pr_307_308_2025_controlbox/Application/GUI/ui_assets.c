#include "lvgl/lvgl.h"
#include "base_type.h"
#include "ui_data.h"
#include "ui_assets.h"


const lv_img_dsc_t* title_get_fan_img(uint8_t fan_mode)
{
    LV_IMG_DECLARE(ImgFanModeHigh_CH)
    LV_IMG_DECLARE(ImgFanModeSilent_CH)
    LV_IMG_DECLARE(ImgFanModeMedium_CH)
    LV_IMG_DECLARE(ImgFanModeSmart_CH)
    LV_IMG_DECLARE(ImgFanModeHigh_EN)
    LV_IMG_DECLARE(ImgFanModeSilent_EN)
    LV_IMG_DECLARE(ImgFanModeMedium_EN)
    LV_IMG_DECLARE(ImgFanModeSmart_EN)

    static const lv_img_dsc_t* img_dsc_fan[][2] = {
        [FAN_MODE_SMART] = {&ImgFanModeSmart_EN, &ImgFanModeSmart_CH},
        [FAN_MODE_HIGH] = {&ImgFanModeHigh_EN, &ImgFanModeHigh_CH}, 
        [FAN_MODE_MEDIUM] = {&ImgFanModeMedium_EN, &ImgFanModeMedium_CH},
        [FAN_MODE_SILENT] =  {&ImgFanModeSilent_EN, &ImgFanModeSilent_CH},
    };

    uint8_t lang = ui_get_lang_type();

    if (fan_mode >= FAN_MODE_MAX) {
        fan_mode = FAN_MODE_SMART;
    }
    
    if (lang >= LANG_TYPE_MAX) {
        lang = LANG_TYPE_EN;
    }

    return img_dsc_fan[fan_mode][lang];
}

const lv_img_dsc_t* title_get_comm_img(uint8_t comm_type)
{
    LV_IMG_DECLARE(ImgBle)
    LV_IMG_DECLARE(ImgLan)
    LV_IMG_DECLARE(ImgWifi)
	LV_IMG_DECLARE(ImgWifi1)
	LV_IMG_DECLARE(ImgWifi2)
	LV_IMG_DECLARE(ImgWifi3)
	LV_IMG_DECLARE(ImgNoWifi)
	LV_IMG_DECLARE(ImgNoWifi1)
	LV_IMG_DECLARE(ImgNoWifi2)
	LV_IMG_DECLARE(ImgNoWifi3)
	LV_IMG_DECLARE(ImgNoSignal)
	LV_IMG_DECLARE(ImgNoSignal1)
	LV_IMG_DECLARE(ImgNoSignal2)
	LV_IMG_DECLARE(ImgNoSignal3)
	uint8_t count;
	
    static const lv_img_dsc_t* img_dsc_com[] = {
        [COM_TYPE_WIFI] = &ImgWifi,
        [COM_TYPE_BLE] = &ImgBle,
        [COM_TYPE_NETWORK] = &ImgLan,
    };
	static const lv_img_dsc_t* img_no_dmx_com[4] = {
        [0] = &ImgNoSignal1,
        [1] = &ImgNoSignal2,
        [2] = &ImgNoSignal3,
		[3] = &ImgNoSignal,
    };
	static const lv_img_dsc_t* img_no_com[4] = {
        [0] = &ImgNoWifi1,
        [1] = &ImgNoWifi2,
        [2] = &ImgNoWifi3,
		[3] = &ImgNoWifi,
    };
	static const lv_img_dsc_t* img_dmx_com[4] = {
        [0] = &ImgWifi1,
        [1] = &ImgWifi2,
        [2] = &ImgWifi3,
		[3] = &ImgWifi,
    };

    if (comm_type >= COM_TYPE_MAX)
    {
        comm_type = COM_TYPE_WIFI;
    }
    
    if (comm_type == COM_TYPE_WIFI)
    {
		count = crmx_get_link_streng();
		
        if (crmx_is_paired() == 0 || crmx_is_paired() == 1)
        {
            return img_no_com[count];
        }
		else if(crmx_is_paired() == 2)
		{
            return img_no_dmx_com[count];
		}
		else if(crmx_is_paired() == 3)
		{
            return img_dmx_com[count];
		}
    }

    return img_dsc_com[comm_type];
}

const lv_img_dsc_t* title_get_power_img(uint8_t power_type)
{
    LV_IMG_DECLARE(img_Dc)
    LV_IMG_DECLARE(img_Adapter)
    static const lv_img_dsc_t* img_dsc_power[] = {
        [POWER_TYPE_SUPPLY_BOX] = &img_Adapter,
        [POWER_TYPE_DC] = &img_Dc,
    };
    if (power_type >= POWER_TYPE_SUPPLY_BOX)
    {
        power_type = POWER_TYPE_SUPPLY_BOX;
    }
    return img_dsc_power[power_type];
}

const lv_img_dsc_t* title_get_indoor_power_img(void)
{
    LV_IMG_DECLARE(img_Indoor_power)
	
    return &img_Indoor_power;
}

const lv_img_dsc_t* title_get_curve_img(uint8_t curve_type)
{
    LV_IMG_DECLARE(ImgCurveLog);
    LV_IMG_DECLARE(ImgCurveExp);
    LV_IMG_DECLARE(ImgCurveLinear);
    LV_IMG_DECLARE(ImgCurveSShape);
    static const lv_img_dsc_t* img_dsc_curve[] = {
        [CURVE_TYPE_LINEAR] = &ImgCurveLinear,
        [CURVE_TYPE_EXP] = &ImgCurveExp,
        [CURVE_TYPE_S_SHAPE] = &ImgCurveSShape,
        [CURVE_TYPE_LOG] = &ImgCurveLog,
    };

    if (curve_type >= CURVE_TYPE_MAX)
    {
        curve_type = CURVE_TYPE_LINEAR;
    }

    return img_dsc_curve[curve_type];
}

const lv_img_dsc_t* title_get_small_curve_img(uint8_t type)
{
    LV_IMG_DECLARE(ImgCurveLog_L);
    LV_IMG_DECLARE(ImgCurveExp_L);
    LV_IMG_DECLARE(ImgCurveLinear_L);
    LV_IMG_DECLARE(ImgCurveSShape_L);
    static const lv_img_dsc_t* img_dsc_curve[] = {
        [CURVE_TYPE_LINEAR] = &ImgCurveLinear_L,
        [CURVE_TYPE_S_SHAPE] = &ImgCurveSShape_L,
        [CURVE_TYPE_LOG] = &ImgCurveLog_L,
        [CURVE_TYPE_EXP] = &ImgCurveExp_L,
    };

    if (type >= CURVE_TYPE_MAX)
    {
        type = CURVE_TYPE_LINEAR;
    }

    return img_dsc_curve[type]; 
}

const lv_img_dsc_t* ui_get_fanmode_img(uint8_t fan_mode)
{
    const lv_img_dsc_t* img = NULL;

    LV_IMG_DECLARE(ImgFanModeSmart_B);
    LV_IMG_DECLARE(ImgFanModeSilent_B);
    LV_IMG_DECLARE(ImgFanModeMid_B);
    LV_IMG_DECLARE(ImgFanModeHigh_B);

    switch(fan_mode)
    {
        case FAN_MODE_SMART: img = &ImgFanModeSmart_B; break;
        case FAN_MODE_HIGH: img = &ImgFanModeHigh_B; break;
        case FAN_MODE_MEDIUM: img = &ImgFanModeMid_B; break;
        case FAN_MODE_SILENT: img = &ImgFanModeSilent_B; break;
    }

    return img;
}

const lv_img_dsc_t *ui_get_mastermode_img(uint8_t mode)
{
	const lv_img_dsc_t* img = NULL;
	
	LV_IMG_DECLARE(ImgFollowMode);
    LV_IMG_DECLARE(ImgLeadMode);
	LV_IMG_DECLARE(ImgMasterMode);
	
	if(mode == 0)
		img = &ImgLeadMode;
	else
		img = &ImgFollowMode;
	
	return img;
}

const lv_img_dsc_t *ui_get_high_speed_img(void)
{
	const lv_img_dsc_t* img = NULL;
	
	LV_IMG_DECLARE(ImgHighSpeed);
	
	img = &ImgHighSpeed;
	
	return img;
}
