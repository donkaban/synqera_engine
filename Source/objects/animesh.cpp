#include "TheEngine/objects/animesh.h"
#include "TheEngine/filesystem/file.h"
#include "TheEngine/engine.h"
#include "TheEngine/logger.h"


the::animesh::animesh() 
{   
  setType("Animesh");
}   
the::animesh::animesh(const std::string &filename) 
{
    setType("Animesh");
    load(filename);
    setMaterial(the::material::get("def::base"));
    setTexture(0,the::texture::get("def::base"));
}

void the::animesh::traverseBoneTree(the::node::cref node, const mat4 &ParentTransform, float runningTime, const BoneAnimationPtr &animation)
{
    mat4 NodeTransformation = node->getLocalTransform();

    mat4 curveTransformation;
    std::string nodeName = node->getTag();

    if (animation->getBoneTransform(nodeName, runningTime, curveTransformation))
        NodeTransformation = curveTransformation;

    auto GlobalTransformation = ParentTransform * NodeTransformation;

    if (boneMapping.find(nodeName) != boneMapping.end())
    {
        auto ndx = boneMapping[nodeName];
        auto FT  = GlobalTransformation * boneInfo[ndx].BoneOffset;

        boneInfo[ndx].FinalTransformation   = FT;
        bonesTransforms[ndx]                = FT;
        bonesTransformsTransposed[ndx]      = FT.transpose();
    }

    for (auto i = 0 ; i < node->getNumChilds() ; i++)
        traverseBoneTree(node->getChild(i), GlobalTransformation, runningTime, animation);
}

void the::animesh::updateAnim()
{
    if (currentAnimation != nullptr)
        traverseBoneTree(rootNode, mat4::identity(), (animateTimer.getTime() + animationDeltaTime)*currentAnimation->getFps(), currentAnimation);

    updateBind();
}

void the::animesh::setFrame(const std::string &animation, float frame)
{
    if (animations.find(animation) != animations.end())
    {
        // logger::error("set frame %f",frame);
        traverseBoneTree(rootNode, mat4_identity, frame, animations[animation]);
    }
    else
        logger::warning("Animation %s not found", animation.c_str());

    updateBind();
}

void the::animesh::loadAnimation(const std::string &filename, std::string animationTag)
{
    if(!the::filesystem::is_file(filename))
        FATAL_ERROR("file  %s  not exist!", filename.c_str());
    
    if (animations.find(animationTag) == animations.end())
    {
        BoneAnimationPtr bone_anim = BoneAnimationPtr(new BoneAnimation(filename));
        bone_anim->setTag(animationTag);
        animations[animationTag] = bone_anim;
    }
    else
        logger::warning("Animation %s already loaded", animationTag.c_str());
}

void the::animesh::play(std::string animationTag, float dt)
{
    animateTimer.reset();
    animationDeltaTime = dt;
    if (animations.find(animationTag) != animations.end())
        currentAnimation = animations[animationTag];
    else
        logger::error("Animation %s not found", animationTag.c_str());
}

BoneAnimationPtr the::animesh::getAnimation(const std::string &animation)
{
    if (animations.find(animation) != animations.end())
        return animations[animation];
    else
    {
        logger::error("Animation %s not found", animation.c_str());
        return nullptr;
    }
}


std::list<BoneAnimationPtr> the::animesh::getAnimationList()
{
    std::list<BoneAnimationPtr> baList;
    for (auto & i : animations)
        baList.push_back(i.second);
    return baList;
}

void the::animesh::serialize(const std::string &filename)
{
    the::object::serialize(filename);
    std::ofstream stream;
    stream.open(filename, std::ios::out | std::ios::binary | std::ios::app);
    int bonesCount = boneInfo.size();
    stream.write(reinterpret_cast<char *>(&bonesCount), sizeof(bonesCount));

    std::map<std::string, uint>::iterator iter;

    for (iter = boneMapping.begin(); iter != boneMapping.end(); ++iter)
    {
        int len = iter->first.length();
        stream.write(reinterpret_cast<char *>(&len), sizeof(int));
        stream.write(iter->first.c_str(), len);
        stream.write(reinterpret_cast<char *>(&iter->second), sizeof(iter->second));
    }

    stream.write(reinterpret_cast<char *>(&boneInfo[0]), sizeof(Bone)*bonesCount);

    serialize(rootNode, stream);

    stream.close();
}

void the::animesh::deserialize(the::filesystem::stream &stream)
{
    the::object::deserialize(stream);
    TheFile the_file(stream);
    ChunkHeader bonesHdr = the_file.getHeader(ChunkType::BONES);
    stream.seek(bonesHdr.offset);
    int bonesCount;
    stream.read(reinterpret_cast<char *>(&bonesCount), sizeof(int));
    boneMapping.clear();
    for (int i = 0; i < bonesCount; i++)
    {
        int len;
        stream.read(reinterpret_cast<char *>(&len), sizeof(int));
        char *str = new char[len + 1]();
        stream.read(str, len);
        str[len] = '\0';
        uint boneIndex;
        stream.read(reinterpret_cast<char *>(&boneIndex), sizeof(boneIndex));
        boneMapping[str] = boneIndex;
    }

    boneInfo.resize(bonesCount);
    stream.read((char *)&boneInfo[0], bonesCount * sizeof(Bone));

    rootNode = readNode(stream);
    deserialize(rootNode, stream);
}

the::node::ptr the::animesh::readNode(the::filesystem::stream &stream)
{
    int len;
    stream.read(reinterpret_cast<char *>(&len), sizeof(int));
    char *str = new char[len + 1]();
    stream.read(str, len);
    str[len] = '\0';
    mat4 t;
    stream.read(reinterpret_cast<char *>(&t), sizeof(float) * 16);

    auto n = std::make_shared<the::node>(str);
    n->setLocalTransform(t);
    return n;
}

void the::animesh::deserialize(the::node::cref parent, the::filesystem::stream &stream)
{
    int numChilds;
    stream.read(reinterpret_cast<char *>(&numChilds), sizeof(int));
    for (auto i = 0 ; i < numChilds ; i++)
    {
        auto n = readNode(stream);
        parent->addChild(n);
        deserialize(n, stream);
    }
}

void the::animesh::serialize(the::node::cref node, std::ofstream &stream)
{
    std::string name = node->getTag();
    int len = name.length();
    stream.write(reinterpret_cast<char *>(&len), sizeof(int));
    stream.write(name.c_str(), len);
    mat4 t = node->getLocalTransform();
    stream.write(reinterpret_cast<char *>(&t), sizeof(float) * 16);
    int numChilds = node->getNumChilds();
    stream.write(reinterpret_cast<char *>(&numChilds), sizeof(int));
    for (int i = 0; i < numChilds; i++)
        serialize(node->getChild(i), stream);
}

#define MIN(a,b)     a = std::min(a,b);
#define MIN_VEC3(a,b) MIN(a.x,b.x) MIN(a.y,b.y) MIN(a.z,b.z)

#define MAX(a,b)     a = std::max(a,b);
#define MAX_VEC3(a,b) MAX(a.x,b.x) MAX(a.y,b.y) MAX(a.z,b.z)

void the::animesh::load(const std::string &filename)
{
    unbind();
    this->filename = filename;
    the::object::load(filename);
    bonesTransforms.resize(boneInfo.size());
    bonesTransformsTransposed.resize(boneInfo.size());
    for (uint i = 0 ; i < bonesTransforms.size() ; i++)
    {
        bonesTransforms[i] = boneInfo[i].FinalTransformation;
        bonesTransformsTransposed[i] = bonesTransforms[i].transpose();
    }
    bind();
    float min = std::numeric_limits<float>::min();
    float max = std::numeric_limits<float>::max();
    std::vector<vec3> maxVector(bonesTransforms.size(), vec3(min, min, min));
    std::vector<vec3> minVector(bonesTransforms.size(), vec3(max, max, max));

    boneBoundingBoxes.resize(bonesTransforms.size());

    for (auto & v : vertexes)
    {
        if (v.weight.x != 0)
        {
            MAX_VEC3(maxVector[v.boneId.x], v.position)
            MIN_VEC3(minVector[v.boneId.x], v.position)
        }

        if (v.weight.y != 0)
        {
            MAX_VEC3(maxVector[v.boneId.y], v.position)
            MIN_VEC3(minVector[v.boneId.y], v.position)
        }

        if (v.weight.z != 0)
        {
            MAX_VEC3(maxVector[v.boneId.z], v.position)
            MIN_VEC3(minVector[v.boneId.z], v.position)
        }

        if (v.weight.w != 0)
        {
            MAX_VEC3(maxVector[v.boneId.w], v.position)
            MIN_VEC3(minVector[v.boneId.w], v.position)
        }
    }

    for (unsigned long i = 0; i < boneBoundingBoxes.size(); i++)
    {
        boneBoundingBoxes[i] = BoundingBox(minVector[i], maxVector[i]);
    }
}

void the::animesh::onRender(render::state &state)
{
    the::object::onRender(state);
    for (auto &s : surfaces)
    {
        s.setDefaultParameters(state);
        s.setUserParameters(state);
        s.setBones(state, bonesTransformsTransposed);
        s.draw();
    }
}

void the::animesh::updateBind()
{
    for (auto & item : bindingNodes)
    {
        BindPose pose = item.second;
        mat4 boneInWorld = bonesTransforms[pose.boneId] * boneInfo[pose.boneId].BoneOffset.inverse();
        mat4 m = getWorldTransform() * boneInWorld * pose.offset;
        item.first->setWorldTransform(m);
    }
}

void the::animesh::serialize(pugi::xml_node &node)
{
    node.set_name("node");
    node.append_attribute("name") = getTag().c_str();
    node.append_attribute("objectID") = getUID();
    node.append_attribute("type") = "AnimatedMesh";
    node.append_attribute("file") = filename.c_str();

    pugi::xml_node n;
    mat4 t = getLocalTransform();
    n = node.append_child("transform");   t.serialize(n);

    for (auto & a : animations)
    {
        n = node.append_child("animation");
        n.append_attribute("name") = a.first.c_str();
        n.append_attribute("file") = a.second->getFileName().c_str();
    }
    for (auto & s : surfaces)
    {
        n = node.append_child();
        s.serialize(n);
    }
    for (auto & c : childs)
    {
        pugi::xml_node n = node.append_child();
        c->serialize(n);
    }

    for (auto & pair : bindingNodes)
    {
        pugi::xml_node bind = node.append_child("bind");
        bind.append_attribute("objectId") = pair.first->getUID();
        bind.append_attribute("boneId") = pair.second.boneId;
        pugi::xml_node offset = bind.append_child("offset");
        pair.second.offset.serialize(offset);
    }
}

void the::animesh::deserialize(const pugi::xml_node &node)
{
    the::object::deserialize(node);
    setMaterial(the::material::get("def::base"));
    setTexture(0, the::texture::get("def::base"));
    std::string defaultPath = getDefaultPath();
    filename = std::string(node.attribute("file").value());

    load(defaultPath + filename);

    setTag(std::string(node.attribute("name").value()));
    for (auto s = node.child("surface"); s; s = s.next_sibling("surface"))
        getSurface(s.attribute("name").value()).deserialize(s,defaultPath);
    for (auto a = node.child("animation"); a; a = a.next_sibling("animation"))
        loadAnimation(defaultPath + a.attribute("file").value(), a.attribute("name").value());
}

void the::animesh::deserializePass2(const pugi::xml_node &node, std::unordered_map<uint, the::node::ptr> &objects)
{
    for (pugi::xml_node child = node.child("bind"); child; child = child.next_sibling("bind"))
    {
        uint objId = child.attribute("objectId").as_uint();
        mat4 offset;
        offset.deserialize(child.child("offset"));
        bindNode(objects[objId], child.attribute("boneId").as_int(), offset);
    }
}

std::vector<Intersection> the::animesh::getIntersection(vec3 p0, vec3 p1)
{
    auto iTransform = getWorldTransform().inverse();
    p0 = p0 * iTransform;
    p1 = p1 * iTransform;
    vec3 rayPos = p0;
    vec3 rayDir = (p1 - p0).normalize();
    std::vector<Intersection> intersections;
    int id = 0;
    for (auto & s : surfaces)
    {
        if (s.getIntersection() && s.getEnabled())
        {
            
            std::vector<uint16_t> &indices = s.getIndecies();
            for (size_t i = 0; i < indices.size(); i += 3)
            {
                Vertex v1 = vertexes[indices[i]];
                Vertex v2 = vertexes[indices[i + 1]];
                Vertex v3 = vertexes[indices[i + 2]];
                mat4 boneTransform1 = s.hasBones() ? bonesTransforms[v1.boneId.x] * v1.weight.x +
                                      bonesTransforms[v1.boneId.y] * v1.weight.y +
                                      bonesTransforms[v1.boneId.z] * v1.weight.z +
                                      bonesTransforms[v1.boneId.w] * v1.weight.w : mat4::identity();

                mat4 boneTransform2 = s.hasBones() ? bonesTransforms[v2.boneId.x] * v2.weight.x +
                                      bonesTransforms[v2.boneId.y] * v2.weight.y +
                                      bonesTransforms[v2.boneId.z] * v2.weight.z +
                                      bonesTransforms[v2.boneId.w] * v2.weight.w : mat4::identity();

                mat4 boneTransform3 = s.hasBones() ? bonesTransforms[v3.boneId.x] * v3.weight.x +
                                      bonesTransforms[v3.boneId.y] * v3.weight.y +
                                      bonesTransforms[v3.boneId.z] * v3.weight.z +
                                      bonesTransforms[v3.boneId.w] * v3.weight.w : mat4::identity();
                vec3 point;
                if (s.intersectTriangle(v1.position * boneTransform1, v2.position * boneTransform2, v3.position * boneTransform3, rayPos, rayDir, point))
                {
                    vec3 t1 = vertexes[indices[i]].position * boneTransform1;
                    vec3 t2 = vertexes[indices[i + 1]].position * boneTransform2;
                    vec3 t3 = vertexes[indices[i + 2]].position * boneTransform3;
                    intersections.push_back(Intersection(id, point, t1, t2, t3));
                }
            }
        }

        id++;
    }

    return intersections;
}

void the::animesh::bindNode(the::node::cref n, int boneId, mat4 offset)
{
    addChild(n);
    if (bindingNodes.find(n) == bindingNodes.end())
        bindingNodes[n] = BindPose(boneId, offset);
    else
        logger::warning("Node %s already binded",n->getTag().c_str());

    updateBind();
}

void the::animesh::bindNode(the::node::cref n, const std::string &boneName, mat4 offset)
{
    if (boneMapping.find(boneName) != boneMapping.end())
        bindNode(n, boneMapping[boneName], offset);
    else
        logger::error("Bone %s not found", boneName.c_str());

    updateBind();
}

void the::animesh::unbindNode(the::node::cref n)
{
    if (bindingNodes.find(n) != bindingNodes.end())
    {
        bindingNodes.erase(n);
        n->setParent(aux::engine->getRoot());
    }
    else
        logger::warning("Node %s not binded",n->getTag().c_str());
}

void the::animesh::setBindPosition(the::node::cref n, mat4 offset)
{
    if (bindingNodes.find(n) != bindingNodes.end())
        bindingNodes[n].offset = offset;
    else
        logger::warning("Node %s not binded",n->getTag().c_str());
}

void the::animesh::unbindAllNodes()
{
    bindingNodes.clear();
}

BoundingBox the::animesh::getBoneBoundingBox(int bone)
{
    BoundingBox b = boneBoundingBoxes[bone];
    b.transform = getWorldTransform() * bonesTransforms[bone];
    return b;
}

BoundingBox the::animesh::getBoundingBox()
{
    vec3 min = vec3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    vec3 max = vec3(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());

    for (unsigned int i = 0; i < boneBoundingBoxes.size(); i++)
    {
        BoundingBox bbox = boneBoundingBoxes[i];
        std::vector<vec3> vecs =
        {
            vec3(bbox.min.x, bbox.max.y, bbox.max.z) *bonesTransforms[i],
            vec3(bbox.max.x, bbox.max.y, bbox.max.z) *bonesTransforms[i],
            vec3(bbox.min.x, bbox.min.y, bbox.max.z) *bonesTransforms[i],
            vec3(bbox.max.x, bbox.min.y, bbox.max.z) *bonesTransforms[i],
            vec3(bbox.min.x, bbox.max.y, bbox.min.z) *bonesTransforms[i],
            vec3(bbox.max.x, bbox.max.y, bbox.min.z) *bonesTransforms[i],
            vec3(bbox.min.x, bbox.min.y, bbox.min.z) *bonesTransforms[i],
            vec3(bbox.max.x, bbox.min.y, bbox.min.z) *bonesTransforms[i]
        };

        for (auto & v : vecs)
        {
            MAX_VEC3(max, v)
            MIN_VEC3(min, v)
        }
    }

    BoundingBox b = BoundingBox(min, max, getWorldTransform());
    return b;
}

void the::animesh::log()
{
    ;  
    logger::system("AnimatedMesh '%s' order: %d blend: %s", 
        getTag().c_str(),
        getRenderOrder(),
        getBlend()? "true" : "false",
        childs.size());
    the::object::log();
    for (const auto &pair : boneMapping)
        logger::extra("[%02d] %-20s",  pair.second, pair.first.c_str());
    for(const auto &child : childs) child->log();
     
}