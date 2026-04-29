#ifndef SM4_ENCRYPTION_h
#define SM4_ENCRYPTION_h

void encode_fun(unsigned char len, unsigned char* key, unsigned char* input, unsigned char* output);   //加密函数
void decode_fun(unsigned char len, unsigned char* key, unsigned char* input, unsigned char* output);   //解密函数

#endif

