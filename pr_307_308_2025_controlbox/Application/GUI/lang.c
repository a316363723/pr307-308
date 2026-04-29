/*********************
 *      INCLUDES
 *********************/
#include "lang.h"
#include "ui_data.h"
/*********************
 *      DEFINES
 *********************/
#define STRING_DEF(id, chinese, en)     [id] = {en, chinese}

/*********************
 *      TPEDEFS
 *********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static uint8_t get_lang_index(void);

/*********************
 *  STATIC VATIABLES
 *********************/
static const char* LangStringEn[] = {
#if LANG_EN    
    #define MUTIPLE_LANG_EN
    #include "multiple_lang_string.h"
#else
    NULL,
#endif
};

static const char* LangStringChinese[] = {
#if LANG_SIMPLE_CHINAESE
    #define MUTIPLE_LANG_CHINESE
    #include "multiple_lang_string.h"
#else 
    NULL, 
#endif 
};

static const char** MutipleLangString[] = {
    LangStringEn,
    LangStringChinese,
};

/*********************
 *  GLOBAL VATIABLES
 *********************/

/*********************
 *  GLOBAL FUNCTIONS
 *********************/
void MutipleLnag_Init(void)
{
    uint16_t lang_flag = 0;

#if ENABLE_LANG_EN
    lang_flag |= LANG_EN_FLAG;
#else 
    lang_flag &= ~(LANG_EN_FLAG);
#endif

#if ENABLE_LANG_SIMPLE_CHINESE
    lang_flag |= LAGN_SIMPLE_CHINESE;
#else
    lang_flag &= ~(LAGN_SIMPLE_CHINESE);
#endif

}

const char* Lang_GetStringByID(uint32_t id)
{
    uint8_t lang = get_lang_index();

    if (lang >= LANG_TYPE_MAX)
    {
        lang = LANG_TYPE_EN;
    }
    
    if (id >= STRING_ID_MAX)
    {
        id = STRING_ID_EN;
    }

    if (MutipleLangString[lang] == NULL)
    {
        return "None";        
    }

    return MutipleLangString[lang][id];
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static uint8_t get_lang_index(void)
{
    return g_tUIAllData.sys_menu_model.lang;
}

