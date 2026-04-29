#ifndef PROJECT_CONGIF_H
#define PROJECT_CONGIF_H


#define LAMP_SOFTWART_VER		(0X03)
#define LAMP_HARDWARE_VER		(0X11)
#define UPGRADE_FILE_NAME_LEN   (9)

#define LAMP_FREQUENCY_DIV       (5) //80K/20K 实际80k,虚拟为20k精度;125k/25k
#define LAMP_DRV_ONE_CHANNEL_NUM (3)
#define LAMP_DRV_TWO_CHANNEL_NUM (4)
/*----------------------------fan-smart-mode------------------------------
    fan_spped
      / \
       |
       |
       |
       |
	   |
  2400 |-------------------------------------------*
       |                                        *  
	   |                                      *    |
	   |                                   *       
  2000 |--------------------------------*          |
	   |                              * |          
	   |                            *   |          |
	   |                          *     |
  1600 |------------------------*       |          |
	   |                     *  |       |
	   |                  *     |       |          |
	   |               *        |       |      
	   |            *           |       |          |
       |         *              |       |
	   |      *                 |       |          |
   900 |---*                    |       |  
       |________________________|_______|__________|______________\
	      35                    77       86        95             /
																Temp
----------------------------------------------------------------------*/

#ifdef PR_308_L2

//升级文件名信息
#define UPGRADE_FILE_NAME       "PR_308_L2"
#define PRODUCT_TYPE_CODE       (0X0010)  

#define LED_LIGHT_NUM            3
#define PWM_POWER                2600 //风扇静音模式cob输出限制在550W：550/1600 = 0.34375
#define ANA_POWER                2600
#define SILENCE_MODE_POWER       550

#define PUMP_NORMAL_SPEED        6000
#define PUMP_SILENT_SPEED        4000

#define FAN_HIGH_SPEED           3300
#define FAN_MIDDLE_SPEED         2350
#define FAN_SILENT_SPEED         900

#define FAN_SMART_SPEED_ONE     3500
#define FAN_SMART_TEMP_ONE      95
#define FAN_SMART_SPEED_TWO     3200
#define FAN_SMART_TEMP_TWO      88
#define FAN_SMART_SPEED_THR     2350
#define FAN_SMART_TEMP_THR      82
#define FAN_SMART_SPEED_FOU     1200
#define FAN_SMART_TEMP_FOU      35

#define MAX_CCT  6500
#define MIN_CCT  2700
#define WITHOUT_REFLECTOR_MAX_CCT  5950
#endif


#ifdef PR_307_L3

//升级文件名信息
#define UPGRADE_FILE_NAME      "PR_307_L3"
#define PRODUCT_TYPE_CODE      (0X0009) 

#define LED_LIGHT_NUM           5
#define PWM_POWER               1600 //风扇静音模式cob输出限制在550W：550/1600 = 0.34375
#define ANA_POWER               1500
#define SILENCE_MODE_POWER      550

#define PUMP_NORMAL_SPEED       4700
#define PUMP_SILENT_SPEED       4000

#define FAN_HIGH_SPEED          2000
#define FAN_MIDDLE_SPEED        1600
#define FAN_SILENT_SPEED        900

#define FAN_SMART_SPEED_ONE     2400
#define FAN_SMART_TEMP_ONE      95
#define FAN_SMART_SPEED_TWO     2000
#define FAN_SMART_TEMP_TWO      86
#define FAN_SMART_SPEED_THR     1600
#define FAN_SMART_TEMP_THR      78
#define FAN_SMART_SPEED_FOU     900
#define FAN_SMART_TEMP_FOU      35

#define MAX_CCT  10000
#define MIN_CCT  2000
#define WITHOUT_REFLECTOR_MAX_CCT  9200
#endif

#endif
