#include "TheEngine/animation/BoneAnimation.h"
#include "TheEngine/filesystem/file.h"
#include "TheEngine/logger.h"


BoneAnimation::BoneAnimation(const std::string &fileName):
    abstract::object("Animation",fileName)
{
    load(fileName);
}


std::shared_ptr<BoneNode> BoneAnimation::findNode(strref name)
{
    if (nodes.find(name) != nodes.end())
        return nodes[name];
    return nullptr;
}

bool BoneAnimation::getBoneTransform(strref boneName, float frame, mat4 &result)
{
    std::shared_ptr<BoneNode> bone = findNode(boneName);
    if (bone == nullptr)
        return false;

    currentFrame = frame <= duration ? frame : duration;

    vec3 scale = bone->calcInterpolatedScale(currentFrame);
    mat4 scaleM = mat4::scale(vec3(
        scale.x == 0 ? the::math::EPSILON : scale.x, 
        scale.y == 0 ? the::math::EPSILON : scale.y, 
        scale.z == 0 ? the::math::EPSILON : scale.z));

    quaternion rotation = bone->calcInterpolatedRotation(currentFrame);
    mat4 rotationM = rotation.getMat4();

    vec3 position = bone->calcInterpolatedPosition(currentFrame);
    mat4 positionM = mat4::translate(vec3(position.x, position.y, position.z));

    result = positionM * rotationM * scaleM;
    return true;
}

void BoneAnimation::serialize(strref filename)
{
    std::ofstream stream;
    stream.open(filename, std::ios::out | std::ios::binary);

    std::map<std::string, std::shared_ptr<BoneNode>>::iterator iter;

    stream.write(reinterpret_cast<char *>(&fps), sizeof(fps));
    stream.write(reinterpret_cast<char *>(&duration), sizeof(duration));

    int boneCount = nodes.size();
    stream.write(reinterpret_cast<char *>(&boneCount), sizeof(boneCount));

    for (iter = nodes.begin(); iter != nodes.end(); ++iter)
    {
        int len = iter->first.length();
        stream.write(reinterpret_cast<char *>(&len), sizeof(int));
        stream.write(iter->first.c_str(), len);
        iter->second->serialize(stream);
    }
}

void BoneAnimation::deserialize(the::filesystem::stream &stream)
{
    TheFile the_file(stream);
    ChunkHeader hdr = the_file.getHeader(ChunkType::BONE_ANIMATION_HEADER);
    stream.seek(hdr.offset);

    stream.read(reinterpret_cast<char *>(&fps), sizeof(float));
    stream.read(reinterpret_cast<char *>(&duration), sizeof(float));

    int boneCount;
    stream.read(reinterpret_cast<char *>(&boneCount), sizeof(int));

    std::vector<ChunkHeader> bone_curves = the_file.getHeaders(ChunkType::BONE_ANIMATION_CURVE);

    for (auto i = 0u; i < bone_curves.size(); i++)
    {
        stream.seek(bone_curves[i].offset);
        int len;
        stream.read(reinterpret_cast<char *>(&len), sizeof(int));
        char *str = new char[len + 1]();
        stream.read(str, len);
        str[len] = '\0';
        std::shared_ptr<BoneNode> bone = std::make_shared<BoneNode>(str);
        bone->deserialize(stream);

        nodes[str] = bone;
    }
}


void BoneAnimation::setNodeCurves(std::map<std::string, std::shared_ptr<BoneNode>> &n)
{
    nodes.clear();
    nodes.insert(n.begin(), n.end());
}
void BoneAnimation::setFps(float fps)              {this->fps = fps;}
void BoneAnimation::setDuration(float duration)    {this->duration = duration;}

float BoneAnimation::getFramesCount()  {return duration - 1;}
float BoneAnimation::getFps()          {return fps;}

void BoneAnimation::load(strref filename)
{
    this->file = filename;
    the::filesystem::stream stream(filename);
    deserialize(stream);
}

float  BoneAnimation::getCurrentFrame()    {return currentFrame;}
strref BoneAnimation::getFileName()        {return file;}