#include "page_manager.h"
#include "ui_common.h"
#include "page_widget.h"
#include "ui_config.h"
#include "ui_data.h"
#include "user.h"
#include <stdlib.h>

#ifndef UI_PLATFORM_MCU
static lv_style_t style_rect;

static void style_init(void)
{
    lv_style_init(&style_rect);
    lv_style_set_bg_color(&style_rect, lv_color_make(255, 0, 0));
}
static void anim_x_cb(void * var, int32_t v)
{
    lv_obj_set_x(var, v);
}

static void anim_size_cb(void * var, int32_t v)
{
    lv_obj_set_size(var, v, v);
}

static lv_obj_t* focused_obj;
static void screen_event_cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);


    if (code == LV_EVENT_KEY)
    {
        uint32_t c = *(uint32_t*)lv_event_get_param(e);

        if (c == LV_KEY_DOWN)
        {
            UI_PRINTF("KEY_DOWN!\r\n");
            int id = lv_obj_get_child_id(focused_obj);
            lv_obj_t* obj_child = lv_obj_get_child(obj, id + 1);

            lv_obj_clear_state(focused_obj, LV_STATE_FOCUSED);
            lv_event_send(focused_obj, LV_EVENT_DEFOCUSED, NULL);

            lv_obj_add_state(obj_child, LV_STATE_FOCUSED);
            lv_event_send(obj_child, LV_EVENT_FOCUSED, NULL);

            focused_obj = obj_child;
        }
        else if (c == LV_KEY_UP)
        {
            UI_PRINTF("KEY_UP!\r\n");

            UI_PRINTF("KEY_DOWN!\r\n");
            int id = lv_obj_get_child_id(focused_obj);
            lv_obj_t* obj_child = lv_obj_get_child(obj, id - 1);

            lv_obj_clear_state(focused_obj, LV_STATE_FOCUSED);
            lv_event_send(focused_obj, LV_EVENT_DEFOCUSED, NULL);

            lv_obj_add_state(obj_child, LV_STATE_FOCUSED);
            lv_event_send(obj_child, LV_EVENT_FOCUSED, NULL);
            focused_obj = obj_child;
        }
    }
}

static void obj_event_cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);

    if (code == LV_EVENT_FOCUSED)
    {
        lv_coord_t width = lv_obj_get_width(obj);
        lv_coord_t height = lv_obj_get_height(obj);

        width = width * 120 / 100;
        height = height * 120 / 100;

        lv_obj_set_size(obj, width, height);
    }
    else if (code == LV_EVENT_DEFOCUSED)
    {
        lv_coord_t width = lv_obj_get_width(obj);
        lv_coord_t height = lv_obj_get_height(obj);

        width = width * 100 / 120;
        height = height * 100 / 120;

        lv_obj_set_size(obj, width, height);
    }
}


static lv_group_t* g;
#define COLOR_FIGURE_WIDTH  171
#define COLOR_FIGURE_HEIGHT 189

#define PARAM_BTN_WIDTH     118
#define PARAM_BTN_HEIGHT    60
#define BTN_CONT_WIDTH      118
#define BTN_CONT_HEIGHT     189

#define PANEL_BG_COLOR              lv_color_make(40,40,40)
#define COMMON_BTN_COLOR            lv_color_make(40,40,40)

#define COMMON_BTN_FOCUSED_COLOR    lv_color_make(230,230,230)
#define COMMON_BTN_TEXT_COLOR       lv_color_make(255,255,255)
#define COMMON_BTN_FOCUSED_TEXT_COLOR       lv_color_make(255,255,255)


//typedef struct _adj_btn {
//    lv_obj_t* obj;
//    const char* name;
//    const char* unit;
//}adj_btn_t;
//static adj_btn_t btn_list[] = {
//    {NULL, "INT", "%"},
//    {NULL, "HUE", "°"},
//    {NULL, "SAT", "%"},
//};
static lv_group_t* indev_group;

void test1(void)
{
    indev_group = lv_group_create();
    lv_group_set_default(indev_group);

    lv_indev_t* indev = NULL;

    do {

        indev = lv_indev_get_next(indev);
        if (indev == NULL)
        {
            break;
        }
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_ENCODER || LV_INDEV_TYPE_KEYPAD == lv_indev_get_type(indev))
        {
            lv_indev_set_group(indev, indev_group);
        }

    } while (1);

    /*
        目的:  让对象被focus的时候被放大
        
    */
    //需要过渡的属性
    static const lv_style_prop_t props[] = { LV_STYLE_WIDTH, LV_STYLE_BG_COLOR, LV_STYLE_BORDER_COLOR, LV_STYLE_BORDER_WIDTH, LV_STYLE_PROP_INV };

    /* A default transition
    * 过渡的时间
     * Make it fast (100ms) and start with some delay (200 ms)*/
    static lv_style_transition_dsc_t trans_def;
    lv_style_transition_dsc_init(&trans_def, props, lv_anim_path_linear, 100, 0, NULL);

    /* A special transition when going to pressed state
     * Make it slow (500 ms) but start  without delay*/
    static lv_style_transition_dsc_t trans_pr;
    lv_style_transition_dsc_init(&trans_pr, props, lv_anim_path_linear, 100, 0, NULL);

    static lv_style_t style_def;
    lv_style_init(&style_def);
    lv_style_set_width(&style_def, 40);
    lv_style_set_height(&style_def, 40);
    lv_style_set_transition(&style_def, &trans_def);

    static lv_style_t style_pr;
    lv_style_init(&style_pr);
    lv_style_set_bg_color(&style_pr, lv_palette_main(LV_PALETTE_RED));
    //lv_style_set_border_width(&style_pr, 6);
    //lv_style_set_border_color(&style_pr, lv_palette_darken(LV_PALETTE_RED, 3));
    lv_style_set_width(&style_pr, 100);
    lv_style_set_height(&style_pr, 100);
    lv_style_set_transition(&style_pr, &trans_pr);

    /*Create an object with the new style_pr*/
    lv_obj_t* obj = lv_obj_create(lv_scr_act());
    lv_obj_add_style(obj, &style_def, 0);
    lv_obj_add_style(obj, &style_pr, LV_STATE_FOCUSED);

    lv_obj_center(obj);
    

}


void test4(void)
{
    //color_figure_cont = lv_obj_create(parent);
    //lv_obj_remove_style_all(color_figure_cont);
    //lv_obj_add_style(color_figure_cont, &style_rect_panel, 0);
    //lv_obj_set_size(color_figure_cont, COLOR_FIGURE_WIDTH, COLOR_FIGURE_HEIGHT);
    //lv_obj_set_pos(color_figure_cont, 0, 0);

    lv_obj_t* btn_cont = lv_obj_create(lv_scr_act());
    //lv_obj_remove_style_all(btn_cont);
    lv_obj_set_scrollbar_mode(btn_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_style(btn_cont, &style_rect_panel, 0);
    lv_obj_set_style_pad_top(btn_cont, 3, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(btn_cont, 3, LV_STATE_DEFAULT);
    
    lv_obj_set_style_bg_color(btn_cont, lv_color_make(255, 0, 0), LV_STATE_DEFAULT);
    //lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_COLUMN);    
    lv_obj_set_size(btn_cont, BTN_CONT_WIDTH, BTN_CONT_HEIGHT);
    lv_obj_center(btn_cont);

    //for (int i = 0; i < 3; i++)
    //{
    //    btn_list[i].obj = lv_obj_create(btn_cont);
    //    //lv_obj_remove_style_all(btn_list[i].obj);
    //    lv_obj_add_style(btn_list[i].obj, &style_rect_panel, 0);
    //    lv_obj_set_style_bg_color(btn_list[i].obj, COMMON_BTN_FOCUSED_COLOR, LV_STATE_FOCUSED);
    //    lv_obj_set_style_pad_all(btn_list[i].obj, 0, LV_STATE_DEFAULT);
    //    lv_obj_set_style_text_color(btn_list[i].obj, COMMON_BTN_FOCUSED_TEXT_COLOR, LV_STATE_FOCUSED);        
    //    lv_obj_set_size(btn_list[i].obj, PARAM_BTN_WIDTH, PARAM_BTN_HEIGHT);

    //    //int_cont_init(btn_list[i].obj);
    //    //refresh_int_cont(btn_list[i].obj);
    //}


}

#define MY_RAND(x)    ((rand() + (x)) % (x))
void dummy_data_process(lv_timer_t* t)
{
//    g_tUIAllData.hsi_model.cct = (rand() + 2000 ) % 10000;
//    g_tUIAllData.hsi_model.hsi.hue = rand() % 360;
//    g_tUIAllData.hsi_model.hsi.intensity = rand() % 1000;
//    g_tUIAllData.hsi_model.hsi.sat = rand() % 100;
//    page_set_invalid_type(PAGE_INVALID_TYPE_PORTION);
#if UI_DUMMY_DATA_ENABLE
    g_tUIAllData.source_model.src_light_index = MY_RAND(SOURCE_MAX);
    g_tUIAllData.source_model.intensity = MY_RAND(1000);

    g_tUIAllData.gel_model.intensity = MY_RAND(1000);
    g_tUIAllData.gel_model.cct_type = MY_RAND(2);
    g_tUIAllData.gel_model.brand_type = MY_RAND(2);
    g_tUIAllData.gel_model.series[0] = MY_RAND(4);    
    g_tUIAllData.gel_model.series[1] = MY_RAND(4);    
    g_tUIAllData.gel_model.gel_index[g_tUIAllData.gel_model.brand_type][g_tUIAllData.gel_model.series[0]] = MY_RAND(8);
    g_tUIAllData.gel_model.gel_index[g_tUIAllData.gel_model.brand_type][g_tUIAllData.gel_model.series[1]] = MY_RAND(8);

    g_tUIAllData.rgb_model.intensity = MY_RAND(1000);
    g_tUIAllData.rgb_model.rgb.r     = MY_RAND(255);
    g_tUIAllData.rgb_model.rgb.g     = MY_RAND(255);
    g_tUIAllData.rgb_model.rgb.b     = MY_RAND(255);

    g_tUIAllData.hsi_model.cct = MY_RAND(8000) + UI_CCT_MIN;
    g_tUIAllData.hsi_model.hsi.hue = MY_RAND(360);
    g_tUIAllData.hsi_model.hsi.sat = MY_RAND(100);
    g_tUIAllData.hsi_model.hsi.intensity = MY_RAND(1000);

    g_tUIAllData.xy_model.intensity = MY_RAND(1000);
    g_tUIAllData.xy_model.xy.x = MY_RAND(8000);
    g_tUIAllData.xy_model.xy.y = MY_RAND(8000);

    int8_t sign_value = MY_RAND(100) % 2  == 1 ? 1 : -1;
    g_tUIAllData.cct_model.cct = MY_RAND(8000) + 2000;
    g_tUIAllData.cct_model.gm = MY_RAND(100) * sign_value;
    g_tUIAllData.cct_model.intensity = MY_RAND(1000);

    g_tUIAllData.candle_model.obj.cct_range = MY_RAND(3);
    g_tUIAllData.candle_model.obj.frq = MY_RAND(10);
    g_tUIAllData.candle_model.obj.intensity = MY_RAND(1000);

    g_tUIAllData.fire_model.obj.cct_range = MY_RAND(3);
    g_tUIAllData.fire_model.obj.frq = MY_RAND(10);
    g_tUIAllData.fire_model.obj.intensity = MY_RAND(1000);
    g_tUIAllData.fire_model.obj.spd = MY_RAND(10);

    g_tUIAllData.clublights_model.obj.intensity = MY_RAND(1000);
    g_tUIAllData.clublights_model.obj.state = MY_RAND(2);
    g_tUIAllData.clublights_model.obj.color = MY_RAND(8);
    g_tUIAllData.clublights_model.obj.spd = MY_RAND(10);

    g_tUIAllData.colorchase_model.obj.spd = MY_RAND(10);
    g_tUIAllData.colorchase_model.obj.intensity = MY_RAND(1000);
    g_tUIAllData.colorchase_model.obj.sat = MY_RAND(100);
    g_tUIAllData.colorchase_model.obj.state = MY_RAND(2);

    g_tUIAllData.partylight_model.obj.intensity = MY_RAND(1000);
    g_tUIAllData.partylight_model.obj.sat = MY_RAND(100);
    g_tUIAllData.partylight_model.obj.spd = MY_RAND(10);
    g_tUIAllData.partylight_model.obj.state = MY_RAND(2);

    g_tUIAllData.copcar_model.obj.color = MY_RAND(4);
    g_tUIAllData.copcar_model.obj.frq = MY_RAND(10);
    g_tUIAllData.copcar_model.obj.intensity = MY_RAND(1000);
    g_tUIAllData.copcar_model.obj.state = MY_RAND(2);

    g_tUIAllData.explosion_model.obj.decay = MY_RAND(10);
    g_tUIAllData.explosion_model.obj.spd = MY_RAND(10);
    g_tUIAllData.explosion_model.obj.intensity = MY_RAND(1000);
    g_tUIAllData.explosion_model.obj.state = MY_RAND(2);
    g_tUIAllData.explosion_model.param_index = MY_RAND(6);

    g_tUIAllData.faultybulb_model.obj.frq = MY_RAND(10);
    g_tUIAllData.faultybulb_model.obj.intensity = MY_RAND(1000);
    g_tUIAllData.faultybulb_model.param_index = MY_RAND(6);
    g_tUIAllData.faultybulb_model.obj.spd = MY_RAND(10);
    g_tUIAllData.faultybulb_model.obj.state = MY_RAND(2);

    g_tUIAllData.fireworks_model.obj.frq = MY_RAND(10);
    g_tUIAllData.fireworks_model.obj.intensity = MY_RAND(100);
    g_tUIAllData.fireworks_model.obj.type = MY_RAND(3);
    g_tUIAllData.fireworks_model.obj.state = MY_RAND(2);

    g_tUIAllData.strobe_model.obj.intensity = MY_RAND(1000);
    g_tUIAllData.strobe_model.obj.spd =  MY_RAND(10);
    g_tUIAllData.strobe_model.obj.state =  MY_RAND(2);
    
    g_tUIAllData.paparazzi_model.obj.cct = MY_RAND(8000) + UI_CCT_MIN;
    g_tUIAllData.paparazzi_model.obj.gm = MY_RAND(100) * sign_value;
    g_tUIAllData.paparazzi_model.obj.intensity = MY_RAND(1000);
    g_tUIAllData.paparazzi_model.obj.state = MY_RAND(2);
    g_tUIAllData.paparazzi_model.obj.frq = MY_RAND(10);

    g_tUIAllData.candle_model.obj.intensity = MY_RAND(1000);
    g_tUIAllData.candle_model.obj.spd = MY_RAND(2);
    g_tUIAllData.candle_model.obj.spd = MY_RAND(10);
    g_tUIAllData.candle_model.obj.state = MY_RAND(2);

    g_tUIAllData.lightning_model.param_index = MY_RAND(2);
    g_tUIAllData.lightning_model.obj.intensity = MY_RAND(1000);
    g_tUIAllData.lightning_model.obj.spd = MY_RAND(10);
    g_tUIAllData.lightning_model.obj.state = MY_RAND(3);
    g_tUIAllData.lightning_model.obj.frq = MY_RAND(10);

    g_tUIAllData.pulsing_model.obj.frq = MY_RAND(10);
    g_tUIAllData.pulsing_model.obj.intensity = MY_RAND(1000);
    g_tUIAllData.pulsing_model.obj.spd = MY_RAND(10);
    g_tUIAllData.pulsing_model.obj.state = MY_RAND(2);
    g_tUIAllData.pulsing_model.param_index = MY_RAND(6);
    //
    g_tUIAllData.strobe_model.obj.intensity = MY_RAND(1000);
    g_tUIAllData.strobe_model.obj.spd = MY_RAND(10);
    g_tUIAllData.strobe_model.obj.state = MY_RAND(2);
    g_tUIAllData.strobe_model.param_index = MY_RAND(6);

    g_tUIAllData.tv_model.obj.intensity = MY_RAND(1000);
    g_tUIAllData.tv_model.limit_index = MY_RAND(2);
    g_tUIAllData.tv_model.obj.spd = MY_RAND(10);
    g_tUIAllData.tv_model.obj.state = MY_RAND(2);
    g_tUIAllData.tv_model.obj.cct_range = MY_RAND(3);

    g_tUIAllData.welding_model.obj.frq = MY_RAND(10);
    g_tUIAllData.welding_model.obj.intensity = MY_RAND(1000);
    g_tUIAllData.welding_model.param_index = MY_RAND(6);
    g_tUIAllData.welding_model.obj.state = MY_RAND(2);
    g_tUIAllData.welding_model.obj.min_int = MY_RAND(10);

    //extern ui_title_t title_model;
    //g_ptUIStateData->title.power_type = MY_RAND(2);
    //g_tUIAllData.sys_menu_model.fan_mode = MY_RAND(4);

    g_tUIAllData.fire2_model.obj.spd = MY_RAND(10);
    g_tUIAllData.fire2_model.limit_index = MY_RAND(2);
    g_tUIAllData.fire2_model.obj.state = MY_RAND(2);
    g_tUIAllData.fire2_model.obj.intensity = MY_RAND(1000);
    g_tUIAllData.fire2_model.setting[0].c_setting.c_down_limit = MY_RAND(10000);
    g_tUIAllData.fire2_model.setting[0].c_setting.c_up_limit = MY_RAND(10000);

    g_tUIAllData.fire2_model.setting[1].h_setting.h_down_limit = MY_RAND(360);
    g_tUIAllData.fire2_model.setting[1].h_setting.h_up_limit = MY_RAND(360);

    page_set_invalid_type(PAGE_INVALID_TYPE_ALL);

 #endif
    
    
    static uint32_t time_cnt = 0;
    static bool b_sp_on = false;

    // if (user_get_interact_status() == INTERACT_STATUS_IDLE && b_sp_on == true)
    // {
    //     b_sp_on = false;
    //     time_cnt = 0;
    // }

    time_cnt += t->period;

#define USER_INTERFACE_TEST     1
#if USER_INTERFACE_TEST
    //if (!b_sp_on && time_cnt >= UI_ENTER_SCREEN_PROCTECTOR_TIME)
    if (!b_sp_on && time_cnt >= 10000)
#else
    if (!b_sp_on && time_cnt >= 10 * 1000)
#endif
    {
#if USER_INTERFACE_TEST
        b_sp_on = true;
        //user_enter_siduspro();
        //user_enter_overheat();
        //user_enter_low_volt_page();
        //screen_load_page(PAGE_CONTROL_SYSTEM, SUB_ID_BLE_RESETING, true);
#else 
        static uint32_t interact_index = 0;
        
        b_sp_on = true;     
        switch(interact_index)
        {
            case 0:user_enter_protector();break;
            case 1:user_enter_mixplug_warn();break;
            case 2:user_enter_siduspro();break;
            case 3:user_enter_low_volt_page();break;
        }  
        interact_index += 1;
        interact_index %= 4;
#endif
    }

    static uint32_t test_time_cnt = 0;    
    static uint8_t state = 0;
    static uint32_t remain_time = 20 * 1000;
    test_time_cnt += t->period;
    
    if (state == 2) //两个电池都在充电
    {            
        g_ptUIStateData->title.power_dsc.dsc.dc_with_batt.batt_sys[0].percent += 3;            
        g_ptUIStateData->title.power_dsc.dsc.dc_with_batt.batt_sys[1].percent += 3;   
        g_ptUIStateData->title.power_dsc.dsc.dc_with_batt.batt_sys[0].percent %= 100;
        g_ptUIStateData->title.power_dsc.dsc.dc_with_batt.batt_sys[1].percent %= 100;
    }
    else if (state == 6) //电池正在放电
    {
        int val = g_ptUIStateData->title.power_dsc.dsc.batt.percent - 15;
        if (val < 0 ) {
            val = 0;
        }           
        g_ptUIStateData->title.power_dsc.dsc.batt.percent = val;
        g_ptUIStateData->title.power_dsc.dsc.batt.remain_time_m -= 10;
        if (g_ptUIStateData->title.power_dsc.dsc.batt.percent < 20)
        {
            g_ptUIStateData->title.power_dsc.dsc.batt.remain_time_m = 10;
        }
    }

    if (test_time_cnt > remain_time)
    {    
        test_time_cnt = 0;
        state++;
        switch (state)
        {
        case 1: //DC供电
            g_ptUIStateData->title.power_type = POWER_TYPE_SUPPLY_BOX;
            remain_time = 5 * 1000;
            break;
        case 2:  // DC加电池供电，正在充电
            g_ptUIStateData->title.power_type = POWER_TYPE_DC_WITH_BATT;
            g_ptUIStateData->title.power_dsc.type = POWER_TYPE_DC_WITH_BATT;
            g_ptUIStateData->title.power_dsc.dsc.dc_with_batt.batt_sys[0].is_exist = 1;
            g_ptUIStateData->title.power_dsc.dsc.dc_with_batt.batt_sys[0].percent = 66;      
            g_ptUIStateData->title.power_dsc.dsc.dc_with_batt.batt_sys[0].state = BATT_STATE_CHARGING;      
            g_ptUIStateData->title.power_dsc.dsc.dc_with_batt.batt_sys[1].is_exist = 1;
            g_ptUIStateData->title.power_dsc.dsc.dc_with_batt.batt_sys[1].percent = 55;             
            g_ptUIStateData->title.power_dsc.dsc.dc_with_batt.batt_sys[1].state = BATT_STATE_CHARGING;
            remain_time = 15 * 1000;
            break;        
        case 3: //电池1充电结束
            g_ptUIStateData->title.power_dsc.dsc.dc_with_batt.batt_sys[1].state = BATT_STATE_CHARGE_END;
            g_ptUIStateData->title.power_dsc.dsc.dc_with_batt.batt_sys[1].percent = 100;
            remain_time = 15 * 1000;
            break;
        case 4: //电池1充完电之后, 被拔出
            g_ptUIStateData->title.power_dsc.dsc.dc_with_batt.batt_sys[1].is_exist = 0;
            remain_time = 10 * 1000;
            break;
        case 5: //电池0充完电
            g_ptUIStateData->title.power_dsc.dsc.dc_with_batt.batt_sys[0].state = BATT_STATE_CHARGE_END;
            g_ptUIStateData->title.power_dsc.dsc.dc_with_batt.batt_sys[0].percent = 100;
            remain_time = 10 * 1000;
            break;
        case 6: //电池DC并且被放电                       
            g_ptUIStateData->title.power_type = POWER_TYPE_BATTERY;
            g_ptUIStateData->title.power_dsc.type = POWER_TYPE_BATTERY;
            g_ptUIStateData->title.power_dsc.dsc.batt.percent = 100;
            g_ptUIStateData->title.power_dsc.dsc.batt.remain_time_m = 180;
            remain_time = 40 * 1000;
            break;
        case 7:
            state = 0;
            remain_time = 10 * 1000;
            break;
        default:
            break;
        }
    }
}

#if 0
void test(void)
{
    g = lv_group_create();
    lv_group_set_default(g);

    lv_indev_t* indev = NULL;

    do {

        indev = lv_indev_get_next(indev);
        if (indev == NULL)
        {
            break;
        }
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD)
        {
            lv_indev_set_group(indev, g);
        }

    } while (1);

    lv_obj_t* screen = lv_obj_create(lv_scr_act());
    lv_obj_set_size(screen, 400, 400);
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_ROW);
    lv_obj_add_event_cb(screen, screen_event_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(g, screen);

    for (int i = 0; i < 3; i++)
    {
        lv_obj_t* obj = lv_btn_create(screen);
        lv_obj_set_style_bg_color(obj, lv_color_make(255, 0, 0), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(obj, lv_color_make(0, 255, 0), LV_STATE_FOCUSED);
        lv_obj_add_event_cb(obj, obj_event_cb, LV_EVENT_ALL, NULL);
        lv_obj_set_size(obj, 100, 100);
        lv_obj_t* label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "%d", i);
    }

    focused_obj = lv_obj_get_child(screen, 0);
    lv_obj_add_state(focused_obj, LV_STATE_FOCUSED);
    lv_event_send(focused_obj, LV_EVENT_FOCUSED, NULL);

    //style_init();

    //UI_PRINTF("width:%d height:%d\r\n", lv_obj_get_width(lv_scr_act()), lv_obj_get_height(lv_scr_act()));

    //lv_obj_t* obj = lv_obj_create(lv_scr_act());
    //lv_obj_set_size(obj, 100, 100);
    //lv_obj_add_style(obj, &style_rect, 0);
    //lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);

    //lv_obj_t* obj1 = lv_obj_create(lv_scr_act());    
    //lv_obj_set_size(obj1, 100, 100);
    //lv_obj_set_pos(obj1, 0, 0);
    //lv_obj_add_style(obj1, &style_rect, 0);
    //lv_obj_set_style_bg_color(obj1, lv_color_make(0, 255, 0), 0);

    //lv_obj_invalidate(obj);
    //lv_obj_invalidate(obj1);
}

#endif

lv_obj_t* slip_obj[3];
lv_point_t point[3] =
{
    {0, 0},
    {70, 0},
    {180, 0},
};

static void anim11_x_cb(void* var, int32_t v)
{
    lv_obj_set_x(var, v);
}

static void pannel_event_cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);

    if (code == LV_EVENT_KEY)
    {
        uint32_t c = *(uint32_t*)lv_event_get_param(e);

        if (c == LV_KEY_DOWN)
        {
            UI_PRINTF("KEY_DOWN!\r\n");

            lv_obj_set_pos(slip_obj[2], -40, point[1].y);

            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, slip_obj[1]);
            lv_anim_set_values(&a, lv_obj_get_x(slip_obj[1]), point[2].x);
            lv_anim_set_repeat_count(&a, 0);

            lv_anim_set_exec_cb(&a, anim11_x_cb);
            lv_anim_set_time(&a, 100);
            lv_anim_set_playback_time(&a, 0);
            lv_anim_start(&a);
            //////////////////////////////////////////////
            lv_anim_set_var(&a, slip_obj[0]);
            lv_anim_set_values(&a, lv_obj_get_x(slip_obj[0]), point[1].x);
            lv_anim_set_repeat_count(&a, 0);

            lv_anim_set_exec_cb(&a, anim11_x_cb);
            lv_anim_set_time(&a, 100);
            lv_anim_set_playback_time(&a, 0);
            lv_anim_start(&a);

            lv_anim_set_var(&a, slip_obj[2]);
            lv_anim_set_values(&a, lv_obj_get_x(slip_obj[2]), point[0].x);
            lv_anim_set_repeat_count(&a, 0);

            lv_anim_set_exec_cb(&a, anim11_x_cb);
            lv_anim_set_time(&a, 100);
            lv_anim_set_playback_time(&a, 0);
            lv_anim_start(&a);

            //lv_anim_set_exec_cb(&a, column_gap_anim);
            //lv_anim_set_time(&a, 3000);
            //lv_anim_set_playback_time(&a, 3000);
            //lv_anim_start(&a);
        }
        else if (c == LV_KEY_UP)
        {
            UI_PRINTF("KEY_UP!\r\n");

        }
    }
}

void test(void)
{

    g = lv_group_create();
    lv_group_set_default(g);

    lv_indev_t* indev = NULL;

    do {

        indev = lv_indev_get_next(indev);
        if (indev == NULL)
        {
            break;
        }
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD)
        {
            lv_indev_set_group(indev, g);
        }

    } while (1);
    

    lv_obj_t* panel = lv_obj_create(lv_scr_act());
    //lv_obj_remove_style_all(panel);    
    lv_group_add_obj(g, panel);
    lv_obj_set_size(panel, 320, 240);
    lv_obj_set_style_bg_color(panel, lv_color_make(255, 0, 0), LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(panel, lv_color_make(0, 255, 0), LV_STATE_DEFAULT);
    lv_obj_center(panel);


   // lv_obj_t* label1 = lv_label_create(panel);
   // lv_label_set_text(label1, "ioio");
   // lv_obj_center(label1);

   lv_obj_add_event_cb(panel, pannel_event_cb, LV_EVENT_ALL, NULL);
   // lv_obj_t* opacity_panel = lv_obj_create(panel);
   ///* lv_obj_remove_style_all(opacity_panel);*/

   // lv_obj_set_size(opacity_panel, 200, 240);
   // lv_obj_center(opacity_panel);
   // //lv_obj_set_style_bg_color(opacity_panel, lv_color_make(0, 255, 0), 0);
   // lv_obj_set_style_opa(opacity_panel, LV_OPA_60, 0);



    for (int i = 0; i < 3; i++)
    {
        slip_obj[i] = lv_obj_create(panel);
        lv_obj_set_pos(slip_obj[i], point[i].x, point[i].y);
        lv_obj_set_size(slip_obj[i], 50, 50);

        lv_obj_t* label = lv_label_create(slip_obj[i]);
        lv_label_set_text_fmt(label, "%d", i);
        lv_obj_center(label);
    }

    
}

#define FOCUSED_OBJ_WIDTH       104
#define OBJ_WIDTH               64
#define INVISIBLE_POS_X         -100
#define INVISIBLE_POS_RIGHT_X   320
#define ANIMATION_TRANSITION_TIME   200

static lv_obj_t* panel_obj[5];
static lv_point_t fixed_pos[] = 
{
    {20, 27},
    {105, 4},
    {224, 27}
};

static void anim_pos_x_cb(void* var, int32_t v)
{
    lv_obj_set_x(var, v);
}

static void anim_pos_y_cb(void* var, int32_t v)
{
    lv_obj_set_y(var, v);
}

static void anim_obj_size_cb(void* var, int32_t v)
{
    lv_obj_set_size(var, v, v);
}

static void item_pos_move_x(lv_obj_t* obj, lv_coord_t start_x, lv_coord_t end_x, uint32_t anim_time)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, start_x, end_x);
    lv_anim_set_exec_cb(&a, anim_pos_x_cb);
    lv_anim_set_repeat_count(&a, 0);    
    lv_anim_set_time(&a, anim_time);
    lv_anim_set_playback_time(&a, 0);
    lv_anim_start(&a);

}

static void item_pos_move(lv_obj_t *obj, lv_coord_t start_x, lv_coord_t start_y, \
                                            lv_coord_t end_x, lv_coord_t end_y, uint32_t time)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, start_x, end_x);
    lv_anim_set_exec_cb(&a, anim_pos_x_cb);
    //lv_anim_path_bounce(&a, lv_anim_path_bounce);
    lv_anim_set_repeat_count(&a, 0);    
    lv_anim_set_time(&a, time);
    lv_anim_set_playback_time(&a, 0);
    lv_anim_start(&a);

    lv_anim_set_values(&a, start_y, end_y);
    lv_anim_set_exec_cb(&a, anim_pos_y_cb);
    lv_anim_start(&a);
}

static void item_size_change(lv_obj_t* obj, lv_coord_t start_width, lv_coord_t end_width, uint32_t time)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, start_width, end_width);
    lv_anim_set_exec_cb(&a, anim_obj_size_cb);
    lv_anim_set_repeat_count(&a, 0);
    lv_anim_set_time(&a, time);
    lv_anim_set_playback_time(&a, 0);
    lv_anim_start(&a);
}


static void item_btn_style_init(lv_style_t *style, bool focused)
{
    lv_style_init(style);
    lv_style_set_radius(style, LV_RADIUS_CIRCLE);
    lv_style_set_pad_all(style, 0);
    lv_style_set_outline_width(style, 0);
    lv_style_set_outline_pad(style, 0);
    lv_style_set_border_width(style, 0);
}

static lv_obj_t* cur_disp_obj[3];
static lv_obj_t* next_disp_obj[3];
static lv_obj_t* cur_focus_obj;
static int cur_index = 0;

static void update_next_disp_obj(void)
{

    int start = cur_index - 2;
    
    if (start < 0)
    {
        start = 5 - (0 - start);
    }
    
    for (int i = 0 ; i < 3; i++)
    {
        next_disp_obj[i] = panel_obj[start++];
        if (start >=5)
        {
            start = 0;
        }
    }
   
}

static void update_up_disp_obj(void)
{
    int start = cur_index;

    for (int i = 0; i < 3; i++)
    {
        next_disp_obj[i] = panel_obj[start++];
        if (start >= 5)
        {
            start = 0;
        }
    }
}

static void update_cur_disp_obj(void)
{
    for (int i = 0; i < 3; i++)
    {
        cur_disp_obj[i] = next_disp_obj[i];
    }
}

static void panel_event_cb(lv_event_t* e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);

    if (LV_EVENT_KEY == event)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        if (key == LV_KEY_UP)
        {
            /*refresh the next display object.*/            
            update_up_disp_obj();
            cur_index += 1;
            if (cur_index >= 5)      cur_index = 0;

            item_pos_move_x(cur_disp_obj[0], lv_obj_get_style_x(cur_disp_obj[0], 0), INVISIBLE_POS_X, 200);
            
            item_pos_move(next_disp_obj[0], lv_obj_get_style_x(next_disp_obj[0], 0), lv_obj_get_style_y(next_disp_obj[0], 0), fixed_pos[0].x, fixed_pos[0].y, 200);
            item_size_change(next_disp_obj[0], lv_obj_get_style_width(next_disp_obj[0], 0), OBJ_WIDTH, 200);
            
            item_pos_move(next_disp_obj[1], lv_obj_get_style_x(next_disp_obj[1], 0), lv_obj_get_style_y(next_disp_obj[1], 0), fixed_pos[1].x, fixed_pos[1].y, 200);
            item_size_change(next_disp_obj[1], lv_obj_get_style_width(next_disp_obj[1], 0), FOCUSED_OBJ_WIDTH, 200);

            lv_obj_set_style_x(next_disp_obj[2], 240, 0);
            lv_obj_set_style_y(next_disp_obj[2], fixed_pos[2].y, 0);

            item_pos_move(next_disp_obj[2], lv_obj_get_style_x(next_disp_obj[2], 0), lv_obj_get_style_y(next_disp_obj[2], 0), fixed_pos[2].x, fixed_pos[2].y, 200);
            lv_obj_set_style_x(next_disp_obj[2], fixed_pos[2].x, 0);
            lv_obj_set_style_y(next_disp_obj[2], fixed_pos[2].y, 0);            
            lv_obj_set_size(next_disp_obj[2], OBJ_WIDTH, OBJ_WIDTH);

            for (int i = 0; i < 5; i++)
            {
                lv_area_t area;
                lv_obj_get_coords(panel_obj[i], &area);
                UI_PRINTF("sn:%d x1: %d x2:%d y1:%d y2:%d\r\n", i, area.x1, area.x2, area.y1, area.y2);
            }

            update_cur_disp_obj();
        }
        else if (key == LV_KEY_DOWN)
        {
            update_next_disp_obj();
            cur_index -= 1;
            if (cur_index < 0)  cur_index = 4;

            item_pos_move_x(cur_disp_obj[2], lv_obj_get_style_x(cur_disp_obj[2], 0), INVISIBLE_POS_RIGHT_X, 200);

            item_pos_move(next_disp_obj[2], lv_obj_get_style_x(next_disp_obj[2], 0), lv_obj_get_style_y(next_disp_obj[2], 0), fixed_pos[2].x, fixed_pos[2].y, 200);
            item_size_change(next_disp_obj[2], lv_obj_get_style_width(next_disp_obj[2], 0), OBJ_WIDTH, 200);

            item_pos_move(next_disp_obj[1], lv_obj_get_style_x(next_disp_obj[1], 0), lv_obj_get_style_y(next_disp_obj[1], 0), fixed_pos[1].x, fixed_pos[1].y, 200);
            item_size_change(next_disp_obj[1], lv_obj_get_style_width(next_disp_obj[1], 0), FOCUSED_OBJ_WIDTH, 200);

            lv_obj_set_style_x(next_disp_obj[0], INVISIBLE_POS_X, 0);
            lv_obj_set_style_y(next_disp_obj[0], fixed_pos[0].y, 0);
            lv_obj_set_size(next_disp_obj[0], OBJ_WIDTH, OBJ_WIDTH);
            item_pos_move(next_disp_obj[0], lv_obj_get_style_x(next_disp_obj[0], 0), lv_obj_get_style_y(next_disp_obj[0], 0), fixed_pos[0].x, fixed_pos[0].y, 200);

            //item_pos_move(next_disp_obj[0], lv_obj_get_style_x(next_disp_obj[0], 0), lv_obj_get_style_y(next_disp_obj[0], 0), fixed_pos[0].x, fixed_pos[0].y, 200);
            //item_size_change(next_disp_obj[0], lv_obj_get_style_width(next_disp_obj[0], 0), OBJ_WIDTH, 200);
            for (int i = 0; i < 5; i++)
            {
                lv_area_t area;
                lv_obj_get_coords(panel_obj[i], &area);
                UI_PRINTF("sn:%d x1: %d x2:%d y1:%d y2:%d\r\n", i, area.x1, area.x2, area.y1, area.y2);
            }

            update_cur_disp_obj();
        }
    }

}

void panel_menu_test(void)
{

    //LV_IMG_DECLARE(ImgTestZoom);
    //lv_obj_t* test_obj = lv_img_create(lv_scr_act());
    //lv_img_set_src(test_obj, &ImgTestZoom);
    //lv_img_set_zoom(test_obj, 128);
    //lv_obj_center(test_obj);
    //return;
    static lv_style_t style_item;
    lv_style_init(&style_item);
    lv_style_set_radius(&style_item, LV_RADIUS_CIRCLE);
    lv_style_set_pad_all(&style_item, 0);
    lv_style_set_outline_width(&style_item, 0);
    lv_style_set_border_width(&style_item, 0);

    g = lv_group_create();
    lv_group_set_default(g);

    lv_indev_t* indev = NULL;

    do {

        indev = lv_indev_get_next(indev);
        if (indev == NULL)
        {
            break;
        }
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD)
        {
            lv_indev_set_group(indev, g);
        }
    } while (1);

    lv_obj_t* cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, 300, 300);
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_add_event_cb(cont, panel_event_cb, LV_EVENT_ALL, NULL);

    //lv_obj_t* move_obj = lv_obj_create(lv_scr_act());
    //lv_obj_add_style(move_obj, &style_item, 0);
    //lv_obj_set_size(move_obj, 100, 100);
    //lv_obj_set_style_bg_color(move_obj, lv_color_make(0, 255, 0), 0);
    //lv_obj_set_pos(move_obj, 0, 0);

    //item_pos_move(move_obj, lv_obj_get_style_x(move_obj, 0), lv_obj_get_style_y(move_obj, 0), 100, 100);
    //item_size_change(move_obj, lv_obj_get_style_width(move_obj, 0), 30);
    for (int i = 0; i < 5; i++)
    {
        panel_obj[i] = lv_obj_create(cont);
        lv_obj_add_style(panel_obj[i], &style_item, 0);
        lv_obj_set_style_bg_color(panel_obj[i], lv_color_make(40, 40, 40), 0);
        //lv_obj_add_flag(panel_obj[i], LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_pos(panel_obj[i], -200, -200);
    }

    ///* 对菜单进行布局 */
    for (int i = 0; i < 3; i++)
    {
        lv_obj_set_pos(panel_obj[i], fixed_pos[i].x, fixed_pos[i].y);
        if (i == 1)
        {
            lv_obj_set_size(panel_obj[i], FOCUSED_OBJ_WIDTH, FOCUSED_OBJ_WIDTH);
        }
        else 
        {
            lv_obj_set_size(panel_obj[i], OBJ_WIDTH, OBJ_WIDTH);
        }
        //lv_obj_clear_flag(panel_obj[i], LV_OBJ_FLAG_HIDDEN);
        cur_disp_obj[i] = panel_obj[i];
    }

    cur_focus_obj = panel_obj[1];
    cur_index = 1;

    lv_group_add_obj(g, cont);
    lv_group_focus_obj(cont);
}
#endif
