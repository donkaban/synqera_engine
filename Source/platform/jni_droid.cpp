#ifdef PLATFORM_ANDROID

#include "TheEngine/engine.h"
#include "TheEngine/platform/input.h"
#include "TheEngine/platform/droid.h"


#include <iostream>

#include <thread>
#include <mutex>

#define HAVE_INTTYPES_H

static platform::input::ptr input;
static std::mutex           locker;
static JavaVM *             javaVM;

#define EXIST_GUARD if(!aux::engine) return;
#define LOCK_GUARD  std::lock_guard<std::mutex> lock(locker);

std::string jstr2str(jstring &js, JNIEnv *env)
{
    const char * raw = env->GetStringUTFChars(js,0);
    auto result = std::string(raw);
    env->ReleaseStringUTFChars(js, raw);
    return result; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
    
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *_reserved)
{
    ANDROID_DBG("[JNI SYS] native module TheEngine loaded");
    javaVM = vm;
    return JNI_VERSION_1_6;
}
JNIEXPORT void JNI_OnUnload(JavaVM* vm, void* reserved)
{
    logger::system("[JNI SYS] native module TheEngine closed");
}


DECL_VOID_METHOD(log) (JNIEnv *env, jobject obj, jstring msg)
{
    logger::info("[JavaVM ] %s", jstr2str(msg, env).c_str());
}

DECL_VOID_METHOD(init) (JNIEnv *env, jobject obj, jstring config, jobject assetMgr)
{
    logger::system("[JNI SYS] INIT");
    if(!aux::engine)
    {
        LOCK_GUARD
        auto cfg_file = jstr2str(config,env);
        logger::debug("[JNI SYS] config file: '%s'",cfg_file.c_str());
        aux::assetManager = AAssetManager_fromJava(env, assetMgr);
        if(!aux::assetManager)
            FATAL_ERROR("[JNI SYS] can't get android asset manager");
        aux::engine = the::engine::create(cfg_file);
        input  = aux::engine->getInput();
    }
    else
        if(aux::engine) aux::engine->resume();   
}

DECL_VOID_METHOD(draw) (JNIEnv *env, jobject obj)
{

    locker.lock();
        input->updateState();
    locker.unlock();
    aux::engine->update();
}

DECL_VOID_METHOD(pause) (JNIEnv *env, jobject obj)
{
    LOCK_GUARD
    logger::system("[JNI SYS] PAUSE");
    if(aux::engine) aux::engine->pause();
}

DECL_VOID_METHOD(resume) (JNIEnv *env, jobject obj)
{
    logger::system("[JNI SYS] RESUME");
}

DECL_VOID_METHOD(shutdown) (JNIEnv *env, jobject obj)
{
    LOCK_GUARD
    logger::system("[JNI SYS] SHUTDOWN");
    if(aux::engine) aux::engine->shutdown();
}   

DECL_VOID_METHOD(resize) (JNIEnv *env, jobject obj, jint w, jint h)
{
    static bool first = true;
    LOCK_GUARD
    if(first)
    {
        aux::engine->tuneUpdate();
        first = false;
    }
    logger::system("[JNI SYS] RESIZE %dx%d",w,h);
    aux::engine->resize(w,h);
}

DECL_BOOL_METHOD(onKeyPress) (JNIEnv *env, jobject obj, jint key, jstring data)
{
    LOCK_GUARD
    input->state.key = key;
    input->state.keypressed = true;
    input->state.keydata = jstr2str(data,env);
    return !(!aux::engine);
}

DECL_BOOL_METHOD(onTouch) (JNIEnv *env, jobject obj, jint x, jint y)
{
    LOCK_GUARD
    input->state.mx = x;
    input->state.my = y;
    input->state.pressed = true;
    return !(!aux::engine);
}

DECL_BOOL_METHOD(onTouchRelease) (JNIEnv *env, jobject obj, jint x, jint y)
{
    LOCK_GUARD
    input->state.mx = x;
    input->state.my = y;
    input->state.released = true;
    return !(!aux::engine);
}
DECL_BOOL_METHOD(onTouchMove) (JNIEnv *env, jobject obj, jint x, jint y)
{
    LOCK_GUARD
    input->state.mx = x;
    input->state.my = y;
    input->state.moved = true;
    return !(!aux::engine);
}
}

/*------------------------------------------------------------------------------------------*/

// void TheEngine::platformCallback(strref id,strref param) 
// {
//     JNIEnv *    env;
//     javaVM->AttachCurrentThread(&env, NULL);
 
//     logger::debug("[JNI SYS] callMethod('%s','%s')",id.c_str(), param.c_str());
//     jstring ij = env->NewStringUTF(id.c_str());
//     jstring pj = env->NewStringUTF(param.c_str());
//     jCall(env,"callMethod","(Ljava/lang/String;Ljava/lang/String;)V",ij,pj);    
// }


#endif