#ifndef _THE_UNIT_H_
#define _THE_UNIT_H_

#include "TheEngine/common.h"
#include "TheEngine/unit/Transition.h"
#include "TheEngine/objects/animesh.h"
#include "TheEngine/unit/UnitAnimation.h"
#include "TheEngine/behavior/Action.h"


class Unit : 
    public abstract::object,
    public serialize::xml 
{
    THE_SHARED(Unit)
public:
    Unit(strref,strref);
    virtual ~Unit();

    inline the::animesh::ptr getRoot() {return mesh;}


   // void addAction(strref, const Action &);
    void addAction(strref, strref, Action::cbRef);
    void addAction(strref, int, Action::cbRef);
    void addTapAction(strref, strref, Action::cbRef);
    void addTapAction(strref, int, Action::cbRef);
    
    void removeAction(strref);
    void removeAllActions();

    void addTransition(strref, const std::shared_ptr<Transition> &t);
    void removeTransition(strref);
    void removeAllTransitions();
    void setTransitionCallback(strref, callable);
    void removeAllTransitionCallbacks();

    void play(const std::string &transition, float frame = 0);
    void playBackward(const std::string &transition, float frame);
    void playTo(const std::string &transition, float frame, float maxFarme);
    void playBackwardTo(const std::string &transition, float frame, float minFrame);
    
    void stop();
    void stop(const std::string &transition);
    float getCurrentFrame(const std::string &transition);
    float getNumFrames(const std::string &transition);
    void setFrame(const std::string &transition, float frame);

    //Joints
    void addJoint(const std::string &name, the::node::cref node, mat4 offset = mat4_identity);
    void removeJoint(the::node::cref node);
    void removeAllJoints();

    //Notifications
    void addNotification(const std::string &transition, int frame, const std::function<void(int)> &);
    void removeNotification(const std::string &name, int frame);

    //Textures
    void setTexture(strref surface, strref texture);
    void setTexture(int surface, strref texture);

    void setCubemap(int, strref t1,strref t2,strref t3,strref t4,strref t5,strref t6);
    void setCubemap(strref, strref t1,strref t2,strref t3,strref t4,strref t5,strref t6);
    void setCubemap(strref t1,strref t2,strref t3,strref t4,strref t5,strref t6);

    void setMaterial(strref surface, strref material);
    void setMaterialParameter(strref, strref, float);


    inline void pause()  {isPaused  = true;}
    inline void resume() {isPaused  = false; lastTimeUpdated = the::timer::globalTime();}

    inline bool isEnabled() {return enabled;}
    void setEnabled(bool mode);
    void setSurfaceEnabled(strref, bool);
    void setSurfaceEnabled(int, bool);
    void setSurfaceEnabled(bool);
    void update();

    //Transform
    inline const mat4 &getLocalTransform() const {return mesh->getLocalTransform();}
    inline const mat4 &getWorldTransform() const {return mesh->getWorldTransform();}
    inline mat4 getLocalTransformCopy() const {return mesh->getLocalTransformCopy();}
    inline mat4 getWorldTransformCopy() const {return mesh->getWorldTransformCopy();}
    inline const vec3  getLocalPosition()  {return mesh->getLocalPosition();}
    inline const vec3  getWorldPosition() {return mesh->getWorldPosition();}
    inline void setLocalTransform(const mat4 &m) {mesh->setLocalTransform(m);}
    inline void setWorldTransform(const mat4 &m) {mesh->setWorldTransform(m);}
    inline void setLocalPosition(const vec3 &v)  {mesh->setLocalPosition(v);}
    inline void setWorldPosition(const vec3 &v)  {mesh->setWorldPosition(v);}
    inline void rotateX(float v) {mesh->rotateX(v);}
    inline void rotateY(float v) {mesh->rotateY(v);}
    inline void rotateZ(float v) {mesh->rotateZ(v);}
    inline void rotate(const vec3 &v) {mesh->rotate(v);}
    inline void translate(const vec3 &v) {mesh->translate(v);}
    inline void scale(const vec3 &v) {mesh->scale(v);}

    void serialize(pugi::xml_node &);
    void deserialize(const pugi::xml_node &);
    void save(const std::string &);
    void load(const std::string &);

    friend  std::ostream &operator<<(std::ostream &o, const Unit &a) {o << a.tostring();return o;}

private:
    the::animesh::ptr mesh;
    std::string dataPath;

    dictionary<std::shared_ptr<Transition>> transitions;
    std::vector<UnitAnimation>   activeAnimations;
    
    double lastTimeUpdated  = 0.0;
    double dt               = 0.0;

    void updateTransitions();
  
    bool isPaused = false;
    bool enabled = true;

    int updateHandler = -1;

    void patchXml(pugi::xml_node node);
    
};




#endif //_UNIT_H_