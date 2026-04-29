//user_triangle.h
#ifndef __USER_TRIANGLE_H
#define __USER_TRIANGLE_H
#include "stdint.h"
#include "stdbool.h"

struct vector3{

    float x_;
    float y_;
    float z_;
};

struct vector2{

    float x_;
    float y_;
};

bool point_in_triangle(struct vector2 *A, struct vector2 *B, struct vector2 *C, struct vector2 *P);
#endif /*__USER_TRIANGLE_H*/
