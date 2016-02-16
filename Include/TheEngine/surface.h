#ifndef _THE_SURFACE_
#define _THE_SURFACE_

#include "TheEngine/common.h"
#include "TheEngine/material.h"
#include "TheEngine/resource/texture.h"
#include "TheEngine/resource/cubetex.h"

#include "TheEngine/render/rstate.h"
#include "TheEngine/logger.h"
namespace the {
class surface : 
    public abstract::object,
    public abstract::gapi,
    public serialize::xml
{
public:
    using ndx_t = uint16_t;   

    surface();
    surface(strref);
    surface(strref, the::material::cref);

    surface(const std::initializer_list<ndx_t> &, strref tag = "--",bool hasBones = false);
    surface(std::vector<ndx_t> &&, strref tag = "--", bool hasBones = false); 
    
    virtual ~surface();
    
    virtual void bind()   override;
    virtual void unbind() override; 
    virtual void log()    override; 
    virtual void serialize(pugi::xml_node &)         override;
    virtual void deserialize(const pugi::xml_node &) override;
    
    void deserialize(const pugi::xml_node &, strref);
     
    uint getSize() const ;
    void add(std::initializer_list<ndx_t>, strref t);
    void add(std::initializer_list<ndx_t>);
    void add(const std::vector<ndx_t> &);
    void add(const ndx_t);
    void clear();

    bool          getEnabled()      const; 
    bool          hasBones()        const;  
    bool          getBlending()     const;  
    int           getRenderOrder()  const;  
    bool          getIntersection() const;
    the::material::ptr getMaterial()     const;
    the::texture::ptr  getTexture(int)   const;
    size_t        getNumTriangles() const;   
    void setEnabled(bool);
    void hasBones(bool); 
    void setBlending(bool); 
    void setRenderOrder(int);
    void setIntersection(bool); 
    void setMaterial(the::material::cref);
    void setTexture(int, the::texture::cref);
    void setTexture(int, strref);
    void setCubemap(int ,strref,strref,strref,strref,strref,strref);
    void setCubemap(int, the::cubetex::cref);
        
    void setUserParam(strref, float);
    void setUserParam(strref, const vec4 &);
    
    void setDefaultParameters(const render::state &);
    void setUserParameters(const render::state &);
    void setBones(const render::state &, const std::vector<mat4> &);
    void draw();
   
    bool intersectTriangle(vec3 v0, vec3 v1, vec3 v2, vec3 p, vec3 d, vec3 &point);
    std::vector<ndx_t> &   getIndecies();
   
    friend  std::ostream &operator<<(std::ostream &o, const surface &a) {o << a.tostring();return o;}

private:
    std::vector<ndx_t>   indecies;        
    
    the::material::ptr   material;   
    the::texture::ptr    texture[4];
    the::cubetex::ptr    cubemap[4];
   
    dictionary<float>  float_params;
    dictionary<vec4>   vec4_params;
   
    bool _blending      = false;
    bool _enabled       = true;
    bool _intersection  = false;
    bool _hasBones      = false;
    int  _renderOrder   = 0;  

    void setDefValues();
};
}


#endif 