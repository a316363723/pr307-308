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
DB_DEFINE(LIGHT_MODE_CCT,                           struct db_cct,                  cct_arg)
DB_DEFINE(LIGHT_MODE_HSI,                           struct db_hsi,                  hsi_arg)
DB_DEFINE(LIGHT_MODE_GEL,                           struct db_gel,                  gel_arg)
DB_DEFINE(LIGHT_MODE_RGB,                           struct db_rgb,                  rgb_arg)
DB_DEFINE(LIGHT_MODE_RGBWW,                         struct db_rgbww,                rgbww_arg)
DB_DEFINE(LIGHT_MODE_SOURCE,                        struct db_source,               source_arg)
DB_DEFINE(LIGHT_MODE_XY,                       		struct db_xy,                   xy_arg)
DB_DEFINE(LIGHT_MODE_FX_CLUBLIGHTS,                 struct db_fx_club_lights,       club_lights)
DB_DEFINE(LIGHT_MODE_FX_PAPARAZZI,                  struct db_fx_paparazzi,         paparazzi)
DB_DEFINE(LIGHT_MODE_FX_LIGHTNING,                  struct db_fx_lightning,         lightning)
DB_DEFINE(LIGHT_MODE_FX_TV,                         struct db_fx_tv,                tv)
DB_DEFINE(LIGHT_MODE_FX_CANDLE,                     struct db_fx_candle,            candle)
DB_DEFINE(LIGHT_MODE_FX_FIRE,                       struct db_fx_fire,              fire)
DB_DEFINE(LIGHT_MODE_FX_STROBE,                     struct db_fx_strobe,            strobe)
DB_DEFINE(LIGHT_MODE_FX_EXPLOSION,                  struct db_fx_explosion,         explosion)
DB_DEFINE(LIGHT_MODE_FX_FAULT_BULB,                 struct db_fx_fault_bulb,        fault_bulb)
DB_DEFINE(LIGHT_MODE_FX_PULSING,                    struct db_fx_pulsing,           pulsing)
DB_DEFINE(LIGHT_MODE_FX_WELDING,                    struct db_fx_welding,           welding)
DB_DEFINE(LIGHT_MODE_FX_COP_CAR,                    struct db_fx_cop_car,           cop_car)
DB_DEFINE(LIGHT_MODE_FX_COLOR_CHASE,                struct db_fx_color_chase,       color_chase)
DB_DEFINE(LIGHT_MODE_FX_PARTY_LIGHTS,               struct db_fx_party_lights,      party_lights)
DB_DEFINE(LIGHT_MODE_FX_FIREWORKS,                  struct db_fx_fireworks,         fireworks)
DB_DEFINE(LIGHT_MODE_FX_PAPARAZZI_II,               struct db_fx_paparazzi_2,       paparazzi_2)
DB_DEFINE(LIGHT_MODE_FX_LIGHTNING_II,               struct db_fx_lightning_2,       lightning_2)
DB_DEFINE(LIGHT_MODE_FX_TV_II,                      struct db_fx_tv_2,              tv_2)
DB_DEFINE(LIGHT_MODE_FX_FIRE_II,                    struct db_fx_fire_2,            fire_2)
DB_DEFINE(LIGHT_MODE_FX_STROBE_II,                  struct db_fx_strobe_2,          strobe_2)
DB_DEFINE(LIGHT_MODE_FX_EXPLOSION_II,               struct db_fx_explosion_2,       explosion_2)
DB_DEFINE(LIGHT_MODE_FX_FAULT_BULB_II,              struct db_fx_fault_bulb_2,      fault_bulb_2)
DB_DEFINE(LIGHT_MODE_FX_PULSING_II,                 struct db_fx_pulsing_2,         pulsing_2)
DB_DEFINE(LIGHT_MODE_FX_WELDING_II,                 struct db_fx_welding_2,         welding_2)
DB_DEFINE(LIGHT_MODE_FX_COP_CAR_II,                 struct db_fx_cop_car_2,         cop_car_2)
DB_DEFINE(LIGHT_MODE_FX_PARTY_LIGHTS_II,            struct db_fx_party_lights_2,    party_lights_2)
DB_DEFINE(LIGHT_MODE_FX_FIREWORKS_II,               struct db_fx_fireworks_2,       fireworks_2)
DB_DEFINE(LIGHT_MODE_FX_LIGHTNING_III,              struct db_fx_lightning_3,       lightning_3)
DB_DEFINE(LIGHT_MODE_FX_TV_III,                     struct db_fx_tv_3,              tv_3)
DB_DEFINE(LIGHT_MODE_FX_FIRE_III,                   struct db_fx_fire_3,            fire_3)
DB_DEFINE(LIGHT_MODE_FX_FAULTY_BULB_III,            struct db_fx_fault_bulb_3,      fault_bulb_3)
DB_DEFINE(LIGHT_MODE_FX_PULSING_III,                struct db_fx_pulsing_3,         pulsing_3)
DB_DEFINE(LIGHT_MODE_FX_COP_CAR_III,                struct db_fx_cop_car_3,         cop_car_3)
DB_DEFINE(LIGHT_MODE_SIDUS_MFX,                     struct db_manual_fx,            mfx)
DB_DEFINE(LIGHT_MODE_SIDUS_PFX,                     struct db_program_fx,           pfx)
DB_DEFINE(LIGHT_MODE_SIDUS_CFX,                     struct db_custom_fx,            cfx)
DB_DEFINE(LIGHT_MODE_SIDUS_CFX_PREVIEW,             struct db_custom_preview_fx,    cfx_preview)
DB_DEFINE(LIGHT_MODE_LOCAL_CFX,                     struct db_local_custom_fx,      local_cfx_arg)
DB_DEFINE(LIGHT_MODE_PIXEL_FX_COLOR_FADE,           struct db_color_fade,           color_fade)
DB_DEFINE(LIGHT_MODE_PIXEL_FX_COLOR_CYCLE,          struct db_color_cycle,          color_cycle)
DB_DEFINE(LIGHT_MODE_PIXEL_FX_RAINBOW,              struct db_rainbow,              rainbow) 
DB_DEFINE(LIGHT_MODE_PIXEL_FX_ONE_COLOR_CHASE,      struct db_one_color_chase,      one_color_chas)
DB_DEFINE(LIGHT_MODE_PIXEL_FX_TWO_COLOR_CHASE,      struct db_two_color_chase,      two_color_chas)  
DB_DEFINE(LIGHT_MODE_PIXEL_FX_THREE_COLOR_CHASE,    struct db_three_color_chase,    three_color_ch) 
DB_DEFINE(LIGHT_MODE_PIXEL_FX_FIRE,                 struct db_pixel_fire,           pixel_fire) 
DB_DEFINE(LIGHT_MODE_PARTITION_DATA,                struct db_partition_data,       partition_data) 
DB_DEFINE(LIGHT_MODE_PARTITION_FX,                  struct db_partition_fx,         partition_fx) 
DB_DEFINE(LIGHT_MODE_PARTITION_CFG,                 struct db_partition_cfg,        partition_cfg) 
// 无需保存数据类型
DB_DEFINE(LIGHT_MODE_DMX, struct db_dmx, dmx)
// LIGHT_MODE_FACTORY_PWM一定要作为最后一个元素
DB_DEFINE(LIGHT_MODE_FACTORY_PWM, struct db_factory_pwm, factory_pwm)

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
