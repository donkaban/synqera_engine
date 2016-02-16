#ifndef _THE_BOUNDBOX_
#define _THE_BOUNDBOX_

#include "TheEngine/math/all.h"

struct BoundingBox
{
    vec3 min;
    vec3 max;
    mat4 transform;

    BoundingBox(vec3 _min, vec3 _max, mat4 _transform = mat4_identity) : 
        min(_min),
        max(_max),
        transform(_transform) 
    {}
    BoundingBox(){}
};

#endif