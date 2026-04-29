/****************************************************************************************
**  Filename :  111.c
**  Abstract :  。
**  By       :  何建国
**  Date     :  
**  Changelog:1.First Create
*****************************************************************************************/

/*****************************************************************************************
									Includes
*****************************************************************************************/
#include "flash.h"
#include "crc_32.h"
#include "data_cmp.h"
#include "define.h"
#include "fan.h" //Phoebe:解除warning
#include "project_config.h"
#include "app_led.h"

/*****************************************************************************************
							  Variables definitions
*****************************************************************************************/
static sFirmwareBin s_str_upgrade_msg;
static Upgrade_W_Flag_TypeDef s_str_upgrade_w_flag;
static uint8_t s_upgrade_state_flag = 0;
static uint16_t s_upgrade_overtime_cnt = 0;
/*****************************************************************************************
							  Functions definitions
*****************************************************************************************/

/*****************************************************************************************
* Function Name: upgrade_overtime_control
* Description  : 升级处理超时控制,在灯体所有流程都正常下，灯控却没有数据过来（10ms）
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void upgrade_overtime_control(void)
{
    if(RS485_File_TX_Step_Start != g_str_ota.stage){
        if(s_upgrade_overtime_cnt >= 1500){//15秒超时
            g_str_ota.stage = RS485_File_TX_Step_Start;
            s_upgrade_overtime_cnt = 0;
        }
        else{
            s_upgrade_overtime_cnt++;
        }
    }
}
/*****************************************************************************************
* Function Name: Upgrade_Process_Ctr
* Description  : 升级接口，协议收到数据后，将数据和规则作为参数；
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
#if 1
AES_ctx AES_OTA_ctx;
const uint8_t aes_key[16] = { 0x81, 0xB4, 0xF1, 0xBE, 0x14, 0xA5, 0xFC, 0x9E, 0x1C, 0x05, 0xD3, 0x74, 0x25, 0xBB, 0x9F, 0x1C };
const uint8_t aes_iv[16]  = { 0x15, 0x62, 0x37, 0x0D, 0x05, 0x07, 0xD8, 0x09, 0x0B, 0xAA, 0x62, 0x31, 0xA0, 0x5F, 0xD1, 0x51 };


void Upgrade_Process_Ctr(rs485_file_transfer_t* p_data, Rs485_Special_Ack_TypeDef* ack)
{
    s_upgrade_overtime_cnt = 0;
	switch(g_str_ota.stage)
	{
		case RS485_File_TX_Step_Start:
		{
			Cob_Output_Disable();
			if(g_str_ota.stage == p_data->step)
			{
				if(0X01 == p_data->step_body.start_arg.firmware_start.type)
				{
					g_str_ota.stage = RS485_File_TX_Step_Size;
					g_str_ota.erase.enable = ENABLE;
					g_str_ota.erase.start_addr = UPGRADE_START_ADDR;
					g_str_ota.erase.sector_num = UPGRADE_USE_SECTORS;
					g_str_ota.file.len = 0;
					g_str_ota.file.acc = 0;
					g_str_ota.file.sel = 0;
					g_str_ota.file.crc = 0xffffffff;
					g_str_ota.write.start_addr = UPGRADE_START_ADDR;
					#if(1 == GLOBAL_PRINT_ENABLE)
					printf("Ota start!\n");
					#endif
					//TITO:
										AES_init_ctx_iv(&AES_OTA_ctx, aes_key, aes_iv);
                    //ack->ack_type = RS485_Ack_Err_Timeout;/*提前设置应答状态为超时*/
                    ack->delay_time = 10000;/*设置超时时间，单位ms*/
				}
                else{
                    #if(1 == GLOBAL_PRINT_ENABLE)
                    printf("Upgrade file type err %d\n", g_str_ota.stage);
                    #endif
                    ack->ack_type = RS485_Ack_Err_Command;
                    g_str_ota.stage = RS485_File_TX_Step_Start;
                }
			}
			else
			{
                #if(1 == GLOBAL_PRINT_ENABLE)
                printf("Upgrade start err %d\n", g_str_ota.stage);
                #endif
                ack->ack_type = RS485_Ack_Err_Step;
                g_str_ota.stage = RS485_File_TX_Step_Start;
//				g_str_ota.ack.enable = ENABLE;
//				g_str_ota.ack.type = RS485_Ack_Err_Step;
			}
		}
		break;
		case RS485_File_TX_Step_Size:
		{
			if(g_str_ota.stage == p_data->step)
			{
				g_str_ota.file.len = p_data->step_body.size_arg.file_size;
				g_str_ota.stage = RS485_File_TX_Step_Data;
//				g_str_ota.ack.enable = ENABLE;
//				g_str_ota.ack.type = RS485_Ack_Ok;
                ack->ack_type = RS485_Ack_Ok;
				#if(1 == GLOBAL_PRINT_ENABLE)
				printf("File len is %d\n", g_str_ota.file.len);
				#endif
			}
			else
			{
                #if(1 == GLOBAL_PRINT_ENABLE)
                printf("Upgrade len err %d\n", g_str_ota.stage);
                #endif
//				g_str_ota.ack.enable = ENABLE;
//				g_str_ota.ack.type = RS485_Ack_Err_Step;
                ack->ack_type = RS485_Ack_Err_Step;
				g_str_ota.stage = RS485_File_TX_Step_Start;
			}
		}
		break;			
		case RS485_File_TX_Step_Data:
		{
			if(g_str_ota.stage == p_data->step)
			{
				if(g_str_ota.file.sel + 1 == p_data->step_body.data_arg.block_cnt)
				{
					if(0 == g_str_ota.file.sel)
					{
						memcpy(&s_str_upgrade_msg, p_data->step_body.data_arg.block_buf, sizeof(s_str_upgrade_msg));
						if(1 == ByteDataOrderCmp((const uint8_t *)UPGRADE_FILE_NAME, (uint8_t*)s_str_upgrade_msg.ProductName, UPGRADE_FILE_NAME_LEN))
						{
//							g_str_ota.ack.enable = ENABLE;
//							g_str_ota.ack.type = RS485_Ack_Err_File_Head;
              ack->ack_type = RS485_Ack_Err_File_Head;
							g_str_ota.stage = RS485_File_TX_Step_Start;
							#if(1 == GLOBAL_PRINT_ENABLE)
							printf("File name err\n");
							#endif
						}
						else
						{
                            if(s_str_upgrade_msg.HardwareVersion == LAMP_HARDWARE_VER){
                                g_str_ota.file.len = s_str_upgrade_msg.FirmwareSize;
                                #if(1 == GLOBAL_PRINT_ENABLE)
                                printf("File len is %d\n", g_str_ota.file.len);
                                #endif
                                switch(s_str_upgrade_msg.FirmwareType)
                                {
                                    case 0://APP
                                    {
                                        s_str_upgrade_w_flag.start_w_addr = g_str_ota.erase.start_addr;
                                        g_str_ota.write.start_addr = s_str_upgrade_w_flag.start_w_addr;
                                        s_str_upgrade_w_flag.up_len = g_str_ota.file.len;
                                        s_str_upgrade_w_flag.sum_len = UPGRADE_START_ADDR - AES_START_ADDR;
                                    }
                                    break;
                                    case 1://BASE PWM
                                    {
                                        s_str_upgrade_w_flag.start_w_addr = g_str_ota.erase.start_addr;
                                        g_str_ota.write.start_addr = s_str_upgrade_w_flag.start_w_addr;
                                        s_str_upgrade_w_flag.up_len = g_str_ota.file.len;
                                        s_str_upgrade_w_flag.sum_len = UPGRADE_START_ADDR - AES_START_ADDR;
										((sFirmwareBin*)(p_data->step_body.data_arg.block_buf))->DriveMode = 0x00;
                                    }
                                    break;
                                    case 2://APP+BASE
                                    {
                                        s_str_upgrade_w_flag.start_w_addr = g_str_ota.erase.start_addr;
                                        g_str_ota.write.start_addr = s_str_upgrade_w_flag.start_w_addr;
                                        s_str_upgrade_w_flag.up_len = g_str_ota.file.len;
                                        s_str_upgrade_w_flag.sum_len = g_str_ota.file.len;
                                    }
                                    break;
									case 3://BASE ANALOG
                                    {
                                        s_str_upgrade_w_flag.start_w_addr = g_str_ota.erase.start_addr;
                                        g_str_ota.write.start_addr = s_str_upgrade_w_flag.start_w_addr;
                                        s_str_upgrade_w_flag.up_len = g_str_ota.file.len;
                                        s_str_upgrade_w_flag.sum_len = UPGRADE_START_ADDR - AES_START_ADDR;
										((sFirmwareBin*)(p_data->step_body.data_arg.block_buf))->DriveMode = 0xff;
										((sFirmwareBin*)(p_data->step_body.data_arg.block_buf))->FirmwareType = 0x01;
                                    }
                                    break;
                                    default:break;
                                }
								if(ERROR == write_data_to_flash((uint32_t*)&g_str_ota.write.start_addr, p_data->step_body.data_arg.block_buf, p_data->step_body.data_arg.block_size))
								{
		//							g_str_ota.ack.enable = ENABLE;
		//							g_str_ota.ack.type = RS485_Ack_Err_Write;
									ack->ack_type = RS485_Ack_Err_Write;
									g_str_ota.stage = RS485_File_TX_Step_Start; 
									#if(1 == GLOBAL_PRINT_ENABLE)
									printf("Write flash err!\n");
									#endif
								}
                                s_str_upgrade_w_flag.up_enable = UPGRADE_ENABLE;
								g_str_ota.write.start_addr += p_data->step_body.data_arg.block_size;
                            }
                            else{
                                ack->ack_type = RS485_Ack_Err_File_Head;
                                g_str_ota.stage = RS485_File_TX_Step_Start;
                            }
						}
					}
					else
					{
						//TITO:
						AES_CBC_decrypt_buffer(&AES_OTA_ctx, p_data->step_body.data_arg.block_buf, p_data->step_body.data_arg.block_size);//数据解密
						g_str_ota.file.crc = update_crc32(g_str_ota.file.crc, p_data->step_body.data_arg.block_buf, p_data->step_body.data_arg.block_size);//CRC32校验
						if(ERROR == write_data_to_flash((uint32_t*)&g_str_ota.write.start_addr, p_data->step_body.data_arg.block_buf, p_data->step_body.data_arg.block_size))
						{
//							g_str_ota.ack.enable = ENABLE;
//							g_str_ota.ack.type = RS485_Ack_Err_Write;
							ack->ack_type = RS485_Ack_Err_Write;
							g_str_ota.stage = RS485_File_TX_Step_Start; 
							#if(1 == GLOBAL_PRINT_ENABLE)
							printf("Write flash err!\n");
							#endif
						}
						else
						{
							g_str_ota.file.acc += p_data->step_body.data_arg.block_size;
							g_str_ota.write.start_addr += p_data->step_body.data_arg.block_size;
							if(g_str_ota.file.acc >= g_str_ota.file.len)
							{
								g_str_ota.file.crc ^= 0xffffffff;
								if(g_str_ota.file.crc == s_str_upgrade_msg.CheckCRC32)
								{
//									g_str_ota.ack.enable = ENABLE;
//									g_str_ota.ack.type = RS485_Ack_Ok;
									ack->ack_type = RS485_Ack_Ok;
									g_str_ota.stage = RS485_File_TX_Step_CRC;
									#if(1 == GLOBAL_PRINT_ENABLE)
									printf("File download success!\n");
									#endif
									s_str_upgrade_w_flag.up_file_crc = g_str_ota.file.crc;
								}
								else
								{
//									g_str_ota.ack.enable = ENABLE;
//									g_str_ota.ack.type = RS485_Ack_Err_Verify;
                                    ack->ack_type = RS485_Ack_Err_Verify;
									g_str_ota.stage = RS485_File_TX_Step_Start;
									#if(1 == GLOBAL_PRINT_ENABLE)
									printf("Crc err!\n");
									#endif
								}
							}
                            else{
//                                g_str_ota.ack.enable = ENABLE;
//                                g_str_ota.ack.type = RS485_Ack_Ok;
                                ack->ack_type = RS485_Ack_Ok;
                            }
						}
					}
					g_str_ota.file.sel++;
				}
				else
				{
                    /*序列号不检测错误的原因是主机可能会同一个数据包发送多次*/
//					g_str_ota.ack.enable = ENABLE;
					g_str_ota.ack.type = RS485_Ack_Err_Sequence;
					g_str_ota.stage = RS485_File_TX_Step_Start;
					#if(1 == GLOBAL_PRINT_ENABLE)
					printf("Frame err\n");
					#endif
				}
			}
			else
			{
//				g_str_ota.ack.enable = ENABLE;
//				g_str_ota.ack.type = RS485_Ack_Err_Step;
                ack->ack_type = RS485_Ack_Err_Step;
				g_str_ota.stage = RS485_File_TX_Step_Start;
				#if(1 == GLOBAL_PRINT_ENABLE)
				printf("Data Order err\n");
				#endif
			}
		}
		break;
		case RS485_File_TX_Step_CRC:
		{
//			g_str_ota.ack.enable = ENABLE;
      ack->ack_type = RS485_Ack_Ok;
			g_str_ota.stage = RS485_File_TX_Step_Start;
			g_str_ota.succ_flag = SUCCESS;
			s_upgrade_state_flag = 1;
			#if(1 == GLOBAL_PRINT_ENABLE)
			printf("Upgrade over!\n");
			#endif
		}
		break;
		default:break;
	}
}
#endif
/*****************************************************************************************
* Function Name: Upgrade_Data_Comb
* Description  : 升级数据组合(10ms时间片)
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void Upgrade_Data_Comb(void)
{
	if(0 == s_upgrade_state_flag)
		return;
	s_upgrade_state_flag = 0;
	switch(s_str_upgrade_msg.FirmwareType)
	{
		case 0://APP
		{
			#if NULL
			if(ERROR == Move_Data_Flash_To_flash(UPGRADE_START_ADDR + BASE_DATA_OFFSET, AES_START_ADDR + BASE_DATA_OFFSET, UPGRADE_START_ADDR - AES_START_ADDR - BASE_DATA_OFFSET))
			{
				Erase_Flash_Area(UPGRADE_START_ADDR, UPGRADE_USE_SECTORS-9);//擦除升级区
				return;
			}
			#endif
			g_str_ota.succ_flag = SUCCESS;
			#if(1 == GLOBAL_PRINT_ENABLE)
			printf("Upgrade app:\n");
			#endif
		}
		break;
		case 1://BASE
		{
			#if NULL
			#if 1 //正常升级基础数据
			if(ERROR == Move_Data_Flash_To_flash(UPGRADE_START_ADDR, AES_START_ADDR, BASE_DATA_OFFSET))
			{
				Erase_Flash_Area(UPGRADE_START_ADDR, UPGRADE_USE_SECTORS-9);//擦除升级区
				return;
			}
			#else   //升级BootLoader 此行为非常危险  *****
			ErrorStatus flash_dispose_flag = SUCCESS;
			if(0 != Check_Update_Data(s_str_upgrade_w_flag.up_file_crc, s_str_upgrade_w_flag.up_len, s_str_upgrade_w_flag.start_w_addr))
				return;
			Erase_Boot:flash_dispose_flag = Erase_Flash_Area(BOOT_START_SECTOR, BOOT_USE_SECTORS);
			if(ERROR == flash_dispose_flag)
			{
				#if(1 == IWDG_EN)
				fwdgt_counter_reload();
				#endif
				goto Erase_Boot;
			}
			if(ERROR == Move_Data_Flash_To_flash(BOOT_START_ADDR, s_str_upgrade_w_flag.start_w_addr, s_str_upgrade_w_flag.up_len))
			{
				#if(1 == IWDG_EN)
				fwdgt_counter_reload();
				#endif
				goto Erase_Boot;
			}
			#if(1 == GLOBAL_PRINT_ENABLE)
			printf("Bootloader data write over:\n");
			#endif
			delay_ms(1000);
			NVIC_SystemReset();
			return;
			#endif
			
			#if(1 == GLOBAL_PRINT_ENABLE)
			printf("Upgrade base data:\n");
			#endif
			
			#endif
			g_str_ota.succ_flag = SUCCESS;
		}
		break;
		case 2://APP+BASE
		{
			g_str_ota.succ_flag = SUCCESS;
			#if(1 == GLOBAL_PRINT_ENABLE)
			printf("Upgrade all:\n");
			#endif
		}
		break;
		case 3://ANALOG_DATA
		{
			g_str_ota.succ_flag = SUCCESS;
			#if(1 == GLOBAL_PRINT_ENABLE)
			printf("Upgrade all:\n");
			#endif
		}
		break;
		default:break;
	}	
}
/*****************************************************************************************
* Function Name: Write_Upgrade_Flag
* Description  : 
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
ErrStatus Write_Upgrade_Flag(void)
{
	uint32_t flag_addr = FLAG_START_ADDR;
	if(ERROR == Erase_Flash_Area(FLAG_START_ADDR, FLAG_USE_SECTORS))
	{
		Erase_Flash_Area(UPGRADE_START_ADDR, UPGRADE_USE_SECTORS-9);//擦除升级区
		g_str_ota.succ_flag = ERROR;
		return ERROR;
	}
	if(ERROR == write_data_to_flash((uint32_t*)&flag_addr, (uint8_t*)&s_str_upgrade_w_flag.up_enable, sizeof(uint32_t)))
	{
		Erase_Flash_Area(UPGRADE_START_ADDR, UPGRADE_USE_SECTORS-9);//擦除升级区
		g_str_ota.succ_flag = ERROR;
		return ERROR;
	}
	#if(1 == GLOBAL_PRINT_ENABLE)
	printf("Upgrade flag succ:\n");
	#endif
	return SUCCESS;
}
/*****************************************************************************************
* Function Name: Upgrade_Flash_Erase
* Description  : 升级flash控制(10ms时间片)
* Arguments    : NONE
* Return Value : NONE
******************************************************************************************/
void Upgrade_Flash_Erase(void)
{
	if(DISABLE == g_str_ota.erase.enable)
		return;
	g_str_ota.erase.enable = DISABLE;
	if(ERROR == Erase_Flash_Area(UPGRADE_START_ADDR, UPGRADE_USE_SECTORS))
	{
		g_str_ota.ack.type = RS485_Ack_Err_Erase;
	}
	else
	{
		g_str_ota.ack.type = RS485_Ack_Ok;
		Fan_Water_Trig_To_Zero();
	}
	g_str_ota.ack.enable = ENABLE;	
	#if(1 == GLOBAL_PRINT_ENABLE)
	printf("Flash erase over!\n");
	#endif
}
/*****************************************************************************************
* Function Name: Get_Upgrade_Start_State
* Description  : 485协议获取升级起始状态信息，
* Arguments    : NONE
* Return Value : RS485_Ack_Err_Timeout：升级开始后的应答状态，所有其他应答需要等待的协议都按照这种方式
                 其他类型：则说明初始状态已经完成；
******************************************************************************************/
rs485_ack_enum Get_Upgrade_Start_State(void)
{
    /*单片机擦除完成，则应答实际的状态，否则应答等待状态*/
    if(ENABLE == g_str_ota.ack.enable){
        g_str_ota.ack.enable = DISABLE;
        return(g_str_ota.ack.type);
    }
    else{
        return(RS485_Ack_Err_Busy);
    }
}
/***********************************END OF FILE*******************************************/
