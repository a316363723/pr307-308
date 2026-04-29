/*file:security.c*/
#include "security.h"
#include "string.h"
#include "security_encrypt.h"

AES_ctx security_ctx;

//函数地址结合chip id进行秘钥拓展
void AES_Key_Create(uint32_t fun_address)
{
	uint8_t chip_id[16] = {0}; 
	uint8_t function_data[16] = {0};
	uint8_t count = 0;
	
	memcpy(chip_id, (const void *)CHIP_ID_ADDRESS, 12);
	memcpy(function_data, (const void *)&fun_address,  4);
	do{function_data[count] ^= chip_id[count];}while((++ count) < 16);
	AES_init_ctx(&security_ctx, function_data);
}

//对加密的函数进行解密，返回函数地址
uint32_t Function_Decrypt(const uint8_t *data)
{
	uint8_t chip_id[16] = {0}; 
	static uint8_t function_data[16] = {0};
	uint8_t count = 0;
	
	memcpy(chip_id, (const void *)CHIP_ID_ADDRESS, 12);					//chip_id读取
	memcpy(function_data, (const void *)data, 16);						//读取加密的数据
	AES_ECB_decrypt((const AES_ctx*)&security_ctx, function_data); 		//2次加密数据解密
	do{function_data[count] ^= chip_id[count];}while((++ count) < 16);	//解析出1次加密数据
	AES_ECB_decrypt((const AES_ctx*)&security_ctx, function_data); 		//1次加密数据解密
	
	return (*((uint32_t *)&function_data));
}
