#include "TheEngine/common.h"
#include "TheEngine/logger.h"
#include "TheEngine/timer.h"


#include <iostream>
#include <stdarg.h>
#include <stdexcept>
#include <fstream>


logger::logger(strref filename) 
{
#ifndef PLATFORM_ANDROID
    std::cout << "[Logger ] create\n";
#else 
    ANDROID_DBG("[Logger ] create\n");
#endif    
  
}
logger::~logger()
{
#ifndef PLATFORM_ANDROID
    std::cout << "[Logger ] close\n";
#else 
    ANDROID_DBG("[Logger ] close \n");
#endif    
}

void logger::setLevel(logger::level lvl) {loglevel = lvl;}
void logger::setLevel(strref lvl) 
{
    const dictionary<logger::level> lvlMap = 
    {
        {"none"    , logger::level::none    },
        {"error"   , logger::level::error   },
        {"system"  , logger::level::system  },
        {"warning" , logger::level::warning },
        {"info"    , logger::level::info    },
        {"debug"   , logger::level::debug   },
        {"extra"   , logger::level::extra   },
        {"full"    , logger::level::full    },
    };
    auto it = lvlMap.find(lvl);
    if(it != lvlMap.end())
    {    
        logger::info("[Logger ] set level to %s", it->first.c_str());
        loglevel = it->second;
    }
    else
    {    
        logger::info("[Logger ] set level to full");
        loglevel = logger::level::full;
    }    
}

logger::level logger::getLevel() const {return loglevel;}

