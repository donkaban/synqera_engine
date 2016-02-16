#ifndef _THE_TEXT_H_
#define _THE_TEXT_H_

#include "TheEngine/objects/object.h"
#include "TheEngine/resource/font.h"
#include "TheEngine/objects/mesh.h"


enum ALIGNMENT
{
	TOP_LEFT = 0,
	TOP_CENTER = 1,
	TOP_RIGHT = 2,

	MIDDLE_LEFT = 3,
	MIDDLE_CENTER = 4,
	MIDDLE_RIGHT = 5,

	BOTTOM_LEFT = 6,
	BOTTOM_CENTER = 7,
	BOTTOM_RIGHT = 8,
};

enum WRAPPING
{
	NONE = 0,
	CHARACTER = 1,
	WORD = 2,
	CUTTING =3,
};

enum SCROLL_STATE
{
	SCROLLING = 0,
	STRETCHING = 1,
	FINISHED = 2,
};

namespace the {

class text : 
	public the::object, 
	public serialize::xml
{
	THE_SHARED(text)
public:
	text();
	text(strref);
	text(the::font::cref f,the::material::cref m, const std::string &t = "", float _size = 1);
	
	virtual void bind()   override;
	virtual void unbind() override;
	virtual void log();

	virtual ~text();
	
 	virtual void update() {the::node::update();}
	virtual void onRender(render::state &);
	void serialize(pugi::xml_node &) ;
	void deserialize(const pugi::xml_node &);
	inline the::font::cref getFont() const {return font;}
	inline the::material::cref getMaterial() const {	return material;}

	

	//USER API METHODS
	void setScreenSize(float x, float y);
	void setText(const std::string& text);

	void setWrapping(WRAPPING wrap);
	void setAlignment(ALIGNMENT alignment);
	void setSuffix(const std::string& s) {this->suffix = s;}
	void setFont(the::font::cref);
	void setSize(float);
	void setColor(const vec4 &);
	void setKerning(float);
	void setText(const std::string &, const std::string &);
	float getSize() const;
	void setMaterial(the::material::cref );
	SCROLL_STATE scrollText(float delta);
	void setOffsetX(float);
	const std::string & getText() const;
	void setBackgroundEnabled(bool mode) {isBackgroundEnabled = mode; if(background)background->setEnabled(isBackgroundEnabled);}

	const vec4 & getColor() const;
	float getWorldSizeX(const std::string& s);
	float getWorldSizeY(const std::string& s);
	float getWorldSizeY() {return lastWorldSizeY;}
	void setLineSpacing(float sp);
	void setStretchSpeed(float speed);
	void setMaxStretch(float val);
	void setBlendOffset(float val);

private:
	std::string text_ = "";
	std::string suffix = "";

	the::font::ptr 	   font;
	the::material::ptr material;
	the::mesh::ptr     background;
	// Surface surface;

	WRAPPING wrapping = WRAPPING::NONE;
	ALIGNMENT alignment = ALIGNMENT::TOP_LEFT;

	int linesCount = 0;

	float kerning=0.0f;
	float size = 1.0f;
	float scale = 0.001;	
	float screenSizeX = 1.0;
	float screenSizeY = 1.0;
	float lastWorldSizeY = 0.0f;
	float lineSpacing = 0.0;
	vec4 textScrollOffset;
	vec4 startStrechingOffset;
	float lineSpacingOffset = 0.0f;
	float stretchSpeed  = 1.0f;
	float maxStretch  = 0.15f;
	const float scaleCoeff = 0.001;

	bool isBackgroundEnabled = false;
	
	vec4 textColor = vec4(1,1,1,1);
	vec2 getCharSize(uint32_t c);

	bool frustumCulling(BoundingBox b, const mat4& worldView);
	bool intersecting(const vec3& p0, const vec3& p1, const vec3& t0, const vec3& t1, const vec3& t2);

	std::vector<std::vector<uint32_t>> tokenize(const std::string& str);
	std::vector<uint32_t> strToUtf8Char(const std::string& s);
	const std::string alignToString(ALIGNMENT a);
	const std::string wrapToString(WRAPPING);

	void appendLines(std::vector<std::vector<uint32_t>>);
	void renderLine(std::vector<uint32_t> line,float yPosition);

	float getFragmentSize(std::vector<uint32_t> s);
	float getFragmentSize(const std::string s);
	float getLineHeight() {return font->getHeight()*scale + lineSpacing;}
	float side(const vec3& p, const vec3& q, const vec3& a, const vec3& b);

	ALIGNMENT stringToAlign(strref);
	WRAPPING stringToWrap(strref);

	std::unordered_map<std::string, ALIGNMENT> strToAlign = {
		{"top_right", ALIGNMENT::TOP_RIGHT},
		{"top_center", ALIGNMENT::TOP_CENTER},
		{"top_left", ALIGNMENT::TOP_LEFT},
		{"middle_right", ALIGNMENT::MIDDLE_RIGHT},
		{"middle_center", ALIGNMENT::MIDDLE_CENTER},
		{"middle_left", ALIGNMENT::MIDDLE_LEFT},
		{"bottom_right", ALIGNMENT::BOTTOM_RIGHT},
		{"bottom_center", ALIGNMENT::BOTTOM_CENTER},
		{"bottom_left", ALIGNMENT::BOTTOM_LEFT}
	};

	std::unordered_map<std::string, WRAPPING> strToWrap = {
		{"none", WRAPPING::NONE},
		{"character", WRAPPING::CHARACTER},
		{"word", WRAPPING::WORD},
		{"cutting", WRAPPING::CUTTING}
	};

	std::map<ALIGNMENT,std::string> alignToStr = {
		{ALIGNMENT::TOP_RIGHT, "top_right"},
		{ALIGNMENT::TOP_CENTER, "top_center"},
		{ALIGNMENT::TOP_LEFT, "top_left"},
		{ALIGNMENT::MIDDLE_RIGHT, "middle_right"},
		{ALIGNMENT::MIDDLE_CENTER, "middle_center"},
		{ALIGNMENT::MIDDLE_LEFT, "middle_left"},
		{ALIGNMENT::BOTTOM_RIGHT, "bottom_right"},
		{ALIGNMENT::BOTTOM_CENTER, "bottom_center"},
		{ALIGNMENT::BOTTOM_LEFT, "bottom_left"}
	};

	std::map<WRAPPING, std::string> wrapToStr{
		{WRAPPING::NONE, "none"},
		{WRAPPING::CHARACTER, "character"},
		{WRAPPING::WORD, "word"},
		{WRAPPING::CUTTING, "cutting"}
	};

	void redraw();

};
}

#endif
