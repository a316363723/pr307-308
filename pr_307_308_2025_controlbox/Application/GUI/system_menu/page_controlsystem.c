/*********************
 *      INCLUDES
 *********************/
#include "../ui_common.h"
#include "../page_widget.h"
#include "user.h"
/*********************
 *      DEFINES
 *********************/

#define LUMENRADIO_PAIR_ANIM_PERIOD                     (150 * 3)
#define LUMENRADIO_PAIR_TIMEOUT                         (7 * 1000)
#define BLE_RESET_TIMEOUT                               (6000)
/*********************
 *      TPEDEFS
 *********************/
typedef struct {
    lv_obj_t* scroll_cont;
	uint8_t last_sub_pid;
}ctr_view_t;
/**********************
 *  STATIC PROTOTYPES
 **********************/
static void page_ctrl_sys_construct(void);
static void page_ctrl_sys_destruct(void);
static void ctrl_sys_list_event_cb(lv_event_t* e, uint8_t index);
static void switch_btn_event_cb(lv_event_t* e);
static void ble_hidden_setting_refresh(void);
static void ble_reset_btn_event(lv_event_t* e);
static void ble_reset_del_event(my_msgbox_ext_t* ext);
static void ctrl_sys_comm_event(lv_event_t* e);
static void refresh_ble_resetting(void);
static void lumenradio_tick_event_cb(lv_event_t* e);
static void ethernet_set_list_event_cb(lv_event_t* e, uint8_t index);
static void ethernet_data_state_set_event_cb(lv_event_t* e, uint8_t index);
static void ethernet_artnet_universe_set_event_cb(lv_event_t* e);
static void ethernet_sacn_universe_set_event_cb(lv_event_t* e);
static void ethernet_artnet_net_event_cb(lv_event_t* e);
static void ethernet_state_set_event_cb(lv_event_t* e, uint8_t index);

static void lumenradio_switch_btn_event_cb(lv_event_t* e);
static void lumenradio_hidden_setting_refresh(void);
static void lumenradio_pair_btn_event(lv_event_t* e);
static void lumenradio_pair_animation(void *, int32_t);
static void lumenradio_unpair_confirm_event(my_msgbox_ext_t* ext);
static void lumenradio_status_init(void);
static void refresh_pair_round(lv_obj_t* cont, uint8_t index);
static void ethernet_uinverse_set_refresh(void);
static void ethernet_uinverse_set_init(void);
static void ethernet_uinverse_state_set_init(void);
static void ethernet_artnet_uinverse_set_init(void);
static void ethernet_sacn_uinverse_set_init(void);
static void ethernet_artnet_net_set_init(void);
static void ethernet_state_set_init(void);

static void ethernet_list_event_cb(lv_event_t* e, uint8_t index);
static void ethernet_ip_btn_user_cb(lv_event_t* e);

static void ctrl_list_panel_init(void);
static void ble_setting_panel_init(void);
static void lumenradio_setting_panel_init(void);
static void ethernet_setting_panel_init(void);
static void ble_reset_confirm_init(void);
static void ble_resetting_init(void);
static void ble_reset_result_init(void);
static void lumenradio_pairinig_init(void);
static void lumenradio_pair_result_init(void);
static void lumenradio_unpair_confirm_init(void);
static void lumenradio_tick_init(void);
static void ethernet_manual_set_ip_init(void);
static void refresh_title_comm_icon1(void);
static lv_obj_t* title_comm_icon_cont_create1(lv_obj_t *label);
//static void ethernet_uinverse_refresh(uint16_t universe);
static void ethernet_artnet_net_refresh(uint8_t count, uint8_t mode);

static void page_ctrl_sys_time_upd_cb(uint32_t ms);
static void ctrl_sys_load_sub_page(void);

static void local_data_pull(uint8_t spid);
static void page_ctrl_sys_event_cb(int event);
/*********************
 *  STATIC VATIABLES
 *********************/
static lv_group_t *indev_group;
static sys_config_t* p_sys_menu_model = NULL;
static my_list_ext_t list_ext;
static my_list_ext_t list_ext2;
static my_list_ext_t list_ext3;
static my_list_ext_t list_ext4;
static lv_obj_t* com_setting_list = NULL;
static int32_t disp_time;
static int32_t life_period;
static ip_btn_ext_t ip_btn_ext_list[4];
static lv_obj_t* crmx_con_img = NULL;
static lv_obj_t* crmx_con_label = NULL;
static uint8_t eth_spid_index[2] = {0, 0};
static ctr_view_t s_ctr_view;
static uint8_t s_art_net_num[5];
static uint8_t s_sacn_num[5];
/*********************
 *  GLOBAL VATIABLES
 *********************/
page_t page_ctrl_sys = PAGE_INITIALIZER("Control System",
                                PAGE_CONTROL_SYSTEM, 
                                page_ctrl_sys_construct, 
                                page_ctrl_sys_destruct, 
                                page_ctrl_sys_time_upd_cb,
                                page_ctrl_sys_event_cb,                                
                                PAGE_MENU);
/*********************
 *  GLOBAL FUNCTIONS
 *********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/                                  

static void local_data_pull(uint8_t spid)
{
	ui_set_eth_state(p_sys_menu_model->eth_start);
    switch (spid)
    {
        case SUB_ID_BLE_SETTING:
        {
            data_center_read_config_data(SYS_CONFIG_BLE_CTRL, &p_sys_menu_model->ble_on);
            data_center_read_config_data(SYS_CONFIG_BLE_SN, p_sys_menu_model->ble_sn_number);            
        }
        break;
        case SUB_ID_LUMENRADIO_SETTING:
        {
            data_center_read_config_data(SYS_CONFIG_CRMX_CTRL, &p_sys_menu_model->crmx_on);                       
        }
        break;
        case SUB_ID_ETHERNET_MANUAL_SET_IP:
        {
            data_center_read_config_data(SYS_CONFIG_NETMASK, p_sys_menu_model->netmask);
            data_center_read_config_data(SYS_CONFIG_LOCAL_IP, p_sys_menu_model->local_ip);
            data_center_read_config_data(SYS_CONFIG_GATEWAY, p_sys_menu_model->gateway);  
			data_center_read_config_data(SYS_CONFIG_ETH_START, &p_sys_menu_model->eth_start);
			ui_set_eth_state(p_sys_menu_model->eth_start);
        }
        break;
		case SUB_ID_ETHERNET_UINVERSE_SET_IP:
        {
            data_center_read_config_data(SYS_CONFIG_ETH_STATE_SET, &p_sys_menu_model->eth_state_set);
            data_center_read_config_data(SYS_CONFIG_ETH_ARTNET_UNIVERSE, &p_sys_menu_model->eth_artnet_universe);
            data_center_read_config_data(SYS_CONFIG_ETH_SACN_UNIVERSE, &p_sys_menu_model->eth_sacn_universe);  
			ui_set_eth_universe_state(p_sys_menu_model->eth_state_set);	
			ui_set_eth_artnet_universe(p_sys_menu_model->eth_artnet_universe);
			ui_set_eth_sacn_universe(p_sys_menu_model->eth_sacn_universe);
        }
        break;
        default:break;
    }
}

static void local_data_write(uint8_t spid)
{
    switch (spid)
    {
        case SUB_ID_BLE_SETTING:
        {
            data_center_write_config_data(SYS_CONFIG_BLE_CTRL, &p_sys_menu_model->ble_on);
            data_center_write_config_data(SYS_CONFIG_BLE_SN, p_sys_menu_model->ble_sn_number);
        }
        break;
        case SUB_ID_LUMENRADIO_SETTING:
        {
            data_center_write_config_data(SYS_CONFIG_CRMX_CTRL, &p_sys_menu_model->crmx_on);            
        }
        break;
        case SUB_ID_ETHERNET_MANUAL_SET_IP:
        {
            data_center_write_config_data(SYS_CONFIG_NETMASK, p_sys_menu_model->netmask);
            data_center_write_config_data(SYS_CONFIG_LOCAL_IP, p_sys_menu_model->local_ip);
            data_center_write_config_data(SYS_CONFIG_GATEWAY, p_sys_menu_model->gateway);
        }
        break;
        default:break;
    }
}


static void page_ctrl_sys_event_cb(int event)
{
    uint8_t spid = screen_get_act_spid();        
    switch (event)
    {
        case EVENT_DATA_PULL:
        {
            local_data_pull(spid);
        }
        break;
        case EVENT_DATA_WRITE:
        {
            local_data_write(spid);
        }
        break;
        default:break;
    }
}

static void page_ctrl_sys_construct(void)
{
    uint8_t spid = screen_get_act_spid();
    life_period = -1;
    disp_time = 0;

    indev_group = lv_group_create();
    p_sys_menu_model = screen_get_act_page_data();
    
    my_indev_set_group(indev_group);
    lv_group_set_default(indev_group);
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    page_event_send(EVENT_DATA_PULL);

    switch(spid)
    {
        case SUB_ID_CTRL_SYS_LIST:                 ctrl_list_panel_init(); break;
        case SUB_ID_BLE_SETTING:                   ble_setting_panel_init();  break;
        case SUB_ID_BLE_RESET_CONFIRM:             ble_reset_confirm_init();break;
        case SUB_ID_BLE_RESETING:                  ble_resetting_init(); break;
        case SUB_ID_BLE_RESET_FAILED:              
        case SUB_ID_BLE_RESET_OK:                  ble_reset_result_init(); break;
        case SUB_ID_LUMENRADIO_SETTING:            lumenradio_setting_panel_init(); break;
        case SUB_ID_LUMENRADIO_PAIR_FAILED:        
        case SUB_ID_LUMENRADIO_PAIR_SUCCEED:       lumenradio_pair_result_init(); break;        
        case SUB_ID_LUMENRADIO_PAIRING:            lumenradio_pairinig_init(); break;
        case SUB_ID_LUMENRADIO_UNPAIR_CONFIRM:     lumenradio_unpair_confirm_init(); break;
		case SUB_ID_LUMENRADIO_UNLINK_SUCCEED:     
		case SUB_ID_LUMENRADIO_LINK_SUCCEED:       
		case SUB_ID_LUMENRADIO_LINK_FAILED:        lumenradio_status_init();break;
		case SUB_ID_LUMENRADIO_LINK_TICK:	       lumenradio_tick_init(); break;
        case SUB_ID_ETHERNET_SETTING:              ethernet_setting_panel_init(); break;
        case SUB_ID_ETHERNET_MANUAL_SET_IP:        ethernet_manual_set_ip_init(); break;
		case SUB_ID_ETHERNET_SATRT_SET_IP:         ethernet_state_set_init();break;
		case SUB_ID_ETHERNET_UINVERSE_SET_IP:      ethernet_uinverse_set_init();  break;
		case SUB_ID_ETHERNET_UINVERSE_STATE_SET:   ethernet_uinverse_state_set_init(); break;
		case SUB_ID_ETHERNET_ARTNET_UINVERSE_SET:  ethernet_artnet_uinverse_set_init(); break;
		case SUB_ID_ETHERNET_ARTNET_NET_SET:       ethernet_artnet_net_set_init(); break;
		case SUB_ID_ETHERNET_SACN_UINVERSE_SET:    ethernet_sacn_uinverse_set_init(); break;
        default:break;
    }
}

/**
 * @brief 根据当前ID返回上一级
 * 
 */
static void ctrl_sys_load_sub_page(void)
{
    uint8_t def_spid = 0, def_index = 0;
    uint8_t subid = screen_get_act_spid();
    
    switch(subid)
    {
		case SUB_ID_LUMENRADIO_SETTING:
		case SUB_ID_BLE_SETTING:
		case SUB_ID_ETHERNET_SETTING:
			def_spid = SUB_ID_CTRL_SYS_LIST; def_index = 1;  
			screen_load_prev_sub_page(def_spid, def_index);
		break;	
		case SUB_ID_BLE_RESET_CONFIRM:
			def_spid = SUB_ID_BLE_SETTING; def_index = 1;  
			screen_load_prev_sub_page(def_spid, def_index);
		break;
        case SUB_ID_BLE_RESETING:
        case SUB_ID_BLE_RESET_FAILED:
        case SUB_ID_BLE_RESET_OK:            
			def_spid = SUB_ID_BLE_SETTING; def_index = 1;  
			screen_load_prev_sub_page(def_spid, def_index);
		break;     	
        case SUB_ID_LUMENRADIO_PAIRING:
			def_spid = SUB_ID_LUMENRADIO_SETTING; def_index = 0;  
			screen_load_prev_sub_page(def_spid, def_index);
		break;
        case SUB_ID_LUMENRADIO_PAIR_FAILED:
        case SUB_ID_LUMENRADIO_UNPAIR_CONFIRM:
        case SUB_ID_LUMENRADIO_PAIR_SUCCEED: 
		case SUB_ID_LUMENRADIO_LINK_SUCCEED:
	    case SUB_ID_LUMENRADIO_LINK_FAILED:
			user_enter_dmx();
//			ui_set_dmx_state(1);
			def_index = 1; 
		break;   
		case SUB_ID_LUMENRADIO_UNLINK_SUCCEED:  
			def_spid = SUB_ID_LUMENRADIO_SETTING; def_index = 1;  
			screen_load_prev_sub_page(def_spid, def_index);
		break;	
		case SUB_ID_ETHERNET_MANUAL_SET_IP:
		case SUB_ID_ETHERNET_SATRT_SET_IP:
		case SUB_ID_ETHERNET_UINVERSE_SET_IP:
		case SUB_ID_ETHERNET_UINVERSE_STATE_SET:
		case SUB_ID_ETHERNET_ARTNET_UINVERSE_SET:
	    case SUB_ID_ETHERNET_ARTNET_NET_SET:
	    case SUB_ID_ETHERNET_SACN_UINVERSE_SET:
			screen_turn_prev_page();	
        default: break;
    }

    
}

static void page_ctrl_sys_destruct(void)
{
    lv_group_del(indev_group);
    lv_obj_clean(lv_scr_act());
    indev_group = NULL;

    page_event_send(EVENT_DATA_WRITE);
	memset(&s_ctr_view, 0, sizeof(s_ctr_view));
}

static void page_ctrl_sys_time_upd_cb(uint32_t ms)
{
    uint8_t spid = screen_get_act_spid();
	uint8_t crmx_on = 0;
	
    ble_status_t * p_ble_status = &g_ptUIStateData->ble_status;
    page_invalid_type_t page = page_get_invalid_type();
    bool redraw = false;
    
    if (page != PAGE_INVALID_TYPE_IDLE)
    {        
        page_set_invalid_type(PAGE_INVALID_TYPE_IDLE);        
        page_event_send(EVENT_DATA_WRITE);
        redraw = true;
    }

    switch (spid)
    {
		case SUB_ID_LUMENRADIO_SETTING:
		{
			refresh_title_comm_icon1();
			
			lv_obj_t* list = lv_obj_get_child(lv_scr_act(), 1);
			lv_obj_t* btn = lv_obj_get_child(list, 1);
			lv_obj_t* btn1 = lv_obj_get_child(list, 0);
			
			data_center_read_config_data(SYS_CONFIG_CRMX_CTRL, &crmx_on);
			if(crmx_is_paired() == 0 || crmx_on == 0)
			{
				lv_obj_clear_state(btn, LV_STATE_FOCUSED);
				lv_event_send(btn, LV_EVENT_DEFOCUSED, NULL);
				list_ext.cur_sel = 0;
				lv_obj_add_state(btn1, LV_STATE_FOCUSED);
				lv_event_send(btn1, LV_EVENT_FOCUSED, NULL);
				lv_obj_add_flag(btn, LV_OBJ_FLAG_HIDDEN);
			}
			else 
			{
				lv_obj_clear_flag(btn, LV_OBJ_FLAG_HIDDEN);
			}
		}
		break;
		case SUB_ID_BLE_RESETING:
		{
			p_ble_status->timecnt += 200;
			if (p_ble_status->timecnt > BLE_RESET_TIMEOUT) {
				ui_enter_critical();
				p_ble_status->percent = 99;                                
				ui_exit_critical();
				screen_load_sub_page(SUB_ID_BLE_RESET_FAILED, 0);                
				break;
			}

			if (p_ble_status->percent < 100)
			{
				p_ble_status->percent += 6;
				if (p_ble_status->percent >= 99) 
					p_ble_status->percent = 99;
			}                    

			if (p_ble_status->percent == 99)
			{
				if (ui_ble_get_reset_status() == BLE_RESET_SUCCEED) {
					screen_load_sub_page(SUB_ID_BLE_RESET_OK, 0);
				}
				else if (ui_ble_get_reset_status() == BLE_RESET_FAILED) {
				   screen_load_sub_page(SUB_ID_BLE_RESET_FAILED, 0);
				}
				break;
			}            
		}    
		break; 
		case SUB_ID_BLE_RESET_FAILED:
		case SUB_ID_BLE_RESET_OK:
		case SUB_ID_LUMENRADIO_PAIR_FAILED:
		case SUB_ID_LUMENRADIO_PAIR_SUCCEED:  
		case SUB_ID_LUMENRADIO_UNLINK_SUCCEED:
		case SUB_ID_LUMENRADIO_LINK_SUCCEED:
		case SUB_ID_LUMENRADIO_LINK_FAILED:
		{
			if (life_period != -1)
			{
				disp_time += 200;
				if (disp_time >= life_period)
				{   
					ctrl_sys_load_sub_page();
				}
			}
		}
		break;
		case SUB_ID_LUMENRADIO_PAIRING:
		{            
			//等待
//			lumenradio_status_t *p_lumen_status = &g_ptUIStateData->lumen_status;

			disp_time += 200;
			if (disp_time > life_period || crmx_is_paired() == 2)
			{
				screen_load_sub_page(SUB_ID_LUMENRADIO_PAIR_SUCCEED, 0);
			}
		}
		break;
		case SUB_ID_ETHERNET_MANUAL_SET_IP:
		{
			if(ETH_DHCP_START == ui_get_eth_state())
			{
				ui_get_dhcp_ip_addr(ip_btn_ext_list[1].buff);
				ui_get_dhcp_netmask_addr(ip_btn_ext_list[2].buff);
				ui_get_dhcp_gateway_addr(ip_btn_ext_list[3].buff);
				redraw = true;
			}
			if (redraw)
			{
				/* 以太网 IP地址 */
				lv_event_send(ip_btn_ext_list[1].obj, LV_EVENT_VALUE_CHANGED, NULL);
				/* 子网掩码 */
				lv_event_send(ip_btn_ext_list[2].obj, LV_EVENT_VALUE_CHANGED, NULL);
				/* 网关 */
				lv_event_send(ip_btn_ext_list[3].obj, LV_EVENT_VALUE_CHANGED, NULL);                
			}
			
		}
		break;
		default:break;
    }

    
}

static void ctrl_list_panel_init(void)
{
    lv_obj_t* title = lv_label_create(lv_scr_act());
    
    lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_label_set_text(title, Lang_GetStringByID(STRING_ID_CTRL_SYSTEM));
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);   

    list_ext.cur_sel = ui_get_ctr_spid_index(); 
	if(list_ext.cur_sel > 3)
		list_ext.cur_sel = 0;
//	if(ui_get_ctr_spid_index() == 1)
//		list_ext.cur_sel+=1;	
//	else if(ui_get_ctr_spid_index() == 2)
//		list_ext.cur_sel=2;
    list_ext.release_cb = ctrl_sys_list_event_cb;
    lv_obj_t* list = my_list_create(lv_scr_act(), lv_pct(85), lv_pct(80), &list_ext);   
    lv_obj_set_flex_align(list, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 38);

    LV_IMG_DECLARE(ImgDmxMode)
    LV_IMG_DECLARE(ImgBle);
    LV_IMG_DECLARE(ImgWifi)
    LV_IMG_DECLARE(ImgLan);

    my_list_add_btn2(list, Lang_GetStringByID(STRING_ID_DMX_SETTING),  &ImgDmxMode, 0);
    my_list_add_btn2(list, Lang_GetStringByID(STRING_ID_BLE),          &ImgBle    , 1);
    my_list_add_btn2(list, Lang_GetStringByID(STRING_ID_LUMEN),        &ImgWifi   , 1);
    my_list_add_btn2(list, Lang_GetStringByID(STRING_ID_LAN),          &ImgLan    , 1);

//	uint8_t act_index = screen_get_act_index();
//	
//	if(act_index != 0)
//		act_index+=1;
//	screen_set_act_index(act_index);
//	lv_obj_t* obj = lv_obj_get_child(lv_scr_act(), act_index);
    lv_group_add_obj(indev_group, list);
//	lv_obj_add_state(obj, LV_STATE_FOCUSED);
//	lv_event_send(obj, LV_EVENT_FOCUSED, NULL);
//	lv_group_focus_obj(obj);
    lv_group_focus_obj(list);      
    lv_group_set_editing(indev_group, true);                
}

static void ble_setting_panel_init(void)
{
    ble_status_t* p_ble_status = &g_ptUIStateData->ble_status;
    lv_obj_t* title = lv_label_create(lv_scr_act());    

    (void)p_ble_status;
    lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_label_set_text(title, Lang_GetStringByID(STRING_ID_BLE));
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP); 

    com_setting_list = my_list_create(lv_scr_act(), lv_pct(83), lv_pct(70), &list_ext);    
    lv_obj_set_flex_align(com_setting_list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);  
    lv_obj_align(com_setting_list, LV_ALIGN_TOP_MID, 0, 53);
    
    
    lv_obj_t* switch_btn = my_list_add_switch_btn(com_setting_list, Lang_GetStringByID(STRING_ID_BLE_STATUS), p_sys_menu_model->ble_on);
    lv_obj_add_event_cb(switch_btn, switch_btn_event_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, switch_btn);
    
    lv_obj_t* btn = my_list_add_btn(com_setting_list, Lang_GetStringByID(STRING_ID_BLE_RESET), 10);
    lv_obj_add_event_cb(btn, ble_reset_btn_event, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, btn);            

    lv_obj_t* ble_sn_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(ble_sn_label, Font_ResouceGet(FONT_14), 0);
    lv_obj_set_style_text_color(ble_sn_label, lv_color_white(), 0);

#if UI_DUMMY_DATA_TEST    
    p_sys_menu_model->ble_sn_number[0] = 'A';
    p_sys_menu_model->ble_sn_number[1] = 'd';
    p_sys_menu_model->ble_sn_number[2] = 'A';
    p_sys_menu_model->ble_sn_number[3] = '5';
    p_sys_menu_model->ble_sn_number[4] = '2';
    p_sys_menu_model->ble_sn_number[5] = '3';
#endif    
	
    p_sys_menu_model->ble_sn_number[6] = '\0';
	for (uint8_t ui = 0; ui < 6; ui++)
	{
		if(p_sys_menu_model->ble_sn_number[ui] >= 97 &&  p_sys_menu_model->ble_sn_number[ui] <= 122 )//×ª´óÐ´
		{
			p_sys_menu_model->ble_sn_number[ui] = p_sys_menu_model->ble_sn_number[ui] - 0x20 ;
		}
		else
		{
			p_sys_menu_model->ble_sn_number[ui] = p_sys_menu_model->ble_sn_number[ui] - 0 ; 
		}
	}
    lv_snprintf(sdb, sizeof(sdb), "%s : %s", Lang_GetStringByID(STRING_ID_BLE_SN), p_sys_menu_model->ble_sn_number);
    lv_label_set_text(ble_sn_label, sdb);
    lv_obj_align(ble_sn_label, LV_ALIGN_BOTTOM_MID, 0, -45);
    lv_obj_invalidate(ble_sn_label);

    ble_hidden_setting_refresh();

    bool ble_on = p_sys_menu_model->ble_on;
    uint8_t act_index = screen_get_act_index();
    lv_obj_t* focused_obj = NULL;

    if (!ble_on) {
        act_index = 0;
    }
    
    focused_obj = lv_obj_get_child(com_setting_list, act_index);
    if (focused_obj)
    {
        lv_group_focus_obj(focused_obj);
    }
    else
    {
        lv_group_focus_obj(switch_btn);
    }

    lv_group_set_wrap(indev_group, false);
    lv_group_set_editing(indev_group, false);    
}

static void ble_reset_confirm_init(void)
{
    ble_status_t* p_ble_status = &g_ptUIStateData->ble_status;
    p_ble_status->percent = 0;
    p_ble_status->timecnt = 0; 

    my_msgbox_reset();

    my_msgbox_ext_t* p_msgbox_ext = my_msgbox_get_ext();
    LV_IMG_DECLARE(ImgBle_B);
    p_msgbox_ext->body_comment_str = NULL;
    p_msgbox_ext->body_img = &ImgBle_B;
    p_msgbox_ext->timecnt = 0;
    p_msgbox_ext->timeout = 30 * 1000;
    p_msgbox_ext->answer = MSGBOX_ID_NO_CONFIRM;
    p_msgbox_ext->type = MSGBOX_TYPE_TIMEOUT;
    p_msgbox_ext->btn_sel = MSGBOX_ID_NO;
    p_msgbox_ext->body_comment_str = NULL;
    p_msgbox_ext->title_str = Lang_GetStringByID(STRING_ID_BLE_RESET);
    p_msgbox_ext->msgbox_del_cb = ble_reset_del_event;
    my_msgbox_create(p_msgbox_ext);
}

/**
 * @brief 刷新进度条函数
 * 
 * @param a 
 * @param val 
 */
static void bar_anim_exec_xcb(void *a, int32_t val)
{
    lv_obj_t* cont = lv_obj_get_parent(a);
    lv_obj_t* process_label = lv_obj_get_child(cont, 1);
    
    lv_bar_set_value(a, val, LV_ANIM_ON);
    lv_label_set_text_fmt(process_label, "%d%%", val);
    lv_obj_align(process_label, LV_ALIGN_TOP_RIGHT, 0, 0);    
}

static void ble_resetting_init(void)
{
    ble_status_t* p_ble_status = &g_ptUIStateData->ble_status;
    p_ble_status->percent = 0;
    p_ble_status->timecnt = 0;
        
    lv_obj_t* cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, lv_pct(77), lv_pct(22));
    lv_obj_set_style_text_color(cont, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_text_font(cont, Font_ResouceGet(FONT_18), 0);
    lv_obj_center(cont);

    lv_obj_t* ble_reset_bar = lv_bar_create(cont);
    lv_obj_set_style_bg_color(ble_reset_bar, lv_color_make(255, 255, 255), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(ble_reset_bar, lv_color_make(128, 128, 128), LV_PART_MAIN);
    lv_obj_set_style_outline_pad(ble_reset_bar, 0, 0);
    lv_obj_set_style_outline_width(ble_reset_bar, 0, 0);
    lv_obj_set_size(ble_reset_bar, lv_pct(84), lv_pct(32));                
    lv_obj_align(ble_reset_bar, LV_ALIGN_TOP_LEFT, 0, 0);    
    lv_bar_set_range(ble_reset_bar, 0, 100);
    //lv_obj_add_event_cb(ble_reset_bar, ctrl_sys_comm_event, LV_EVENT_ALL, NULL);        
    

    lv_obj_t* process_label = lv_label_create(cont);
    lv_obj_t* status_label = lv_label_create(cont);
    lv_label_set_text(status_label, Lang_GetStringByID(STRING_ID_BLE_RESETING));            
    lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, 0);         

    refresh_ble_resetting();

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, ble_reset_bar);
    lv_anim_set_exec_cb(&a, bar_anim_exec_xcb);
    lv_anim_set_time(&a, BLE_RESET_TIMEOUT);
    lv_anim_set_repeat_count(&a, 1);
    lv_anim_set_values(&a, 0, 99);
    lv_anim_start(&a);

    lv_group_add_obj(indev_group, ble_reset_bar);
    lv_group_focus_obj(ble_reset_bar);    
    lv_group_set_editing(indev_group, true);
}

static void ble_reset_result_init(void)
{
    uint8_t spid = screen_get_act_spid();   
    lv_obj_t* panel;

    disp_time = 0;
    if (spid == SUB_ID_BLE_RESET_FAILED)
    {
        LV_IMG_DECLARE(ImgConfirmFailed)
        life_period = 5 * 1000;
        panel = warning_info_init(lv_scr_act(), &ImgConfirmFailed, 54, 25,Lang_GetStringByID(STRING_ID_BLE_RESET_FAILED));
    }
    else
    {
        LV_IMG_DECLARE(ImgConfirmOk)
        life_period = 2 * 1000;
        panel = warning_info_init(lv_scr_act(), &ImgConfirmOk, 54, 25,Lang_GetStringByID(STRING_ID_BLE_RESET_OK));            
    }
    lv_obj_add_event_cb(panel, ctrl_sys_comm_event, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, panel);
}

static lv_obj_t* title_comm_icon_cont_create1(lv_obj_t *label)
{
    LV_IMG_DECLARE(ImgWifi)
    
	lv_obj_t *boj = lv_obj_create(lv_scr_act()); 
	lv_obj_remove_style_all(boj);
    lv_obj_set_size(boj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(boj, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_gap(boj, 7, 0);
    lv_obj_set_flex_align(boj, LV_FLEX_ALIGN_CENTER,LV_FLEX_ALIGN_CENTER,LV_FLEX_ALIGN_CENTER);
	lv_obj_align(boj, LV_ALIGN_TOP_RIGHT, -10, SYS_MENU_TITLE_TO_TOP);
	
//	lv_obj_t* label1 = lv_label_create(boj);  
//	lv_obj_set_style_text_font(label1, Font_ResouceGet(FONT_14), 0);
//	lv_obj_set_style_text_color(label1, lv_color_white(), 0);
//	lv_label_set_text(label1, Lang_GetStringByID(STRING_ID_CRMX_NO_CONNECT));
	
    lv_obj_t* crmx = lv_img_create(boj);
//    lv_obj_set_style_img_recolor_opa(crmx, LV_OPA_100, 0);
//    lv_obj_set_style_img_recolor(crmx, lv_color_white(), 0);     
    lv_img_set_src(crmx, &ImgWifi);
	
    lv_obj_add_flag(crmx, LV_OBJ_FLAG_HIDDEN);

    return boj;
}

static void refresh_title_comm_icon1(void)
{
//	static uint8_t dmx_icon_count;
	uint8_t curre_mode = 0;
    /* 标题栏 */
    data_center_read_config_data(SYS_CONFIG_CRMX_CTRL, &curre_mode);
    ui_set_crmx_state(curre_mode);
	
//	lv_obj_t* label = lv_obj_get_child(crmx_con_img, 0);
    lv_obj_t* crmx = lv_obj_get_child(crmx_con_img, 0);
	
    if (p_sys_menu_model->crmx_on)
    {
        lv_img_set_src(crmx, title_get_comm_img(COM_TYPE_WIFI));
		if(crmx_is_paired() == 0)
		{
//			lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(crmx, LV_OBJ_FLAG_HIDDEN);
		}
		else if(crmx_is_paired() == 1)
		{
			lv_obj_clear_flag(crmx, LV_OBJ_FLAG_HIDDEN);
			
//			if(dmx_icon_count == 1)
//			{
//				dmx_icon_count = 0;
//				lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
//				lv_label_set_text(label, Lang_GetStringByID(STRING_ID_CRMX_NO_CONNECT));
//			}
//			else
//			{
//				dmx_icon_count = 1;
//				lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
//			}
		}
		else if(crmx_is_paired() == 2)
		{
			lv_obj_clear_flag(crmx, LV_OBJ_FLAG_HIDDEN);
//			if(dmx_icon_count++ >= 1)
//			{
//				dmx_icon_count = 0;
//				lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
//				lv_label_set_text(label, Lang_GetStringByID(STRING_ID_CRMX_NO_DATA));
//			}
//			else
//			{
//				lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
//			}
		}
		else if(crmx_is_paired() == 3)
		{
			lv_obj_clear_flag(crmx, LV_OBJ_FLAG_HIDDEN);
//			lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
		}
    }
    else  
    {
//		lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(crmx, LV_OBJ_FLAG_HIDDEN);
    }

}

static void lumenradio_setting_panel_init(void)
{
	uint8_t crmx_on = 0;
	
    lv_obj_t* title = lv_label_create(lv_scr_act());
    
    lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_label_set_text(title, Lang_GetStringByID(STRING_ID_LUMEN));
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP); 

    com_setting_list = my_list_create(lv_scr_act(), lv_pct(83), lv_pct(70), &list_ext);          
    lv_obj_align(com_setting_list, LV_ALIGN_TOP_MID, 0, 53);
    
    lv_obj_t* switch_btn = my_list_add_switch_btn(com_setting_list, Lang_GetStringByID(STRING_ID_LUMEN), p_sys_menu_model->crmx_on);
    lv_obj_set_flex_align(com_setting_list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);  
    lv_obj_add_event_cb(switch_btn, lumenradio_switch_btn_event_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, switch_btn);
    
    lv_obj_t* btn = my_list_add_btn(com_setting_list, Lang_GetStringByID(STRING_ID_CRMX_UNLINK), 10);
    lv_obj_add_event_cb(btn, lumenradio_pair_btn_event, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, btn);            
	
    lumenradio_hidden_setting_refresh();
	data_center_read_config_data(SYS_CONFIG_CRMX_CTRL, &crmx_on);
	if(crmx_is_paired() == 0 || crmx_on == 0)
	{
		lv_obj_add_flag(btn, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_state(btn, LV_STATE_FOCUSED);
		lv_event_send(btn, LV_EVENT_DEFOCUSED, NULL);
		list_ext.cur_sel = 0;
		lv_obj_add_state(switch_btn, LV_STATE_FOCUSED);
		lv_event_send(switch_btn, LV_EVENT_FOCUSED, NULL);
	}
	else 
	{
		lv_obj_clear_flag(btn, LV_OBJ_FLAG_HIDDEN);
	}
	crmx_con_img = title_comm_icon_cont_create1(crmx_con_label);
	refresh_title_comm_icon1();
	
    bool lumen_on = p_sys_menu_model->crmx_on;
    uint8_t act_index = screen_get_act_index();
//    lv_obj_t* focused_obj = NULL;
    
    if (!lumen_on) {
        act_index = 0;
    }    
//    focused_obj = lv_obj_get_child(com_setting_list, act_index);
//    if (focused_obj)
//    {
//        lv_group_focus_obj(focused_obj);
//    }
//    else
//    {
        lv_group_focus_obj(switch_btn);
//    }

    lv_group_set_wrap(indev_group, false);
    lv_group_set_editing(indev_group, false);     
}

static void lumenradio_pairinig_init(void)
{
    lumenradio_status_t *p_lumen_status = &g_ptUIStateData->lumen_status;

    life_period = LUMENRADIO_PAIR_TIMEOUT;
    disp_time = 0;

#if !UI_DUMMY_DATA_ENABLE
    if (p_lumen_status->status != LUMENRADIO_START_PAIR) {
        ui_enter_critical();
        p_lumen_status->status = LUMENRADIO_START_PAIR;
        p_lumen_status->force_upd = false;
        ui_exit_critical();
    }    
#endif

    lv_obj_t* cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, 41, 44);
    lv_obj_align(cont, LV_ALIGN_CENTER, 0, -28);
    lv_obj_add_event_cb(cont, ctrl_sys_comm_event, LV_EVENT_ALL, 0);

    lv_obj_t *obj;
    for (uint8_t i = 0; i < 3; i++)
    {
        obj = lv_obj_create(cont);
        lv_obj_add_style(obj, &style_rect_panel, 0);
        lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_size(obj, 19, 19);
    }

    obj = lv_obj_get_child(cont, 0);
    lv_obj_align(obj, LV_ALIGN_TOP_RIGHT, 0, 0);
    obj = lv_obj_get_child(cont, 1);
    lv_obj_align(obj, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    obj = lv_obj_get_child(cont, 2);
    lv_obj_align(obj, LV_ALIGN_LEFT_MID, 0, 0);
    
    lv_obj_t* status_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(status_label, Font_ResouceGet(FONT_18), 0);
    lv_obj_set_style_text_color(status_label, lv_color_white(), 0);
    lv_label_set_text(status_label, Lang_GetStringByID(STRING_ID_LINKING));
    lv_obj_align(status_label, LV_ALIGN_CENTER, 0, 15);
    
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, cont);
    lv_anim_set_values(&a, 0, 3);
    lv_anim_set_exec_cb(&a, lumenradio_pair_animation);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_time(&a, LUMENRADIO_PAIR_ANIM_PERIOD);
    lv_anim_set_playback_time(&a, 0);
    lv_anim_start(&a);

    lv_group_add_obj(indev_group, cont);
    lv_group_focus_obj(cont);
    lv_group_set_editing(indev_group, true); 
}

static void lumenradio_unpair_confirm_init(void)
{
    //LV_IMG_DECLARE(ImgConfirmFailed);
    LV_IMG_DECLARE(ImgCRMXReset);
    lumenradio_status_t *p_lumen_status = &g_ptUIStateData->lumen_status;    
    
    (void)p_lumen_status;
    //reset message box..
    my_msgbox_reset();
    
    my_msgbox_ext_t* p_msgbox_ext = my_msgbox_get_ext();
    p_msgbox_ext->body_comment_str = NULL;
    p_msgbox_ext->body_img = &ImgCRMXReset;
    p_msgbox_ext->timecnt = 0;
    p_msgbox_ext->timeout = 30 * 1000;
    p_msgbox_ext->answer = MSGBOX_ID_NO_CONFIRM;
    p_msgbox_ext->type = MSGBOX_TYPE_TIMEOUT;
    p_msgbox_ext->btn_sel = MSGBOX_ID_NO;
    p_msgbox_ext->body_comment_str = NULL;
    p_msgbox_ext->title_str = Lang_GetStringByID(STRING_ID_UNPAIR);
    p_msgbox_ext->msgbox_del_cb = lumenradio_unpair_confirm_event;
    my_msgbox_create(p_msgbox_ext);
}

static void lumenradio_pair_result_init(void)
{
    uint8_t spid = screen_get_act_spid();      
    lv_obj_t* panel = NULL;
    disp_time = 0;

    if (spid == SUB_ID_LUMENRADIO_PAIR_FAILED)
    {
        LV_IMG_DECLARE(ImgConfirmFailed)
        life_period = 1 * 1000;
        panel = warning_info_init(lv_scr_act(), &ImgConfirmFailed, 54, 25, Lang_GetStringByID(STRING_ID_LINK_FAIL));
    }
    else
    {
        LV_IMG_DECLARE(ImgConfirmOk)        
        life_period = 1 * 1000;
        panel = warning_info_init(lv_scr_act(), &ImgConfirmOk, 54, 25, Lang_GetStringByID(STRING_ID_LINK_OK));            
    }

    lv_obj_add_event_cb(panel, ctrl_sys_comm_event, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, panel);
    lv_group_focus_obj(panel);      
    lv_group_set_editing(indev_group, true);    
}

static void lumenradio_status_init(void)
{
    uint8_t spid = screen_get_act_spid();      
    lv_obj_t* panel = NULL;
    disp_time = 0;

    if (spid == SUB_ID_LUMENRADIO_LINK_FAILED)
    {
        LV_IMG_DECLARE(ImgConfirmFailed)
        life_period = 1 * 1000;
        panel = warning_info_init(lv_scr_act(), &ImgConfirmFailed, 54, 25, Lang_GetStringByID(STRING_ID_LINK_FAIL));
    }
    else if(spid == SUB_ID_LUMENRADIO_UNLINK_SUCCEED)
    {
        LV_IMG_DECLARE(ImgConfirmOk)        
        life_period = 1 * 1000;
        panel = warning_info_init(lv_scr_act(), &ImgConfirmOk, 54, 25, Lang_GetStringByID(STRING_ID_UNLINK_OK));            
    }
	else if(spid == STRING_ID_LINK_OK)
    {
        LV_IMG_DECLARE(ImgConfirmOk)        
        life_period = 1 * 1000;
        panel = warning_info_init(lv_scr_act(), &ImgConfirmOk, 54, 25, Lang_GetStringByID(STRING_ID_LINK_OK));            
    }
	

    lv_obj_add_event_cb(panel, ctrl_sys_comm_event, LV_EVENT_ALL, NULL);
    lv_group_add_obj(indev_group, panel);
    lv_group_focus_obj(panel);      
    lv_group_set_editing(indev_group, true);    
}

static void lumenradio_tick_init(void)
{
    lv_obj_t* title = lv_label_create(lv_scr_act());
    
    lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_18_HEAVR), 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
	lv_obj_set_width(title, 270);  /*Set smaller width to make the lines wrap*/
	lv_label_set_long_mode(title, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(title, Lang_GetStringByID(STRING_ID_CRMX_TICK));
    lv_obj_align(title, LV_ALIGN_CENTER, 0, -20); 
    
	lv_obj_t* yes_btn = lv_obj_create(lv_scr_act());
	lv_obj_add_style(yes_btn, &style_rect_panel, 0);
	lv_obj_add_style(yes_btn, &style_common_focued_btn, LV_STATE_FOCUSED);
	lv_obj_set_size(yes_btn, 75, 40);
	lv_obj_align_to(yes_btn, lv_scr_act(), LV_ALIGN_BOTTOM_MID, 0, -10);
	lv_obj_add_event_cb(yes_btn, lumenradio_tick_event_cb, LV_EVENT_ALL, NULL);
	lv_group_add_obj(indev_group, yes_btn);

	lv_obj_t* res_label = lv_label_create(yes_btn);
	lv_label_set_text(res_label, Lang_GetStringByID(STRING_ID_BIG_YES));
	lv_obj_set_style_text_font(res_label, Font_ResouceGet(FONT_22), 0);
	lv_obj_set_style_text_color(res_label, lv_color_white(), 0);
	lv_obj_center(res_label);
	
    lv_group_focus_obj(yes_btn);
    lv_group_set_wrap(indev_group, false);
    lv_group_set_editing(indev_group, false);     
}

static void ethernet_setting_panel_init(void)
{
    lv_obj_t* title = lv_label_create(lv_scr_act());
    
    lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_label_set_text(title, Lang_GetStringByID(STRING_ID_LAN));
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);   

    list_ext.cur_sel = eth_spid_index[0];   
    list_ext.release_cb = ethernet_list_event_cb;
    lv_obj_t* list = my_list_create(lv_scr_act(), 265, lv_pct(70), &list_ext);            
    lv_obj_set_flex_align(list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 53);

    my_list_add_btn(list, Lang_GetStringByID(STRING_ID_IP_SETTINGS), 10);
	my_list_add_btn(list, Lang_GetStringByID(STRING_ID_LAN_UINVER), 10);
	
    lv_group_add_obj(indev_group, list);
    lv_group_focus_obj(list);      
    lv_group_set_editing(indev_group, true);    
    lv_group_set_wrap(indev_group, false);                
}

static void ethernet_manual_set_ip_init(void)
{
    lv_obj_t* title = lv_label_create(lv_scr_act());
    
    lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_label_set_text(title, Lang_GetStringByID(STRING_ID_LAN));
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);   

    lv_obj_t* btn_cont = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(btn_cont);
    lv_obj_set_size(btn_cont, 265, lv_pct(60));
	lv_obj_add_flag(btn_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(btn_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_ver(btn_cont, 7, 0);
    lv_obj_set_style_pad_gap(btn_cont, 7, 0);
//	lv_obj_set_scrollbar_mode(btn_cont, LV_SCROLLBAR_MODE_OFF);
	lv_obj_align_to(btn_cont, title, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
	
	ui_get_mac_addr();
	
	lv_snprintf((char*)sdb, sizeof(sdb), "%s: %02x:%02x:%02x:%02x:%02x:%02x", Lang_GetStringByID(STRING_ID_MAC_ADDRESS), g_ptUIStateData->mac_address[0], g_ptUIStateData->mac_address[1],\
                                                                                g_ptUIStateData->mac_address[2], g_ptUIStateData->mac_address[3],\
                                                                                g_ptUIStateData->mac_address[4], g_ptUIStateData->mac_address[5]);
    
	if(p_sys_menu_model->eth_start == ETH_STATIC_START)
		ip_btn_ext_list[0].buff = (uint8_t *)Lang_GetStringByID(STRING_ID_IP_MANUAL_SET);
	else
		ip_btn_ext_list[0].buff = (uint8_t *)Lang_GetStringByID(STRING_ID_IP_AUTO_SET);
    ip_btn_ext_list[0].user_event_cb = ethernet_ip_btn_user_cb;
    ip_btn_ext_list[1].buff = &p_sys_menu_model->local_ip[0];
    ip_btn_ext_list[1].user_event_cb = ethernet_ip_btn_user_cb;
    ip_btn_ext_list[2].buff = &p_sys_menu_model->netmask[0];
    ip_btn_ext_list[2].user_event_cb = ethernet_ip_btn_user_cb;
	ip_btn_ext_list[3].buff = &p_sys_menu_model->gateway[0];
    ip_btn_ext_list[3].user_event_cb = ethernet_ip_btn_user_cb;
	
	if(p_sys_menu_model->eth_start == ETH_DHCP_START)
	{
		ui_get_dhcp_ip_addr(ip_btn_ext_list[1].buff);
		ui_get_dhcp_netmask_addr(ip_btn_ext_list[2].buff);
		ui_get_dhcp_gateway_addr(ip_btn_ext_list[3].buff);
	}
	ip_btn_ext_list[0].obj = ip_btn_create(btn_cont, Lang_GetStringByID(STRING_ID_UINVERSE_STATE), &ip_btn_ext_list[0], 1);
    lv_group_add_obj(indev_group, ip_btn_ext_list[0].obj);
	
    ip_btn_ext_list[1].obj = ip_btn_create(btn_cont, Lang_GetStringByID(STRING_ID_IP_ADDRESS), &ip_btn_ext_list[1], 0);
    lv_group_add_obj(indev_group, ip_btn_ext_list[1].obj);
    
    ip_btn_ext_list[2].obj = ip_btn_create(btn_cont, Lang_GetStringByID(STRING_ID_NETMASK), &ip_btn_ext_list[2], 0);
    lv_group_add_obj(indev_group, ip_btn_ext_list[2].obj);
    
    ip_btn_ext_list[3].obj = ip_btn_create(btn_cont, Lang_GetStringByID(STRING_ID_GATEWAY), &ip_btn_ext_list[3], 0);
    lv_group_add_obj(indev_group, ip_btn_ext_list[3].obj);
	
	lv_obj_t* mac_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(mac_label, Font_ResouceGet(FONT_18), 0);
    lv_obj_set_style_text_color(mac_label, lv_color_white(), 0);
    
	
    lv_label_set_text(mac_label, sdb);
    lv_obj_align_to(mac_label, btn_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

	s_ctr_view.scroll_cont = my_list_scroll_create(lv_scr_act(), 3, 160);
	lv_obj_align_to(s_ctr_view.scroll_cont, btn_cont, LV_ALIGN_OUT_RIGHT_TOP, 21, 2);  
	
    lv_group_set_wrap(indev_group, false);
	lv_group_focus_obj(ip_btn_ext_list[0].obj);
}

static void ethernet_state_set_init(void)
{
	lv_obj_t* title = lv_label_create(lv_scr_act());            
	lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
	lv_obj_set_style_text_color(title, lv_color_white(), 0);
	lv_label_set_text(title, Lang_GetStringByID(STRING_ID_LAN));
	lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);   
   
	list_ext4.cur_sel = screen_get_act_index();  
	list_ext4.release_cb = ethernet_state_set_event_cb;
	lv_obj_t* list = my_list_create(lv_scr_act(), lv_pct(85), lv_pct(60), &list_ext4); 
	lv_obj_set_flex_align(list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);                       
	lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 50);
	
	my_list_add_btn(list, Lang_GetStringByID(STRING_ID_IP_MANUAL_SET), 10);
	lv_obj_t* btn =  lv_obj_get_child(list, 0);    
	lv_obj_t* img =  lv_obj_get_child(btn, 1); 
	lv_obj_add_flag(img, LV_OBJ_FLAG_HIDDEN);
	my_list_add_btn(list, Lang_GetStringByID(STRING_ID_IP_AUTO_SET), 10);
	lv_obj_t* btn1 =  lv_obj_get_child(list, 1);    
	lv_obj_t* img1 =  lv_obj_get_child(btn1, 1);   
	lv_obj_add_flag(img1, LV_OBJ_FLAG_HIDDEN);
	
	lv_group_add_obj(indev_group, list);
	lv_group_focus_obj(list);
	lv_group_set_editing(indev_group, true);
}

static void ethernet_uinverse_set_refresh(void)
{
	uint8_t state = 0;
	uint16_t universe = 0;
	uint8_t net; 
    uint8_t sub_net;
    uint8_t uni;
	
	lv_obj_t* parent = lv_obj_get_child(lv_scr_act(), 1);
	lv_obj_t* btn =  lv_obj_get_child(parent, 0);    //获取状态文本对象
	lv_obj_t* btn1 =  lv_obj_get_child(parent, 1);    //获取artnet universe对象
	lv_obj_t* label =  lv_obj_get_child(btn1, 0);    //获取artnet universe文本对象
	lv_obj_t* btn2 =  lv_obj_get_child(parent, 2);    //获取net文本对象
	lv_obj_t* btn3 =  lv_obj_get_child(parent, 3);    //获取sacn universe对象
	lv_obj_t* label2 =  lv_obj_get_child(btn3, 0);    //获取sacn universe文本对象
	
	lv_obj_t* mac_label = lv_label_create(btn);
	lv_obj_set_style_text_font(mac_label, Font_ResouceGet(FONT_18_BOLD), 0);
	state = ui_get_eth_universe_state();
	switch(state)
	{
		case ETH_UNIVERSE_AUTOMATIC: lv_label_set_text(mac_label, Lang_GetStringByID(STRING_ID_ETH_AUTO_STATE)); break;
		case ETH_UNIVERSE_ARTNET:    lv_label_set_text(mac_label, Lang_GetStringByID(STRING_ID_ETH_ARTNET_STATE)); break;
		case ETH_UNIVERSE_SACN:      lv_label_set_text(mac_label, Lang_GetStringByID(STRING_ID_ETH_SACN_STATE)); break;
		case ETH_UNIVERSE_OFF:       lv_label_set_text(mac_label, Lang_GetStringByID(STRING_ID_ETH_OFF_STATE)); break;
		default:break;
	}
	lv_obj_align(mac_label, LV_ALIGN_CENTER, 0, 2);
	
	if(state == ETH_UNIVERSE_OFF)
	{
		return;
	}
	lv_obj_t* mac_labe2 = lv_label_create(btn1);
	lv_obj_set_style_text_font(mac_labe2, Font_ResouceGet(FONT_18_BOLD), 0);
	universe = ui_get_eth_artnet_universe();
	if(universe > 32767)
		universe = 32767;
	lv_snprintf(sdb, sizeof(sdb), "%d", universe);
    lv_label_set_text(mac_labe2, sdb);
	lv_obj_align_to(mac_labe2, label, LV_ALIGN_OUT_RIGHT_MID, -10, 2);
	
	lv_obj_t* mac_labe3 = lv_label_create(btn2);
	lv_obj_set_style_text_font(mac_labe3, Font_ResouceGet(FONT_18_BOLD), 0);
	net = (universe & 0x7f00) >> 8;
	lv_snprintf(sdb, sizeof(sdb), "%d", net);
	lv_label_set_text(mac_labe3, sdb);
	lv_obj_align(mac_labe3, LV_ALIGN_LEFT_MID, 45, 1);
    sub_net = (universe & 0x00f0) >> 4;
	lv_snprintf(sdb, sizeof(sdb), "%d", sub_net);
	lv_obj_t* mac_labe4 = lv_label_create(btn2);
	lv_obj_set_style_text_font(mac_labe4, Font_ResouceGet(FONT_18_BOLD), 0);
	lv_label_set_text(mac_labe4, sdb);
	if(uni < 10)
		lv_obj_align(mac_labe4, LV_ALIGN_CENTER, 5, 1);
	else
		lv_obj_align(mac_labe4, LV_ALIGN_CENTER, 10, 1);
	
    uni = (universe & 0x000f);
	lv_snprintf(sdb, sizeof(sdb), "%d", uni);
	lv_obj_t* mac_labe5 = lv_label_create(btn2);
	lv_obj_set_style_text_font(mac_labe5, Font_ResouceGet(FONT_18_BOLD), 0);
	lv_label_set_text(mac_labe5, sdb);
	if(uni < 10)
		lv_obj_align(mac_labe5, LV_ALIGN_RIGHT_MID, -45, 1);
    else
		lv_obj_align(mac_labe5, LV_ALIGN_RIGHT_MID, -35, 1);
	
	lv_obj_t* mac_labe6 = lv_label_create(btn3);
	lv_obj_set_style_text_font(mac_labe6, Font_ResouceGet(FONT_18_BOLD), 0);
	universe = ui_get_eth_sacn_universe();
	lv_snprintf(sdb, sizeof(sdb), "%d", universe);
    lv_label_set_text(mac_labe6, sdb);
	lv_obj_align_to(mac_labe6, label2, LV_ALIGN_OUT_RIGHT_MID, -10, 2);
}

static void ethernet_uinverse_set_init(void)
{
	uint8_t state = 0;
	
	lv_obj_t* title = lv_label_create(lv_scr_act());            
	lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
	lv_obj_set_style_text_color(title, lv_color_white(), 0);
	lv_label_set_text(title, Lang_GetStringByID(STRING_ID_LAN_UINVER_SET));
	lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);   
	
    state = ui_get_eth_universe_state();
	if(state == ETH_UNIVERSE_OFF)
		eth_spid_index[1] = 0;
	list_ext2.cur_sel = eth_spid_index[1];  
	list_ext2.release_cb = ethernet_set_list_event_cb;
	lv_obj_t* list = my_list_create(lv_scr_act(), lv_pct(85), lv_pct(80), &list_ext2); 
	lv_obj_set_flex_align(list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);                       
	lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 38);
	
	my_list_add_btn(list, Lang_GetStringByID(STRING_ID_UINVERSE_STATE), 10);
	if(state != ETH_UNIVERSE_OFF)
	{
		my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ART_NET_UINVERSE), 10);
		lv_obj_t* btn1 =  lv_obj_get_child(list, 1);    //获取artnet universe对象
		lv_obj_t* label =  lv_obj_get_child(btn1, 0);    //获取artnet universe文本对象
		lv_obj_set_width(label, 174);
		my_list_add_btn(list, Lang_GetStringByID(STRING_ID_NET_SUB_UNI), 10);
		my_list_add_btn(list, Lang_GetStringByID(STRING_ID_SACN_UINVERSE), 10);
		lv_obj_t* btn2 =  lv_obj_get_child(list, 3);    //获取artnet universe对象
		lv_obj_t* labe2 =  lv_obj_get_child(btn2, 0);    //获取artnet universe文本对象
		lv_obj_set_width(labe2, 155);
	}
	
	lv_group_add_obj(indev_group, list);
	lv_group_focus_obj(list);
	lv_group_set_editing(indev_group, true);
	
	ethernet_uinverse_set_refresh();
}

static void ethernet_uinverse_state_set_init(void)
{
	lv_obj_t* title = lv_label_create(lv_scr_act());            
	lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
	lv_obj_set_style_text_color(title, lv_color_white(), 0);
	lv_label_set_text(title, Lang_GetStringByID(STRING_ID_LAN_UINVER_STATE));
	lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);   
   
	list_ext3.cur_sel = screen_get_act_index();  
	list_ext3.release_cb = ethernet_data_state_set_event_cb;
	lv_obj_t* list = my_list_create(lv_scr_act(), lv_pct(85), lv_pct(80), &list_ext3); 
	lv_obj_set_flex_align(list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);                       
	lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 38);
	
	my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ETH_AUTO_STATE), 10);
	lv_obj_t* btn =  lv_obj_get_child(list, 0);    
	lv_obj_t* img =  lv_obj_get_child(btn, 1); 
	lv_obj_add_flag(img, LV_OBJ_FLAG_HIDDEN);
	my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ETH_ARTNET_STATE), 10);
	lv_obj_t* btn1 =  lv_obj_get_child(list, 1);    
	lv_obj_t* img1 =  lv_obj_get_child(btn1, 1);   
	lv_obj_add_flag(img1, LV_OBJ_FLAG_HIDDEN);
	my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ETH_SACN_STATE), 10);
	lv_obj_t* btn2 =  lv_obj_get_child(list, 2);    
	lv_obj_t* img2 =  lv_obj_get_child(btn2, 1); 
	lv_obj_add_flag(img2, LV_OBJ_FLAG_HIDDEN);
	my_list_add_btn(list, Lang_GetStringByID(STRING_ID_ETH_OFF_STATE), 10);
	lv_obj_t* btn3 =  lv_obj_get_child(list, 3);    
	lv_obj_t* img3 =  lv_obj_get_child(btn3, 1);    
	lv_obj_add_flag(img3, LV_OBJ_FLAG_HIDDEN);
	
	lv_group_add_obj(indev_group, list);
	lv_group_focus_obj(list);
	lv_group_set_editing(indev_group, true);
}

static void ethernet_artnet_uinverse_set_init(void)
{
	uint16_t universe;
	
	
	lv_obj_t* title = lv_label_create(lv_scr_act());            
	lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
	lv_obj_set_style_text_color(title, lv_color_white(), 0);
	lv_label_set_text(title, Lang_GetStringByID(STRING_ID_ART_NET_UINVERSE_NAME));
	lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);  
	
	lv_obj_t* btn_plate = lv_obj_create(lv_scr_act());
	lv_obj_add_style(btn_plate, &style_common_btn, 0);
	lv_obj_set_style_text_font(btn_plate, Font_ResouceGet(FONT_48), 0);
	lv_obj_set_style_radius(btn_plate, 0, 0);
	lv_obj_set_size(btn_plate, 265, 159);
//	lv_obj_add_event_cb(btn_plate, ethernet_artnet_universe_set_event_cb, LV_EVENT_ALL, NULL);
	lv_obj_align(btn_plate, LV_ALIGN_CENTER, 0, 10);
	
	lv_obj_t* uni_btn = lv_obj_create(btn_plate);
	lv_obj_add_style(uni_btn, &style_common_btn, 0);
	lv_obj_set_style_text_font(uni_btn, Font_ResouceGet(FONT_48), 0);
	lv_obj_set_style_radius(uni_btn, 0, 0);
	lv_obj_set_size(uni_btn, 175, 52);
	lv_obj_set_flex_flow(uni_btn, LV_FLEX_FLOW_ROW);
//	lv_obj_add_event_cb(btn_plate, ethernet_artnet_universe_set_event_cb, LV_EVENT_ALL, NULL);
	lv_obj_align(uni_btn, LV_ALIGN_CENTER, 0, 0);
	
	universe = ui_get_eth_artnet_universe();
	if(universe > 32767)
		universe = 32767;
	s_art_net_num[0] = universe/10000;
	s_art_net_num[1] = universe%10000/1000;
	s_art_net_num[2] = universe%10000%1000/100;
	s_art_net_num[3] = universe%10000%1000%100/10;
	s_art_net_num[4] = universe%10000%1000%100%10;
	for(uint8_t i = 0; i < 5; i++)
	{
		lv_obj_t* obj = lv_obj_create(uni_btn);
		lv_obj_add_style(obj, &style_common_btn, LV_STATE_DEFAULT);
		lv_obj_add_style(obj, &style_common_focued_btn, LV_STATE_EDITED);
		lv_obj_add_style(obj, &style_select_panel, LV_STATE_FOCUSED);
		lv_obj_set_size(obj, 28, 52);
		lv_obj_add_event_cb(obj, ethernet_artnet_universe_set_event_cb, LV_EVENT_ALL, NULL);
		lv_obj_set_user_data(obj, &s_art_net_num[i]);
		lv_group_add_obj(indev_group, obj);
		lv_obj_t* label = lv_label_create(obj);
		lv_obj_set_style_text_color(label, lv_color_white(), 0);
		lv_snprintf(sdb, sizeof(sdb), "%d", s_art_net_num[i]);
		lv_label_set_text(label, sdb);
		lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
	}
	lv_obj_t* child = lv_obj_get_child(uni_btn, 4);
	
	lv_group_focus_obj(child);
	lv_group_set_editing(indev_group, false);
}

static void ethernet_artnet_uinverse_set_refresh(lv_obj_t* obj, uint8_t mode)
{
	uint32_t universe = 0;
	uint8_t number[5] = {0};
//	char *data = NULL;
	uint16_t index = 10000;
	
	lv_obj_t *pren = lv_obj_get_parent(obj);
	uint8_t child_sum = lv_obj_get_child_cnt(pren);
	for (uint8_t cnt = 0; cnt < child_sum; cnt++)
	{
		lv_obj_t* child = lv_obj_get_child(pren, cnt);
		uint8_t* user_data = lv_obj_get_user_data(child);
		number[cnt] = * user_data;
		universe += (number[cnt] * index);
		index = index / 10;
	} 
	
	if(mode == 0)
	{
		if(universe > 32767)
			universe = 32767;
		p_sys_menu_model->eth_artnet_universe = universe;      
		data_center_write_config_data_no_event(SYS_CONFIG_ETH_ARTNET_UNIVERSE, &p_sys_menu_model->eth_artnet_universe);
		ui_set_eth_artnet_universe(p_sys_menu_model->eth_artnet_universe);
	}
	else if(mode == 1)
	{
		if(universe > 63999)
			universe = 63999;
		p_sys_menu_model->eth_sacn_universe = universe;      
		data_center_write_config_data_no_event(SYS_CONFIG_ETH_SACN_UNIVERSE, &p_sys_menu_model->eth_sacn_universe);
		ui_set_eth_sacn_universe(p_sys_menu_model->eth_sacn_universe);
	}
}

static uint8_t net_count;
static uint8_t net_enter;

static void ethernet_artnet_net_set_init(void)
{
	uint16_t universe;
	uint8_t net = 0;
	uint8_t sub = 0;
	uint8_t uni = 0;
	
	net_count = 0;
	net_enter = 0;
	universe = ui_get_eth_artnet_universe();
	if(universe > 32767)
		universe = 32767;
	net = (universe & 0x7f00) >> 8;
	sub = (universe & 0x00f0) >> 4;
	uni = (universe & 0x000f);
	
	lv_obj_t* title = lv_label_create(lv_scr_act());            
	lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
	lv_obj_set_style_text_color(title, lv_color_white(), 0);
	lv_label_set_text(title, Lang_GetStringByID(STRING_ID_ART_NET_UINVERSE_NAME));
	lv_obj_add_event_cb(title, ethernet_artnet_net_event_cb, LV_EVENT_ALL, NULL);
	lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);  
	
	lv_obj_t* btn_plate = lv_obj_create(lv_scr_act());
	lv_obj_add_style(btn_plate, &style_common_btn, 0);
	lv_obj_set_style_text_font(btn_plate, Font_ResouceGet(FONT_48), 0);
	lv_obj_set_style_border_color(btn_plate, lv_color_make(40,40,40), 0);
	lv_obj_set_style_border_width(btn_plate, 2, 0);
	lv_obj_set_style_border_side(btn_plate, LV_BORDER_SIDE_FULL, 0); //设置样式边框显示范围
	lv_obj_set_size(btn_plate, 95, 63);
	lv_obj_align(btn_plate, LV_ALIGN_LEFT_MID, 5, -5);
	
	lv_obj_t* label = lv_label_create(btn_plate);
	lv_obj_set_style_text_color(label, lv_color_make(121,121,121), 0);
	lv_snprintf(sdb, sizeof(sdb), "%d", net);
    lv_label_set_text(label, sdb);
	lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
	
	lv_obj_t* btn_plate2 = lv_obj_create(lv_scr_act());
	lv_obj_add_style(btn_plate2, &style_common_btn, 0);
	lv_obj_set_style_text_font(btn_plate2, Font_ResouceGet(FONT_48), 0);
	lv_obj_set_style_border_width(btn_plate2, 2, 0);
	lv_obj_set_style_outline_color(btn_plate2, lv_color_make(40,40,40), 0);
	lv_obj_set_style_border_side(btn_plate2, LV_BORDER_SIDE_NONE, 0); //设置样式边框显示范围
	lv_obj_set_size(btn_plate2, 95, 63);
	lv_obj_align(btn_plate2, LV_ALIGN_CENTER, 0, -5);
	
	lv_obj_t* label2 = lv_label_create(btn_plate2);
	lv_obj_set_style_text_color(label2, lv_color_make(121,121,121), 0);
	lv_snprintf(sdb, sizeof(sdb), "%d", sub);
    lv_label_set_text(label2, sdb);
	lv_obj_align(label2, LV_ALIGN_CENTER, 0, 0);
	
	lv_obj_t* btn_plate3 = lv_obj_create(lv_scr_act());
	lv_obj_add_style(btn_plate3, &style_common_btn, 0);
	lv_obj_set_style_text_font(btn_plate3, Font_ResouceGet(FONT_48), 0);
	lv_obj_set_style_outline_color(btn_plate3, lv_color_make(40,40,40), 0);
	lv_obj_set_size(btn_plate3, 95, 63);
	lv_obj_align(btn_plate3, LV_ALIGN_RIGHT_MID, -5, -5);
	
	lv_obj_t* label3 = lv_label_create(btn_plate3);
	lv_obj_set_style_text_color(label3, lv_color_make(121,121,121), 0);
	lv_snprintf(sdb, sizeof(sdb), "%d", uni);
    lv_label_set_text(label3, sdb);
	lv_obj_align(label3, LV_ALIGN_CENTER, 0, 0);
	
	lv_obj_t* name_label = lv_label_create(lv_scr_act());
	lv_obj_set_style_text_color(name_label, lv_color_make(121,121,121), 0);
	lv_obj_set_style_text_font(name_label, Font_ResouceGet(FONT_18_MEDIUM), 0);
    lv_label_set_text(name_label, "Net");
	lv_obj_align_to(name_label, btn_plate, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
	
	lv_obj_t* name_label2 = lv_label_create(lv_scr_act());
	lv_obj_set_style_text_color(name_label2, lv_color_make(121,121,121), 0);
	lv_obj_set_style_text_font(name_label2, Font_ResouceGet(FONT_18_MEDIUM), 0);
    lv_label_set_text(name_label2, "Sub-Net");
	lv_obj_align_to(name_label2, btn_plate2, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
	
	lv_obj_t* name_label3 = lv_label_create(lv_scr_act());
	lv_obj_set_style_text_color(name_label3, lv_color_make(121,121,121), 0);
	lv_obj_set_style_text_font(name_label3, Font_ResouceGet(FONT_18_MEDIUM), 0);
    lv_label_set_text(name_label3, "Universe");
	lv_obj_align_to(name_label3, btn_plate3, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
	
	ethernet_artnet_net_refresh(net_count,0);
	
	lv_group_add_obj(indev_group, title);
	lv_group_focus_obj(title);
	lv_group_set_editing(indev_group, true);
}

static void ethernet_artnet_net_refresh(uint8_t count, uint8_t mode)
{
	if(mode == 0)
	{
		for(uint8_t i=0; i < 3; i++)
		{
			lv_obj_t* btn = lv_obj_get_child(lv_scr_act(), 1+i);
			if(i == count)
			{
				lv_obj_set_style_border_color(btn, RED_THEME_COLOR, 0);
				lv_obj_set_style_border_width(btn, 2, 0);
				lv_obj_set_style_border_side(btn, LV_BORDER_SIDE_FULL, 0); //设置样式边框显示范围
			}
			else
			{
				lv_obj_set_style_border_width(btn, 0, 0);
				lv_obj_set_style_border_side(btn, LV_BORDER_SIDE_NONE, 0); //设置样式边框显示范围
				lv_obj_set_style_border_color(btn, lv_color_make(40,40,40), 0);
			}
		}
	}
	else if(mode == 1)
	{
		lv_obj_t* btn = lv_obj_get_child(lv_scr_act(), 1+net_count);
		lv_obj_t* label = lv_obj_get_child(btn, 0);
		uint16_t universe;
		uint8_t net = 0;
		uint8_t sub = 0;
		uint8_t uni = 0;
		
		universe = ui_get_eth_artnet_universe();
		
		if(count == 0)
		{
			net = (universe & 0x7f00) >> 8;
			lv_snprintf(sdb, sizeof(sdb), "%d", net);
			lv_label_set_text(label, sdb);
		}
		else if(count == 1)
		{
			sub = (universe & 0x00f0) >> 4;
			lv_snprintf(sdb, sizeof(sdb), "%d", sub);
			lv_label_set_text(label, sdb);
		}
		else if(count == 2)
		{
			uni = (universe & 0x000f);
			lv_snprintf(sdb, sizeof(sdb), "%d", uni);
			lv_label_set_text(label, sdb);
		}
	}
}

static void ethernet_artnet_net_event_cb(lv_event_t* e)
{
	lv_event_code_t event = lv_event_get_code(e);
	uint32_t key = *(uint32_t *)lv_event_get_param(e);
	
	if (event == LV_EVENT_RELEASED)
    {
		if(net_enter == 0)
		{
			lv_obj_t* btn = lv_obj_get_child(lv_scr_act(), 1+net_count);
			lv_obj_t* label = lv_obj_get_child(btn, 0);
			lv_obj_set_style_text_color(label, lv_color_white(), 0);
			lv_obj_t* labe2 = lv_obj_get_child(lv_scr_act(), 4+net_count);
			lv_obj_set_style_text_color(labe2, lv_color_white(), 0);
			net_enter = 1;
		}
		else if(net_enter == 1)
		{
			lv_obj_t* btn = lv_obj_get_child(lv_scr_act(), 1+net_count);
			lv_obj_t* label = lv_obj_get_child(btn, 0);
			lv_obj_set_style_text_color(label, lv_color_make(121,121,121), 0);
			lv_obj_t* labe2 = lv_obj_get_child(lv_scr_act(), 4+net_count);
			lv_obj_set_style_text_color(labe2, lv_color_make(121,121,121), 0);
			net_enter = 0;
		}
	}
	else if (event == LV_EVENT_KEY)
    {
        if (key == LV_KEY_RIGHT)
        {
			if(net_enter == 0)
			{
				net_count++;
				if(net_count > 2)
					net_count = 2;
				ethernet_artnet_net_refresh(net_count, 0);
			}
			else if(net_enter == 1)
			{
				uint16_t universe;
				uint8_t net = 0;
				uint8_t sub = 0;
				uint8_t uni = 0;
				
				universe = ui_get_eth_artnet_universe();
				if(universe > 32767)
					universe = 32767;
				net = (universe & 0x7f00) >> 8;
				sub = (universe & 0x00f0) >> 4;
				uni = (universe & 0x000f);
				if(net_count == 0)
				{
					net = net + 1 * common_angle_get_step(enc_get_step_index());
					if(net > 127)
						net = 127;
				}
				else if(net_count == 1)
				{
					sub = sub + 1;
					if(sub > 15)
						sub = 15;
				}
				else if(net_count == 2)
				{
					uni = uni + 1;
					if(uni > 15)
						uni = 15;
				}
				universe = (net & 0x7f) << 8 | (sub & 0x0f) << 4 | (uni & 0x0f);
				ui_set_eth_artnet_universe(universe);
				p_sys_menu_model->eth_artnet_universe = universe;      
				data_center_write_config_data_no_event(SYS_CONFIG_ETH_ARTNET_UNIVERSE, &p_sys_menu_model->eth_artnet_universe);
				ethernet_artnet_net_refresh(net_count, 1);
			}
		}
		else if(key == LV_KEY_LEFT)
		{
			if(net_enter == 0)
			{
				net_count--;
				if(net_count > 2)
					net_count = 0;
				ethernet_artnet_net_refresh(net_count, 0);
			}
			else if(net_enter == 1)
			{
				uint16_t universe;
				uint8_t net = 0;
				uint8_t sub = 0;
				uint8_t uni = 0;
				
				universe = ui_get_eth_artnet_universe();
				if(universe > 32767)
					universe = 32767;
				net = (universe & 0x7f00) >> 8;
				sub = (universe & 0x00f0) >> 4;
				uni = (universe & 0x000f);
				
				if(net_count == 0)
				{
					net = net - 1 * common_angle_get_step(enc_get_step_index());
					if(net > 127)
						net = 0;
				}
				else if(net_count == 1)
				{
					sub = sub - 1;
					if(sub > 15)
						sub = 0;
				}
				else if(net_count == 2)
				{
					uni = uni - 1;
					if(uni > 15)
						uni = 0;
				}
				universe = (net & 0x7f) << 8 | (sub & 0x0f) << 4 | (uni & 0x0f);
				ui_set_eth_artnet_universe(universe);
				p_sys_menu_model->eth_artnet_universe = universe;      
				data_center_write_config_data_no_event(SYS_CONFIG_ETH_ARTNET_UNIVERSE, &p_sys_menu_model->eth_artnet_universe);
				ethernet_artnet_net_refresh(net_count, 1);
			}
		}
		else if (key == LV_KEY_BACKSPACE)
        {            
			if(net_enter == 0)
				screen_turn_prev_page();
			else
			{
				lv_obj_t* btn = lv_obj_get_child(lv_scr_act(), 1+net_count);
				lv_obj_t* label = lv_obj_get_child(btn, 0);
				lv_obj_set_style_text_color(label, lv_color_make(121,121,121), 0);
				lv_obj_t* labe2 = lv_obj_get_child(lv_scr_act(), 4+net_count);
				lv_obj_set_style_text_color(labe2, lv_color_make(121,121,121), 0);
				net_enter = 0;
			}
        }
	}
}

static void ethernet_sacn_uinverse_set_init(void)
{
	uint16_t universe;
	
	lv_obj_t* title = lv_label_create(lv_scr_act());            
	lv_obj_set_style_text_font(title, Font_ResouceGet(FONT_22_BOLD), 0);
	lv_obj_set_style_text_color(title, lv_color_white(), 0);
	lv_label_set_text(title, Lang_GetStringByID(STRING_ID_SACN_UINVERSE_NAME));
	lv_obj_align(title, LV_ALIGN_TOP_MID, 0, SYS_MENU_TITLE_TO_TOP);  
	
	lv_obj_t* btn_plate = lv_obj_create(lv_scr_act());
	lv_obj_add_style(btn_plate, &style_common_btn, 0);
	lv_obj_set_style_text_font(btn_plate, Font_ResouceGet(FONT_48), 0);
	lv_obj_set_style_radius(btn_plate, 0, 0);
	lv_obj_set_size(btn_plate, 265, 159);
	lv_obj_align(btn_plate, LV_ALIGN_CENTER, 0, 10);
	
//	lv_obj_t* label = lv_label_create(btn_plate);
//	lv_obj_set_style_text_color(label, RED_THEME_COLOR, 0);
//	universe = ui_get_eth_sacn_universe();
//	lv_snprintf(sdb, sizeof(sdb), "%d", universe);
//    lv_label_set_text(label, sdb);
//	lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
	
	lv_obj_t* uni_btn = lv_obj_create(btn_plate);
	lv_obj_add_style(uni_btn, &style_common_btn, 0);
	lv_obj_set_style_text_font(uni_btn, Font_ResouceGet(FONT_48), 0);
	lv_obj_set_style_radius(uni_btn, 0, 0);
	lv_obj_set_size(uni_btn, 175, 52);
	lv_obj_set_flex_flow(uni_btn, LV_FLEX_FLOW_ROW);
//	lv_obj_add_event_cb(btn_plate, ethernet_artnet_universe_set_event_cb, LV_EVENT_ALL, NULL);
	lv_obj_align(uni_btn, LV_ALIGN_CENTER, 0, 0);
	
	universe = ui_get_eth_sacn_universe();
	if(universe > 63999)
		universe = 63999;
	s_sacn_num[0] = universe/10000;
	s_sacn_num[1] = universe%10000/1000;
	s_sacn_num[2] = universe%10000%1000/100;
	s_sacn_num[3] = universe%10000%1000%100/10;
	s_sacn_num[4] = universe%10000%1000%100%10;
	for(uint8_t i = 0; i < 5; i++)
	{
		lv_obj_t* obj = lv_obj_create(uni_btn);
		lv_obj_add_style(obj, &style_common_btn, LV_STATE_DEFAULT);
		lv_obj_add_style(obj, &style_common_focued_btn, LV_STATE_EDITED);
		lv_obj_add_style(obj, &style_select_panel, LV_STATE_FOCUSED);
		lv_obj_set_size(obj, 28, 52);
		lv_obj_add_event_cb(obj, ethernet_sacn_universe_set_event_cb, LV_EVENT_ALL, NULL);
		lv_obj_set_user_data(obj, &s_sacn_num[i]);
		lv_group_add_obj(indev_group, obj);
		lv_obj_t* label = lv_label_create(obj);
		lv_obj_set_style_text_color(label, lv_color_white(), 0);
		lv_snprintf(sdb, sizeof(sdb), "%d", s_sacn_num[i]);
		lv_label_set_text(label, sdb);
		lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
	}
	lv_obj_t* child = lv_obj_get_child(uni_btn, 4);
	
	lv_group_focus_obj(child);
	lv_group_set_editing(indev_group, false);
}

//static void ethernet_uinverse_refresh(uint16_t universe)
//{
//	lv_obj_t* btn =  lv_obj_get_child(lv_scr_act(), 1);    
//	lv_obj_t* label =  lv_obj_get_child(btn, 0);
//	
//	lv_snprintf(sdb, sizeof(sdb), "%d", universe);
//	lv_label_set_text(label, sdb);
//}

static void ctrl_sys_list_event_cb(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);

    if (event == LV_EVENT_RELEASED)
    {
        if(index != 0)
		{
			ui_set_ctr_spid_index(index);
			if(index == 2 && crmx_is_paired() == 1)
			{
//				ui_set_ctr_spid_index(2);
				user_turn_to_page(PAGE_CONTROL_SYSTEM, SUB_ID_LUMENRADIO_LINK_TICK, false);
			}
			else
			{
				screen_load_sub_page_with_stack(index, index-1);
			}
		}
        else
		{
			ui_set_ctr_spid_index(0);
			struct db_dmx dmx_light;
            struct sys_info_dmx        dmx;
            data_center_read_sys_info(SYS_INFO_DMX, &dmx);
            data_center_read_light_data(LIGHT_MODE_DMX, &dmx_light);
            if(dmx.eth_linked == 1 || dmx.wired_linked == 1 || dmx.wireless_linked == 1)
            {
                data_center_write_light_data(LIGHT_MODE_DMX, &dmx_light);
            }
            user_turn_to_page(PAGE_DMX_MODE, 0, false);
		}
    }
    else if (event == LV_EVENT_KEY)
    {
		ui_set_ctr_spid_index(index);
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
        
        if (key == LV_KEY_BACKSPACE)
        {
            screen_turn_prev_page();
        }
    }    
}

static void ethernet_list_event_cb(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);

    if (event == LV_EVENT_RELEASED)
    {
		eth_spid_index[0] = index;
		if(index == 0)
			screen_load_sub_page_with_stack(SUB_ID_ETHERNET_MANUAL_SET_IP, index);
		else
			screen_load_sub_page_with_stack(SUB_ID_ETHERNET_UINVERSE_SET_IP, index);
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
        
        if (key == LV_KEY_BACKSPACE)
        {
			screen_set_act_index(index);  
            ctrl_sys_load_sub_page();
        }
    }
}

static void ethernet_set_list_event_cb(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);

    if (event == LV_EVENT_PRESSED)
    {
		eth_spid_index[1] = index;
		screen_load_sub_page_with_stack(SUB_ID_ETHERNET_UINVERSE_STATE_SET+index, index);
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
        
        if (key == LV_KEY_BACKSPACE)
        {
			screen_set_act_index(index);  
            ctrl_sys_load_sub_page();
        }
    }
}

static void ethernet_state_set_event_cb(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);

    if (event == LV_EVENT_PRESSED)
    {
		uint8_t state = index;
		
		p_sys_menu_model->eth_start = state;
		
		data_center_write_config_data(SYS_CONFIG_ETH_START, &p_sys_menu_model->eth_start);
		ui_set_eth_state(p_sys_menu_model->eth_start);
		screen_load_sub_page_with_stack(SUB_ID_ETHERNET_MANUAL_SET_IP, 0);
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
        
        if (key == LV_KEY_BACKSPACE)
        {
			screen_set_act_index(index);  
            ctrl_sys_load_sub_page();
        }
    }
}

static void ethernet_data_state_set_event_cb(lv_event_t* e, uint8_t index)
{
    lv_event_code_t event = lv_event_get_code(e);

    if (event == LV_EVENT_PRESSED)
    {
		uint8_t state = index;
		
		p_sys_menu_model->eth_state_set = state;
		
		data_center_write_config_data_no_event(SYS_CONFIG_ETH_STATE_SET, &p_sys_menu_model->eth_state_set);
		ui_set_eth_universe_state(p_sys_menu_model->eth_state_set);
		screen_load_sub_page_with_stack(SUB_ID_ETHERNET_UINVERSE_SET_IP, index);
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);
        
        if (key == LV_KEY_BACKSPACE)
        {
			screen_set_act_index(index);  
            ctrl_sys_load_sub_page();
        }
    }
}

static void ethernet_artnet_universe_set_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);
	
    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
		uint8_t* user_data = lv_obj_get_user_data(obj);
		lv_group_t* group = lv_obj_get_group(obj);
		
        if (key == LV_KEY_RIGHT && lv_group_get_editing(group))
        {
			*user_data = *user_data + 1;
			if(*user_data > 9)
				*user_data = 9;
			
			lv_snprintf(sdb, sizeof(sdb), "%d", *user_data);
			lv_obj_t* label = lv_obj_get_child(obj, 0);
			lv_label_set_text(label, sdb);
			ethernet_artnet_uinverse_set_refresh(obj, 0);
        }
        if (key == LV_KEY_LEFT && lv_group_get_editing(group))
        {
			*user_data = *user_data - 1;
			if(*user_data > 9)
				*user_data = 0;
			
			lv_snprintf(sdb, sizeof(sdb), "%d", *user_data);
			lv_obj_t* label = lv_obj_get_child(obj, 0);
			lv_label_set_text(label, sdb);
			ethernet_artnet_uinverse_set_refresh(obj,0);
        }
        if (key == LV_KEY_BACKSPACE && !lv_group_get_editing(group))
        {            
            screen_load_sub_page_with_stack(SUB_ID_ETHERNET_UINVERSE_SET_IP, 1);
        }
		else if (key == LV_KEY_BACKSPACE && lv_group_get_editing(group))
		{
			lv_obj_add_state(obj, LV_STATE_DEFAULT);
			lv_group_set_editing(group, false);
		}
    }
	else if (event == LV_EVENT_PRESSED)
	{
		lv_group_t* group = lv_obj_get_group(obj);
		if (lv_group_get_editing(group))
		{
			lv_obj_add_state(obj, LV_STATE_DEFAULT);
			lv_group_set_editing(group, false);
		}
		else
		{
			lv_obj_add_state(obj, LV_STATE_FOCUSED);
			lv_group_set_editing(group, true);
		}
	}
}

static void ethernet_sacn_universe_set_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
	lv_obj_t* obj = lv_event_get_target(e);
	
    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
		uint8_t* user_data = lv_obj_get_user_data(obj);
		lv_group_t* group = lv_obj_get_group(obj);
        if (key == LV_KEY_RIGHT)
        {
			*user_data = *user_data + 1;
			if(*user_data > 9)
				*user_data = 9;
			
			lv_snprintf(sdb, sizeof(sdb), "%d", *user_data);
			lv_obj_t* label = lv_obj_get_child(obj, 0);
			lv_label_set_text(label, sdb);
			ethernet_artnet_uinverse_set_refresh(obj, 1);
        }
        else if (key == LV_KEY_LEFT)
        {
			*user_data = *user_data - 1;
			if(*user_data > 9)
				*user_data = 0;
			
			lv_snprintf(sdb, sizeof(sdb), "%d", *user_data);
			lv_obj_t* label = lv_obj_get_child(obj, 0);
			lv_label_set_text(label, sdb);
			ethernet_artnet_uinverse_set_refresh(obj, 1);
        }
        if (key == LV_KEY_BACKSPACE && !lv_group_get_editing(group))
        {            
            screen_load_sub_page_with_stack(SUB_ID_ETHERNET_UINVERSE_SET_IP, 3);
        }
		else if (key == LV_KEY_BACKSPACE && lv_group_get_editing(group))
		{
			lv_obj_add_state(obj, LV_STATE_DEFAULT);
			lv_group_set_editing(group, false);
		}
    }
	else if (event == LV_EVENT_PRESSED)
	{
		lv_group_t* group = lv_obj_get_group(obj);
		if (lv_group_get_editing(group))
		{
			lv_obj_add_state(obj, LV_STATE_DEFAULT);
			lv_group_set_editing(group, false);
		}
		else
		{
			lv_obj_add_state(obj, LV_STATE_FOCUSED);
			lv_group_set_editing(group, true);
		}
	}
}

static void ethernet_ip_btn_user_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* target = lv_event_get_target(e);
    lv_group_t* group = lv_obj_get_group(target);
	uint8_t obj_index = 0;
	
	for(uint8_t i = 0; i < 4; i++)
	{
		if(target == ip_btn_ext_list[i].obj)
		{
			obj_index = i;
			break;
		}
	} 
	
    if (event == LV_EVENT_KEY && !lv_group_get_editing(group))
    {
        uint32_t key = *(uint32_t *)lv_event_get_param(e);

        if (lv_group_get_editing(group))
        {
            if (key == LV_ENCODER_KEY_CC || key == LV_ENCODER_KEY_CW)
            {
                page_set_invalid_type(PAGE_INVALID_TYPE_PORTION);
            }
        }
        else
        {
            if (key == LV_KEY_BACKSPACE)
            {
                ctrl_sys_load_sub_page();
            }
        }        
    }   
	else if (event == LV_EVENT_PRESSED)
	{
		if(target == ip_btn_ext_list[0].obj)
			screen_load_sub_page_with_stack(SUB_ID_ETHERNET_SATRT_SET_IP, 0);
	}
	else if (event == LV_EVENT_FOCUSED)
	{
		my_list_scroll_refresh(s_ctr_view.scroll_cont, lv_map(obj_index, 0, 4, 0, 100));
	}
	else if (event == LV_EVENT_KEY)
	{
		uint32_t key = *(uint32_t *)lv_event_get_param(e);
		if ( LV_ENCODER_KEY_CC == key) //up
		{           
			my_list_scroll_refresh(s_ctr_view.scroll_cont, lv_map(obj_index, 0, 4, 0, 100));
		}
		else if (LV_ENCODER_KEY_CW == key) //down
		{   
			my_list_scroll_refresh(s_ctr_view.scroll_cont, lv_map(obj_index, 0, 4, 0, 100));
		}
	}	
}

static void switch_btn_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target(e);
    ui_title_t* p_title = &g_ptUIStateData->title;

    shortcut_keys_event_handler(e);
	
    if (event == LV_EVENT_RELEASED)
    {
        p_sys_menu_model->ble_on = !p_sys_menu_model->ble_on;                
        if (p_sys_menu_model->ble_on) {
            if (p_sys_menu_model->crmx_on)
            {
                p_sys_menu_model->crmx_on = false;

                ui_crmx_power_changed(p_sys_menu_model->crmx_on);
                data_center_write_config_data(SYS_CONFIG_CRMX_CTRL, &p_sys_menu_model->crmx_on);
            }
                        
            p_title->com_type = COM_TYPE_BLE;            
        }
		if(ui_get_master_mode())
		{
			extern void gui_data_sync_event_gen(void);		
			gui_data_sync_event_gen();
		}
        ui_ble_power_changed(p_sys_menu_model->ble_on);
        data_center_write_config_data(SYS_CONFIG_BLE_CTRL, &p_sys_menu_model->ble_on);        

        list_switch_btn_refresh(btn, p_sys_menu_model->ble_on);     
        ble_hidden_setting_refresh();   
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        if (key == LV_KEY_BACKSPACE)
        {
            ctrl_sys_load_sub_page();
        }
    }
}

static void ble_hidden_setting_refresh(void)
{
    if (com_setting_list == NULL)   return;

    lv_obj_t* btn1 = lv_obj_get_child(com_setting_list, 0);
    lv_obj_t* btn2 = lv_obj_get_child(com_setting_list, 1);

    if (p_sys_menu_model->ble_on)
    {
        lv_obj_clear_flag(btn2, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_add_flag(btn2, LV_OBJ_FLAG_HIDDEN);
    }
}

static void lumenradio_switch_btn_event_cb(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    ui_title_t* p_title = &g_ptUIStateData->title;
    lv_obj_t* btn = lv_event_get_target(e);

    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_RELEASED)
    {
        p_sys_menu_model->crmx_on = !p_sys_menu_model->crmx_on;        
        if (p_sys_menu_model->crmx_on) {
            p_sys_menu_model->ble_on = false;            
            p_title->com_type = COM_TYPE_WIFI;
            ui_ble_power_changed(p_sys_menu_model->ble_on);
            data_center_write_config_data(SYS_CONFIG_BLE_CTRL, &p_sys_menu_model->ble_on);
//			user_turn_to_page(PAGE_CONTROL_SYSTEM, SUB_ID_LUMENRADIO_PAIRING, 1);
        }           
		if(ui_get_master_mode())
		{
			extern void gui_data_sync_event_gen(void);		
			gui_data_sync_event_gen();
		}
        ui_crmx_power_changed(p_sys_menu_model->crmx_on);        
        data_center_write_config_data(SYS_CONFIG_CRMX_CTRL, &p_sys_menu_model->crmx_on);

        list_switch_btn_refresh(btn, p_sys_menu_model->crmx_on);     
        lumenradio_hidden_setting_refresh();  
		lv_obj_t* list = lv_obj_get_child(lv_scr_act(), 1);
		lv_obj_t* btn = lv_obj_get_child(list, 1);
		lv_obj_t* btn1 = lv_obj_get_child(list, 0);
		
		if(crmx_is_paired() == 0)
		{
			lv_obj_add_state(btn1, LV_STATE_FOCUSED);
			lv_obj_clear_state(btn, LV_STATE_FOCUSED);
			lv_event_send(btn, LV_EVENT_DEFOCUSED, NULL);
			list_ext.cur_sel = 0;
			lv_obj_add_state(btn1, LV_STATE_FOCUSED);
			lv_event_send(btn1, LV_EVENT_FOCUSED, NULL);
			lv_obj_add_flag(btn, LV_OBJ_FLAG_HIDDEN);
		}
		else 
		{
			lv_obj_clear_flag(btn, LV_OBJ_FLAG_HIDDEN);
		}
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        if (key == LV_KEY_BACKSPACE)
        {
            ctrl_sys_load_sub_page();
        }
    }
}

static void lumenradio_hidden_setting_refresh(void)
{
    if (com_setting_list == NULL)   return;

    lv_obj_t* btn1 = lv_obj_get_child(com_setting_list, 0);
    lv_obj_t* btn2 = lv_obj_get_child(com_setting_list, 1);

    if (p_sys_menu_model->crmx_on)
    {
        lv_obj_clear_flag(btn2, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        lv_obj_add_flag(btn2, LV_OBJ_FLAG_HIDDEN);
    }
}

static void lumenradio_pair_btn_event(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target(e);
    lumenradio_status_t *p_lumen_status = &g_ptUIStateData->lumen_status;

    (void)p_lumen_status;
    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_RELEASED)
    {
        uint8_t index = lv_obj_get_child_id(btn);
		ui_crmx_start_unlink();
        screen_load_sub_page_with_stack(SUB_ID_LUMENRADIO_UNLINK_SUCCEED, index);
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        if (key == LV_KEY_BACKSPACE)
        {            
            ctrl_sys_load_sub_page();
        }
    }     
}

static void lumenradio_pair_animation(void *cont, int32_t index)
{
    refresh_pair_round(cont, index);
}

static void refresh_pair_round(lv_obj_t* cont, uint8_t index)
{   
    uint8_t cnt = lv_obj_get_child_cnt(cont);
    uint8_t bg_color;
    lv_obj_t* obj;

    (void)bg_color;
    index %= 3;
    for (uint8_t i = 0; i < cnt; i++)
    {        
        obj = lv_obj_get_child(cont, i);
        if (i != index)
        {
            lv_obj_set_style_bg_color(obj, lv_color_make(51, 51, 51), 0);
        } 
        else
        {
            lv_obj_set_style_bg_color(obj, lv_color_make(255, 255, 255), 0);
        }        
    }

    index++;
}

static void lumenradio_unpair_confirm_event(my_msgbox_ext_t* ext)
{
    lumenradio_status_t *p_lumen_status = &g_ptUIStateData->lumen_status;

    if (ext->answer == MSGBOX_ID_YES)
    {
        UI_PRINTF("MSGBOX_ID_YES -> Force to reset lumenradio.\r\n");
        p_lumen_status->unpair_status = LUMENRADIO_UNPAIR_START;
        ui_crmx_start_unlink();
        ctrl_sys_load_sub_page();
    }
    else if (ext->answer == MSGBOX_ID_NO)    
    {
        p_lumen_status->unpair_status = LUMENRADIO_UNPAIR_IDLE;
        ctrl_sys_load_sub_page();
    }       
    else if (ext->answer == MSGBOX_ID_NO_CONFIRM)
    {
        p_lumen_status->unpair_status = LUMENRADIO_UNPAIR_IDLE;
        ctrl_sys_load_sub_page();
    }
}

static void ble_reset_btn_event(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target(e);

    shortcut_keys_event_handler(e);

    if (event == LV_EVENT_RELEASED)
    {
        uint8_t index = lv_obj_get_child_id(btn);
        screen_load_sub_page_with_stack(SUB_ID_BLE_RESET_CONFIRM, index);
    }
    else if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        if (key == LV_KEY_BACKSPACE)
        {
            ctrl_sys_load_sub_page();
        }
    }    
}

static void ble_reset_del_event(my_msgbox_ext_t* ext)
{
    if (ext->answer == MSGBOX_ID_YES)
    {                  
        ui_ble_start_reset();     
        screen_load_sub_page(SUB_ID_BLE_RESETING, 0);       
		if(ui_get_master_mode())
		{
			extern void gui_ble_reset_event_gen(void);		
			gui_ble_reset_event_gen();
		}		
    }
    else if (ext->answer == MSGBOX_ID_NO)    
    {
        ctrl_sys_load_sub_page();
    }
}

static void refresh_ble_resetting(void)
{
    ble_status_t* p_ble_status = &g_ptUIStateData->ble_status;
    lv_obj_t* cont = lv_obj_get_child(lv_scr_act(), 0);
    lv_obj_t* ble_reset_bar = lv_obj_get_child(cont, 0);
    lv_obj_t* process_label = lv_obj_get_child(cont, 1);

    lv_bar_set_value(ble_reset_bar, p_ble_status->percent, LV_ANIM_ON);
    lv_label_set_text_fmt(process_label, "%d%%", p_ble_status->percent);
    lv_obj_align(process_label, LV_ALIGN_TOP_RIGHT, 0, 0);
}

/**
 * @brief 
 * @param e 
 */
static void ctrl_sys_comm_event(lv_event_t* e)
{
    lv_event_code_t event = lv_event_get_code(e);

    if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        if (key == LV_KEY_BACKSPACE)
        {
            ctrl_sys_load_sub_page();
			ui_set_dmx_state(1);
        }
    }
}

static void lumenradio_tick_event_cb(lv_event_t* e)
{
	lv_event_code_t event = lv_event_get_code(e);
	
	shortcut_keys_event_handler(e);
	
    if (event == LV_EVENT_KEY)
    {
        uint32_t key = *(uint32_t*)lv_event_get_param(e);
        if (key == LV_KEY_BACKSPACE)
        {
            user_turn_to_page(PAGE_CONTROL_SYSTEM, SUB_ID_CTRL_SYS_LIST, false);
        }
    }
	if (event == LV_EVENT_RELEASED)
	{
		user_turn_to_page(PAGE_CONTROL_SYSTEM, SUB_ID_LUMENRADIO_SETTING, false);
	}
}
