#ifndef _THE_BONE_NODE_
#define _THE_BONE_NODE_

#include "TheEngine/common.h"
#include "TheEngine/objects/node.h"
#include "TheEngine/animation/Curve.h"
using namespace animation;

class BoneNode :
	public abstract::object
{
private:
	curve<vec3> 		positionKeys;
	curve<quaternion> 	rotationKeys;
	curve<vec3> 		scaleKeys;

	bool hasCurves = false;

public:
	vec3 calcInterpolatedPosition(float time);
	quaternion calcInterpolatedRotation(float time);
	vec3 calcInterpolatedScale(float time);

	inline bool isDummy() {return !hasCurves;}
	inline void setDummy(bool val){hasCurves=!val;}
	inline void setCurves(curve<vec3> &pos, curve<quaternion> &rot, curve<vec3> &scale)
	{
		hasCurves = true;
		this->positionKeys = pos;
		this->rotationKeys = rot;
		this->scaleKeys = scale;
	}
	
	BoneNode(std::string tag, curve<vec3> &pos, curve<quaternion> &rot, curve<vec3> &scale) 
	{
		setCurves(pos,rot,scale);
	}

	void serialize(std::ofstream& stream);
	void deserialize(the::filesystem::stream &stream);

	BoneNode(std::string tag){}

	virtual ~BoneNode(){}
};


#endif //_BONENODE_H_