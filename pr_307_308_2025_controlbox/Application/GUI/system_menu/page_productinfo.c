/*********************
 *      INCLUDES
 *********************/
#include "../ui_common.h"
#include "../page_widget.h"
#include "math.h"
#include "stdio.h"
#include "os_event.h"
/*********************
 *      DEFINES
 *********************/
#define SYS_MENU_TITLE_FROM_TOP_PX              38        
#define INFO_PANEL_WIDTH                    265
#define INFO_PANEL_HEIGHT                   114
#define UID_PANEL_WIDTH                     265
#define UID_PANEL_HEIGHT                    51

/*********************
 *      TPEDEFS
 *********************/
 typedef struct {
    lv_obj_t* dmx_addr_value_label;
    lv_obj_t* dmx_lock_img;
    lv_obj_t* scroll_cont;
    lv_obj_t* imgbtn;
    lv_obj_t* img_cont;
    lv_obj_t* dmx_protector;
    lv_obj_t* prev_focus_obj;
	uint8_t last_sub_pid;
}view_t;
 
enum {
	PRODUCT_INFO_ID_ENTRY ,
	PRODUCT_INFO_ID_ERROR,
	PRODUCT_INFO_ID_VERSION,
	PRODUCT_INFO_ID_TEMP,
	PRODUCT_INFO_ID_FIXTURE_ORIENTA,
	PRODUCT_INFO_ID_FIXTURE_TIME,
	PRODUCT_INFO_ID_ERROR_CODE,
	PRODUCT_INFO_ID_ERROR_INFO,
	PRODUCT_INFO_ID_MAX,
};

enum {
	PRODUCT_INFO_ID_ERROR_EH_01 = 0,
	PRODUCT_INFO_ID_ERROR_EH_02 = 1,
	PRODUCT_INFO_ID_ERROR_EH_03 = 2,
	PRODUCT_INFO_ID_ERROR_EH_04 = 3,
	PRODUCT_INFO_ID_ERROR_EH_05 = 4,
	PRODUCT_INFO_ID_ERROR_EH_06 = 5,
	PRODUCT_INFO_ID_ERROR_EH_07 = 6,
	PRODUCT_INFO_ID_ERROR_EH_08 = 7,
	PRODUCT_INFO_ID_ERROR_EH_09 = 8,
	PRODUCT_INFO_ID_ERROR_EH_10 = 9,
	PRODUCT_INFO_ID_ERROR_EH_11 = 10,
	PRODUCT_INFO_ID_ERROR_EH_12 = 11,
	PRODUCT_INFO_ID_ERROR_EH_13 = 12,
	PRODUCT_INFO_ID_ERROR_EH_14 = 13,
	PRODUCT_INFO_ID_ERROR_EH_15 = 14,
	PRODUCT_INFO_ID_ERROR_EH_16 = 15,
	PRODUCT_INFO_ID_ERROR_EH_17 = 16,
	PRODUCT_INFO_ID_ERROR_EH_18 = 17,
	PRODUCT_INFO_ID_ERROR_EH_19 = 18,
	PRODUCT_INFO_ID_ERROR_EH_20 = 19,
	PRODUCT_INFO_ID_ERROR_EH_21 = 20,
	PRODUCT_INFO_ID_ERROR_EH_22 = 21,
	PRODUCT_INFO_ID_ERROR_EH_23 = 22,
	PRODUCT_INFO_ID_ERROR_EH_24 = 23,
	PRODUCT_INFO_ID_ERROR_EH_25 = 24,
	PRODUCT_INFO_ID_ERROR_EH_26 = 25,
	PRODUCT_INFO_ID_ERROR_EH_27 = 26,
	PRODUCT_INFO_ID_ERROR_EH_28 = 27,
	PRODUCT_INFO_ID_ERROR_EH_29 = 28,
	PRODUCT_INFO_ID_ERROR_EH_30 = 29,
	PRODUCT_INFO_ID_ERROR_EH_31 = 30,
	PRODUCT_INFO_ID_ERROR_EH_32 = 31,
	PRODUCT_INFO_ID_ERROR_EC_01 = 32,
	PRODUCT_INFO_ID_ERROR_EC_02 = 33,
	PRODUCT_INFO_ID_ERROR_EC_03 = 34,
	PRODUCT_INFO_ID_ERROR_EC_04 = 35,
	PRODUCT_INFO_ID_ERROR_EC_05 = 36,
	PRODUCT_INFO_ID_ERROR_EC_06 = 37,
	PRODUCT_INFO_ID_ERROR_EC_07 = 38,
	PRODUCT_INFO_ID_ERROR_EC_08 = 39,
	PRODUCT_INFO_ID_ERROR_EC_09 = 40,
	PRODUCT_INFO_ID_ERROR_EC_10 = 41,
	PRODUCT_INFO_ID_ERROR_EC_11 = 42,
	PRODUCT_INFO_ID_ERROR_EC_12 = 43,
	PRODUCT_INFO_ID_ERROR_EC_13 = 44,
	PRODUCT_INFO_ID_ERROR_EC_14 = 45,
	PRODUCT_INFO_ID_ERROR_EC_15 = 46,
	PRODUCT_INFO_ID_ERROR_EC_16 = 47,
	PRODUCT_INFO_ID_ERROR_EC_17 = 48,
	PRODUCT_INFO_ID_ERROR_EC_18 = 49,
	PRODUCT_INFO_ID_ERROR_EC_19 = 50,
	PRODUCT_INFO_ID_ERROR_EC_20 = 51,
	PRODUCT_INFO_ID_ERROR_EC_21 = 52,
	PRODUCT_INFO_ID_ERROR_EC_22 = 53,
	PRODUCT_INFO_ID_ERROR_EC_23 = 54,
	PRODUCT_INFO_ID_ERROR_EC_24 = 55,
	PRODUCT_INFO_ID_ERROR_EC_25 = 56,
	PRODUCT_INFO_ID_ERROR_EC_26 = 57,
	PRODUCT_INFO_ID_ERROR_EC_27 = 58,
	PRODUCT_INFO_ID_ERROR_EC_28 = 59,
	PRODUCT_INFO_ID_ERROR_EC_29 = 60,
	PRODUCT_INFO_ID_ERROR_EC_30 = 61,
	PRODUCT_INFO_ID_ERROR_EC_31 = 62,
	PRODUCT_INFO_ID_ERROR_EC_32 = 63,
	PRODUCT_INFO_ID_ERROR_max,
};

typedef struct
{
	bool  eh_01;
	bool  eh_02;
	bool  eh_03;
	bool  eh_04;
	bool  eh_05;
	bool  eh_06;
	bool  eh_07;
	bool  eh_08;
	bool  eh_09;
	bool  eh_10;
	bool  eh_11;
	bool  eh_12;
	bool  eh_13;
	bool  eh_14;
    bool  eh_15;
    bool  eh_16;
    bool  eh_17;
	bool  eh_18;
	bool  eh_19;
	bool  eh_20;
	bool  eh_21;	
	bool  eh_22;
	bool  eh_23;
	bool  ec_01;
    bool  ec_02;
	bool  ec_03;
    bool  ec_04;
	bool  ec_05;
    bool  ec_06;
	bool  ec_07;
    bool  ec_08;
	bool  ec_09;
    bool  ec_10;	
    bool  ec_11;
    bool  ec_12;
	bool  ec_13;
    bool  ec_14;
	bool  ec_15;
    bool  ec_16;
}error_info_record_t;

typedef struct
{
    uint64_t id;
	char *code_str;
}error_index_t;

static error_index_t s_error_index[] ={
{PRODUCT_INFO_ID_ERROR_EH_01, "Code: EH.001"},{PRODUCT_INFO_ID_ERROR_EH_02, "Code: EH.002"},
{PRODUCT_INFO_ID_ERROR_EH_03, "Code: EH.003"},{PRODUCT_INFO_ID_ERROR_EH_04, "Code: EH.004"},
{PRODUCT_INFO_ID_ERROR_EH_05, "Code: EH.005"},{PRODUCT_INFO_ID_ERROR_EH_06, "Code: EH.006"},
{PRODUCT_INFO_ID_ERROR_EH_07, "Code: EH.007"},{PRODUCT_INFO_ID_ERROR_EH_08, "Code: EH.008"},
{PRODUCT_INFO_ID_ERROR_EH_09, "Code: EH.009"},{PRODUCT_INFO_ID_ERROR_EH_10, "Code: EH.010"},
{PRODUCT_INFO_ID_ERROR_EH_11, "Code: EH.011"},{PRODUCT_INFO_ID_ERROR_EH_12, "Code: EH.012"},
{PRODUCT_INFO_ID_ERROR_EH_13, "Code: EH.013"},{PRODUCT_INFO_ID_ERROR_EH_14, "Code: EH.014"},
{PRODUCT_INFO_ID_ERROR_EH_15, "Code: EH.015"},{PRODUCT_INFO_ID_ERROR_EH_16, "Code: EH.016"},
{PRODUCT_INFO_ID_ERROR_EH_17, "Code: EH.017"},{PRODUCT_INFO_ID_ERROR_EH_18, "Code: EH.018"},
{PRODUCT_INFO_ID_ERROR_EH_19, "Code: EH.019"},{PRODUCT_INFO_ID_ERROR_EH_20, "Code: EH.020"},
{PRODUCT_INFO_ID_ERROR_EH_21, "Code: EH.021"},{PRODUCT_INFO_ID_ERROR_EH_22, "Code: EH.022"},
{PRODUCT_INFO_ID_ERROR_EH_23, "Code: EH.023"},{PRODUCT_INFO_ID_ERROR_EH_24, "Code: EH.024"},
{PRODUCT_INFO_ID_ERROR_EH_25, "Code: EH.025"},{PRODUCT_INFO_ID_ERROR_EH_26, "Code: EH.026"},
{PRODUCT_INFO_ID_ERROR_EH_27, "Code: EH.027"},{PRODUCT_INFO_ID_ERROR_EH_28, "Code: EH.028"},
{PRODUCT_INFO_ID_ERROR_EH_29, "Code: EH.029"},{PRODUCT_INFO_ID_ERROR_EH_30, "Code: EH.030"},
{PRODUCT_INFO_ID_ERROR_EH_31, "Code: EH.031"},{PRODUCT_INFO_ID_ERROR_EH_32, "Code: EH.032"},
{PRODUCT_INFO_ID_ERROR_EC_01, "Code: EC.001"},{PRODUCT_INFO_ID_ERROR_EC_02, "Code: EC.002"},
{PRODUCT_INFO_ID_ERROR_EC_03, "Code: EC.003"},{PRODUCT_INFO_ID_ERROR_EC_04, "Code: EC.004"},
{PRODUCT_INFO_ID_ERROR_EC_05, "Code: EC.005"},{PRODUCT_INFO_ID_ERROR_EC_06, "Code: EC.006"},
{PRODUCT_INFO_ID_ERROR_EC_07, "Code: EC.007"},{PRODUCT_INFO_ID_ERROR_EC_08, "Code: EC.008"},
{PRODUCT_INFO_ID_ERROR_EC_09, "Code: EC.009"},{PRODUCT_INFO_ID_ERROR_EC_10, "Code: EC.010"},
{PRODUCT_INFO_ID_ERROR_EC_11, "Code: EC.011"},{PRODUCT_INFO_ID_ERROR_EC_12, "Code: EC.012"},
{PRODUCT_INFO_ID_ERROR_EC_13, "Code: EC.013"},{PRODUCT_INFO_ID_ERROR_EC_14, "Code: EC.014"},
{PRODUCT_INFO_ID_ERROR_EC_15, "Code: EC.015"},{PRODUCT_INFO_ID_ERROR_EC_16, "Code: EC.016"},
{PRODUCT_INFO_ID_ERROR_EC_17, "Code: EC.017"},{PRODUCT_INFO_ID_ERROR_EC_18, "Code: EC.018"},
{PRODUCT_INFO_ID_ERROR_EC_19, "Code: EC.019"},{PRODUCT_INFO_ID_ERROR_EC_20, "Code: EC.020"},
{PRODUCT_INFO_ID_ERROR_EC_21, "Code: EC.021"},{PRODUCT_INFO_ID_ERROR_EC_22, "Code: EC.022"},
{PRODUCT_INFO_ID_ERROR_EC_23, "Code: EC.023"},{PRODUCT_INFO_ID_ERROR_EC_24, "Code: EC.024"},
{PRODUCT_INFO_ID_ERROR_EC_25, "Code: EC.025"},{PRODUCT_INFO_ID_ERROR_EC_26, "Code: EC.026"},
{PRODUCT_INFO_ID_ERROR_EC_27, "Code: EC.027"},{PRODUCT_INFO_ID_ERROR_EC_28, "Code: EC.028"},
{PRODUCT_INFO_ID_ERROR_EC_29, "Code: EC.029"},{PRODUCT_INFO_ID_ERROR_EC_30, "Code: EC.030"},
{PRODUCT_INFO_ID_ERROR_EC_31, "Code: EC.031"},{PRODUCT_INFO_ID_ERROR_EC_32, "Code: EC.032"},      
};
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void page_productinfo_construct(void);
static void page_productinfo_destruct(void);
static void page_productinfo_time_upd_cb(uint32_t ms);
static void common_event_cb(lv_event_t* e);
static void product_list_event_cb(lv_event_t* e, uint8_t index);
static void page_error_code_set(lv_obj_t * obj, uint64_t id );
static void page_error_code_lang_get(lv_obj_t * obj, uint64_t id );
static void page_error_code_sub_construct(uint64_t id, bool sta);
static const char* get_error_laber_str(uint8_t  type);
void page_error_info_construct(uint32_t chose);
static void page_error_info_set(lv_obj_t * obj, uint64_t id );
static void page_error_code_construct(void);
static void error_code_event_cb(lv_event_t* e, uint8_t index);
//static void error_code_info_event_cb(lv_event_t* e);
static void error_code_info_ignore_event_cb(lv_event_t* e);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t *indev_group;
static sys_config_t* p_sys_menu_model = NULL;
static my_list_ext_t list_ext;
static my_list_ext_t list_ext1;
//static my_list_ext_t list_ext2;
bool no_error_ignored = 0;
bool error_ignored = 0 ;
static sys_error_type  error_code = {0};
uint64_t error_chose = 0;
bool   error_code_dir = 0;
static view_t local_view;
extern void sys_box_lcd_lock(uint8_t value);
error_info_record_t  info_record;
static uint8_t window_index = 0;
static uint16_t  error_time = 0;
//uint32_t error_ec = 0;
//uint32_t error_eh = 0;

/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_productinfo = PAGE_INITIALIZER("Product Info",
                                PAGE_PRODUCT_INFO, 
                                page_productinfo_construct, 
                                page_productinfo_destruct, 
                                page_productinfo_time_upd_cb,
                                NULL,                                
                                PAGE_MENU);
/*********************
 *  GLOBAL FUNCTIONS
 *********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/    
static void page_productinfo_construct(void)
{
    indev_group = lv_group_create();
    p_sys_menu_model = screen_get_act_page_data();
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);
    (void)p_sys_menu_model;
	struct sys_info_uuid  production_uuid;
    uint8_t spid = screen_get_act_spid();
     switch (spid)
    {
        case PRODUCT_INFO_ID_ENTRY:
        {
			uint64_t error_id = 0;
			uint8_t error_num = 0;
			
			lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_t* title = lv_label_create(lv_scr_act());            
            lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
            lv_obj_set_style_text_color(title, lv_color_white(), 0);
            lv_label_set_text(title, Lang_GetStringByID(STRING_ID_PRODUCT_INFO));
            lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);   
            list_ext.cur_sel = screen_get_act_index();  
            list_ext.release_cb = product_list_event_cb;
			list_ext.max = 5;
            lv_obj_t* list = my_list_create(lv_scr_act(), lv_pct(78), lv_pct(80), &list_ext); 
            lv_obj_set_flex_align(list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);                       
            lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 38);
			for(uint8_t  i = 0 ; i < 64 ; i++)
			{
				error_id = gui_get_64type_convert(i);
				if(error_id & error_code.error_data)
				{
					error_num++;
				}
			}
			my_list_add_btn3(list, Lang_GetStringByID(STRING_ID_ERROR), 10,error_num);
            my_list_add_btn(list,  Lang_GetStringByID(STRING_ID_PRODUCT_VERSION), 10);
            my_list_add_btn(list,  Lang_GetStringByID(STRING_ID_TEMP), 10);
            my_list_add_btn(list,  Lang_GetStringByID(STRING_ID_FIXTURE_ORIENTA), 10);
            my_list_add_btn(list,  Lang_GetStringByID(STRING_ID_FIXTURE_RUN_TIME), 10);
			lv_obj_clear_flag(list, LV_OBJ_FLAG_SCROLLABLE);
			
			local_view.scroll_cont = my_list_scroll_create(lv_scr_act(), 3, 215);
            lv_obj_align_to(local_view.scroll_cont, list, LV_ALIGN_OUT_RIGHT_TOP, 21, 2); 
			
            lv_group_add_obj(indev_group, list);
            lv_group_focus_obj(list);
            lv_group_set_editing(indev_group, true);
        }
        break;
        case PRODUCT_INFO_ID_ERROR :
		{
			lv_obj_t* title = lv_label_create(lv_scr_act());            
			lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
			lv_obj_set_style_text_color(title, lv_color_white(), 0);
			lv_label_set_text(title, Lang_GetStringByID(STRING_ID_ERROR));
			lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);   

			list_ext1.cur_sel = screen_get_act_index();  
			list_ext1.release_cb = error_code_event_cb;
			
			lv_obj_t* list = my_list_create(lv_scr_act(), lv_pct(78), lv_pct(80), &list_ext1); 
			lv_obj_set_flex_align(list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);                       
			lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 38);
			
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_01))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EH_001), 10);
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_02))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EH_002), 10);						
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_03))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EH_003), 10);	
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_05))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EH_005), 10);		
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_10))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EH_010), 10);	
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_11))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EH_011), 10);	
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_12))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EH_012), 10);			
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_13))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EH_013), 10);	
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_14))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EH_014), 10);
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_15))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EH_015), 10);
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_16))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EH_016), 10);	
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_17))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EH_017), 10);	
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_18))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EH_018), 10);
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_19))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EH_019), 10);	
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_20))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EH_020), 10);		
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_21))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EH_021), 10);	
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_22))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EH_022), 10);	
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_23))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EH_023), 10);	
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_02))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EC_002), 10);
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_03))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EC_003), 10);	
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_06))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EC_006), 10);						
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_07))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EC_007), 10);						
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_09))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EC_009), 10);						
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_10))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EC_010), 10);					
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_11))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EC_011), 10);
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_12))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EC_012), 10);
			if(error_code.error_data &gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_13))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EC_013), 10);
			if(error_code.error_data &gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_14))
				my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ERROR_CODE_EC_014), 10);

			lv_group_add_obj(indev_group, list);
			lv_group_focus_obj(list);
			lv_group_set_editing(indev_group, true);
		
		}
		break;
        case PRODUCT_INFO_ID_VERSION:
        {
	    #if UI_DUMMY_DATA_ENABLE            
            lv_snprintf(sdb, sizeof(sdb), "UID:  %s", "23123210");        
        #else 
            data_center_read_sys_info(SYS_INFO_UUID, &production_uuid);
            production_uuid.uuid[13] = '\0';
            lv_snprintf(sdb, sizeof(sdb), "UID:  %s", production_uuid.uuid);        
        #endif 
			
            lv_obj_t* title = lv_label_create(lv_scr_act());            
            lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
            lv_obj_set_style_text_color(title, lv_color_white(), 0);
            lv_label_set_text(title, Lang_GetStringByID(STRING_ID_PRODUCT_VERSION));
            lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);
			lv_obj_t* screen = lv_obj_create(lv_scr_act());
			lv_obj_remove_style_all(screen);
			lv_obj_set_size(screen, lv_pct(100), lv_pct(100));
			lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_t* info_panel = lv_obj_create(screen);
            lv_obj_clear_flag(info_panel, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_remove_style_all(info_panel);    
            lv_obj_add_style(info_panel, &style_common_btn, 0);
            lv_obj_set_style_bg_opa(info_panel, LV_OPA_100, 0);
            lv_obj_set_style_text_font(info_panel, Font_ResouceGet(FONT_18), 0);
            lv_obj_set_size(info_panel, INFO_PANEL_WIDTH, INFO_PANEL_HEIGHT);
            lv_obj_align(info_panel, LV_ALIGN_TOP_MID, 0, 56);

            lv_obj_t* ctrlbox_name = lv_label_create(info_panel);
            lv_label_set_text(ctrlbox_name, Lang_GetStringByID(STRING_ID_CONTROL_BOX));
            lv_obj_align(ctrlbox_name, LV_ALIGN_TOP_LEFT, 6, 34);

            lv_obj_t* ver1 = lv_label_create(info_panel);
            lv_label_set_text(ver1, "Ver. :");
            lv_obj_align(ver1, LV_ALIGN_TOP_LEFT, 179, 34);
            
            lv_obj_t* ctrlbox_ver = lv_label_create(info_panel);
            lv_label_set_text_fmt(ctrlbox_ver, " %d.%d", ui_get_ctrlbox_hw_version()/10, ui_get_ctrlbox_hw_version()%10);        
            lv_obj_align(ctrlbox_ver, LV_ALIGN_TOP_RIGHT, -6, 34);

            lv_obj_t* lamp_name = lv_label_create(info_panel);
            lv_label_set_text(lamp_name, Lang_GetStringByID(STRING_ID_LAMP_HEAD));
            lv_obj_align(lamp_name, LV_ALIGN_TOP_LEFT, 6, 66);

            lv_obj_t* ver2 = lv_label_create(info_panel);
            lv_label_set_text(ver2, "Ver. :");
            lv_obj_align(ver2, LV_ALIGN_TOP_LEFT, 179, 66);

            lv_obj_t* lamp_ver = lv_label_create(info_panel);
            lv_label_set_text_fmt(lamp_ver, " %d.%d",  ui_get_lamp_hw_version()/10,ui_get_lamp_hw_version()%10);     
            lv_obj_align(lamp_ver, LV_ALIGN_TOP_RIGHT, -6, 66);

            lv_obj_t* uid_panel = lv_obj_create(screen);
            lv_obj_remove_style_all(uid_panel);
            lv_obj_add_style(uid_panel, &style_common_btn, 0);
            lv_obj_set_style_text_font(uid_panel, Font_ResouceGet(FONT_18), 0);
            lv_obj_set_style_bg_opa(uid_panel, LV_OPA_100, 0);
            lv_obj_set_size(uid_panel, UID_PANEL_WIDTH, UID_PANEL_HEIGHT);
            lv_obj_align_to(uid_panel, info_panel, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

            lv_obj_t* uid_label = lv_label_create(uid_panel);
            lv_label_set_text(uid_label, sdb);
            lv_obj_center(uid_label);

            lv_obj_add_event_cb(info_panel, common_event_cb, LV_EVENT_ALL, 0);
            lv_group_add_obj(indev_group, info_panel);
            lv_group_focus_obj(info_panel);      
            lv_group_set_editing(indev_group, true);
        }
            break;
        case PRODUCT_INFO_ID_TEMP:
        {
			uint16_t circle_high = 32;
			lv_obj_t* screen = lv_obj_create(lv_scr_act());
			lv_obj_remove_style_all(screen);
			lv_obj_set_size(screen, lv_pct(100), lv_pct(100));
			lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_t* title = lv_label_create(lv_scr_act());            
            lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
            lv_obj_set_style_text_color(title, lv_color_white(), 0);
            lv_label_set_text(title, Lang_GetStringByID(STRING_ID_TEMP));
            lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);
            lv_obj_t* temp_panel = lv_obj_create(screen);
            lv_obj_clear_flag(temp_panel, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_remove_style_all(temp_panel);    
            lv_obj_add_style(temp_panel, &style_common_btn, 0);
            lv_obj_set_style_bg_opa(temp_panel, LV_OPA_100, 0);
            lv_obj_set_style_text_font(temp_panel, Font_ResouceGet(FONT_18), 0);
            lv_obj_set_size(temp_panel, INFO_PANEL_WIDTH, INFO_PANEL_HEIGHT+30);
            lv_obj_align(temp_panel, LV_ALIGN_TOP_MID, 0, 56);
            
            int8_t temp_value[3] = {0};

            temp_value[0] = ui_get_lamp_cob_temp();
            temp_value[1] = ui_get_box_mcu_temp();
            temp_value[2] = ui_get_box_ambient_temp();
            
            lv_obj_t* cob_name = lv_label_create(temp_panel);
            lv_label_set_text(cob_name, Lang_GetStringByID(STRING_ID_COB_TIME));
            lv_obj_align(cob_name, LV_ALIGN_TOP_LEFT, 6, 30);
            lv_obj_t* cob_panel= lv_label_create(temp_panel); 
            lv_obj_align(cob_panel, LV_ALIGN_TOP_RIGHT, -20, 30);
            if( temp_value[0] >= 0)
            { 
                lv_label_set_text_fmt(cob_panel, "%d  C", temp_value[0]);     
            }
            else
            {
                 lv_label_set_text_fmt(cob_panel, "-%d  C", -temp_value[0]);  
            }
            lv_obj_t* mcu_name = lv_label_create(temp_panel);
            lv_label_set_text(mcu_name, Lang_GetStringByID(STRING_ID_MCU_TIME));
            lv_obj_align(mcu_name, LV_ALIGN_TOP_LEFT, 6, 62);
            lv_obj_t* mcu_temp = lv_label_create(temp_panel);      
            lv_obj_align(mcu_temp, LV_ALIGN_TOP_RIGHT, -20, 62);
             if( temp_value[1] >= 0)
            { 
                lv_label_set_text_fmt(mcu_temp, "%d  C", temp_value[1]);     
            }
            else
            {
                 lv_label_set_text_fmt(mcu_temp, "-%d  C", -(temp_value[1]));  
            }  
            lv_obj_t* ambient_name = lv_label_create(temp_panel);
            lv_label_set_text(ambient_name, Lang_GetStringByID(STRING_ID_Ambient_TIME));
            lv_obj_align(ambient_name, LV_ALIGN_TOP_LEFT, 6, 94);
            
            lv_obj_t* ambient_temp = lv_label_create(temp_panel);      
            lv_obj_align(ambient_temp, LV_ALIGN_TOP_RIGHT, -20, 94);
            if( temp_value[2] >= 0)
            { 
                lv_label_set_text_fmt(ambient_temp, "%d  C", temp_value[2]);     
            }
            else
            {
                 lv_label_set_text_fmt(ambient_temp, "-%d  C", -(temp_value[2]));  
            }
			for(uint8_t i = 0; i< 3 ;i++)
			{
				lv_obj_t* circle_obj = lv_obj_create(temp_panel);
				
				lv_obj_remove_style_all(circle_obj);
				lv_obj_set_style_radius(circle_obj, 0x7ff, 0);
				lv_obj_set_style_bg_opa(circle_obj, LV_OPA_100, 0);
				lv_obj_set_style_bg_color(circle_obj, lv_color_white(), 0);
				lv_obj_set_size(circle_obj, 6, 6);                                    
				lv_obj_align(circle_obj, LV_ALIGN_TOP_RIGHT,  -32, circle_high);
				lv_obj_t* circle_panel = lv_obj_create(circle_obj);
				lv_obj_remove_style_all(circle_panel);
				lv_obj_set_style_radius(circle_panel, 0x7ff, 0);
				lv_obj_add_style(circle_panel, &style_common_btn, 0);
				lv_obj_set_style_bg_opa(circle_panel, LV_OPA_100, 0);
				lv_obj_set_size(circle_panel, 4, 4);                                    
				lv_obj_align(circle_panel, LV_ALIGN_CENTER, 0, 0);
				circle_high += 32;
			}
            lv_obj_add_event_cb(temp_panel, common_event_cb, LV_EVENT_ALL, 0);
            lv_group_add_obj(indev_group, temp_panel);
            lv_group_focus_obj(temp_panel);      
            lv_group_set_editing(indev_group, true);
        }
            
		 break;
         case PRODUCT_INFO_ID_FIXTURE_ORIENTA:
         {
			uint16_t circle_high = 56;
			lv_obj_t* screen = lv_obj_create(lv_scr_act());
			lv_obj_remove_style_all(screen);
			lv_obj_set_size(screen, lv_pct(100), lv_pct(100));
			lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_t* title = lv_label_create(lv_scr_act());            
            lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
            lv_obj_set_style_text_color(title, lv_color_white(), 0);
            lv_label_set_text(title, Lang_GetStringByID(STRING_ID_FIXTURE_ORIENTA));
             lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);
            lv_obj_t* info_panel = lv_obj_create(screen);
            lv_obj_clear_flag(info_panel, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_remove_style_all(info_panel);    
            lv_obj_add_style(info_panel, &style_common_btn, 0);
            lv_obj_set_style_bg_opa(info_panel, LV_OPA_100, 0);
            lv_obj_set_style_text_font(info_panel, Font_ResouceGet(FONT_18), 0);
            lv_obj_set_size(info_panel, INFO_PANEL_WIDTH, INFO_PANEL_HEIGHT+30);
            lv_obj_align(info_panel, LV_ALIGN_TOP_MID, 0, 56);
            
            lv_obj_t* pitch_name = lv_label_create(info_panel);
            lv_label_set_text(pitch_name, Lang_GetStringByID(STRING_ID_ORIENTA_PITCH));
            lv_obj_align(pitch_name, LV_ALIGN_TOP_LEFT, 6, 52);
            
            lv_obj_t* pitch_angle = lv_label_create(info_panel);
            lv_label_set_text_fmt(pitch_angle, " %d", ui_get_orienta_pitch());        
            lv_obj_align(pitch_angle, LV_ALIGN_TOP_RIGHT, -20, 52);
            
            lv_obj_t* roll_name = lv_label_create(info_panel);
            lv_label_set_text(roll_name, Lang_GetStringByID(STRING_ID_ORIENTA_ROLL));
            lv_obj_align(roll_name, LV_ALIGN_TOP_LEFT, 6, 82);
            
            lv_obj_t* roll_angle = lv_label_create(info_panel);
            lv_label_set_text_fmt(roll_angle, " %d", ui_get_orienta_roll());  	   
            lv_obj_align(roll_angle, LV_ALIGN_TOP_RIGHT, -20, 82);
		
			for(uint8_t i = 0; i < 2 ;i++)
			{
				lv_obj_t* circle_obj = lv_obj_create(info_panel);
				lv_obj_remove_style_all(circle_obj);
				lv_obj_set_style_radius(circle_obj, 0x7ff, 0);
				lv_obj_set_style_bg_opa(circle_obj, LV_OPA_100, 0);
				lv_obj_set_style_bg_color(circle_obj, lv_color_white(), 0);
				lv_obj_set_size(circle_obj, 6, 6);                                    
				lv_obj_align(circle_obj, LV_ALIGN_TOP_RIGHT,  -13, circle_high);
				lv_obj_t* circle_panel = lv_obj_create(circle_obj);
				lv_obj_remove_style_all(circle_panel);
				lv_obj_set_style_radius(circle_panel, 0x7ff, 0);
				lv_obj_add_style(circle_panel, &style_common_btn, 0);
				lv_obj_set_style_bg_opa(circle_panel, LV_OPA_100, 0);
				lv_obj_set_size(circle_panel, 4, 4);                                    
				lv_obj_align(circle_panel, LV_ALIGN_CENTER, 0, 0);
				circle_high += 30;
			}
            lv_obj_add_event_cb(info_panel, common_event_cb, LV_EVENT_ALL, 0);
            lv_group_add_obj(indev_group, info_panel);
            lv_group_focus_obj(info_panel);      
            lv_group_set_editing(indev_group, true);
        }
         break;
        case PRODUCT_INFO_ID_FIXTURE_TIME:
        {
			char data[10] = {0};
			
			lv_obj_t* screen = lv_obj_create(lv_scr_act());
			lv_obj_remove_style_all(screen);
			lv_obj_set_size(screen, lv_pct(100), lv_pct(100));
			lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_t* title1 = lv_label_create(lv_scr_act());            
            lv_obj_set_style_text_font(title1, Font_ResouceGet(FONT_22_BOLD), 0);
            lv_obj_set_style_text_color(title1, lv_color_white(), 0);
            lv_label_set_text(title1, Lang_GetStringByID(STRING_ID_LED_TIME));
            lv_obj_align(title1, LV_ALIGN_TOP_MID, 0, 5);
//            lv_obj_set_size(title1, INFO_PANEL_WIDTH, INFO_PANEL_HEIGHT);
            lv_obj_t* title2 = lv_label_create(lv_scr_act()); 
            lv_obj_set_style_text_font(title2, Font_ResouceGet(FONT_22_BOLD), 0);
            lv_obj_set_style_text_color(title2, lv_color_white(), 0);
            lv_label_set_text(title2, Lang_GetStringByID(STRING_ID_SYSTEM_TIME));
            lv_obj_align(title2, LV_ALIGN_CENTER, 0, 10);
            
			lv_obj_t* led_obj = lv_obj_create(screen);
            lv_obj_clear_flag(led_obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_remove_style_all(led_obj);    
            lv_obj_add_style(led_obj, &style_common_btn, 0);
            lv_obj_set_style_bg_opa(led_obj, LV_OPA_100, 0);
            lv_obj_set_size(led_obj, 270, 48);
			lv_obj_set_pos(led_obj, 28, 51);
			
			lv_obj_t* led_text = lv_label_create(led_obj);          
            lv_obj_set_style_text_font(led_text, Font_ResouceGet(FONT_18_BOLD), 0);
            lv_obj_set_style_text_color(led_text, lv_color_white(), 0);
            lv_label_set_text(led_text, Lang_GetStringByID(STRING_ID_TOTAL_TIME));
            lv_obj_align(led_text, LV_ALIGN_LEFT_MID, 10, 4);
			lv_obj_t* led_time = lv_label_create(led_obj);
			lv_obj_set_style_text_font(led_time, Font_ResouceGet(FONT_18_BOLD), 0);
            lv_label_set_text_fmt(led_time, " %dh", ui_get_led_time());        
            lv_obj_align(led_time, LV_ALIGN_RIGHT_MID, -20, 0);
          
            lv_obj_t* sys_obj = lv_obj_create(screen);
            lv_obj_clear_flag(sys_obj, LV_OBJ_FLAG_SCROLLABLE);
            lv_obj_remove_style_all(sys_obj);    
            lv_obj_add_style(sys_obj, &style_common_btn, 0);
            lv_obj_set_style_bg_opa(sys_obj, LV_OPA_100, 0);
			lv_obj_set_size(sys_obj, 270, 48);
			lv_obj_set_pos(sys_obj, 28, 171);
			
			lv_obj_t* sys_text = lv_label_create(sys_obj);
            lv_obj_set_style_text_font(sys_text, Font_ResouceGet(FONT_18_BOLD), 0);
            lv_obj_set_style_text_color(sys_text, lv_color_white(), 0);
            lv_label_set_text(sys_text, Lang_GetStringByID(STRING_ID_TOTAL_TIME));
            lv_obj_align(sys_text, LV_ALIGN_LEFT_MID, 10, 4);
			lv_obj_t* sys_time = lv_label_create(sys_obj);
			lv_obj_set_style_text_font(sys_time, Font_ResouceGet(FONT_18_BOLD), 0);
			sprintf(data, "%dh", ui_get_sys_time());
			lv_label_set_text(sys_time, (char *)data);
//            lv_label_set_text_fmt(sys_time, " %dh", ui_get_sys_time());        
            lv_obj_align(sys_time, LV_ALIGN_RIGHT_MID, -20,0);
			lv_obj_add_event_cb(sys_obj, common_event_cb, LV_EVENT_ALL, 0);
            lv_group_add_obj(indev_group, sys_obj);
            lv_group_focus_obj(sys_obj);      
            lv_group_set_editing(indev_group, true);   
        }
			break;    
		case PRODUCT_INFO_ID_ERROR_CODE:
		{
			error_ignored = 0;		
			page_error_code_construct();
		}
		break;
		case PRODUCT_INFO_ID_ERROR_INFO:
		{
			switch(window_index)
			{
				case PRODUCT_INFO_ID_ERROR_EH_01:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_01,0);

					break;
				case PRODUCT_INFO_ID_ERROR_EH_02:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_02,1);
					break;
				case PRODUCT_INFO_ID_ERROR_EH_03:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_03,1);
					break;
				case PRODUCT_INFO_ID_ERROR_EH_05:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_05,1);
					break;
				case PRODUCT_INFO_ID_ERROR_EH_10:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_10,1);
					break;
				case PRODUCT_INFO_ID_ERROR_EH_11:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_11,0);
					info_record.eh_11 = 1;
					break;
				case PRODUCT_INFO_ID_ERROR_EH_12:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_12,1);
					break;
				case PRODUCT_INFO_ID_ERROR_EH_13:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_13,0);
					info_record.eh_13 = 1;
					break;
				case PRODUCT_INFO_ID_ERROR_EH_14:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_14,0);
					info_record.eh_14 = 1;
					break;
				case PRODUCT_INFO_ID_ERROR_EH_15:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_15,1);
					break;
				case PRODUCT_INFO_ID_ERROR_EH_16:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_16,1);
					break;
				case PRODUCT_INFO_ID_ERROR_EH_17:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_17,1);
					break;
				case PRODUCT_INFO_ID_ERROR_EH_18:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_18,0);
					break;
				case PRODUCT_INFO_ID_ERROR_EH_19:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_19,0);
					break;
				case PRODUCT_INFO_ID_ERROR_EH_20:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_20,0);
					break;
				case PRODUCT_INFO_ID_ERROR_EH_21:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_21,1);
					break;
				case PRODUCT_INFO_ID_ERROR_EH_22:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_22,1);
					break;
				case PRODUCT_INFO_ID_ERROR_EH_23:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_23,1);
					break;
				case PRODUCT_INFO_ID_ERROR_EC_02:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EC_02,1);
					break;
				case PRODUCT_INFO_ID_ERROR_EC_03:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EC_03,1);
					break;
				case PRODUCT_INFO_ID_ERROR_EC_06:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EC_06,1);
					break;
				case PRODUCT_INFO_ID_ERROR_EC_07:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EC_07,0);
					info_record.ec_07 = 1;
					break;
				case PRODUCT_INFO_ID_ERROR_EC_09:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EC_09,0);
					info_record.ec_09 = 1;
					break;
				case PRODUCT_INFO_ID_ERROR_EC_10:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EC_10,1);
					break;
				case PRODUCT_INFO_ID_ERROR_EC_11:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EC_11,1);
					break;
				case PRODUCT_INFO_ID_ERROR_EC_12:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EC_12,1);
					break;
				case PRODUCT_INFO_ID_ERROR_EC_13:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EC_13,1);
					break;
				case PRODUCT_INFO_ID_ERROR_EC_14:
					page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EC_14,1);
					break;
				default : break;
			}
		}

	}
}

static void page_productinfo_time_upd_cb(uint32_t ms)
{
	uint8_t spid = screen_get_act_spid();
	
	switch(spid)
	{
		case PRODUCT_INFO_ID_TEMP:
		{
			int8_t temp_value[3] = {0};

            temp_value[0] = ui_get_lamp_cob_temp();
            temp_value[1] = ui_get_box_mcu_temp();
            temp_value[2] = ui_get_box_ambient_temp();
			
			lv_obj_t *pre_obj = lv_obj_get_child(lv_scr_act(), 0);
			lv_obj_t *temp_pre_obj = lv_obj_get_child(pre_obj, 0);
			lv_obj_t *led_temp_label = lv_obj_get_child(temp_pre_obj, 1);
			lv_obj_t *mcu_temp_label = lv_obj_get_child(temp_pre_obj, 3);
			lv_obj_t *ad_temp_label = lv_obj_get_child(temp_pre_obj, 5);
			
			if( temp_value[0] >= 0)
            { 
                lv_label_set_text_fmt(led_temp_label, "%d  C", temp_value[0]);     
            }
            else
            {
                 lv_label_set_text_fmt(led_temp_label, "-%d  C", -temp_value[0]);  
            }
			
            if( temp_value[1] >= 0)
            { 
                lv_label_set_text_fmt(mcu_temp_label, "%d  C", temp_value[1]);     
            }
            else
            {
                 lv_label_set_text_fmt(mcu_temp_label, "-%d  C", -(temp_value[1]));  
            }  
			
            if( temp_value[2] >= 0)
            { 
                lv_label_set_text_fmt(ad_temp_label, "%d  C", temp_value[2]);     
            }
            else
            {
                 lv_label_set_text_fmt(ad_temp_label, "-%d  C", -(temp_value[2]));  
            }
		}
		break;
		case PRODUCT_INFO_ID_FIXTURE_ORIENTA:
		{
			lv_obj_t *pre_obj = lv_obj_get_child(lv_scr_act(), 0);
			lv_obj_t *angle_pre_obj = lv_obj_get_child(pre_obj, 0);
//			lv_obj_t *level_angle_label = lv_obj_get_child(angle_pre_obj, 1);
			lv_obj_t *pitch_angle_label = lv_obj_get_child(angle_pre_obj, 1);
			lv_obj_t *roll_angle_label = lv_obj_get_child(angle_pre_obj, 3);
			
//            lv_label_set_text_fmt(level_angle_label, " %d", ui_get_orienta_level());       
            
            
            lv_label_set_text_fmt(pitch_angle_label, " %d", ui_get_orienta_pitch());       
            
            lv_label_set_text_fmt(roll_angle_label, " %d", ui_get_orienta_roll());  	
		}
		break;
		default:break;
	}
}


static void page_error_code_construct(void)
{
	lv_obj_clean(lv_scr_act());
	uint64_t   chose = error_chose;
	
	switch(chose)
	{
		case PRODUCT_INFO_ID_ERROR_EH_01:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_01,0);
			break;
		case PRODUCT_INFO_ID_ERROR_EH_02:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_02,1);
			break;
		case PRODUCT_INFO_ID_ERROR_EH_03:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_03,1);
			break;
		case PRODUCT_INFO_ID_ERROR_EH_05:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_05,1);
			break;
		case PRODUCT_INFO_ID_ERROR_EH_10:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_10,1);
			break;
		case PRODUCT_INFO_ID_ERROR_EH_11:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_11,0);
			break;
		case PRODUCT_INFO_ID_ERROR_EH_12:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_12,1);
			break;
		case PRODUCT_INFO_ID_ERROR_EH_13:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_13,0);
			break;
		case PRODUCT_INFO_ID_ERROR_EH_14:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_14,0);
			break;
		case PRODUCT_INFO_ID_ERROR_EH_15:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_15,1);
			break;
		case PRODUCT_INFO_ID_ERROR_EH_16:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_16,1);
			break;
		case PRODUCT_INFO_ID_ERROR_EH_17:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_17,1);
			break;
		case PRODUCT_INFO_ID_ERROR_EH_18:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_18,0);
			break;
		case PRODUCT_INFO_ID_ERROR_EH_19:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_19,0);
			break;	
		case PRODUCT_INFO_ID_ERROR_EH_20:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_20,0);
			break;
		case PRODUCT_INFO_ID_ERROR_EH_21:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_21,0);
			break;
		case PRODUCT_INFO_ID_ERROR_EH_22:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_22,0);
			break;
		case PRODUCT_INFO_ID_ERROR_EH_23:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EH_23,0);
			break;
		case PRODUCT_INFO_ID_ERROR_EC_02:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EC_02,1);
			break;
		case PRODUCT_INFO_ID_ERROR_EC_03:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EC_03,1);
			break;
		case PRODUCT_INFO_ID_ERROR_EC_06:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EC_06,1);
			break;
		case PRODUCT_INFO_ID_ERROR_EC_07:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EC_07,0);
			break;
		case PRODUCT_INFO_ID_ERROR_EC_09:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EC_09,0);
			break;
		case PRODUCT_INFO_ID_ERROR_EC_10:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EC_10,1);
			break;
		case PRODUCT_INFO_ID_ERROR_EC_11:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EC_11,1);
			break;
		case PRODUCT_INFO_ID_ERROR_EC_12:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EC_12,1);
			break;
		case PRODUCT_INFO_ID_ERROR_EC_13:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EC_13,1);
			break;
		case PRODUCT_INFO_ID_ERROR_EC_14:
			page_error_code_sub_construct(PRODUCT_INFO_ID_ERROR_EC_14,1);
			break;
		default :
			break;
	}
}
  

static void page_productinfo_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;
    lv_obj_clean(lv_scr_act());
	
    memset(&local_view, 0, sizeof(local_view));
}

static void common_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    shortcut_keys_event_handler(e);
    if (event == LV_EVENT_KEY)  
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);

        if (key == LV_KEY_BACKSPACE)
        {
            screen_turn_prev_page();
        }
    }  
	else if (event == LV_EVENT_RELEASED)
    {
		screen_turn_prev_page();
    }
}

static void product_list_event_cb(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);
	my_list_ext_t* ext = lv_obj_get_user_data(obj);
	
    shortcut_keys_event_handler(e);
	
    if (event == LV_EVENT_RELEASED)
    {
		if(error_code.error_data == 0 && index ==0) return;
        screen_load_sub_page_with_stack(index+1, index);
    }
	else if (event == LV_EVENT_FOCUSED)
    {
        my_list_scroll_refresh(local_view.scroll_cont, lv_map(ext->cur_sel, 0, ext->max, 0, 100));
    }
    else if (event == LV_EVENT_KEY)
    {        
        uint32_t key = *(uint32_t *)lv_event_get_param(e);

        if (LV_KEY_BACKSPACE == key)
        {            
            screen_turn_prev_page();
        }   
		else if (LV_ENCODER_KEY_CC == key) //up
        {            
            my_list_scroll_refresh(local_view.scroll_cont,lv_map(ext->cur_sel, 0, ext->max, 0, 100));
        }
        else if (LV_ENCODER_KEY_CW == key) //down
        {   
            my_list_scroll_refresh(local_view.scroll_cont, lv_map(ext->cur_sel, 0, ext->max, 0, 100));
        }

    }    
}

static void error_code_event_cb(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);
	
    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_RELEASED)
    {  
		if(no_error_ignored)
		{
			 screen_turn_prev_page();
		}
		else
		{
			lv_obj_t *btn = lv_obj_get_child(obj, index);
			lv_obj_t *label = lv_obj_get_child(btn, 0);
			char *label_str = NULL;
			label_str = lv_label_get_text(label);
			for(uint8_t i = 0; i < 64; i++)
			{
				if(0 == memcmp(label_str, s_error_index[i].code_str, 12))
				{
					error_chose = s_error_index[i].id;
					screen_load_sub_page_with_stack(PRODUCT_INFO_ID_ERROR_CODE, index);
					break;
				}
			}
		}
    }
    else if (event == LV_EVENT_KEY)
    {        
        uint32_t key = *(uint32_t *)lv_event_get_param(e);

        if (LV_KEY_BACKSPACE == key)
        {            
            screen_turn_prev_page();
        }   
    }    
}

static void error_code_info_ignore_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    shortcut_keys_event_handler(e);
    if (event == LV_EVENT_RELEASED)
    {
		screen_turn_prev_page();
    }
    else if (event == LV_EVENT_KEY)
    {        
        uint32_t key = *(uint32_t *)lv_event_get_param(e);

        if (LV_KEY_BACKSPACE == key)
        {            
            screen_turn_prev_page();
        }   
    }    
}

void error_textarea_event_cb(lv_event_t* e)
{
	lv_obj_t* obj = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);
    uint8_t* tmp = lv_obj_get_user_data(obj);
    
    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
		if (key == LV_ENCODER_KEY_CW)
		{
			lv_textarea_cursor_down(obj);
		}
		else if (key == LV_ENCODER_KEY_CC)
		{
			lv_textarea_cursor_up(obj);

		}
	}
}

static void page_error_code_sub_construct(uint64_t id, bool sta)
{
	indev_group = lv_group_create();
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
	
	lv_obj_t* screen = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(screen);
    lv_obj_set_size(screen, lv_pct(100), lv_pct(100));
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_t* remedy = lv_label_create(screen);            
	lv_obj_set_style_text_font(remedy, Font_ResouceGet(FONT_22_BOLD), 0);
	lv_obj_set_style_text_color(remedy, lv_color_white(), 0);
	lv_obj_set_size(remedy, 320, 30);
	lv_label_set_text(remedy, Lang_GetStringByID(STRING_ID_ERROR_REMEDY));
	lv_obj_align(remedy, LV_ALIGN_TOP_LEFT, 25, SYS_MENU_TITLE_TO_TOP); 

	lv_obj_t* code_name =  lv_label_create(remedy); 
	page_error_code_lang_get(code_name, id);
	lv_obj_align(code_name, LV_ALIGN_CENTER, 25, 0);
	
	lv_obj_t* error_obj = lv_obj_create(screen);
	lv_obj_clear_flag(error_obj, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_remove_style_all(error_obj);    
	lv_obj_add_style(error_obj, &style_common_btn, 0);
	lv_obj_set_style_text_font(error_obj, Font_ResouceGet(FONT_18_MEDIUM), 0);	
	lv_obj_align(error_obj, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP+30);
	lv_obj_set_size(error_obj, 320, LV_SIZE_CONTENT);
	lv_obj_set_style_text_align(error_obj, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_t* error_name =  lv_label_create(error_obj); 
	lv_obj_set_style_text_align(error_name, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_long_mode(error_name, LV_LABEL_LONG_WRAP); 
	lv_obj_set_size(error_name, 320, LV_SIZE_CONTENT);
	page_error_code_set(error_name, id);
	lv_obj_align(error_name, LV_ALIGN_TOP_MID, 0, 5);

	lv_obj_t* textarea = lv_obj_create(screen);
	lv_obj_remove_style_all(textarea);
	lv_obj_set_style_text_color(textarea, lv_color_white(), 0);
	lv_obj_set_style_text_line_space(textarea, 0, 0);
	lv_obj_set_style_text_font(textarea, Font_ResouceGet(FONT_12), 0);
	lv_obj_set_size(textarea, 288, 116);
	lv_obj_align_to(textarea, error_name, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
			
	lv_obj_t* label_pane2 = lv_label_create(textarea);
	lv_obj_set_size(label_pane2, 288, LV_SIZE_CONTENT);
	lv_obj_set_style_text_color(label_pane2, lv_color_white(), 0);
	lv_obj_set_style_text_font(label_pane2, Font_ResouceGet(FONT_12), 0);
	lv_obj_set_style_text_align(label_pane2, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_long_mode(label_pane2, LV_LABEL_LONG_WRAP); 
	lv_obj_set_scroll_dir(label_pane2, LV_DIR_VER);
	page_error_info_set(label_pane2, id);
	lv_obj_align(label_pane2, LV_ALIGN_CENTER, 0, 0);
	
	lv_obj_t* ignored_obj = lv_obj_create(screen);      
	lv_obj_clear_flag(ignored_obj, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_style(ignored_obj, &style_common_btn, 0);
	lv_obj_add_style(ignored_obj, &style_common_focued_btn, LV_STATE_FOCUSED);
	lv_obj_set_style_text_color(ignored_obj, lv_color_white(), LV_STATE_FOCUSED); 
	lv_obj_set_size(ignored_obj, 85, 30);  
	lv_obj_align(ignored_obj, LV_ALIGN_BOTTOM_MID, 0, -5);
	lv_obj_set_style_pad_all(ignored_obj, 8, 0);
	lv_obj_set_style_pad_all(ignored_obj, 8, LV_STATE_FOCUSED);
	lv_obj_set_style_radius(ignored_obj, 8, 0);
	lv_obj_set_style_radius(ignored_obj, 8, LV_STATE_FOCUSED); 
	lv_obj_add_event_cb(ignored_obj, error_code_info_ignore_event_cb, LV_EVENT_ALL, 0);
	const char* str = get_error_laber_str(0);
	lv_obj_t* label = lv_label_create(ignored_obj);   
	lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_18_MEDIUM), 0); 
	lv_label_set_text(label, str);
	lv_obj_center(label);
	lv_obj_add_state(ignored_obj, LV_STATE_FOCUSED); 
	if(sta)
	{
		lv_obj_add_flag(ignored_obj, LV_OBJ_FLAG_HIDDEN);
		sys_box_lcd_lock(1);
		lv_group_add_obj(indev_group, ignored_obj);
		lv_group_focus_obj(ignored_obj);      
		lv_group_set_editing(indev_group, true);
	}
	else
	{
		lv_obj_clear_flag(ignored_obj, LV_OBJ_FLAG_HIDDEN);	
		lv_group_add_obj(indev_group, ignored_obj);
		lv_group_focus_obj(ignored_obj);      
		lv_group_set_editing(indev_group, true);
	}
	
}


static void page_error_code_set(lv_obj_t * obj, uint64_t id )
{
	switch(id)
	{
		case PRODUCT_INFO_ID_ERROR_EH_01:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EH_001));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_02:
			lv_label_set_text( obj, Lang_GetStringByID(STRING_ID_ERROR_EH_002));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_03:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EH_003));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_05:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EH_005));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_10:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EH_010));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_11:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EH_011));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_12:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EH_012));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_13:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EH_013));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_14:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EH_014));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_15:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EH_015));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_16:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EH_016));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_17:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EH_017));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_18:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EH_018));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_19:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EH_019));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_20:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EH_020));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_21:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EH_021));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_22:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EH_022));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_23:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EH_023));
			break;
		case PRODUCT_INFO_ID_ERROR_EC_02:
			lv_label_set_text( obj, Lang_GetStringByID(STRING_ID_ERROR_EC_002));
			break;
		case PRODUCT_INFO_ID_ERROR_EC_03:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EC_003));
			break;
		case PRODUCT_INFO_ID_ERROR_EC_06:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EC_006));
			break;
		case PRODUCT_INFO_ID_ERROR_EC_07:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EC_007));
			break;
		case PRODUCT_INFO_ID_ERROR_EC_09:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EC_009));
			break;
		case PRODUCT_INFO_ID_ERROR_EC_10:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EC_010));
			break;
		case PRODUCT_INFO_ID_ERROR_EC_11:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EC_011));
			break;
		case PRODUCT_INFO_ID_ERROR_EC_12:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EC_012));
			break;
		case PRODUCT_INFO_ID_ERROR_EC_13:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EC_013));
			break;
		case PRODUCT_INFO_ID_ERROR_EC_14:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_EC_014));
			break;	
		default:break;
	}
}


static void page_error_info_set(lv_obj_t * obj, uint64_t id )
{
	switch(id)
	{
		case PRODUCT_INFO_ID_ERROR_EH_01:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EH_001));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_02:
			lv_label_set_text( obj, Lang_GetStringByID(STRING_ID_ERROR_DEAL_EH_002));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_03:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EH_003));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_05:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EH_005));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_10:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EC_010));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_11:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EH_011));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_12:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EH_012));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_13:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EH_013));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_14:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EH_014));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_15:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EH_015));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_16:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EH_016));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_17:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EH_017));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_18:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EH_018));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_19:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EH_019));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_20:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EH_020));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_21:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EH_021));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_22:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EH_022));
			break;
		case PRODUCT_INFO_ID_ERROR_EH_23:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EH_023));
			break;
		case PRODUCT_INFO_ID_ERROR_EC_02:
			lv_label_set_text( obj, Lang_GetStringByID(STRING_ID_ERROR_DEAL_EC_002));
			break;
		case PRODUCT_INFO_ID_ERROR_EC_03:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EC_003));
			break;
		case PRODUCT_INFO_ID_ERROR_EC_06:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EC_006));
			break;
		case PRODUCT_INFO_ID_ERROR_EC_07:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EC_007));
			break;
		case PRODUCT_INFO_ID_ERROR_EC_09:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EC_009));
			break;
		case PRODUCT_INFO_ID_ERROR_EC_10:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EC_010));
			break;	
		case PRODUCT_INFO_ID_ERROR_EC_11:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EC_011));
			break;
		case PRODUCT_INFO_ID_ERROR_EC_12:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EC_012));
			break;
		case PRODUCT_INFO_ID_ERROR_EC_13:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EC_013));
			break;
		case PRODUCT_INFO_ID_ERROR_EC_14:
			lv_label_set_text( obj,Lang_GetStringByID(STRING_ID_ERROR_DEAL_EC_014));
			break;
		default  :
			break;
	}
}

void page_error_return_ui(void)
{
	uint8_t res = 0;
	if(info_record.eh_01 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_01)) == 0)
		{
			res = 1;
			info_record.eh_01 = false;
			ui_error_code_event_set(0);
		}
	}
	
	if(info_record.eh_02 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_02)) == 0)
		{
			res = 1;
			ui_error_code_event_set(0);
			info_record.eh_02 = false;
		}
	}
	if(info_record.eh_03 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_03)) == 0)
		{
			res = 1;
			ui_error_code_event_set(0);
			info_record.eh_03 = false;
		}
	}
	if(info_record.eh_05 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_05)) == 0)
		{
			res = 1;
			info_record.eh_05 = false;
		}
	}
	if(info_record.eh_10 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_10)) == 0)
		{
			res = 1;
			ui_error_code_event_set(0);
			info_record.eh_10 = false;
		}
	}
	if(info_record.eh_11 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_11)) == 0)
		{
			res = 1;
			info_record.eh_11 = false;
		}
	}
	if(info_record.eh_12 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_12)) == 0)
		{
			res = 1;
			ui_error_code_event_set(0);
			info_record.eh_12 = false;
		}
	}
		if(info_record.eh_13 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_13)) == 0)
		{
			res = 1;
			info_record.eh_13 = false;
		}
	}
		if(info_record.eh_14 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_14)) == 0)
		{
			res = 1;
			info_record.eh_14 = false;
		}
	}
	if(info_record.eh_15 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_15)) == 0)
		{
			res = 1;
			info_record.eh_15 = false;
		}
	}
	if(info_record.eh_16 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_16)) == 0)
		{
			res = 1;
			ui_error_code_event_set(0);
			info_record.eh_16 = false;
		}
	}
	if(info_record.eh_17 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_17)) == 0)
		{
			res = 1;
			info_record.eh_17 = false;
		}
	}
	if(info_record.eh_18 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_18)) == 0)
		{
			res = 1;
			info_record.eh_18 = false;
		}
	}
	if(info_record.eh_19 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_19)) == 0)
		{
			res = 1;
			info_record.eh_19 = false;
		}
	}
	if(info_record.eh_20 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_20)) == 0)
		{
			res = 1;
			info_record.eh_20 = false;
		}
	}
	if(info_record.eh_21 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_21)) == 0)
		{
			res = 1;
			info_record.eh_21 = false;
		}
	}
	if(info_record.eh_22 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_22)) == 0)
		{
			res = 1;
			info_record.eh_22 = false;
		}
	}
	if(info_record.eh_23 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_23)) == 0)
		{
			res = 1;
			info_record.eh_23 = false;
		}
	}
	if(info_record.ec_02 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_02)) == 0)
		{
			res = 1;
			ui_error_code_event_set(0);
			info_record.ec_02 = false;
		}
	}
	if(info_record.ec_03 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_03)) == 0)
		{
			res = 1;
			ui_error_code_event_set(0);
			info_record.ec_03 = false;
		}
	}
		if(info_record.ec_05 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_05)) == 0)
		{
			res = 1;
			info_record.ec_05 = false;
		}
	}
	if(info_record.ec_06 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_06)) == 0)
		{
			res = 1;
			info_record.ec_06 = false;
		}
	}
	if(info_record.ec_07 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_07)) == 0)
		{
			res = 1;
			info_record.ec_07 = false;
		}
	}
	
		if(info_record.ec_09 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_09)) == 0)
		{
			res = 1;
			info_record.ec_09 = false;
		}
	}
	
	if(info_record.ec_10 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_10)) == 0)
		{
			res = 1;
			ui_error_code_event_set(0);
			info_record.ec_10 = false;
		}
	}
	if(info_record.ec_11 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_11)) == 0)
		{
			res = 1;
			info_record.ec_11 = false;
		}
	}
	if(info_record.ec_12 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_12)) == 0)
		{
			res = 1;
			ui_error_code_event_set(0);
			info_record.ec_12 = false;
		}
	}
	if(info_record.ec_13 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_13)) == 0)
		{
			res = 1;
			info_record.ec_13 = false;
		}
	}
	if(info_record.ec_14 == true)
	{
		if((error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_14)) == 0)
		{
			res = 1;
			ui_error_code_event_set(0);
			info_record.ec_14 = false;
		}
	}
	if(res == 1)
	{
		res = 0;
		screen_turn_prev_page();
		sys_box_lcd_lock(0);
		error_time = 500;

	}

}

void page_error_info_loop(void)
{
	static uint8_t error_step = 0;
	static uint8_t error_event = 0;
	if (screen_get_act_pid() == PAGE_TEST || screen_get_act_pid() == PAGE_CALIBRATION_MODE) return;
	if ((screen_get_act_pid() == PAGE_UPDATE && screen_get_act_spid() == SUB_ID_UPDATING) || 
		(PAGE_WORK_MODE == screen_get_act_pid() && screen_get_act_spid() == SUB_ID_SYNC_UPDATAING)) return;
	switch (error_step)
	{
		case 0:
		{	
			error_code.error_data  = ui_get_error_type();
			page_error_return_ui();
			if(error_time > 0) {error_time-- ; }	
			if(error_code.error_data == 0) return ;
			error_ignored = 1;	
			if((PAGE_PRODUCT_INFO == screen_get_act_pid() && PRODUCT_INFO_ID_ERROR_INFO == screen_get_act_spid()) || 
			  (PAGE_PRODUCT_INFO == screen_get_act_pid() && PRODUCT_INFO_ID_ERROR_CODE == screen_get_act_spid()))
			{
				error_event = 0;
			}
			else{
				error_event = 1;
				
			}
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_01))
			{
				info_record.eh_01 = 1;
				ui_error_code_event_set(1);
				if(error_event && error_time == 0) {screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};
				window_index = PRODUCT_INFO_ID_ERROR_EH_01;
			}
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_02))
			{
				info_record.eh_02 = true;
				ui_error_code_event_set(1);
				if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};	
				window_index = PRODUCT_INFO_ID_ERROR_EH_02;
			}				
			else if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_03))
			{
				info_record.eh_03 = true;
				ui_error_code_event_set(1);
				if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};
				window_index = PRODUCT_INFO_ID_ERROR_EH_03;
			}
			else if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_05))
			{
				info_record.eh_05 = true;
				if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};	
				window_index = PRODUCT_INFO_ID_ERROR_EH_05;
			}
			else if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_10))
			{
				info_record.eh_10 = true;
				ui_error_code_event_set(1);
				if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};		
				window_index = PRODUCT_INFO_ID_ERROR_EH_10;
			}			
			else if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_12))
			{
				info_record.eh_12 = true;	
				ui_error_code_event_set(1);
				if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};	
				window_index = PRODUCT_INFO_ID_ERROR_EH_12;
			}
			else if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_15))
			{
				info_record.eh_15 = true;
				if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};
				window_index = PRODUCT_INFO_ID_ERROR_EH_15;
			}
			else if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_16))
			{
				info_record.eh_16 = true;
				ui_error_code_event_set(1);
				if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};
				window_index = PRODUCT_INFO_ID_ERROR_EH_16;
			}
			else if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_17))
			{
				info_record.eh_17 = true;
				if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};
				window_index = PRODUCT_INFO_ID_ERROR_EH_17;
			}
			else if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_21))
			{
				info_record.eh_21 = true;
				ui_error_code_event_set(1);
				if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};
				window_index = PRODUCT_INFO_ID_ERROR_EH_21;
			}
			else if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_22))
			{
				info_record.eh_22 = true;
				ui_error_code_event_set(1);
				if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};
				window_index = PRODUCT_INFO_ID_ERROR_EH_22;
			}
			else if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_23))
			{
				info_record.eh_23 = true;
				ui_error_code_event_set(1);
				if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};
				window_index = PRODUCT_INFO_ID_ERROR_EH_23;
			}
			else if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_02))
			{
				info_record.ec_02 = true;
				ui_error_code_event_set(1);
				if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};
				window_index = PRODUCT_INFO_ID_ERROR_EC_02;
			}
			else if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_03))
			{
				info_record.ec_03 = true;
				ui_error_code_event_set(1);
				if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};
				window_index = PRODUCT_INFO_ID_ERROR_EC_03;
			}
			else if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_06))
			{
				ui_error_code_event_set(1);
				info_record.ec_06 = true;
				if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};	
				window_index = PRODUCT_INFO_ID_ERROR_EC_06;
			}				
			else if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_10))
			{
				ui_error_code_event_set(1);
				info_record.ec_10 = true;
				if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};	
				window_index = PRODUCT_INFO_ID_ERROR_EC_10;
			}						
			else if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_11))
			{
				info_record.ec_11 = true;
				if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};	
				window_index = PRODUCT_INFO_ID_ERROR_EC_11;
			}	
			else if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_12))
			{
				ui_error_code_event_set(1);
				info_record.ec_12 = true;
				if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};	
				window_index = PRODUCT_INFO_ID_ERROR_EC_12;
			}	
			else if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_13))
			{
				info_record.ec_13 = true;
				if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};	
				window_index = PRODUCT_INFO_ID_ERROR_EC_13;
			}	
			else if(error_code.error_data &  gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_14))
			{
				ui_error_code_event_set(1);
				info_record.ec_14 = true;
				if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};	
				window_index = PRODUCT_INFO_ID_ERROR_EC_14;
			}	
			else if(error_code.error_data &  gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_11) || \
				error_code.error_data &  gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_13) || error_code.error_data &  gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_14)|| \
				error_code.error_data &  gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_18) || error_code.error_data &  gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_19) || \
				error_code.error_data &  gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_20) || error_code.error_data &  gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_07) || \
				error_code.error_data &  gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_09))
			{
		
				if(info_record.eh_11 == 0 && (error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_11)))
				{
					if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};
					window_index = PRODUCT_INFO_ID_ERROR_EH_11;
				}
				else if(info_record.eh_13 == 0 && (error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_13))) 
				{
					if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};
					window_index = PRODUCT_INFO_ID_ERROR_EH_13;
				}
				else if(info_record.eh_14 == 0 && (error_code.error_data &  gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_14 )))
				{
					if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};
					window_index = PRODUCT_INFO_ID_ERROR_EH_14;
				}
				else if(info_record.eh_18 == 0 && (error_code.error_data &  gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_18 )))
				{
					info_record.eh_18 = true;
					if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};
					window_index = PRODUCT_INFO_ID_ERROR_EH_18;
				}
				else if(info_record.eh_19 == 0 && (error_code.error_data &  gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_19 )))
				{
					info_record.eh_19 = true;
					if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};
					window_index = PRODUCT_INFO_ID_ERROR_EH_19;
				}
				else if(info_record.eh_20 == 0 && (error_code.error_data &  gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_20 )))
				{
					info_record.eh_20 = true;
					if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};
					window_index = PRODUCT_INFO_ID_ERROR_EH_20;
				}
				else if(info_record.ec_07 == 0&& (error_code.error_data &  gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_07)))
				{
					if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};
					window_index = PRODUCT_INFO_ID_ERROR_EC_07;
				}
				else if(info_record.ec_09 == 0 && (error_code.error_data &  gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EC_09)))
				{
					if(error_event && error_time == 0){screen_load_page_and_index(PAGE_PRODUCT_INFO, PRODUCT_INFO_ID_ERROR_INFO, 0,1);error_step = 1;};
					window_index = PRODUCT_INFO_ID_ERROR_EC_09;
				}
			}
		}
		break;
		case 1:
			if(error_code.error_data & gui_get_64type_convert(PRODUCT_INFO_ID_ERROR_EH_01)) 
				error_time = 6000;
			else
				error_time = 1000;
				error_step = 0;
		break;
	}
}

static void page_error_code_lang_get(lv_obj_t * obj, uint64_t id )
{
	for(uint8_t i = 0; i < 64; i++)
	{
		if(id == s_error_index[i].id)
		{
			lv_label_set_text(obj,s_error_index[i].code_str);
			return;
		}
	}
}

static const char* get_error_laber_str(uint8_t  type)
{
    const char* str = NULL;
    switch(type)
    {
        case 0: str = Lang_GetStringByID(STRING_ID_ERROR_IGNORE); break;
        case 1: str = Lang_GetStringByID(STRING_ID_ERROR_REMEDY); break;
      
    }
    return str;
}

