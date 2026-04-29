
# 一、使用配置

`1.色温配置（光效和光模式统一共用） `  

    宏定义 DMX_CCT_MIN         (2000UL)    
    宏定义 DMX_CCT_MAX         (10000UL)  
    使用说明：
        常规光模式和光效色温范围大小选择
`2.常规光模式GM配置`  

    宏定义 DMX_DUV_MIN         ((float)-0.02f)  
    宏定义 DMX_DUV_MAX         ((float)0.02f)  
    使用说明：
        常规光模式GM温范围大小选择
`3.光效GM配置`  

    宏定义 DMX_FX_GM_MIN       ((int8_t)-100)  
    宏定义 DMX_FX_GM_MAX       ((int8_t)100) 
    使用说明：
        光效模式GM温范围大小选择
`4.dmx_0709_fx_pixel_8_bit颜色种类选择定义`  

    宏定义 PRODUCT_MT_PRO      0  
    宏定义 PRODUCT_APUTURE_S1C 1  
    宏定义 PRODUCT_APUTURE_S2C 2  
    宏定义 PRODUCT_APUTURE_S4C 3  
    宏定义 DMX_FX_PIXEL_CFG    (PRODUCT_MT_PRO)
    使用说明：
        DMX_FX_PIXEL_CFG值可为 0-3 表示不同颜色种类表格选择
`5.分屏数量`  

    宏定义 PROFILE_LIGHT_ENGINE_NUM   N  
    使用说明：
        PROFILE_LIGHT_ENGINE_NUM  假设N值为2 表示使用2分屏
`6.ULTIMATE灯光模式选择` 

    宏定义 DMX_ULTIMATE_RGB_PROFILE_CFG_CCT_RGB    (0x08xx)  
    宏定义 DMX_ULTIMATE_RGB_PROFILE_CFG_RGB        (0x02xx)  
    宏定义 DMX_ULTIMATE_RGB_PROFILE_CFG_HSI        (0x03xx)  
    宏定义 DMX_ULTIMATE_RGB_PROFILE_CFG_FX         (0x07xx)
    使用说明：
        DMX_ULTIMATE_RGB_PROFILE_CFG_CCT_RGB 的值可为 0x801-0x808 可选：08XX 8\16-bit全系列
        DMX_ULTIMATE_RGB_PROFILE_CFG_RGB  的值可为 0x0201-0x208 可选：02XX 8\16-bit全系列
        DMX_ULTIMATE_RGB_PROFILE_CFG_HSI 的值可为 0x0301-0x304  可选：03XX 8\16-bit全系列
        DMX_ULTIMATE_RGB_PROFILE_CFG_FX  的值可为 0x0705-0x707  可选：07XX 8\16-bit全系列
`7.Profile选择 `

    宏定义 DMX_PROFILE_xxxx_EN 1 
    使用说明
        DMX_PROFILE_xxxx_EN 当值为1，对应XXX Profile解析函数将会被注册，表示支持此XXX profile 
        DMX_PROFILE_xxxx_EN 当值为0，对应XXX Profile解析函数不被注册， 表示不支持此XXX profile 


# 二、对外接口说明

1. profile执行解析DMX数据接口

```C
/**
 * @brief   profile执行解析DMX数据接口
 * @param   id  根据id 值选择执行的profile解析函数 
 * @param   profile_data  传入未解析的DMX数据
 * @param   dmx_data      输出解析后DMX数据结构
 * @param   fan_data      输出解析后的风扇数据
 * @retval  常规模式返回是否选择频闪 光效返回光效执行状态        
**/
uint8_t dmx_profile_analysis(dmx_profile_id id, const uint8_t* profile_data, dmx_data_t* dmx_data, dmx_fan_t* fan_data);
```
