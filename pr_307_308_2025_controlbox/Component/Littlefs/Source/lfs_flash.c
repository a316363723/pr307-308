#include "lfs_flash.h"
#include "dev_w25qxx.h"
// variables used by the filesystem

lfs_t lfs;
lfs_file_t file;
#define LFS_FLASH_NAME "board_spiflash"


lfs_file_t lfs_file_cfg;
lfs_file_t lfs_file_light;
lfs_file_t lfs_file_preset;

char cfg_file_name[]="auto_data";
char light_file_name[] = "light_data";
char preset_file_name[] = "preset_data";

lfs_file_t lfs_file_iot;
char iot_file_name[2][16] = {
    {"auto_iot_data1",},
	{"auto_iot_data2",},
};


//static char preset_file_name[16] = "preset_data";


int  lfs_mount_fail;
int user_provided_block_device_read(const struct lfs_config *c, lfs_block_t block,
lfs_off_t off, void *buffer, lfs_size_t size)
{
	dev_w25qxx_read_data((block* c->block_size + off) + LFS_FLASH_OFFS, buffer, size);
	return 0;	
}
int user_provided_block_device_prog(const struct lfs_config *c, lfs_block_t block,
lfs_off_t off, const void *buffer, lfs_size_t size)
{
	dev_w25qxx_write_data((block* c->block_size + off) + LFS_FLASH_OFFS, (uint8_t *)buffer, size);
	return 0;
}
int user_provided_block_device_erase(const struct lfs_config *c, lfs_block_t block)
{
	dev_w25qxx_erase_sectors((LFS_FLASH_OFFS / c->block_size) +block , 1);
	return 0;
}
int user_provided_block_device_sync(const struct lfs_config *c)
{
    return 0;
}
// configuration of the filesystem is provided by this struct
const struct lfs_config lfs_cfg = {
    // block device operations
    .read  = user_provided_block_device_read,
    .prog  = user_provided_block_device_prog,
    .erase = user_provided_block_device_erase,
    .sync  = user_provided_block_device_sync,

    // block device configuration
    .read_size = 512,
    .prog_size = 32,
    .block_size = 4096,
    .block_count = 1024,
	.cache_size = 1024,
    .lookahead_size = 1024,
	.block_cycles = 1000,
};
/*****************************************************************************************
* Function Name : sys_lfs_mount
* Description   : littlefs文件系统挂载（初始化函数调用）
* Arguments     : void
* Return Value	: void
******************************************************************************************/
int sys_lfs_mount(void)
{
	// mount the filesystem
    int err = lfs_mount(&lfs, &lfs_cfg);
    
    // reformat if we can't mount the filesystem  
    // this should only happen on the first boot
    if (err) {
        lfs_format(&lfs, &lfs_cfg);
        err = lfs_mount(&lfs, &lfs_cfg);
    }	
    return err;
}
/*****************************************************************************************
* Function Name : read_cfg_data_from_store
* Description   : 获取存在flash中的配置数据（上电获取 read）
* Arguments     : 存配置数据的地址
* Return Value	: void
******************************************************************************************/
void read_cfg_data_from_store(uint8_t* p_cfg, uint32_t len)
{
	if(lfs_mount_fail == 0)
	{
		lfs_file_open(&lfs, &lfs_file_cfg, 
					  cfg_file_name, 
					  LFS_O_RDWR | LFS_O_CREAT);
		lfs_file_read(&lfs, &lfs_file_cfg, p_cfg, len);
		lfs_file_close(&lfs, &lfs_file_cfg);
	}
}
/*****************************************************************************************
* Function Name : write_cfg_data_to_store
* Description   : 写配置数据到存储芯片
* Arguments     : 存配置数据的地址、数据长度、文件组序号、文件组内文件序号
* Return Value	: void
******************************************************************************************/
void write_cfg_data_to_store(uint8_t *p_cfg, uint32_t len)
{
	if(lfs_mount_fail == 0)
	{
		lfs_file_open(&lfs, &lfs_file_cfg, cfg_file_name, LFS_O_RDWR | LFS_O_CREAT);
		lfs_file_rewind(&lfs, &lfs_file_cfg);
		lfs_file_write(&lfs, &lfs_file_cfg, p_cfg, len);
		lfs_file_close(&lfs, &lfs_file_cfg);
	}
}
/*****************************************************************************************
* Function Name : read_preset_data_from_store
* Description   : 获取存在flash中的预设光效数据
* Arguments     : 存配置数据的地址
* Return Value	: void
******************************************************************************************/
int read_preset_data_from_store(uint8_t* p_cfg, uint16_t len)
{
    int res = 0;
    
	if(lfs_mount_fail == 0)
	{
		lfs_file_open(&lfs, &lfs_file_preset, 
					  preset_file_name, 
					  LFS_O_RDWR);
        lfs_file_read(&lfs, &lfs_file_preset, p_cfg, len);
        lfs_file_close(&lfs, &lfs_file_preset);
	}
    
    return res;
}
/*****************************************************************************************
* Function Name : write_preset_data_to_store
* Description   : 写预设光效数据到存储芯片
* Arguments     : 存配置数据的地址、数据长度、文件组序号、文件组内文件序号
* Return Value	: res：0----写成功         
******************************************************************************************/
int write_preset_data_to_store(uint8_t *p_cfg, uint16_t len)
{
    int res;
    
	if(lfs_mount_fail == 0)
	{
		res = lfs_file_open(&lfs, &lfs_file_preset, preset_file_name,LFS_O_RDWR | LFS_O_CREAT);
        if(res != 0)
        {
            return res;
        }
        
        lfs_file_rewind(&lfs, &lfs_file_preset);
        lfs_file_write(&lfs, &lfs_file_preset, p_cfg, len);
        lfs_file_close(&lfs, &lfs_file_preset);
	}
    
    return res;
}
/*****************************************************************************************
* Function Name : read_iot_data_from_store
* Description   : 获取存在flash中的IOTA数据（上电获取 read）
* Arguments     : 存配置数据的地址
* Return Value	: void
******************************************************************************************/
void read_iot_data_from_store(uint8_t* p_cfg, uint16_t len, uint8_t num)
{
	if(lfs_mount_fail == 0)
	{
		lfs_file_open(&lfs, &lfs_file_iot, 
					  iot_file_name[num], 
					  LFS_O_RDWR | LFS_O_CREAT);
		lfs_file_read(&lfs, &lfs_file_iot, p_cfg, len);
		lfs_file_close(&lfs, &lfs_file_iot);
	}
}
/*****************************************************************************************
* Function Name : write_iot_data_to_store
* Description   : 写iot数据到存储芯片
* Arguments     : p_cfg:存配置数据的地址.
                  len:数据长度
* Return Value	: void
******************************************************************************************/
void write_iot_data_to_store(uint8_t *p_cfg, uint16_t len, uint8_t num)
{
	if(lfs_mount_fail == 0)
	{
		lfs_file_open(&lfs, &lfs_file_iot, iot_file_name[num], LFS_O_RDWR | LFS_O_CREAT);
		lfs_file_rewind(&lfs, &lfs_file_iot);
		lfs_file_write(&lfs, &lfs_file_iot, p_cfg, len);
		lfs_file_close(&lfs, &lfs_file_iot);
	}
}
