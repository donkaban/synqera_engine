#include "TheEngine/render/scene.h"

the::scene::scene(const pugi::xml_node &root)
{
    _tag = root.attribute("name").value();
    logger::info("[Scene] load '%s' from xml ",_tag.c_str());
    ;  
    auto cameraNode     = root.child("camera");
    auto lightNode      = root.child("light");
    auto cubeNode      = root.child("cubemap");
    if(cameraNode.empty()) FATAL_ERROR("can't find mandatory <camera> section in <scene>");
    if(lightNode.empty())  FATAL_ERROR("can't find mandatory <light>  section in <scene>");
    _camera = std::make_shared<the::camera>(cameraNode);
    _light  = std::make_shared<the::light>(lightNode);
    if(!cubeNode.empty())
        _cubemap = the::cubetex::addToCache(std::make_shared<the::cubetex>(cubeNode),"the::scene::cubemap");    
     
    
}
the::scene::~scene()
{
    logger::info("[Scene] destroy '%s' from xml ",_tag.c_str());
}   

the::camera::cref  	the::scene::getCamera()  const {return _camera;} 
the::light::cref    the::scene::getLight()   const {return _light;}
the::cubetex::cref  the::scene::getCubeMap() const {return _cubemap;}  

void the::scene::onRender(render::state &state)
{
	_camera->onRender(state);
	_light->onRender(state);
}
strref the::scene::getTag() const {return _tag;}

