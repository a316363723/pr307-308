#ifndef SM4_USER_H
#define SM4_USER_H

#define ENCRYPTION_LEN     (16)
#define DATA_LENGTH (500)
typedef struct sm4_data_type{
	unsigned char  data[DATA_LENGTH];
	unsigned short data_len;
}sm4_data_t;

/* 用户明文加密 */
sm4_data_t* user_plaintext_encryption(unsigned char* str, unsigned short len);
/* 用户密文解密 */
sm4_data_t* user_chiphertext_decryption(unsigned char* str, unsigned short len);


/*
使用方式：
//485数据加密
data：需要发送的原始485数据
enc_data：加密后的数据结构体
29：需要发送的原始485数据长度

enc_data = user_plaintext_encryption(data, 29);

//把加密后的密文拼接到起始包后面
memcpy(encode_Result, data, 11);
i = 0;
j = enc_data->data_len;
while (j)
{
	encode_Result[11 + i] = enc_data->data[i];
	i++;
	j--;
}

//485数据解密
data：接收到的原始485数据
dec_data：解密后的数据结构体
43：需要发送的原始485数据长度
dec_data = user_chiphertext_decryption(data, 43);

//把解密后的密文拼接到起始包后面就可以解析真正的485数据了
memcpy(decode_Result, dec_data->data, 11);
i = 0;
j = dec_data->data_len;
while (j)
{
	encode_Result[11 + i] = dec_data-data[i];
	i++;
	j--;
}
*/

#endif

