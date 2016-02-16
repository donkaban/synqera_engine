#ifndef _THE_CORE_LOGGER_
#define _THE_CORE_LOGGER_

#define SIMPLE_LOGGER

#include "TheEngine/comtypes.h"
#include "TheEngine/aux.h"

#include <cstdio>

#ifdef PLATFORM_ANDROID
    #include <android/log.h>
    #define TAG "[TheEngine]"
    #define ANDROID_DBG(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
    #define ANDROID_ERR(...)  __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
#endif

#define COL_BLACK   "\33[0;30m"
#define COL_RED     "\33[0;31m"
#define COL_GREEN   "\33[0;32m"
#define COL_YELLOW  "\33[0;33m"
#define COL_BLUE    "\33[0;34m"
#define COL_MAGENTA "\33[0;35m"
#define COL_CYAN    "\33[0;36m"
#define COL_WHITE   "\33[0;37m"
#define COL_RESET   "\33[0m"   

#define INDENT_SIZE  3

class logger
{
    THE_SHARED(logger)
public:
    enum class level {none=0,error,system,warning,info,debug,extra,full};
   
    logger(strref);
    ~logger();
    void  setLevel(level);
    void  setLevel(strref);
    level getLevel() const;

    template<typename ... T>
    void message(strref c, strref format, T && ... args)
    {
        const char *fmt = (c + format + "\33[0m\n").c_str(); 
        #ifdef PLATFORM_ANDROID
            ANDROID_DBG(fmt, args...);
        #else
            printf(fmt, args...);
        #endif
    }
      
    template<typename ... T> 
    static void extra(const char* fmt, T && ... args)   
    {
        if(aux::log && aux::log->getLevel() >= level::extra)
            aux::log->message(COL_BLUE,fmt,args...);
    }
    template<typename ... T> 
    static void debug(const char* fmt, T && ... args)   
    {
        if(aux::log && aux::log->getLevel() >= level::debug)
            aux::log->message(COL_CYAN,fmt,args...);
    }
    
    template<typename ... T> 
    static void info(const char* fmt, T && ... args)    
    {
        if(aux::log && aux::log->getLevel() >= level::info)
            aux::log->message(COL_GREEN,fmt,args...);
    }
    
    template<typename ... T> 
    static void warning(const char* fmt, T && ... args) 
    {
        if(aux::log && aux::log->getLevel() >= level::warning)
            aux::log->message(COL_YELLOW,fmt,args...);
    }
    
    template<typename ... T> 
    static void system(const char* fmt, T && ... args)   
    {
        if(aux::log && aux::log->getLevel() >= level::system)
            aux::log->message(COL_MAGENTA,fmt,args...);
    }
    template<typename ... T> 
    static void error(const char* fmt, T && ... args)   
    {
        if(aux::log && aux::log->getLevel() >= level::error)
            aux::log->message(COL_RED,fmt,args...);
    }

    template<typename ... T> 
    static void fatal(const char *file, int line,const char* fmt, T && ... args)   
    {
        aux::log->message(COL_RED,"[FATAL  ] %s:%d ", file, line); 
        aux::log->message(COL_RED,fmt,args...);
        exit(-1); 
    }

private:
    level  loglevel = level::full; 
};

#define FATAL_ERROR(...) logger::fatal(__FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERR(...)     logger::error(__VA_ARGS__)
#define LOG_INF(...)     logger::info(__VA_ARGS__)
#define LOG_WRN(...)     logger::warning(__VA_ARGS__)

#endif