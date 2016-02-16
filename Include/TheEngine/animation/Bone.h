#ifndef _THE_BONE_H_
#define _THE_BONE_H_

#include "TheEngine/math/all.h"

struct Bone
{
    mat4 BoneOffset   		 = mat4_identity;
    mat4 FinalTransformation = mat4_identity;
};

#endif 