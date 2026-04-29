/*********************
 *      INCLUDES
 *********************/
#include "ui_common.h"
#include "page_pool.h"
#include "user.h"
#include "project_config.h"
/*********************
 *      DEFINES
 *********************/
#define list_for_each_entry(_tmp, _head)           for (_tmp = _head;  _tmp != NULL; _tmp = _tmp->next_page)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void page_run_init_cb(page_t *page);
static void page_run_exit_cb(page_t *page);
static void page_run_time_upd_cb(page_t *page, uint32_t time_ms);

static void restore_last_state(void);
static void maintain_pm_data(uint8_t page_id, uint8_t spid);
static void turn_to_next_page(uint8_t id, uint8_t subid, uint8_t index, bool b_record);
/**********************
 *  STATIC VARIABLES
 **********************/
static page_t* page_head = NULL;
static page_t* cur_page = NULL;
static page_t* last_page = NULL;

static uint8_t prev_pid = 0;

static page_pool_t my_page_pool;
static page_dsc_t  my_page_dsc[20];
/*********************
 *  GLOBAL VATIABLES
 *********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
/**
 * @brief 注册一个页面
 * 
 * @param page 
 */
void page_maneger_register(page_t* page, void* user_data)
{
    page->next_page = NULL;

    if (page_head == NULL)
    {
        page_head = page;
        page_head->next_page = NULL;
    }
    else 
    {
        page_t* tmp = page_head;

        while (tmp->next_page != NULL)
        {
            if (page->id == tmp->id)
                return;
            tmp = tmp->next_page;
        }

        tmp->next_page = page;
    }

    page->user_data = user_data;    
}

void page_maneger_delete(page_t* page)
{
    page->next_page = NULL;

    if (page_head == NULL)
    {
        //page_head = page;
        //page_head->next_page = NULL;
    }
    else
    {
        page_t* tmp = page_head;
        page_t* pre_page = NULL;


        while (tmp->next_page != NULL)
        {
            if (page->id == tmp->id)
            {
                pre_page->next_page = tmp->next_page;
            }
            pre_page = tmp;
            tmp = tmp->next_page;
        }
    }
}

/**
 * @brief 获取当前页面ID
 * 
 * @return uint8_t 
 */
uint8_t screen_get_act_pid(void)
{
    if (cur_page != NULL)
        return cur_page->id;

    return 0;
}

/**
 * @brief 获取当前页面聚焦索引值
 * 
 * @return uint8_t 
 */
uint8_t screen_get_act_index(void)
{
    if (cur_page != NULL)
        return cur_page->index;

    return 0;
}

/**
 * @brief 获取上一次页面的ID
 * 
 * @return uint8_t 
 */
uint8_t screen_get_last_page_id(void)
{    
    return prev_pid;
}

/**
 * @brief 设置当前页面聚焦索引值
 * 
 * @param index 
 */
void screen_set_act_index(uint8_t index)
{
    if (cur_page != NULL)
        cur_page->index = index;
}

page_invalid_type_t page_get_invalid_type(void)
{
    page_invalid_type_t ret = cur_page != NULL ? cur_page->invalid_type : PAGE_INVALID_TYPE_IDLE;
    return ret;
}

void page_set_invalid_type(page_invalid_type_t type)
{
    if (cur_page != NULL)
    {
        ui_enter_critical();
        cur_page->invalid_type = type;
        ui_exit_critical();
    }
}

/**
 * @brief 获取当前页面SUBID
 * 
 * @return uint8_t 
 */
uint8_t screen_get_act_spid(void)
{
    if (cur_page != NULL)
        return cur_page->subid;
    
    return 0;
}

/**
 * @brief 设置当前的SUB ID
 * 
 * @param spid 
 */
void screen_set_act_spid(uint8_t spid)
{
    if (cur_page != NULL)
    {
        ui_enter_critical();
        cur_page->subid = spid;        
        ui_exit_critical();
    }
}

/**
 * @brief 获取顶层的ID
 * 
 * @return uint8_t 
 */
uint8_t screen_get_top_id(void)
{
    pm_data_t* p_data = ui_get_pm_data();
    return p_data->top_id;
}

/**
 * @brief 获取最后一次页面的ID
 * 
 */
uint8_t screen_get_prev_id(void)
{
    pm_data_t* p_data = ui_get_pm_data();
    return p_data->last_id;
}

/**
 * @brief 切换页面
 * 
 * @param id 
 * @param subid 
 */
void screen_load_page(uint8_t id, uint8_t subid, bool b_record)
{
    turn_to_next_page(id, subid, 0, b_record);
}

/**
 * @brief 切换界面，并且指定聚焦的位置
 * 
 * @param id 
 * @param subid 
 * @param index 
 * @param b_record 
 */
void screen_load_page_and_index(uint8_t id, uint8_t subid, uint8_t index, bool b_record)
{
    turn_to_next_page(id, subid, index, b_record);
}

/**
 * @brief 切换页面，并且清理页面池
 * 
 * @param id 
 * @param subid 
 * @param b_record 
 */
void screen_load_page_with_clear(uint8_t id, uint8_t subid, bool b_record)
{
    ui_enter_critical();
    page_pool_clear(&my_page_pool);     
    ui_exit_critical();
    turn_to_next_page(id, subid, 0, b_record);
}

void power_screen_load_page_with_clear(uint8_t id, uint8_t subid, bool b_record)
{
    ui_enter_critical();
    page_pool_clear(&my_page_pool);     
    ui_exit_critical();
    foce_turn_to_next_page(id, subid, b_record);
}

/**
 * @brief 
 * 
 * @param sid 
 * @param index 
 */
void screen_load_sub_page(uint8_t sid, uint8_t index)
{
    turn_to_next_page(cur_page->id, sid, index, false);
}

/**
 * @brief 跳转回上一个界面
 * 
 * @return true 
 * @return false 
 */
bool screen_turn_prev_page(void)
{
    page_dsc_t dsc;
    bool ret = page_pool_pop(&my_page_pool, &dsc);
    
    if (ret) 
    {
        turn_to_next_page(dsc.pid, dsc.spid, dsc.index, false);
    }
    else if (cur_page->prev_def_pid != PAGE_NONE)
    {        
        pm_data_t* p_data = ui_get_pm_data();
        uint8_t index = 0;        
        uint8_t pid = cur_page->prev_def_pid;

        if (cur_page->prev_def_pid == PAGE_MENU)
        {
            index = menu_get_index_by_id(cur_page->id);
        }
        else if (cur_page->prev_def_pid == PAGE_LIGHT_EFFECT_MENU)
        {
            index = light_effect_get_index(cur_page->id);
            ui_light_effect_index_save(index);
        }        
        else if (cur_page->id == PAGE_MENU)
        {
            pid = p_data->top_id;
        }

        screen_load_page_and_index(pid, 0, index, false);
    }
    else if (cur_page->prev_def_pid == PAGE_NONE)
    {
        return false;
    }

    return true;
}

/**
 * @brief 获取当前页面的用户数据
 * 
 * @return void* 
 */
void* screen_get_act_page_data(void)
{
    return cur_page->user_data;
}



/**
 * @brief 设置页面的用户数据.
 * 
 * @param id 
 * @param user_data 
 */
//void*  page_get_user_data(uint8_t id)
//{
//    page_t* tmp;

//    list_for_each_entry(tmp, page_head) {
//        if (tmp->id == id)
//        {
//            return cur_page->user_data;
//        }
//    }
//}


/**
 * @brief 获取该页面的名字.
 * 
 * @return const char* 
 */
const char* screen_get_act_page_name(void)
{
    return cur_page->name;
}

/**
 * @brief 向页面发送事件
 * 
 * @param[in]event          事件
 */
void page_event_send(int event)
{
    if (cur_page && cur_page->event_cb)
    {
        cur_page->event_cb(event);
    }
}

/**
 * @brief 打印页面链表
 *
 * @param id
 * @param user_data
 */
void page_list_print(void)
{
    page_t* tmp;

    list_for_each_entry(tmp, page_head) {
        if (tmp->name != NULL)
            UI_PRINTF("page name:%s\r\n", tmp->name);
    }
}

/**
 * @brief 设置页面的用户数据.
 * 
 * @param id 
 * @param user_data 
 */
void page_set_user_data(uint8_t id, void *user_data)
{
    page_t* tmp;

    list_for_each_entry(tmp, page_head) {
        if (tmp->id == id)
        {
            tmp->user_data = user_data;
            return;
        }
    }
}

/**
 * @brief 获取页面子ID
 * 
 * @param id 
 * @return uint8_t 
 */
uint8_t page_get_sub_id(uint8_t id)
{
   page_t* tmp;
   uint8_t spid = 0;

    list_for_each_entry(tmp, page_head) {
        if (tmp->id == id)
        {
            spid = tmp->subid;
            break;
        }
    }

    return spid;
}

/**
 * @brief 设置页面子ID
 * 
 * @param id 
 */
void page_set_sub_id(uint8_t id,  uint8_t subid)
{
    page_t* tmp;

    list_for_each_entry(tmp, page_head) {
        if (tmp->id == id)
        {
            tmp->subid = subid;
            break;
        }
    }
}

/**
 * @brief 根据ID页面获取那个页面的索引.
 * 
 * @param pid 
 * @return uint8_t 
 */
uint8_t page_get_index(uint8_t pid)
{
    page_t* tmp;

    list_for_each_entry(tmp, page_head) {
        if (tmp->id == pid)
        {
            return tmp->index;
        }
    }

    return 0;
}

/**
 * @brief 页面管理器初始化
 * 
 */
void page_manager_init(void)
{   
    page_maneger_register(&page_cct,                &g_tUIAllData.cct_model);
    page_maneger_register(&page_sys_menu,           NULL);
    page_maneger_register(&page_light_effect,       NULL);
#if (PROJECT_TYPE == 308)
    page_maneger_register(&page_explosion,          &g_tUIAllData.explosion_model);
    page_maneger_register(&page_faultybulb,         &g_tUIAllData.faultybulb_model);
    page_maneger_register(&page_pulsing,            &g_tUIAllData.pulsing_model);
    page_maneger_register(&page_strobe,             &g_tUIAllData.strobe_model);
    page_maneger_register(&page_lightning,          &g_tUIAllData.lightning_model);
    page_maneger_register(&page_tv,                 &g_tUIAllData.tv_model);
    page_maneger_register(&page_fire,               &g_tUIAllData.fire_model);
#else
    page_maneger_register(&page_rgb,                &g_tUIAllData.rgb_model);
    page_maneger_register(&page_xy,                 &g_tUIAllData.xy_model);
    page_maneger_register(&page_hsi,                &g_tUIAllData.hsi_model);
    page_maneger_register(&page_gel,                &g_tUIAllData.gel_model);
    page_maneger_register(&page_source,             &g_tUIAllData.source_model);
    page_maneger_register(&page_rgb_setting,        &g_tUIAllData.pulsing2_model.mode_arg);
    page_maneger_register(&page_xy_setting,         &g_tUIAllData.pulsing2_model.mode_arg);
    page_maneger_register(&page_cct_setting,        &g_tUIAllData.pulsing2_model.mode_arg);
    page_maneger_register(&page_hsi_setting,        &g_tUIAllData.pulsing2_model.mode_arg);
    page_maneger_register(&page_gel_setting,        &g_tUIAllData.pulsing2_model.mode_arg);
    page_maneger_register(&page_source_setting,     &g_tUIAllData.pulsing2_model.mode_arg);
    page_maneger_register(&page_explosion,          &g_tUIAllData.explosion2_model);
    page_maneger_register(&page_faultybulb,         &g_tUIAllData.faultybulb2_model);
    page_maneger_register(&page_strobe,             &g_tUIAllData.strobe2_model);
    page_maneger_register(&page_pulsing,            &g_tUIAllData.pulsing2_model);
    page_maneger_register(&page_lightning,          &g_tUIAllData.lightning2_model);
    page_maneger_register(&page_tv,                 &g_tUIAllData.tv2_model);
    page_maneger_register(&page_fire,               &g_tUIAllData.fire2_model);    
    page_maneger_register(&page_copcar,             &g_tUIAllData.copcar2_model);
    page_maneger_register(&page_colorchase,         &g_tUIAllData.colorchase_model);
    page_maneger_register(&page_clublights,         &g_tUIAllData.clublights_model);
    page_maneger_register(&page_candle,             &g_tUIAllData.candle_model);   
    page_maneger_register(&page_cct_limit_settiing, &g_tUIAllData.tv2_model.mode_arg);    
    page_maneger_register(&page_hsi_limit_setting,  &g_tUIAllData.tv2_model.mode_arg);  
    page_maneger_register(&page_welding,            &g_tUIAllData.welding2_model);
    page_maneger_register(&page_partylight,         &g_tUIAllData.partylight_model);
#endif   
    page_maneger_register(&page_fireworks,          &g_tUIAllData.fireworks_model);
    page_maneger_register(&page_paparazzi,          &g_tUIAllData.paparazzi_model);
    page_maneger_register(&page_outputmode,         &g_tUIAllData.sys_menu_model);   
	page_maneger_register(&page_high_spd_mode,      &g_tUIAllData.sys_menu_model);
    page_maneger_register(&page_language,           &g_tUIAllData.sys_menu_model);   
    page_maneger_register(&page_dmxmode,            &g_tUIAllData.sys_menu_model);   
    page_maneger_register(&page_dimmingcurve,       &g_tUIAllData.sys_menu_model);       
    page_maneger_register(&page_fanmode,            &g_tUIAllData.sys_menu_model);       
    page_maneger_register(&page_frqsel,             &g_tUIAllData.sys_menu_model);       
    page_maneger_register(&page_productinfo,        &g_tUIAllData.sys_menu_model);       
    page_maneger_register(&page_ctrl_sys,           &g_tUIAllData.sys_menu_model);   
    page_maneger_register(&page_update,             &g_tUIAllData.sys_menu_model);       
    page_maneger_register(&page_customfx,           &g_tUIAllData.sys_menu_model);       
    page_maneger_register(&page_studiomode,         &g_tUIAllData.sys_menu_model);  
    page_maneger_register(&page_factory_reset,      &g_tUIAllData.sys_menu_model);
    page_maneger_register(&page_workmode,           &g_tUIAllData.sys_menu_model);  
    page_maneger_register(&page_battery_option,     &g_tUIAllData.sys_menu_model);  
    page_maneger_register(&page_overheat,           &g_tUIAllData.sys_menu_model); 
    page_maneger_register(&page_uitest,             g_ptUIStateData);  
    page_maneger_register(&page_siduspro,           NULL);  
    page_maneger_register(&page_screensavar,        &g_tUIAllData.sys_menu_model);
    page_maneger_register(&page_elect_accessory,    g_ptUIStateData);
    page_maneger_register(&page_calibration_mode,   g_ptUIStateData);
    
    //init the page pool
    page_pool_init(&my_page_pool, my_page_dsc, GET_ARRAY_SIZE(my_page_dsc));

#ifdef UI_PLATFORM_PC        
    ui_restore_def_setting();   //reset ui setting
#endif    
    
    restore_last_state();
}   

/**
 * @brief 判断当前页面ID是否需要自动开机.
 * 
 * @return true 
 * @return false 
 */
bool screen_auto_power_on(void)
{
    pm_data_t* p_data = ui_get_pm_data();
    bool bret = false;

    if (PAGE_DMX_MODE == p_data->last_id)
        bret = true;
    
    return bret;
}

/**
 * @brief 页面管理器运行
 * 
 */
void page_manager_process(void* t)
{
    static uint32_t runtime = 0;
    bool enbale_time_upd = false;

    if (last_page != cur_page)
    {            
        page_run_exit_cb(last_page);                
        page_run_init_cb(cur_page);

        if (last_page != NULL)
        {
            prev_pid = last_page->id;
        }

        last_page = cur_page; 
        runtime = 0;         
    }
    else if (cur_page)
    {
        if (cur_page->subid != cur_page->next_subid)
        {
            page_run_exit_cb(cur_page); //释放上一次的子页面资源
            cur_page->subid = cur_page->next_subid;            
            page_run_init_cb(cur_page); //初始化新的页面资源
            runtime = 0;        
        }        
    }
    
    enbale_time_upd = user_interact_with_ui();    
    if (enbale_time_upd)
    {
        runtime += ((lv_timer_t*)t)->period;
        if (runtime > PAGE_TIMER_UPD_PERIOD_MS)
        {
            runtime = 0;
            page_run_time_upd_cb(cur_page, PAGE_TIMER_UPD_PERIOD_MS);        
        }
    }
}

/**
 * @brief 获取顶层ID.
 * 
 * @return uint8_t 
 */
uint8_t page_get_stacktop_id(void)
{
    page_dsc_t* p_dsc = page_pool_peek(&my_page_pool, 0);

    if (p_dsc != NULL)
        return p_dsc->pid;

    return 0;
}

/**
 * @brief 压栈跳转子页面
 * 
 * @param sid 
 */
void screen_load_sub_page_with_stack(uint8_t sid, uint8_t index)
{
    cur_page->index = index;
    if (cur_page->subid != sid)
    {
        turn_to_next_page(cur_page->id, sid, index, true);
    }
}

/**
 * @brief 默认情况下是0, 0
 * 
 * @param def_spid  0 
 * @param def_index  0 
 * @return true 
 * @return false 
 */
bool screen_load_prev_sub_page(uint8_t def_spid, uint8_t def_index)
{
    page_dsc_t dsc;
    bool ret = page_pool_pop(&my_page_pool, &dsc);
    
    if (ret) 
        turn_to_next_page(dsc.pid, dsc.spid, dsc.index, false);
    else 
        turn_to_next_page(cur_page->id, def_spid, def_index, false);

    return ret;
}

/**
* @brief 强制切换到下一个页面
 * 
 * @param id 
 * @param subid 
 * @param index 
 * @param b_record 
 */
void foce_turn_to_next_page(uint8_t id, uint8_t subid, bool b_record)
{
    page_t* tmp;
    page_dsc_t dsc;

    list_for_each_entry(tmp, page_head) {
        //根据ID找到当前页面
        if (tmp->id == id)
        {
            //判断是否需要记录
            if (b_record && (cur_page != NULL))
            {
                PAGE_DSC_INITIALIZER(&dsc, cur_page->id, cur_page->subid, cur_page->index);
            }
            
            if (cur_page != tmp)
            {                
                cur_page = tmp;
                cur_page->subid = subid;    
                cur_page->next_subid = subid;
            }
            else if (subid != tmp->subid)
            {                
                tmp->next_subid = subid;                  
            }

            cur_page->index = cur_page->index;
            //将页面添加进入页面池
            if (b_record)
                page_pool_push(&my_page_pool, &dsc);

            maintain_pm_data(cur_page->id, subid);            
            break;
        }
    }

    UI_PRINTF("----------------------------------\r\n");
    page_pool_print(&my_page_pool);

    uint8_t top = page_pool_get_top(&my_page_pool);
    for (uint8_t i = 0; i < top; i++)
    {
        page_dsc_t* p_dsc_tmp = page_pool_peek(&my_page_pool, i);
        if (p_dsc_tmp != NULL && (p_dsc_tmp->spid == subid && p_dsc_tmp->pid == id))
        {
            page_pool_set_top(&my_page_pool, i);
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * @brief 切换到下一个页面
 * 
 * @param id 
 * @param subid 
 * @param index 
 * @param b_record 
 */
static void turn_to_next_page(uint8_t id, uint8_t subid, uint8_t index, bool b_record)
{
    page_t* tmp;
    page_dsc_t dsc;

    if (cur_page != NULL && cur_page->id == id && cur_page->subid == subid) {
        return;
    }

    list_for_each_entry(tmp, page_head) {
        //根据ID找到当前页面
        if (tmp->id == id)
        {
            //判断是否需要记录
            if (b_record && (cur_page != NULL))
            {
                PAGE_DSC_INITIALIZER(&dsc, cur_page->id, cur_page->subid, cur_page->index);
            }
            
            if (cur_page != tmp)
            {                
                cur_page = tmp;
                cur_page->subid = subid;    
                cur_page->next_subid = subid;
            }
            else if (subid != tmp->subid)
            {                
                tmp->next_subid = subid;                  
            }

            cur_page->index = index;
            //将页面添加进入页面池
            if (b_record)
                page_pool_push(&my_page_pool, &dsc);

            maintain_pm_data(cur_page->id, subid);            
            break;
        }
    }

    UI_PRINTF("----------------------------------\r\n");
    page_pool_print(&my_page_pool);

    uint8_t top = page_pool_get_top(&my_page_pool);
    for (uint8_t i = 0; i < top; i++)
    {
        page_dsc_t* p_dsc_tmp = page_pool_peek(&my_page_pool, i);
        if (p_dsc_tmp != NULL && (p_dsc_tmp->spid == subid && p_dsc_tmp->pid == id))
        {
            page_pool_set_top(&my_page_pool, i);
        }
    }

}

/**
 * @brief 运行页面初始化函数
 * 
 * @param page 
 */
static void page_run_init_cb(page_t *page)
{
    if (page && page->init_cb)
    {
        ui_update_light_mode(); //暂时先放在这里...
        page->init_cb();
    }
}

/**
 * @brief 退出界面
 * 
 * @param page 
 */
static void page_run_exit_cb(page_t *page)
{
    if (page && page->exit_cb)
    {
        page->exit_cb();
    }
}

/**
 * @brief 调用定时刷新函数
 * 
 * @param page 
 * @param time_ms 
 */
static void page_run_time_upd_cb(page_t *page, uint32_t time_ms)
{
    if (page && page->time_upd_cb)
    {
        page->time_upd_cb(time_ms);
    }
}

/**
 * @brief 恢复上一次的状态
 * 
 */
static void restore_last_state(void)
{
//    pm_data_t* p_data = ui_get_pm_data();
    uint8_t pid = PAGE_LIGHT_MODE_CCT;
    uint8_t spid = 0;
//    
//    if (PAGE_DMX_MODE == p_data->last_id)
//    {
//        pid = p_data->last_id;
//        spid = p_data->page_spid;     
//    }
//    else if (PAGE_CUSTOM_FX == p_data->last_id && SUB_ID_FX_INTENSITY == p_data->page_spid)
//    {
//        pid = p_data->last_id;
//        spid = SUB_ID_FX_INTENSITY;
//    }
//    else if (PAGE_LIGHT_EFFECT_MENU == p_data->last_id)
//    {
//        pid = p_data->last_id;        
//    }
//    else 
//    {
//        if ((p_data->top_id >= PAGE_LIGHT_MODE_HSI && p_data->top_id < PAGE_LIGHT_EFFECT_MENU) || \
//                            (p_data->top_id >= PAGE_PULSING && p_data->top_id <= PAGE_PARTYLIGHT) || \
//                                (p_data->top_id == PAGE_SIDUS_PRO_FX))        
//        {
//            pid = p_data->top_id; 
//        }
//        else
//        {
//            pid = PAGE_LIGHT_MODE_CCT;
//            p_data->top_id = PAGE_LIGHT_MODE_CCT;
//        }
//    }
    data_center_read_config_data(SYS_CONFIG_LOCAL_UI, &pid);
    data_center_read_config_data(SYS_CONFIG_LOCAL_SUB_UI, &spid);
    
    if(pid == PAGE_CUSTOM_FX && spid == SUB_ID_FX_INTENSITY)
    {
		extern void sys_dmx_power_on_set(uint8_t mode);
		
		sys_dmx_power_on_set(1);
		ui_set_trigger_cfx_state(1);
        spid = SUB_ID_CUSTOM_FX_LOADING;
    }
    else if(pid == PAGE_CUSTOM_FX && spid != SUB_ID_FX_INTENSITY)
    {
        pid = PAGE_LIGHT_MODE_CCT;
        spid = 0;
    }
	else if(pid == PAGE_DMX_MODE)
    {
        pid = PAGE_DMX_MODE;
    }
	if(pid <= PAGE_MENU || pid >= PAGE_TEST)
        pid = PAGE_LIGHT_MODE_CCT;
    prev_pid = pid;
    screen_load_page(prev_pid, spid, false);
    //screen_load_page(PAGE_MENU, 0, false);
    //screen_load_page(PAGE_BATTERY_OPTION, SUB_ID_BATTERY_NOT_SUPPORT, false);
    //screen_load_page(PAGE_BATTERY_OPTION, SUB_ID_FULL_POWER_CONFIRM, false);
    //screen_load_page(PAGE_BATTERY_OPTION, SUB_ID_ACTIVATE_BATT_CONFIRM, false);
    //screen_load_page(PAGE_FREQUENCY_SELECTION, 0, false);
    //screen_load_page(PAGE_LIGHT_MODE_SOURCE, 0, false);
    //screen_load_page(PAGE_UPDATE, SUB_ID_UPDATE_FAILED, false);
    //screen_load_page(PAGE_UPDATE, SUB_ID_UPDATE_OK, false);
    //screen_load_page(PAGE_UPDATE, SUB_ID_U_DISK_FORMAT_ERR, false);
    //screen_load_page(PAGE_CONTROL_SYSTEM, SUB_ID_BLE_RESET_OK, false);
    //screen_load_page(PAGE_CONTROL_SYSTEM, SUB_ID_BLE_RESET_FAILED, false);
    //screen_load_page(PAGE_CONTROL_SYSTEM, SUB_ID_LUMENRADIO_PAIR_SUCCEED, false);
    //screen_load_page(PAGE_CONTROL_SYSTEM, SUB_ID_LUMENRADIO_PAIR_FAILED, false);
    //screen_load_page(PAGE_CUSTOM_FX, SUB_ID_FX_INTENSITY, false);
    //screen_load_page(PAGE_CCT_LIMIT_SETTING, 0, 0);
    //screen_load_page(PAGE_CUSTOM_FX, SUB_ID_FX_INTENSITY, false);
    //screen_load_page(PAGE_OVERHEAT, 0, false);
    //screen_load_page(PAGE_LIGHT_MODE_XY, 0, false);
    //screen_load_page(PAGE_DMX_MODE, 0, false);
}

/**
 * @brief 维护页面管理器保存数据
 * 
 * @param cur_id 
 */
static void maintain_pm_data(uint8_t cur_id, uint8_t spid)
{
    pm_data_t* p_data = ui_get_pm_data();

    /* 记录最后一次所在ID */
    p_data->last_id = cur_id;

    /* 记录最顶层的ID */
    if ((cur_id >= PAGE_LIGHT_MODE_HSI && cur_id < PAGE_LIGHT_EFFECT_MENU) || \
                        (cur_id >= PAGE_PULSING && cur_id <= PAGE_PARTYLIGHT) || \
							 (cur_id == PAGE_SIDUS_PRO_FX) ||\
                                (cur_id == PAGE_CUSTOM_FX)) 
    {
        p_data->top_id = cur_id;    
    }

    /* 记录当前页面ID和子ID. */
    if ((cur_id == PAGE_DMX_MODE || cur_id == PAGE_CUSTOM_FX) || \
                (cur_id >= PAGE_LIGHT_MODE_HSI && cur_id <= PAGE_LIGHT_EFFECT_MENU) || \
                        (cur_id >= PAGE_PULSING && cur_id <= PAGE_PARTYLIGHT))
    {
        if (p_data->page_id != cur_id || p_data->page_spid != spid)
        {
            p_data->page_id = cur_id;
            if (cur_id == PAGE_DMX_MODE || cur_id == PAGE_CUSTOM_FX)//DMX模式
            {
                p_data->page_spid = spid;
            }
        }
    }
}
