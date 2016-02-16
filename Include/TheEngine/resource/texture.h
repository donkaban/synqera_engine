#ifndef _THE_TEXTURE_
#define _THE_TEXTURE_

#include "TheEngine/common.h"
#include "TheEngine/resource/image.h"
namespace the {

class texture :  
    public abstract::object,
    public abstract::gapi, 
    public ICacheable<texture>
{
    THE_SHARED(texture)
public:
    virtual void bind()     override; 
    virtual void unbind()   override;
    virtual void log()      override;
  
    virtual ~texture();
    explicit texture(the::image::cref, strref tag = "--");

    the::image::cref getImage() const              {return image;}
    void             setImage(the::image::cref im) {unbind(); image = im;    bind();}
    void             setFiltering(int f)           {unbind(); filtering = f; bind();}

    texture(int filtering, strref tag="--");
    texture(strref, int filtering = 0);
    texture(uint, uint, uint);
    texture(const uint8_t *, uint width, uint height, uint bpp, int filtering = 0, strref name="--");
    
    
    void load(strref, int filtering = -1);
        
    static void initDefaults();
    friend  std::ostream &operator<<(std::ostream &o, const texture &a) {o << a.tostring();return o;}

private:
    the::image::ptr  image;
    int  filtering  = 0;
};
}
#endif 
