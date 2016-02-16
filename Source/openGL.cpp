#include "TheEngine/openGL.h"
#include "TheEngine/logger.h"
#include "TheEngine/math/all.h"

#include <unordered_map> 

static const char * OK_MSG  = "OK   ";
static const char * ERR_MSG = "ERROR";

static std::unordered_map<GLenum, std::string> ERR_TABLE = 
{
    {GL_INVALID_ENUM,                   "GL_INVALID_ENUM"},
    {GL_INVALID_VALUE,                  "GL_INVALID_VALUE"},
    {GL_INVALID_OPERATION,              "GL_INVALID_OPERATION"},
    {GL_OUT_OF_MEMORY,                  "GL_OUT_OF_MEMORY"},
    {GL_INVALID_FRAMEBUFFER_OPERATION,  "GL_INVALID_FRAMEBUFFER_OPERATION"},
}; 

static const char * _err() {return (GL_NO_ERROR == glGetError()) ? OK_MSG : ERR_MSG;}
 
static void shader_check(const char * where,int getter, GLuint id, GLenum type)
{
    static struct 
    {
        std::function<void(GLuint, GLenum, GLint *)> get;
        std::function<void(GLuint, GLsizei, GLsizei *, GLchar *)> log;
    } 
    nfo[2] = {{glGetShaderiv,glGetShaderInfoLog},{glGetProgramiv,glGetProgramInfoLog}}; 
    int result = 0;
    nfo[getter].get(id, type, &result);
    if(!result)
    {
        GLint len;
        std::string msg;
        nfo[getter].get(id, GL_INFO_LOG_LENGTH, &len);
        if (len > 1)
        {
            msg.resize(static_cast<uint>(len));
            nfo[getter].log(id, len, NULL, &msg[0]);
            FATAL_ERROR("[Shader ] %s : %s ",where, msg.c_str());
        }
    }   
}



void   GL::COMPILE_CHECK(const char *w, GLuint id)  {shader_check(w, 0, id, GL_COMPILE_STATUS);}
void   GL::LINK_CHECK(GLuint id)                    {shader_check("link",1, id, GL_LINK_STATUS);}

GLenum GL::BPP(int bpp)
{
    switch(bpp)
    {
        case 32: return GL_RGBA;
        case 24: return GL_RGB;
        case 16: return GL_LUMINANCE_ALPHA;
        case 8 : return GL_LUMINANCE;
        default:
            FATAL_ERROR("[OpenGL ] bad bpp value : %d", bpp);
            return GL_RGBA;
    }
}
void GL::CHECK(const char *file, int line,const char * where)
{
    auto error = glGetError();
    if(error !=GL_NO_ERROR)
    {
        if(ERR_TABLE.find(error) != ERR_TABLE.end())
            logger::error("[OpenGL ] error [%s:%d][%s]: %s", file, line,where,ERR_TABLE[error].c_str());
        else 
            logger::error("[OpenGL ]unknown error [%s]: %d", where,error);
    }    
}

void GL::TUNE()
{
    glClearColor(.1, .1, .2, 0);
    glDepthFunc(GL_LEQUAL); glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int   GL::GET_INT  (GLenum e) {GLint val = -1;        glGetIntegerv(e, &val); return val;}
bool  GL::GET_BOOL (GLenum e) {GLboolean val = false; glGetBooleanv(e, &val); return val;}
float GL::GET_FLOAT(GLenum e) {GLfloat val = -1.0;    glGetFloatv(e, &val);   return val;}

#define _INT(e) {auto r = GL::GET_INT(e) ; logger::extra("[%s] %s : %d",_err(), #e,r);} 
#define _STR(e) {auto r = glGetString(e);  logger::extra("[%s] %s : %s",_err(), #e,r);} 
#define _BOL(e) {auto r = GL::GET_BOOL(e); logger::extra("[%s] %s : %s",_err(), #e,r ? "TRUE" : "FALSE");} 

void GL::SHOW_INFO()
{
    _STR(GL_VENDOR); 
    _STR(GL_VERSION); 
    _STR(GL_RENDERER);  
    _STR(GL_SHADING_LANGUAGE_VERSION); 
    _BOL(GL_SHADER_COMPILER);
    _INT(GL_ALPHA_BITS);
    _INT(GL_DEPTH_BITS);
    _INT(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
    _INT(GL_MAX_TEXTURE_IMAGE_UNITS);
    _INT(GL_MAX_TEXTURE_SIZE);
    _INT(GL_MAX_CUBE_MAP_TEXTURE_SIZE);
    _INT(GL_NUM_COMPRESSED_TEXTURE_FORMATS);
    _INT(GL_NUM_SHADER_BINARY_FORMATS);
    _INT(GL_PACK_ALIGNMENT);
    _INT(GL_SAMPLE_BUFFERS);
    _INT(GL_MAX_RENDERBUFFER_SIZE);
    _INT(GL_MAX_VARYING_VECTORS); 
    _INT(GL_MAX_VERTEX_ATTRIBS);
    _INT(GL_MAX_VERTEX_UNIFORM_VECTORS);
    _INT(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
#ifndef RENDER_OPENGLES2    
    _INT(GL_MINOR_VERSION);
    _INT(GL_MAX_DRAW_BUFFERS);
    _INT(GL_MAX_ELEMENTS_INDICES);
    _INT(GL_MAX_ELEMENTS_VERTICES);
    _INT(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS);
    _INT(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS);
    _INT(GL_MAX_GEOMETRY_UNIFORM_BLOCKS);
    _INT(GL_MAX_SERVER_WAIT_TIMEOUT);
    _INT(GL_MAX_VARYING_FLOATS);
    _INT(GL_MAX_VIEWPORTS);
    _INT(GL_MAX_VARYING_COMPONENTS);
    _INT(GL_MAX_RECTANGLE_TEXTURE_SIZE);
    _INT(GL_MAX_3D_TEXTURE_SIZE);
    _INT(GL_MAX_ARRAY_TEXTURE_LAYERS);
    _INT(GL_MAX_COLOR_TEXTURE_SAMPLES);
    _INT(GL_MAX_COMBINED_UNIFORM_BLOCKS);
    _INT(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS);
    _INT(GL_NUM_PROGRAM_BINARY_FORMATS);
    _INT(GL_CONTEXT_FLAGS);
    _INT(GL_DOUBLEBUFFER);
#endif
    auto ogl_ext = std::string((const char *)glGetString(GL_EXTENSIONS));
    the::math::replace<std::string>(ogl_ext, " ","\n    "); ogl_ext = "    " + ogl_ext;
    logger::extra("\n[OpenGL ] extensions :\n\n%s", ogl_ext.c_str()); 
}
namespace GL
{
    template<> void uniform<int  >::set(const int &v)   const  {glUniform1i(_id, v);}
    template<> void uniform<float>::set(const float &v) const  {glUniform1f(_id, v);}
    template<> void uniform<vec3 >::set(const vec3 &v)  const  {glUniform3f(_id,v.x,v.y,v.z);}
    template<> void uniform<vec4 >::set(const vec4 &v)  const  {glUniform4f(_id,v.x,v.y,v.z,v.w);}
    template<> void uniform<mat3 >::set(const mat3 &v)  const  {glUniformMatrix3fv(_id, 1,GL_FALSE, v.ptr());}
    template<> void uniform<mat4 >::set(const mat4 &v)  const  {glUniformMatrix4fv(_id, 1,GL_FALSE, v.ptr());}
}


