#include "TheEngine/objects/all.h"
#include "TheEngine/logger.h"
#include <algorithm>

the::node::node() :
    the::node("Node","--")
{}
the::node::node(strref t) :
    the::node("Node",t)
{}

the::node::node(strref type,strref tag) :
    abstract::object(type,tag)
{}  

the::node::~node() {}

bool   the::node::getEnabled()     const   {return _enabled;}
bool   the::node::getBlend()       const   {return _blending;}
int    the::node::getRenderOrder() const   {return _order;}
int    the::node::getNumChilds()   const   {return childs.size();}
strref the::node::getDefaultPath() const   {return defaultPath;}

the::node::ptr the::node::getThis() {return std::static_pointer_cast<the::node>(shared_from_this());}
the::node::ptr the::node::getParent()      const   {return parent.lock();}
the::node::ptr the::node::getChild(int ndx)        {return childs[ndx];}
the::node::ptr the::node::getChild(strref name)
{
    for (auto & ch : childs)
        if (ch->getTag().compare(name) == 0)
            return ch;
    return nullptr;
}

void the::node::setEnabled(bool mode)       {_enabled = mode;}
void the::node::setRenderOrder(int order)   {_order = order;}
void the::node::setBlend(bool mode)         {_blending = mode;}
void the::node::setDefaultPath(strref path) {defaultPath = path;}

const mat4 & the::node::getLocalTransform() const {return localTransform;}
const mat4 & the::node::getWorldTransform() const {return worldTransform;}

mat4 the::node::getWorldTransformCopy()     const {return worldTransform;}
mat4 the::node::getLocalTransformCopy()     const {return worldTransform;}
vec3 the::node::getLocalPosition()          const {return localTransform.getPosition();}
vec3 the::node::getWorldPosition()          const {return worldTransform.getPosition();}
vec3 the::node::getEuler()                  const {return worldTransform.getEuler();}             

void the::node::serialize(pugi::xml_node &node)
{
    logger::error("call not implemented method the::node::serialize()");
}

void the::node::deserialize(const pugi::xml_node &node)
{
    setLocalTransform(mat4(node.child("transform")));
    setTag(node.attribute("name").value());
    if(!node.attribute("order").empty())   _order = node.attribute("order").as_int();
    if(!node.attribute("enabled").empty()) _enabled = node.attribute("enabled").as_bool();
}

void the::node::log()
{
    logger::system("Node '%s' :", getTag().c_str());
    ;  
    for (const auto & ch : childs) ch->log();
     
}

void the::node::removeChild(const the::node::ptr &node)
{
    if(node)
    {
        childs.erase(std::remove(childs.begin(), childs.end(), node), childs.end());
        node->parent.reset();
        node->localTraversal();
    }
}

void the::node::setParent(const the::node::ptr &node)
{
    the::node::ptr par = parent.lock();
    if (par == node || node == nullptr)
        return;
    mat4 worldTransform = getWorldTransform();
    node->addChild(shared_from_this());
    setWorldTransform(worldTransform);
}

void the::node::addChild(const the::node::ptr &node)
{
    if (std::find(childs.begin(), childs.end(), node) != childs.end()) return;
    the::node::ptr par = node->parent.lock();
    if (par != nullptr)
        par->removeChild(node);
    node->parent = shared_from_this();
    childs.push_back(node);
    node->localTraversal();
}

void the::node::localTraversal()
{
    the::node::ptr par = parent.lock();
    if(!par)
        worldTransform = localTransform;
    else
        worldTransform = par->getWorldTransform() * localTransform;
    update();
    for (const auto & node : childs)
        node->localTraversal();
}

void the::node::worldTraversal()
{
    the::node::ptr par = parent.lock();
    if(par)
        localTransform = par->getWorldTransform().inverse() * worldTransform;
    else
        localTransform = worldTransform;
    update();
    for (const auto & node : childs)
        node->localTraversal();
}

void the::node::setLocalTransform(const mat4 &t)
{
    localTransform = t;
    localTraversal();
}
void the::node::setWorldTransform(const mat4 &t)
{
    worldTransform = t;
    worldTraversal();
}
void the::node::setLocalPosition(const vec3 &p)
{
    localTransform.setPosition(p);
    localTraversal();
}
void the::node::setWorldPosition(const vec3 &p)
{
    worldTransform.setPosition(p);
    worldTraversal();
}

void the::node::rotateX(float angle)
{
    localTransform *= mat4::rotateX(the::math::DEG2RAD * angle);
    localTraversal();
}
void the::node::rotateY(float angle)
{
    localTransform *= mat4::rotateY(the::math::DEG2RAD * angle);
    localTraversal();
}
void the::node::rotateZ(float angle)
{
    localTransform *= mat4::rotateZ(the::math::DEG2RAD * angle);
    localTraversal();
}
void the::node::rotate(const vec3 &v)
{
    localTransform *= mat4::rotateX(the::math::DEG2RAD * v.x) * mat4::rotateY(the::math::DEG2RAD * v.y) * mat4::rotateZ(the::math::DEG2RAD * v.z);
    localTraversal();
}
void the::node::rotate(const vec3 & axis, float angle)
{
    localTransform *= mat4::rotate(axis,the::math::DEG2RAD * angle);
    localTraversal();
}
void the::node::translate(const vec3 &v)
{
    localTransform *= mat4::translate(v);
    localTraversal();
}
void the::node::scale(const vec3 &v)
{
    localTransform *= mat4::scale(v);
    localTraversal();
}
void the::node::setScale(const vec3 &p)
{
    localTransform.setScale(p);
    localTraversal();
}
vec3 the::node::getScale()
{
    return localTransform.getScale();
}

void the::node::onRender(render::state &state)
{
    state.modelMatrix  = worldTransform;
   
}

the::node::ptr the::node::load(const pugi::xml_node &xml,const std::string &defpath)
{
    std::unordered_map<uint, the::node::ptr> objects;
    the::node::ptr n = _firstPass(xml, objects, defpath);
    n->orderSort();
    _secondPass(xml, objects);
    return n;
}

the::node::ptr the::node::load(const std::string &xmlPath, const std::string &defpath)
{
    logger::info("loading node: %s", xmlPath.c_str());
    pugi::xml_document xml;
    if(!the::filesystem::is_file(xmlPath))
        FATAL_ERROR("file %s not exist!",xmlPath.c_str());
    xml.load(the::filesystem::load_as_string(xmlPath).c_str());
    pugi::xml_node node = xml.child("node");
    return load(node, defpath);
}

the::node::ptr the::node::_firstPass(const pugi::xml_node &xml,std::unordered_map<uint, the::node::ptr> &objects, strref defpath)
{
    the::node::ptr n = nullptr;
    std::string type(xml.attribute("type").value());

    if(type.compare("Node")==0)                 n = std::make_shared<the::node>();
    else if(type.compare("AnimatedMesh")==0)    n = std::make_shared<the::animesh>();
    else if(type.compare("Animesh")==0)         n = std::make_shared<the::animesh>();
    else if(type.compare("Mesh")==0)            n = std::make_shared<the::mesh>();
    else if(type.compare("Object")==0)          n = std::make_shared<the::object>();
    else if(type.compare("TextField")==0)       n = std::make_shared<the::text>();
    else if(type.compare("Text")==0)            n = std::make_shared<the::text>();
    
    if(n)
    {        
        n->setDefaultPath(defpath);
        n->deserialize(xml);
        objects[xml.attribute("objectID").as_uint()] = n;
    }
    else
        FATAL_ERROR("[Node] Wrong xml tag: %s",xml.name());

    for(pugi::xml_node child = xml.child("node"); child; child = child.next_sibling("node"))
    {
        the::node::ptr c = _firstPass(child, objects, defpath);
        n->addChild(c);
    }
    return n;
}

void the::node::_secondPass(const pugi::xml_node &xml,std::unordered_map<uint, the::node::ptr> &objects)
{
    std::string type(xml.attribute("type").value());
    uint id = xml.attribute("objectID").as_uint();
    objects[id]->deserializePass2(xml,objects);
    for(pugi::xml_node child = xml.child("node"); child; child = child.next_sibling("node"))
        _secondPass(child, objects);
}

void the::node::foreachChild(const std::function<void(cref)> &func)
{
    for(auto &n : childs)
    {
        func(n);
        n->foreachChild(func);
    }
}

