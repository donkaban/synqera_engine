#ifndef _THE_IMAGE_
#define _THE_IMAGE_

#include "TheEngine/common.h"

namespace the {

class image : 
	public abstract::object,
	public ICacheable<image>
{
	THE_SHARED(image)
public:
	using data_type = std::vector<uint8_t>;
	using data_cptr = const uint8_t *;
 	
 	virtual ~image();    

	image();
	image(strref filename);
	image(uint width, uint height, uint bpp); 
	image(data_cptr, uint width, uint height, uint bpp,strref tag = "--");
     
	uint getWidth()  		const; 
	uint getHeight() 		const; 
	uint getBpp()    		const; 
	uint getBytePerPixel() 	const; 

	ivec4 getPixel(int, int);
	void  setPixel(int, int, const ivec4 &);

	void  forEachPixel(const std::function<ivec4 (ivec4)> &);

	data_type & getData();    
	data_cptr   getDataPtr() const;
	void        setData(data_cptr, int width, int height, int bpp);
	void        setData(data_type &, int width, int height, int bpp);
	void        setDataFromScreen(int width, int height);

	void saveTGA(std::string);
	void savePNG(std::string);
	void saveBMP(std::string);
	
    static cref cachedLoad(strref filename);
    static void initDefaults();

private:

	data_type data;

	int _width;
	int _height;
	int _bitPerPixel;
	int _bytePerPixel;

	void load(strref);
};

}
#endif
