#ifndef _ANIMATED_MESH_H_
#define _ANIMATED_MESH_H_

#include "TheEngine/objects/object.h"
#include "TheEngine/animation/Bone.h"
#include "TheEngine/animation/BoneAnimation.h"
#include "TheEngine/timer.h"

namespace the {

class animesh : 
    public the::object
{
    THE_SHARED(animesh)
public:
    animesh(); 
    animesh(const std::string &filename); 

    virtual ~animesh() {}
    virtual void onRender(render::state &);
    virtual void update() {the::node::update();}

    virtual void serialize(const std::string &)      override;
    virtual void deserialize(the::filesystem::stream &)   override;
    virtual void serialize(pugi::xml_node &)         override;
    virtual void deserialize(const pugi::xml_node &) override;
    virtual void deserializePass2(const pugi::xml_node &, std::unordered_map<uint, the::node::ptr> &) override; 
  
    virtual void load(const std::string &);
    
    void loadAnimation(const std::string& filename, std::string tag);
    void play(std::string animationTag, float dt = 0);

    void stop(){currentAnimation = nullptr;}
    
    void addBones (std::vector<Bone> b) {boneInfo.clear(); boneInfo.insert(boneInfo.end(), b.begin(), b.end());}
    void setBoneMapping (std::map<std::string,uint> m) {boneMapping.clear(); boneMapping.insert(m.begin(), m.end());}
    void setRootBone(the::node::cref node) { rootNode = node;}
    void addBoneTransforms(std::vector<mat4>& b) { bonesTransforms = b;}
    void setFrame(const std::string &animation, float frame);
    void bindNode(the::node::cref n, int boneId , mat4 offset=mat4_identity);
    void bindNode(the::node::cref n, const std::string &boneName, mat4 offset=mat4_identity);
    void unbindNode(the::node::cref n);
    void setBindPosition(the::node::cref n, mat4 offset);
    void unbindAllNodes();

    virtual std::vector<Intersection> getIntersection(vec3 p0, vec3 p1);

    BoundingBox getBoneBoundingBox(int bone);
    virtual BoundingBox getBoundingBox();
    int getBonesCount() {return bonesTransforms.size();}

    BoneAnimationPtr getAnimation(const std::string &animation);
    
    std::list<BoneAnimationPtr> getAnimationList();
    
    virtual void log();

private:

    struct BindPose
    {
        int boneId;
        mat4 offset;

        BindPose(int boneId, mat4 offset)
        {
            this->boneId = boneId;
            this->offset = offset;
        }

        BindPose(){}
    };

    the::node::ptr rootNode;

    //bones
    std::vector<mat4>           bonesTransforms;
    std::vector<mat4>           bonesTransformsTransposed;
    
    std::map<std::string,uint>  boneMapping;
    std::vector<Bone>           boneInfo;

    //animations
    the::timer animateTimer; 
    std::map<std::string,BoneAnimationPtr> animations;
    BoneAnimationPtr currentAnimation = nullptr;
    float animationDeltaTime = 0;

    void traverseBoneTree(the::node::cref node,const mat4& ParentTransform,float runningTime,const BoneAnimationPtr &animation);
    void serialize(the::node::cref node, std::ofstream &stream);
    void deserialize(the::node::cref parent, the::filesystem::stream &stream);
   
    void updateBind();
    void updateAnim();

    the::node::ptr readNode(the::filesystem::stream &stream);


    std::unordered_map<the::node::ptr,BindPose> bindingNodes;
    std::vector<BoundingBox> boneBoundingBoxes;

    std::string filename;

   
};
}


#endif



















