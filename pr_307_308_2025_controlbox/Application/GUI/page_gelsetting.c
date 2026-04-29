/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_widget.h"

/*********************
 *      DEFINES
 *********************/
#define GEL_TABLE_CONT_HEIGHT       228
#define BTN_DIVIDER_LINE_COLOR      lv_color_make(255, 0, 0)
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void cct_btn_ralease_cb(gel_switch_btn_t* btn);
static void brand_btn_release_cb(gel_switch_btn_t* btn);
static void serials_cont_event_cb(lv_event_t* e);
static void cont_table_event_cb(lv_event_t* e);

static void page_gel_setting_construct(void);
static void page_gel_setting_destruct(void);
static void page_gel_settiing_storage(void);
/**********************
 *  STATIC VARIABLES
 **********************/
static lv_group_t *indev_group;
static struct db_fx_mode_arg* p_gel_setting_model;
static gel_switch_btn_t cct_btn;
static gel_switch_btn_t brand_btn;
static lv_obj_t* serials_cont;

static lv_obj_t* gel_table_cont;

static custom_cont_t serial_cont_dsc;
static custom_cont_t gel_table_dsc;
static void page_gel_time_upd_cb(uint32_t ms);
static adj_btn_t btn_list[4] ={0};
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_gel_setting = PAGE_INITIALIZER("gel setting",
                                PAGE_GEL_SETTING, 
                                page_gel_setting_construct, 
                                page_gel_setting_destruct, 
                                page_gel_time_upd_cb,
                                NULL,                                
                                PAGE_NONE);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void page_gel_setting_init(lv_obj_t* parent)
{
    p_gel_setting_model = (struct db_fx_mode_arg*)screen_get_act_page_data();

    indev_group = lv_group_create();        
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);

    lv_obj_set_style_bg_color(parent, lv_color_black(), 0);

    lv_obj_t* obj_panel = lv_obj_create(parent);
    lv_obj_remove_style_all(obj_panel);
    lv_obj_set_size(obj_panel, lv_pct(100), lv_pct(98));
    lv_obj_center(obj_panel);

    gel_table_cont = lv_obj_create(obj_panel);
    lv_obj_add_style(gel_table_cont, &style_rect_panel, 0);
    lv_obj_set_size(gel_table_cont, lv_pct(65), GEL_TABLE_CONT_HEIGHT);    
    lv_obj_align(gel_table_cont, LV_ALIGN_LEFT_MID, 6, 0);    
    lv_obj_set_flex_flow(gel_table_cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_gap(gel_table_cont, 0, 0);    
    lv_obj_set_user_data(gel_table_cont, &gel_table_dsc);

    gel_cont_table_init(gel_table_cont, &(p_gel_setting_model->gel),76);
    gel_cont_table_fliping_refresh(gel_table_cont, &(p_gel_setting_model->gel), gel_model_get_index((&p_gel_setting_model->gel)));
    lv_obj_add_event_cb(gel_table_cont, cont_table_event_cb, LV_EVENT_ALL, NULL);
	btn_list[0].obj = gel_table_cont;
	
    cct_btn.btn_id = 0;
    cct_btn.sel_index = p_gel_setting_model->gel.cct == 3200 ? 0 : 1;
    cct_btn.ralease_cb = cct_btn_ralease_cb;
    lv_obj_t* cont1 = gel_switch_btn_create(obj_panel, "3200K", "5600K", lv_pct(25), 19);
    lv_obj_clear_flag(cont1, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align_to(cont1, gel_table_cont, LV_ALIGN_OUT_RIGHT_TOP, 8, 14);
    lv_obj_set_user_data(cont1, &cct_btn);
    lv_obj_add_event_cb(cont1, gel_switch_btn_event, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, cont1);
    lv_event_send(cont1, LV_EVENT_DEFOCUSED, NULL);
	btn_list[1].obj = cont1;
	
    brand_btn.btn_id = 1;
    brand_btn.sel_index = p_gel_setting_model->gel.brand;
    brand_btn.ralease_cb = brand_btn_release_cb;
    lv_obj_t* cont2 = gel_switch_btn_create(obj_panel, "R", "L", lv_pct(25), 19);
    lv_obj_clear_flag(cont2, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align_to(cont2, cont1, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_set_user_data(cont2, &brand_btn);
    lv_obj_add_event_cb(cont2, gel_switch_btn_event, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, cont2);
    lv_event_send(cont2, LV_EVENT_DEFOCUSED, NULL);
    btn_list[2].obj = cont2;
    serials_cont = lv_obj_create(obj_panel);
    serial_cont_dsc.obj = serials_cont;
    lv_obj_add_style(serials_cont, &style_rect_panel, 0);
    lv_obj_set_scroll_dir(serials_cont, LV_DIR_NONE);
    lv_obj_set_scrollbar_mode(serials_cont, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_size(serials_cont, lv_pct(25), 144);
    lv_obj_set_flex_flow(serials_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(serials_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_gap(serials_cont, 10, 0);
    lv_obj_set_style_outline_width(serials_cont, 1, LV_STATE_FOCUSED);
    lv_obj_set_style_outline_color(serials_cont, lv_color_white(), LV_STATE_FOCUSED);
    //lv_obj_set_style_pad_left(serials_cont, 15, 0);
    lv_obj_align_to(serials_cont, cont2, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_set_user_data(serials_cont, &serial_cont_dsc);
    serials_cont_init(serials_cont, &(p_gel_setting_model->gel));
    //lv_obj_add_event_cb(serials_cont, )
    lv_obj_add_event_cb(serials_cont, serials_cont_event_cb, LV_EVENT_ALL, NULL);
    lv_event_send(serials_cont, LV_EVENT_DEFOCUSED, NULL);
    lv_group_add_obj(indev_group, serials_cont);
    //lv_group_add_obj(indev_group, gel_table_cont);
    lv_group_focus_obj(cont1);
    lv_group_set_wrap(indev_group, false);
	btn_list[3].obj = serials_cont;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/
static void page_gel_setting_construct(void)
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    lv_obj_t* b_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(b_cont);
    lv_obj_set_size(b_cont, lv_pct(100), lv_pct(100));
    lv_obj_center(b_cont);

    page_gel_setting_init(b_cont);
}

static void page_gel_setting_destruct(void)
{
    lv_group_focus_freeze(indev_group, true);
    lv_group_remove_all_objs(indev_group);
    lv_group_del(indev_group);
    indev_group = NULL;

    lv_obj_clean(lv_scr_act());
	page_gel_settiing_storage();
}

static void cont_table_event_cb(lv_event_t* e)
{
    backspace_key_event_handler(e);
    gel_cont_table_fliping_event_cb(e, &(p_gel_setting_model->gel));
}

static void serials_cont_event_cb(lv_event_t* e)
{
    shortcut_keys_event_handler(e);    
    backspace_key_event_handler(e);
    gel_serials_cont_event_cb(e, &(p_gel_setting_model->gel), gel_table_cont);    
}

static void cct_btn_ralease_cb(gel_switch_btn_t* btn)
{
    if (btn->sel_index == 0)
    {
        p_gel_setting_model->gel.cct = 3200;
    }
    else  
    {
        p_gel_setting_model->gel.cct = 5600;
    }    
}

static void brand_btn_release_cb(gel_switch_btn_t* btn)
{
    if (p_gel_setting_model->gel.brand != btn->sel_index)
    {
        p_gel_setting_model->gel.brand = btn->sel_index;
        lv_obj_clean(serials_cont);
        serials_cont_init(serials_cont, &(p_gel_setting_model->gel));
        gel_cont_table_fliping_refresh(gel_table_cont, &(p_gel_setting_model->gel), gel_model_get_index((&p_gel_setting_model->gel)));        
    }
}

static void page_gel_settiing_storage(void)
{
	#if PROJECT_TYPE==307
    uint32_t src_crc32, dest_crc32;
	uint8_t mode;
	mode = date_center_get_light_mode();
	switch(mode)
	{	
		case LIGHT_MODE_FX_LIGHTNING_II:
		{
			struct db_fx_lightning_2 lightning;
			struct db_fx_lightning_2  p_lightning_model;
			data_center_read_light_data(date_center_get_light_mode(), &lightning);   
			dest_crc32 = ui_calc_checksum((uint8_t*)&lightning,  sizeof(struct db_fx_lightning_2)); 
			memcpy(&p_lightning_model, &g_tUIAllData.lightning2_model, sizeof(struct db_fx_lightning_2));
			src_crc32 = ui_calc_checksum((uint8_t*)&g_tUIAllData.lightning2_model, sizeof(struct db_fx_lightning_2));            
			if (dest_crc32 != src_crc32)
			{
				data_center_write_light_data(LIGHT_MODE_FX_LIGHTNING_II, &p_lightning_model);
				memcpy(p_gel_setting_model,&g_tUIAllData.lightning2_model.mode_arg,sizeof(g_tUIAllData.lightning2_model.mode_arg));
			}
		}
		break;
		
		case LIGHT_MODE_FX_STROBE_II:
		{
			struct db_fx_strobe_2 strobe;
			struct db_fx_strobe_2  p_strobe_model;
			data_center_read_light_data(date_center_get_light_mode(), &strobe);   
			dest_crc32 = ui_calc_checksum((uint8_t*)&strobe,  sizeof(struct db_fx_strobe_2)); 
			memcpy(&p_strobe_model, &g_tUIAllData.strobe2_model, sizeof(struct db_fx_strobe_2));
			src_crc32 = ui_calc_checksum((uint8_t*)&g_tUIAllData.strobe2_model, sizeof(struct db_fx_strobe_2));            
			if (dest_crc32 != src_crc32)
			{
				data_center_write_light_data(LIGHT_MODE_FX_STROBE_II, &p_strobe_model);
				memcpy(p_gel_setting_model,&g_tUIAllData.strobe2_model.mode_arg,sizeof(g_tUIAllData.strobe2_model.mode_arg)); 
			}
		}
		break;
		
		case LIGHT_MODE_FX_EXPLOSION_II:
		{
			struct db_fx_explosion_2 explosion;
			struct db_fx_explosion_2  p_explosion_model;
			data_center_read_light_data(date_center_get_light_mode(), &explosion);   
			dest_crc32 = ui_calc_checksum((uint8_t*)&explosion,  sizeof(struct db_fx_explosion_2)); 
			memcpy(&p_explosion_model, &g_tUIAllData.explosion2_model, sizeof(struct db_fx_explosion_2));
			src_crc32 = ui_calc_checksum((uint8_t*)&g_tUIAllData.explosion2_model, sizeof(struct db_fx_explosion_2));            
			if (dest_crc32 != src_crc32)
			{
				data_center_write_light_data(LIGHT_MODE_FX_EXPLOSION_II, &p_explosion_model);
				memcpy(p_gel_setting_model,&g_tUIAllData.explosion2_model.mode_arg,sizeof(g_tUIAllData.explosion2_model.mode_arg)); 
			}
		}
		break;
		
			case LIGHT_MODE_FX_FAULT_BULB_II:
		{
			struct db_fx_fault_bulb_2 fault_bulb;
			struct db_fx_fault_bulb_2  p_fault_bulb_model;
			data_center_read_light_data(date_center_get_light_mode(), &fault_bulb);   
			dest_crc32 = ui_calc_checksum((uint8_t*)&fault_bulb,  sizeof(struct db_fx_fault_bulb_2)); 
			memcpy(&p_fault_bulb_model, &g_tUIAllData.faultybulb2_model, sizeof(struct db_fx_fault_bulb_2));
			src_crc32 = ui_calc_checksum((uint8_t*)&g_tUIAllData.faultybulb2_model, sizeof(struct db_fx_fault_bulb_2));            
			if (dest_crc32 != src_crc32)
			{
				data_center_write_light_data(LIGHT_MODE_FX_FAULT_BULB_II, &p_fault_bulb_model);
				memcpy(p_gel_setting_model,&g_tUIAllData.faultybulb2_model.mode_arg,sizeof(g_tUIAllData.faultybulb2_model.mode_arg));
			}
		}
		break;
		case LIGHT_MODE_FX_PULSING_II:
		{
			struct db_fx_pulsing_2 pulsing;
			struct db_fx_pulsing_2  p_pulsing_model;
			data_center_read_light_data(date_center_get_light_mode(), &pulsing);   
			dest_crc32 = ui_calc_checksum((uint8_t*)&pulsing,  sizeof(struct db_fx_pulsing_2)); 
			memcpy(&p_pulsing_model, &g_tUIAllData.pulsing2_model, sizeof(struct db_fx_pulsing_2));
			src_crc32 = ui_calc_checksum((uint8_t*)&g_tUIAllData.pulsing2_model, sizeof(struct db_fx_pulsing_2));            
			if (dest_crc32 != src_crc32)
			{
				data_center_write_light_data(LIGHT_MODE_FX_PULSING_II, &p_pulsing_model);
				memcpy(p_gel_setting_model,&g_tUIAllData.pulsing2_model.mode_arg,sizeof(g_tUIAllData.pulsing2_model.mode_arg));
			}
		}
		break;
		
		case LIGHT_MODE_FX_WELDING_II:
		{
			struct db_fx_welding_2 welding;
			struct db_fx_welding_2  p_welding_model;
			data_center_read_light_data(date_center_get_light_mode(), &welding);   
			dest_crc32 = ui_calc_checksum((uint8_t*)&welding,  sizeof(struct db_fx_welding_2)); 
			memcpy(&p_welding_model, &g_tUIAllData.welding2_model, sizeof(struct db_fx_welding_2));
			src_crc32 = ui_calc_checksum((uint8_t*)&g_tUIAllData.welding2_model, sizeof(struct db_fx_welding_2));            
			if (dest_crc32 != src_crc32)
			{
				data_center_write_light_data(LIGHT_MODE_FX_WELDING_II, &p_welding_model);
				memcpy(p_gel_setting_model,&g_tUIAllData.welding2_model.mode_arg,sizeof(g_tUIAllData.welding2_model.mode_arg)); 
			}
		}
		break;
		
	}
  
   #endif
}

static void page_gel_time_upd_cb(uint32_t ms)
{
	page_gel_settiing_storage();
		
	page_invalid_type_t type = page_get_invalid_type();
	if(type!=PAGE_INVALID_TYPE_PORTION) return;	
	for(uint8_t i = 0; i < 2; i++)
	{
		lv_obj_t* child1 = lv_obj_get_child(btn_list[1].obj,i);
		lv_obj_clear_state(child1, LV_STATE_FOCUSED);
		lv_obj_t* child2 = lv_obj_get_child(btn_list[2].obj,i);
		lv_obj_clear_state(child2, LV_STATE_FOCUSED);
	}
	page_set_invalid_type(PAGE_INVALID_TYPE_IDLE);
	uint16_t cct_type = g_tUIAllData.pulsing2_model.mode_arg.gel.cct == 3200 ? 0 : 1;		
	if (cct_btn.sel_index != cct_type)
	{
		cct_btn.sel_index = cct_type;
		for (uint8_t i = 0; i < 2; i++)
		{
			lv_obj_t* child = lv_obj_get_child(btn_list[1].obj,i);
			if (cct_type == i)
			{   
				lv_obj_clear_state(child, LV_STATE_CHECKED);
				lv_obj_add_state(child, LV_STATE_FOCUSED);            
			}
			else
			{
				lv_obj_clear_state(child, LV_STATE_CHECKED);
				lv_obj_clear_state(child, LV_STATE_FOCUSED);            
			}
		}
	}
 
	 if (brand_btn.sel_index != gel_model_get_brand(&(g_tUIAllData.pulsing2_model.mode_arg.gel)) ) 
	{
		brand_btn.sel_index = gel_model_get_brand(&(g_tUIAllData.pulsing2_model.mode_arg.gel));
		serial_cont_dsc.cur_index = gel_model_get_series(&(g_tUIAllData.pulsing2_model.mode_arg.gel));
		gel_table_dsc.cur_index = gel_model_get_index(&(g_tUIAllData.pulsing2_model.mode_arg.gel));
		for (uint8_t i = 0; i < 2; i++)
		{
			lv_obj_t* child = lv_obj_get_child(btn_list[2].obj,i);
			if (brand_btn.sel_index == i)
			{   
				lv_obj_clear_state(child, LV_STATE_CHECKED);
				lv_obj_add_state(child, LV_STATE_FOCUSED);            
			}
			else
			{
				lv_obj_clear_state(child, LV_STATE_CHECKED);
				lv_obj_clear_state(child, LV_STATE_FOCUSED);            
			}
		}
		
		lv_obj_clean(serials_cont);
		serials_cont_init(serials_cont, &(g_tUIAllData.pulsing2_model.mode_arg.gel));
		gel_cont_table_fliping_refresh(gel_table_cont, &(g_tUIAllData.pulsing2_model.mode_arg.gel), gel_table_dsc.cur_index);        
	}
	else if (serial_cont_dsc.cur_index != gel_model_get_series(&(g_tUIAllData.pulsing2_model.mode_arg.gel))) //??????
	{
		serial_cont_dsc.cur_index = gel_model_get_series(&(g_tUIAllData.pulsing2_model.mode_arg.gel));
		gel_serials_cont_refresh(&serial_cont_dsc);
		gel_cont_table_fliping_refresh(gel_table_cont, &(g_tUIAllData.pulsing2_model.mode_arg.gel), gel_table_dsc.cur_index);                
	}
	else if (gel_table_dsc.cur_index != gel_model_get_index(&(g_tUIAllData.pulsing2_model.mode_arg.gel))) //??????
	{
		gel_table_dsc.cur_index = gel_model_get_index(&(g_tUIAllData.pulsing2_model.mode_arg.gel));
		gel_cont_table_fliping_refresh(gel_table_cont, &(g_tUIAllData.pulsing2_model.mode_arg.gel), gel_table_dsc.cur_index);        
	}

}
