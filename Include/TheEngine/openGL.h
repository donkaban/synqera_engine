#ifndef _THE_OPENGL_
#define _THE_OPENGL_

#include <iostream>
#if defined(RENDER_OPENGLES2)  
    #include <GLES2/gl2.h>
#elif defined(RENDER_OPENGLES3)
    #include <GLES3/gl3.h>
#elif defined(RENDER_OPENGL)
    #define GL_GLEXT_PROTOTYPES
    #include <GL/gl.h>
    #include <GL/glext.h>
#else
    #error RENDER_* NOT DEFINED!
#endif    

namespace GL
{
   
    typedef GLuint  hdl;
    typedef GLfloat scalar;
    typedef GLbyte  byte;
    typedef GLubyte ubyte;
    
    GLenum BPP(int bpp);
    void   TUNE();
    void   SHOW_INFO();
    void   CHECK(const char *file, int line, const char * where = "somewhere over the rainbow ...");
    void   COMPILE_CHECK(const char *, GLuint);
    void   LINK_CHECK(GLuint);
    int    GET_INT  (GLenum); 
    bool   GET_BOOL (GLenum); 
    float  GET_FLOAT(GLenum);
   


template <typename T>
struct uniform
{
public:
    uniform(const std::string & name, const T& def) : 
    _name(name),
    _def(def) 
    {}    

    void set(const T &) const;
    const T & def() const {return _def;}
    bool find(GL::hdl matID) 
    {
        _id = glGetUniformLocation(matID, _name.c_str());
        return (_id != -1);
    } 
private:
    std::string _name;
    GL::hdl     _id;
    T           _def;
};



#ifdef GL_DEBUG
    #define GL_CHECK(msg) GL::CHECK(__FILE__,__LINE__, msg) 
#else
    #define GL_CHECK(msg) 
#endif



}







#endif