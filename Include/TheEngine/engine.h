#ifndef _THE_ENGINE_MAIN_HEADER_
#define _THE_ENGINE_MAIN_HEADER_

#include "TheEngine/common.h"
#include "TheEngine/common/taskQueue.h"

#include "TheEngine/render/all.h"
#include "TheEngine/geometry/all.h"
#include "TheEngine/objects/all.h"
#include "TheEngine/resource/all.h"

#include "TheEngine/aux.h"
#include "TheEngine/logger.h"

#include "TheEngine/timer.h"
#include "TheEngine/platform/window.h"
#include "TheEngine/platform/input.h"

#include "TheEngine/material.h"
#include "TheEngine/surface.h"
#include "TheEngine/unit/Unit.h"
#include "TheEngine/render/scene.h"

#include "pugixml.hpp"

#ifdef THE_ANDROID_BUNDLE
    #include "TheEngine/platform/droid.h"
#endif

namespace the {
class engine
{
    THE_SHARED(engine)
public:
    using upd_t = const std::function<void(float)> &;
    using ini_t = const std::function<void(void)> &;
    using evt_t = const std::function<void(std::string)> &;
    using key   = size_t;

    virtual ~engine();

    static ptr & create(strref);
    static ptr & get();
    
    void  log();

    void  pause();
    void  resume(); 
    void  shutdown(); 
    void  resize(uint, uint); 
    bool  update();

    ptr & mainLoop();
    ptr & tuneUpdate();
    
    platform::input::ptr & getInput();
    the::renderer::ptr   & getRender();
    the::scene::ptr      & getScene(); 
    the::plotter::ptr    & getPlotter();
    the::node::ptr       & getRoot();

    ivec2   getViewport()      const;
    float   getFPS()           const;
    strref  getDataPath()      const;
    int     getFilteringMode() const;
    float   getFrameTime()     const;
   
    void bindAll();
    void unbindAll();
   
    ptr & addObject(the::node::cref);
    ptr & addMesh(the::mesh::cref);
    ptr & removeObject(the::node::ptr);
    ptr & showFPS(bool mode);
    ptr & takeScreenshot();
    
    key  addUpdateHandler(upd_t);
    key  addInitHandler(ini_t);
    key  addExitHandler(ini_t);
 
    void delUpdateHandler(key);
    void delInitHandler(key);
    void delExitHandler(key);
    
private:
    the::scene::ptr  scene;
    the::node::ptr   rootNode;
    the::text::ptr   fpsTextField;
    
    platform::window::ptr window;
    platform::input::ptr  input;
    the::renderer::ptr    render;
    
    std::string        configFile;
   
    int                filteringMode = 0; 
    float              currentTime;
    the::image         screenshot;
    
    taskQueue<void(float)> _updateQueue;
    taskQueue<void(void)>  _initQueue;
    taskQueue<void(void)>  _exitQueue;
      
    engine(strref);
    struct private_deleter {void operator ()(engine *p) {delete p;}};
    
    void init(const pugi::xml_node &);

    void resizeFpsCounter();
};
}   



#ifndef THE_ANDROID_BUNDLE                     
#define BUNDLE_ENTRY_POINT static auto _e_ = the::engine::create("./assets/config.xml"); int main() 
#define RUN_APP(name) BUNDLE_ENTRY_POINT {auto A = std::make_shared<name>(); A->bind(); _e_->mainLoop(); return 0;}  
#else                                           
#define BUNDLE_ENTRY_POINT                                              \
    extern "C" JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *_reserved);  \
    extern "C" DECL_BOOL_METHOD(bundleMain) (JNIEnv *env, jobject obj); \
    JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *_reserved)              \
    {                                                                   \
        ANDROID_DBG("[JNI SYS] entry point bundle loaded");             \
        return JNI_VERSION_1_6;                                         \
    }                                                                   \
    DECL_BOOL_METHOD(bundleMain) (JNIEnv *env, jobject obj)
#define RUN_APP(name)                                                   \
    static std::shared_ptr<name> A;                                     \ 
    BUNDLE_ENTRY_POINT {                                                \
        A = std::make_shared<name>();                                   \
        A->bind();                                                      \
        A->onInit();                                                    \
        return 0;                                                       \
    }  
#endif   

#endif                                       
