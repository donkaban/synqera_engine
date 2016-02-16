#ifndef _THE_BONE_ANIMATION_
#define _THE_BONE_ANIMATION_

#include "TheEngine/common.h"
#include "TheEngine/math/all.h"
#include "TheEngine/animation/BoneNode.h"

class BoneAnimation : 
    public abstract::object
{
private:
    float fps;
    float duration;
    float currentFrame;

    std::shared_ptr<BoneNode> findNode(strref);
    std::map<std::string, std::shared_ptr<BoneNode>> nodes;

    std::string file;

public:
    
    BoneAnimation(strref);
    BoneAnimation() {}
    virtual ~BoneAnimation() {}
   
    bool getBoneTransform(strref, float, mat4 &);

    void serialize(strref);
    void deserialize(the::filesystem::stream &stream);
   
    void   setNodeCurves(std::map<std::string, std::shared_ptr<BoneNode>> &);
    void   setFps(float);
    void   setDuration(float);
    float  getFramesCount(); 
    float  getFps();
    float  getCurrentFrame();
    strref getFileName();

    void   load(strref);

    friend  std::ostream &operator<<(std::ostream &o, const BoneAnimation &a) {o << a.tostring();return o;}

};

typedef std::shared_ptr<BoneAnimation> BoneAnimationPtr;

#endif 
