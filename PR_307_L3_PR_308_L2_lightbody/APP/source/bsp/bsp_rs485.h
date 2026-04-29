#ifndef BSP_RS485_H
#define BSP_RS485_H
#include <stdint.h>
extern void (*g_pRs485TxCallback)(void);
extern void (*g_pRs485RxCallback)(uint8_t *pBuff,uint16_t Length);
extern void (*g_pRsBaseDataTxCallback)(void);
extern void (*g_pRsBaseDataRxCallback)(uint8_t *pBuff,uint16_t Length);
extern void rs485_slave_init(void);
extern void rx485_send(uint8_t *pData, uint16_t Cnt);
#endif
