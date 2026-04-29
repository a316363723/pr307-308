#include "security.h"
#include "string.h"

//函数地址结合chip id进行AES128加密
uint8_t * Function_Encrypt(uint32_t fun_address)
{
	uint8_t chip_id[16] = {0}; 
	static uint8_t function_data[16] = {0};
	uint8_t count = 0;
	
	memset(function_data, 0, 16);
	
	memcpy(function_data, (const void *)&fun_address, 4);				//函数地址读取
	AES_ECB_encrypt((const AES_ctx*)&security_ctx, function_data); 		//函数地址加密，得到1次加密数据
	memcpy(chip_id, (const void *)CHIP_ID_ADDRESS, 12);					//芯片加密数据读取
	do{function_data[count] ^= chip_id[count];}while((++ count) < 16);	//1次加密数据与chip id进行位异或运算
	AES_ECB_encrypt((const AES_ctx*)&security_ctx, function_data); 		//再次进行加密，得到2次加密数据
	
	return function_data;
}
