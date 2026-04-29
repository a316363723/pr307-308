#include "sm4_enc_user.h"
#include "sm4_encryption.h"
#include "stdio.h"
#include "string.h"

enum {
	ENCRYPTION_TYPE = 0,
	DECRYPTION_TYPE,
	ENCRYPTION_NUM ,
};

enum {
	ENCRYPTION_OK = 0,
	ENCRYPTION_KEY_ERROR = -1,
};

static unsigned char s_encryption_key[16] = {
0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF};

static unsigned char s_decryption_key[16] = {
0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF};

static sm4_data_t s_enc_data_t;
static sm4_data_t s_dec_data_t;

static int user_secret_key_get(unsigned char* str, unsigned char mode);
static void plaintext_padding(unsigned char* str, unsigned short len);

/*
函数作用：根据原始数据中的明文和秘钥进行加密
函数形参：@str：原始数据
          @len：原始数据长度
函数返回值：@p_encdata：加密后的数据结构体
*/
sm4_data_t* user_plaintext_encryption(unsigned char* str, unsigned short len)
{
	int res = 0;

	unsigned char plaintext_data[DATA_LENGTH] = { 0 };
	unsigned short plaintext_len = ((str[4] << 8)|str[3]) - str[2];//明文的长度
	unsigned short star_len = str[2];
	unsigned short encryption_star_add = 0;
	unsigned short encryption_len_count = 0;
	sm4_data_t *p_encdata = &s_enc_data_t;
	unsigned short enc_len = 0;

	if(str == NULL || plaintext_len > DATA_LENGTH)
	{
		return NULL;
	}

	memset(p_encdata->data, 0, DATA_LENGTH);
	//得到秘钥
	res = user_secret_key_get(str, ENCRYPTION_TYPE);
	if(res != 0)
	{
		return NULL;
	}

	//得到真正的明文，也就是真正需要加密的数据
	for(unsigned short i = 0; i < plaintext_len; i++)
	{
		plaintext_data[i] = str[star_len + i];
	}

	encryption_len_count = plaintext_len;
	while(1)
	{
		if(encryption_len_count / ENCRYPTION_LEN == 0)
		{
//			printf("__LINE__:%d,__FUNCTION__:%d\r\n",__LINE__,__func__);
			plaintext_padding(&plaintext_data[encryption_star_add], encryption_len_count);
			encode_fun(ENCRYPTION_LEN, s_encryption_key, &plaintext_data[encryption_star_add], &p_encdata->data[encryption_star_add]);
//			printf("__LINE__:%d,__FUNCTION__:%d\r\n",__LINE__,__func__);
			enc_len += 16;
			break;
		}
		else
		{
//			printf("__LINE__:%d,__FUNCTION__:%d\r\n",__LINE__,__func__);
			encode_fun(ENCRYPTION_LEN, s_encryption_key, &plaintext_data[encryption_star_add], &p_encdata->data[encryption_star_add]);
			encryption_len_count -= ENCRYPTION_LEN;
//			printf("__LINE__:%d,__FUNCTION__:%d\r\n",__LINE__,__func__);
			encryption_star_add += 16;
			enc_len += 16;
		}
	}
	p_encdata->data_len = enc_len;
	
	return p_encdata;
}

/*
函数作用：根据原始数据中的密文和秘钥进行解密
函数形参：@str：需要解密的数据地址
		  @len：原始数据长度
函数返回值：@data：加密后的数据地址（密文）
*/
sm4_data_t* user_chiphertext_decryption(unsigned char* str, unsigned short len)
{
	int res = 0;

	unsigned char chiphertext_data[DATA_LENGTH] = { 0 };
	unsigned short plaintext_len = ((str[4] << 8) | str[3]) - str[2];//解密后明文的长度
	unsigned short star_len = str[2];
	unsigned short chipherption_star_add = 0;
  short chipherption_len_count = 0;
	sm4_data_t* p_decdata = &s_dec_data_t;

	if (str == NULL && plaintext_len > DATA_LENGTH)
	{
		return NULL;
	}
	memset(p_decdata->data, 0, DATA_LENGTH);
	//得到秘钥
	res = user_secret_key_get(str, DECRYPTION_TYPE);
	if (res != 0)
	{
		return NULL;
	}

	//得到真正的密文，也就是真正需要解密的数据
	for (unsigned short i = 0; i < len - star_len; i++)
	{
		chiphertext_data[i] = str[star_len + i];
	}

	chipherption_len_count = len - star_len;
	while (1)
	{
//		printf("__LINE__:%d,__FUNCTION__:%d\r\n",__LINE__,__func__);
		decode_fun(ENCRYPTION_LEN, s_decryption_key, &chiphertext_data[chipherption_star_add], &p_decdata->data[chipherption_star_add]);
		if (chipherption_len_count - ENCRYPTION_LEN <= 0)
		{
			break;
		}
		chipherption_len_count -= ENCRYPTION_LEN;
		chipherption_star_add += 16;
//		printf("__LINE__:%d,__FUNCTION__:%d\r\n",__LINE__,__func__);
	}
	p_decdata->data_len = plaintext_len;
//	printf("__LINE__:%d,__FUNCTION__:%d\r\n",__LINE__,__func__);
	return p_decdata;
}

/*
函数作用：得到加密或者解密用的秘钥
函数形参：@str：需要加密或者解密的原始数据地址
函数返回值：@data：秘钥地址
*/
static int user_secret_key_get(unsigned char* str, unsigned char mode)
{
	unsigned char* data = NULL;
	
	if(mode >= ENCRYPTION_NUM)
	{
		return ENCRYPTION_KEY_ERROR;
	}
	else if(mode == ENCRYPTION_TYPE)
	{
		data = s_encryption_key;
	}
	else if(mode == DECRYPTION_TYPE)
	{
		data = s_decryption_key;
	}
	if (data == NULL)
	{
		return ENCRYPTION_KEY_ERROR;
	}

	for (unsigned char i = 0; i < 11; i++)
		data[i] = str[i];

	return ENCRYPTION_OK;
}

/*
函数作用：没满16byte的明文填充0xFF
函数形参：@str：需要填充的明文起始地址
	      @len：当前明文有效数据的长度
函数返回值：NULL
*/
static void plaintext_padding(unsigned char* str, unsigned short len)
{
	unsigned short padding_len = ENCRYPTION_LEN - len;

	for (unsigned char i = 0; i < padding_len; i++)
	{
		str[len + i] = 0XFF;
	}
}

