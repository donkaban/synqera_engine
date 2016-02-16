#include "TheEngine/render/fbuffer.h"
#include "TheEngine/logger.h"
#include "TheEngine/openGL.h"

the::fbuffer::fbuffer(int w, int h) : 
    abstract::object("FBO","--"),    
	width(w),
	height(h)
{
    bind();  
}
void the::fbuffer::bind()
{
    GLint  maxRBSize; 
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRBSize);
    if(maxRBSize <= width || maxRBSize <= height)
        FATAL_ERROR("[FBO] bad size : %dx%d ", width,height); 
    glGenFramebuffers(1, &_bindId);
    glBindFramebuffer(GL_FRAMEBUFFER, _bindId);
 
    glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 ,GL_TEXTURE_2D, textureID, 0);
   
    glGenRenderbuffers(1, &depthID);
    glBindRenderbuffer(GL_RENDERBUFFER, depthID);
    #ifdef RENDER_OPENGL
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,width,height);
    #else    
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16,width,height);
    #endif    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, depthID);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        FATAL_ERROR("[FBO] %dx%d id: %d not complete!", width,height,_bindId); 
    logger::debug("[FBO    ] create [%d] %dx%d  texture: %d", _bindId, width,height,textureID);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GL_CHECK("Framebuffer::Framebuffer");
}

void the::fbuffer::unbind()
{
    logger::debug("[FBO    ] delete [%d] %dx%d", _bindId, width,height); 
    glDeleteTextures(1, &textureID);
    glDeleteRenderbuffers(1, &depthID);
    glDeleteFramebuffers(1, &_bindId);
    resetID();
}

the::fbuffer::~fbuffer()
{
    unbind();
}

void the::fbuffer::setCurrent()
{
    glBindFramebuffer(GL_FRAMEBUFFER,_bindId);
   	glViewport(0,0,width,height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

uint the::fbuffer::getTextureID()  {return textureID;}
the::image::ptr the::fbuffer::getImage()
{
    the::image::ptr image = std::make_shared<the::image>();
    image->setDataFromScreen(width, height);
    return image;
}

