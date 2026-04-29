#ifndef __LANG_H
#define __LANG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stddef.h>

/*********************
 *      DEFINES
 *********************/
#define LANG_SIMPLE_CHINAESE    1
#define LANG_EN                 1

#define ENABLE_LANG_EN              1
#define ENABLE_LANG_SIMPLE_CHINESE  1

/**********************
 *      TYPEDEFS
 **********************/
//产品支持语言
enum {
    LANG_TYPE_EN = 0,    
    LANG_TYPE_SIMPLE_CHINESE = 1,
    LANG_TYPE_MAX,
};

enum {
    LANG_EN_FLAG = (1 << LANG_TYPE_EN),
    LAGN_SIMPLE_CHINESE = (1 << LANG_TYPE_SIMPLE_CHINESE),
};

enum {
    #define MUTIPLE_LANG_STR_ID
    #include "multiple_lang_string.h"
    STRING_ID_MAX,
};

/**********************8
 * GLOBAL PROTOTYPES
 **********************/
const char* Lang_GetStringByID(uint32_t id);

#ifdef __cplusplus
}
#endif

#endif
