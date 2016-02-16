#include "TheEngine/render/light.h"
#include "TheEngine/logger.h"


the::light::light() :
    the::node("render::light")
{}
the::light::light(const pugi::xml_node &n) :
   the::node("render::light") 
{
    deserialize(n);
    auto pos = getWorldPosition();
    logger::debug("[Light  ] create [%.1f:%.1f:%.1f]",pos.x, pos.y,pos.z);

}
   
void the::light::onRender(render::state &state)
{
    state.lightPosition = getWorldPosition();
    state.lightDiffuseColor  = _diffuse;
    state.lightSpecularColor = _specular;
    state.lightAmbientColor  = _ambient;
}

void the::light::setDiffuseColor(const vec4 &c)  {_diffuse  = c;}

void the::light::setSpecularColor(const vec4 &c) {_specular = c;}
void the::light::setAmbientColor(const vec4 &c)  {_ambient  = c;}
void the::light::setRadius(float r)              {_radius   = r;}

const vec4 &the::light::getDiffuseColor()  const {return _diffuse;}
const vec4 &the::light::getSpecularColor() const {return _specular;}
const vec4 &the::light::getAmbientColor()  const {return _ambient;}
float       the::light::getRadius()        const {return _radius;}

void the::light::serialize(pugi::xml_node &node)
{logger::error("call not implemented the::light::serialize()");}

void the::light::deserialize(const pugi::xml_node &node)
{
    vec3 position;
    pugi::xml_node n = node.child("position"); position.deserialize(n);
    n = node.child("ambient");  _ambient.deserialize(n);
    n = node.child("diffuse");  _diffuse.deserialize(n);
    n = node.child("specular"); _specular.deserialize(n);
    setLocalPosition(position);
}   