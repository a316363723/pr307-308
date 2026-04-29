/*********************
 *      INCLUDES
 *********************/
#include "../ui_common.h"
#include "../page_widget.h"

#include <stdio.h>


#include "dev_fan.h"

/*********************
 *      DEFINES
 *********************/
#define FAN_MAX_SPEED       10000    
#define FAN_MIN_SPEED       0
#define RGBWW_MAX_INT       12000
#define RGBWW_MIN_INT       0
#define TEXT_COLOR          lv_color_make(11, 244, 122)
/*********************
 *      TPEDEFS
 *********************/
//static adj_btn_t btn_list[7] =
//{

//    ADJ_BTN_INITIALIZER(UI_RGB_MIN, UI_RGB_MAX, UI_RGB_STEP, UI_RGB_DEF_VAL, "R", "", 13, 39, -3, false, false,  rgb_get_value_str, NULL, NULL, NULL, NULL), 
//    ADJ_BTN_INITIALIZER(UI_RGB_MIN, UI_RGB_MAX, UI_RGB_STEP, UI_RGB_DEF_VAL, "G", "", 13, 39, -3, false, false,  rgb_get_value_str, NULL, NULL, NULL, NULL), 
//    ADJ_BTN_INITIALIZER(UI_RGB_MIN, UI_RGB_MAX, UI_RGB_STEP, UI_RGB_DEF_VAL, "B", "", 13, 39, -3, false, false,  rgb_get_value_str, NULL, NULL, NULL, NULL), 
//    ADJ_BTN_INITIALIZER(UI_RGB_MIN, UI_RGB_MAX, UI_RGB_STEP, UI_RGB_DEF_VAL, "WW1", "", 13, 39, -3, false, false,  rgb_get_value_str, NULL, NULL, NULL, NULL), 
//    ADJ_BTN_INITIALIZER(UI_RGB_MIN, UI_RGB_MAX, UI_RGB_STEP, UI_RGB_DEF_VAL, "WW2", "", 13, 39, -3, false, false,  rgb_get_value_str, NULL, NULL, NULL, NULL), 
//    ADJ_BTN_INITIALIZER(UI_RGB_MIN, UI_RGB_MAX, UI_RGB_STEP, UI_RGB_DEF_VAL, "CW1", "", 13, 39, -3, false, false,  rgb_get_value_str, NULL, NULL, NULL, NULL), 
//    ADJ_BTN_INITIALIZER(UI_RGB_MIN, UI_RGB_MAX, UI_RGB_STEP, UI_RGB_DEF_VAL, "CW2", "", 13, 39, -3, false, false,  rgb_get_value_str, NULL, NULL, NULL, NULL), 
//   
//};
//测试界面子页面
enum {
    SUB_ID_UITEST_ENTRY = 0,
    SUB_ID_UITEST_POWER_DIPLAY,
    SUB_ID_UITEST_FAN_CONTROL,
    SUB_ID_UITEST_POWER_CALIB,
    SUB_ID_UITEST_INDUSTRIAL_MODE,
    SUB_ID_UITEST_RGBWW_MODE,
    SUB_ID_UITEST_WATER_PUMP,
	SUB_ID_UITEST_MOTOR,
	SUB_ID_UITEST_POWER_SET,
	SUB_ID_UITEST_POWER_CRC,
	SUB_ID_UITEST_CLEAR_TIME,
	SUB_ID_UITEST_FRE_INFO,
	SUB_ID_UITEST_FRESNEL_INDUSTRIAL_MODE,
};

enum {
    LABEL_ID_SYS = 0,
    LABEL_ID_BAT1,
    LABEL_ID_BAT2,
    LABEL_ID_ADAPTER,
    LABEL_ID_LAMP_NTC,
    LABEL_ID_BOX_NTC,
    LABEL_ID_OUTPUTPOWER,
    LABEL_ID_REMAINTIME,
    LABEL_ID_MAX,
};

enum {
    LABEL_F14_TEMP1 = 0,
    LABEL_F14_TEMP2,
    LABEL_TILT_L_HW,
    LABEL_TILT_L_SW,
    LABEL_TILT_R_HW,
    LABEL_TILT_R_SW,
	LABEL_PAN_HW,
    LABEL_PAN_SW,
    LABEL_ID_MAX_NUM,
};

enum {
    LABEL_ID_TARGET_SPD,
    LABEL_ID_ACTUAL_SPD,
};

typedef struct {    
    my_list_ext_t list;
    union {
        struct {
            lv_obj_t* label[LABEL_ID_MAX];
        } power_disp;
		struct {
            lv_obj_t* label[LABEL_ID_MAX_NUM];
        } info_disp;
        struct {
            lv_obj_t* spd_label[2][2];
        } fan_ctrl;

        struct {
            lv_obj_t* power_label;
            lv_obj_t* state_label;
        }power_calib;
		struct {
            lv_obj_t* power_label;
            lv_obj_t* state_label;
        }single_power_calib;
		
    } sub_page;
}test_view_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void page_uitest_construct(void);
static void page_uitest_destruct(void);
static void page_uitest_upd_cb(uint32_t ms);

static void slider_event_cb(lv_event_t* e);
static void uitest_index_entry(lv_obj_t* parent);
static void uitest_power_display_init(lv_obj_t* parent);
static void uitest_power_fancontrol_init(lv_obj_t* parent);
static void uitest_power_powercalibrate_init(lv_obj_t* parent);;
static void uitest_motor_init(lv_obj_t* parent);

static void uitest_list_event_cb(lv_event_t* e, uint8_t index);
//static void uitest_rgbww_event(lv_event_t* e);
static void refresh_power_disp(void);
static void refresh_info_disp(void);
static void refresh_power_calibrate(void);
static void refresh_fanctrl_screen(void);
static void uitest_industrial_init(lv_obj_t* parent);
static void uitest_common_event(lv_event_t* e);
static void rgbww_event_cb(lv_event_t* e);
static void uitest_power_rgbwwcontrol_init(lv_obj_t* parent);
static void uitest_power_water_pump_init(lv_obj_t* parent);
static void pump_event_cb(lv_event_t* e);
static void motor_test_event_cb(lv_event_t* e);
//static void switch_btn_event_cb(lv_event_t* e);
static void uitest_clear_time_event(lv_event_t* e);
//static void limit_btn_event_cb(lv_event_t* e);
static void refresh_motor_disp(void);
static void uitest_power_init(lv_obj_t* parent);
static void refresh_power_crc(void);
static void uitest_power_crc_init(lv_obj_t* parent);
static void uitest_clear_time_init(lv_obj_t* parent);
//static void motor_demo_text_init(lv_obj_t *parent, int16_t *limit_value);
static void uitest_motor_info_init(lv_obj_t* parent);
static void uitest_Fresnel_industrial_init(lv_obj_t* parent);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t *indev_group;
static UI_State_Data_t* p_model = NULL;
static uint32_t disp_time = 0;
static test_view_t view;
static lv_obj_t* motor_text[4];
static lv_obj_t* motor_demo_list = NULL;
static my_list_ext_t list_ext;
static my_list_ext_t s_list_ext[3];
static uint8_t fan_mode = 0;
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_uitest = PAGE_INITIALIZER("Test",
                                PAGE_TEST, 
                                page_uitest_construct, 
                                page_uitest_destruct, 
                                page_uitest_upd_cb,
                                NULL,                                
                                PAGE_NONE);
/*********************
 *  GLOBAL FUNCTIONS
 *********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/    

/**
 * @brief 测试页面构造函数
 * 
 */
static void page_uitest_construct(void)
{
    data_center_read_config_data(SYS_CONFIG_FAN_MODE, &fan_mode);
    
    indev_group = lv_group_create();
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
    lv_obj_set_style_bg_color(lv_scr_act(),  lv_color_make(40,40,40), 0);

    p_model = screen_get_act_page_data();
    disp_time = 0;
    (void)disp_time;

    lv_obj_t* scr = lv_obj_create(lv_scr_act());
    lv_obj_add_style(scr, &style_common_btn, 0);
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
    lv_obj_set_size(scr, lv_pct(100), lv_pct(100));
    lv_obj_set_style_text_font(scr, Font_ResouceGet(FONT_14), 0);    
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(scr);
    
    uint8_t subid = screen_get_act_spid() > SUB_ID_UITEST_FRESNEL_INDUSTRIAL_MODE ? 0 : screen_get_act_spid();
	g_ptUIStateData->test_data.clear_run_time_flag = 0;
	g_ptUIStateData->test_data.clear_davice_run_time_flag = 0;
	
    switch (subid)
    {
        case SUB_ID_UITEST_ENTRY:
            uitest_index_entry(scr);      
            p_model->test_data.PwmMode = INDUSTRIAL_MODE_IDLE;
            p_model->test_data.PumpMode = INDUSTRIAL_MODE_IDLE;
            break;
        case SUB_ID_UITEST_POWER_DIPLAY:    
            uitest_power_display_init(scr);
            lv_obj_add_event_cb(scr, uitest_common_event, LV_EVENT_CLICKED, NULL);
            lv_group_add_obj(indev_group, scr);
            lv_group_focus_obj(scr);
            break;
        case SUB_ID_UITEST_FAN_CONTROL:
            uitest_power_fancontrol_init(scr); 
            break;
        case SUB_ID_UITEST_POWER_CALIB:
        {
            p_model->test_data.CalibrateState = CALIBRATE_POWER_PWM_START;
            uitest_power_powercalibrate_init(scr); 
            lv_obj_add_event_cb(scr, uitest_common_event, LV_EVENT_CLICKED, NULL);
            lv_group_add_obj(indev_group, scr);
            lv_group_focus_obj(scr);
        }
        break;
        case SUB_ID_UITEST_INDUSTRIAL_MODE:
            p_model->test_data.IndustrialMode = INDUSTRIAL_MODE_START;
            uitest_industrial_init(scr);
            lv_obj_add_event_cb(scr, uitest_common_event, LV_EVENT_CLICKED, NULL);
            lv_group_add_obj(indev_group, scr);
            lv_group_focus_obj(scr);
            break;
        case SUB_ID_UITEST_RGBWW_MODE:
            p_model->test_data.PwmMode = INDUSTRIAL_MODE_START;
            uitest_power_rgbwwcontrol_init(scr);
			ui_test_rgbpwm_mode_set(1);
            break;
        case SUB_ID_UITEST_WATER_PUMP:
             p_model->test_data.PumpMode = INDUSTRIAL_MODE_START;
             uitest_power_water_pump_init(scr);
            break;
		case SUB_ID_UITEST_MOTOR:
			uitest_motor_init(scr);
			break;
		case SUB_ID_UITEST_POWER_SET:
			ui_test_power_mode_set(1);
			g_ptUIStateData->test_data.power_set_value = 0;
			uitest_power_init(scr);
		break;
		case SUB_ID_UITEST_POWER_CRC:
			p_model->test_data.powercalibrateState = CALIBRATE_POWER_PWM_START;
			uitest_power_crc_init(scr);
			lv_obj_add_event_cb(scr, uitest_common_event, LV_EVENT_CLICKED, NULL);
            lv_group_add_obj(indev_group, scr);
            lv_group_focus_obj(scr);
		break;
		case SUB_ID_UITEST_CLEAR_TIME:
			uitest_clear_time_init(scr);
			lv_obj_add_event_cb(scr, uitest_clear_time_event, LV_EVENT_ALL, NULL);
			lv_group_add_obj(indev_group,  scr);
			lv_group_focus_obj(scr);
		break;
		case SUB_ID_UITEST_FRE_INFO:
			uitest_motor_info_init(scr);
			lv_obj_add_event_cb(scr, uitest_common_event, LV_EVENT_CLICKED, NULL);
			lv_group_add_obj(indev_group, scr);
            lv_group_focus_obj(scr);
			refresh_info_disp();
		break;
		case SUB_ID_UITEST_FRESNEL_INDUSTRIAL_MODE:
			p_model->test_data.Fresnel_Industrial = INDUSTRIAL_MODE_START;
            uitest_Fresnel_industrial_init(scr);
            lv_obj_add_event_cb(scr, uitest_common_event, LV_EVENT_CLICKED, NULL);
            lv_group_add_obj(indev_group, scr);
            lv_group_focus_obj(scr);
		break;	
        default: break;
    }
}

/**
 * @brief 测试页面析构函数
 * 
 */
static void page_uitest_destruct(void)
{
	ui_test_rgbpwm_mode_set(0);
//	ui_test_power_mode_set(2);
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;
    lv_obj_clean(lv_scr_act());
}

/**
 * @brief 定时页面刷新函数
 * 
 * @param ms 
 */
static void page_uitest_upd_cb(uint32_t ms)
{
    uint8_t spid = screen_get_act_spid();
	
    if (SUB_ID_UITEST_FAN_CONTROL == spid)
    {
        refresh_fanctrl_screen();
    }
    else if (SUB_ID_UITEST_POWER_CALIB == spid)
    {
        refresh_power_calibrate();
    }
    else if (SUB_ID_UITEST_POWER_DIPLAY == spid)
    {
        refresh_power_disp();
    }
	else if (SUB_ID_UITEST_MOTOR == spid)
    {
		refresh_motor_disp();
    }
	else if (SUB_ID_UITEST_POWER_CRC == spid)
    {
		refresh_power_crc();
    }
	else if(SUB_ID_UITEST_FRE_INFO == spid)
	{
		refresh_info_disp();
	}

}

/**
 * @brief 测试页面选项.
 * 
 * @param parent 
 */
static void uitest_index_entry(lv_obj_t* parent)
{
    lv_obj_t* title = lv_label_create(lv_scr_act());
    
    lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22), 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_label_set_text(title, "Test");
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);   

    view.list.cur_sel = screen_get_act_index();  
    view.list.release_cb = uitest_list_event_cb;;
    lv_obj_t* list = my_list_create(parent, lv_pct(79), lv_pct(80), &view.list); 
    lv_obj_set_flex_align(list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);                       
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 38);
    
    my_list_add_btn(list, "Power params", 10);
    my_list_add_btn(list, "Fan Control", 10);
    my_list_add_btn(list, "Power Calibrate", 10);
    my_list_add_btn(list ,"Industrial mode" , 10);
    my_list_add_btn(list ,"RGBWW mode" , 10);
    my_list_add_btn(list ,"Water Pump" , 10);
    my_list_add_btn(list ,"Motor Test" , 10);
	my_list_add_btn(list ,"Power Set Test" , 10);
	my_list_add_btn(list ,"Single Power Calibrate" , 10);
	my_list_add_btn(list ,"Clear Time" , 10);
	my_list_add_btn(list ,"Fresnel Info" , 10);
	my_list_add_btn(list ,"Fresnel Industrial" , 10);
    lv_group_add_obj(indev_group, list);
    lv_group_focus_obj(list);
    lv_group_set_editing(indev_group, true);   
}

/**
 * @brief 创建功率参数显示界面
 * 
 * @param parent 
 */
static void uitest_power_display_init(lv_obj_t* parent)
{
    static const char* string[] = {
        "System:",
        "BAT1-V/Ic/Id:",
        "BAT2-V/Ic/Id:",
        "Adapter-V/I:",
        "Lamp Temperature:",
        "ControlBox Temp:",
        "Output Power:",
        "Remain Time: "
    };

    //测试界面标题
    lv_obj_t* label;
    label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_22), 0);
    lv_label_set_text(label, "Power Parameters");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t* label_cont = lv_obj_create(parent);
    lv_obj_add_style(label_cont, &style_common_btn, 0);
    lv_obj_set_style_bg_color(label_cont, lv_color_black(), 0);
    lv_obj_set_style_text_font(label_cont, Font_ResouceGet(FONT_14), 0);    
    lv_obj_set_size(label_cont, lv_pct(100), lv_pct(90));    
    lv_obj_set_pos(label_cont, 0, 40);

   
    for (uint8_t i = 0; i < sizeof(string) / sizeof(char*); i++)
    {
        label = lv_label_create(label_cont);
        lv_label_set_text(label, string[i]);    
        lv_obj_set_pos(label, 20, i * 24);       
    }

    for (uint8_t i = 0; i < sizeof(string) / sizeof(char*); i++)
    {
        view.sub_page.power_disp.label[i]  = lv_label_create(label_cont);
        lv_label_set_recolor(view.sub_page.power_disp.label[i], true);
        lv_obj_set_style_text_color(view.sub_page.power_disp.label[i], TEXT_COLOR, 0);
        lv_obj_set_size(view.sub_page.power_disp.label[i] , 160, 24);            
        lv_obj_set_pos(view.sub_page.power_disp.label[i] , 160, i * 24);   
    }

    refresh_power_disp();
}

/**
 * @brief 创建功率参数显示界面
 * 
 * @param parent 
 */
static void uitest_industrial_init(lv_obj_t* parent)
{
    static const char* string[] = {
        "Please start lighting"
    };

       //测试界面标题
    lv_obj_t* label;
    label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_22), 0);
    lv_label_set_text(label, "Industrial mode");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t* label_cont = lv_obj_create(parent);
    lv_obj_add_style(label_cont, &style_common_btn, 0);
    lv_obj_set_style_bg_color(label_cont, lv_color_black(), 0);
    lv_obj_set_style_text_font(label_cont, Font_ResouceGet(FONT_22), 0);    
    lv_obj_set_size(label_cont, lv_pct(100), lv_pct(90));    
    lv_obj_set_pos(label_cont, 0, 40);

    label = lv_label_create(label_cont);
    lv_label_set_text(label, string[0]);    
    lv_obj_set_pos(label, 60,  80);       

}



/**
 * @brief 创建功率参数显示界面
 * 
 * @param parent 
 */
static void uitest_Fresnel_industrial_init(lv_obj_t* parent)
{
    static const char* string[] = {
        "Fresnel 1500° "
    };

       //测试界面标题
    lv_obj_t* label;
    label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_22), 0);
    lv_label_set_text(label, "Fresnel Industrial");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t* label_cont = lv_obj_create(parent);
    lv_obj_add_style(label_cont, &style_common_btn, 0);
    lv_obj_set_style_bg_color(label_cont, lv_color_black(), 0);
    lv_obj_set_style_text_font(label_cont, Font_ResouceGet(FONT_22), 0);    
    lv_obj_set_size(label_cont, lv_pct(100), lv_pct(90));    
    lv_obj_set_pos(label_cont, 0, 40);

    label = lv_label_create(label_cont);
    lv_label_set_text(label, string[0]);    
    lv_obj_set_pos(label, 60,  80);       

}

/**
 * @brief 刷新电动支架测试页面
 * 
 */
static void refresh_motor_disp(void)
{
	struct sys_info_motor_state  motor_state;
	char str[10] = {0};
	data_center_read_sys_info(SYS_INFO_MOTOR_STATE, &motor_state);
	lv_snprintf(str, sizeof(str), "%d", motor_state.fresnel_loop_count);
	lv_label_set_text(motor_text[3], str);
	
}

/**
 * @brief 刷新功率参数显示页面
 * 
 */
static void refresh_power_disp(void)
{

    lv_obj_t* label;
    for (uint8_t i = 0; i < LABEL_ID_MAX; i++)
    {
        label = view.sub_page.power_disp.label[i];
        switch (i)
        {
        case LABEL_ID_SYS:
            lv_snprintf(sdb, sizeof(sdb), "%d#ffffff mV/#%d#ffffff mA#", p_model->test_data.DC48_Vout_mV, p_model->test_data.DC48_Iout_mA);
            break;
        case LABEL_ID_BAT1:
            lv_snprintf(sdb, sizeof(sdb), "%d#ffffff mV/#%d#ffffff mA/#%d#ffffff mA#", p_model->test_data.Bat1Vout_mV,  
                                                    p_model->test_data.Bat1ChargeCurrent_mA, 
                                                    p_model->test_data.Bat1DischargeCurrent_mA);
            break;
        case LABEL_ID_BAT2:
            lv_snprintf(sdb, sizeof(sdb), "%d#ffffff mV/#%d#ffffff mA/#%d#ffffff mA#", p_model->test_data.Bat2Vout_mV,  
                                                    p_model->test_data.Bat2ChargeCurrent_mA, 
                                                    p_model->test_data.Bat2DischargeCurrent_mA);        
            break;
        case LABEL_ID_ADAPTER:
            lv_snprintf(sdb, sizeof(sdb), "%d#ffffff mV#/%d#ffffff mA#", p_model->test_data.AdapterVout_mV,  
                                                    p_model->test_data.AdapterIout_mA);
            break;
        case LABEL_ID_BOX_NTC:
            lv_snprintf(sdb, sizeof(sdb), "%d℃", p_model->test_data.NTCTemp);     
            break;
        case LABEL_ID_LAMP_NTC:
            lv_snprintf(sdb, sizeof(sdb), "%d℃", p_model->test_data.LampNTCTemp);
            break;
        case LABEL_ID_OUTPUTPOWER:
            lv_snprintf(sdb, sizeof(sdb), "%dW", (long)p_model->test_data.SupplyPower);
            break;            
        case LABEL_ID_REMAINTIME:
            lv_snprintf(sdb, sizeof(sdb), "%d h %02d min", p_model->test_data.RemainTime / 3600,  (p_model->test_data.RemainTime % 3600) / 60);
            break;
        }
        lv_label_set_text(label, sdb);
    }
}

/**
 * @brief 风扇控制页面
 * 
 * @param parent 
 */
static void uitest_power_fancontrol_init(lv_obj_t* parent)
{
    const char* spd_string[] = {"Target Speed:", "Actual Speed:"};
    const char* title[] = {"Box", "Lamp"};
    uint8_t i = 0;

    lv_obj_t* label;

    label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_22), 0);
    lv_label_set_text(label, "Fan Speed Control");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    for (i = 0; i < 2; i++)
    {   
        label = lv_label_create(parent);
        lv_label_set_text(label, title[i]); 
        lv_obj_set_pos(label, 160 * i + 0, 40);
        
        //target speed
        label = lv_label_create(parent);
        lv_label_set_text(label, spd_string[0]);
        lv_obj_set_pos(label,  160 * i , 40 + 25);            

        //real speed
        label = lv_label_create(parent);
        lv_label_set_text(label, spd_string[1]);        
        lv_obj_set_pos(label,  160 * i , 40 + 25 + 25);

        //target speed number
        label = lv_label_create(parent);
        lv_label_set_text(label, "9999");
        lv_obj_set_pos(label,  160 * i + 95, 40 + 25);
        lv_obj_set_style_text_color(label, TEXT_COLOR, 0);
        view.sub_page.fan_ctrl.spd_label[i][0]  = label;

        //real speed number
        label = lv_label_create(parent);
        lv_label_set_text(label, "9999");
        lv_obj_set_pos(label,  160 * i + 95, 40 + 25 + 25);
        lv_obj_set_style_text_color(label, TEXT_COLOR, 0);
        view.sub_page.fan_ctrl.spd_label[i][1]  = label;     
    }

    for (i = 0; i < 2; i++)
    {
        lv_obj_t* slider_cont1 = lv_obj_create(parent);
        lv_obj_add_style(slider_cont1, &style_common_btn, 0);
        lv_obj_set_style_bg_color(slider_cont1, lv_color_black(), 0);
        lv_obj_add_style(slider_cont1, &style_common_focued_btn, LV_STATE_FOCUSED);
        lv_obj_set_size(slider_cont1, 160, 20);
        lv_obj_set_pos(slider_cont1, i * 160, 120);
        lv_group_add_obj(indev_group, slider_cont1);

        lv_obj_t* slider = lv_slider_create(slider_cont1);
        lv_obj_set_style_bg_color(slider_cont1, COMMON_EDITED_COLOR, LV_PART_INDICATOR);
        lv_obj_set_style_bg_color(slider, lv_color_make(0, 128, 64), LV_PART_KNOB);
        lv_obj_remove_style(slider, NULL, LV_PART_KNOB);
        lv_obj_set_width(slider, 150);
        lv_slider_set_range(slider, FAN_MIN_SPEED, FAN_MAX_SPEED);                
        lv_obj_align(slider, LV_ALIGN_LEFT_MID, 0, 0);
        lv_group_remove_obj(slider);

        if (i == 0)
        {        
            lv_obj_set_user_data(slider_cont1, &p_model->test_data.FanAdjRPM);            
        }
        else
        {
            lv_obj_set_user_data(slider_cont1, &p_model->test_data.LampFanAdjRPM);            
        }   

        lv_obj_add_event_cb(slider_cont1, slider_event_cb, LV_EVENT_ALL, NULL);
    }

    refresh_fanctrl_screen();
}

/**
 * @brief 刷新风扇控制页面转速
 * 
 */
static void refresh_fanctrl_screen(void)
{
    for (uint8_t i=0; i<2; i++)
    {     
        if (i == 0)   //刷新控制盒
        {
            lv_snprintf(sdb, sizeof(sdb), "%d", p_model->test_data.FanAdjRPM);
            lv_label_set_text(view.sub_page.fan_ctrl.spd_label[i][0], sdb);
            lv_snprintf(sdb, sizeof(sdb), "%d", p_model->test_data.FanRealRPM);
            lv_label_set_text(view.sub_page.fan_ctrl.spd_label[i][1], sdb);            
        }
        else if (i == 1)//刷新灯体
        {
            lv_snprintf(sdb, sizeof(sdb), "%d", p_model->test_data.LampFanAdjRPM);
            lv_label_set_text(view.sub_page.fan_ctrl.spd_label[i][0], sdb);
            lv_snprintf(sdb, sizeof(sdb), "%d", p_model->test_data.LampFanRealRPM);
            lv_label_set_text(view.sub_page.fan_ctrl.spd_label[i][1], sdb);  
        }            
    }
}

/**
 * @brief 功率校准页面
 * 
 * @param parent 
 */
static void uitest_power_powercalibrate_init(lv_obj_t* parent)
{
    lv_obj_t* label;

    label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_22), 0);
    lv_label_set_text(label, "Power Calibrate");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    label = lv_label_create(parent);
    lv_obj_set_size(label, 200, 25);
    lv_obj_set_pos(label, 61, 102);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_18), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    view.sub_page.power_calib.state_label = label;

    label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_22), 0);
    lv_obj_set_style_text_color(label, TEXT_COLOR, 0);
    lv_obj_set_size(label, 100, 25);
    lv_obj_set_pos(label, 111, 142);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    view.sub_page.power_calib.power_label = label;
    
    refresh_power_calibrate();
}

/**
 * @brief RGBWW模式测试
 * 
 * @param parent 
 */
static void uitest_power_rgbwwcontrol_init(lv_obj_t* parent)
{
    const char* title[] = {"R :", "G :" ,"B :" ,"WW1 :" , "CW1 :" ,"WW2 :" , "CW2 :"};
//    uint8_t i = 0;
//    lv_obj_t* label_title;
    char str[10] = {0};
    lv_obj_t* btn_cont = lv_obj_create(parent);
    lv_obj_remove_style_all(btn_cont);  
    lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(btn_cont, &style_rect_panel, 0);
    lv_obj_set_style_text_font(btn_cont, Font_ResouceGet(FONT_10), 0 );
    lv_obj_set_style_bg_color(btn_cont, lv_color_make(0, 0, 0), 0);
    lv_obj_set_style_pad_gap(btn_cont, 6, 0);
    lv_obj_set_size(btn_cont, 160, 200);
    lv_obj_align(btn_cont, LV_ALIGN_CENTER,  0, 0);
    for (uint8_t i = 0; i < 7; i++)
    {
        lv_obj_t* obj = adj_btn_create(btn_cont, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);        
        lv_obj_set_size(obj, lv_pct(100), 23); 
        lv_obj_set_user_data(obj, &p_model->test_data.rgbww[i]); 
        lv_obj_t* label =lv_label_create(obj);
//        lv_obj_add_event_cb(obj, uitest_rgbww_event, LV_EVENT_RELEASED, NULL);
        lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
        lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_10), 0 );
        lv_obj_set_width(label, lv_pct(50));
        lv_label_set_text(label, title[i]);
        lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);
        lv_obj_t* text =lv_label_create(obj);
        lv_obj_set_width(text, lv_pct(20));
        lv_obj_align(text, LV_ALIGN_RIGHT_MID, -10, 0);
        lv_obj_set_style_text_font(text, Font_ResouceGet(FONT_10), 0 );
        lv_obj_add_event_cb(obj, rgbww_event_cb, LV_EVENT_ALL, NULL);
        lv_group_add_obj(indev_group, obj);
        sprintf( str ,"%d",p_model->test_data.rgbww[i]);             
        lv_label_set_text(text , str);
    }
}

/**
 * @brief RGBWW模式测试
 * 
 * @param parent 
 */
static void uitest_power_water_pump_init(lv_obj_t* parent)
{
    const char* title[] = {"Water Pump :"};
//    uint8_t i = 0;
//    lv_obj_t* label_title;
    char str[10] = {0};
    lv_obj_t* btn_cont = lv_obj_create(parent);
    lv_obj_remove_style_all(btn_cont);  
    lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(btn_cont, &style_rect_panel, 0);
    lv_obj_set_style_text_font(btn_cont, Font_ResouceGet(FONT_10), 0 );
    lv_obj_set_style_bg_color(btn_cont, lv_color_make(0, 0, 0), 0);
    lv_obj_set_style_pad_gap(btn_cont, 1, 0);
    lv_obj_set_size(btn_cont, 160, 200);
    lv_obj_align(btn_cont, LV_ALIGN_CENTER,  0, 0);

    for (uint8_t i = 0; i < 1; i++)
    {
		p_model->test_data.pump_speed = 7000;
        lv_obj_t* obj = adj_btn_create(btn_cont, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);        
        lv_obj_set_size(obj, lv_pct(100), 50); 
        lv_obj_set_user_data(obj, &p_model->test_data.pump_speed); 
        lv_obj_t* label =lv_label_create(obj);
//        lv_obj_add_event_cb(obj, uitest_rgbww_event, LV_EVENT_RELEASED, NULL);
        lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
        lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_10), 0 );
        lv_obj_set_width(label, lv_pct(50));
        lv_label_set_text(label, title[i]);
        lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);
        lv_obj_t* text =lv_label_create(obj);
        lv_obj_set_width(text, lv_pct(20));
        lv_obj_align(text, LV_ALIGN_RIGHT_MID, -10, 0);
        lv_obj_set_style_text_font(text, Font_ResouceGet(FONT_10), 0 );
        lv_obj_add_event_cb(obj, pump_event_cb, LV_EVENT_ALL, NULL);
        lv_group_add_obj(indev_group, obj);
        sprintf( str ,"%d",p_model->test_data.pump_speed);             
        lv_label_set_text(text , str);
    }
}

/**
 * @brief 电动支架模式测试
 * 
 * @param parent 
 */
static void uitest_motor_init(lv_obj_t* parent)
{
	const char* title[] = {"Pitch Motor", "Rotale Motor" , "Fresnel Motor","Loop Count"};
//    uint8_t i = 0;
//    lv_obj_t* label_title;
//    char str[10] = {0};
	
	lv_obj_t* label =lv_label_create(parent);
	lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
	lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_22_BOLD), 0 );
	lv_obj_align_to(label, parent, LV_ALIGN_TOP_MID, -20, 0);
	lv_label_set_text(label, "Motor Test");
	
    lv_obj_t* btn_cont = lv_obj_create(parent);
    lv_obj_remove_style_all(btn_cont);  
    lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(btn_cont, &style_rect_panel, 0);
    lv_obj_set_style_text_font(btn_cont, Font_ResouceGet(FONT_10), 0 );
    lv_obj_set_style_bg_color(btn_cont, lv_color_make(0, 0, 0), 0);
    lv_obj_set_style_pad_gap(btn_cont, 4, 0);
    lv_obj_set_size(btn_cont, 250, 200);
    lv_obj_align(btn_cont, LV_ALIGN_CENTER, 0, 0);
	
	p_model->test_data.motor_test[1] = 0;
	p_model->test_data.motor_test[0] = 0;
    for (uint8_t i = 0; i < 4; i++)
    {
        lv_obj_t* obj = adj_btn_create(btn_cont, LIGHT_MODE_COMMON_RADIUS, ADJ_BTN_TYPE_18);        
        lv_obj_set_size(obj, lv_pct(100), 40); 
        lv_obj_set_user_data(obj, &p_model->test_data.motor_test[i]); 
        lv_obj_t* label =lv_label_create(obj);
//        lv_obj_add_event_cb(obj, uitest_rgbww_event, LV_EVENT_RELEASED, NULL);
        lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
        lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_18_MEDIUM), 0 );
        lv_obj_set_width(label, lv_pct(50));
		lv_label_set_text(label, title[i]);
        lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);
        motor_text[i] =lv_label_create(obj);
        lv_obj_set_width(motor_text[i], lv_pct(20));
        lv_obj_align(motor_text[i], LV_ALIGN_RIGHT_MID, -10, 0);
        lv_obj_set_style_text_font(motor_text[i], Font_ResouceGet(FONT_18_MEDIUM), 0 );
//		if(i == 0)
//		{
			if(p_model->test_data.motor_test[i] == 0)
				lv_label_set_text(motor_text[i] , "stop");
			else
				lv_label_set_text(motor_text[i] , "star");
			lv_obj_add_event_cb(obj, motor_test_event_cb, LV_EVENT_ALL, NULL);
			lv_group_add_obj(indev_group, obj);
//		}	
//		else if(i ==  1)
//		{
//			sprintf( str ,"count:    %d", p_model->test_data.motor_test[1]);             
//			lv_label_set_text(motor_text[i] , str);
//		}
    }
}

static void refresh_powersetmode_disp(void)
{
    LV_IMG_DECLARE(ImgStudioOn)
    LV_IMG_DECLARE(ImgStudioOff)

    lv_obj_t* pre = lv_obj_get_child(lv_scr_act(), 0);
    lv_obj_t* img = lv_obj_get_child(pre, 1);

    if (p_model->test_data.power_set_value == 1)
    {
        lv_img_set_src(img, &ImgStudioOn);
        lv_obj_align(img, LV_ALIGN_BOTTOM_MID, 0, -32);      
    }
    else
    {
        lv_img_set_src(img, &ImgStudioOff);                             
        lv_obj_align(img, LV_ALIGN_BOTTOM_MID, 0, -32);
    }
}

static void power_mode_even_cb(lv_event_t* e)
{
	lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target(e);
//    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_RELEASED)
    {
		p_model->test_data.power_set_value =!p_model->test_data.power_set_value;
		ui_test_power_mode_set(1);
		refresh_powersetmode_disp();
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        if (key == LV_KEY_BACKSPACE)
        {
			screen_turn_prev_page();
        }
    }
}

/**
 * @brief 测试功率模式测试
 * 
 * @param parent 
 */
static void uitest_power_init(lv_obj_t* parent)
{
	lv_obj_t* label;
	
    label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_22), 0);
    lv_label_set_text(label, "Power Set Test");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);
	
	lv_obj_t *img_btn = lv_img_create(parent);
    refresh_powersetmode_disp();  

    //lv_obj_align_to(body_cont, img_btn, LV_ALIGN_OUT_TOP, 0, -30);
    lv_obj_add_event_cb(img_btn, power_mode_even_cb, LV_EVENT_ALL, NULL);

    lv_group_add_obj(indev_group, img_btn);
    lv_group_set_wrap(indev_group, false);
    lv_group_set_editing(indev_group, false);
}

/**
 * @brief 测试功率模式测试
 * 
 * @param parent 
 */
static void uitest_power_crc_init(lv_obj_t* parent)
{
	lv_obj_t* label;

    label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_22), 0);
    lv_label_set_text(label, "Single Power Calibrate");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    label = lv_label_create(parent);
    lv_obj_set_size(label, 200, 25);
    lv_obj_set_pos(label, 61, 102);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_18), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    view.sub_page.single_power_calib.state_label = label;

    label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_22), 0);
    lv_obj_set_style_text_color(label, TEXT_COLOR, 0);
    lv_obj_set_size(label, 100, 25);
    lv_obj_set_pos(label, 111, 142);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    view.sub_page.single_power_calib.power_label = label;
    
    refresh_power_crc();
}

/**
 * @brief 测试功率模式测试
 * 
 * @param parent 
 */
static void uitest_clear_time_init(lv_obj_t* parent)
{
	lv_obj_t* label;

    label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_22), 0);
    lv_label_set_text(label, "Clear Time");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t* yes_btn = lv_obj_create(parent);
	lv_obj_clear_flag(yes_btn, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_style(yes_btn, &style_common_focued_btn, LV_STATE_FOCUSED);
	lv_obj_set_style_radius(yes_btn, 15, 0);
	lv_obj_set_style_radius(yes_btn, 15, LV_STATE_FOCUSED);        
	lv_obj_set_size(yes_btn, 75, 40);
	lv_obj_center(yes_btn);
	
	lv_obj_t* btn_label = lv_label_create(yes_btn);
	lv_obj_set_style_text_font(btn_label, Font_ResouceGet(FONT_18), 0);
	lv_label_set_text(btn_label, "Clear");
	lv_obj_center(btn_label);
}

static void uitest_motor_info_init(lv_obj_t* parent)
{
	static const char* string[] = {
        "F14_Temp1:",
        "F14_Temp2:",	
		"Tilt_L_hw:",
		"Tilt_L_sw:",
		"Tilt_R_hw:",
		"Tilt_R_sw:",
		"Pan_hw:",
		"Pan_sw:",
    };

    //测试界面标题
    lv_obj_t* label;
    label = lv_label_create(parent);
    lv_obj_set_style_text_font(label, Font_ResouceGet(FONT_22), 0);
    lv_label_set_text(label, "Accessory Info");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t* label_cont = lv_obj_create(parent);
    lv_obj_add_style(label_cont, &style_common_btn, 0);
    lv_obj_set_style_bg_color(label_cont, lv_color_black(), 0);
    lv_obj_set_style_text_font(label_cont, Font_ResouceGet(FONT_12), 0);    
    lv_obj_set_size(label_cont, lv_pct(100), lv_pct(90));    
    lv_obj_set_pos(label_cont, 0, 40);

   
    for (uint8_t i = 0; i < sizeof(string) / sizeof(char*); i++)
    {
        label = lv_label_create(label_cont);
        lv_label_set_text(label, string[i]);    
        lv_obj_set_pos(label, 20, i * 24);       
    }

    for (uint8_t i = 0; i < sizeof(string) / sizeof(char*); i++)
    {
        view.sub_page.info_disp.label[i]  = lv_label_create(label_cont);
        lv_label_set_recolor(view.sub_page.info_disp.label[i], true);
        lv_obj_set_style_text_color(view.sub_page.info_disp.label[i], TEXT_COLOR, 0);
        lv_obj_set_size(view.sub_page.info_disp.label[i] , 200, 24);            
        lv_obj_set_pos(view.sub_page.info_disp.label[i] , 200, i * 24);   
    }
}

/**
 * @brief 刷新电动菲涅尔显示页面
 * 
 */
static void refresh_info_disp(void)
{
    lv_obj_t* label;
    for (uint8_t i = 0; i < LABEL_ID_BAT2; i++)
    {
        label = view.sub_page.power_disp.label[i];
        switch (i)
        {
			case LABEL_F14_TEMP1 :
				 lv_snprintf(sdb, sizeof(sdb), "%d°C", p_model->test_data.zoom1_Temp);
				 break;
			case LABEL_F14_TEMP2:
				 lv_snprintf(sdb, sizeof(sdb), "%d°C", p_model->test_data.zoom2_Temp);
				 break;
			case LABEL_TILT_L_HW:
				 break;
			case LABEL_TILT_L_SW:
				 break;
			case LABEL_TILT_R_HW:
				 break;
			case LABEL_TILT_R_SW:
				 break;
			case LABEL_PAN_HW:
				 break;
			case LABEL_PAN_SW:
				 break;
			case LABEL_ID_MAX_NUM:
			default : break;
        }
        lv_label_set_text(label, sdb);
    }
}

/**
 * @brief 刷新功率校准页面
 * 
 */
static void refresh_power_calibrate(void)
{
    const char* state_string[] =  {
        "Calibrate Idle",
        "Calibrate PWM_Begin",
		"Calibrate ANALOG_Begin",
        "Calibrating",
        "Pwm Calibrate OK ！Please continue to wait",
		"Analog Calibrate Ok",
        "Calibrate Failed"
    };

    if (p_model->test_data.CalibrateState > CALIBRATE_POWER_FAIL) {
        p_model->test_data.CalibrateState = CALIBRATE_POWER_PWM_START;
    }
	lv_snprintf(sdb, sizeof(sdb), "%s", state_string[p_model->test_data.CalibrateState]);
	lv_label_set_text(view.sub_page.power_calib.state_label, sdb);

	lv_snprintf(sdb, sizeof(sdb), "%dW", p_model->test_data.CalibratePower);
	lv_label_set_text(view.sub_page.power_calib.power_label, sdb);
}

/**
 * @brief 刷新单路功率校准页面
 * 
 */
static void refresh_power_crc(void)
{
    const char* state_string[] =  {
        "Calibrate Idle",
        "Calibrate PWM_Begin",
		"Calibrate ANALOG_Begin",
        "Calibrating",
        "Pwm Calibrate OK ！Please continue to wait",
		"Analog Calibrate Ok",
        "Calibrate Failed"
    };

    if (p_model->test_data.powercalibrateState > CALIBRATE_POWER_FAIL) {
        p_model->test_data.powercalibrateState = CALIBRATE_POWER_PWM_START;
    }
	lv_snprintf(sdb, sizeof(sdb), "%s", state_string[p_model->test_data.powercalibrateState]);
	lv_label_set_text(view.sub_page.single_power_calib.state_label, sdb);

	lv_snprintf(sdb, sizeof(sdb), "%dW", p_model->test_data.powercalibratePower);
	lv_label_set_text(view.sub_page.single_power_calib.power_label, sdb);
}

/**
 * @brief 滑动条事件回调函数
 * 
 * @param e 
 */
static void slider_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* target = lv_event_get_target(e);
    lv_group_t* group = lv_obj_get_group(target);
    lv_obj_t* slider = lv_obj_get_child(target, 0);
    int32_t* user_data = (int32_t*)lv_obj_get_user_data(target);

    if (event == LV_EVENT_KEY)  
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);

        if (key == LV_KEY_BACKSPACE && !lv_group_get_editing(group))
        {            
            screen_turn_prev_page();           
        }        
        else if (key == LV_ENCODER_KEY_CW)
        {
            int32_t value = *user_data + 50;
            if (value >= FAN_MAX_SPEED)
            {
                value = FAN_MAX_SPEED;
            }
            *user_data = value;
            lv_slider_set_value(slider, *user_data, LV_ANIM_OFF);
        }
        else if (key == LV_ENCODER_KEY_CC)
        {
            int32_t value = *user_data- 50;
            if (value <= FAN_MIN_SPEED)
            {
                value = FAN_MIN_SPEED;
            }
            *user_data = value;          
            lv_slider_set_value(slider, *user_data, LV_ANIM_OFF);
        }
    }
    else if (event == LV_EVENT_RELEASED)
    {
        if (!lv_group_get_editing(group))
        {
            lv_group_set_editing(group, true);
        }
        else
        {
            lv_group_set_editing(group, false);
        }
    }
}



/**
 * @brief 滑动条事件回调函数
 * 
 * @param e 
 */
static void rgbww_event_cb(lv_event_t* e)
{
    char str[10] = {0};
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* target = lv_event_get_target(e);
    lv_group_t* group = lv_obj_get_group(target);
    lv_obj_t* slider = lv_obj_get_child(target, 1);
    int16_t* user_data = (int16_t*)lv_obj_get_user_data(target);

    if (event == LV_EVENT_KEY)  
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);

        if (key == LV_KEY_BACKSPACE && !lv_group_get_editing(group))
        {            
            screen_turn_prev_page();           
        }        
        else if (key == LV_ENCODER_KEY_CW)
        {
            if(*user_data > 100 )
            {
                *user_data = *user_data + 50;
            }
            else
            {
                *user_data = *user_data + 1;
            
            }
            int16_t value = *user_data ;
           
            if (value >= RGBWW_MAX_INT || value < RGBWW_MIN_INT)
            {
                value = RGBWW_MAX_INT;
            }
            *user_data = value;
            sprintf( str ,"%d",*user_data);
            lv_label_set_text(slider,str );
        }
        else if (key == LV_ENCODER_KEY_CC)
        {
            int16_t value = 0;
            
            if( *user_data < 100)
            {
                 value = *user_data - 1;
            }
            else if(*user_data > 50 & *user_data < 100)
            {
                 value = *user_data - 10;
            }
			else if(*user_data > 100 )
			{
				value = *user_data - 50;
			}
    
            if (value <= RGBWW_MIN_INT ||  value > RGBWW_MAX_INT )
            {
                value = RGBWW_MIN_INT;
            }
            *user_data = value;
            sprintf( str ,"%d",*user_data);            
            lv_label_set_text(slider, str);
        }
    }
    else if (event == LV_EVENT_RELEASED)
    {
        if (!lv_group_get_editing(group))
        {
            lv_group_set_editing(group, true);
        }
        else
        {
            lv_group_set_editing(group, false);
        }
    }
}

/**
 * @brief 滑动条事件回调函数
 * 
 * @param e 
 */
static void motor_test_event_cb(lv_event_t* e)
{
//    char str[10] = {0};
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* target = lv_event_get_target(e);
    lv_group_t* group = lv_obj_get_group(target);
    lv_obj_t* slider = lv_obj_get_child(target, 1);
    int32_t* user_data = (int32_t*)lv_obj_get_user_data(target);

    if (event == LV_EVENT_KEY)  
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);

        if (key == LV_KEY_BACKSPACE && !lv_group_get_editing(group))
        {            
            screen_turn_prev_page();           
        }        
        else if (key == LV_ENCODER_KEY_CW)
        {
			(*user_data) = 1;
            lv_label_set_text(slider, "start");
			ui_test_motor_test_set(1);
        }
        else if (key == LV_ENCODER_KEY_CC)
        {
			(*user_data) = 0;       
            lv_label_set_text(slider, "stop");
			ui_test_motor_test_set(1);
        }
    }
    else if (event == LV_EVENT_RELEASED)
    {
        if (!lv_group_get_editing(group))
        {
            lv_group_set_editing(group, true);
        }
        else
        {
            lv_group_set_editing(group, false);
        }
    }
}
/**
 * @brief 滑动条事件回调函数
 * 
 * @param e 
 */
static void pump_event_cb(lv_event_t* e)
{
    char str[10] = {0};
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* target = lv_event_get_target(e);
    lv_group_t* group = lv_obj_get_group(target);
    lv_obj_t* slider = lv_obj_get_child(target, 1);
    int16_t* user_data = (int16_t*)lv_obj_get_user_data(target);

    if (event == LV_EVENT_KEY)  
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);

        if (key == LV_KEY_BACKSPACE && !lv_group_get_editing(group))
        {            
            screen_turn_prev_page();           
        }        
        else if (key == LV_ENCODER_KEY_CW)
        {
            int16_t value = *user_data + 100;
            if (value <= RGBWW_MIN_INT )
            {
                value = RGBWW_MIN_INT;
            }
			if(  value > RGBWW_MAX_INT )
			{
				value = RGBWW_MAX_INT;
			
			}
            *user_data = value;
            sprintf( str ,"%d",*user_data);
            lv_label_set_text(slider,str );
        }
        else if (key == LV_ENCODER_KEY_CC)
        {
            int16_t value = *user_data- 100;
            if (value <= RGBWW_MIN_INT )
            {
                value = RGBWW_MIN_INT;
            }
			if(  value > RGBWW_MAX_INT )
			{
				value = RGBWW_MIN_INT;
			
			}
            *user_data = value;  
            sprintf( str ,"%d",*user_data);            
            lv_label_set_text(slider, str);
        }
    }
    else if (event == LV_EVENT_RELEASED)
    {
        if (!lv_group_get_editing(group))
        {
            lv_group_set_editing(group, true);
        }
        else
        {
            lv_group_set_editing(group, false);
        }
    }
}
/**
 * @brief 测试界面选项路口
 * 
 * @param e 
 * @param index 
 */
static void uitest_list_event_cb(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);

    if (event == LV_EVENT_RELEASED)
    {
        screen_load_sub_page_with_stack(index+1, index);
    }
    else if (event == LV_EVENT_KEY)
    {        
        uint32_t key = *(uint32_t *)lv_event_get_param(e);

        if (LV_KEY_BACKSPACE == key)
        {            
            screen_load_page(PAGE_LIGHT_MODE_CCT, 0, false);
           
        }   
    }
    else if (event == LV_EVENT_DELETE)
    {
        if (PAGE_TEST != screen_get_act_pid())
        {
            data_center_write_config_data(SYS_CONFIG_FAN_MODE, &fan_mode);  // 退出测试界面，恢复之前的风扇模式
        }
    }
}

/**
 * @brief 返回上一级界面
 * 
 * @param e 
 */
static void uitest_common_event(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_CLICKED)
    {
        screen_turn_prev_page();
        p_model->test_data.IndustrialMode = INDUSTRIAL_MODE_IDLE;
		p_model->test_data.Fresnel_Industrial = INDUSTRIAL_MODE_IDLE;
    }
}

/**
 * @brief 清除系统运行时间
 * 
 * @param e 
 */
static void uitest_clear_time_event(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    if (event == LV_EVENT_PRESSED)
    {
        screen_turn_prev_page();
        g_ptUIStateData->test_data.clear_run_time_flag = 1;
		g_ptUIStateData->test_data.clear_davice_run_time_flag = 1;
    }
	else if (event == LV_EVENT_KEY)
    {        
        uint32_t key = *(uint32_t *)lv_event_get_param(e);

        if (LV_KEY_BACKSPACE == key)
        {            
            screen_load_page(PAGE_LIGHT_MODE_CCT, 0, false);
           
        }   
    } 
}


