#ifndef _THE_PRECOMPILED_
#define _THE_PRECOMPILED_

#include "TheEngine/comtypes.h"
#include <string>
#include <vector>

namespace precompiled { 

namespace shader 
{
    struct shader
    {
        std::string name;
        std::string vertex;
        std::string fragment;
        shader(strref n,strref v, strref f) : 
            name(n),
            vertex(v),
            fragment(f)
        {}    

    };

    extern shader base;
    extern shader text;
    extern shader line;
    extern shader alphatest;
    extern shader mix;
    extern shader simple_text;
}

namespace font 
{
    struct  fontData
    {
        const size_t size;
        bytevec data;
        
        fontData(size_t s, const bytevec && d): 
            size(s),
            data(std::move(d))
        {}
    };  
    extern fontData fnt;
}

}
#endif