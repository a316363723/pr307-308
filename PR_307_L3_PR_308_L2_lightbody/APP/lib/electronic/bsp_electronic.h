#ifndef BSP_ELECTRONIC
#define BSP_ELECTRONIC

#include "hc32_ddl.h"
#include "rs485_protocol.h"
typedef enum {
	Electronic_Idle = 0,
	Electronic_Send,
	Electronic_Recive,
	Electronic_Anlaysic,
	Electronic_Check_Error 
}Electronic_Status;

typedef  struct{
	uint8_t Electronic_Data[512];
	uint16_t Electronic_Size;
}Electronic;

typedef struct{
    uint8_t   hard_ver_major;   /*硬件主版本号 版本号定义：主.次.修订 如0x01 0x00 0x00 表示1.0.0 两位版本号可舍弃修订版本号*/
    uint8_t   hard_ver_minor;   /*硬件次版本号*/
    uint8_t   hard_ver_revision;/*硬件修订版本号*/
    uint8_t   soft_ver_major;   /*软件主版本号 版本号定义：主.次.修订 如0x01 0x00 0x00 表示1.0.0 两位版本号可舍弃修订版本号*/
    uint8_t   soft_ver_minor;   /*软件次版本号*/
    uint8_t   soft_ver_revision;/*软件修订版本号*/
}Electronic_version2_t;               /*size: 10*/

typedef  struct{
	Electronic W_Bracket_Buf;
	Electronic W_Bracket_Buf_Sent_Pitch_Angel;
	Electronic W_Bracket_Buf_Sent_Rotale_Angel;
	Electronic W_Bayont_Buf;
	Electronic R_Bracket_Buf;
	Electronic R_Bayont_Buf;
	Electronic_Status RW_Bracket_State;
	Electronic_Status RW_Bayont_State;
	Electronic_Status Bracket_Send_State;
	Electronic_Status Bayont_Send_State;
	uint8_t Bracket_Status;
	uint8_t Bayonet_Status;
	uint32_t Electronic_Connect;
	Electronic_version2_t Bracket_Version;
	Electronic_version2_t Fresnel_Version;
	uint8_t Bayonet_Times;
	uint8_t Bracket_Times;
	uint8_t Bayonet_Times_connect;
	uint8_t Bracket_Times_connect;
	uint8_t Fresnel_Times_Read_version;
	uint8_t Bracket_Times_Read_version;
	int16_t Fresnel_Temperature1;
	int16_t Fresnel_Temperature2;
	uint8_t Frensel_Angel_Time;
	uint8_t Bracket_Angel_Time;
	uint8_t Bracket_Loading_State;
	uint8_t Bayonet_Loading_State;
	uint8_t Bayonet_Communicate_Error;
	uint8_t Bayonet_Error_Reset_Cnt;
}Electronic_RW_Buf;

# define MAXQSIZE 10 //定义队列最大容量（长度）
typedef	struct{
	uint8_t data[256];
	uint8_t data_length;
}Electronic_data;

typedef struct {
	Electronic_data data[MAXQSIZE];
	int font, rear;
}Electronic_Queue;


extern Electronic_RW_Buf Electronic_Buf;
extern Electronic_Queue Bracket_Queue;
extern Electronic_Queue Bayont_Queue;
void Electronic_Stall_Crc_Reset_Package(void);
void Electronic_Para_Package(void);
void Electronic_angel_Package(void);
void Electronic_Package_Fuction(uint8_t *Electronic_Data,uint8_t RW_Cmd,rs485_motor_t motor,uint8_t onoff_cmd);
uint8_t Task_Send_Electronic_Cmd(uint8_t *Electronic_Data,uint16_t Electronic_Size,local_rs485_motor_t *motor);
void Task_AccessBaylet(uint8_t *AccessBaylet,uint8_t RW_Cmd,uint8_t Cmd_type,uint8_t Temperature_Type);
int push_electronic_data(Electronic_Queue *Queue,uint8_t *pData, uint16_t Cnt);
int get_electronic_data(Electronic_Queue *Queue,uint16_t type);
void Sent_Electronic_Data(void);
void init_queue(Electronic_Queue *Queue);
void less_than_48V_electronic_off(void);
void sent_heartbeat(void);
#endif

