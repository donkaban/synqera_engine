#ifndef _THE_ICACHEABLE_INTERFACE
#define _THE_ICACHEABLE_INTERFACE

#include "TheEngine/comtypes.h"
#include "TheEngine/common/abstract.h"
#include "TheEngine/logger.h"

#include "TheEngine/aux.h"

template <typename T> 
class ICacheable : public std::enable_shared_from_this<T>
{
public:
    using ptr  = std::shared_ptr<T>;
    using cref = const ptr &;

    ICacheable(strref dname = "def::base") 
    {

        default_name = dname;
        static_assert(std::is_convertible<T*, abstract::object *>::value,"[ICacheable] not inherited from abstract::object!");
    }
    virtual ~ICacheable() 
    {}
    
    ptr getPtr()   {return this->shared_from_this();}
    static cref get(strref name)
    {
        if(!check(name)) FATAL_ERROR("[%-07s] can't find key %s in cache", type_name.c_str(), name.c_str());
        return cache[name];  
    }
    static bool check(strref name) {return cache.find(name) != cache.end();}
    static std::string dumpCache() 
    { 
        std::string res;
        for(auto &i : cache) res += aux::format("\n\t'%s'", i.first.c_str());
        return res;    
    }
    static void clearCache() 
    {
        for(auto &i : cache) removeFromCache(i.first);
        cache.clear();
    }
    static cref addToCache(cref t, strref name) 
    {
        if(check(name)) return cache[name];
        type_name = t->getType();
        cache[name] = t;
        logger::debug("[%-07s] cache  '%s'", type_name.c_str(),name.c_str()); 
        return t;
    }
    static cref addToCache(cref t) 
    {
        return addToCache(t, t->getTag());
    }
   
    static void removeFromCache(strref name)   
    {
        if(check(name)) 
        { 
            logger::debug("[%-07s] remove '%s' from cache", type_name.c_str(),name.c_str()); 
            cache.erase(name);
        }
    }
    static void foreach(const std::function<void(std::string,ptr)> &func)
    {
        for(auto &i : cache) func(i.first,i.second);
    }

private:
    static dictionary<std::shared_ptr<T> > cache;
    static std::string default_name;
    static std::string type_name;
    static std::vector<std::string> types;
};

template <typename T> dictionary<std::shared_ptr<T> > ICacheable<T>::cache;
template <typename T> std::string ICacheable<T>::default_name;
template <typename T> std::string ICacheable<T>::type_name;


#endif