#ifndef _SYSTEMCLOCK_H_
#define _SYSTEMCLOCK_H_

#include "hc32_ddl.h"


void SystemClockConfig(void);
void SystemClockConfig_Xtal(void);
void Peripheral_WE(void);
void Peripheral_WP(void);


#endif /* __SYSTEMCLOCK_H_ */
