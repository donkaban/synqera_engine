#ifndef _THE_TIMER_
#define _THE_TIMER_

#include "TheEngine/common.h"
namespace the {

class timed_task : 
    public abstract::object
{
public: 
 
    timed_task(float,const callable & ,bool);
    virtual ~timed_task();
    
    bool invoke(float);
    bool looped() const;
    void looped(bool) ;

private:
    float    _delay    = 0;
    float    _current  = 0;
    bool     _looped   = false;
    callable _task {};
};

class timer : 
    public abstract::object
{
  
public:
 
    timer();
    timer(strref);
    virtual ~timer();
    
    float getTime();
    void  reset();
    void  update();
    
    uint addCallback(float,const callable &, bool loop);
    void unloopCallback(uint);
    void removeCallback(uint);

    static std::string timestamp();
    static float globalTime();
    template <typename F, typename ... A>
    static float trace(F && f, A &&... args)
    {
        timepoint start = std::chrono::system_clock::now();
        std::bind(std::forward<F>(f), std::forward<A>(args)...)();
        std::chrono::duration<float> dt(std::chrono::system_clock::now() - start);
        return dt.count();
    } 
    static void update_all();
    static void lock();
    static void unlock();
    
    friend  std::ostream &operator<<(std::ostream &o, const timer &a) {o << a.tostring();return o;}
  
private: 
    static timepoint globalStart;
    static bool globalLock;
  
    timepoint start;
    timepoint last;
    timepoint current;
   
    objectMap<timed_task> tasks;
    float  _getDT();

    static std::vector<timer *> timers;
    
};
}

#endif 
