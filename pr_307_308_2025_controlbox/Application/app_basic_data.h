#ifndef	_APP_DEBUG_H
#define	_APP_DEBUG_H
#include "stdint.h"
#include "dev_rs485.h"
#include "rs485_protocol.h"


#define _base_data_  1

typedef enum {
	UPGRAD_ERR = 0,
	UPGRAD_STATUS_STEP,
	UPGRAD_LENGHT_STEP,
    UPGRAD_DATA_STEP,
	UPGRAD_VERIFICATION_STEP,
	UPGRAD_END,
}upgrad_status_type;

void base_data_collect_init(void);
void base_data_general_ack_pack(void);
bool get_base_updata_state(void) ;
uint8_t debug_base_data_collect_cb(rs485_cmd_body_t *p_body);
#endif

