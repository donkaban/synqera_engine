#include <jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/input.h>

#define ENGINE_CLASS_NAME "com/synqera/the/engine"
#define DECL_VOID_METHOD(tag,...) JNIEXPORT void JNICALL Java_com_synqera_the_engine_##tag
#define DECL_BOOL_METHOD(tag,...) JNIEXPORT jint JNICALL Java_com_synqera_the_engine_##tag

template<typename ... T>
void jCall(JNIEnv *  env,const char *methodName, const char *methoodSignature,T && ... args)
{
    auto klass  = env->FindClass(ENGINE_CLASS_NAME);
    if(!klass)  FATAL_ERROR("[JNI SYS] can't find engine class");
    auto method = env->GetStaticMethodID(klass, methodName,methoodSignature); 
    if(!method) FATAL_ERROR("[JNI SYS] can't find method %s.%s(%s)", ENGINE_CLASS_NAME,methodName, methoodSignature);
    env->CallVoidMethod(klass,method,args...);
}  