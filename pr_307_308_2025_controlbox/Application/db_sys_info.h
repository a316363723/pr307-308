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

DB_DEFINE(SYS_INFO_POWER,        struct sys_info_power,        power)
DB_DEFINE(SYS_INFO_USB,          struct sys_info_usb,          usb)
DB_DEFINE(SYS_INFO_ETH,          struct sys_info_eth,          eth)
DB_DEFINE(SYS_INFO_LAMP,         struct sys_info_lamp,         lamp)
DB_DEFINE(SYS_INFO_BLE,          struct sys_info_ble,          ble)
DB_DEFINE(SYS_INFO_CRMX,         struct sys_info_crmx,         crmx)
DB_DEFINE(SYS_INFO_DMX,          struct sys_info_dmx,          dmx)
DB_DEFINE(SYS_INFO_CTRL,         struct sys_info_ctrl_box,     ctrl_box)
DB_DEFINE(SYS_INFO_ACCESS,       struct sys_info_accessories,  access_info)
DB_DEFINE(SYS_INFO_MOTOR,        struct sys_info_motor,        motor_angle_info)
DB_DEFINE(SYS_INFO_MOTOR_STATE,  struct sys_info_motor_state,  motor_state_info)
DB_DEFINE(SYS_INFO_MOTOR_TEST,   struct sys_info_motor_test,   motor_test_info)
DB_DEFINE(SYS_INFO_CFX_NAME,     struct sys_info_cfx_name,     cfx_name )
DB_DEFINE(SYS_INFO_ERROR_TYPE,   sys_error_type,          	   error_type)	
DB_DEFINE(SYS_INFO_UUID,         struct sys_info_uuid,         uuid)
DB_DEFINE(SYS_INFO_WORK_UPDATE,  struct sys_info_update_name,  update_name)


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
