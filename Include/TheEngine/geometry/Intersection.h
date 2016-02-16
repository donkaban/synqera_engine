#ifndef _THE_INTERSECTION_
#define _THE_INTERSECTION_

#include "TheEngine/math/all.h"
namespace the {class object;}

struct Intersection
{
    int surfaceId;          //surface
    vec3 point;             //intersection coordinate
    vec3 v1;                //first triangle vertex
    vec3 v2;                //sencond triangle vertex
    vec3 v3;                //third triangle vertex

    Intersection()  {}
    Intersection(int surfaceId, vec3 point, vec3 v1, vec3 v2, vec3 v3) :
        surfaceId(surfaceId),
        point(point),
        v1(v1),
        v2(v2),
        v3(v3)
    {}

    static bool      get(std::shared_ptr<the::object> m, int mx, int my, Intersection &intersection);
    static std::shared_ptr<the::object> get(int mx, int my, Intersection &intersection);
    static void getMouseDirection(int mx, int my, vec3 &p0, vec3 &p1);

};





#endif