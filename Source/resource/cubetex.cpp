#include "TheEngine/resource/cubetex.h"
#include "TheEngine/openGL.h"


the::cubetex::~cubetex() 
{}

the::cubetex::cubetex() :
	abstract::object("Tex3D  ","def::base")
{
	for(auto &i : images) i = the::image::get("def::base");
	bind();	
}

the::cubetex::cubetex(strref i0,strref i1,strref i2,strref i3,strref i4,strref i5) :
	abstract::object("Tex3D  ","--")
{
	images[NEGATIVE_X] = the::image::cachedLoad(i0);
	images[POSITIVE_X] = the::image::cachedLoad(i1);
	images[POSITIVE_Y] = the::image::cachedLoad(i2);
	images[NEGATIVE_Y] = the::image::cachedLoad(i3);
	images[POSITIVE_Z] = the::image::cachedLoad(i4);
	images[NEGATIVE_Z] = the::image::cachedLoad(i5);
	generateTag();
	bind();
}
the::cubetex::cubetex(strref i) :
	cubetex(i,i,i,i,i,i)
{}	

the::cubetex::cubetex(the::image::cref i0,the::image::cref i1,the::image::cref i2,the::image::cref i3,the::image::cref i4,the::image::cref i5) :
	abstract::object("Tex3D  ","--")
{	
	images[0] = i0;
	images[1] = i1;
	images[2] = i2;
	images[3] = i3;
	images[4] = i4;
	images[5] = i5;
	generateTag();
	bind();

}
the::cubetex::cubetex(the::image::cref i) :
	cubetex(i,i,i,i,i,i)
{}	

the::cubetex::cubetex(const pugi::xml_node &root, strref path) : 
	abstract::object("Tex3D  ","--xml--")
{
	if(root.attribute("all") != nullptr)
	{
		
		auto img = the::image::cachedLoad(path + root.attribute("all").value());
		images[NEGATIVE_X] = img;
		images[POSITIVE_X] = img;
		images[POSITIVE_Y] = img;
		images[NEGATIVE_Y] = img;
		images[POSITIVE_Z] = img;
		images[NEGATIVE_Z] = img;
 	}
	else	
	{	

		images[NEGATIVE_X] = the::image::cachedLoad(path + root.attribute("left").value());
		images[POSITIVE_X] = the::image::cachedLoad(path + root.attribute("right").value());
		images[NEGATIVE_Y] = the::image::cachedLoad(path + root.attribute("top").value());
		images[POSITIVE_Y] = the::image::cachedLoad(path + root.attribute("bottom").value());
		images[POSITIVE_Z] = the::image::cachedLoad(path + root.attribute("back").value());
		images[NEGATIVE_Z] = the::image::cachedLoad(path + root.attribute("front").value());
	}
	generateTag();
	bind();
}
	
void the::cubetex::setImage(planes n, the::image::cref im)
{
	images[n] = im;
}

void the::cubetex::generateTag()
{
	setTag(generateTag(
		images[POSITIVE_X]->getTag(),
		images[NEGATIVE_X]->getTag(),
		images[POSITIVE_Y]->getTag(),
		images[NEGATIVE_Y]->getTag(),
		images[POSITIVE_Z]->getTag(),
		images[NEGATIVE_Z]->getTag()));
}
std::string the::cubetex::generateTag(const pugi::xml_node &root, strref path)
{
		if(root.attribute("name") != nullptr)
			return root.attribute("name").value();		
		if(root.attribute("all") != nullptr)
		{ 
			std::string t = path + root.attribute("all").value();
			return generateTag(t,t,t,t,t,t);
		}
		else
			return generateTag(
				path + root.attribute("left").value(),
				path + root.attribute("right").value(),
				path + root.attribute("top").value(),
				path + root.attribute("bottom").value(),
				path + root.attribute("front").value(),
				path + root.attribute("back").value());
}

std::string the::cubetex::generateTag(strref i0,strref i1,strref i2,strref i3,strref i4,strref i5)
{
	return("\n\t" + aux::trim(i0,aux::dataPath) + 
		   "\n\t" + aux::trim(i1,aux::dataPath) +
		   "\n\t" + aux::trim(i2,aux::dataPath) +
		   "\n\t" + aux::trim(i3,aux::dataPath) +
		   "\n\t" + aux::trim(i4,aux::dataPath) +
		   "\n\t" + aux::trim(i5,aux::dataPath));
}

void the::cubetex::initDefaults()
{
    logger::info("[cubetex] create defaults...");
    ;   
    addToCache(std::make_shared<cubetex>
    (
    	the::image::get("def::red"),
        the::image::get("def::red"),
        the::image::get("def::green"),
        the::image::get("def::green"),
        the::image::get("def::blue"),
        the::image::get("def::blue")),
    "def::base");
     
}



#define DECL(X) glTexImage2D(		\
	GL_TEXTURE_CUBE_MAP_##X, 		\
	0,								\
	GL::BPP(images[X]->getBpp()),	\
	images[X]->getWidth(), 			\
	images[X]->getHeight(), 		\
		0,							\
	GL::BPP(images[X]->getBpp()), 	\
	GL_UNSIGNED_BYTE, 				\
	images[X]->getDataPtr());

void the::cubetex::bind()
{
    logger::debug("[Tex3D  ] create : %s", getTag().c_str());
    glGenTextures(1, &_bindId);
    glBindTexture(GL_TEXTURE_CUBE_MAP,_bindId);
    DECL(POSITIVE_X)
    DECL(NEGATIVE_X)
    DECL(POSITIVE_Y)
    DECL(NEGATIVE_Y)
    DECL(POSITIVE_Z)
    DECL(NEGATIVE_Z)
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    GL_CHECK("the::cubetex::bind");
}

void the::cubetex::unbind()
{
    if(!glIsTexture(_bindId)) return; 
    glDeleteTextures(1,&_bindId);
    resetID();
    GL_CHECK("the::cubetex::unbind");
}
