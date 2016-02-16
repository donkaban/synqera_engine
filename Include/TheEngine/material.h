#ifndef _THE_MATERIAL_
#define _THE_MATERIAL_

#include "TheEngine/comtypes.h"
#include "TheEngine/common/abstract.h"
#include "TheEngine/common/serialize.h"
#include "TheEngine/common/cache.h"

#include "TheEngine/pre.h"
#include "TheEngine/math/all.h"
#include "TheEngine/geometry/vertex.h"


namespace the {

class material : 
    public abstract::object, 
    public abstract::gapi, 
    public serialize::xml, 
    public ICacheable<material>

{
    THE_SHARED(material)
public:
    int attributes[NDX::VERTEX_SIZE];
    int samplers[4];
    int cubemaps[4];
   
    dictionary<int>   defParameters;
    dictionary<int>   userParameters;

    dictionary<float> float_param_def;
    dictionary<vec3>  vec3_param_def;
    dictionary<vec4>  vec4_param_def;

    static void  initDefaults();
  
    material(); 
    material(const pugi::xml_node &);  
    material(strref , strref , strref t = "noname");
    material(precompiled::shader::shader &, strref defs = "");
    virtual ~material() ;

    virtual void bind()    final;
    virtual void unbind()  final;
    virtual void serialize(pugi::xml_node &)         final ;
    virtual void deserialize(const pugi::xml_node &) final;
    virtual void log() final;
  
    void addUserParam(strref, float);   
    void addUserParam(strref, const vec4 &); 
    void addDefTexture(int, strref);

  
    std::string defTextures[4] = {"","","",""};
    std::string defCubemaps[4] = {"","","",""};
  
private:
    std::string vertexShader;
    std::string fragmentShader;
    std::string vertexFile;
    std::string fragmentFile;
    
    std::vector<std::string> defines;  
   
    void lookup();
    void setDefines();
};
}
#endif 
