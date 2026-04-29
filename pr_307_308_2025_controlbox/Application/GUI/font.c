/*********************
 *      INCLUDES
 *********************/

#include "font.h"
#include "lvgl/lvgl.h"

/**********************
 *   GLOBAL PROTOTYPE
 **********************/
LV_FONT_DECLARE(MisansMedium16)
LV_FONT_DECLARE(MisansBold16)
LV_FONT_DECLARE(MisansMedium18)
LV_FONT_DECLARE(MisansBold18)
LV_FONT_DECLARE(MiSansHeavy18)
LV_FONT_DECLARE(MisansRegular9)
LV_FONT_DECLARE(MisansMedium10)
LV_FONT_DECLARE(MisansBold10)
LV_FONT_DECLARE(MisansRegular10)
LV_FONT_DECLARE(MisansMedium12)
LV_FONT_DECLARE(MisansBold12)
LV_FONT_DECLARE(MisansMedium14)
LV_FONT_DECLARE(MiSansHeavy14)
LV_FONT_DECLARE(MisansBold14)
LV_FONT_DECLARE(MisansBold13)
LV_FONT_DECLARE(MisansMedium22)
LV_FONT_DECLARE(MisansBold22)
LV_FONT_DECLARE(MisansMedium24)
LV_FONT_DECLARE(MisansBold24)
LV_FONT_DECLARE(MiSansDemiBold49)
/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief 根据资源ID获取字体
 * 
 * @param resource_id 
 * @return const struct _lv_font_t* 
 */
const struct _lv_font_t* Font_ResouceGet(uint8_t resource_id)
{
    const struct _lv_font_t* font = &MisansMedium18;
    switch(resource_id)
    {
        case FONT_10:        font = &MisansRegular10; break;
        case FONT_10_MEDIUM: font = &MisansMedium10; break; 
        case FONT_10_BOLD:   font = &MisansBold10; break;
        case FONT_12:        font = &MisansMedium12; break;
        case FONT_12_BOLD:   font = &MisansBold12;break;
		case FONT_13_BOLD:   font = &MisansBold13;break;
        case FONT_14:        font = &MisansMedium14; break;
        case FONT_14_MEDIUM: font = &MisansMedium14; break;
		case FONT_14_Heavy:  font = &MiSansHeavy14;break;
        case FONT_14_BOLD:   font = &MisansBold14;break;        
        case FONT_16:        font = &MisansMedium16; break;
        case FONT_16_MEDIUM: font = &MisansMedium16; break;
        case FONT_16_BOLD:   font = &MisansBold16; break;
        case FONT_18:        font = &MisansMedium18; break;
        case FONT_18_MEDIUM: font = &MisansMedium18; break;
        case FONT_18_BOLD:   font = &MisansBold18; break;  
		case FONT_18_HEAVR:  font = &MiSansHeavy18;break;
        case FONT_22:        font = &MisansMedium22; break;
        case FONT_22_BOLD:   font = &MisansBold22; break;
        case FONT_24_MEDIUM: font = &MisansMedium24; break;
        case FONT_24_BOLD:   font = &MisansBold24; break;
        case FONT_48:        font = &MiSansDemiBold49; break;
        default: break;
    }

    return font;
}
