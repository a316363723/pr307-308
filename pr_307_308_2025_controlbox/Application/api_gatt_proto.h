#ifndef API_GATT_PROTO_H
#define API_GATT_PROTO_H
#include "user_gatt_proto.h"
#include <stdint.h>
int api_gatt_packet_check(const gatt_packet_t* p_packet);
void api_gatt_data_pack_up(uint8_t *p_data, gatt_cmd_enum cmd, const uint8_t *body, uint8_t body_length, uint8_t sequence, uint8_t rw);
uint8_t checksum_calculate(const uint8_t* p_data, uint8_t length);

#endif

