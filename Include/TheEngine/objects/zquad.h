#ifndef _BILLBOARD_H_
#define _BILLBOARD_H_

#include "TheEngine/objects/object.h"
namespace the {

class zquad : 
	public the::object
{
	THE_SHARED(zquad)
public:
	zquad(float zPosition); 
	virtual ~zquad() {} 
	virtual void serialize(pugi::xml_node &)         override ;
	virtual void deserialize(const pugi::xml_node &) override ;
	virtual void onRender(render::state &);
	virtual void load(strref);
	virtual void update();
	virtual void log();

	void setZPosition(float zPos);

private:
	mat4 prjViewMatrix;
	float zPosition;
};

}
#endif