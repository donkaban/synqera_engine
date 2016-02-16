#ifndef _THE_ACTION_
#define _THE_ACTION_

#include "TheEngine/common.h"

class Action 
{
public:
    typedef std::function<void(const vec3 &)> callback_t;
    typedef const callback_t & cbRef;

    enum class type 
    {
        PRESS   = 0,
        TAP     = 1,
        RELEASE = 2,
        HSWIPE  = 3,
        VSWIPE  = 4  
    };
    Action() {}
    Action(type actionType, int id, cbRef callback) : 
        surfaceId(id),
        actionType(actionType),
        callback(callback)
    {}
    
    ~Action() 
    {}

    void invoke(const vec3 &p) {if(callback) callback(p);}
    int  getSurface()    const {return surfaceId;}
    type getActionType() const {return actionType;}
    bool destroed()      const {return need_destroy;}
    void destroy()             {need_destroy = true;}
    

private:
    int         surfaceId;
    type        actionType;
    callback_t  callback = callback_t();
    bool        need_destroy = false;
};

#endif 
