#include "TheEngine/resource/image.h"
#include "TheEngine/openGL.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "TheExternals/stb/stb_image.h"
#include "TheExternals/stb/stb_image_write.h"


using namespace the;

image::~image()
{
    logger::debug("[Image  ] destruct '%s' %dx%dx%d",getTag().c_str(), _width,_height,_bitPerPixel);

}
image::image() :
	abstract::object("Image","--")
{}
image::image(strref filename) :
	abstract::object("Image",filename)
{
	load(filename);
    logger::debug("[Image  ] create '%s' %dx%dx%d",getTag().c_str(), _width,_height,_bitPerPixel);
}

image::image(data_cptr d, uint width, uint height, uint bpp, strref tag) :
    abstract::object("Image", tag),
    _width(width),
    _height(height),
    _bitPerPixel(bpp),
    _bytePerPixel(bpp/8)
{
    data.assign(d, d + _bytePerPixel * _width * _height);
    logger::debug("[Image  ] create '%s' %dx%dx%d ",getTag().c_str(), _width,_height,_bitPerPixel);
}    

image::image(uint width, uint height, uint bpp) :
    abstract::object("Image", aux::format("%dx%dx%d",width,height,bpp)),
    _width(width),
    _height(height),
    _bitPerPixel(bpp),
    _bytePerPixel(bpp/8)

{
    auto size = _bytePerPixel * _width * _height;
    data.resize(size);
    logger::debug("[Image  ] create '%s' %dx%dx%d ",getTag().c_str(), _width,_height,_bitPerPixel);
}

image::cref image::cachedLoad(strref filename)
{
   if(!image::check(filename))
   	    image::addToCache(std::make_shared<image>(filename));
	return image::get(filename);
}

void  image::forEachPixel(const std::function<ivec4 (ivec4)> & f)
{
    for(auto y = 0; y < _height; y++)
        for(auto x = 0; x < _width; x++)
            setPixel(x,y,f(getPixel(x,y)));
}


ivec4 image::getPixel(int x,int y)
{
    if(x >= _width || y >= _height)
        FATAL_ERROR("[Image  ] try get pixel %d:%d from image '%s' %dx%d",x,y,getTag().c_str(), _width,_height);

    ivec4 pixel;
    int ndx = (y * (_width * _bytePerPixel)) + (x * _bytePerPixel);
    switch(_bytePerPixel)
    {
        case 1: pixel=ivec4(data[ndx],0,0,255); break;
        case 2: pixel=ivec4(data[ndx],data[ndx+1],0,255); break;
        case 3: pixel=ivec4(data[ndx],data[ndx+1],data[ndx+2],255); break;
        case 4: pixel=ivec4(data[ndx],data[ndx+1],data[ndx+2],data[ndx+3]); break;
        default:
            FATAL_ERROR("[Image  ] '%s' getPixel() error. Bad byte per pixel: %d",getTag().c_str(),_bytePerPixel);
    }
    return pixel; 
}
void  image::setPixel(int x, int y, const ivec4 & v)
{
    if(x > _width || y > _height)
        FATAL_ERROR("[Image  ] try get pixel %d:%d from image '%s' %dx%d",x,y,getTag().c_str(), _width,_height);
    int ndx = (y * (_width * _bytePerPixel)) + (x * _bytePerPixel);
    switch(_bytePerPixel)
    {
        case 1: {data[ndx] = v.x; break;}
        case 2: {data[ndx] = v.x; data[ndx+1] = v.y; break;}
        case 3: {data[ndx] = v.x; data[ndx+1] = v.y; data[ndx+2] = v.z; break;}
        case 4: {data[ndx] = v.x; data[ndx+1] = v.y; data[ndx+2] = v.z; data[ndx+3] = v.w; break;}
        default:
            FATAL_ERROR("[Image  ] '%s' setPixel() error. Bad byte per pixel: %d",getTag().c_str(),_bytePerPixel);
    }
}

void image::setData(data_cptr d, int width, int height, int bpp)
{
    _width  = width;
    _height = height;
    _bitPerPixel    = bpp;
    _bytePerPixel   = bpp/8;

    data.assign(d, d + _bytePerPixel * _width * _height);
}
void image::setData(data_type &d, int width, int height, int bpp)
{
    _width  = width;
    _height = height;
    _bitPerPixel    = bpp;
    _bytePerPixel   = bpp/8;

    data.assign(&d[0], &d[0] + _bytePerPixel * _width * _height);
}
void image::setDataFromScreen(int width, int height)
{
    logger::extra("[Image  ] create from screenbuffer %dx%dx32 size: %d bytes", width, height, 4 * width * height);
    _width = width;
    _height = height;
    _bitPerPixel    = 32;
    _bytePerPixel   = 4;

    data.resize(4 * _width * _height);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glReadPixels(0,0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
    setTag("shot");
}

uint image::getWidth()  const {return _width;}
uint image::getHeight() const {return _height;}
uint image::getBpp()    const {return _bitPerPixel;}
uint image::getBytePerPixel()  const {return _bytePerPixel;} 


image::data_type & image::getData()    {return data;}
image::data_cptr   image::getDataPtr() const {return &data[0];}


void image::initDefaults()
{
    logger::info("[Image  ] create defaults...");
    ;   
    #define F 255 
    uint8_t _black[]  = {0, 0, 0, F,  0, 0, 0, F,  0, 0, 0, F,  0, 0, 0, F};
    uint8_t _red[]    = {F, 0, 0, F,  F, 0, 0, F,  F, 0, 0, F,  F, 0, 0, F};
    uint8_t _green[]  = {0, F, 0, F,  0, F, 0, F,  0, F, 0, F,  0, F, 0, F};
    uint8_t _blue[]   = {0, 0, F, F,  0, 0, F, F,  0, 0, F, F,  0, 0, F, F};
    uint8_t _base[]   = {0, 0, F, F,  F, F, 0, F,  F, 0, 0, F,  0, F, 0, F};
    uint8_t _trans[]  = {0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0};
    uint8_t _white[]  = {F, F, F, F,  F, F, F, F,  F, F, F, F,  F, F, F, F};
    
    addToCache(std::make_shared<image>(_black, 2,2,32,"def::black"));
    addToCache(std::make_shared<image>(_red,   2,2,32,"def::red"));
    addToCache(std::make_shared<image>(_green, 2,2,32,"def::green"));
    addToCache(std::make_shared<image>(_blue , 2,2,32,"def::blue"));
    addToCache(std::make_shared<image>(_trans, 2,2,32,"def::transparent"));
    addToCache(std::make_shared<image>(_base,  2,2,32,"def::base"));
    addToCache(std::make_shared<image>(_white, 2,2,32,"def::white"));
     
    #undef F 
}

void image::savePNG(std::string filename)
{
    if(stbi_write_png(filename.c_str(), _width, _height, _bytePerPixel, &data[0], _bytePerPixel * _width) == 0)
        FATAL_ERROR("[Image  ] can't save png file %s",filename.c_str());  
}

void image::saveTGA(std::string filename)
{
   if(stbi_write_tga(filename.c_str(), _width, _height, _bytePerPixel, &data[0]) == 0)
        FATAL_ERROR("[Image  ] can't save tga file %s",filename.c_str()); 
}

void image::saveBMP(std::string filename)
{
   if(stbi_write_bmp(filename.c_str(), _width, _height, _bytePerPixel, &data[0]) == 0)
        FATAL_ERROR("[Image  ] can't save bmp file %s",filename.c_str()); 
}
void image::load(strref filename)
{
    if(!the::filesystem::is_file(filename))
        FATAL_ERROR("[Image  ] can't load file %s",filename.c_str());
    std::vector<char> content;
    int w,h, bpp;
    the::filesystem::load_content(filename, content);
    auto d = stbi_load_from_memory(reinterpret_cast<unsigned char *>(&content[0]),content.size(), &w, &h, &bpp,0);
    _width = w;
    _height = h;
    _bytePerPixel = bpp;
    _bitPerPixel  = bpp * 8;
    auto line_size = _width * bpp;
    for(h=_height-1; h >= 0; h-- )
    {
        auto beg = h * line_size ;
        auto end = beg + line_size;
       data.insert(data.end(),d+beg, d+end);
    }    
    stbi_image_free(d);
}

