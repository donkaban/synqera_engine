#include <TheEngine/resource/font.h>
#include "TheEngine/pre.h"
#include "TheEngine/logger.h"

#include "TheExternals/utf8/checked.h"
#include "TheExternals/utf8/unchecked.h"




#define Font_FT_ERR_CHECK(err,func) if (err) {logger::error("[Font   ] error in %s, code: %d",#func,err);}

const std::string the::FontCharset::def     = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ЁёйцукенгшщзхъфывапролджэячсмитьбюЙЦУКЕНГШЩЗХЪФЫВАПРОЛДЖЭЯЧСМИТЬБЮ";
const std::string the::FontCharset::russian = " !\"#$%&'()*+,-./0123456789:;<=>?@[\\]^_{|}~ЁёйцукенгшщзхъфывапролджэячсмитьбюЙЦУКЕНГШЩЗХЪФЫВАПРОЛДЖЭЯЧСМИТЬБЮ";
const std::string the::FontCharset::english = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
const std::string the::FontCharset::numbers = " !\"#$%&'()*+,-./0123456789:;<=>?@[\\]^_{|}~";

std::vector<std::pair<std::string,std::string>> the::font::charsetToString=
{
    {"russian",the::FontCharset::russian},
    {"english",the::FontCharset::english},
    {"numbers",the::FontCharset::numbers},
    {"default",the::FontCharset::def}
};

the::font::font(const std::string &filename, const std::string &charset,int size, int bold, int italic, int DPI) :
  abstract::object("Font",filename),
  size(size),
  DPI(DPI),
  italic(italic),
  bold(bold),
  currentCharset(charset)
{
    if(!the::filesystem::is_file(filename))
        FATAL_ERROR("file %s not exist",filename.c_str());
    loadTTF(filename);
}

the::font::font(const uint8_t * buffer, long int bufferSize, const std::string &charset,int size, int bold, int italic, int DPI) :
  abstract::object("Font","default"),
  size(size),
  DPI(DPI),
  italic(italic),
  bold(bold),
  currentCharset(charset)
{
    fontRawData.assign(&buffer[0], &buffer[0]+bufferSize);
    init();
}

the::font::~font()
{
    FT_Done_Face(face);
    FT_Done_FreeType(library);
}

the::font::charInfo the::font::getChar(uint32_t ch)
{
    auto it =  chars.find(ch);
    if (it == chars.end())
    {
        appendCharacter(ch);
        auto it2 = chars.find(ch);
        if (it2 == chars.end())
            FATAL_ERROR("[Font   ] bad key for find charInfo: \"%s\" with code: %d", Utf8StringIterator::charToStr(ch).c_str(),ch);
        fontTexture->bind();
        return it2->second;
    }
    return it->second;
}

bool the::font::hasChar(uint32_t ch)
{
    auto it =  chars.find(ch);
    return it != chars.end();
}

void the::font::appendCharacters(UTF8String str)
{
    UTF8String charsToAppend;
    for (uint32_t i = 0; i < str.size(); ++i)
    {
        if (chars.find(str[i]) == chars.end())
            charsToAppend.push_back(str[i]);
    }
    if (charsToAppend.size()>0)
    {
        for (uint32_t i = 0; i < charsToAppend.size(); ++i)
            appendCharacter(charsToAppend[i]);
        
        fontTexture->bind();
    }
}

void the::font::appendCharacter(uint32_t code)
{
    FT_Glyph glyph;
    uint32_t index = FT_Get_Char_Index(face, code);
    if (index == 0) 
    {
        logger::error("[Font   ] unsupported font character: %s",Utf8StringIterator::charToStr(code).c_str());
        return;
    }
    FT_Int32 flags = FT_LOAD_DEFAULT;
    int error = FT_Load_Glyph(face, index, flags);
    Font_FT_ERR_CHECK(error,FT_Load_Glyph);

    if (bold != 0)
    {
        FT_Pos strength = size * bold;
        if (face->glyph->format == FT_GLYPH_FORMAT_OUTLINE)
            FT_Outline_Embolden(&face->glyph->outline, strength);           
    }
    if (face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
    {
        error = FT_Render_Glyph(face->glyph, antialiased ? FT_RENDER_MODE_NORMAL : FT_RENDER_MODE_MONO);
        Font_FT_ERR_CHECK(error,FT_Render_Glyph);
    }
    if (error)
        return;
    
    FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

    FT_Get_Glyph(face->glyph, &glyph);

    FT_GlyphSlot slot = face->glyph;        
    FT_Bitmap& bitmap = slot->bitmap;
    
    int      offsetX = slot->bitmap_left;
    int      offsetY = ascender - slot->bitmap_top;
    width = (slot->advance.x >> 6);

    if (width < bitmap.width)
        width = bitmap.width;

    if (currentColumn+bitmap.width > textureSize)
    {
        currentColumn = 0;
        currentRow+=height+pixelSeparator;
    }

    int characterY = textureSize - (offsetY+currentRow-descender);
    int characterX = currentColumn;
    
    charInfo character(code,
                        0,
                        0,
                        static_cast<uint32_t>(bitmap.width),
                        static_cast<uint32_t>(bitmap.rows),
                        offsetX,
                        offsetY,
                        static_cast<float>(characterX)/static_cast<float>(textureSize), // uv
                        static_cast<float>(characterY-bitmap.rows)/static_cast<float>(textureSize), // uv
                        static_cast<float>(characterX+bitmap.width)/static_cast<float>(textureSize), // uv
                        static_cast<float>(characterY)/static_cast<float>(textureSize), // uv
                        width);

    chars.insert(std::pair<uint32_t, charInfo>(character.code, character));

    std::vector<uint8_t > &textureData = fontTexture->getImage()->getData();

    if (characterY - bitmap.rows >= 0 && characterX + bitmap.width <=textureSize)
    {
        for(int y =0;y<bitmap.rows;y++)
        {
            for(int x =0;x<bitmap.width;x++)
            {
                
                unsigned char c = bitmap.buffer[x+bitmap.width*y];

                int textureY = characterY - y;
                int textureX = characterX+x;

                int startPoint = (textureX + textureSize*textureY)*4;

                textureData[startPoint] = 255;
                textureData[startPoint+1] = 255;
                textureData[startPoint+2] = 255;
                textureData[startPoint+3] = c;

            }
        }
    }
    else
    {
        FATAL_ERROR("[Font   ] not enough space for character \"%s\" in the texture",Utf8StringIterator::charToStr(code).c_str());
    }

    currentColumn+=bitmap.width+pixelSeparator;
}

const std::string the::font::getFontId(strref name, int size, int bold, int italic, int DPI)
{
    return aux::format("%s_%d_%d_%d_%d",name.c_str(),size,bold,italic,DPI);
}

the::font::cref the::font::loadFont(const pugi::xml_node &node, strref defaultPath)
{
    std::string filename = defaultPath + node.attribute("file").value();

    pugi::xml_attribute sizeAttrib = node.attribute("size");
    pugi::xml_attribute italicAttrib = node.attribute("italic");
    pugi::xml_attribute boldAttrib = node.attribute("bold");
    pugi::xml_attribute dpiAttrib = node.attribute("dpi");

    int font_size = sizeAttrib.empty()?DEFAULT_FONT_SIZE:sizeAttrib.as_int();
    int font_italic = italicAttrib.empty()?DEFAULT_FONT_ITALIC:italicAttrib.as_int();
    int font_bold = boldAttrib.empty()?DEFAULT_FONT_BOLD:boldAttrib.as_int();
    int font_DPI = dpiAttrib.empty()?DEFAULT_FONT_DPI:dpiAttrib.as_int();
    
    std::string fontId = the::font::getFontId(filename,font_size,font_bold,font_italic,font_DPI);

    std::string charset = node.attribute("charset").value();
    auto it = std::find_if(
        charsetToString.begin(),
        charsetToString.end(),
        [charset] (const std::pair<std::string,std::string>& s) {return s.first==charset;} 
    );

    charset = it!=charsetToString.end()?(*it).second:the::FontCharset::def;
    if(!the::font::check(fontId))
    {
        the::font::addToCache(std::shared_ptr<the::font>(std::shared_ptr<the::font>
            (new the::font(filename,charset,font_size,font_bold,font_italic,font_DPI))),fontId);
    }

    return the::font::get(fontId);
}

the::font::cref the::font::loadFont(strref filename)
{
    std::string fontId = the::font::getFontId(filename);

    if(the::font::check(filename))
    {
        return the::font::get(filename);
    }

    if(!the::font::check(fontId))
    {
        the::font::addToCache(std::shared_ptr<the::font>(new the::font(filename)),fontId);
    }

    return the::font::get(fontId);
}

void the::font::init()
{
    fontTexture = std::make_shared<the::texture>(textureSize,textureSize,32);
    fontTexture->setFiltering(2);
    int error = FT_Init_FreeType(&library);
    Font_FT_ERR_CHECK(error,FT_New_Memory_Face)

    error = FT_New_Memory_Face( library,
                                reinterpret_cast<const FT_Byte* >(&fontRawData[0]),    /* first byte in memory */
                                fontRawData.size(),      /* size in bytes        */
                                0,         /* face_index           */
                                &face );
    Font_FT_ERR_CHECK(error,FT_New_Memory_Face);
    

    FT_Set_Char_Size(face, size << 6, size << 6, DPI, DPI);
    Font_FT_ERR_CHECK(error,FT_Set_Char_Size);

    if (italic != 0)
    {
        FT_Matrix matrix;
        const float angle = (-M_PI * italic) / 180.0f;
        matrix.xx = static_cast<FT_Fixed>( cos(angle) * 0x10000L );
        matrix.xy = static_cast<FT_Fixed>(-sin(angle) * 0x10000L );
        matrix.yx = static_cast<FT_Fixed>( 0/*sin( angle )*/ * 0x10000L );
        matrix.yy = static_cast<FT_Fixed>( 1/*cos( angle )*/ * 0x10000L );
        FT_Set_Transform(face, &matrix, 0);
    } 
    else
    {
        FT_Set_Transform(face, 0, 0);
    }

    if (FT_IS_SCALABLE(face))
    {
        ascender  = static_cast<int>(face->size->metrics.ascender)   >> 6;
        descender = static_cast<int>(face->size->metrics.descender)  >> 6;
        height    = static_cast<uint32_t>(face->size->metrics.height)>> 6;
    }
    else
    {
        ascender  = static_cast<int>(face->ascender);
        descender = static_cast<int>(face->descender);
        height    = static_cast<uint32_t>(face->height);
    }

    UTF8String utf8Str = Utf8StringIterator::createUTF8(currentCharset);

    for (uint32_t i = 0; i < utf8Str.size(); ++i)
    {
        appendCharacter(utf8Str[i]);
    }

    fontTexture->bind();
}


void the::font::loadTTF(const std::string &file)
{
    setTag(file);

    the::filesystem::stream stream(file);
    fontRawData.resize(stream.size());
    stream.read(&fontRawData[0],fontRawData.size());

    init();
}

void the::font::initDefaults()
{
    logger::info("[Font   ] create defaults...");  
    the::font::addToCache(font::make(&precompiled::font::fnt.data[0],precompiled::font::fnt.size,FontCharset::english),"def::base");
     
}