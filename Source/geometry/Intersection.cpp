#include "TheEngine/engine.h"

void Intersection::getMouseDirection(int mx, int my, vec3 &p0, vec3 &p1)
{
    auto camera = aux::engine->getScene()->getCamera();
    auto projection = camera->getPVMatrix() * camera->getWorldTransform();
    auto modelView  = camera->getWorldTransform();

    float height = aux::engine->getViewport().y;
    float width  = aux::engine->getViewport().x;
    
    float x = -(float(mx) / width * 2.0f - 1.0f + projection[0][2]) / projection[0][0];
    float y = (float(my) / height * 2.0f - 1.0f + projection[1][2]) / projection[1][1];
    p0 = vec3(modelView[0][3],modelView[1][3],modelView[2][3]);
    p1 = modelView*vec3(x,y,1.0f);
    p1 = p0 - (p1 - p0).normalize() * camera->zFar();
}

bool Intersection::get(the::object::ptr m, int mx, int my, Intersection &intersection)
{
    vec3 p0,p1;
    getMouseDirection(mx, my, p0, p1);
    std::vector<Intersection> intersections = m->getIntersection(p0,p1);
    mat4 cameraTransform = aux::engine->getScene()->getCamera()->getWorldTransform();
    bool find = false;
    float cameraZ = -aux::engine->getScene()->getCamera()->zFar();
    //find point closest to camera

    for(auto &i:intersections)
    {
        vec3 p = i.point*m->getWorldTransform()*cameraTransform;
        if(p.z > cameraZ)
        {
            cameraZ = p.z;
            intersection = i;
            find = true;
        }
    }
    //convert coordinates to world space
    if(find)
    {
        intersection.point*=m->getWorldTransform();
        intersection.v1*=m->getWorldTransform();
        intersection.v2*=m->getWorldTransform();
        intersection.v3*=m->getWorldTransform();
    }

    return find;
}

the::object::ptr Intersection::get(int mx, int my, Intersection &intersection)
{
    the::object::ptr findedObj = nullptr;
    mat4 cameraTransform = aux::engine->getScene()->getCamera()->getWorldTransform();
    intersection.point.z = -aux::engine->getScene()->getCamera()->zFar();

    aux::engine->getRoot()->foreachChild([&](the::node::ptr c)
    {
        Intersection is;
        auto obj = the::node::ptr_cast<the::object>(c);
        if(obj!=nullptr && obj->getEnabled() && get(obj, mx, my, is))
        {
            vec3 p = is.point*cameraTransform;//intersection point, related to camera
            vec3 intersectionP = intersection.point*cameraTransform;//intersection point, related to camera
            if(p.z > intersectionP.z)
            {
                intersection = is;
                findedObj = obj;
            }
        }
    });
    return findedObj;
}