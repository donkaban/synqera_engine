#include "TheEngine/common.h"
#include "TheEngine/logger.h"

#include <stdarg.h>
#include <mutex>

#define BUFFER_SIZE 4096

const char * ENGINE_VERSION_TAG = "Winter Is Coming";
const char * ENGINE_BUILD_DATE  = __DATE__;
const int    ENGINE_VERSION_MAJOR = 3;
const int    ENGINE_VERSION_MINOR = 0;
const int    ENGINE_VERSION_WIP   = 0;

size_t UID = 1;

namespace aux {

std::shared_ptr<logger>       log;
std::shared_ptr<the::engine>  engine = nullptr;
std::string                   dataPath = "";
bool                          runned = false;

std::string datapath(strref path) 
{
    return dataPath + path;
}

std::string trim(strref in, strref path )
{
    std::string result(in);
    auto i = result.find(path);
    if (i != std::string::npos) result.erase(i, path.length());
    return result;
}

std::string format(const char *fmt, ...)
{
    char buff[BUFFER_SIZE];
    if(!fmt) return "";
    va_list arg;
    va_start(arg, fmt);
    vsnprintf(buff, BUFFER_SIZE, fmt, arg);
    va_end(arg);
    return std::string(buff);
}

size_t getUID() {return ++UID;}

int commandline::_check(strref name)
{
    for(int i = 1; i< _ac; i++)
            if(std::string(_av[i]) == name) return i;
        return -1;
}

std::string commandline::_get(int pos)
{
    if(pos >= _ac) FATAL_ERROR("[Command ] can't read commandline parameter number %d", pos+1);
    return std::string(_av[pos+1]);
}


template<> float commandline::get<float>(strref name,const float & def) 
{
    auto pos = _check(name); 
    return (pos == -1) ? def : ::atof(_get(pos).c_str());
}

template<> int commandline::get<int>(strref name,const int & def) 
{
    auto pos = _check(name); 
    return (pos == -1) ? def : ::atoi(_get(pos).c_str());
}

template<> std::string commandline::get<std::string>(strref name,strref def) 
{
    auto pos = _check(name); 
    return (pos == -1) ? def : _get(pos);
}
}

#ifdef PLATFORM_ANDROID

    AAssetManager *aux::assetManager;
    namespace std
    {
        const string __fake = "fake std::to_string() call";
        const string &to_string(...) {return __fake;} 
    }
    lconv* localeconv()
    {
        auto fake = new(lconv);
        std::memset(fake,0,sizeof(lconv));
        return fake;
    } 
#endif






