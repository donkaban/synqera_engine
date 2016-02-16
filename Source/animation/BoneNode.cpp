#include "TheEngine/animation/BoneNode.h"
#include "TheEngine/math/all.h"
#include "TheEngine/logger.h"


vec3 BoneNode::calcInterpolatedPosition(float time)
{
    vec3    start,end;
    float   factor;
    if(!positionKeys.getValue(time, start, end, factor))
        logger::error("interpolate animation position problem");
    return the::math::lerp(start, end, factor);
}

quaternion BoneNode::calcInterpolatedRotation(float time)
{
    quaternion  start,end;
    float       factor;
    if(!rotationKeys.getValue(time, start, end, factor))
        logger::error("interpolate animation rotation problem");
    return quaternion::slerp(start, end, factor);
}

vec3 BoneNode::calcInterpolatedScale(float time)
{
    vec3    start,end;
    float   factor;
    if(!scaleKeys.getValue(time, start, end, factor))
        logger::error("interpolate animation scale problem");
    return the::math::lerp(start, end, factor);
}

void BoneNode::serialize(std::ofstream &stream)
{
    positionKeys.serialize(stream);
    rotationKeys.serialize(stream);
    scaleKeys.serialize(stream);
}

void BoneNode::deserialize(the::filesystem::stream &stream)
{
    positionKeys.deserialize(stream);
    rotationKeys.deserialize(stream);
    scaleKeys.deserialize(stream);
}