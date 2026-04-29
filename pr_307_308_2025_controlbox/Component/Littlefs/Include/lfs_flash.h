#ifndef __LFT_FLASH_H
#define __LFT_FLASH_H
#include "lfs.h"

extern int  lfs_mount_fail;
extern lfs_t lfs;
extern lfs_file_t lfs_file_cfg;
extern lfs_file_t lfs_file_light;
extern lfs_file_t lfs_file_preset;

extern char cfg_file_name[];
extern char light_file_name[];
extern char preset_file_name[];

int sys_lfs_mount(void);
void read_cfg_data_from_store(uint8_t* p_cfg, uint32_t len);
void write_cfg_data_to_store(uint8_t *p_cfg, uint32_t len);
void read_iot_data_from_store(uint8_t* p_cfg, uint16_t len, uint8_t num);
void write_iot_data_to_store(uint8_t *p_cfg, uint16_t len, uint8_t num);
int read_preset_data_from_store(uint8_t* p_cfg, uint16_t len);
int write_preset_data_to_store(uint8_t *p_cfg, uint16_t len);

#endif
