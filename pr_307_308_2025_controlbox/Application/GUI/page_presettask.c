/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "preset_task_item.h"
#include "app_data_center.h"
/*********************
 *      DEFINES
 *********************/
#define   PRESET_TASK_SCR_OPA           LV_OPA_50
#define   PRESET_TASK_CONT_WIDTH        125
#define   PRESET_TASK_CONT_HEIGHT       240
#define   PRESET_TASK_BTN_WIDTH         125
#define   PRESET_TASK_BTN_HEIGHT        47
#define   TASK_TYPE_BTN_WIDTH           28
#define   TASK_TYPE_BTN_HEIGHT          28

#define   DEFALUT_PARAM_GAP             -10

#define   LEVEL2_WIN_WIDTH              70
#define   LEVEL2_WIN_HEIGHT             100
#define   LEVEL3_WIN_WIDTH              45
#define   LEVEL3_WIN_HEIGHT             78
#define   SHADOW_WIDTH                  5

#define   PRESET_TASK_CONT_BG_COLOR             lv_color_make(0, 0, 0)
#define   PRESET_TASK_BTN_FOCUESD_BG_COLOR     lv_color_make(255,255,255)
#define   PRESET_TASK_BTN_TEXT_COLOR           lv_color_make(255, 255, 255)
#define   PRESET_TASK_BTN_FOCUSED_TEXT_COLOR   lv_color_make(17, 17, 16)

#define   TYPE_BTN_TEXT_COLOR           lv_color_make(0, 0, 0)

#define   IMG_NEXT_ARROW_COLOR          lv_color_make(255, 255, 255)
#define   IMG_NEXT_ARROW_FOCUSED_COLOR  lv_color_make(0, 0, 0)

#define   PARAM_NAME_TEXT_COLOR             lv_color_make(128, 128, 128)
#define   PARAM_VALUE_TEXT_COLOR            lv_color_make(255, 255, 255)
#define   PARAM_NAME_FOCUSED_TEXT_COLOR     lv_color_make(128, 128, 128)
#define   PARAM_VALUE_FOCUSED_TEXT_COLOR    lv_color_make(0, 0, 0)

#define   LEVEL2_WIN_BG_COLOR               lv_color_make(0, 0, 0)
#define   LEVEL2_WIN_FOCUSED_BG_COLOR       RED_THEME_COLOR
#define   LEVEL2_WIN_TEXT_COLOR             lv_color_make(255, 255, 255)
#define   LEVEL2_WIN_FOCUSED_TEXT_COLOR     lv_color_make(255, 255, 255)

#define   LEVEL3_WIN_BG_COLOR               lv_color_make(0, 0, 0)
#define   LEVEL3_WIN_FOCUSED_BG_COLOR       RED_THEME_COLOR
#define   LEVEL3_WIN_TEXT_COLOR             lv_color_make(255, 255, 255)
#define   LEVEL3_WIN_FOCUSED_TEXT_COLOR     lv_color_make(255, 255, 255)

#define   PRESET_TASK_CONFIRM_ITEM_DISABLE_COLOR    lv_color_make(161, 161, 161);
/*********************
 *      TPEDEFS
 *********************/
typedef uint8_t btn_type_t;

enum {
    BTN_ID_OPEN = 0,
    BTN_ID_REPLACE,
    BTN_ID_DEL
};

enum {
    BTN_ID_YES = 0,
    BTN_ID_NO
};
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void preset_task_btn_event(lv_event_t* e);
static void l2_btnmatrix_event(lv_event_t* e);
static void l3_btnmatrix_event(lv_event_t* e);
static void preset_scr_del_event(lv_event_t* e);
static void preset_task_panel_del(void);

static lv_obj_t* add_preset_btn_to_list(lv_obj_t* list);
static lv_obj_t* l2_menu_create(void);
static void l2_menu_del(void);
static lv_obj_t* l3_menu_create(void);
static void l3_menu_del(void);

static bool page_is_valid(void);
static void anim_x_cb(void * var, int32_t v);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t* indev_group;
static lv_group_t* l2_indev_group;
static lv_group_t* l3_indev_group;

static lv_obj_t* l2_sub_menu;
static lv_obj_t* l3_sub_menu;
static lv_obj_t* preset_task_list;
static lv_obj_t* preset_task_scr;

/**********************
 *       MACROS
 **********************/

/**********************
 *   GLOBAL VARIABLES
 **********************/
#if EFFECTS_USE_PULSING
LV_IMG_DECLARE(ImgPresetPulsing)
#endif
#if EFFECTS_USE_STROBE
LV_IMG_DECLARE(ImgPresetStrobe)
#endif
#if EFFECTS_USE_EXPLOSIONS
LV_IMG_DECLARE(ImgPresetExplosion)
#endif 
#if EFFECTS_USE_FAULT_BULB
LV_IMG_DECLARE(ImgPresetFaultyBulb)
#endif   
#if EFFECTS_USE_WELDING 
LV_IMG_DECLARE(ImgPresetWelding)
#endif  
#if EFFECTS_USE_COP_CAR 
LV_IMG_DECLARE(ImgPresetCopcar)
#endif
#if EFFECTS_USE_CANDLE 
LV_IMG_DECLARE(ImgPresetCandle)
#endif
#if EFFECTS_USE_CLUB_LIGHTS 
LV_IMG_DECLARE(ImgPresetClubLights)
#endif
#if EFFECTS_USE_COLOR_CHASE 
LV_IMG_DECLARE(ImgPresetColorChase)
#endif
#if EFFECTS_USE_TV 
LV_IMG_DECLARE(ImgPresetTV)
#endif     
#if EFFECTS_USE_PAPARAZZI 
LV_IMG_DECLARE(ImgPresetPaparazzi)
#endif
#if EFFECTS_USE_LIGHTNING 
LV_IMG_DECLARE(ImgPresetLightning)
#endif
#if EFFECTS_USE_FIREWORKS 
LV_IMG_DECLARE(ImgPresetFireoworks)
#endif
#if EFFECTS_USE_FIRE 
LV_IMG_DECLARE(ImgPresetFire)
#endif      
#if EFFECTS_USE_PARTY 
LV_IMG_DECLARE(ImgPresetPapartyLights)
#endif
/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t* page_preset_task_init(lv_obj_t* parent)
{
    indev_group = lv_group_create();
    l2_indev_group = lv_group_create();
    l3_indev_group = lv_group_create();
    my_indev_set_group(indev_group);

    preset_task_scr = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(preset_task_scr);
    lv_obj_set_size(preset_task_scr, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(preset_task_scr, PRESET_TASK_SCR_OPA, 0);
    lv_obj_set_style_radius(preset_task_scr, 0, 0);
    lv_obj_add_event_cb(preset_task_scr, preset_scr_del_event, LV_EVENT_DELETE, NULL);

    preset_task_list = lv_obj_create(preset_task_scr) ;    
    lv_obj_add_style(preset_task_list, &style_rect_panel, 0);  
    lv_obj_add_flag(preset_task_list, LV_OBJ_FLAG_SCROLLABLE);      
    lv_obj_set_style_bg_color(preset_task_list, PRESET_TASK_CONT_BG_COLOR, 0);
    lv_obj_set_size(preset_task_list, 125, lv_pct(100));
    lv_obj_set_style_radius(preset_task_list, 0, 0);
    lv_obj_set_flex_flow(preset_task_list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(preset_task_list, 0, 0);
    lv_obj_set_style_pad_column(preset_task_list, 0, 0);
    lv_obj_set_style_pad_all(preset_task_list, 0, 0);
    lv_obj_set_style_width(preset_task_list, 2, LV_PART_SCROLLBAR);    
    lv_obj_set_style_pad_top(preset_task_list, 0, LV_PART_SCROLLBAR);
    lv_obj_set_style_pad_right(preset_task_list, 0, LV_PART_SCROLLBAR);
    lv_obj_set_scrollbar_mode(preset_task_list, LV_SCROLLBAR_MODE_ON);
    lv_obj_set_style_bg_opa(preset_task_list, LV_OPA_50, LV_PART_SCROLLBAR);
    lv_obj_set_scroll_dir(preset_task_list, LV_DIR_VER);

    lv_obj_t* btn;
    preset_data_t param;
    for (int i = 0; i < PRESET_TASK_MAXIUM; i++)
    {
        btn = add_preset_btn_to_list(preset_task_list);        
        ui_preset_task_get(i, &param);
        preset_task_item_set_param(btn, &param);
        lv_obj_add_flag(btn, LV_OBJ_FLAG_SCROLL_ON_FOCUS);        
        lv_obj_add_event_cb(btn, preset_task_btn_event, LV_EVENT_ALL, NULL);
        lv_obj_invalidate(btn);
        lv_group_add_obj(indev_group, btn);
    }

    lv_group_set_editing(indev_group, false);
    lv_group_set_wrap(indev_group, false);
    lv_obj_scroll_to_view(preset_task_list, LV_ANIM_OFF);

    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, preset_task_list);
    lv_anim_set_values(&anim, -120, 0); //120 文字不会闪
    lv_anim_set_time(&anim, 150);
    lv_anim_set_exec_cb(&anim, anim_x_cb);
    //lv_anim_set_path_cb(&anim, lv_anim_path_bounce);
    lv_anim_set_path_cb(&anim, lv_anim_path_bounce);
    lv_anim_start(&anim);
    
    return preset_task_scr;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void preset_task_panel_del(void)
{
    uint32_t key = LV_KEY_BACKSPACE;    
    lv_group_t* default_g = lv_group_get_default();                    

    obj_send_event_to_screen(LV_EVENT_KEY, (void *)&key);                            
    my_indev_set_group(default_g);                   
}

static void preset_task_btn_event(lv_event_t* e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);

    if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());

        if (LV_KEY_BACKSPACE == key)
        {
           preset_task_panel_del();             
        }
        else if (LV_KEY_DEL == key)
        {
            uint8_t index = page_get_index(PAGE_MENU);//获取上一次的字ID
            screen_load_page_and_index(PAGE_MENU, 0, index, true);
        }        
    }
    else if (event == LV_EVENT_CLICKED)
    {
		struct sys_info_dmx        dmx;
		
		data_center_read_sys_info(SYS_INFO_DMX, &dmx);
		if((dmx.eth_linked == 1 || dmx.wired_linked == 1 || dmx.wireless_linked == 1))
		{
			return;
		}
        if (page_is_valid())
        {
            if (preset_task_item_get_type(obj) == LIGHT_MODE_FACTORY_PWM) {
                //save the preset task setting.
                lv_obj_t* content_cont = lv_obj_get_child(obj, -1);
                uint8_t id = lv_obj_get_child_id(obj);
                preset_data_t task;
                                        
                ui_preset_task_save(id, &task);
                preset_task_item_set_param(obj, &task);
                lv_event_send(obj, LV_EVENT_FOCUSED, NULL); 
				lv_obj_invalidate(preset_task_list);                   
                return ;         
            }                  
        }   

        lv_obj_add_state(obj, LV_STATE_USER_1);
        l2_menu_create();  
    }
}

static void l2_btnmatrix_event(lv_event_t* e)
{
    lv_obj_t* obj = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);

    if (event == LV_EVENT_DRAW_PART_BEGIN)
    {
        lv_obj_draw_part_dsc_t * dsc = lv_event_get_param(e);
        if (lv_btnmatrix_get_selected_btn(obj) == dsc->id)
        {
            if (lv_btnmatrix_has_btn_ctrl(obj, dsc->id, LV_BTNMATRIX_CTRL_CUSTOM_1))
            {
                dsc->label_dsc->color = lv_color_white();                
                dsc->rect_dsc->bg_color = PRESET_TASK_CONFIRM_ITEM_DISABLE_COLOR;
            }            
        }
        // [!example]
        // if(lv_btnmatrix_get_selected_btn(obj) == dsc->id)
        // {
        //     dsc->label_dsc->color = lv_color_make(255, 0, 0);
        //     dsc->rect_dsc->bg_color = lv_color_make(0, 0, 0);
        // }
        // else
        // {
        //     dsc->label_dsc->color = lv_color_make(0, 255, 0);
        //     dsc->rect_dsc->bg_color = lv_color_make(255, 255, 255);
        // }
    }
    else if (event == LV_EVENT_DRAW_PART_END)
    {

    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);        
        if (LV_KEY_BACKSPACE == key)
        {
            l2_menu_del();
            lv_group_focus_freeze(indev_group, false);
            my_indev_set_group(indev_group);
        }
        else if (LV_KEY_DEL == key)
        {
            uint8_t index = page_get_index(PAGE_MENU);//获取上一次的字ID
            screen_load_page_and_index(PAGE_MENU, 0, index, true);
        }
    }
    else if (event == LV_EVENT_RELEASED)
    {
        uint32_t key = lv_event_get_key(e);
        uint8_t id = (uint8_t)lv_btnmatrix_get_selected_btn(obj);
        if (lv_btnmatrix_has_btn_ctrl(obj, id, LV_BTNMATRIX_CTRL_CUSTOM_1))
        {
            l2_menu_del();
            lv_group_focus_freeze(indev_group, false);
            my_indev_set_group(indev_group);
            return;
        }

        lv_obj_invalidate(preset_task_list);

        if (BTN_ID_OPEN == id)
        {
            //删除整个屏幕， 然后跳转页面
            uint32_t key = LV_KEY_BACKSPACE;
            lv_group_t* default_g = lv_group_get_default();
            lv_obj_t* btn = lv_group_get_focused(indev_group);
            uint8_t page_id;
            if (preset_task_item_get_type(btn) != LIGHT_MODE_FACTORY_PWM)
            {
                page_id = ui_preset_task_apply(lv_obj_get_child_id(btn), preset_task_item_get_param(btn));
                obj_send_event_to_screen(LV_EVENT_KEY, (void*)&key);
                my_indev_set_group(default_g);                    
                screen_load_page(page_id, 0, false);

                //如果跳转ID和当前ID相同， 设置页面部分刷新
                if (page_id == screen_get_act_pid())
                {
                    page_set_invalid_type(PAGE_INVALID_TYPE_ALL);
                }
            }
        }
        else
        {
            uint8_t l2_btn_sel_id = (uint8_t)lv_btnmatrix_get_selected_btn(l2_sub_menu);                
            lv_btnmatrix_set_btn_ctrl(l2_sub_menu, l2_btn_sel_id, LV_BTNMATRIX_CTRL_CUSTOM_1);
            l3_menu_create();
        }
    }
}

static void l2_menu_del(void)
{
    //2. 修改删除l3\l2
    lv_obj_t* l2_menu_cont = lv_obj_get_parent(l2_sub_menu);
    lv_group_set_editing(l2_indev_group, false);
    lv_group_focus_freeze(l2_indev_group, true);    
    lv_obj_del_async(l2_menu_cont);    
}

static lv_obj_t* l2_menu_create(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(l2_indev_group);
    lv_group_focus_freeze(l2_indev_group, false);
    lv_group_set_editing(l2_indev_group, false);  
    my_indev_set_group(l2_indev_group);

    lv_obj_t* l2_menu_cont = lv_obj_create(preset_task_scr);
    lv_obj_set_style_pad_all(l2_menu_cont, 0, 0);
    lv_obj_set_style_border_width(l2_menu_cont, 0, 0);
    lv_obj_set_style_bg_color(l2_menu_cont, LEVEL2_WIN_BG_COLOR, 0);
    lv_obj_set_style_clip_corner(l2_menu_cont, false, 0);
    lv_obj_set_size(l2_menu_cont, LEVEL2_WIN_WIDTH, LEVEL2_WIN_HEIGHT);
    
    lv_obj_t* shadow = lv_obj_create(l2_menu_cont);
    lv_obj_add_style(shadow, &style_rect_panel, 0);
    lv_obj_set_style_bg_color(shadow, PRESET_TASK_CONT_BG_COLOR, 0);
    lv_obj_set_style_bg_grad_color(shadow, LEVEL2_WIN_BG_COLOR, 0);
    lv_obj_set_style_bg_grad_dir(shadow, LV_GRAD_DIR_HOR, 0);
    lv_obj_set_style_clip_corner(shadow, false, 0);
    lv_obj_set_size(shadow, SHADOW_WIDTH, lv_pct(100));
    lv_obj_set_pos(shadow, 0, 0);

    static const char* map[] = { "Open", "\n", "Replace", "\n", "Del", "" };
    //static lv_btnmatrix_ctrl_t ctrl_map = {_LV_BTNMATRIX_WIDTH, LV_BTNMATRIX_CTRL_DISABLED, _LV_BTNMATRIX_WIDTH};
    map[0] = Lang_GetStringByID(STRING_ID_OPEN);
    map[2] = Lang_GetStringByID(STRING_ID_REPLACE);
    map[4] = Lang_GetStringByID(STRING_ID_DELETE);
    
    l2_sub_menu = lv_btnmatrix_create(l2_menu_cont);
    lv_btnmatrix_set_map(l2_sub_menu, map);    

    //lv_obj_t* align_obj = lv_group_get_focused(indev_group);
    lv_group_t* tmp = lv_obj_get_group(l2_sub_menu);
    lv_group_remove_obj(l2_sub_menu);

    lv_obj_add_style(l2_sub_menu, &style_levelx_win, 0);
    lv_obj_add_style(l2_sub_menu, &style_levelx_btn, LV_PART_ITEMS);
    lv_obj_set_style_outline_width(l2_sub_menu, 0, LV_STATE_EDITED);
    lv_obj_set_style_outline_width(l2_sub_menu, 0, LV_STATE_EDITED | LV_STATE_FOCUSED | LV_PART_ITEMS);
    lv_obj_set_style_radius(l2_sub_menu, 5, LV_PART_ITEMS | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(l2_sub_menu, LEVEL2_WIN_FOCUSED_BG_COLOR, LV_PART_ITEMS | LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(l2_sub_menu, LEVEL2_WIN_FOCUSED_TEXT_COLOR, LV_PART_ITEMS | LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(l2_sub_menu, LEVEL2_WIN_TEXT_COLOR, LV_PART_ITEMS);

    lv_obj_add_event_cb(l2_sub_menu, l2_btnmatrix_event, LV_EVENT_ALL, NULL);
    lv_obj_set_size(l2_sub_menu, LEVEL2_WIN_WIDTH - 2 * SHADOW_WIDTH, lv_pct(100)); //中英文需要手动配置.
    lv_obj_set_style_height(l2_sub_menu, 14, LV_PART_ITEMS);
    lv_obj_center(l2_sub_menu);

    //计算动画位置.
    lv_obj_t* align_obj = lv_group_get_focused(indev_group);
    lv_coord_t pos_y = align_obj->coords.y1;
    lv_align_t align = LV_ALIGN_OUT_RIGHT_MID;

    if (pos_y < (240 * 20 / 100))
    {
        align = LV_ALIGN_OUT_RIGHT_TOP;
    }
    else if (pos_y > (240 - (240 * 30 / 100)))
    {
        align = LV_ALIGN_OUT_RIGHT_BOTTOM;
    }
    
    lv_obj_align_to(l2_menu_cont, align_obj, align, 0, 0);

    //判断各个按键是否有效
    lv_obj_t* btn = lv_group_get_focused(indev_group);
    if (preset_task_item_get_type(btn) == LIGHT_MODE_FACTORY_PWM)
    {
        lv_btnmatrix_set_btn_ctrl(l2_sub_menu, BTN_ID_OPEN, LV_BTNMATRIX_CTRL_CUSTOM_1);
        lv_btnmatrix_set_btn_ctrl(l2_sub_menu, BTN_ID_DEL, LV_BTNMATRIX_CTRL_CUSTOM_1);
    }    

    if (!page_is_valid())
    {
        lv_btnmatrix_set_btn_ctrl(l2_sub_menu, BTN_ID_REPLACE, LV_BTNMATRIX_CTRL_CUSTOM_1);
    }    

    lv_group_add_obj(l2_indev_group, l2_sub_menu);    
    lv_group_focus_obj(l2_sub_menu);
    lv_group_set_editing(l2_indev_group, true);
    lv_group_set_wrap(l2_indev_group, false);

    lv_obj_invalidate(preset_task_list);
    return l2_sub_menu;
}

/**
 * @brief 释放预设任务页面资源
 * 
 * @param e 
 */
static void preset_scr_del_event(lv_event_t* e)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);

    lv_group_focus_freeze(l2_indev_group, true);
    lv_group_remove_all_objs(l2_indev_group);
    lv_group_del(l2_indev_group);

    lv_group_focus_freeze(l3_indev_group, true);
    lv_group_remove_all_objs(l3_indev_group);
    lv_group_del(l3_indev_group);    
}

static void l3_btnmatrix_event(lv_event_t* e)
{   
    lv_obj_t* obj = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e); 
    if (event == LV_EVENT_KEY)
    {   
        uint32_t key = *(uint32_t *)lv_event_get_param(e);        
        if (LV_KEY_BACKSPACE == key)
        {
            l3_menu_del();
            uint8_t l2_btn_sel_id = (uint8_t)lv_btnmatrix_get_selected_btn(l2_sub_menu);
            lv_btnmatrix_clear_btn_ctrl(l2_sub_menu, l2_btn_sel_id, LV_BTNMATRIX_CTRL_CUSTOM_1);
            lv_group_focus_freeze(l2_indev_group, false);
            my_indev_set_group(l2_indev_group);                          
            lv_obj_invalidate(l2_sub_menu);
        }
        else if (LV_KEY_DEL == key)
        {
            uint8_t index = page_get_index(PAGE_MENU);//获取上一次的字ID
            screen_load_page_and_index(PAGE_MENU, 0, index, true);
        }        
    }
    else if (event == LV_EVENT_CLICKED)
    {                
        uint8_t btn_id = (uint8_t)lv_btnmatrix_get_selected_btn(obj);
        lv_obj_invalidate(preset_task_list);
        
        if (BTN_ID_YES == btn_id)
        {
            uint8_t l2_btn_sel_id = (uint8_t)lv_btnmatrix_get_selected_btn(l2_sub_menu);
        
            l2_menu_del();
            l3_menu_del();

            lv_group_focus_freeze(indev_group, false);
            my_indev_set_group(indev_group);

            lv_obj_t* btn = lv_group_get_focused(indev_group);
            lv_obj_t* content_cont = lv_obj_get_child(btn, -1);
            uint8_t id = lv_obj_get_child_id(btn);
            preset_data_t task;
            switch(l2_btn_sel_id)
            {
                case BTN_ID_REPLACE:
                {
                    ui_preset_task_save(id, &task);
                }
                break;
                case BTN_ID_DEL:
                {
                    ui_preset_task_del(id);
                    task.mode = LIGHT_MODE_FACTORY_PWM;
                }
                break;
            }

            preset_task_item_set_param(btn, &task);
            lv_event_send(btn, LV_EVENT_FOCUSED, NULL);       
        }
        else if (BTN_ID_NO == btn_id)
        {
            l3_menu_del();
            uint8_t l2_btn_sel_id = (uint8_t)lv_btnmatrix_get_selected_btn(l2_sub_menu);
            lv_btnmatrix_clear_btn_ctrl(l2_sub_menu, l2_btn_sel_id, LV_BTNMATRIX_CTRL_CUSTOM_1);
            lv_group_focus_freeze(l2_indev_group, false);
            my_indev_set_group(l2_indev_group);            
        }        
    }
}

static void l3_menu_del(void)
{
    lv_obj_t* l3_menu_cont = lv_obj_get_parent(l3_sub_menu);
    lv_group_set_editing(l3_indev_group, false);
    lv_group_focus_freeze(l3_indev_group, true);
    lv_obj_del_async(l3_menu_cont);
}

static lv_obj_t* l3_menu_create(void)
{
    lv_group_focus_freeze(l2_indev_group, true);
    lv_group_remove_all_objs(l3_indev_group);
    lv_group_set_editing(l3_indev_group, false);  
    lv_group_focus_freeze(l3_indev_group, false);
    my_indev_set_group(l3_indev_group);

    lv_obj_t* l3_menu_cont = lv_obj_create(preset_task_scr);
    lv_obj_set_style_pad_all(l3_menu_cont, 0, 0);
    lv_obj_set_style_border_width(l3_menu_cont, 0, 0);
    lv_obj_set_style_bg_color(l3_menu_cont, LEVEL3_WIN_BG_COLOR, 0);
    lv_obj_set_style_clip_corner(l3_menu_cont, false, 0);
    lv_obj_set_size(l3_menu_cont, LEVEL3_WIN_WIDTH, LEVEL3_WIN_HEIGHT);

    lv_obj_t* shadow = lv_obj_create(l3_menu_cont);
    lv_obj_add_style(shadow, &style_rect_panel, 0);
    lv_obj_set_style_bg_color(shadow, LEVEL2_WIN_BG_COLOR, 0);
    lv_obj_set_style_bg_grad_color(shadow, LEVEL3_WIN_BG_COLOR, 0);
    lv_obj_set_style_bg_grad_dir(shadow, LV_GRAD_DIR_HOR, 0);
    lv_obj_set_style_clip_corner(shadow, false, 0);
    lv_obj_set_size(shadow, SHADOW_WIDTH, lv_pct(100));
    lv_obj_set_pos(shadow, 0, 0);
    
    static const char* map[] = { "Yes", "\n", "No", "" };    
    map[0] = Lang_GetStringByID(STRING_ID_YES);
    map[2] = Lang_GetStringByID(STRING_ID_NO);
    
    l3_sub_menu = lv_btnmatrix_create(l3_menu_cont);
    lv_group_remove_obj(l3_sub_menu);
    lv_btnmatrix_set_map(l3_sub_menu, map);
    lv_obj_add_style(l3_sub_menu, &style_levelx_win, 0);
    lv_obj_add_style(l3_sub_menu, &style_levelx_btn, LV_PART_ITEMS);
    lv_obj_set_style_pad_top(l3_sub_menu, 9, 0);
    lv_obj_set_style_pad_bottom(l3_sub_menu, 9,0);
    lv_obj_set_style_bg_color(l3_sub_menu, LEVEL3_WIN_BG_COLOR, 0);
    lv_obj_set_style_bg_color(l3_sub_menu, LEVEL3_WIN_BG_COLOR, LV_PART_ITEMS);
    lv_obj_set_style_outline_width(l3_sub_menu, 0, LV_STATE_EDITED);
    lv_obj_set_style_outline_width(l3_sub_menu, 0, LV_STATE_EDITED | LV_STATE_FOCUSED | LV_PART_ITEMS);
    lv_obj_set_style_radius(l3_sub_menu, 5, LV_PART_ITEMS | LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(l3_sub_menu, LEVEL2_WIN_FOCUSED_BG_COLOR, LV_PART_ITEMS | LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(l3_sub_menu, LEVEL2_WIN_FOCUSED_TEXT_COLOR, LV_PART_ITEMS | LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(l3_sub_menu, LEVEL3_WIN_TEXT_COLOR, LV_PART_ITEMS);
    lv_obj_add_event_cb(l3_sub_menu, l3_btnmatrix_event, LV_EVENT_ALL, NULL);
    lv_obj_set_size(l3_sub_menu, LEVEL3_WIN_WIDTH - 2 * SHADOW_WIDTH, lv_pct(100)); //中英文需要手动配置.
    lv_obj_center(l3_sub_menu);

    ////计算动画位置.
    //lv_obj_t* align_obj = lv_group_get_focused(l2_indev_group);
    lv_obj_align_to(l3_menu_cont, lv_obj_get_parent(l2_sub_menu), LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    
    lv_group_add_obj(l3_indev_group, l3_sub_menu);
    lv_group_focus_obj(l3_sub_menu);
    lv_group_set_editing(l3_indev_group, true);   
    lv_group_set_wrap(l3_indev_group, false);
    //UI_PRINTF("indev_group:%d\r\n", lv_group_get_obj_count(l2_indev_group));

    return l3_sub_menu;
}

static lv_obj_t* add_preset_btn_to_list(lv_obj_t* list)
{
    lv_obj_t* btn = preset_task_item_create(list);    
    //lv_obj_t* btn = lv_btn_create(list); 
    lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    lv_obj_add_style(btn, &style_rect_panel, 0);
    lv_obj_set_style_pad_gap(btn, 0, 0);
    lv_obj_set_style_pad_all(btn, 0, 0);
    lv_obj_set_style_radius(btn, 0, 0);
    lv_obj_set_size(btn, lv_pct(100), 48);        
    lv_obj_set_style_text_font(btn, Font_ResouceGet(FONT_14), 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_100, 0);
    lv_obj_set_style_bg_color(btn, PRESET_TASK_CONT_BG_COLOR, 0);
    lv_obj_set_style_text_color(btn, PRESET_TASK_BTN_TEXT_COLOR, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_100, LV_STATE_FOCUSED);
    lv_obj_set_style_bg_color(btn, PRESET_TASK_BTN_FOCUESD_BG_COLOR, LV_STATE_FOCUSED);
    //lv_obj_set_style_bg_color(btn, lv_color_make(128, 128, 128), LV_STATE_USER_1); 添加失焦颜色
    lv_obj_set_style_text_color(btn, PRESET_TASK_BTN_FOCUSED_TEXT_COLOR, LV_STATE_FOCUSED);
    lv_obj_set_style_border_side(btn, LV_BORDER_SIDE_BOTTOM, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(btn, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_set_style_border_color(btn, lv_color_make(90,90,90), LV_STATE_DEFAULT);
    return btn;

}

static bool page_is_valid(void)
{
    uint8_t pid = screen_get_act_pid();
    if (((pid >= PAGE_LIGHT_MODE_HSI) && (pid <= PAGE_LIGHT_MODE_CCT)) \
            || ((pid >= PAGE_PULSING) && (pid <= PAGE_PARTYLIGHT)))
        return true;
    return false;
}

static void anim_x_cb(void * var, int32_t v)
{
    lv_obj_set_x(var, v);
}
