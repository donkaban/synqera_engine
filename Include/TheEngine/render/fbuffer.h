#ifndef _THE_FRAMEBUFFER_
#define _THE_FRAMEBUFFER_

#include "TheEngine/common.h"
#include "TheEngine/resource/image.h"
namespace the {

class fbuffer :
    public abstract::object, 
    public abstract::gapi, 
    
    public ICacheable<fbuffer> 
{
public: 
	using ptr  = std::shared_ptr<fbuffer>;
    using cref =  const ptr &; 
   
    virtual void bind()     override; 
    virtual void unbind()   override;
    
    fbuffer(int w, int h);
    ~fbuffer();

    void            setCurrent();
    uint            getTextureID();
    the::image::ptr getImage();

private:
    int  width     = 0;
    int  height    = 0;
    uint depthID   = 0;
    uint textureID = 0;
};
}



#endif
