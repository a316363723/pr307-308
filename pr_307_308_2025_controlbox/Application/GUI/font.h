#ifndef __FONT_H
#define __FONT_H

#ifdef __cplusplus
extern "C" {
#endif
/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>

/**********************
 *      TYPEDEFS
 **********************/
enum {
    FONT_6 = 0,
    FONT_8,
    FONT_9,
    FONT_10,
    FONT_10_MEDIUM,
    FONT_10_BOLD,
    FONT_11S,
    FONT_12,
    FONT_12_BOLD,
    FONT_13,
	FONT_13_BOLD,
    FONT_14,
    FONT_14_MEDIUM,
	FONT_14_Heavy,
    FONT_14_BOLD,    
    FONT_15,
    FONT_16,
    FONT_16_MEDIUM,
    FONT_16_BOLD,
    FONT_17_MEDIUM,    
    FONT_18,
    FONT_18_MEDIUM,
    FONT_18_BOLD,
	FONT_18_HEAVR,
    FONT_22,
    FONT_22_BOLD,
    FONT_24,
    FONT_24_MEDIUM,
    FONT_24_BOLD,
    FONT_26, 
    FONT_32,
    FONT_48,
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/
struct _lv_font_t;
const struct _lv_font_t* Font_ResouceGet(uint8_t resource_id);


#ifdef __cplusplus
}
#endif

#endif // ! __FONT_H
