#ifndef _THE_UNIT_ANIMATION_H_
#define _THE_UNIT_ANIMATION_H_

#include "TheEngine/common.h"
#include "TheEngine/unit/Transition.h"
#include "TheEngine/objects/animesh.h"
#include "TheEngine/resource/sound.h"

class UnitAnimation
{
public:
    UnitAnimation(the::animesh::cref, const TransitionPtr &, strref name, float,float,bool);
    inline const TransitionPtr& getTransition() {return transition;}
    bool update(float dt);
    void stop();

    inline float getCurrentFrame(){return currentFrame;}
    inline const std::string getName(){return name;}
private:
    TransitionPtr       transition;
    the::animesh::ptr   mesh;
    the::sound::ptr     sound;
    
    bool revert         = false;
    float currentFrame  = 0;
    float maxFrame  = 0;
    float endFrame = 0;
    float fps;
    int lastNotificationFrame = -1;
    
    std::string name;
    
    bool updateRevert(float);
    bool updateForward(float);

};

#endif