#ifndef _THE_PLANE_
#define _THE_PLANE_

#include "TheEngine/common.h"

struct Plane
{
    Plane(vec3 v0, vec3 n):
    	v0(v0),
    	n(n)
    {}
    
    vec3 v0;
    vec3 n;
};

#endif