#include "api_gatt_proto.h"
#include <stddef.h>
#include <string.h>

uint8_t checksum_calculate(const uint8_t* p_data, uint8_t length)
{
    uint8_t checksum = 0;
    while (length--)
    {
        checksum += *p_data++;
    }
    return checksum;
}


int api_gatt_packet_check(const gatt_packet_t* p_packet)
{
    int res = 0;
    if( p_packet->head.cmd >= GATT_CMD_NULL)
    {
        return -1;
    }
    if(p_packet->body.body_buffer[p_packet->head.length - sizeof(gatt_head_t)] != 0x0d)
    {
        return -2;
    }
    if(p_packet->head.check_sum != checksum_calculate((const uint8_t*)&p_packet->head.length, p_packet->head.length - 1))
    {
        return -3;
    }
    return res;
}

/* 升级应答数据打包 */
void api_gatt_data_pack_up(uint8_t *p_data, gatt_cmd_enum cmd, const uint8_t *body, uint8_t body_length, uint8_t sequence, uint8_t rw)
{
    if (p_data == NULL || body == NULL)
    {
        return;
    }
    gatt_packet_t *p_packet = (gatt_packet_t *)p_data;
    
    p_packet->head.length = body_length + 6;
    p_packet->head.sequence = sequence;
    p_packet->head.proto_ver = 0x20;
    p_packet->head.cmd = cmd;
    p_packet->head.rw = rw;
    memcpy(&p_packet->body, body, body_length);
    p_packet->head.check_sum = checksum_calculate((const uint8_t*)&p_packet->head.length, p_packet->head.length - 1);
}




