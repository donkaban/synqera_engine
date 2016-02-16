#ifndef _THE_SCENE_
#define _THE_SCENE_

#include "TheEngine/render/camera.h"
#include "TheEngine/render/light.h"
#include "TheEngine/render/rstate.h"
#include "TheEngine/resource/cubetex.h"

namespace the {
class scene
{

public:
  	using ptr   = std::shared_ptr<scene>;
    using cref  = const ptr &;
  
	scene(const pugi::xml_node &);
    ~scene();

 	the::camera::cref 	   getCamera()  const;
 	the::light::cref  	   getLight()   const;
 	the::cubetex::cref getCubeMap() const;  
 	void               onRender(render::state &); 
 	strref             getTag() const;

private:
	std::string        _tag; 
 	the::camera::ptr   _camera;
    the::light::ptr    _light;
    the::cubetex::ptr  _cubemap;
};
}

#endif
