#ifndef __GEL_H
#define __GEL_H
#include <stdint.h>

struct gel_table{

    uint16_t cood_x;
    uint16_t cood_y;
};

//Resco 3200K
extern const struct gel_table r32_color_correction[];
extern const struct gel_table r32_cal_color[];
extern const struct gel_table r32_storaro_selection[];
extern const struct gel_table r32_cinelux[];

//Resco 5600K
extern const struct gel_table r56_color_correction[];
extern const struct gel_table r56_cal_color[];
extern const struct gel_table r56_storaro_selection[];
extern const struct gel_table r56_cinelux[];

//LEE 3200K
extern const struct gel_table l32_color_correction[];
extern const struct gel_table l32_color_filters[];
extern const struct gel_table l32_600_series[];
extern const struct gel_table l32_cosmetic_fliter[];
extern const struct gel_table l32_700_series[];

//LEE 5600K
extern const struct gel_table l56_color_correction[];
extern const struct gel_table l56_color_filters[];
extern const struct gel_table l56_600_series[];
extern const struct gel_table l56_cosmetic_fliter[];
extern const struct gel_table l56_700_series[];
extern const uint16_t gel_length[];

#endif /*__GEL_H*/
