#include "TheEngine/openGL.h"
#include "TheEngine/material.h"
#include "TheEngine/render/render.h"
#include "TheEngine/logger.h"
#include "TheEngine/filesystem/stream.h"
#include "TheEngine/pre.h"

void the::material::bind ()
{
    auto v_src = vertexShader.c_str();
    auto f_src = fragmentShader.c_str();
    auto v_shader = glCreateShader(GL_VERTEX_SHADER);    
    auto f_shader = glCreateShader(GL_FRAGMENT_SHADER);  
     _bindId = glCreateProgram(); 
    if(v_shader == 0 || f_shader == 0 || _bindId == 0) 
       FATAL_ERROR("[Material] can't create shader");
   
    glShaderSource(v_shader, 1, &v_src, NULL); 
    glShaderSource(f_shader, 1, &f_src, NULL); 
    glCompileShader(v_shader);  GL::COMPILE_CHECK(vertexFile.c_str(),   v_shader);
    glCompileShader(f_shader);  GL::COMPILE_CHECK(fragmentFile.c_str(), f_shader);
    glAttachShader(_bindId, v_shader);
    glAttachShader(_bindId, f_shader);
    glLinkProgram(_bindId);     GL::LINK_CHECK(_bindId);
    glDetachShader(_bindId, v_shader);
    glDetachShader(_bindId, f_shader);
    glDeleteShader(v_shader);
    glDeleteShader(f_shader);
    GL_CHECK("the::material::bind");
}
void the::material::unbind()
{
    if(!glIsProgram(_bindId)) return;
    glDeleteProgram(_bindId);
    resetID();
}

void the::material::lookup()
{
    #define DEF_PARAM(x) defParameters[x]  = glGetUniformLocation(_bindId,x)
    
    attributes[NDX::POSITION]    = glGetAttribLocation(_bindId,"position");
    attributes[NDX::TEXCOORD]    = glGetAttribLocation(_bindId,"texcoord");
    attributes[NDX::TANGENT]     = glGetAttribLocation(_bindId,"tangent");
    attributes[NDX::BINORMAL]    = glGetAttribLocation(_bindId,"binormal");
    attributes[NDX::NORMAL]      = glGetAttribLocation(_bindId,"normal");
    attributes[NDX::BONE_ID]     = glGetAttribLocation(_bindId,"bone_id");
    attributes[NDX::BONE_WEIGHT] = glGetAttribLocation(_bindId,"bone_weight");

    samplers[0] = glGetUniformLocation(_bindId, "texture0");
    samplers[1] = glGetUniformLocation(_bindId, "texture1");
    samplers[2] = glGetUniformLocation(_bindId, "texture2");
    samplers[3] = glGetUniformLocation(_bindId, "texture3");
    cubemaps[0] = glGetUniformLocation(_bindId, "cubemap0");
    cubemaps[1] = glGetUniformLocation(_bindId, "cubemap1");
    cubemaps[2] = glGetUniformLocation(_bindId, "cubemap2");
    cubemaps[3] = glGetUniformLocation(_bindId, "cubemap3");

    DEF_PARAM("modelMatrix");
    DEF_PARAM("prjMatrix");
    DEF_PARAM("viewMatrix");
    DEF_PARAM("inverseViewMatrix");
   
    DEF_PARAM("prjViewMatrix");

    DEF_PARAM("time");
    DEF_PARAM("eyePosition");
    DEF_PARAM("lightPosition");
    DEF_PARAM("lightAmbient");
    DEF_PARAM("lightDiffuse");
    DEF_PARAM("lightSpecular");
    DEF_PARAM("bones");
   
    for(const auto &param : userParameters) 
        userParameters[param.first] = glGetUniformLocation(_bindId,param.first.c_str());
    GL_CHECK("the::material::lookup()");

}

#if defined(RENDER_OPENGL)
    static std::string _intro = "#version 120\n#define highp\n#define highp\n#define lowp\n";
#elif defined(RENDER_OPENGLES2) || defined(RENDER_OPENGLES3)
    static std::string _intro = "precision highp float;\n";
#endif

void the::material::setDefines()
{
    for(const auto &def : defines)
    {
        vertexShader    = "#define " + def + "\n" + vertexShader ;
        fragmentShader  = "#define " + def + "\n" + fragmentShader;
    }
    vertexShader     = _intro + vertexShader;
    fragmentShader   = _intro + fragmentShader;

    //logger::debug("VERTEX:\n%s\n",vertexShader.c_str());
    //logger::debug("FRAGMENT:\n%s\n",fragmentShader.c_str());


}
void the::material::addDefTexture(int n, strref tag)
{
    if(n<0 || n > 3)
        FATAL_ERROR("[Material] try set non exist def texture %d", n);
    defTextures[n] = tag;
    setDefines();

}

the::material::material(strref vsh, strref fsh, strref t): 
    abstract::object("Material",t),
    vertexFile(vsh), 
    fragmentFile(fsh)
{
    auto v = aux::dataPath + vertexFile;
    auto f = aux::dataPath + fragmentFile;
    if(!the::filesystem::is_file(v) && !the::filesystem::is_file(f))
       FATAL_ERROR("[Material] can't open shader file");
    vertexShader   = the::filesystem::load_as_string(v);
    fragmentShader = the::filesystem::load_as_string(f);
    setDefines();
    bind();
    lookup();
    logger::debug("[Material] create '%s'",getTag().c_str());

}

the::material::material(precompiled::shader::shader &sh,strref d): 
    abstract::object("Material",sh.name),
    vertexShader(sh.vertex), 
    fragmentShader(sh.fragment)
{
    std::istringstream defs(d);
    std::string def;
    while(std::getline(defs,def, ';')) defines.push_back(def);
    setDefines();
    bind();
    lookup();
    logger::debug("[Material] create '%s' [%s]",getTag().c_str(),d.c_str());

}

the::material::material(const pugi::xml_node &n) : 
    abstract::object("Material",n.attribute("name").value())
{
    deserialize(n);
    setDefines();
    bind();
    lookup();
    logger::debug("[Material] load from xml '%s'",getTag().c_str());
}

the::material::material() : abstract::object("Material","--") 
{}

the::material::~material() 
{
    unbind();
}

void the::material::serialize(pugi::xml_node &node)
{
    logger::error("call not implemented the::material::serialize()");
}

void the::material::deserialize(const pugi::xml_node &node)
{
    unbind();
    setTag(node.attribute("name").value());
    std::istringstream defs(node.attribute("define").value());
    std::string def;
    while(std::getline(defs,def, ';')) defines.push_back(def);

    vertexFile      = node.attribute("vertex").value(); 
    fragmentFile    = node.attribute("fragment").value(); 
    vertexShader    = the::filesystem::load_as_string(aux::dataPath + vertexFile);
    fragmentShader  = the::filesystem::load_as_string(aux::dataPath + fragmentFile);
    if(!node.attribute("texture0").empty()) defTextures[0] = node.attribute("texture0").value();
    if(!node.attribute("texture1").empty()) defTextures[1] = node.attribute("texture1").value();
    if(!node.attribute("texture2").empty()) defTextures[2] = node.attribute("texture2").value();
    if(!node.attribute("texture3").empty()) defTextures[3] = node.attribute("texture3").value();
   
    if(!node.attribute("cubemap0").empty()) defCubemaps[0] = node.attribute("cubemap0").value();
    if(!node.attribute("cubemap1").empty()) defCubemaps[1] = node.attribute("cubemap1").value();
    if(!node.attribute("cubemap2").empty()) defCubemaps[2] = node.attribute("cubemap2").value();
    if(!node.attribute("cubemap3").empty()) defCubemaps[3] = node.attribute("cubemap3").value();
   
    for(auto param = node.child("parameter"); param; param = param.next_sibling("parameter"))
    {    
        const std::string paramName = param.attribute("name").value();
        const std::string paramType = param.attribute("type").value();
        userParameters[paramName] = -1;
        if(paramType=="float") 
            float_param_def[paramName] = param.attribute("val").as_float(); 
        else if(paramType=="vec4")
            vec4_param_def[paramName] = aux::deserialize<vec4>(param);
        else 
            logger::error("Can't parse material parameter type '%s'",paramType.c_str());}       
}

void the::material::initDefaults()
{
    logger::info("[Material] create defaults...");
    ;  
    the::material::addToCache(std::make_shared<the::material>(precompiled::shader::line),"def::line");
    the::material::addToCache(std::make_shared<the::material>(precompiled::shader::base),"def::base");
    the::material::addToCache(std::make_shared<the::material>(precompiled::shader::alphatest),"def::alphatest");
    the::material::addToCache(std::make_shared<the::material>(precompiled::shader::base,"SKINNING;"),"def::bone");
    the::material::addToCache(std::make_shared<the::material>(precompiled::shader::base,"LIGHTING;"),"def::light");
    the::material::addToCache(std::make_shared<the::material>(precompiled::shader::base,"LIGHTING;CUBEMAP"),"def::cubemap");
    the::material::addToCache(std::make_shared<the::material>(precompiled::shader::base,"SKINNING;LIGHTING;"),"def::bone_light");
    the::material::addToCache(std::make_shared<the::material>(precompiled::shader::base,"SKINNING;LIGHTING;CUBEMAP"),"def::bone_cubemap");
   
    the::material::addToCache(std::make_shared<the::material>(precompiled::shader::text),"def::text");
    the::material::addToCache(std::make_shared<the::material>(precompiled::shader::simple_text),"def::simple_text");
    the::material::addToCache(std::make_shared<the::material>(precompiled::shader::mix),"def::mix");
    the::material::addToCache(std::make_shared<the::material>(precompiled::shader::mix,"SKINNING;"),"def::bone_mix");

    the::material::get("def::text")->addUserParam("color",  vec4(1, 1, 1, 1));
    the::material::get("def::text")->addUserParam("size",   vec4(0, 0, 0, 0));
    the::material::get("def::text")->addUserParam("offset", vec4(0, 0, 0, 0));
    the::material::get("def::text")->addUserParam("blend_size", 0.0f);
    the::material::get("def::mix")     -> addUserParam("mixer", 0.0f);
    the::material::get("def::bone_mix")-> addUserParam("mixer", 0.0f);
    the::material::get("def::simple_text")->addUserParam("color",  vec4(1, 1, 1, 1));
     
}

void the::material::log()
{
    logger::debug("[Material] '%s':", getTag().c_str());
    ;  
    std::string uni = "";
    std::string def = "";
    std::string tex = "";
    std::string cub = "";
    
    for(const auto &d : defines)       def = def += d + " "; 
    for(const auto &u : defParameters) if(u.second != -1) uni += (u.first + " ");
    for(const auto &t : defTextures)   tex += t + " ";
    for(const auto &t : defCubemaps)   cub += t + " ";
 
    logger::extra("formula  : %s",def.c_str());    
    logger::extra("uniforms : %s",uni.c_str());    
  
    for(const auto &param : userParameters)
    {
        if (param.second != -1)
        {    
            auto name = param.first;
            std::string v = "";
            auto fparam = float_param_def.find(name);
            auto vparam = vec4_param_def.find(name);
            if(fparam != float_param_def.end()) v = aux::format("[%.1f]",fparam->second); 
            if(vparam != vec4_param_def.end())  v = aux::format("[%.1f,%.1f,%.1f,%.1f]", vparam->second.x,vparam->second.y,vparam->second.z,vparam->second.w); 
            logger::extra("user     : %s -> %s", name.c_str(),v.c_str());
        }
    }   
    logger::extra("textures : %s",tex.c_str());    
    logger::extra("cubemaps : %s",cub.c_str());    
    
         
}

void the::material::addUserParam(strref name, float val)    
{
    userParameters[name]  = -1; 
    float_param_def[name] = val; 
    lookup();
}
void the::material::addUserParam(strref name, const vec4 &val) 
{
    userParameters[name] = -1; 
    vec4_param_def[name] = val; 
    lookup(); 
}
  
