#include "TheEngine/objects/text.h"
#include "TheEngine/logger.h"
#include "TheEngine/math/all.h"

the::text::text(the::font::cref f,the::material::cref m, const std::string &t, float _size) :
	the::object("Text","--"),
	font(f),
	// surface("tf_surface",m),
	size(_size)
{
	addSurface(std::move(the::surface("tf_surface",m)));
	auto &surface = getSurface(0);
	surface.setBlending(true);
	scale = scaleCoeff * size;
	setBlend(true);
	if (font != nullptr)
	{
		surface.setTexture(0, font->getTexture());
		setText(t);
	}
}

the::text::text() : 
	the::text(the::font::get("def::base"),the::material::get("def::text"),"", 1)
{}  

the::text::text(strref fontname) :
	the::text(the::font::loadFont(fontname),the::material::get("def::text"),"", 1)
{

}

the::text::~text()  
{};

void the::text::redraw()
{
	auto lines = tokenize(text_);
	linesCount = lines.size();
	lastWorldSizeY = getLineHeight()*linesCount;
	appendLines(lines);
}

void the::text::setText(strref t) 
{
	text_ = t;
	//font->appendCharacters(Utf8StringIterator::createUTF8(t));
	redraw();
}
void the::text::setText(strref str, strref suff) { setSuffix(suff); setText(str);}

void the::text::setWrapping(WRAPPING wrap)  
{
	wrapping = wrap;
	redraw();
}

void the::text::setAlignment(ALIGNMENT alignment)
{
	this->alignment = alignment;
	redraw();
}

vec2 the::text::getCharSize(uint32_t c)
{
	switch(c)
	{
		case '\n':
			return vec2(0,0);
		default:
			const the::font::charInfo &character = font->getChar(c);
			return vec2(character.len,character.rect.w)*scale;
	}
}

std::vector<uint32_t> the::text::strToUtf8Char(const std::string& s)
{
	std::vector<uint32_t> result;
	Utf8StringIterator it(s);
	while(it.next())
	{
		uint32_t c = it.current();
		if (c == 0) 
			continue;

		result.push_back(c);
	}

	return result;
}

void the::text::setScreenSize(float x, float y) 
{
	screenSizeX = x; 
	screenSizeY = y; 
	if(background)
		removeChild(background);

	background = the::mesh::createPlane(vec2(screenSizeX,screenSizeY));
	background->translate(vec3(screenSizeX/2,-screenSizeY/2,-0.01));
	background->setBlending(0, true);
	background->setTag(getTag() + "#background");
	setBackgroundEnabled(isBackgroundEnabled);
	addChild(background);
	redraw();
}


void the::text::onRender(render::state &state)
{
   // if(frustumCulling(getBoundingBox(),state.prjViewMatrix)) 
   // {    
   //     logger::extra("culling!");
   //     return;
   // }    
	auto &surface = getSurface(0);
	the::object::onRender(state);
	surface.setDefaultParameters(state);
	surface.setUserParameters(state);
	surface.draw();
}

float the::text::getFragmentSize(std::vector<uint32_t> s)
{
	float sz =0;
	for(size_t i =0;i<s.size();i++)
	{
		float charSize = getCharSize(s[i]).x;
		sz+=charSize+kerning;
	}

	return sz;
}

float the::text::getFragmentSize(const std::string str)
{
	float sz =0;
	Utf8StringIterator it(str);
	while(it.next())
	{
		uint32_t c = it.current();
		if (c == 0) 
			continue;

		float charSize = getCharSize(c).x;
		sz+=charSize+kerning;
	}

	return sz;
}

std::vector<std::vector<uint32_t>> the::text::tokenize(const std::string& str)
{
	Utf8StringIterator it(str);
	float currentSize = 0;
	std::vector<uint32_t> stablePart;
	std::vector<uint32_t> unstablePart;

	float maxSizeX = screenSizeX;

	if(wrapping==CUTTING)
		maxSizeX-=getFragmentSize(suffix);

	std::vector<std::vector<uint32_t>> lines;
	while(it.next())
	{
		uint32_t c = it.current();
		if (c == 0) 
			continue;

		float charSize = getCharSize(c).x;
		currentSize+=charSize+kerning;

		if(c=='\n')
		{
			stablePart.insert(stablePart.end(), unstablePart.begin(), unstablePart.end());
			unstablePart.clear();
			lines.push_back(stablePart);
			currentSize = getFragmentSize(unstablePart);
			stablePart.clear();
			continue;
		}

		unstablePart.push_back(c);

		if(c==' ' && wrapping==WRAPPING::WORD) // end of token
		{
			stablePart.insert(stablePart.end(), unstablePart.begin(), unstablePart.end());
			unstablePart.clear();
		}

		if(currentSize > maxSizeX && wrapping==CUTTING)
		{
			unstablePart.pop_back();
			stablePart.insert(stablePart.end(), unstablePart.begin(), unstablePart.end());
			std::vector<uint32_t> suffixPart = strToUtf8Char(suffix);
			stablePart.insert(stablePart.end(), suffixPart.begin(), suffixPart.end());
			lines.push_back(stablePart);
			return lines;
		}

		if(currentSize > maxSizeX && stablePart.size()>0)
		{
			lines.push_back(stablePart);
			currentSize = getFragmentSize(unstablePart);
			stablePart.clear();
		}


		if(wrapping == WRAPPING::CHARACTER) // end of token
		{
			stablePart.insert(stablePart.end(), unstablePart.begin(), unstablePart.end());
			unstablePart.clear();
		}
	}

	stablePart.insert(stablePart.end(), unstablePart.begin(), unstablePart.end());
	lines.push_back(stablePart);
	return lines;
}

void the::text::renderLine(std::vector<uint32_t> line, float yPosition)
{
	float xPosition = 0;
	if(line.size()>0 && line[line.size()-1]==' ')
		line.pop_back();
	if(alignment == ALIGNMENT::TOP_CENTER || alignment == ALIGNMENT::MIDDLE_CENTER || alignment == ALIGNMENT::BOTTOM_CENTER)
		xPosition = (screenSizeX - getFragmentSize(line))/2;
	if(alignment == ALIGNMENT::TOP_RIGHT || alignment == ALIGNMENT::MIDDLE_RIGHT || alignment == ALIGNMENT::BOTTOM_RIGHT)
		xPosition = screenSizeX - getFragmentSize(line);

	for(size_t i =0;i<line.size();i++)
	{
		uint32_t c = line[i];			
		vec2 charSize = getCharSize(c);

		if(font->hasChar(c))
		{
			the::font::charInfo character = font->getChar(c);

			float height = (font->getHeight() - character.offset.y)*scale; 
			float bottom = (height - charSize.y);

			float xOffset = character.offset.x*scale;

			vec3 topleft = vec3(xPosition+xOffset,yPosition+height,0);
			vec3 rightbottom = vec3(xPosition+xOffset+character.rect.z*scale,yPosition+bottom,0);

			float xMax = std::max(topleft.x,rightbottom.x);
			float xMin = std::min(topleft.x,rightbottom.x);

			addVertex
			({
				Vertex({ xMin, topleft.y, 0},			{character.uv.x, character.uv.w},{0,0,1}),
				Vertex({ xMin, rightbottom.y, 0},		{character.uv.x, character.uv.y},{0,0,1}),
				Vertex({ xMax, rightbottom.y, 0},	{character.uv.z, character.uv.y},{0,0,1}),
				Vertex({ xMax, topleft.y, 0},		{character.uv.z, character.uv.w},{0,0,1})
			});
		}
		
		xPosition+=charSize.x+kerning;
	}
}

void the::text::appendLines(std::vector<std::vector<uint32_t>> lines)
{
	float yPosition = -getLineHeight();
	if(alignment == ALIGNMENT::MIDDLE_CENTER || alignment == ALIGNMENT::MIDDLE_LEFT || alignment == ALIGNMENT::MIDDLE_RIGHT)
		yPosition -= (screenSizeY - (getLineHeight())*lines.size())/2;
	if(alignment == ALIGNMENT::BOTTOM_CENTER || alignment == ALIGNMENT::BOTTOM_LEFT || alignment == ALIGNMENT::BOTTOM_RIGHT)
		yPosition -= screenSizeY - (getLineHeight())*lines.size();

	unbind();
		vertexes.clear();
		for(auto & l : lines)
		{
			renderLine(l, yPosition);
			yPosition -= getLineHeight() + lineSpacingOffset;
		}
	bind();
   	auto &surface = getSurface(0);
	surface.clear();
		for(uint16_t i = 0 ; i <  vertexes.size(); i += 4)
			surface.add({i,static_cast<uint16_t>(i + 1),static_cast<uint16_t>(i + 2), 
						   static_cast<uint16_t>(i + 2),static_cast<uint16_t>(i + 3), i});
		surface.setUserParam("color",textColor);
		surface.setUserParam("size",vec4(screenSizeX, -screenSizeY,0,0));
		surface.setUserParam("offset",textScrollOffset);
	surface.bind();
}

void the::text::setFont(the::font::cref f) 	
{
	auto &surface = getSurface(0);
	font = f;
	surface.setTexture(0, font->getTexture());
	redraw();
}

void the::text::setSize(float s) 
{
	size = s; 
	scale = scaleCoeff * size; 
	redraw();
}
	
void the::text::setColor(const vec4 &_color) 
{ 
	auto &surface = getSurface(0);
	textColor = _color;
	surface.setUserParam("color",textColor);
}

void the::text::setKerning(float k) 	{kerning = k; redraw();}
void the::text::setLineSpacing(float sp) 	{lineSpacing = sp; redraw();}

float the::text::getWorldSizeX(const std::string& s)
{
	std::vector<std::vector<uint32_t>> lines = tokenize(s);
	float result = -1;
	for(size_t i =0;i<lines.size();i++)
	{
		float lineSize = getFragmentSize(lines[i]);
		result = std::max(result,lineSize);
	}
	return result;
}

float the::text::getWorldSizeY(const std::string& s)
{
	std::vector<std::vector<uint32_t>> lines = tokenize(s);
	return (getLineHeight())*lines.size();
}


float the::text::side(const vec3 &p, const vec3 &q, const vec3 &a, const vec3 &b)
{
	float z1 = (b.x - a.x) * (p.y - a.y) - (p.x - a.x) * (b.y - a.y);
	float z2 = (b.x - a.x) * (q.y - a.y) - (q.x - a.x) * (b.y - a.y);
	return z1 * z2;
}

bool the::text::intersecting(const vec3 &p0, const vec3 &p1, const vec3 &t0, const vec3 &t1, const vec3 &t2)
{
	float f1 = side(p0, t2, t0, t1), f2 = side(p1, t2, t0, t1);
	float f3 = side(p0, t0, t1, t2), f4 = side(p1, t0, t1, t2);
	float f5 = side(p0, t1, t2, t0), f6 = side(p1, t1, t2, t0);
	float f7 = side(t0, t1, p0, p1);
	float f8 = side(t1, t2, p0, p1);

	return !((f1 < 0 && f2 < 0) || (f3 < 0 && f4 < 0) || (f5 < 0 && f6 < 0) || (f7 > 0 && f8 > 0));
}

bool the::text::frustumCulling(BoundingBox b, const mat4 & worldView)
{
	mat4 world = worldView*b.transform;
	vec3 v1 = vec3(b.max.x,b.min.y,b.min.z)*world;
	vec3 v2 = vec3(b.min.x,b.min.y,b.min.z)*world;
	vec3 v3 = vec3(b.max.x,b.max.y,b.min.z)*world;
	vec3 v4 = vec3(b.min.x,b.max.y,b.min.z)*world;

	vec3 t1 = vec3(-1,-1,0);
	vec3 t2 = vec3(-1,1,0);
	vec3 t3 = vec3(1,1,0);
	vec3 t4 = vec3(1,-1,0);

	return  !(intersecting(v1,v2,t1,t2,t3) ||
			  intersecting(v2,v3,t1,t2,t3) ||
			  intersecting(v3,v4,t1,t2,t3) ||
			  intersecting(v1,v4,t1,t2,t3) ||
			  intersecting(v1,v3,t1,t2,t3) ||

			  intersecting(v1,v2,t1,t3,t4) ||
			  intersecting(v2,v3,t1,t3,t4) ||
			  intersecting(v3,v4,t1,t3,t4) ||
			  intersecting(v1,v4,t1,t3,t4) ||
			  intersecting(v1,v3,t1,t3,t4));
}

void the::text::setMaterial(the::material::cref m) 
{
	material = m;
	for(auto &s:surfaces)
	{
		s.setMaterial(material);
		s.setBlending(true);
	}
}

ALIGNMENT the::text::stringToAlign(const std::string& s)
{
	if (strToAlign.find(s) != strToAlign.end())
		return strToAlign[s];
	else
		return ALIGNMENT::TOP_LEFT;
}

const std::string the::text::alignToString(ALIGNMENT a)
{
	if (alignToStr.find(a) != alignToStr.end())
		return alignToStr[a];
	else
		return "top_left";
}

WRAPPING the::text::stringToWrap(const std::string& s)
{
	if (strToWrap.find(s) != strToWrap.end())
		return strToWrap[s];
	else
		return WRAPPING::NONE;
}

const std::string the::text::wrapToString(WRAPPING w)
{
	if (wrapToStr.find(w) != wrapToStr.end())
		return wrapToStr[w];
	else
		return "none";
}

void the::text::deserialize(const pugi::xml_node & node)	
{
	setMaterial(the::material::get("def::text"));
	the::object::deserialize(node);

	setFont(the::font::loadFont(node.child("font"),getDefaultPath()));
	
	setTag(node.attribute("name").value());
	
	setSize(node.attribute("size").as_float());	
	setKerning(node.attribute("kerning").as_float());
	setScreenSize(node.attribute("sizeX").as_float(), node.attribute("sizeY").as_float());

	setAlignment(stringToAlign(node.attribute("alignment").value()));
	setWrapping(stringToWrap(node.attribute("wrapping").value()));
	setLineSpacing(node.attribute("line_spacing").as_float());

	setText(node.child("text").child_value());
	vec4 c;
	c.deserializeColor(node.child("color"));
	setColor(c);
}

void the::text::serialize(pugi::xml_node & node)	
{
	node.set_name("node");
	node.append_attribute("name") = getTag().c_str();
	node.append_attribute("font") = font->getTag().c_str();
	node.append_attribute("objectID") = getUID();
	node.append_attribute("type") = "TextField";
	node.append_attribute("size") = size;
	node.append_attribute("kerning") = kerning;
	node.append_attribute("alignment") = alignToString(alignment).c_str();
	node.append_attribute("wrapping") = wrapToString(wrapping).c_str();
	node.append_attribute("sizeX") = screenSizeX;
	node.append_attribute("sizeY") = screenSizeY;
	node.append_attribute("line_spacing") = lineSpacing;
	
	pugi::xml_node n;
	mat4 t = getLocalTransform();
	n = node.append_child("transform");   t.serialize(n);
	
	node.append_attribute("material") = material->getTag().c_str();

	pugi::xml_node c = node.append_child("color");
	textColor.serializeColor(c);

	pugi::xml_node txtContainer = node.append_child("text");
	pugi::xml_node txt = txtContainer.append_child(pugi::node_cdata);
	txt.set_value(text_.c_str());
}

void the::text::log()
{
  	logger::system("TextField '%s' order: %d blend: %s size: %.1f; kerning: %.1f",
      getTag().c_str(), getRenderOrder(), getBlend()? "true" : "false",size, kerning);
	;  
	for(const auto & child : childs) child->log();
	  
}

SCROLL_STATE the::text::scrollText(float delta)
{
	float newScroll = textScrollOffset.y+delta;

	if (newScroll <= 0)
		return SCROLL_STATE::FINISHED;

	if (lineSpacingOffset != 0)
	{
		lineSpacingOffset = 0;
		setText(getText());
	}
	textScrollOffset.y = newScroll;
	startStrechingOffset.y = newScroll;
	auto &surface = getSurface(0);
	surface.setUserParam("offset",textScrollOffset);
	return SCROLL_STATE::SCROLLING;
}

void the::text::setOffsetX(float offsetX)
{
	textScrollOffset.x = offsetX;
	auto &surface = getSurface(0);
	surface.setUserParam("offset",textScrollOffset);
}

strref the::text::getText() const { return text_; }
float  the::text::getSize() const { return size; }
const vec4 & the::text::getColor() const { return textColor; }
void the::text::setStretchSpeed(float speed) {stretchSpeed = speed;}
void the::text::setMaxStretch(float val) {maxStretch = val;}

void the::text::bind()
{
 	uint sz = vertexes.size();
    if(sz == 0) return;
    glGenBuffers(1, &_bindId);
    glBindBuffer(GL_ARRAY_BUFFER, _bindId);
    glBufferData(GL_ARRAY_BUFFER, sz * sizeof(Vertex) , &vertexes[0], GL_STATIC_DRAW);
	auto &surface = getSurface(0);
	surface.bind();
}

void the::text::unbind()
{
	if(!glIsBuffer(_bindId)) return;
    glDeleteBuffers(1,&_bindId);
	resetID();
	auto &surface = getSurface(0);
	surface.unbind();
}

void the::text::setBlendOffset(float val)
{
	auto &surface = getSurface(0);
	surface.setUserParam("blend_size",val);
}