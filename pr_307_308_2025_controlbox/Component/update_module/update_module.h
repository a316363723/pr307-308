#ifndef UPDATE_MODELE_H
#define UPDATE_MODELE_H
#include <stdint.h>

struct update_device_ops
{
    int (*open)(const char *file_name);  // 开始升级设备
    int (*erase)(uint32_t addr, uint32_t size); // 升级擦除对应大小空间
    int (*write)(uint32_t addr, const uint8_t *data, uint32_t size); // 写数据,addr默认从0开始
    int (*close)(void *arg);  //关闭升级，写入的升级文件校验成功则返回0
};

struct update_info
{
    uint8_t progress;
    const char *file_name;
};

int update_device_regsiter(const char *dev_name, const struct update_device_ops *ops);

int update_open(const char *dev_name, const char *file_name);
int update_erase(const char *dev_name, uint32_t addr, uint32_t size);
int update_write(const char *dev_name, uint32_t addr, const uint8_t *data, uint32_t size);
int update_close(const char *dev_name, void *arg);

// 获取当前升级信息
int update_get_info(const char *dev_name, struct update_info* info);

/*
使用示例：

void app_light_thread(void *argument)
{
    update_device_ops ops = 
    {
        .open = 
        .erase = 
        .write = 
        .close =
    }
    //注册升级设备，记得做互斥
    update_device_regsiter("dev_lamp", const struct update_device_ops *ops);  
}

void app_usb_thread(void *argument)
{
    int res = update_open("dev_lamp", "test.bin");
    int res = update_erase("dev_lamp", addr, 256 * 1024);
    int res = update_write("dev_lamp", addr, data, size);
    int res = update_close("dev_lamp");
}

*/


#endif
