#include "TheEngine/engine.h"
#include "TheEngine/pre.h"
#include "TheEngine/openGL.h"


the::texture::texture(int filtering, strref name) : 
    abstract::object("Texture",name),
    image(the::image::get("def::base")),
    filtering(filtering) 
{}
   
the::texture::texture(strref filename,int filtering) :
    abstract::object("Texture",filename),
    image(the::image::cachedLoad(filename)),
    filtering(filtering)
{
    load(filename,filtering);
}

the::texture::texture(const uint8_t *d, uint width, uint height, uint bpp, int filtering, strref name) :
    abstract::object("Texture",name),
    filtering(filtering)
{
    image = the::image::make(d,width,height,bpp,name);
    bind();
}

the::texture::texture(uint w, uint h, uint b) :
    abstract::object("Texture", aux::format("%dx%dx%d",w,h,b))
{

  image = the::image::make(w,h,b);
}
 

the::texture::texture(the::image::cref im, strref tag) :
    abstract::object("Texture", tag),
    image(im)
{
    bind();
}    

the::texture::~texture()
{
   unbind();
}

void the::texture::load(strref filename, int filtering)
 {
    image = the::image::cachedLoad(filename);
    bind();
 }

void the::texture::initDefaults()
{
    logger::info("[Texture] create defaults...");
    addToCache(the::texture::make(the::image::get("def::black"),"def::black"));
    addToCache(the::texture::make(the::image::get("def::red")  ,"def::red"));
    addToCache(the::texture::make(the::image::get("def::green"),"def::green"));
    addToCache(the::texture::make(the::image::get("def::blue") ,"def::blue"));
    addToCache(the::texture::make(the::image::get("def::base") ,"def::base"));
    addToCache(the::texture::make(the::image::get("def::white"),"def::white"));
    addToCache(the::texture::make(the::image::get("def::transparent"),"def::transparent"));
     
}

void the::texture::log()
{
    logger::debug("[Texture] '%s' %dx%dx%d", getTag().c_str(), image->getWidth(), image->getHeight(), image->getBpp());
}    

static const GLenum filter[] = {GL_LINEAR,GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR_MIPMAP_LINEAR};
static const bool   mipmap[] = {false, true, true};

void the::texture::bind()
{
    auto width  = image->getWidth();
    auto height = image->getHeight();
    auto bpp    = image->getBpp(); 
    logger::debug("[Texture] create '%s' %dx%dx%d [%d]",getTag().c_str(), width,height,bpp, filtering);
    glGenTextures(1, &_bindId);
    glBindTexture(GL_TEXTURE_2D, _bindId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL::BPP(bpp), width, height, 0, GL::BPP(bpp), GL_UNSIGNED_BYTE, image->getDataPtr());
    #ifdef RENDER_OPENGL
        GLfloat max_aniso;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);
    #endif

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter[filtering]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  

    if(mipmap[filtering]) glGenerateMipmap(GL_TEXTURE_2D);
   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    GL_CHECK("the::texture::bind");
}

void the::texture::unbind()
{
    if(!glIsTexture(_bindId)) return; 
    auto width  = image->getWidth();
    auto height = image->getHeight();
    auto bpp    = image->getBpp(); 
   
    logger::debug("[Texture] remove '%s' %dx%dx%d [%d]",getTag().c_str(), width,height,bpp, filtering);
   
    glDeleteTextures(1,&_bindId);
    resetID();
    GL_CHECK("the::texture::unbind");
}