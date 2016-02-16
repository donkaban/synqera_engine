#include "TheEngine/render/camera.h"
#include "TheEngine/objects/text.h"
#include "TheEngine/logger.h"

the::camera::camera(float f,float a, float zn, float zf) :
    the::node("render::camera"),
    _fov(f),
    _aspect(a),
    _zNear(zn),
    _zFar(zf)
{
    eyePosition   = getWorldPosition();
    logger::debug("[Camera ] create fov: %.2f, aspect: %.2f, [%.1f:%.1f:%.1f]",
        _fov,_aspect, eyePosition.x, eyePosition.y,eyePosition.z);

}

the::camera::camera(const pugi::xml_node &n) :
    the::node("render::camera")
{
    eyePosition   = getWorldPosition();
    deserialize(n);
    auto rot = getWorldTransform().getEuler(); 
    logger::debug("[Camera ] create fov: %.2f, aspect: %.2f, pos: [%.1f:%.1f:%.1f] rot: [%.1f:%.1f:%.1f]",
                _fov,_aspect, eyePosition.x, eyePosition.y,eyePosition.z,rot.x, rot.y, rot.z);

}
the::camera::~camera() {}
    
float the::camera::fov()    const {return _fov;}
float the::camera::zNear()  const {return _zNear;}
float the::camera::zFar()   const {return _zFar;}
float the::camera::aspect() const {return _aspect;}

void  the::camera::fov(float n)    {_fov    = the::math::max(the::math::EPSILON,n); update();}
void  the::camera::zNear(float n)  {_zNear  = the::math::max(the::math::EPSILON,n); update();}
void  the::camera::zFar(float n)   {_zFar   = the::math::max(the::math::EPSILON,n); update();}
void  the::camera::aspect(float n) {_aspect = the::math::max(the::math::EPSILON,n); update();}

const mat4 & the::camera::getPrjMatrix()  const  {return prjMatrix;}
const mat4 & the::camera::getViewMatrix() const  {return viewMatrix;}
const mat4 & the::camera::getPVMatrix()   const  {return prjViewMatrix;}

const vec3 & the::camera::getEyePosition() const {return eyePosition;}

void the::camera::update()
{
    the::node::update();
    eyePosition   = getWorldPosition();
    prjMatrix     = mat4::perspective(_fov, _aspect, _zNear, _zFar);
    viewMatrix    = getLocalTransform().inverse();
    prjViewMatrix = prjMatrix * viewMatrix;
    _needUpdate = true;
}

void the::camera::onRender(render::state &state)
{
    if(!_needUpdate) return;
    auto rot = getWorldTransform().getEuler(); 
    logger::debug("[Camera ] set fov: %.2f, aspect: %.2f, pos: [%.1f:%.1f:%.1f] rot: [%.1f:%.1f:%.1f] ",
                _fov,_aspect, eyePosition.x, eyePosition.y,eyePosition.z,rot.x, rot.y, rot.z);

    state.eyePosition       = eyePosition;
    state.projectionMatrix  = prjMatrix;
    state.viewMatrix        = viewMatrix;
    state.inverseViewMatrix = viewMatrix.inverse();
    state.prjViewMatrix     = prjViewMatrix;
    _needUpdate = false;
}

void the::camera::serialize(pugi::xml_node &node)
{
    logger::error("call not implemented the::camera::serialize()");
}

void the::camera::deserialize(const pugi::xml_node &node)
{
    _fov     = node.attribute("FOV").as_float();
    _zFar    = node.attribute("zFar").as_float();
    _zNear   = node.attribute("zNear").as_float();
    setWorldTransform(mat4(node));
    //setLocalPosition(eyePosition);
}