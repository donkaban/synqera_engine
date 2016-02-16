#include "TheEngine/unit/Unit.h"
#include "TheEngine/engine.h"

Unit::Unit(strref dataPath, strref xmlPath) : 
    abstract::object("Unit","--"),
    dataPath(aux::engine->getDataPath()+dataPath)
{
    updateHandler = aux::engine->addUpdateHandler([&](float dt) {update();});    
    pugi::xml_document xml;
    auto filename = aux::engine->getDataPath() + xmlPath; 
    if(!the::filesystem::is_file(filename))
        FATAL_ERROR("file %s not exist",filename.c_str());
    xml.load(the::filesystem::load_as_string(filename).c_str());
    pugi::xml_node node = xml.child("unit");
    deserialize(node);
    setTag(mesh->getTag());
    aux::engine->addObject(mesh);
}

Unit::~Unit()
{
    aux::engine->delUpdateHandler(updateHandler);
    transitions.clear();
    activeAnimations.clear();
    aux::engine->removeObject(mesh);
}

void Unit::addTransition(strref name, const std::shared_ptr<Transition> &t)
{
    if (transitions.find(name) == transitions.end()) 
    {
        transitions[name] = t;
        //t->setAudio(dataPath + t->getAudio()); 
        //t->setAnimation(dataPath + t->getAnimation()); 
        mesh->loadAnimation(t->getAnimation(), name); 
    }
}

void Unit::setTransitionCallback(strref name, callable callback)
{
    if (transitions.find(name) != transitions.end()) 
        transitions[name]->setCallback(callback);
}

void Unit::removeAllTransitionCallbacks()
{
    // transitions.clear();
    for (auto & transition : transitions)
        transition.second->setCallback(callable());
}
void Unit::removeTransition(const std::string &name)
{
    transitions.erase(name);
}

void Unit::removeAllTransitions()
{
    transitions.clear();
}

void Unit::play(const std::string &transition, float frame)
{
    stop();
    if (transitions.find(transition) != transitions.end())
        activeAnimations.push_back(UnitAnimation(mesh, transitions[transition], transition,frame,mesh->getAnimation(transition)->getFramesCount(),false));
    else
        logger::error("cant find transition with name '%s'", transition.c_str());
}   

void Unit::playBackward(const std::string &transition, float frame)
{
    stop();
    if (transitions.find(transition) != transitions.end())
        activeAnimations.push_back(UnitAnimation(mesh, transitions[transition], transition, frame,0,true));
    else
        logger::error("cant find transition with name '%s'", transition.c_str());
}

void Unit::playTo(const std::string &transition, float frame, float maxFarme)
{
    stop();
    if (transitions.find(transition) != transitions.end())
        activeAnimations.push_back(UnitAnimation(mesh, transitions[transition], transition,frame,maxFarme,false));
    else
        logger::error("cant find transition with name '%s'", transition.c_str());
}

void Unit::playBackwardTo(const std::string &transition, float frame, float minFrame)
{
    stop();
    if (transitions.find(transition) != transitions.end())
        activeAnimations.push_back(UnitAnimation(mesh, transitions[transition], transition, frame,minFrame,true));
    else
        logger::error("cant find transition with name '%s'", transition.c_str());
}


void Unit::updateTransitions()
{
    auto it = activeAnimations.begin();
    while (it != activeAnimations.end()) 
    {
        if(it->update(dt))
        {
            TransitionPtr t = it->getTransition();
            it = activeAnimations.erase(it);

            if (t->hasCallback())
                t->invoke();
        }
        else
            ++it;
    }
}


void Unit::stop()
{
    for (auto & animation : activeAnimations)
        animation.stop();
    activeAnimations.clear();
}

void Unit::stop(const std::string &transition)
{
    auto it = activeAnimations.begin();
    while (it != activeAnimations.end())
    {
        it = std::find_if(it, activeAnimations.end(),[&](UnitAnimation a)
        {
            return a.getName() == transition;
        });
        if (it != activeAnimations.end())
        {
            it->stop();
            it = activeAnimations.erase(it);
        }
    }
}

float Unit::getCurrentFrame(const std::string &transition)
{
    auto result = std::find_if(activeAnimations.begin(), activeAnimations.end(),[&](UnitAnimation a)
    {
        return a.getName() == transition;
    });
    if (result != activeAnimations.end())
    {
        return result->getCurrentFrame();
    }
    else
    {
        logger::warning("Transition %s stopped", transition.c_str());
        return -1;
    }
}

float Unit::getNumFrames(const std::string &transition)
{
    BoneAnimationPtr anim = mesh->getAnimation(transition);
    if (anim != nullptr)
        return anim->getFramesCount();
    else
        return -1;
}

void Unit::setFrame(const std::string &transition, float frame)
{
    mesh->setFrame(transition, frame);
}

void Unit::addJoint(const std::string &name, the::node::cref node, mat4 offset)
{
    mesh->bindNode(node, name, offset);
    mesh->addChild(node);
}

void Unit::removeJoint(the::node::cref node)
{
    mesh->unbindNode(node);
    //mesh->removeChild(node);
}

void Unit::removeAllJoints()
{
    mesh->unbindAllNodes();
}

void Unit::addNotification(const std::string &transition, int frame, const std::function<void(int)> &callback)
{
    auto it = transitions.find(transition);
    if (it != transitions.end()) //Add transition
    {
        it->second->addNotification(TransitionNotification(frame, callback));
    }
}

void Unit::removeNotification(const std::string &transition, int frame)
{
    auto it = transitions.find(transition);
    if (it != transitions.end()) //Add transition
    {
        it->second->removeNotification(frame);
    }
}

void Unit::setTexture(strref surface, strref texture)
{
    std::string textureName = dataPath+texture;
    mesh->setTexture(surface,0, textureName);       
}

void Unit::setTexture(int surface, strref texture)
{
    std::string textureName = dataPath+texture;
    mesh->setTexture(surface,0, textureName);       
}

void Unit::setCubemap(int s, strref t1,strref t2,strref t3,strref t4,strref t5,strref t6)
{
    mesh->setCubemap(s,t1,t2,t3,t4,t5,t6);       

}
void Unit::setCubemap(strref s, strref t1,strref t2,strref t3,strref t4,strref t5,strref t6)
{
    mesh->setCubemap(s,0,t1,t2,t3,t4,t5,t6);       

}
void Unit::setCubemap(strref t1,strref t2,strref t3,strref t4,strref t5,strref t6)
{
    mesh->setCubemap(0,t1,t2,t3,t4,t5,t6);       

}


void Unit::setMaterial(const std::string &surface, const std::string &material)
{
   mesh->setMaterial(surface, the::material::get(material));
}

void Unit::setMaterialParameter(strref sname, strref pname, float val)
{
    mesh->setMaterialParam(sname, pname, val);

}


void Unit::setEnabled(bool mode)
{
    if (mode) resume();
    else     pause();
    mesh->setEnabled(mode);
    enabled = mode;
}

void Unit::setSurfaceEnabled(strref s, bool mode)
{
    mesh->setSurfaceEnabled(s,mode);
}

void Unit::setSurfaceEnabled(int s, bool mode)
{
    mesh->setSurfaceEnabled(s,mode);
}

void Unit::setSurfaceEnabled(bool mode)
{
    mesh->setSurfaceEnabled(mode);
}


void Unit::update()
{

    if (isPaused)
        return;
    auto curTime = the::timer::globalTime();
    dt = curTime - lastTimeUpdated;
    lastTimeUpdated = curTime;
    updateTransitions();
}


void Unit::addAction(strref name, strref surface, Action::cbRef callback)
{
    mesh->addAction(name,surface,callback);
}

void Unit::addAction(strref name, int surface, Action::cbRef callback)
{
    mesh->addAction(name,surface,callback);
}

void Unit::addTapAction(strref name, strref surface, Action::cbRef callback)
{
    mesh->addTapAction(name,surface,callback);
}

void Unit::addTapAction(strref name, int surface, Action::cbRef callback)
{
    mesh->addTapAction(name, surface, callback);
}


void Unit::removeAction(strref name)
{
    mesh->removeAction(name);
}

void Unit::removeAllActions()
{
    mesh->removeAllActions();
}

void Unit::serialize(pugi::xml_node &node)
{
    node.set_name("unit");
    pugi::xml_node meshNode = node.append_child();
    mesh->serialize(meshNode);
    for (auto & pair : transitions)
    {
        pugi::xml_node t = node.append_child("transition");
        t.append_attribute("name") = pair.first.c_str();
        t.append_attribute("sound") = aux::trim(pair.second->getAudio(), dataPath).c_str();
        t.append_attribute("animation") = aux::trim(pair.second->getAnimation(), dataPath).c_str();
        t.append_attribute("looped") = pair.second->isLooped();
    
    }
}

void Unit::deserialize(const pugi::xml_node &node)
{
    pugi::xml_node meshNode = node.child("node");
    mesh = the::node::ptr_cast<the::animesh>(the::node::load(meshNode, dataPath));
    mesh->orderSort();
    for (pugi::xml_node child = node.child("transition"); child; child = child.next_sibling("transition"))
    {
        TransitionPtr t = std::make_shared<Transition>();
        t->setAnimation(dataPath + child.attribute("animation").value());
        //t->setAudio(dataPath + child.attribute("sound").value());
        t->setLoop(child.attribute("looped").as_bool());
        addTransition(child.attribute("name").value(), t);
    }
}

void Unit::patchXml(pugi::xml_node node)
{
    pugi::xml_node meshNode = node.child("node");
    std::string modelPath = dataPath + std::string(meshNode.attribute("name").value());
    meshNode.attribute("name") = modelPath.c_str();
}

void Unit::save(const std::string &filename)
{
    pugi::xml_document xml;
    pugi::xml_node node = xml.append_child();
    serialize(node);
    xml.save_file(filename.c_str());
}

void Unit::load(const std::string &filename)
{
    pugi::xml_document xml;
    if(!the::filesystem::is_file(filename))
        FATAL_ERROR("file %s not exist",filename.c_str());
    xml.load(the::filesystem::load_as_string(filename).c_str());
    pugi::xml_node root  = xml.child("unit");
    if (!root)
        FATAL_ERROR("can't find <unit> root in %s",filename.c_str());
    deserialize(root);
}
