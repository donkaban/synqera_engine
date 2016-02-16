#ifndef _THE_TASK_QUEUE_
#define _THE_TASK_QUEUE_

#include "TheEngine/comtypes.h"
#include "TheEngine/common/method.h"
#include "TheEngine/logger.h"

template <typename T> class taskQueue;
template <typename R, typename... A> class taskQueue<R(A...)> 
{
     
public:
    using type = std::function<R(A...)>; 
    using key  = size_t;

    taskQueue(strref tag = "noName") : _tag(tag) 
    {
        logger::debug("[Methods] create '%s'", _tag.c_str());
    }

    bool check(key k) {return _map.find(k) != _map.end();}
    size_t add(const type &f) 
    {
        ++uid; 
        logger::debug("[Methods] add to '%s' task #%d", _tag.c_str(), uid);
        _map[uid] = method<R(A...)>(f); 
        return uid;
    }
    taskQueue & emit(A ... args) 
    {
        for(auto &i: _map) 
            i.second.param(args...); 
        return *this;
    }
    taskQueue & update()
    {
        for(auto i = _map.begin(); i != _map.end();)
        {
            auto s = i->second.state();
            if(s == mstate::DELETED)
            {
                logger::debug("[Methods] remove from '%s' task #%d",_tag.c_str(),i->first);
                i = _map.erase(i);
            } 
            else if(s != mstate::PAUSED)
            {    
                i->second.call();
                i++;  
            }
            else
                i++;    
        }
        return *this;
    }  
   
    taskQueue & delAll() 
    {
        for(auto &i : _map) 
            i.second.state(mstate::DELETED); 
        return *this;
    }
   
    bool isPaused(key k)  const {return _map[k].state()==mstate::PAUSED;}
    bool isDeleted(key k) const {return _map[k].state()==mstate::DELETED;}

    void pause(key k)    {if(check(k)) _map[k].state(mstate::PAUSED);}
    void unpause(key k)  {if(check(k)) _map[k].state(mstate::DIRECT);}
    void pauseAll()      {for(auto &i : _map) i.second.state(mstate::PAUSED);}
    void unpauseAll()    {for(auto &i : _map) i.second.state(mstate::DIRECT);}
    void del(key k)     
    {
        if(check(k)) 
        {
            _map[k].state(mstate::DELETED);
            logger::debug("[Methods] mark #task%d deleted for '%s' ",k, _tag.c_str());
        }           
    }

    int size()     {return _map.size();}

private:    
    std::string _tag;
    std::unordered_map<size_t,method<R(A...)>> _map;
    key uid = 0;

};




#endif