#include "update_module.h"
#include <string.h>
#include <stdlib.h>

struct update_device
{
    struct update_device_ops ops;
    struct {
        uint8_t progress;
        char *dev_name;
        char *file_name;
        int err_code;
        uint32_t firmware_size;
    } info;
    struct update_device *next;
};
static struct update_device* s_head = NULL;

static struct update_device *find_device(const char *dev_name)
{
    struct update_device *p = s_head;
    while(p != NULL)
    {
        if(0 == strcmp(p->info.dev_name, dev_name))
        {
            break;
        }
        p = p->next;
    }
    
    return p;
}

int update_device_regsiter(const char *dev_name, const struct update_device_ops *ops)
{ 
    if(NULL == ops || NULL == dev_name || NULL != find_device(dev_name))
        return -1;

    struct update_device *p_dev = (struct update_device *)malloc(sizeof(struct update_device));
    if(NULL == p_dev)
        return -2;
    
    p_dev->next = NULL;
    p_dev->info.err_code = 0;
    p_dev->info.dev_name = malloc(strlen(dev_name) + 1);
    p_dev->info.file_name = NULL;
    if(NULL != p_dev->info.dev_name)
    {
        strcpy(p_dev->info.dev_name, dev_name);
    }
    memcpy(&p_dev->ops, ops, sizeof(struct update_device_ops));
    
    if(NULL == s_head)
    {
        s_head = p_dev;
        return 0;
    }

    struct update_device *p = s_head;
    while(p->next != NULL)
    {
        p = p->next;
    }
    p->next = p_dev;

    return 0;
}

int update_open(const char *dev_name, const char *file_name)
{
    struct update_device *p_dev = find_device(dev_name);
    if(NULL == p_dev)
        return -1;

    p_dev->info.file_name = realloc(p_dev->info.file_name, strlen(file_name) + 1);
    strcpy(p_dev->info.file_name, file_name);
    p_dev->info.progress = 0;
    p_dev->info.err_code = 0;
    p_dev->info.err_code = p_dev->ops.open(file_name);

    return p_dev->info.err_code;
}

int update_erase(const char *dev_name, uint32_t addr, uint32_t size)
{
    struct update_device *p_dev = find_device(dev_name);
    if(NULL == p_dev)
        return -1;

    p_dev->info.firmware_size = size;
    p_dev->info.err_code = p_dev->ops.erase(addr, size);

    return p_dev->info.err_code;
}

int update_write(const char *dev_name, uint32_t addr, const uint8_t *data, uint32_t size)
{
    struct update_device *p_dev = find_device(dev_name);
    if(NULL == p_dev)
        return -1;

    p_dev->info.progress = 99 * addr / size;//Ó¦¸ÃÊÇp_dev->info.firmware_size£¿£¿ chen
    p_dev->info.err_code = p_dev->ops.write(addr, data, size);
    
    return p_dev->info.err_code;
}

int update_close(const char *dev_name,void *arg)
{
    struct update_device *p_dev = find_device(dev_name);
    if(NULL == p_dev)
        return -1;

    p_dev->info.err_code = p_dev->ops.close(arg);
    if(0 == p_dev->info.err_code)
    {
        p_dev->info.progress = 100;
    }

    return p_dev->info.err_code;
}

int update_get_info(const char *dev_name, struct update_info* info)
{
    struct update_device *p_dev = find_device(dev_name);
    if(NULL == p_dev || NULL == info)
        return -1;
    
    if(0 == p_dev->info.err_code)
    {
        info->progress = p_dev->info.progress;
        info->file_name = p_dev->info.file_name;
    }

    return p_dev->info.err_code;
}
