#ifndef _MESH_H_
#define _MESH_H_

#include "TheEngine/objects/object.h"
namespace the {
class mesh : 
    public the::object
{
    THE_SHARED(mesh)
public:
    mesh(); 
    mesh(strref);
    virtual ~mesh();
    
    virtual void serialize(pugi::xml_node &)         override ;
    virtual void deserialize(const pugi::xml_node &) override ;
    virtual void onRender(render::state &);
    virtual void load(strref);
    virtual void update() {the::node::update();}
    virtual void log();
    
    static ptr load(strref, strref);
   
    static ptr createPlane(vec2::cref);
    static ptr createTriangle(vec3::cref, vec3::cref, vec3::cref);
    static ptr createCube(vec3::cref);
    static ptr createMonoCube(vec3::cref);
    static ptr createSlicedPlane(vec2::cref dim, vec2::cref pow);
    static ptr createSphere(float, int);
    static ptr createTorus(float innerRadius, float outterRadius, uint sides, uint rings);
};
}

#endif