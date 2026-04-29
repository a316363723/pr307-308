#ifndef __GEL_H
#define __GEL_H
#include <stdint.h>

struct gel_coord{

    uint16_t cood_x;
    uint16_t cood_y;
};

struct gel_table{
    const struct gel_coord* coord_table;
    const uint8_t  table_size;
};

extern const struct gel_table gel_table_resco[2][4];
extern const struct gel_table gel_table_lee[2][5];

#endif /*__GEL_H*/
