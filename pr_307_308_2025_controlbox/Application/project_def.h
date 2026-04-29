#ifndef PROJECT_DEF_H
#define PROJECT_DEF_H
#include "stddef.h"

enum light_curve
{
    LIGHT_CURVE_LINEAR,
    LIGHT_CURVE_EXP,
    LIGHT_CURVE_LOG,
    LIGHT_CURVE_S
};

enum light_illumination_mode
{
    LIGHT_ILL_CONSTANT,
    LIGHT_ILL_MAX
};

enum light_fan_mode
{
    LIGHT_FAN_FIXTURE,
    LIGHT_FAN_AUTO,
    LIGHT_FAN_HIGH,
    LIGHT_FAN_LOW,
    LIGHT_FAN_MAX,
    LIGHT_FAN_OFF,
};

enum light_file_type
{
    LIGHT_FILE_FIRMWARE,
    LIGHT_FILE_CFX,
};

struct light_cfg
{
    int frq;
    enum light_curve curve;
    enum light_illumination_mode illumination;
    struct {
        enum light_fan_mode mode;
        int speed;
    }fan;
};

struct light_info
{
    int hardware_version;
    int software_version;
    float temp;
};

#endif
