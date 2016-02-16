#ifndef _THE_TEXTURE_3D_
#define _THE_TEXTURE_3D_

#include "TheEngine/common.h"
#include "TheEngine/resource/image.h"

namespace the {
class cubetex :
 	public abstract::object, 
 	public abstract::gapi,
    public ICacheable<cubetex>
    
{
	THE_SHARED(cubetex)
public:
	enum planes
	{
		NEGATIVE_X = 0,
		POSITIVE_X = 1,
		NEGATIVE_Y = 2,
		POSITIVE_Y = 3,
		NEGATIVE_Z = 4,
		POSITIVE_Z = 5
	};

    virtual void bind()     override; 
    virtual void unbind()   override;
    virtual void log()      override {}
    virtual ~cubetex();

    cubetex();
    cubetex(the::image::cref);
	cubetex(strref);
	
    cubetex(the::image::cref,the::image::cref,the::image::cref,the::image::cref,the::image::cref,the::image::cref);
	cubetex(strref,strref,strref,strref,strref,strref);
	cubetex(const pugi::xml_node &, strref path = aux::dataPath);

	void setImage(planes, the::image::cref);
	static void initDefaults();
	static std::string generateTag(strref,strref,strref,strref,strref,strref);
	static std::string generateTag(const pugi::xml_node &, strref);


private:
	void generateTag();
	the::image::ptr images[6];

};
}

#endif