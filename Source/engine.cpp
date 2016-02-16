#include "TheEngine/engine.h"   
#include "TheEngine/aux.h"
#include "TheEngine/logger.h"

#include <functional> 

the::engine::ptr & the::engine::create(strref config)
{
    if(aux::engine == nullptr)
        if(!aux::log)
            aux::log = logger::make("the.log");
        aux::engine = std::shared_ptr<the::engine>(new the::engine(config),private_deleter());        
    return aux::engine;
}

the::engine::ptr & the::engine::get()
{
    if(aux::engine == nullptr)
        FATAL_ERROR("[Engine ] try get non existing TheEngine instance.");
    return aux::engine;
}

the::engine::engine(strref filename) :
    _updateQueue("onUpdate"),
    _initQueue("onInit"),
    _exitQueue("onExit")
{
    the::timer::lock();
    auto loadTime = the::timer::trace([&]()
    {
        logger::info("[Engine ] create engine");
                configFile = the::filesystem::load_as_string(filename);
        pugi::xml_document xml;
        if(!xml.load(configFile.c_str()))
            FATAL_ERROR("[Engine ] can't open config file '%s'",filename.c_str());
        auto engineConfigNode  = xml.child("TheEngine");
        if(engineConfigNode.empty())
            FATAL_ERROR("[Engine ] can't find <TheEngine> root in '%s'",filename.c_str());
        init(engineConfigNode);
    });
    logger::warning("[Engine ] TheEngine module was loaded in %0.2f sec",loadTime);
}

the::engine::~engine() 
{
    logger::info("[Engine ] closed successfuly");
} 

platform::input::ptr  & the::engine::getInput() {return input;}

void the::engine::log()
{
    logger::system("-----------------------------------------------------");
    logger::system("[Engine ] Build date : %s",ENGINE_BUILD_DATE);
    logger::system("[Engine ] Version    : %d.%d.%d [%s]",ENGINE_VERSION_MAJOR,ENGINE_VERSION_MINOR,ENGINE_VERSION_WIP,ENGINE_VERSION_TAG);
    logger::system("[Engine ] Platform   : %s",THE_PLATFORM_NAME);
    logger::system("[Engine ] Render     : %s",THE_RENDER_NAME);
    logger::system("[Engine ] Sound      : %s",THE_SOUND_NAME);
    logger::system("-----------------------------------------------------");
}    

the::engine::ptr & the::engine::tuneUpdate()
{
    window->updateHandler([&](float dt)
    {
        if(!aux::runned) return;
        float current = the::timer::globalTime();
        float delta = current - currentTime;
        currentTime = current;
        _updateQueue.emit(delta).update();
        the::timer::update_all();
        render->render(scene,rootNode);
        if(fpsTextField->getEnabled()) fpsTextField->setText(aux::format("%0.2f / %d",getFPS(), render->getNumTriangles()));
    });   
    logger::info("[Engine ] call onInit() callbacks");
        auto t = the::timer::trace([&]() {_initQueue.emit().update();});
    logger::info("[Engine ] onInit() callbacks time: %0.2f seconds",t );
    the::timer::unlock();
    return aux::engine;
}

void the::engine::shutdown()
{
    logger::system("[ENGINE ] SHUTDOWN");
    _exitQueue.emit().update();
    aux::runned = false;
    _updateQueue.delAll().update();
    _initQueue.delAll().update();
    _exitQueue.delAll().update();

    unbindAll();
    logger::info("[Engine ] clear caches ..."); 
            the::texture::clearCache();
        the::cubetex::clearCache();
        the::material::clearCache();
        the::font::clearCache();
        the::image::clearCache();
    the::sound::close();
}


the::engine::ptr & the::engine::mainLoop()  
{
    #ifndef PLATFORM_ANDROID                     
        tuneUpdate();
        logger::info("[Engine ] start internal update looop");
        while(window->update()) 
        {}
        logger::info("[Engine ] end internal update loop");
        shutdown(); 
    #endif
    return aux::engine;
}


bool the::engine::update()    
{
    if(aux::runned)
        return window->update();
    return true;
}

void the::engine::pause()
{
    logger::system("[ENGINE ] PAUSE");
    aux::runned = false;
}

void the::engine::resume()
{
    logger::system("[ENGINE ] RESUME");
    bindAll(); 
    aux::runned = true;
}

void the::engine::resize(uint w, uint h) 
{   
    
    window->resize(w,h);
    render->resize(w,h);
    getScene()->getCamera()->aspect(float(w)/float(h));
    resizeFpsCounter();
}     

the::engine::ptr & the::engine::removeObject(the::node::ptr o) 
{
    logger::debug("[Engine ] '%s' was removed from render tree",o->getTag().c_str());
    if(o->getParent())
        o->getParent()->removeChild(o);
    rootNode->removeChild(o);
    return aux::engine;
}

the::engine::ptr & the::engine::addObject(the::node::cref o)    
{
    rootNode->addChild(o); 
    logger::debug("[Engine ] '%s' was added to render tree",o->getTag().c_str());
    o->log();
    return aux::engine;  
}
the::engine::ptr & the::engine::addMesh(the::mesh::cref o) {return addObject(o);}     

size_t the::engine::addUpdateHandler(upd_t hdl)     {return _updateQueue.add(hdl);}
size_t the::engine::addInitHandler(ini_t hdl)       {return _initQueue.add(hdl);}
size_t the::engine::addExitHandler(ini_t hdl)       {return _exitQueue.add(hdl);}

void   the::engine::delUpdateHandler(size_t id)     {_updateQueue.del(id);}
void   the::engine::delInitHandler(size_t id)       {_initQueue.del(id);}
void   the::engine::delExitHandler(size_t id)       {_initQueue.del(id);}


the::renderer::ptr & the::engine::getRender()  {return render;}
the::node::ptr     & the::engine::getRoot()    {return rootNode;}
the::scene::ptr    & the::engine::getScene()   {return scene;}
the::plotter::ptr  & the::engine::getPlotter() {return render->getPlotter();}

strref  the::engine::getDataPath()      const {return aux::dataPath;}
ivec2   the::engine::getViewport()      const {return ivec2(window->width(), window->height());}   
float   the::engine::getFPS()           const {return window->fps();}
int     the::engine::getFilteringMode() const {return filteringMode;}
float   the::engine::getFrameTime()     const {return currentTime;}

   
void the::engine::init(const pugi::xml_node &root)
{
    logger::info("[Engine ] load config from xml"); 
    auto systemNode     = root.child("system");
    auto sceneNode      = root.child("scene");
    auto materialsNode  = root.child("materials");
    auto texturesNode   = root.child("textures");
    auto cubemapsNode   = root.child("cubemaps");
    
    if(systemNode.empty()) FATAL_ERROR("[Engine ] can't find mandatory <system> section in config file");
    if(sceneNode.empty())  FATAL_ERROR("[Engine ] can't find mandatory <scene> section in config file");
  
    aux::log->setLevel(systemNode.attribute("log").value());
    auto w = the::math::max(systemNode.attribute("width").as_int() ,512);
    auto h = the::math::max(systemNode.attribute("height").as_int(),512); 
    #ifdef PLATFORM_ANDROID
        aux::dataPath = "";
    #else
        aux::dataPath = systemNode.attribute("dataPath").value();
    #endif
    filteringMode = systemNode.attribute("filtering").as_int();

    window = std::make_shared<platform::window>(w,h,systemNode.attribute("fullscreen").as_bool());
    window->resizeHandler([&](uint w, uint h)  {getScene()->getCamera()->aspect(float(w)/float(h));});
    input = std::make_shared<platform::input>();
#ifndef PLATFORM_ANDROID
    using std::placeholders::_1;
    using std::placeholders::_2;
    window->setKeyHandler     (std::bind(&platform::input::onKey,     input, _1, _2));
    window->setPressHandler   (std::bind(&platform::input::onPress,   input, _1, _2));
    window->setReleaseHandler (std::bind(&platform::input::onRelease, input, _1, _2));
    window->setMoveHandler    (std::bind(&platform::input::onMove,    input, _1, _2));
#endif
    log();
    the::sound::init();
    the::image::initDefaults();
    the::texture::initDefaults();
    the::cubetex::initDefaults();
    the::material::initDefaults();
    the::font::initDefaults();

    render   = std::make_shared<the::renderer>(window->width(), window->height());    
    rootNode = std::make_shared<the::node>("engine::rootNode");
    scene    = std::make_shared<the::scene>(sceneNode);
    getScene()->getCamera()->aspect(float(w)/float(h));
  
    if(!materialsNode.empty())
    {    
        std::string ns = (materialsNode.attribute("namespace") != nullptr) ? (materialsNode.attribute("namespace").value()+ std::string("::")) : ""; 
        logger::info("[Engine ] load materials");
                for(pugi::xml_node mat = materialsNode.first_child(); mat; mat=mat.next_sibling())
            the::material::addToCache(material::make(mat),ns+mat.attribute("name").value());     
    }
    if(!texturesNode.empty())
    {    
        std::string ns = (texturesNode.attribute("namespace") != nullptr) ? (texturesNode.attribute("namespace").value()+ std::string("::")) : ""; 
        logger::info("[Engine ] load textures");
                for(pugi::xml_node tex = texturesNode.first_child(); tex; tex=tex.next_sibling())
            the::texture::addToCache(the::texture::make(aux::dataPath+tex.attribute("file").value()),ns+tex.attribute("name").value());     
    }

    if(!cubemapsNode.empty())
    {    
        std::string ns = (cubemapsNode.attribute("namespace") != nullptr) ? (cubemapsNode.attribute("namespace").value()+ std::string("::")) : ""; 
        logger::info("[Engine ] load cubemaps");
                for(pugi::xml_node tex = cubemapsNode.first_child(); tex; tex=tex.next_sibling())

            the::cubetex::addToCache(the::cubetex::make(tex, getDataPath()),ns+tex.attribute("name").value());     
    }
    logger::info("[Engine ] caches:");
        the::material::foreach([](strref tag, the::material::ptr m)  {m->setTag(tag);m->log();});    
    the::texture::foreach([](strref tag,the::texture::ptr t)     {t->setTag(tag);t->log();});    
    logger::debug("[Engine ] data path is '%s'", getDataPath().c_str());
     
    fpsTextField = the::text::make(the::font::get("def::base"),the::material::get("def::text"),"FPS");
    
    fpsTextField->setTag("FPS");
    showFPS(systemNode.attribute("showFps").as_bool());
    fpsTextField->setRenderOrder(the::math::INF);
    addObject(fpsTextField);
    resizeFpsCounter();
    aux::runned = true;
}

void the::engine::resizeFpsCounter()
{
    auto cam = getScene()->getCamera();
    float yPos = tan(the::math::DEG2RAD *  cam->fov() * 0.5) * cam->zNear();
    float xPos = yPos * cam->aspect();
    
    fpsTextField->setScreenSize(2,yPos*2);
    fpsTextField->setSize(0.35*(30.0f/window->height()));
    fpsTextField->setWorldTransform(
        cam->getWorldTransform()*
        mat4::translate(vec3(-xPos,yPos,-(cam->zNear()+0.001)))
    );
}

the::engine::ptr & the::engine::showFPS(bool mode)
{
    fpsTextField->setEnabled(mode);
    return aux::engine;
}

void the::engine::bindAll()
{
    the::material::foreach([](strref,the::material::ptr n) {n->bind();});
    the::texture::foreach([](strref,the::texture::ptr n)   {n->bind();});
    the::cubetex::foreach([](strref,the::cubetex::ptr n)   {n->bind();});
    the::font::foreach([](strref,the::font::ptr n)         {n->bind();});
    the::fbuffer::foreach([](strref,the::fbuffer::ptr n)   {n->bind();});
    rootNode->foreachChild([](the::node::ptr n)            {n->bind();}); 
}

void the::engine::unbindAll()
{
    the::material::foreach([](strref,the::material::ptr n) {n->unbind();});
    the::texture::foreach([](strref,the::texture::ptr n)   {n->unbind();});
    the::cubetex::foreach([](strref,the::cubetex::ptr n)   {n->unbind();});
    the::font::foreach([](strref,the::font::ptr n)         {n->unbind();});
    the::fbuffer::foreach([](strref,the::fbuffer::ptr n)   {n->unbind();});
    rootNode->foreachChild([](the::node::ptr n)            {n->unbind();});
}
 
the::engine::ptr & the::engine::takeScreenshot()
{
    auto fn = aux::format("shot-%s.png",the::timer::timestamp().c_str());
    auto w = window->width();
    auto h = window->height();
    logger::debug("[Engine ] take screenshot %dx%d -> %s", w,h, fn.c_str() );
    screenshot.setDataFromScreen(w,h);
    std::thread save_thr([&](){screenshot.savePNG(fn);});
    save_thr.detach();
    return aux::engine;
}
