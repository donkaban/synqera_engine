#ifndef _THE_ENGINE_AUX_
#define _THE_ENGINE_AUX_

#include "TheEngine/comtypes.h"
#include "TheEngine/common/serialize.h"

#include <mutex>

#if defined(PLATFORM_LINUX)
    #define THE_PLATFORM_NAME "Linux"
#elif defined(PLATFORM_ANDROID)
    #define THE_PLATFORM_NAME "Android"
#elif defined(PLATFORM_MACOS)
    #define THE_PLATFORM_NAME "MacOS X"
#elif defined(PLATFORM_ARM)
    #define THE_PLATFORM_NAME "ARM Linux"
#else
    #error PLATFORM_* NOT DEFINED!!!
#endif

#if defined(RENDER_OPENGLES2)
    #define THE_RENDER_NAME "OpenGL|ES 2"
#elif defined(RENDER_OPENGLES3)
    #define THE_RENDER_NAME "OpenGL|ES 3"
#elif defined(RENDER_OPENGL)
    #define THE_RENDER_NAME "OpenGL"
#else
    #error RENDER_* NOT DEFINED!!!
#endif

#if defined(SOUND_OPENSL_ES_ANDROID)
    #define THE_SOUND_NAME "OpenSL|ES Android subsystem"
#elif defined(SOUND_OPENAL)
    #define THE_SOUND_NAME "OpenAL subsystem"
#elif defined(SOUND_NULL)
    #define THE_SOUND_NAME "No sound subsystem"
#else
    #error SOUND_* NOT DEFINED!!!
#endif        

#ifdef PLATFORM_ANDROID	
	#include <android/asset_manager.h>
    namespace aux {extern AAssetManager *assetManager;}
#endif
namespace the {class engine;}
class logger;

namespace aux
{
    extern std::shared_ptr<the::engine> engine;
    extern std::shared_ptr<logger> log;  
    extern std::string dataPath;
    extern bool        runned;
    
	std::string trim(strref, strref);
    std::string format(const char *fmt, ...);
    std::string datapath(strref);

    size_t getUID();  


    template <typename T>
    T deserialize(const pugi::xml_node in)
    {
        T val;
        val.deserialize(in);
        return val;
    }

    class commandline
    {
    public:
        commandline(int c, char **v):
            _ac(c),
            _av(v)
        {}    
        template <typename T> T get(strref, const T &);
        bool check(strref name) {return (_check(name) != -1);}
    private:
        int     _ac;
        char ** _av; 

        int _check(strref name);
        std::string _get(int);
    };
}

#ifdef PLATFORM_ANDROID
  namespace std {strref to_string(...);}
  lconv* localeconv();
#endif



#endif 
