/****************************************************************************************
**  Filename :  dmx_rdm.h
**  Abstract :  This file include API function statement.
**  By       :  HeJianGuo <hejianguo5515@126.com>
**  Date     :  2018-01-23
**  Changelog:1.First Create
*****************************************************************************************/
#ifndef _APP_RDM
#define _APP_RDM

/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "hc32_ddl.h"
#include "project_config.h"
/*****************************************************************************************
								Macro definitions
*****************************************************************************************/
//#define DEBUG									//use Output for debugging
#define F_CPU 16000000

//Default UID;
#define UID_0 0x00								//ESTA device ID High Byt
#define UID_1 0x09								//ESTA device ID Low Byt
#define UID_2 0x01								//Device ID 3 Byt
#define UID_3 0x02								//Device ID 2 Byt                                                                                
#define UID_4 0x03								//Device ID 1 Byt
#define UID_5 0x05								//Device ID 0 Byt

#define F_OSC		    F_CPU/1000		  			//oscillator freq. in kHz (typical 8MHz or 16MHz)
#define RESPONSE_DATA_START_INFO 256		//发送数据在数组中的起始位置，此数组是接收DMX RDM共用，
										 	//而DMX最大514个数据，但不需要应答控制器；而RDM最多254个数据，
										 	//但有应答，因此在RDM通信是，将数组从第256位置的数据序列作为应答数据区。
#define RDM_DATA_NUM        (255)
#define RDM_MIN_BYTE_NUM    (24)
#define RDM_PD_NUM          (RDM_DATA_NUM - RDM_MIN_BYTE_NUM)


#define EEPROM_STATUS 0         // 1 byte, set to 0 if eeprom valid
#define EEPROM_DEVICE_ID 1      // 6 bytes
#define EEPROM_DMX_ADDRESS 7    // 2 bytes
#define EEPROM_LABEL_LEN 9      
#define EEPROM_LABEL 10         // 32 bytes
#define EEPROM_LAMP_HOURS 42    // 4 bytes
#define EEPROM_DEVICE_HOURS 46  // 4 bytes

// RDM CODES (ANSI E1.20)
//command classes
#define DISC_CMD				(0x10)
#define DISC_CMD_RESPONSE		(0x11)
#define GET_CMD					(0x20)
#define GET_CMD_RESPONSE		(0x21)
#define SET_CMD					(0x30)
#define SET_CMD_RESPONSE		(0x31)

// command class defines ??
#define CC_GET					(0x01)
#define CC_SET					(0x02)
#define CC_GET_SET				(0x03)

//response type
#define RESPONSE_TYPE_ACK		(0x00)
#define RESPONSE_TYPE_NACK		(0x02)	//with reason!!

//NACK reasons (MSB is always zero - left out!!)
#define NR_UNKNOWN_PID			(0x00)
#define NR_FORMAT_ERROR			(0x01)
#define NR_HARDWARE_FAULT		(0x02)
#define NR_WRITE_PROTECT		(0x04)
#define NR_UNSUPPORTED_CC		(0x05)
#define NR_DATA_OUT_OF_RANGE	(0x06)
#define NR_BUFFER_FULL			(0x07)
#define NR_PACKET_SIZE_UNSUPPORTED (0x08)
#define NR_NO_SUBDEVICE			(0x09)

//parameter IDs !!bytes are swapped!!
#define DISC_UNIQUE_BRANCH		(0x0001)	//network management
#define DISC_MUTE				(0x0002)
#define DISC_UN_MUTE			(0x0003)
#define IDENTIFY				(0x1000)
#define RESET_DRV			    (0x1001)
#define STATUS_MESSAGES			(0x0030)
#define SUPPORTED_PARAMETERS	(0x0050)	//information
#define PARAMETER_DESCRIPTION	(0x0051)
#define DEVICE_INFO				(0x0060)
#define PRODUCT_DETAIL_ID_LIST	(0x0070)
#define DEVICE_LABEL			(0x0082)
#define MANUFACT_LABEL          (0x0081)
#define DMX_PERSONALITY			(0x00E0)
#define QUEUED_MESSAGES			(0x0020)
#define DMX_PERSONALITY_DESCRIPTION 	(0x00E1)
#define DMX_START_ADDRESS		(0x00F0)	//DMX setup
#define SLOT_INFO               (0x0120)
#define LAMP_HOURS              (0x0401)
#define DEVICE_HOURS            (0x0400)

#define SOFTWARE_VERSION_LABEL	(0x00C0)
#define DEVICE_MODEL_DESCRIPTION	(0x0080)

//status types
#define STATUS_NONE				(0x00)		//not in queued msgs
#define STATUS_GET_LAST_MSG		(0x01)
#define STATUS_WARNING			(0x03)
#define STATUS_ERROR			(0x04)

//status msg IDs (error types),(MSB is zero and left out...)
#define STS_CAL_FAIL			(0x01)	//slot label
#define STS_SENS_NOT_FOUND		(0x02)	//"
#define STS_SENS_ALWAYS_ON		(0x03)	//"
#define STS_LAMP_DOUSED			(0x11)
#define STS_LAMP_STRIKE			(0x12)
#define STS_OVERTEMP			(0x21)	//SensorID, Temp
#define STS_UNDERTEMP			(0x22)	//"
#define STS_SENS_OUT_RANGE		(0x23)	//sensorID
#define STS_OVERVOLTAGE_PHASE	(0x31)	//Phase, Voltage
#define STS_UNDERVOLTAGE_PHASE	(0x32)	//"
#define STS_OVERCURRENT			(0x33)	//Phase, Current
#define STS_PHASE_ERROR			(0x36)	//Phase
#define STS_BREAKER_TRIP		(0x42)
#define STS_DIM_FAILURE			(0x44)
#define STS_READY				(0x50)	//slot label
#define STS_NOT_READY			(0x51)	//"
#define STS_LOW_FLUID			(0x52)	//"

//slot labels
#define SD_INTENSITY			(0x0001)
#define SD_INTENSITY_MASTER		(0x0002)
#define SD_PAN					(0x0101)
#define SD_TILT					(0x0102)
#define SD_COLOR_WHEEL			(0x0201)
#define SD_STATIC_GOBO_WHEEL	(0x0301)
#define SD_ROTO_GOBO_WHEEL		(0x0302)
#define SD_PRISM_WHEEL			(0x0303)
#define SD_STROBE				(0x0404)
#define SD_COLOR_ADD_RED		(0x0205)
#define SD_COLOR_ADD_GREEN		(0x0206)
#define SD_COLOR_ADD_BLUE		(0x0207)
#define SD_INTENSITY			(0x0001)
#define SD_INTENSITY			(0x0001)


#define SD_UNDEFINED			(0xFFFF)


//product category
#define FIXT_FIXED				(0x0101)	//not moving fixture
#define FIXT_MOVING_YOKE		(0x0102)	//moving head
#define FIXT_MOVING_MIRROR		(0x0103)	//scanner
#define FIXT_PROJECTOR			(0x0300)	//beamer
#define FIXT_ATMOSPHERIC_FX		(0x0401)	//fogger, hazer
#define FIXT_ATMOSPHERIC_PYRO	(0x0402)	//harr, harr...
#define FIXT_DIMMER				(0x0501)	//dimmer pack
#define FIXT_DIM_FLUORESCENT	(0x0502)	//cfl dimmer
#define FIXT_DIM_LED			(0x0509)	//LED dimmer

/*****************************************************************************************
								Typedef definitions
*****************************************************************************************/
#pragma anon_unions
typedef union {
   uint16_t u16;
   struct {
		  uint8_t u8l;
		  uint8_t u8h;
		  };
} uint16or8;

//常规数据包格式
typedef enum
{
    RDM_PACKET_SEL_LEN = 2,
    RDM_PACKET_SEL_S_DEST_ID = 3,
    RDM_PACKET_SEL_S_SRC_ID = 9,
    RDM_PACKET_SEL_PORT_ID = 16,
    RDM_PACKET_SEL_CC = 20,
    RDM_PACKET_SEL_S_PID = 21,
    RDM_PACKET_SEL_PDL = 23,
    RDM_PACKET_SEL_PD = 24,
}RDM_Packet_Sel_TypeDef;
typedef __packed struct
{
	uint8_t  ASC;								//alternate start code
	uint8_t  SSC;								//sub start code
	uint8_t  Length;							//total packet size
	uint8_t  DestID[6];							//target address
	uint8_t  SrcID[6];				    		//source address
	uint8_t  TransNo;							//tranaction number
	uint8_t  PortID;							//Port ID / Response Type
	uint8_t  MsgCount;							//message count
	uint16_t SubDev;							//sub device number (root = 0) 
	uint8_t  Cmd;								//command class
	uint16_t PID;								//parameter ID
	uint8_t  PDLen;								//parameter data length in bytes
	uint8_t  Data[RDM_PD_NUM];							//data byte field
}RDM_Packet_TypeDef;
//main flags
enum {
    EVAL_DMX, 
    DO_IDENTIFY
};	
typedef __packed struct
{
    uint8_t mute_flag;
    uint8_t recv_flag;
}RDM_Logic_TypeDef;
//设备信息
typedef __packed struct
{
    uint16_t Rdm_Protocol;
    uint16_t device_model_id;
    uint16_t product_category;
    uint32_t soft_ver_id;
    uint16_t dmx512_footprint;
    uint16_t dmx512_personality;
    uint16_t dmx512_start_addr;
    uint16_t sub_device_count;
    uint8_t  sensor_count;
}Device_Info_TypeDef;

//PID值
typedef __packed struct
{
    uint8_t pid_h;
    uint8_t pid_l;
}PID_Para_TypeDef;

typedef __packed struct 
{
	uint16_t slots;
	uint8_t  text[32];
}Dmx_Profile_Msg_TypeDef;
/*****************************************************************************************
							Global variables and functions
*****************************************************************************************/
extern uint16or8	 RxCount_16;
extern uint8_t g_rdm_tx_buf[RDM_DATA_NUM];
extern uint8_t g_rdm_rx_buf[RDM_DATA_NUM];
extern RDM_Logic_TypeDef g_rdm_ctr;
extern uint8_t DevID[];
//---------------------------使用者需要根据实际项目更改
//下面的宏定义是DMX模型的个数
#if PROJECT_TYPE==307
#define DMX_PROFILE_NUM		22
#endif
#if PROJECT_TYPE == 308
#define DMX_PROFILE_NUM		6
#endif
//下面的宏定义是当前项目的RDM所支持的命令个数
#define SUPPORT_PID_NUM		8

extern const char s_manufacturer_lable[32];
extern char s_device_lable[32];
//extern const char s_software_ver[32];
//下面这个参数是选择DMX模型的，AP-107 AP-179有，其他的灯只有一个模型，所以只需要保留一条
extern const Dmx_Profile_Msg_TypeDef s_dmx_profile[DMX_PROFILE_NUM];

//设备描述
extern Device_Info_TypeDef s_device_info_msg;
extern const PID_Para_TypeDef s_suppord_pid[SUPPORT_PID_NUM];
//-----------------------------------------------------------------------------------------
extern void    init_RDM(void);
uint16_t Swap_Short(uint16_t val);
uint32_t Swap_Int(uint32_t val);
extern void Set_RdmData(uint8_t* p_data, uint16_t len);
//extern void Dispose_Recv_Rdm_Data(void);
void Dispose_Recv_Rdm_Data(uint8_t *data);
//寻灯
extern void (*g_pRdmIdentifyCallback)(uint8_t flag);
//DMX起始地址
extern void (*g_pRdmSetStartAddrCallback)(uint16_t addr);
extern uint16_t (*g_pRdmGetStartAddrCallback)(void);
//DMX模型
extern void (*g_pRdmSetProfileCallback)(uint16_t profile);
extern uint16_t (*g_pRdmGetProfileCallback)(void);
//获取灯体工作总时间
extern uint32_t (*g_pRdmGetLampHousCallback)(void);

#endif


