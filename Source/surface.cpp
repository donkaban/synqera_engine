#include "TheEngine/surface.h"
#include "TheEngine/logger.h"
#include "TheEngine/engine.h"
#include "TheEngine/openGL.h"


the::surface::surface() : the::surface("--") 
{}

the::surface::surface(strref tag): 
    abstract::object("Surface",tag) 
{
    setDefValues();
}

the::surface::surface(strref tag, the::material::cref mat) : 
    the::surface(tag)
{
    setMaterial(mat);
}    

the::surface::surface(const std::initializer_list<the::surface::ndx_t> &l, strref tag, bool bones) : 
    the::surface(tag)
{
    hasBones(bones);
    indecies.insert(indecies.end(), l.begin(), l.end());
}

the::surface::surface(std::vector<the::surface::ndx_t> &&l, strref tag, bool bones) : 
    the::surface(tag)

{
    hasBones(bones);
    indecies.insert(indecies.end(), l.begin(), l.end());
}

the::surface::~surface() 
{
    unbind();
}

void the::surface::log()
{
    // !todo

} 

size_t the::surface::getNumTriangles() const {return indecies.size()/3;}
void the::surface::bind()
{
    uint sz = getSize();
    if (sz == 0)
        return;
    glGenBuffers(1,&_bindId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_bindId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sz * sizeof(uint16_t), &indecies[0],GL_STATIC_DRAW);
    GL_CHECK("the::surface::bind");
}

void the::surface::unbind()
{
    if(!glIsBuffer(_bindId)) return;
    glDeleteBuffers(1,&_bindId);
    resetID();
    GL_CHECK("the::surface::unbind");
}

static int xx; 
#define GET_PARAM(x) xx = material->defParameters[x]; if(xx!=-1)

void the::surface::setBones(const render::state & state, const std::vector<mat4> &transforms)
{
    if(_hasBones && _enabled)
    {    
        GET_PARAM("bones")  glUniformMatrix4fv(xx,transforms.size(), GL_FALSE, static_cast<const GLfloat*>(transforms[0].ptr()));
    }
    GL_CHECK("the::surface::setBones");

    ///TODO: insert identity bones vector
}

void the::surface::setDefaultParameters(const render::state &state)
{
    
    if(!_enabled) return;  
    if(_blending) glEnable (GL_BLEND); 
    else          glDisable(GL_BLEND);

    glUseProgram(material->getID());

    GET_PARAM("time")          glUniform1f(xx, state.currentTime);
    GET_PARAM("eyePosition")   glUniform3f(xx, state.eyePosition.x,state.eyePosition.y,state.eyePosition.z);
    GET_PARAM("lightPosition") glUniform3f(xx, state.lightPosition.x,state.lightPosition.y,state.lightPosition.z);
    GET_PARAM("lightDiffuse")  glUniform4f(xx, state.lightDiffuseColor.r, 
                                               state.lightDiffuseColor.g, 
                                               state.lightDiffuseColor.b, 
                                               state.lightDiffuseColor.a);
    GET_PARAM("lightSpecular") glUniform4f(xx, state.lightSpecularColor.r, 
                                               state.lightSpecularColor.g, 
                                               state.lightSpecularColor.b, 
                                               state.lightSpecularColor.a);
   
    GET_PARAM("modelMatrix")       glUniformMatrix4fv(xx, 1,GL_FALSE,state.modelMatrix.ptr());
    GET_PARAM("prjMatrix")         glUniformMatrix4fv(xx, 1,GL_FALSE,state.projectionMatrix.ptr());
    GET_PARAM("viewMatrix")        glUniformMatrix4fv(xx, 1,GL_FALSE,state.viewMatrix.ptr());
    GET_PARAM("inverseViewMatrix") glUniformMatrix4fv(xx, 1,GL_FALSE,state.inverseViewMatrix.ptr());
    GET_PARAM("prjViewMatrix")     glUniformMatrix4fv(xx, 1,GL_FALSE,state.prjViewMatrix.ptr());
    
    for(auto i=0; i< 4; i++)
    {
        if(material->samplers[i]!=-1 && texture[i])
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, texture[i]->getID()); 
            glUniform1i(material->samplers[i],i);
        }
        if(material->cubemaps[i] !=-1 && cubemap[i])
        {
            glActiveTexture(GL_TEXTURE5+i);
            glBindTexture(GL_TEXTURE_CUBE_MAP,cubemap[i]->getID());
            glUniform1i(material->cubemaps[i],i+5);
        }   
    }     
    GL_CHECK("the::surface::setDefaultParameters");
}


void the::surface::setUserParameters(const render::state &state)
{
    if(!_enabled) return;
    for(const auto &v : float_params) glUniform1f(material->userParameters[v.first], v.second);
    for(const auto &v : vec4_params)  glUniform4f(material->userParameters[v.first], v.second.x,v.second.y,v.second.z,v.second.w);
    GL_CHECK("the::surface::setUserParameters");
}



void the::surface::draw()
{
    static int xx;
    #define SET_ATTR(x)  xx = material->attributes[x];if(xx!=-1)\
    {glVertexAttribPointer(xx,Vertex::SIZE[x],GL_FLOAT,GL_FALSE,sizeof(Vertex),(const void *) Vertex::OFFSET[x]);\
     glEnableVertexAttribArray(xx);}    
   
    if(!_enabled) return;
    SET_ATTR(NDX::POSITION)
    SET_ATTR(NDX::TEXCOORD)
    SET_ATTR(NDX::BONE_ID)
    SET_ATTR(NDX::BONE_WEIGHT)
    SET_ATTR(NDX::TANGENT)
    SET_ATTR(NDX::BINORMAL)
    SET_ATTR(NDX::NORMAL)
 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, getID());
    glDrawElements(GL_TRIANGLES, getSize(),GL_UNSIGNED_SHORT,0);        
    GL_CHECK("the::surface::draw");
    #undef SET_ATTR    
}


   
void the::surface::add(std::initializer_list<the::surface::ndx_t> l, strref t)
{
    indecies.insert(indecies.end(), l.begin(), l.end());
    setTag(t);
}
void the::surface::add(std::initializer_list<the::surface::ndx_t> l)
{
    indecies.insert(indecies.end(), l.begin(), l.end());
}
void the::surface::add(const std::vector<the::surface::ndx_t> &l)
{
    indecies.insert(indecies.end(), l.begin(), l.end());
}
void the::surface::add(const the::surface::ndx_t s) 
{
    indecies.emplace_back(s);
}
void the::surface::clear() 
{ 
    unbind(); 
    indecies.clear(); 
};

bool the::surface::getEnabled()      const   {return _enabled;}
uint the::surface::getSize()         const   {return indecies.size();}
bool the::surface::getBlending()     const   {return _blending;}
bool the::surface::hasBones()        const   {return _hasBones;}
bool the::surface::getIntersection() const   {return _intersection;}
int  the::surface::getRenderOrder()  const   {return _renderOrder;}

the::material::ptr the::surface::getMaterial()     const {return material;}
the::texture::ptr  the::surface::getTexture(int i) const {return texture[i];}

void the::surface::setEnabled(bool mode)         {_enabled = mode;}
void the::surface::setIntersection(bool mode)    {_intersection = mode;}
void the::surface::setBlending(bool mode)        {_blending = mode;}
void the::surface::hasBones(bool mode)           {_hasBones = mode;}
void the::surface::setRenderOrder(int order)     {_renderOrder =order;}
void the::surface::setTexture(int i, the::texture::cref t) {texture[i] = t;}
void the::surface::setTexture(int i, strref name)
{
    if(!the::texture::check(name)) 
        the::texture::addToCache(std::make_shared<the::texture>(name,aux::engine->getFilteringMode()),name);
    texture[i] = the::texture::get(name);
}

void the::surface::setCubemap(int i,strref t1,strref t2,strref t3,strref t4,strref t5,strref t6)
{
    auto t = the::cubetex::generateTag(t1, t2, t3, t4, t5, t6);
    if(the::cubetex::check(t))
        cubemap[i] = the::cubetex::get(t);
    else
        cubemap[i] = the::cubetex::addToCache(std::make_shared<the::cubetex>(t1,t2,t3,t4,t5,t6));   
}
void the::surface::setCubemap(int i, the::cubetex::cref c)
{
    cubemap[i] = c;
}
  

std::vector<the::surface::ndx_t> &the::surface::getIndecies() {return indecies;}
    
void the::surface::setUserParam(strref name, float v)        
{
    if(float_params.find(name)!= float_params.end())
        float_params[name] = v;
    else
        logger::error("can't set not exist user param %s", name.c_str());

}
void the::surface::setUserParam(strref name, const vec4 & v) {vec4_params[name]  = v;}

void the::surface::setMaterial(the::material::cref m) 
{
    material = m;
    float_params = m->float_param_def;
    vec4_params  = m->vec4_param_def;
    for(auto i = 0; i< 4; i++)
    {    
        if(!material->defTextures[i].empty())    
            texture[i] = the::texture::get(material->defTextures[i]);
        if(!material->defCubemaps[i].empty())
            cubemap[i] = the::cubetex::get(material->defCubemaps[i]);    
    }
}   

void the::surface::setDefValues()
{
    material   = the::material::get("def::base");
    for(auto i = 0; i< 4; i++)
    {
        texture[i] = the::texture::get("def::base");
        if(aux::engine)
            cubemap[i] = aux::engine->getScene()->getCubeMap();
        else
            cubemap[i] = the::cubetex::get("def::base");
    }  
}    
     
        


void the::surface::serialize(pugi::xml_node & node)    
{ 
    logger::error("call unimplemented method the::surface::serialize()");
}    
void the::surface::deserialize(const pugi::xml_node & node) 
{
    deserialize(node,aux::dataPath);
}    

void the::surface::deserialize(const pugi::xml_node & node,strref path)    
{
    logger::debug("[Surface] load from xml '%s'; path: %s", getTag().c_str(), path.c_str());
    if (!node.attribute("enabled").empty())
        _enabled = node.attribute("enabled").as_bool();
    setMaterial(the::material::get(node.attribute("material").value()));
    _blending      = node.attribute("blending").as_bool();
    _renderOrder   = node.attribute("order").as_int();
    _intersection  = node.attribute("intersection").as_bool();

    std::vector<pugi::xml_node> textures 
    {
        node.child("texture0"), 
        node.child("texture1"), 
        node.child("texture2"), 
        node.child("texture3")
    };
    std::string def_prefix = "def::";
    for(uint i = 0; i < textures.size(); i++)
    {
        if(textures[i])
        {
            auto tex = std::string(textures[i].attribute("file").value());

            if (!tex.compare(0, def_prefix.size(), def_prefix))
                setTexture(i,the::texture::get(tex));
            else
                setTexture(i, path + tex);
        }
    }
}

bool the::surface::intersectTriangle(vec3 v0, vec3 v1, vec3 v2, vec3 p, vec3 d, vec3 &point)
{
    vec3 e1,e2,h,s,q;
    float a,f,u,v;
    e1=v1-v0;
    e2=v2-v0;

    h=d.cross(e2);
    a = e1*h;

    if (a > -0.00001 && a < 0.00001)
        return false;

    f = 1/a;
    s=p-v0;
    u = f * (s*h);

    if (u < 0.0 || u > 1.0)
        return false;

    q=s.cross(e1);
    v = f * (d*q);

    if (v < 0.0 || u + v > 1.0)
        return false;

    float t = f * (e2*q);

    if (t > 0.00001) // ray intersection
    {
        vec3 e1e2 = e1.cross(e2);
        float a = -(e1e2*s);
        float b = e1e2*d;
        if(b>0) // angle between ray and triangle normal > 180
            return false;
        float r = a / b;
        point = p + r * d;

        return true;
    }
    else 
        return false;
}

