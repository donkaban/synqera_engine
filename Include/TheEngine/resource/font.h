#ifndef _THE_FONT_H_
#define _THE_FONT_H_

#include "TheEngine/comtypes.h"
#include "TheEngine/logger.h"
#include "TheEngine/resource/texture.h"
#include "TheEngine/math/all.h"

#include "TheExternals/utf8/iterator.h"


extern "C" 
{
	#include <ft2build.h>
	#include <freetype/freetype.h>
	#include <freetype/ftglyph.h>
	#include <freetype/ftoutln.h>
	#include <freetype/fttrigon.h>
}

#define DEFAULT_FONT_SIZE 	96
#define DEFAULT_FONT_DPI 	96
#define DEFAULT_FONT_ITALIC 0
#define DEFAULT_FONT_BOLD 	0

namespace the{

struct FontCharset
{
	static const std::string def;
	static const std::string russian;
	static const std::string english;
	static const std::string numbers;
};

class font : 
	public abstract::object, 
	public abstract::gapi, 
	public ICacheable<font>
{
	THE_SHARED(font)	

public:
	struct charInfo
	{
		uint32_t  code;
		vec4 rect;
		vec2 offset;
		vec4  uv;
		float len;

		charInfo(uint32_t ch, float x, float y, float w, float h, float offX, float offY, float s, float t, float u, float v,float l)
			: code(ch),rect(vec4(x,y,w,h)),offset(vec2(offX,offY)),uv(vec4(s,t,u,v)), len(l)
		{}
		charInfo(uint32_t ch, const vec4 & _rect , const vec2 & _offset, const vec4 & _uv,float l)
			: code(ch), rect(_rect), offset(_offset), uv(_uv), len(l)
		{}
		
	};

	font() {};
	font(strref,strref=FontCharset::def,int size = DEFAULT_FONT_SIZE, int bold=DEFAULT_FONT_BOLD, int italic=DEFAULT_FONT_ITALIC, int DPI=DEFAULT_FONT_DPI);
	font(const uint8_t* buffer, long int bufferSize, strref=FontCharset::def,int size = DEFAULT_FONT_SIZE, int bold=DEFAULT_FONT_BOLD, int italic=DEFAULT_FONT_ITALIC, int DPI=DEFAULT_FONT_DPI);

	
  
	virtual ~font();
  	virtual const std::string getTypeName(){return "Font";}
    
    virtual void bind()     override {fontTexture->bind();}
    virtual void unbind()   override {fontTexture->unbind();}
    
	void loadTTF(const std::string& file);
	charInfo getChar(uint32_t ch);
	bool hasChar(uint32_t ch);

	inline int 			getHeight() 	const    {return height; }
	inline int 			getWidth()		const    {return width;  }
	inline the::texture::ptr 	getTexture() 	const 	 {return fontTexture; }

	static const std::string getFontId(strref name, int size = DEFAULT_FONT_SIZE, int bold=DEFAULT_FONT_BOLD, int italic=DEFAULT_FONT_ITALIC, int DPI=DEFAULT_FONT_DPI);

	static const ptr& loadFont(const pugi::xml_node &, strref defaultPath="");
	static const ptr& loadFont(strref file);
	static const ptr& loadFont(const uint8_t*,long int size);

	static void initDefaults();
	
	void appendCharacters(UTF8String str);

private:



	std::unordered_map<uint32_t, font::charInfo> chars;

	std::string file;
	std::string style;
	std::string family;
	
	the::texture::ptr fontTexture;
	
	static ptr defFont;

	int height;
	int width;
	
	friend  std::ostream &operator<<(std::ostream &o, const font &a) {o << a.tostring();return o;}

	void appendCharacter(uint32_t code);

	FT_Library  library;   /* handle to library     */
    FT_Face     face;      /* handle to face object */
    //Font specific
    int size = DEFAULT_FONT_SIZE;
    int DPI = DEFAULT_FONT_DPI;
    int italic = DEFAULT_FONT_ITALIC;
    int bold = DEFAULT_FONT_BOLD;

    bool antialiased = true;
    int pixelSeparator = 5;
    int ascender = 0;
    int descender = 0;
    int textureSize = 2048;
    int currentColumn = 0;
    int currentRow = 0;
    std::vector<char> fontRawData;

    static std::vector<std::pair<std::string,std::string>> charsetToString;
    const std::string currentCharset = FontCharset::english;

    void init();
};
}

#endif
