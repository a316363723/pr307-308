#include "lfs_flash.h"

// variables used by the filesystem
lfs_t lfs;
lfs_file_t lfs_file_cfg;
lfs_file_t lfs_file_iot;

uint8_t  lfs_mount_fail = 0;				

							
int user_provided_block_device_read(const struct lfs_config *c, lfs_block_t block,
lfs_off_t off, void *buffer, lfs_size_t size)
{
	W25Q64_ReadData((block* c->block_size + off), buffer, size);
	return 0;	
}
int user_provided_block_device_prog(const struct lfs_config *c, lfs_block_t block,
lfs_off_t off, const void *buffer, lfs_size_t size)
{
	W25Q64_WriteData((block* c->block_size + off), (uint8_t *)buffer, size);
	return 0;
}
int user_provided_block_device_erase(const struct lfs_config *c, lfs_block_t block)
{
	/* (4KB sector erase), (32KBblock erase) */
	W25Q64_EraseSector((block));
	return 0;
}
int user_provided_block_device_sync(const struct lfs_config *c)
{
    return 0;
}




/*
malloc size: (3*cache_size+lookahead_size)  //heap堆的大小要大于动态内存占用的空间大小
*/
// configuration of the filesystem is provided by this struct
const struct lfs_config lfs_cfg = {
    // block device operations
    .read  = user_provided_block_device_read,
    .prog  = user_provided_block_device_prog,
    .erase = user_provided_block_device_erase,
    .sync  = user_provided_block_device_sync,

    // block device configuration
    .read_size = 512,		//单位K；读取块的缓存大小
    .prog_size = 32,
    .block_size = 4096,		//单位Byte=4k：可擦除快的大小，必须是读取块和程序块的倍数
    .block_count = 1024,	//单位扇区：可擦除块的数目（1024*4K=4M，因为是以扇区擦除）
    .cache_size = 1024,		//单位Byte：读取数据缓存区，必须是读取块和程序块的倍数，缓存区大则新=性能好
    .lookahead_size = 1024,	//前向缓存区的大小，必须是8的倍数
    .block_cycles = 100000,//100000,  //上一个块到下一个块擦除的间隔周期
	
	
	
//	.read_size = 4,       //读取块的最小大小
//    .prog_size = 4,  
//    .block_size = 4096,   //可擦快的大小，必须是读取块和程序块的倍数
//    .block_count = 1024,    //可擦快的数目
//    .cache_size = 256,     //读取数据缓存区，必须是读取块和程序块的倍数，缓存区大则新=性能好
//    .lookahead_size = 256, //前向缓存区的大小，必须是8的倍数
//    .block_cycles = 1000,//90000, //上一个块到下一个块擦除的间隔周期，建议数值在100-1000之间	
	
};


/* littlefs文件系统挂载（初始化调用） */
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

/* 读取掉电数据（上电获取） */
void Read_Data_From_W25qxx(uint8_t* data, uint16_t size, const char *path)
{
	if(lfs_mount_fail == 0)
	{
		lfs_file_open(&lfs, &lfs_file_cfg, path, LFS_O_RDWR | LFS_O_CREAT);
		lfs_file_read(&lfs, &lfs_file_cfg, data, size);
		lfs_file_close(&lfs, &lfs_file_cfg);
	}
}

/* 保存IOT数据到外部Flash */
void Write_Data_To_W25qxx(uint8_t *data, uint16_t size, const char *path)
{
	if(lfs_mount_fail == 0)
	{
		lfs_file_open(&lfs, &lfs_file_cfg, path, LFS_O_RDWR | LFS_O_CREAT);
		lfs_file_rewind(&lfs, &lfs_file_cfg);
		lfs_file_write(&lfs, &lfs_file_cfg, data, size);
		lfs_file_close(&lfs, &lfs_file_cfg);
	}
}
