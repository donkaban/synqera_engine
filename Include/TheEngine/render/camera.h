#ifndef _THE_CAMERA_H_
#define _THE_CAMERA_H_

#include "TheEngine/objects/node.h"
namespace the {
class camera : public the::node
{
public:
  typedef std::shared_ptr<camera> ptr;
  typedef const ptr &             cref; 

  camera(float,float,float,float);
  camera(const pugi::xml_node &n);
  virtual ~camera();
  
  virtual void update();
  virtual void onRender(render::state &);
  virtual void serialize(pugi::xml_node &) ;
  virtual void deserialize(const pugi::xml_node &);
   
	float fov() 	 const;
  float aspect() const;
 	float zNear()  const;
  float zFar()   const; 

  void  fov(float);   
  void  aspect(float);
  void  zNear(float);
  void  zFar(float); 

	const mat4 & getPrjMatrix()   const;
  const mat4 & getViewMatrix()  const;
  const mat4 & getPVMatrix()    const;
  const vec3 & getEyePosition() const; 

private:
  mat4 prjMatrix     = mat4_identity;
  mat4 viewMatrix    = mat4_identity;
  mat4 prjViewMatrix = mat4_identity;
  vec3 eyePosition   = vec3_zero;

  float _fov;
  float _aspect = 1.0;
  float _zNear;
  float _zFar;
  bool  _needUpdate = true;

};
}

#endif 
