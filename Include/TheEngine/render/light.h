#ifndef _THE_LIGHT_
#define _THE_LIGHT_

#include "TheEngine/common.h"
#include "TheEngine/math/all.h"
#include "TheEngine/objects/node.h"
namespace the {
class light : public the::node
{

public:
    using ptr  = std::shared_ptr<light>;
    using cref = const ptr &; 

    light();
    light(const pugi::xml_node &n);
    virtual ~light() = default;

    virtual void onRender(render::state &);
    virtual void serialize(pugi::xml_node &) ;
    virtual void deserialize(const pugi::xml_node &);

    void setDiffuseColor(const vec4 &);
    void setSpecularColor(const vec4 &);
    void setAmbientColor(const vec4 &);
    void setRadius(float);

    const vec4 & getDiffuseColor()  const;
    const vec4 & getSpecularColor() const;
    const vec4 & getAmbientColor()  const;
    float getRadius()               const;

private:
    vec4  _diffuse  = vec4(1,1,1,1);
    vec4  _specular = vec4(1,1,1,1);
    vec4  _ambient  = vec4(1,1,1,1);
    float _radius   = 1.0;

};

}


#endif
