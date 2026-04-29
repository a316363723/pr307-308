

#ifdef DB_ENUM
    #undef DB_DEFINE
    #define DB_DEFINE(enum_name, struct_name, name, ...)       enum_name,
#endif

#ifdef DB_STRUCT
    #undef DB_DEFINE
    #define DB_DEFINE(enum_name, struct_name, name, ...)       struct_name name __VA_ARGS__;
#endif

#ifdef DB_MEM_INFO
    #undef DB_DEFINE
    #ifndef DB_CONTAINER_STRUCT
        #error "请定父容器结构体"
    #endif

    #define DB_DEFINE(enum_name, struct_name, name, ...)       {offsetof(DB_CONTAINER_STRUCT, name), sizeof(((DB_CONTAINER_STRUCT *)0)->name)},
#endif

/* 后续添加本地掉电保存数据请在最后面添加，请勿在中间或者前面添加，也不要删除 */
DB_DEFINE(SYS_CONFIG_LOCAL_UI,             uint8_t,    local_ui                    )
DB_DEFINE(SYS_CONFIG_LOCAL_SUB_UI,         uint8_t,    local_sub_ui                )
DB_DEFINE(SYS_CONFIG_OUTPUT_MODE,          uint8_t,    output_mode                 )
DB_DEFINE(SYS_CONFIG_LANG,                 uint8_t,    lang                        )
DB_DEFINE(SYS_CONFIG_SCREEN_SAVER,         uint8_t,    screensaver_index           )
DB_DEFINE(SYS_CONFIG_CURVE_TYPE,           uint8_t,    curve_type                  )
DB_DEFINE(SYS_CONFIG_DMX_LOCKED,           uint8_t,    dmx_locked                  )
DB_DEFINE(SYS_CONFIG_DMX_ADDR,             uint16_t,   dmx_addr                    )
DB_DEFINE(SYS_CONFIG_DMX_LOSS_BEHAVIOR,    uint8_t,    dmx_loss_behavior           )
DB_DEFINE(SYS_CONFIG_DMX_PROFILE,          uint8_t,    dmx_profile_index           )
DB_DEFINE(SYS_CONFIG_DMX_EXTENSION,        uint8_t,    dmx_extension_index         )
DB_DEFINE(SYS_CONFIG_DMX_TEMINATION,       uint8_t,    dmx_temination_on           )
DB_DEFINE(SYS_CONFIG_DMX_FADETIME,         uint16_t,   dmx_fade_time               )
DB_DEFINE(SYS_CONFIG_FAN_MODE,             uint8_t,    fan_mode                    )
DB_DEFINE(SYS_CONFIG_LIGHT_EFFECT_IDX,     uint8_t,    light_effect_index          )
DB_DEFINE(SYS_CONFIG_FRQ,                  uint16_t,   frequency                   )
DB_DEFINE(SYS_CONFIG_LOCAL_DATA_SIZE,      uint16_t,   local_data_size             )
DB_DEFINE(SYS_CONFIG_BLE_CTRL,             uint8_t,    ble_on                      )
DB_DEFINE(SYS_CONFIG_CRMX_CTRL,            uint8_t,    crmx_on                     )
DB_DEFINE(SYS_CONFIG_STUDIO_MODE,          uint8_t,    studiomode_on               )
DB_DEFINE(SYS_CONFIG_WORK_MODE,            uint8_t,    work_mode                   )
DB_DEFINE(SYS_CONFIG_LOCAL_IP,             uint8_t,    local_ip,           [4]     )
DB_DEFINE(SYS_CONFIG_NETMASK,              uint8_t,    netmask,            [4]     )
DB_DEFINE(SYS_CONFIG_GATEWAY,              uint8_t,    gateway,            [4]     )
DB_DEFINE(SYS_CONFIG_CUSTOM_FX_TYPE,       uint8_t,    custom_fx_type              )
DB_DEFINE(SYS_CONFIG_CUSTOM_FX_SEL,        uint8_t,    custom_fx_sel,      [3]     )
DB_DEFINE(SYS_CONFIG_BLE_SN,               uint8_t,    ble_sn_number,      [6]     )
DB_DEFINE(SYS_CONFIG_HIGH_SPEED_MODE,      uint8_t,    hs_mode,                    )
DB_DEFINE(SYS_CONFIG_YOKE_PAN_LIMIT,       int16_t,   yoke_pan_limit,     [2]     )
DB_DEFINE(SYS_CONFIG_YOKE_TILT_LIMIT,      int16_t,   yoke_tilt_limit,    [2]     )
DB_DEFINE(SYS_CONFIG_YOKE_PAN,             float,   yoke_pan,                   )
DB_DEFINE(SYS_CONFIG_YOKE_TILT,            float,   yoke_tilt,                  )
DB_DEFINE(SYS_CONFIG_FRESNEL_ANGLE,        float,   fresnel_angle,              )
DB_DEFINE(SYS_CONFIG_ANGLE_LIMIT,          uint8_t,    angle_limit_starte,         )
DB_DEFINE(SYS_CONFIG_NOT_ACCESS_SELECT,    uint8_t,    access_select,              )
DB_DEFINE(SYS_CONFIG_YOKE_NO_PAN_LIMIT,    int16_t,   yoke_pan_limit1,     [12][2] )
DB_DEFINE(SYS_CONFIG_YOKE_NO_TILT_LIMIT,   int16_t,   yoke_tilt_limit1,    [12][2] )
DB_DEFINE(SYS_CONFIG_YOKE_NO_PAN,          float,   yoke_pan1,           [12]    )
DB_DEFINE(SYS_CONFIG_YOKE_NO_TILT,         float,   yoke_tilt1,          [12]    )
DB_DEFINE(SYS_CONFIG_NO_ANGLE_LIMIT,       uint8_t,    angle_limit_starte1, [12]    )
DB_DEFINE(SYS_CONFIG_EFFECTS_MODE,         uint8_t,    effects_mode,               )
DB_DEFINE(SYS_CONFIG_ETH_STATE_SET,        uint8_t,    eth_state_set,              )
DB_DEFINE(SYS_CONFIG_ETH_ARTNET_UNIVERSE,  uint16_t,   eth_artnet_universe,        )
DB_DEFINE(SYS_CONFIG_ETH_SACN_UNIVERSE,    uint16_t,   eth_sacn_universe,          )
DB_DEFINE(SYS_CONFIG_ETH_START,            uint8_t,    eth_start,                  )
DB_DEFINE(SYS_CONFIG_OUTPUT_POWER_VALUE,   uint16_t,   power_value,          )
DB_DEFINE(SYS_CONFIG_OUTPUT_POWER_LIMIT,   uint8_t,    power_limit,          )
DB_DEFINE(SYS_CONFIG_DEMO_STATE,           uint8_t,    demo_state,          )
DB_DEFINE(SYS_CONFIG_PAN_DEMO_STATE,       uint8_t,    pan_demo_state,          )
DB_DEFINE(SYS_CONFIG_PAN_DEMO_LIMIT,       int16_t,    pan_demo_limit, [2]          )
DB_DEFINE(SYS_CONFIG_TILI_DEMO_STATE,      uint8_t,    tili_demo_state,          )
DB_DEFINE(SYS_CONFIG_TILI_DEMO_LIMIT,      int16_t,    tili_demo_limit, [2]          )
DB_DEFINE(SYS_CONFIG_ZOOM_DEMO_STATE,      uint8_t,    zoom_demo_state,          )
DB_DEFINE(SYS_CONFIG_ZOOM_DEMO_LIMIT,      int16_t,    zoom_demo_limit, [2]          )
DB_DEFINE(SYS_CONFIG_DEMO_POWER_ON_LIMIT,  uint8_t,    power_on_state,          )
//DB_DEFINE(SYS_CONFIG_TEST,  uint8_t,    test111,          )
DB_DEFINE(SYS_CONFIG_ALL,                  uint8_t,    placeholder,        [0]     ) //结构体中占位符不占空间 枚举用于读取全部数据
DB_DEFINE(SYS_CONFIG_ACC_RECOGNITION,      uint8_t,    acc_recognition            )

#ifdef DB_STRUCT
    #undef DB_STRUCT
#endif

#ifdef DB_ENUM
    #undef DB_ENUM
#endif

#ifdef DB_MEM_INFO
    #undef DB_MEM_INFO
#endif

#ifdef DB_CONTAINER_STRUCT
    #undef DB_CONTAINER_STRUCT
#endif

#ifdef DB_DEFINE
    #undef DB_DEFINE
#endif
